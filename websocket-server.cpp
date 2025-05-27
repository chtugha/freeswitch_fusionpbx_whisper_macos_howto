#include <uwebsockets/App.h>
#include <portaudio.h>

#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <string.h>

std::mutex audioMutex;
std::queue<std::vector<int16_t>> audioQueue;

// PortAudio callback to feed audio device
static int paCallback(const void* inputBuffer, void* outputBuffer,
                      unsigned long framesPerBuffer,
                      const PaStreamCallbackTimeInfo* timeInfo,
                      PaStreamCallbackFlags statusFlags, void* userData) {
    int16_t* out = (int16_t*)outputBuffer;
    size_t framesRequested = framesPerBuffer;

    std::lock_guard<std::mutex> lock(audioMutex);

    if (audioQueue.empty()) {
        memset(out, 0, framesRequested * sizeof(int16_t));
        return paContinue;
    }

    auto& frontBuffer = audioQueue.front();
    size_t framesAvailable = frontBuffer.size();
    size_t framesToCopy = std::min(framesRequested, framesAvailable);

    memcpy(out, frontBuffer.data(), framesToCopy * sizeof(int16_t));

    if (framesToCopy < framesAvailable) {
        frontBuffer.erase(frontBuffer.begin(), frontBuffer.begin() + framesToCopy);
    } else {
        audioQueue.pop();
    }

    if (framesToCopy < framesRequested) {
        memset(out + framesToCopy, 0, (framesRequested - framesToCopy) * sizeof(int16_t));
    }

    return paContinue;
}

int main() {
    // Initialize PortAudio
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio init error: " << Pa_GetErrorText(err) << std::endl;
        return 1;
    }

    // Find BlackHole device index
    int numDevices = Pa_GetDeviceCount();
    int blackHoleDeviceIndex = -1;
    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(i);
        std::string name(deviceInfo->name);
        if (name.find("BlackHole") != std::string::npos) {
            blackHoleDeviceIndex = i;
            std::cout << "Found BlackHole device: " << name << " at index " << i << std::endl;
            break;
        }
    }
    if (blackHoleDeviceIndex == -1) {
        std::cerr << "BlackHole audio device not found." << std::endl;
        Pa_Terminate();
        return 1;
    }

    // Open PortAudio stream to BlackHole
    PaStream* stream;
    PaStreamParameters outputParams;
    outputParams.device = blackHoleDeviceIndex;
    outputParams.channelCount = 1;        // mono audio
    outputParams.sampleFormat = paInt16;  // signed 16-bit
    outputParams.suggestedLatency = Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(&stream,
                        nullptr,  // no input
                        &outputParams,
                        16000,    // sample rate - match your PCM data
                        256,      // frames per buffer
                        paClipOff,
                        paCallback,
                        nullptr);
    if (err != paNoError) {
        std::cerr << "Failed to open PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "Failed to start PortAudio stream: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return 1;
    }

    std::cout << "Starting uWebSocket server on port 2700..." << std::endl;

    // Create uWebSockets App and listen on port 2700
    uWS::App().ws<struct PerSocketData>("/*", {
        // On connection open
        .open = [](auto* ws) {
            std::cout << "Client connected!" << std::endl;
        },
        // On message received (binary audio expected)
        .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
            if (opCode == uWS::OpCode::BINARY) {
                // Convert received bytes to int16_t PCM samples
                size_t samples = message.size() / 2;
                std::vector<int16_t> pcmBuffer(samples);

                memcpy(pcmBuffer.data(), message.data(), message.size());

                // Push buffer into queue safely
                {
                    std::lock_guard<std::mutex> lock(audioMutex);
                    audioQueue.push(std::move(pcmBuffer));
                }
            } else {
                std::cout << "Received non-binary message, ignoring." << std::endl;
            }
        },
        // On connection close
        .close = [](auto* ws, int /*code*/, std::string_view /*message*/) {
            std::cout << "Client disconnected." << std::endl;
        }
    }).listen(2700, [](auto* listenSocket) {
        if (listenSocket) {
            std::cout << "Listening on port 2700" << std::endl;
        } else {
            std::cerr << "Failed to listen on port 2700" << std::endl;
        }
    }).run();

    // Cleanup PortAudio on exit (never reached in this example)
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    return 0;
}

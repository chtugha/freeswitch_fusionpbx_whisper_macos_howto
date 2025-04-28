# freeswitch_fusionpbx_whisper_macos_howto
**How to install a local voip phonebox on macos whith realtime voice processing**
<BR>
<BR>
<BR>
As prerequisite follow this tutorial: https://github.com/chtugha/whisper.cpp_macos_howto
and install everything exactly as shown there.
<BR>
<BR>
<BR>
<BR>
1.) First we need to get the necessary libraries and programs:
<BR>
<BR>
_`brew install lua nginx node opus sox sqlite freeswitch postgresql`_
<BR>
<BR>
<BR>
2.) Prepare a database for FusionPBX and get the repository:
<BR>
<BR>
_`brew services start postgresql`_
<BR>
_`git clone https://github.com/fusionpbx/fusionpbx.git /opt/homebrew/var/www/fusionpbx`_

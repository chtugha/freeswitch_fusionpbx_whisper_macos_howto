# freeswitch_fusionpbx_whisper_macos_howto
**How to install a local voip phonebox on macos whith realtime voice processing**
<BR>
UNFINISHED VERSION!!!! NOT YET WORKING !!!
<BR>
<BR>
As prerequisite follow this tutorial: https://github.com/chtugha/whisper.cpp_macos_howto
and install everything exactly as shown there.
<BR>
<BR>
<BR>
<BR>
1.) First we need to get the necessary libraries and programs. Enter into terminal:
<BR>
<BR>
![Terminal](https://github.com/user-attachments/assets/a5ca18ff-867f-46ef-b008-f584b6303f77)
<BR>
<BR>
if more channels are needed exchange blackhole-16ch for blackhole-64ch 
<BR>
_`brew install git-lfs nginx sqlite freeswitch postgresql psqlodbc unixodbc portaudio pkg-config blackhole-16ch`_ 
<BR>
_`brew tap shivammathur/php`_
<BR>
_`brew install shivammathur/php/php@8.0`_
<BR>
_`brew link --overwrite --force php@8.0`_
<BR>
_`brew services start php@8.0`_
<BR>
<BR>
<BR>
2.) Prepare a database for FusionPBX:
<BR>
<BR>
_`brew services start postgresql`_
<BR>
_`createdb`_
<BR>
<BR>
Enter the postgres shell
<BR>
_`psql`_
<BR>
<BR>
Create a Database:
<BR>
_`create role freeswitch with password 'fusionpbx' login;`_
<BR>
_`create role fsadmin with password 'fsadmin' login superuser createdb;`_
<BR>
_`create role fusionpbx with password 'fusionpbx' login superuser createdb;`_
<BR>
_`create database freeswitch owner freeswitch;`_
<BR>
_`create database fusionpbx owner fusionpbx;`_
<BR>
_`grant all privileges on database fusionpbx to fusionpbx;`_
<BR>
<BR>
<BR>
Check if the database was created:
<BR>
_`\l`_
<BR>
<BR>
Exit postgres shell
<BR>
_`\q`_
<BR>
<BR>
<BR>
3.)Configure ODBC
<BR>
<BR>
_`nano /opt/homebrew/etc/odbc.ini`_
<BR>
<BR>
Paste following lines into odbc.ini:

```ini
[freeswitch]
Description         = PostgreSQL
Driver              = PostgreSQL Unicode
Trace               = No
TraceFile           = /tmp/psqlodbc.log
Database            = freeswitch
Servername          = 127.0.0.1
UserName            = freeswitch
Password            = fusionpbx
Port                = 5432
ReadOnly            = No
RowVersioning       = No
ShowSystemTables    = No
ShowOidColumn       = No
FakeOidIndex        = No
#ConnSettings        = set search_path to coredb,public
```
Save changes and quit nano by pressing: `CTRL+O` (confirm with `y`) and `CTRL+X`.
<BR>
<BR>
Now edit odbcinst.ini:
<BR>
_`nano /opt/homebrew/etc/odbcinst.ini`_
<BR>
<BR>
Paste following lines into odbcinst.ini:
<BR>
```ini
[PostgreSQL Unicode]
Description     = ODBC for PostgreSQL
Driver          = /opt/homebrew/lib/psqlodbcw.so  # Apple Silicon path
```
Save changes and quit nano by pressing: `CTRL+O` (confirm with `y`) and `CTRL+X`.
<BR>
<BR>
Test ODBC:
<BR>
_`isql freeswitch -v`_
<BR>
<BR>
And exit the SQL-Shell again:
<BR>
_`quit`_
<BR>
<BR>
<BR>
3.) Get FusionPBX and prepare it:
<BR>
<BR>
_`git clone https://github.com/fusionpbx/fusionpbx.git /opt/homebrew/var/www/fusionpbx`_
<BR>
_`sudo chown -R $(whoami):staff /opt/homebrew/var/www/fusionpbx`_
<BR>
_`mkdir /opt/homebrew/var/cache/fusionpbx`_
<BR>
_`sudo chown -R $(whoami):staff /opt/homebrew/var/cache/fusionpbx`_
<BR>
_`sudo chmod -R 755 /opt/homebrew/var/www/fusionpbx`_
<BR>
_`sudo chmod -R 755 /opt/homebrew/var/cache/fusionpbx`_
<BR>
<BR>
4.) Adjust the installation files:
<BR>
<BR>
_`sudo sed -i '' "85s/.*/\$language_code = \$_SESSION['domain']['language']['code'] ?? 'en-us';/" /opt/homebrew/var/www/fusionpbx/resources/classes/text.php`_
<BR>
<BR>
_`nano /opt/homebrew/var/www/fusionpbx/core/install/install.php`_
<BR>
<BR>
Go to line 74 and change the code from:
<BR>
```ini
//if the config file exists then disable the install page
	$config_exists = false;
	if (file_exists("/usr/local/etc/fusionpbx/config.conf")) {
		//bsd
		$config_exists = true;
```
to the following:
```ini
//if the config file exists then disable the install page
	$config_exists = false;
	if (file_exists("/opt/homebrew/var/www/fusionpbx/resources/config.conf")) {
		//Apple Silicon
		$config_exists = true;
```
Now go to line 97 and change the code from:
```ini
//if the config.php exists create the config.conf file
	if (!$config_exists) {
		if (file_exists("/usr/local/etc/fusionpbx/config.php")) {
			//bsd
			$config_path = "/usr/local/etc/fusionpbx";
		}
```
To the following:
```ini
//if the config.php exists create the config.conf file
	if (!$config_exists) {
		if (file_exists("/opt/homebrew/var/www/fusionpbx/resources/config.php")) {
			//Apple Silicon
			$config_path = "/opt/homebrew/var/www/fusionpbx/resources";
		}
```
Now go to line 180 and change the code from:
```ini
//add the database schema
			$output = shell_exec('cd '.$_SERVER["DOCUMENT_ROOT"].' && php /var/www/fusionpbx/core/upgrade/upgrade_schema.php');
```
To the following:
```ini
//add the database schema
			$output = shell_exec('cd '.$_SERVER["DOCUMENT_ROOT"].' && php core/upgrade/upgrade_schema.php');
```
Now go to line 234 and change the code from:
```ini
//app defaults
			$output = shell_exec('cd '.$_SERVER["DOCUMENT_ROOT"].' && php /var/www/fusionpbx/core/upgrade/upgrade_domains.php');
```
To the following:
```ini
//app defaults
			$output = shell_exec('cd '.$_SERVER["DOCUMENT_ROOT"].' && php core/upgrade/upgrade_domains.php');
```
Now go to line 332 and change the code from:
```ini
			#app defaults
			$output = shell_exec('cd '.$_SERVER["DOCUMENT_ROOT"].' && php /var/www/fusionpbx/core/upgrade/upgrade_domains.php');
```
To the following:
```ini
			#app defaults
			$output = shell_exec('cd '.$_SERVER["DOCUMENT_ROOT"].' && php core/upgrade/upgrade_domains.php');
```
5.) Adjust the installation class:
<BR>
<BR>
_`nano /opt/homebrew/var/www/fusionpbx/core/install/resources/classes/install.php`_
<BR>
<BR>
Add the following case to the switch:
<BR>
<BR>
```ini
case "DAR":
                                $config_path = '/opt/homebrew/var/www/fusionpbx/resources';
                                $config_file = $config_path.'/config.conf';
                                $document_root = '/opt/homebrew/var/www/fusionpbx';
                        
                                $conf_dir = '/opt/homebrew/etc/freeswitch';
                                $sounds_dir = '/opt/homebrew/share/freeswitch/sounds';
                                $database_dir = '/opt/homebrew/Cellar/freeswitch/1.10.12/var/lib/freeswitch/db';
                                $recordings_dir = '/opt/homebrew/Cellar/freeswitch/1.10.12/var/lib/freeswitch/recordings';
                                $storage_dir = '/opt/homebrew/Cellar/freeswitch/1.10.12/var/lib/freeswitch/storage';
                                $voicemail_dir = '/opt/homebrew/Cellar/freeswitch/1.10.12/var/lib/freeswitch/storage/voicemail';
                                $scripts_dir = '/opt/homebrew/share/freeswitch/scripts';
                                $php_dir = PHP_BINDIR;
                                $cache_location = '/opt/homebrew/var/cache/fusionpbx';
                                break;
```
Edit functions
<BR>
_`nano /opt/homebrew/var/www/fusionpbx/resources/funtions.php`_
<BR>
<BR>
Add the following at line 211:
<BR>
<BR>
```ini
if (PHP_OS === 'Darwin') {
				$uuid = trim(shell_exec("uuidgen"));
				if (is_uuid($uuid)) {
					return $uuid;
				} else {
					echo "Please install uuidgen.\n";
					exit;
				}
			}
```
Edit messages
<BR>
_`nano /opt/homebrew/var/www/fusionpbx/resources/classes/messages.php`_
<BR>
<BR>
Replace the following at line 31:
<BR>
<BR>
```ini
//set mood
				$mood = $mood ?: 'positive';
        // set delay
				// $delay = $delay ?: (1000 * (float) $_SESSION['theme']['message_delay']['text']);
        if (isset($_SESSION['theme']['message_delay']['text']) && is_numeric($_SESSION['theme']['message_delay']['text'])) {
          $delay = $delay ?: (1000 * (float) $_SESSION['theme']['message_delay']['text']);
        } else {
          $delay = $delay ?: 1000; // Default to 1000ms if message_delay is not set or invalid
        }
```
Edit config
<BR>
_`nano /opt/homebrew/var/www/fusionpbx/resources/classes/config.php`_
<BR>
<BR>
Go to line 196 and change the code from:
```ini
			// Load the document root
			$doc_root = $this->get('document.root', '/var/www/fusionpbx');
```
To the following:
```ini
			// Load the document root
			$doc_root = $this->get('document.root', '/opt/homebrew/var/www/fusionpbx');
```
Add the following at line 232:
<BR>
<BR>
```ini
elseif (file_exists("/opt/homebrew/var/www/fusionpbx/resources/config.conf")) {
			$file = "/opt/homebrew/var/www/fusionpbx/resources/config.conf";
		}
```
Edit switch
<BR>
_`nano /opt/homebrew/var/www/fusionpbx/resources/switch.php`_
<BR>
<BR>
Go to line 855 and change the code from:
```ini
					//write the XML config file
			$switch_configuration_dir = !empty($_SESSION['switch']['conf']['dir']) ? $_SESSION['switch']['conf']['dir'] : '/etc/freeswitch';
			$fout = fopen($switch_configuration_dir . "/autoload_configs/xml_cdr.conf.xml","w");
			fwrite($fout, $file_contents);
			fclose($fout);
```
To the following:
```ini
					//write the XML config file
			$switch_configuration_dir = !empty($_SESSION['switch']['conf']['dir']) ? $_SESSION['switch']['conf']['dir'] : '/opt/homebrew/etc/freeswitch';
			$fout = fopen($switch_configuration_dir . "/autoload_configs/xml_cdr.conf.xml","w");
			fwrite($fout, $file_contents);
			fclose($fout);
```
Edit dashboard
<BR>
_`nano /opt/homebrew/var/www/fusionpbx/core/dashboard/index.php`_
<BR>
<BR>
Go to line 34 and add the following:
```ini
	elseif (file_exists("/opt/homebrew/var/www/fusionpbx/resources/config.conf")){
		//Apple Silicon
	}
```
7.) get nginx going
<BR>
<BR>
_`mv /opt/homebrew/etc/nginx/nginx.conf /opt/homebrew/etc/nginx/nginx.conf.old`_
<BR>
_`nano /opt/homebrew/etc/nginx/nginx.conf`_
<BR>
<BR>
And paste the following into nginx.conf:
<BR>
<BR>
```ini
worker_processes  1;

events {
    worker_connections  1024;
}

http {
    include       mime.types;
    default_type  application/octet-stream;

    sendfile        on;
    keepalive_timeout  65;

    server {
        listen       8080;
        server_name  localhost;

        root   /opt/homebrew/var/www/fusionpbx;
        index  index.php index.html index.htm;

        location / {
            try_files $uri $uri/ /index.php?$query_string;
        }

        location ~ \.php$ {
            include fastcgi_params;
            fastcgi_pass 127.0.0.1:9000;  # PHP-FPM auf Port 9000
            fastcgi_index index.php;
            fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
        }

        location ~ /\.ht {
            deny all;
        }

        error_page   500 502 503 504  /50x.html;
        location = /50x.html {
            root   html;
        }
    }

    include servers/*;
}
```
Save changes and quit nano by pressing: `CTRL+O` (confirm with `y`) and `CTRL+X`.
<BR>
<BR>
8.) Prepare fusionpbx
<BR>
<BR>
create config.php
<BR>
<BR>
_`nano /opt/homebrew/var/www/fusionpbx/resources/config.php`_
<BR>
<BR>
And paste following code into it
<BR>
<BR>
```ini
<?php
$db_type = 'pgsql';
$db_host = 'localhost';
$db_port = '5432';
$db_name = 'fusionpbx';
$db_username = 'fusionpbx';
$db_password = 'fusionpbx';
?>
```
Save changes and quit nano by pressing: `CTRL+O` (confirm with `y`) and `CTRL+X`.
<BR>
<BR>
create config.conf
<BR>
<BR>
_`nano /opt/homebrew/var/www/fusionpbx/resources/config.conf`_
<BR>
<BR>
And paste following lines into it. Check if the freeswitch version (here 1.10.12) is right. Adjust if needed.
<BR>
<BR>
```ini

#database system settings
database.0.type = pgsql
database.0.host = localhost
database.0.port = 5432
database.0.sslmode = prefer
database.0.name = fusionpbx
database.0.username = fusionpbx
database.0.password = fusionpbx

#database switch settings
database.1.type = sqlite
database.1.path = /opt/homebrew/Cellar/freeswitch/1.10.12/var/lib/freeswitch/db
database.1.name = core.db

#general settings
document.root = /opt/homebrew/var/www/fusionpbx
project.path =
temp.dir = /tmp
php.dir = /opt/homebrew/Cellar/php@8.0/8.0.30_6/bin
php.bin = php

#cache settings
cache.method = file
cache.location = /opt/homebrew/var/cache/fusionpbx
cache.settings = true

#switch settings
switch.conf.dir = /opt/homebrew/etc/freeswitch
switch.sounds.dir = /opt/homebrew/share/freeswitch/sounds
switch.database.dir = /opt/homebrew/Cellar/freeswitch/1.10.12/var/lib/freeswitch/db
switch.recordings.dir = /opt/homebrew/Cellar/freeswitch/1.10.12/var/lib/freeswitch/recordings
switch.storage.dir = /opt/homebrew/Cellar/freeswitch/1.10.12/var/lib/freeswitch/storage
switch.voicemail.dir = /opt/homebrew/Cellar/freeswitch/1.10.12/var/lib/freeswitch/storage/voicemail
switch.scripts.dir = /opt/homebrew/share/freeswitch/scripts

#switch xml handler
xml_handler.fs_path = false
xml_handler.reg_as_number_alias = false
xml_handler.number_as_presence_id = true

#error reporting options: user,dev,all
error.reporting = user

```
Save changes and quit nano by pressing: `CTRL+O` (confirm with `y`) and `CTRL+X`.
<BR>
<BR>
Now populate the database
<BR>
<BR>
_`php /opt/homebrew/var/www/fusionpbx/core/upgrade/upgrade_schema.php`_
<BR>
<BR>
And add the admin user (password admin) into the database
<BR>
<BR>
_`nano /opt/homebrew/var/www/fusionpbx/resources/populate.php`_
<BR>
Paste the following code into the file
<BR>
<BR>
```ini
<?php
// Variables
$domain_name = 'localhost';
$system_username = 'admin';
$system_password = 'admin';
$user_password = $system_password;
$database_host = '127.0.0.1';
$database_port = '5432';
$database_username = 'fusionpbx';
$database_name = 'fusionpbx';
$database_password = 'fusionpbx';

// DB-connection
$dsn = "pgsql:host=$database_host;port=$database_port;dbname=$database_name;";
$db = new PDO($dsn, $database_username, $database_password);
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

// Generate Domain-UUID
$domain_uuid = trim(shell_exec('php /opt/homebrew/var/www/fusionpbx/resources/uuid.php'));

// Insert domain
$sql = "INSERT INTO v_domains (domain_uuid, domain_name, domain_enabled)
        VALUES (:uuid, :name, 'true')";
$stmt = $db->prepare($sql);
$stmt->execute([
  ':uuid' => $domain_uuid,
  ':name' => $domain_name
]);

// App-Defaults
shell_exec('php /opt/homebrew/var/www/fusionpbx/core/upgrade/upgrade_domains.php');

// Benutzer anlegen
$user_uuid = trim(shell_exec('php /opt/homebrew/var/www/fusionpbx/resources/uuid.php'));
$user_salt = trim(shell_exec('php /opt/homebrew/var/www/fusionpbx/resources/uuid.php'));
$password_hash = md5($user_salt . $user_password);

// Insert user into DB
$sql = "INSERT INTO v_users (user_uuid, domain_uuid, username, password, salt, user_enabled)
        VALUES (:user_uuid, :domain_uuid, :username, :password, :salt, 'true')";
$stmt = $db->prepare($sql);
$stmt->execute([
  ':user_uuid' => $user_uuid,
  ':domain_uuid' => $domain_uuid,
  ':username' => $system_username,
  ':password' => $password_hash,
  ':salt' => $user_salt
]);

// Get superadmin-group
$sql = "SELECT group_uuid FROM v_groups WHERE group_name = 'superadmin'";
$group_uuid = $db->query($sql)->fetchColumn();

// Add user to group
$user_group_uuid = trim(shell_exec('php /opt/homebrew/var/www/fusionpbx/resources/uuid.php'));
$sql = "INSERT INTO v_user_groups (user_group_uuid, domain_uuid, group_name, group_uuid, user_uuid)
        VALUES (:uuid, :domain_uuid, 'superadmin', :group_uuid, :user_uuid)";
$stmt = $db->prepare($sql);
$stmt->execute([
  ':uuid' => $user_group_uuid,
  ':domain_uuid' => $domain_uuid,
  ':group_uuid' => $group_uuid,
  ':user_uuid' => $user_uuid
]);

// Generate XML-CDR user and password
function generateToken($length = 20) {
  return preg_replace('/[^a-zA-Z0-9]/', '', base64_encode(random_bytes($length)));
}
$xml_cdr_username = generateToken();
$xml_cdr_password = generateToken();

// xml_cdr.conf.xml aktualisieren
$xml_file = '/opt/homebrew/etc/freeswitch/autoload_configs/xml_cdr.conf.xml';
$xml_content = file_get_contents($xml_file);
$xml_content = str_replace('{v_http_protocol}', 'http', $xml_content);
$xml_content = str_replace('{domain_name}', $database_host, $xml_content);
$xml_content = str_replace('{v_project_path}', '', $xml_content);
$xml_content = str_replace('{v_user}', $xml_cdr_username, $xml_content);
$xml_content = str_replace('{v_pass}', $xml_cdr_password, $xml_content);
file_put_contents($xml_file, $xml_content);

// Showoff
echo "Domain added: $domain_name ($domain_uuid)\n";
echo "User: $system_username ($user_uuid), Password: $user_password\n";
echo "xml_cdr User: $xml_cdr_username, Password: $xml_cdr_password\n";
?>
```
Create language folder
<BR>
_`mkdir /opt/homebrew/etc/freeswitch/languages`_
<BR>
And populate the database
<BR>
_`php /opt/homebrew/var/www/fusionpbx/resources/populate.php`_
<BR>
<BR>
change the paths in switch.php on line 119
<BR>
<BR>
_`nano /opt/homebrew/var/www/fusionpbx/app/switch/resources/conf/autoload_configs/switch.php`_
<BR>
<BR>
_`<param name="mailer-app" value="/usr/bin/php /var/www/fusionpbx/secure/v_mailto.php"/>`_
To the following:
<BR>
<BR>
_`<param name="mailer-app" value="/opt/homebrew/bin/php /opt/homebrew/var/www/fusionpbx/secure/v_mailto.php"/>`_
<BR>
<BR>
<BR>
9.) Start nginx and freeswitch:
<BR>
<BR>
_`brew services start nginx freeswitch`_
<BR>
<BR>
<BR>
10.) Log into your PBX via http://127.0.0.1:8080

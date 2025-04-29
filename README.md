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
1.) First we need to get the necessary libraries and programs. Enter into terminal:
<BR>
<BR>
![Terminal](https://github.com/user-attachments/assets/a5ca18ff-867f-46ef-b008-f584b6303f77)
<BR>
<BR>
_`brew install lua nginx node opus sox sqlite freeswitch postgresql psqlodbc unixodbc`_
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
_`create role freeswitch with password 'freeswitch!' login;`_
<BR>
_`create role fsadmin with password 'fsadmin' login superuser createdb;`_
<BR>
_`create role fusionpbx with password 'fusionpbx' login superuser createdb;`_
<BR>
_`create database freeswitch owner freeswitch;`_
<BR>
_`create database fusionpbx owner fusionpbx;`_
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
Password            = freeswitch!
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
_`sudo chown -R $(whoami):staff /opt/homebrew/etc/fusionpbx`_
<BR>
_`sudo chmod -R 755 /opt/homebrew/var/www/fusionpbx`_
<BR>
_`sudo chmod -R 755 /opt/homebrew/etc/fusionpbx`_
<BR>
_`sudo sed -i '' "85s/.*/\$language_code = \$_SESSION['domain']['language']['code'] ?? 'en-us';/" /opt/homebrew/var/www/fusionpbx/resources/classes/text.php`_
<BR>
<BR>
4.) Adjust the installation file:
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
	if (file_exists("/opt/homebrew/etc/fusionpbx/config.conf")) {
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
		if (file_exists("/opt/homebrew/etc/fusionpbx/config.php")) {
			//Apple Silicon
			$config_path = "/opt/homebrew/etc/fusionpbx";
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
                                $config_path = '/opt/homebrew/etc/fusionpbx';
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
6.) get nginx going
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
Start nginx:
<BR>
<BR>
_`brew services start nginx`_
<BR>
<BR>
<BR>
7.) Start freeswitch:

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
<BR>
<BR>
2.) Prepare a database for FusionPBX:
<BR>
<BR>
_`brew services start postrgesql`_
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
_`create database freeswitch owner freeswitch;`_
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
And Configure ODBC
<BR>
_`nano /opt/homebrew/etc/odbc.ini`_
<BR>
<BR>
Paste following lines into odbc.ini:
<BR>
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
<BR>
Save changes and quit nano by pressing: CTRL+O (confirm with y) and CTRL+X.
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
<BR>
Save changes and quit nano by pressing: CTRL+O (confirm with y) and CTRL+X.
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
<BR>
<BR>

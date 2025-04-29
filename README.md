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
_`brew install lua nginx node opus sox sqlite freeswitch postgresql`_
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
_`create role fsadmin with password 'fsadmin' login superuser createdb;`_
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
<BR>
3.) Get FusionPBX and prepare it:
<BR>
<BR>
_`git clone https://github.com/fusionpbx/fusionpbx.git /opt/homebrew/var/www/fusionpbx`_
<BR>
<BR>
<BR>

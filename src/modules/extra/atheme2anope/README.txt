Convert Atheme databases to Anope
_________________________________

This is an Anope module used to convert Atheme databases. Note at this time we
only support Athemes poxis or rawmd5 encryptions (or no encryption). If you
are using another encryption this will not work.

First, get a clean Anope installation, if you were using encryption on Atheme
this must be using no encryption (enc_none). Copy these modules to src/modules
and compile and install them. Place your atheme.db in your services data
directory (most likely ~/services). Start Anope and load the atheme2anope
module, this will convert the atheme.db database and then shut down services.

If you didn't use encryption with Atheme then you are done, you can start up
services and things should have been imported from the atheme.db

If you are using encryption you need to add the atheme2anope_identify module
to your ModuleAutoload directive. This module overrides Anopes identify
commands and when given a password, checks it against Athemes known encryption
methods. If the password is correct it will reencrypt the password using
Anopes own encryption. Once everyones password has been converted (The time it
takes for a nick to expire, by then everyone would have identified or expired
(of course, noexpire nicks are an exception)) then this module can be
unloaded.

It is important you use no encryption until at least atheme2anope_identify
is done converting passwords. Anope needs Athemes encryption to match the
passwords with, not an encrypted version of Athemes encrypted password.


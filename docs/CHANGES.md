# Anope Change Log

## Anope 2.1.21 (unreleased)

### Breaking Changes

* `{fantasy}:fantasycharacter` has been replaced with `{fantasy}:prefix` which allows multiple-character fantasy prefixes. If you have multiple custom fantasy characters set you should separate them with a space when upgrading your config.

### Changes

* Fixed a crash when clearing channel entry messages.

* Fixed a memory leak when cloning akicks.

* Fixed confirming accounts using the webcpanel.

* Fixed limiting the number of accounts per email address.

* Fixed the `chanserv/enforce` command erroneously enforcing against channel founders.

* Fixed the syntax of the `chanserv/suspend` command.

* Fixed the syntax of the `nickserv/suspend` command.

* Improved password rehash detection in the enc_argon2 module.

* Various minor improvements to how services work internally.

## Anope 2.1.20 (2025-12-01)

### Breaking Changes

* Changed the registration of database types added by modules to be delayed until after the module constructor has been called. This might affect any custom modules you are using.

* Moved akicks out of the core into cs_akick. Modules which depend on akicks now require the cs_akick module to be loaded.

### Changes

* Added `{db_json}:preserve_unknown_data` to configure whether unknown database types are kept in the JSON database. By default unknown database types from unloaded modules will be preserved in the database to allow reloading later. This setting can be used to disable this and prune the database.

* Added support for forbidding passwords. This is intended to be used with file forbids (see below).

  ```
  /OPERSERV FORBID ADD PASSWORD +30d hunter2 This password is insecure
  -OperServ- Added a forbid on hunter2 of type password to expire on Mon 29 Dec 2025 11:51:13 AM UTC (30 days from now).
  ```

* Added support for loading forbids from a file.

  ```cpp
  file
  {
    type = "email"
    file = "temp-emails.txt"
    reason = "Temporary email"
  }
  ```

* Added support for the UnrealIRCd `+F` flood profile mode.

* Added the `anope-mkpasswd` script to help generate passwords for use in the config.

  ```
  $ ./anope-mkpasswd argon2id hunter2
  For use in the database:
      argon2id:$argon2id$v=19$m=65536,t=3,p=4$AmGWdtn1OUT9WSKSqESsPw$iguvHs6oIi/hF7e3t/bGNwgqP41vl/J4qP3a/yH9SLo

  For use in an oper:
      password = "$argon2id$v=19$m=65536,t=3,p=4$AmGWdtn1OUT9WSKSqESsPw$iguvHs6oIi/hF7e3t/bGNwgqP41vl/J4qP3a/yH9SLo"
      password_hash = "argon2id"

  For use in an jsonrpc/xmlrpc token:
      token = "$argon2id$v=19$m=65536,t=3,p=4$AmGWdtn1OUT9WSKSqESsPw$iguvHs6oIi/hF7e3t/bGNwgqP41vl/J4qP3a/yH9SLo"
      token_hash = "argon2id"

  Make sure you have the enc_argon2 module loaded!
  ```

* Added the DISPLAY flag to `nickserv/list` to only show account display nicknames.

  ```
  /NICKSERV LIST *
  -NickServ- List of entries matching *:
  -NickServ- nick1 (last mask: foo@example.com)
  -NickServ- nick1|afk (last mask: bar@example.com)
  -NickServ- nick2 (last mask: baz@example.com)
  -NickServ- End of list - 3/3 matches shown.

  /NICKSERV LIST * DISPLAY
  -NickServ- List of entries matching *:
  -NickServ- nick1 (last mask: foo@example.com)
  -NickServ- nick2 (last mask: baz@example.com)
  -NickServ- End of list - 2/2 matches shown.
  ```

* Added the hs_offer module which allows offering templated vhosts to users (based on a
modsite module by @genius3000 on GitHub).

  ```
  /HOSTSERV OFFER ADD {account}.users.example.com

  /HOSTSERV OFFERLIST
  -HostServ- Current host offer list:
  -HostServ- 2: {account}.users.example.com / FooBar.users.example.com -- does not expire
  -HostServ- End of host offer list.
  ```

* Changed chanserv/mode lock messages to stack the responses into one message per type instead of sending one message per mode.

  ```
  /CHANSERV MODE #stest LOCK ADD +bb foo!foo@foo bar!bar@bar
  -ChanServ- +bb foo!foo@foo bar!bar@bar has been locked on #stest.
  ```

* Changed database objects to rehook to their type when it becomes available again.

* Changed the `nickserv/set/language` and `nickserv/set/timezone` commands to allow setting back to the default value by omitting the last parameter.

  ```
  /NICKSERV SET LANGUAGE
  12:23 -NickServ- Language changed to English.

  /NICKSERV SET TIMEZONE
  12:24 -NickServ- Timezone changed to UTC.
  ```

* Changed the default install directory from `~/anope` to `~/anope-2.1`.

* Changed the enc_sha1 module to use a vendored SHA-1 implementation.

* Expanded password obscurity checks and added an event hook to allow modules to reject passwords.

* Fixed the rpc_user module sending the "invalid account" and "invalid password" error codes inverted.

* Fixed unintentionally reloading the core database when reloading a module that provides a database type.

* Removed a bunch of obsolete build system cruft.

## Anope 2.1.19 (2025-11-01)

### Breaking Changes

* `pkg-config` is now required to find dependencies for the following modules on UNIX systems:
  - enc_argon2
  - ldap
  - mysql
  - regex_pcre2
  - regex_tre

* Support for InspIRCd v3 has been dropped ahead of it going EOL in two months. Please migrate to InspIRCd v4 to keep using Anope 2.1 with InspIRCd.

### Changes

* Added a Romanian translation (contributed by @KidProtect on GitHub).

* Added support for associating a timezone with an account to allow users to receive timestamps in their local timezone.

  ```
  /NICKSERV SET TIMEZONE Europe/London
  -NickServ- Timezone changed to Europe/Berlin.

  /NICKSERV INFO test
  -NickServ- Account registered: Thu 09 Oct 2025 15:22:45 CEST (45 seconds ago)
  ```

  NOTE: This requires a compiler with C++20 timezone support.

* Added support for IRCv3 message tags when using Solanum git.

* Added support for language-specific time formats.

  ```
  /NICKSERV SET LANGUAGE tr_TR.UTF-8
  -NickServ- Dil Türkçe olarak değiştirildi.
  
  /NICKSERV INFO test
  -NickServ- Hesap kaydedildi: Prş 09 Eki 2025 15:22:45 (6 dakika, 16 saniye önce)
  ```

* Channel entry messages are now tagged with an IRCv3 time tag for the time they were created on supporting IRCds. This defaults to on but can be disabled using `{cs_entrymsg}:timestamp`.

* Reordered the information in the `nickserv/info` command output to show the registration dates before the seen information.

* Updated the Turkish translation (contributed by @CaPaCuL on GitHub).

* Updated the vendored libraries.

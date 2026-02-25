# Anope Change Log

## Anope 2.1.22 (unreleased)

### Breaking Changes

* Automatic login using a known SSL fingerprint now requires the `AUTOLOGIN` option to be set on accounts. Users can enable it on existing accounts with `/NS SET AUTOLOGIN ON`.

* Conan 2 is now used for packaging dependencies on Windows. If you are building from source you will need to upgrade Conan.

* Non-breaking spaces in translatable messages now use 0x1B instead of 0x1A due to recent msgfmt releases treating 0x1A as an EOF character. If you have an out of tree translation you will need to update it.

* User TLS certificates are now stored in their own `NSCert` table instead of as a column in the `NickCore` table. If you are reading this information you will need to update your code.

## Changes

* Fixed `{botserv}:botmodes` erroneously allowing setting non-status modes on channels.

* Fixed the consistency of indenting and line wrapping command examples in help output.

* Fixed handling incoming `FIDENT` messages on InspIRCd.

* Fixed matching stacked extended bans on InspIRCd and UnrealIRCd.

* Fixed parameter modes in `chanserv/mode` locks erroneously requiring a parameter to unset a lock.

* Fixed restoring the object identifier when unserialising objects in db_json.

* Redesigned the output of `nickserv/list` to show more relevant information.

  ```
  /NICKSERV LIST *
  -NickServ- List of entries matching *:
  -NickServ- alice (account: alice)
  -NickServ- alice|work (account: alice)
  -NickServ- bob -- Unconfirmed (account: bob)
  -NickServ- mallory -- Suspended (account: mallory)
  -NickServ- End of list - 4/4 matches shown.
  ```

* The cs_set_misc and ns_set_misc modules now can use a separate title from the command name.

  ```
  /NICKSERV SET MASTODON @example@mastodon.social
  -NickServ- Mastodon for testuser set to @example@mastodon.social
  ```

* The cs_set_misc and ns_set_misc modules now support validation of user-specified data.

  ```
  /NICKSERV SET MASTODON example.mastodon.social
  -NickServ- Mastodon syntax is invalid.
  -NickServ- Syntax: SET MASTODON [@user@host.tld]
  ```

* The db_atheme module can now import arbitrary metadata to fields from the ns_set_misc module.

* The local clock will now be checked for synchronisation with the IRCd clock on UnrealIRCd.

* The `nickserv/cert` command will now show the time a TLS certificate was created and the nickname of the creator if the `VIEW` subcommand is used.

  ```
  /NICKSERV CERT VIEW
  -NickServ- d41d8cd98f00b204e9800998ecf8427e -- created by nick1 at Wed 25 Feb 00:18:50 GMT
  ```

* The ns_set_misc module can now add account data to the WHOIS output of authenticated users on InspIRCd (with the swhois_ext module) and UnrealIRCd.

  ```
  /WHOIS nick1
  * [nick1] (nick1@example.com): nick1
  * [stest] Mastodon: @example@mastodon.social
  * [stest] irc.example.com :Example-IRC server
  ...
  * [stest] End of WHOIS list.
  ```

* The regex_posix module is now available on Windows (using the PCRE2 POSIX compatibility layer).

* The regex_tre module is now available on Windows.

* The Windows dependencies have been updated.

## Anope 2.1.21 (2026-02-07)

### Breaking Changes

* `{fantasy}:fantasycharacter` has been replaced with `{fantasy}:prefix` which allows multiple-character fantasy prefixes. If you have multiple custom fantasy characters set you should separate them with a space when upgrading your config.

* The db_json module will now terminate the process if it fails to write the database. This replicates the behaviour previously used by the db_flatfile module.

* When adding an unregistered user to an access list you must now explicitly specify their hostmask. This prevents accidentally adding a hostmask which is too wide.

### Changes

* Added cleaning up of hostmasks when adding them to an access list and `{chanserv}:disallow_malformed_hostmask` to allow rejecting them instead.

* Changed access commands to add the account of a user who is logged in to an account but not using a nickname belonging to that account.

* Fixed a crash when clearing channel entry messages.

* Fixed a memory leak when cloning akicks.

* Fixed cleaning up ban masks.

* Fixed confirming accounts using the webcpanel.

* Fixed importing the time a nickname was used from Atheme.

* Fixed limiting the number of accounts per email address.

* Fixed locking modes that take a parameter when they are added.

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

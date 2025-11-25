# Anope Change Log

## Anope 2.1.20 (unreleased)

### Breaking Changes

* Changed the registration database types added by modules to be delayed until after the module constructor has been called.

* Moved akicks out of the core into cs_akick. Modules which depend on akicks now require the cs_akick module to be loaded.

### Changes

* Added `{db_json}:preserve_unknown_data` to configure whether unknown database types are kept in the JSON database. By default unknown database types from unloaded modules will be preserved in the database to allow reloading later. This setting can be used to disable this and prune the database.

* Added support for forbidding passwords. This is intended to be used with file forbids (see below).

* Added support for loading forbids from a file.

* Added support for the UnrealIRCd `+F` flood profile mode.

* Added the DISPLAY flag to `nickserv/list` to only show account display nicknames.

* Added the hs_offer module which allows offering templated vhosts to users (based on a modsite module by @genius3000 on GitHub).

* Changed chanserv/mode lock messages to stack the responses into one message per type instead of sending one message per mode.

* Changed database objects to rehook to their type when it becomes available again.

* Changed the `nickserv/set/language` and `nickserv/set/timezone` commands to allow setting back to the default value by omitting the last parameter.

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

# Anope Change Log

## Anope 2.1.26 (2026-08-15)

### Breaking Changes

* Support for multi-part (hybrid HTML/text) and localised emails has been added. If you are using a custom email message you will need to move them to an external file and update your email block with the new sub-blocks for each email type. Otherwise, you can just replace the config with that from the example config.

* The hostserv/setall command has been removed. You should instead specify the SYNC flag to the hostserv/set command to synchronise vhosts across an entire account.

## General Changes

* Added support for using UNIX-style line endings with sendmail-incompatible mailers like nullmailer.

* Added the `{expiry}` template variable to the registration, email change, and password reset emails.

* Fixed a crash in the `chanserv/suspend` and `nickserv/suspend` commands.

* Fixed checking whether a nickname is a guest nickname.

* Redocumented the email configuration.

## Developer Changes

* Added `Mail::Template` for sending templatable and translatable mail messages.

## Anope 2.1.25 (2026-07-10)

### General Changes

* Added a cooldown timer to `hs_request` for after a vhost is approved or rejected.

* Added environment variables for configuring hashing algorithms to `anope-mkpasswd`.

* Added support for building with position-independent code on supported platforms.

* Added support for prioritising WHOIS entries in `ns_set_misc`.

* Documented the `ns_flexible` and `ns_monospace` account options.

* Dropped support for the `+draft/reply` tag in favour of the non-draft variant.

* Fixed a crash on shutdown when trying to send a global without an uplink.

* Fixed a crash when a temporary ban expires.

* Fixed a crash when trying to upgrade TLS fingerprints from an older algorithm.

* Fixed a potential crash caused by unusual timestamps being passed to `Anope::strftime`.

* Fixed confirming accounts when `{ns_register}:registration` is set to "admin".

* Fixed edge cases relating to IRCd-sent account names.

* Fixed extra CMake arguments from `./Config` being ignored.

* Fixed flag migration sometimes giving users higher privileges than they had originally when a flag provides multiple privileges.

* Fixed not being able to update the description of a flags access entry.

* Fixed not migrating the last seen time with an access entry.

* Fixed not using the extra CMake arguments from `./Config`.

* Fixed some command descriptions not being imperative statements.

* Fixed some help messages that did not reflect the default flag/level/xop access when changed in the configuration.

* Fixed the default memory hardness in the `enc_argon2` module.

* Improved configuring paths at build time.

* Improved help output for the `nickserv/set/timezone` command.

* Improved importing databases from Atheme.

* Improved support for reproducible builds.

* Redocumented the `nickserv/set/keepmodes` command.

* Separated the delay between nickname registrations from the delay between grouping nicknames.

* Updated the Portuguese translation.

* Updated the Romanian translation.

### Developer Changes

* Added `Block::GetBlocks` to allow using a range-based for loop to iterate config blocks.

* Added an event to `IRCDProto` to populate common tags onto a S2S message.

* Added automated testing on InspIRCd, Solanum, and UnrealIRCd using irctest.

* Added support for adding ajoin entries from modules.

* Changed various configuration block index fields to use an unsigned integer.

* Changed vendored libraries to be built as static libraries and linked into the modules that require them.

* Simplified timer repeat logic by allowing tick events to return bool to control whether the timer continues running.

## Anope 2.1.24 (2026-04-01)

### Breaking Changes

* If a database contains duplicate corrupt entries from a prior write failure the oldest ones will now be purged from the database. This is a destructive action so make sure you take a manual backup of your database before upgrading.

* Removed support for storing the Anope database in Redis. The Redis code was extremely bitrotted, had not been tested in years, and to our knowledge has almost no (if any) users. It is recommended that db_redis users migrate to db_json or db_sql.

* SQL tables now use versioned prefixes by default. For the SQL database backends the default is `anope21_` and for ChanStats the default is `chanstats21_`. If you do not have a prefix explicitly set in your config you will need to add one it. Alternatively, you may also want to consider exporting to db_json and re-importing to update your SQL schema for the recent database layout changes.

### Changes

* Added some helper methods to `CommandSource` to allow quickly translting messages.

* Changed the Config script to allow multiple dashes in front of options, i.e. `-quick` and `--quick` are now equivalent.

* Converted some language strings to use format strings instead of concatenation.

* Fixed a rare crash in the ns_cert module.

* Fixed building Anope as a unity build.

* Fixed the ns_cert module erasing certificate entries if using an old database.

* Fixed users having the wrong real name in log messages on InspIRCd if it has been previously changed with `CHGNAME` or `SETNAME`.

## Anope 2.1.23 (2026-04-01)

### Changes

* Added examples to several BotServ commands.

* Added missing fields to the `RPL_STATSLINKINFO` output.

* Added support for migrating access entries between systems (currently only `chanserv/flags` is supported).

* Added the default levels to the `chanserv/levels` DESC help.

* Changed access listing commands to only show their own access entries unless `ALL` is specified.

* Fixed a non-translatable string which has been marked as translatable.

* Fixed the missing AUTOLOGIN extension.

* Fixed translating the help output when the flexible layout is used.

* Improved the accuracy of the X-line expiry time in `operserv/stats`.

* Updated the Portuguese translation.

* Updated the Romanian translation.

## Anope 2.1.22 (2026-03-01)

### Breaking Changes

* Automatic login using a known SSL fingerprint now requires the `AUTOLOGIN` option to be set on accounts with `/NS SET AUTOLOGIN ON`. Automatic login is largely obsolete now SASL EXTERNAL exists and is widely supported.

* Conan 2 is now used for packaging dependencies on Windows. If you are building from source you will need to upgrade Conan.

* Non-breaking spaces in translatable messages now use 0x1B instead of 0x1A due to recent msgfmt releases treating 0x1A as an EOF character. If you have an out of tree translation you will need to update it.

* User TLS certificates are now stored in their own `NSCert` table instead of as a column in the `NickCore` table. If you are reading this information you will need to update your code.

### Changes

* Fixed `{botserv}:botmodes` erroneously allowing setting non-status modes on channels.

* Fixed echoing message tags on Solanum.

* Fixed handling incoming `FIDENT` messages on InspIRCd.

* Fixed matching stacked extended bans on InspIRCd and UnrealIRCd.

* Fixed parameter modes in `chanserv/mode` locks erroneously requiring a parameter to unset a lock.

* Fixed restoring the object identifier when unserialising objects in db_json.

* Fixed the consistency of indenting and line wrapping command examples in help output.

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
  * [nick1] Mastodon: @example@mastodon.social
  * [nick1] irc.example.com :Example-IRC server
  ...
  * [nick1] End of WHOIS list.
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

## Anope 2.1.18 (2025-10-01)

### Changes

* Added a check that a non-deprecated database module is loaded.

* Added support for flexible and monospace layouts to make text easier to read on clients that use a variable-width font.

* Added support for logging about deprecated modules on boot.

* Added support for per-IRCd hints when a link fails.

* Added support for self-service validation of vhosts using DNS TXT records.

* Added support for separate bad password limits for pre-connection SASL authentication.

* Added support for SRV and TXT records to the dns module.

* Added the --nodb option to disable database and encrytption module checks.

* Added the nickname registration date to the nickserv/glist output.

* Changed db_flatfile to be import-only (migrate to db_json).

* Changed the default registration confirmation type to code validation.

* Changed the fantasy !help command to not require the FANTASY privilege by default.

* Changed various length measurement code to be UTF-8 aware.

* Disabled the nickname registration delay by default.

* Fixed reporting the MySQL version that the mysql module was built against.

* Improved the layout of the nickserv/info command.

* Modularised the ns_sasl module to make it easier to pick SASL mechanisms.

* Moved duration rounding logic from Anope::Expires to Anope::Duration.

* Removed support for importing old databases from 1.8.

* Removed support for verifying "old MD5" passwords from 1.7.

* Reworked how memory is allocated when formatting messages.

### Config Changes

* Added the hostserv/validate command.

* Added the nickserv/saset/layout command.

* Added the nickserv/set/layout command.

* Added the ns_sasl_anonymous module.

* Added the ns_sasl_external module.

* Added the ns_sasl_plain module.

* Added the ns_set_layout module.

* Added the ns_set_op module.

* Added {hs_request}:validationcooldown (defaults to 5 minutes).

* Added {hs_request}:validationrecord (defaults to "anope-dns-validation").

* Added {ns_sasl}:badpasslimit (defaults to options:badpasslimit).

* Added {ns_sasl}:badpasstimeout (defaults to options:badpasstimeout).

* Moved nickserv/set/autoop and nickserv/saset/autoop to the ns_set_op module.

* Moved nickserv/set/display and nickserv/saset/display to the ns_set_group module.

* Moved nickserv/set/neverop and nickserv/saset/neverop to the ns_set_op module.

* Removed the db_old module.

* Removed the enc_old module.

* Removed {db_flatfile}:fork (module is now import-only).

* Removed {db_flatfile}:keepbackups (module is now import-only).

* Removed {db_flatfile}:nobackupokay (module is now import-only).

## Anope 2.1.17 (2025-08-01)

### Changes

* Allowed opers to resend passwords for users in nickserv/resend.

* Fixed HostServ using a different valid username character set to the protocol module.

* Fixed losing the channel and nickname registration time when upgrading from an earlier 2.1 release.

* Improved the messages sent when a user is forced off a protected nickname.

* Simplified copying modules to the runtime directory on Windows.

### Config Changes

* Added the nickserv/resend oper privilege.

## Anope 2.1.16 (2025-07-01)

### Changes

* Added support for on-IRC code confirmation.

* Added the ability for fantasy commands to be executable without the FANTASY privilege.

* Added the ability to prepend to topics as well as appending to them.

* Changed various fields to serialize to the database as a string not an integer.

* Disabled db_flatfile by default in preparation for becoming import-only.

* Fixed a memory leak in the db_json module.

* Fixed building on OpenSSL 1.1.1 (for now).

* Fixed removed and later re-added temporary bans being removed automatically.

* Fixed sometimes sending malformed LMODE messages on InspIRCd.

* Fixed the "did you mean" message suggesting unloaded commands.

* Fixed various issues with the example config files.

* Marked db_json as the recommended database module.

* Moved the BAN, UNBAN, and KICK commands to the chanserv/management group.

* Removed support for the 1.8-style seen command.

* Reworked confirmation to allow confirmation of multiple account actions.

* When dropping a display nickname the new display will now be the oldest in the group.

### Config Changes

* Added fantasy:require_privilege (defaults to yes).

* Added the nickserv/confirm/email command.

* Added the nickserv/confirm/email oper privilege.

* Added the nickserv/confirm/register command.

* Added the ns_confirm module.

* Added {ns_email}:changeexpire (defaults to 1 day).

* Added {ns_resetpass}:resetexpire (defaults to 1 day).

* Removed the irc2sql module (migrate to JSON-RPC instead).

* Removed {ns_seen}:simple (1.8-style seen has been removed).

* Renamed the nickserv/confirm oper privilege to nickserv/confirm/register.

## Anope 2.1.15 (2025-06-01)

### Changes

* Added a workaround to the jsonrpc module for JavaScript truncating big integers.

* Added an example Ruby library for accessing the RPC interface.

* Added away state and tls usage to the anope.user RPC event.

* Added support for looking up accounts by identifier in the anope.account RPC event.

* Added support for storing the setter and set time and setter of list modes and restoring them on InspIRCd and Solanum.

* Added support for token authentication to the RPC modules.

* Added the anope.checkCredentials, anope.identify, anope.listCommands, and anope.command RPC events to the new rpc_user module.

* Bumped the minimum supported version of ircd-hybrid to 8.2.34.

* Deprecated irc2sql in favour of rpc_data.

* Dropped support for Bahamut as it has no known users.

* Fixed creating duplicate Stats rows on some servers.

* Fixed loading databases in db_json.

* Fixed restoring cloaked hosts on InspIRCd when the cloak module is not loaded.

* Fixed some variable shadowing that potentially caused issues with the SQL database backends.

* Fixed sometimes writing accounts to the database without a unique identifier.

* Fixed various documentation issues with the example JavaScript JSON-RPC client.

* Improved CTCP handling and added support for more CTCP types.

### Config Changes

* Added the ns_email module.

* Added the rpc_user module.

* Added {jsonrpc}:integer_bits (defaults to 64).

* Added {jsonrpc}:token.

* Added {nickserv}:enforcerreal (defaults to "Services Enforcer").

* Added {xmlrpc}:token.

* Moved nickserv/set/email and nickserv/saset/email to the ns_email module.

* Removed the bahamut module.

* Removed the ns_getemail module (load ns_email instead).

* Removed the ns_maxemail module (load ns_email instead).

* Removed the rpc_main module (migrate to the other RPC modules).

* Removed {chanstats}:cs_def_chanstats (add CS_STATS to {chanserv}:defaults instead).

* Removed {chanstats}:ns_def_chanstats (add NS_STATS to {nickserv}:defaults instead).

* Renamed service:gecos to service:real.

## Anope 2.1.14 (2025-05-02)

### Changes

* Added a detail specifier to the anope.list{Channels,Opers,Servers,Users} RPC methods.

* Added a matcher for the InspIRCd oper extban.

* Added support for hashed operator passwords.

* Added support for hiding the date news was added in os_news.

* Added support for local password comparison to the sql_authentication module.

* Added support for monthly backups to db_json.

* Added support for unbanning virtual modes using cs_unban.

* Added the !unmute fantasy command.

* Added the anope.account and anope.listAccounts RPC methods to the rpc_data module.

* Added the protection time to the INFO output.

* Allowed unprivileged channel successors to remove themselves from succession.

* Bumped the minimum required CMake version to 3.20.

* Changed deletion callbacks to specify the mask that was deleted if only one was.

* Changed nickserv/alist to show all permissions not just the highest ranked one.

* Fixed NEVEROP not being respected in chanserv/set/founder and chanserv/set/successor.

* Fixed stripping IRC formatting codes from messages.

* Messages are now automatically line wrapped to options:linelength.

* Redocumented the ns_sasl module.

* Removed hardcoded command names from several messages.

* Updated the Windows CI to Windows Server 2025 and Visual Studio 2022.

### Config Changes

* Added oper:password_hash.

* Added options:codelength (defaults to 15).

* Added {os_news}:showdate (defaults to yes).

* Added {sql_authentication}:password_field (defaults to "password").

* Added {sql_authentication}:password_hash.

* Changed the default value for options:linelength to "100".

* Changed the default value for {enc_sha2}:algorithm to "sha512".

* Changed the default value for {ns_seen}:purgetime to "90d".

* Changed the syntax for template variables in mail:emailchange_message, mail:emailchange_subject, mail:memo_message, mail:memo_subject, mail:registration_message, mail:registration_subject, mail:reset_message, mail:reset_subject, {chanserv}:signkickformat, {dnsbl}:blacklist:reason, {ldap_authentication}:search_filter, {ldap_oper}:binddn, {ldap_oper}:filter, {nickserv}:unregistered_notice, {os_session}:sessionlimitexceeded, {proxyscan}:proxyscan:reason.

## Anope 2.1.13 (2025-04-01)

### Changes

* Added a Config check to ensure users actually want to use the development branch.

* Added a flag to the version string when Anope is compiled in reproducible mode.

* Added a warning on rehash when the max password is longer than the maximum bcrypt password length.

* Added an ALLTIME handler on InspIRCd.

* Added an opt-out for extended XML-RPC types.

* Added RPC messages for sending global messages.

* Added support for importing cs_set_misc and ns_set_misc data from Atheme.

* Added support for importing news from Atheme.

* Added support for oper-only quit messages.

* Added support for the experimental "services cloak" system from the InspIRCd development branch.

* Added support for using defines from the environment.

* Added support for using defines within the value of a variable.

* Blacklisted an old version of an UnrealIRCd module that is known to send malformed S2S messages.

* Changed RPC events to be registered as core services.

* Changed the database to refer to accounts by their account identifier instead of their display nick.

* Changed the syntax of defines from "foo" to "${foo}".

* Deduplicated JSON generation code in the jsonrpc module.

* Fixed a warning when importing an Atheme account that uses external authentication.

* Fixed counting email addresses in ns_maxemail.

* Fixed db_atheme creating duplicate accounts, bots, and nicks when importing over an existing database.

* Fixed deleting old database backups after Anope has been restarted.

* Fixed importing user metadata from Anope 1.8.

* Fixed including a port in uplink messages when connecting to a UNIX socket endpoint.

* Fixed memo ignores being erroneously case sensitive.

* Fixed modules with third-party dependencies writing generic log messages instead of module log messages.

* Fixed not performing SQL database updates in some rare circumstances.

* Fixed sending global messages with the default sender.

* Imported mkauthors from InspIRCd and used it to generate docs/AUTHORS.txt

* Moved around a bunch of module headers.

* Moved database serialization from the serializable to the serializable type.

* Moved the SASL protocol interface to its own service.

* Refactored handling S2S metadata on InspIRCd.

* Updated more messages to use gettext plural forms.

### Config Changes

* Added options:linelength (defaults to 120).

* Added the db_json module.

* Added the rpc_message module.

* Added {nickserv}:defaultprotect (defaults to 1m).

* Added {nickserv}:maxprotect (defaults to 10m).

* Added {nickserv}:minprotect (defaults to 10s)

* Added {xmlrpc}:enable_i8 (defaults to yes).

* Added {xmlrpc}:enable_nil (defaults to yes).

* Changed the syntax for using defines (all existing defines will need to be updated).

* Removed {nickserv}:kill (replaced by custom protection timer durations).

* Removed {nickserv}:killquick (replaced by custom protection timer durations).

* Removed {ns_set_kill}:allowkillimmed (replaced by custom protection timer durations).

* Renamed the nickserv/saset/kill command to nickserv/saset/protect.

* Renamed the nickserv/saset/kill oper privilege to nickserv/saset/protect.

* Renamed the nickserv/set/kill command to nickserv/set/protect.

* Renamed the ns_set_kill module to ns_set_protect.

* Renamed the sasl module to ns_sasl and moved it to nickserv.example.conf.

## Anope 2.1.12 (2025-03-01)

### Changes

* Added an example JavaScript library for accessing the RPC interface.

* Added an option to require specifying the server name when running destructive network commands like restart or shutdown.

* Added support for importing X-line identifiers from Atheme.

* Added support for JSON-RPC to the RPC interface.

* Added support for killing SASL users that fail to log in repeatedly.

* Added support for more RPC response types to the RPC interface.

* Added support for multiple targets in chanserv/modes.

* Added support for SSL client certificate fallback fingerprints on InspIRCd.

* Added the anope. prefix to the channel and user RPC events and moved them to the rpc_data module.

* Added the anope.listChannels, anope.listServers, anope.listUsers, and anope.server RPC events to the new rpc_data module.

* Added the OPERONLY, UNUSED and VANITY filters to botserv/botlist.

* Added the system.listMethods RPC event to the new rpc_system module.

* Deprecated support for InspIRCd v3 (scheduled to be removed in around a year).

* Fixed enc_bcrypt silently truncating passwords longer than 71 characters.

* Fixed ns_set_language being able to be loaded when Anope was built without language support.

* Fixed sql_authentication not being able to use the IP address of a RPC, SASL, or web user in SQL queries.

* Made modules that use third-party libraries log the version in use on load.

* Redesigned the RPC interface to add support for emitting multiple data types.

* Replaced the opers RPC event with rpc.listOpers and rpc.oper events in the new rpc_data module.

* Updated the Dutch translation.

### Config Changes

* Added the jsonrpc module.

* Added the rpc_data module.

* Added the rpc_system module.

* Added {hostserv}:activate_on_deoper (defaults to yes).

* Added {os_shutdown}:requirename (defaults to yes).

* Moved nickserv/set/keepmodes and nickserv/saset/keepmodes to the ns_set_keepmodes module.

* Moved the xmlrpc module to extra.

* Renamed the xmlrpc_main module to rpc_main.

## Anope 2.1.11 (2024-12-01)

### Changes

* Added support for database migrations to the mysql module.

* Added support for renicking users to their UID when enforcing nickname protection.

* Added support for sending channel URLs to joining users.

* Allowed selecting languages using a shorter version of their name.

* Changed various messages to use human-readable durations instead of seconds.

* Improved the creation of expiry and duration messages.

* Improved the translation system with support for plural forms.

* Reworked how guest nicknames are generated.

* Simplified how account identifiers are allocated.

### Config Changes

* Moved nickserv/set/kill and nickserv/saset/kill to the ns_set_kill module.

* Moved {ns_set}:allowkillimmed to {ns_set_kill}:allowkillimmed.

* Replaced {nickserv}:guestnickprefix with {nickserv}:guestnick (defaults to Guest####).

## Anope 2.1.10 (2024-11-01)

### Changes

* Added support for NEXTBANS on UnrealIRCd.

* Changed hostmask access entries added by nick to use that nick as the default description.

* Changed modes to be handled internally in their split form.

* Changed ns_cert to notify a user that their certificate is being automatically added to their account.

* Fixed matching users against extended bans.

* Fixed parsing name-only extended bans on InspIRCd.

* Fixed respecting the preferred extended ban format on InspIRCd.

* Fixed the name of the cron script in the docs.

* Updated the list of supported IRCds.

* Updated the location of the Anope IRC channels

### Config Changes

* Added options:servicealias (defaults to no)

* Moved nickserv/set/message and nickserv/saset/message to the ns_set_message module.

* Removed options:useprivmsg (replaced by loading the ns_set_message module to enable).

* Removed options:usestrictprivmsg (feature unusable on modern servers, consider migrating to options:servicealias instead).

## Anope 2.1.9 (2024-10-01)

### Changes

* Bumped the minimum supported version of UnrealIRCd to 6.

* Fixed granting IRC operator status to services operators.

* Fixed making users an IRC operator on InspIRCd.

* Fixed nonicknameownership on InspIRCd v4.

* Fixed some messages not being translatable.

* Fixed the Argon2 module not having test vectors.

* Increased the default nickname expiry period to one year.

## Anope 2.1.8 (2024-09-01)

### Changes

* Added account identifiers to the nickserv/info output.

* Added support for bool, float, and uint SQL columns.

* Added the ability to automatically determine SQL column types based on the native type.

* Added UNIX socket support to mysql module.

* Changed smartjoin to use SendClearBans where available.

* Dropped support for MinGW in favour of native builds.

* Fixed parsing named extbans on InspIRCd.

* Fixed parsing SVSMODE and SVS2MODE from UnrealIRCd.

* Fixed sending global messages to remotely linked servers.

* Removed the services server name from the CTCP version response.

### Config Changes

* Added the nickserv/drop/display oper privilege.

* Added {nickserv}:preservedisplay (defaults to no).

## Anope 2.1.7 (2024-07-01)

### Changes

* Added importing of akick reasons, forbid reasons, opers and session exceptions to db_atheme.

* Added support for sending tag messages.

* Added the ability to look up account information of an authenticated user.

* Fixed a crash in ns_cert when an IRC user is not present during a nick registration.

* Fixed a null pointer dereference in the global module.

* Fixed a rare memory leak in os_akill and os_sxline.

* Improved the performance of some code that looks up the primary nick from an account.

* Removed the broken Catalan, Hungarian, and Russian translations.

* Reworked the protocol interface for sending messages.

* Updated the Turkish translation.

### Config Changes

* Moved nickserv/set/language and nickserv/saset/language to the ns_set_language module.

* Renamed the FANTASIA channel privilege to FANTASY.

* Renamed {cs_suspend}:expire to {cs_suspend}:suspendexpire.

## Anope 2.1.6 (2024-06-01)

### Changes

* Added opportunistic upgrading of TLS fingerprints to more secure algorithms on InspIRCd.

* Added support for logging out partially connected users on Plexus.

* Added the account registration time to nickserv/info.

* Changed ns_cert to automatically add a TLS fingerprint to new accounts if available.

* Clarified that a non-deprecated encryption module must be loaded.

* Fixed creating the runtime directory on Windows.

* Fixed mistakenly allowing badpasslimit to be set to a negative value.

* Fixed parsing backup TLS fingerprints on InspIRCd.

* Fixed parsing the flood mode on UnrealIRCd.

* Fixed parsing the history mode on UnrealIRCd.

* Fixed various iterator invalidation issues.

* Partially rewrote the Portuguese translation.

* Removed some incorrect strings from the Turkish translation.

* Renamed the --modulesdir option to --moduledir to match the name of other path options.

### Config Changes

* Added {ns_cert}:automatic (defaults to yes).

* Removed {hybrid,inspircd,solanum,unrealircd}:use_server_side_mlock (now always enabled).

* Removed {inspircd}:use_server_side_topiclock (now always enabled).

## Anope 2.1.5 (2024-05-01)

### Changes

* Added an example systemd unit file.

* Added support for BIGLINES on UnrealIRCd.

* Bumped the minimum supported version of Bahamut to 2.0.

* Fixed truncating messages in global/global and global/server.

* Improved building Anope for use as a system package.

* Updated the Turkish translation.

### Config Changes

* Added the nickserv/drop/override and chanserv/drop/override oper privileges.

## Anope 2.1.4 (2024-04-01)

### Changes

* Added a check for a non-deprecated encryption module on start.

* Added a way for protocol modules to report an error to the uplink.

* Added more account settings to the webcpanel.

* Added self-test functionality for all encryption modules.

* Added support for challenge authentication on InspIRCd.

* Added support for importing databases from Atheme.

* Added support for sending client tags on UnrealIRCd.

* Added support for the InspIRCd 1206 (v4) protocol.

* Added the --nopid option to disable writing a pid file.

* Added the enc_argon2 module to encrypt passwords with Argon2.

* Added the enc_sha2 module to encrypt passwords with HMAC-SHA-2.

* Added the global/queue command for queueing multi-line messages.

* Added the global/server command for sending messages to an individual server.

* Added the PASSWORD category to operserv/stats to view password encryption methods.

* Added the verify-only enc_posix module to validate passwords from Atheme that were encrypted with Argon2.

* Changed nickserv/drop to use confirmation codes to confirm a nickname drop.

* Changed various paths to be relative to the data and config directories.

* Converted some IRCDProto member functions to variables.

* Converted the enc_md5, enc_none, enc_old, enc_sha1, and enc_sha256 modules to be verify-only.

* Deduplicated page headers and footers in the webcpanel templates.

* Deprecated the enc_sha256 module.

* Fixed inconsistent spelling/casing of email, vhost, and vident.

* Fixed various bugs in the inspircd module.

* Improved portability of email sending.

* Improved protocol debug messages.

* Improved the performance and reliability of internal conversion logic.

* Improved the randomness of randomly generated numbers.

* Refactored the enc_bcrypt module and exposed it as an encryption context.

* Removed several duplicate translation strings.

* Replaced the custom MD5 implementation in enc_md5 with a vendored one.

* Replaced the custom SHA256 implementation in enc_sha256 with a vendored one.

* The ldap_authentication, ldap_oper, sql_authentication, sql_log, and sql_oper modules are now always enabled.

### Config Changes

* Added the db_atheme module.

* Added the enc_argon2 module.

* Added the enc_posix module.

* Added the enc_sha2 module.

* Added the gl_queue module.

* Added the gl_server module.

* Added the global/queue oper privilege.

* Added the global/server oper privilege.

* Changed serverinfo:motd to be relative to the config directory.

* Changed serverinfo:pid to be relative to the data directory.

* Changed the default value of mail:sendmailpath to "/usr/sbin/sendmail -it".

* Changed the default value of {chanserv}:accessmax to 1000.

* Changed the default value of {chanserv}:inhabit to 1 minute.

* Changed the default value of {cs_mode}:max to 50.

* Changed the default value of {ms_ignore}:max to 50.

* Removed options:seed (not needed with modern random number generation).

* Replaced {webcpanel}:template with {webcpanel}:template_dir (defaults to webcpanel/templates/default).

## Anope 2.1.3 (2024-03-04)

### Changes

* Added alternate command suggestions when a user runs an invalid command.

* Added support for the IRCv3 +draft/channel-context tag.

* Added support for the IRCv3 +draft/reply tag.

* Allow using more than one fingerprint in an oper block.

* Changed chanserv/drop to use confirmation codes to confirm a channel drop.

* Cleaned up more of the codebase to use Modern C++17.

* Enabled using more field limits sent by the IRC server instead of requiring the user to configure them.

* Fixed NickServ lying about the minimum password length.

* Fixed a crash when sending emails.

* Fixed bs_kick not using the correct kick message for automatic kicks.

* Increased the security of randomly generated confirmation codes.

* Removed the ns_access module and associated cs_secure and ns_secure options.

* Removed the ns_status module.

* Reworked how messages are sent in protocol modules to allow sending message tags.

### Config Changes

* Added options:didyoumeandifference (defaults to 4).

* Added support for multiple SSL fingerprints in oper:certfp.

* Added the chanserv/cert oper privilege for modifying other user's certificate lists.

* Changed networkinfo:chanlen to default to 32 if not set.

* Changed networkinfo:hostlen to default to 64 if not set.

* Changed networkinfo:modelistsize to default to 100 if not set.

* Changed networkinfo:nicklen to default to 31 if not set.

* Changed networkinfo:userlen to default to 10 if not set.

* Increased the default maximum password length to 50 characters.

* Increased the default minimum password length to 10 characters.

* Removed the cs_secure option in {chanserv}:defaults (now always enabled as support for nick access lists has been removed).

* Removed the nickserv/saset/secure command (support for nick access lists has been removed).

* Removed the nickserv/saset/secure oper privilege (support for nick access lists has been removed).

* Removed the nickserv/set/secure command (support for nick access lists has been removed).

* Removed the nickserv/status command (obsolete with modern IRCv3 extensions and the removal of nick access lists).

* Removed the ns_access module (support for nick access lists has been removed).

* Removed the ns_secure option in {nickserv}:defaults (now always enabled as support for nick access lists has been removed).

## Anope 2.1.2 (2024-02-17)

### Changes

* Bumped the minimum OpenSSL version to 1.1.0.

* Bumped the minimum GnuTLS version to 3.0.0.

* Disabled SSLv3 support in the m_ssl_openssl module.

* Modernized mutex and thread code to use Modern C++.

* Normalised the program exit codes.

* Updated the Dutch translation.

* Updated the French translation.

### Config Changes

* Added {ssl_openssl}:tlsv10 for configuring whether TLSv1.0 is usable (defaults to no).

* Added {ssl_openssl}:tlsv11 for configuring whether TLSv1.1 is usable (defaults to yes).

* Added {ssl_openssl}:tlsv12 for configuring whether TLSv1.2 is usable (defaults to yes).

* Removed the m_ prefix from the names of the chanstats, dns, dnsbl, helpchan, httpd, ldap, ldap_oper, mysql, proxyscan, redis, regex_pcre2, regex_posix, regex_stdlib, regex_tre, rewrite, sasl, sql_log, sql_oper, sqlite, ssl_gnutls, ssl_openssl, xmlrpc, and xmlrpc_main modules.

* Removed {ssl_openssl}:sslv3 (now always disabled).

## Anope 2.1.1 (2024-01-04)

### Changes

* Added the UNBANME privilege to allow users to unban themselves.

* Fixed building on Windows systems without chgrp/chmod.

* Fixed creating sockets in the m_dns, m_httpd, m_proxyscan, and m_redis modules.

* Fixed reading the values of command line arguments.

* Moved core privilege descriptions to the example configs.

* Updated the Italian translation.

* Updated the Polish translation.

### Config Changes

* Added the m_regex_stdlib module.

* Removed the m_regex_pcre module (use m_regex_pcre2 instead).

## Anope 2.1.0 (2024-12-24)

### Changes

* Added support for access list entry descriptions.

* Added support for linking over a UNIX socket.

* Added support for server-initiated logins and logouts on UnrealIRCd.

* Added support for server-initiated logouts on InspIRCd.

* Added support for the ANONYMOUS SASL mechanism.

* Allowed users to opt-out of being added to channel access lists.

* Cleaned up the codebase to use Modern C++17.

* Modernized the build system to use a modern version of CMake.

* Removed support for using insecure encryption methods as the primary method.

* Removed the Windows-only anopesmtp tool.

* Removed the two day X-line cap.

* Updated all references to IRCServices to refer to Anope instead.

### Config Changes

* Added {nickserv}:minpasslen for configuring the minimum password length (defaults to 8).

* Removed {nickserv}:strictpasswords (obsolete now {nickserv}:minpasslen exists).

* Removed the inspircd12 and inspircd20 modules (use inspircd instead).

* Removed the ns_getpass module (no supported encryption modules).

* Removed the os_oline module (no supported IRCds).

* Removed the unreal module (use unrealircd instead).

* Renamed {nickserv}:passlen to {nickserv}:maxpasslen.

* Renamed the charybdis module to solanum.

* Renamed the inspircd3 module to inspircd.

* Renamed the unreal4 module to unrealircd.

* Replaced uplink:ipv6 with uplink:protocol (defaults to ipv4).

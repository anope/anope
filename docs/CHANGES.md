# Anope Change Log

## Anope 2.1.19 (unreleased)

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

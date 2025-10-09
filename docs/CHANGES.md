# Anope Change Log

## Anope 2.1.19 (unreleased)

<!-- Last updated for 9f6f0b445bc9387d32a78a089bea529581049683 -->

### Breaking Changes

* `pkg-config` is now required to find dependencies for the following modules on UNIX systems:
  - enc_argon2
  - ldap
  - mysql
  - regex_pcre2
  - regex_tre

### Changes

* Added support for associating a timezone with an account to allow users to receive timestamps in their local timezone.

* Added support for language-specific time formats.

* Reordered the information in the `nickserv/info` command output to show the registration dates before the seen information.

* Updated the vendored libraries.

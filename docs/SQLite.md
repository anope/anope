# How to modify Anope sqlite databases

Anope uses a custom SQLite function (anope_canonicalize) for canonicalizing strings for case insensitive comparisons.
It does this by using SQLites support for having indexes on expressions https://www.sqlite.org/expridx.html.

For example the account table could look like:

```
CREATE TABLE `anope_account` (
	`id` INTEGER PRIMARY KEY AUTOINCREMENT,
	`display`,
	`pass`,
	`email`,
	`language`
);

CREATE INDEX idx_display ON `anope_account` (anope_canonicalize(display));
```

So, to do a SELECT which utilizes the indicies, Anope does something like:

```
SELECT id FROM `anope_account` WHERE anope_canonicalize(display) = anope_canonicalize('Adam');
```

If you are using your own SQLite instance, like the sqlite command line interface, the anope_canonicalize function
will be missing. To fix this load one of libanope_sqlite_ascii.so, libanope_sqlite_rfc1459_inspircd.so,
or libanope_sqlite_rfc1459.so into SQLite, depending on your casemap configuration.

```
sqlite> .load lib/libanope_sqlite_ascii.so
```

## Example of registering a new user via SQLite

```
BEGIN TRANSACTION;
-- Insert new account
INSERT INTO anope_account (display, email, private, autoop, killprotect) VALUES ('Adam', 'adam@anope.org', 1, 1, 1);
-- Insert nickname, linking it to the account
INSERT INTO anope_nick (nick, account) VALUES ('Adam', last_insert_rowid());
COMMIT;
```

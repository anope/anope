# How to modify Anope sqlite databases

Anope uses a custom SQLiite function (anope_canonicalize) for canonicalizing strings for case insensitivity comparisons.
It does this by using SQLites support for having indexes on expressions https://www.sqlite.org/expridx.html.

For example the account table could look like:

```
CREATE TABLE `anope_db_account` (
	`display`,
	`pass`,
	`email`,
	`language`,
	`id` NOT NULL PRIMARY KEY,
	FOREIGN KEY (id) REFERENCES anope_db_objects(id) ON DELETE CASCADE DEFERRABLE INITIALLY DEFERRED
);
CREATE INDEX idx_display ON `anope_db_account` (anope_canonicalize(display));
```

So, to do a SELECT which utilizes the indicies, Anope does something like:

```
SELECT id FROM `anope_db_account` WHERE anope_canonicalize(display) = anope_canonicalize('Adam');
```

If you are using your own SQLite instance, like the sqlite command line interface, the anope_canonicalize function
will be missing. To fix this load one of libanope_sqlite_ascii.so, libanope_sqlite_rfc1459_inspircd.so,
or libanope_sqlite_rfc1459.so into SQLite, depending on your casemap configuration.

```
sqlite> .load lib/libanope_sqlite_ascii.so
```

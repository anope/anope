# How to install rfc1459 character set into MySQL:

Copy data/mysql/rfc1459.xml to your character_sets_dir, identified by

```
SHOW VARIABLES LIKE 'character_sets_dir';
```

Add the following charset to Index.xml in the character_sets_dir directory:

```
<charset name="rfc1459">
  <collation name="rfc1459_ci" id="800"/>
  <collation name="rfc1459_inspircd_ci" id="801" flag="primary"/>
</charset>
```

Restart MySQL.

Now create the database using the appropriate character set:

```
CREATE DATABASE anope DEFAULT CHARACTER SET rfc1459 DEFAULT COLLATE rfc1459_inspircd_ci;
```

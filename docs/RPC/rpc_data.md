# Anope `rpc_data` RPC interface

## `anope.listAccounts`

Lists all accounts that exist on the network.

### Parameters

Index | Description
----- | -----------
0     | If specified then the level of detail to retrieve. Can be set to "full" to retrieve all information or "name" to just retrieve the account names. Defaults to "name".

### Errors

Code   | Description
------ | -----------
-32099 | The specified detail level does not exist.

### Result

If the detail level is not specified or is "name" then an array of account names.

If the detail level is "full" then a mapping of account names to information about the account. See `anope.account` for more information on the data structure.

#### Example

```json
["account1", "account2", "account3"]
```

## `anope.account`

Retrieves information about the specified account.

### Parameters

Index | Description
----- | -----------
0     | A nickname belonging to the account.

### Errors

Code   | Description
------ | -----------
-32099 | The specified account does not exist.

### Result

Returns a map containing information about the account.

Key                    | Type           | Description
---                    | ----           | -----------
display                | string         | The display nickname of the account.
email                  | string or null | The email address associated with the account or null if one has not been specified.
extensions             | map            | A key-value map of the extensions set on this account by modules.
language               | string or null | The language associated with the account or null if the account uses the default language.
lastmail               | int            | The time at which an email was last sent for this account.
nicks                  | map            | Information about nicknames that belong to the account keyed by the nickname.
nicks.\*.extensions    | map            | A key-value map of the extensions set on this nickname by modules.
nicks.\*.lastseen      | int            | The time at which this nickname was last used.
nicks.\*.registered    | int            | The time at which this nickname was registered.
nicks.\*.vhost         | map or null    | The vhost associated with the account or null if the user has no vhost.
nicks.\*.vhost.created | int            | The time at which the vhost was created.
nicks.\*.vhost.creator | string         | The nickname of the creator of the vhost.
nicks.\*.vhost.host    | string         | The host segment of the vhost.
nicks.\*.vhost.ident   | string or null | The ident segment of the vhost or null if there is not one.
nicks.\*.vhost.mask    | string         | The user@host or host mask of the vhost.
opertype               | map or null    | The oper type associated with the account or null if they are not a services operator.
opertype.commands      | array[string]  | The commands that the services operator type can use.
opertype.name          | string         | The name of the services operator type.
opertype.privileges    | array[string]  | The privileges that the services operator type has.
registered             | int            | The time at which the account was registered.
uniqueid               | uint           | The unique immutable identifier of the account.
users                  | array[string]  | The IRC users who are currently logged in to this account.

#### Example

```json
{
	"display": "foo",
	"email": "example@example.com",
	"extensions": {
		"AUTOOP": true,
		"HIDE_EMAIL": true,
		"HIDE_MASK": true,
		"MEMO_RECEIVE": true,
		"MEMO_SIGNON": true,
		"NS_PRIVATE": true,
		"PROTECT": true,
		"PROTECT_AFTER": 69
	},
	"language": null,
	"lastmail": 1745071858,
	"nicks": {
		"foo": {
			"extensions": {
				"NS_NO_EXPIRE": true
			},
			"lastseen": 1745074153,
			"registered": 1745071857,
			"vhost": null
		},
		"bar": {
			"extensions": {},
			"lastseen": 1745072602,
			"registered": 1745071857,
			"vhost": {
				"created": 1745072653,
				"creator": "foo",
				"host": "bar.baz",
				"ident": "foo",
				"mask": "foo@bar.baz"
			}
		}
	},
	"opertype": {
		"commands": ["hostserv/*", "operserv/session"],
		"name": "Helper",
		"privileges": ["chanserv/no-register-limit"]
	},
	"registered": 1745071857,
	"uniqueid": 11085415958920757000,
	"users": [
		"foo"
	]
}
```
## `anope.listChannels`

Lists all channels that exist on the network.

### Parameters

Index | Description
----- | -----------
0     | If specified then the level of detail to retrieve. Can be set to "full" to retrieve all information or "name" to just retrieve the channel names. Defaults to "name".

### Errors

Code   | Description
------ | -----------
-32099 | The specified detail level does not exist.

### Result

If the detail level is not specified or is "name" then an array of channel names.

If the detail level is "full" then a mapping of channel names to information about the channel. See `anope.channel` for more information on the data structure.

#### Example

```json
["#chan1", "#chan2", "#chan3"]
```

## `anope.channel`

Retrieves information about the specified channel.

### Parameters

Index | Description
----- | -----------
0     | The name of the channel.

### Errors

Code   | Description
------ | -----------
-32099 | The specified channel does not exist.

### Result

Returns a map containing information about the channel.

Key         | Type          | Description
---         | ----          | -----------
created     | int           | The UNIX time at which the channel was originally created.
listmodes   | map           | List modes which are set on the channel keyed by the mode character.
modes       | array[string] | Flag and parameter modes which are set on the channel.
name        | string        | The name of the channel.
registered  | boolean       | Whether the channel is registered.
topic       | map or null   | The channel topic or null if no topic is set.
topic.setat | int           | The time at which the topic was set.
topic.setby | string        | The nick or nuh of the user who set the topic.
topic.value | string        | The text of the topic.
users       | array[string] | The users that are current in the channel prefixed by their status mode prefixes.

#### Example

```json
{
	"created": 1740402691,
	"listmodes": {
		"b": ["foo!bar@baz", "account:bax"],
	},
	"modes": ["+knrt", "secret"],
	"name": "#chan1",
	"registered": true,
	"topic": {
		"setat": 1740404706,
		"setby": "nick1",
		"value": "Example channel topic"
	},
	"users": ["@nick1", "nick2"]
}
```

## `anope.listOpers`

Lists all services operators that exist on the network.

### Parameters

Index | Description
----- | -----------
0     | If specified then the level of detail to retrieve. Can be set to "full" to retrieve all information or "name" to just retrieve the services operator nicknames. Defaults to "name".

### Errors

Code   | Description
------ | -----------
-32099 | The specified detail level does not exist.

### Result

If the detail level is not specified or is "name" then an array of services operator names.

If the detail level is "full" then a mapping of services operator names to information about the services operator. See `anope.oper` for more information on the data structure.

#### Example

```json
["nick1", "nick2", "nick3"]
```

## `anope.oper`

Retrieves information about the specified services operator.

### Parameters

Index | Description
----- | -----------
0     | The name of the services operator.

### Errors

Code   | Description
------ | -----------
-32099 | The specified services operator does not exist.

### Result

Returns a map containing information about the services operator.

Key                 | Type                  | Description
---                 | ----                  | -----------
fingerprints        | array[string] or null | The client certificate fingerprints that a user must be using to log in as this services operator or null if there are no client certificate restrictions.
hosts               | array[string] or null | The user@ip and user@ip masks that a user must be connecting from to log in as this services operator or null if there are no host restrictions.
name                | string                | The name of the services operator.
operonly            | boolean               | Whether a user has to be a server operator to log in as this services operator.
opertype            | map                   | The oper type associated with the services operator.
opertype.commands   | array[string]         | The commands that the services operator type can use.
opertype.name       | string                | The name of the services operator type.
opertype.privileges | array[string]         | The privileges that the services operator type has.
password            | boolean               | Whether a user has to specify a password to log in as the services operator.
vhost               | string or null        | The vhost of the services operator or null if there is no vhost.

#### Example

```json
{
	"fingerprints": null,
	"hosts": ["*@*.example.com"],
	"name": "stest",
	"operonly": true,
	"opertype": {
		"commands": ["hostserv/*", "operserv/session"],
		"name": "Helper",
		"privileges": ["chanserv/no-register-limit"]
	},
	"password": false,
	"vhost": null
}
```

## `anope.listServers`

Lists all servers that exist on the network.

### Parameters

Index | Description
----- | -----------
0     | If specified then the level of detail to retrieve. Can be set to "full" to retrieve all information or "name" to just retrieve the server names. Defaults to "name".

### Errors

Code   | Description
------ | -----------
-32099 | The specified detail level does not exist.

### Result

If the detail level is not specified or is "name" then an array of server names.

If the detail level is "full" then a mapping of server names to information about the server. See `anope.server` for more information on the data structure.

#### Example

```json
["irc1.example.com", "irc2.example.com", "services.example.com"]
```

## `anope.server`

Retrieves information about the specified server.

### Parameters

Index | Description
----- | -----------
0     | The name of the server.

### Errors

Code   | Description
------ | -----------
-32099 | The specified server does not exist.

### Result

Returns a map containing information about the server.

Key         | Type           | Description
---         | ----           | -----------
description | string         | The description of the server.
downlinks   | array[string]  | The servers which are behind this server
juped       | boolean        | Whether the server has been juped.
name        | string         | The name of the server.
sid         | string or null | The unique immutable identifier of the server or null if the IRCd does not use SIDs.
synced      | boolean        | Whether the server has finished syncing.
ulined      | boolean        | Whether the server is U-lined.
uplink      | string or null | The server in front of this server or null if it is the services server.

#### Example

```json
{
	"description": "Anope IRC Services",
	"downlinks": ["irc.example.com"],
	"juped": false,
	"name": "services.example.com",
	"sid": "00B",
	"synced": true,
	"ulined": true,
	"uplink": null
}
```

## `anope.listUsers`

Lists all users that exist on the network.

### Parameters

Index | Description
----- | -----------
0     | If specified then the level of detail to retrieve. Can be set to "full" to retrieve all information or "name" to just retrieve the user nicknames. Defaults to "name".

### Errors

Code   | Description
------ | -----------
-32099 | The specified detail level does not exist.

### Result

If the detail level is not specified or is "name" then an array of user nicknames.

If the detail level is "full" then a mapping of user nicknames to information about the user. See `anope.user` for more information on the data structure.

#### Example

```json
["nick1", "nick2", "nick3"]
```

## `anope.user`

Retrieves information about the specified user.

### Parameters

Index | Description
----- | -----------
0     | The nickname of the user.

### Errors

Code   | Description
------ | -----------
-32099 | The specified user does not exist.

### Result

Returns a map containing information about the user.

Key              | Type           | Description
---              | ----           | -----------
account          | map or null    | The user's account or null if they are not logged in to an account.
account.display  | string         | The display nickname of the account.
account.opertype | string or null | The account's oper type or null if the account is not a services operator.
account.uniqueid | uint           | The unique immutable identifier of the account.
address          | string         | The IP address the user is connecting from.
channels         | array[string]  | The channels that the user is in prefixed by their status mode prefixes.
chost            | string or null | The cloaked hostname of the user or null if they have no cloak.
fingerprint      | string or null | The fingerprint of the user's client certificate or null if they are not using one.
host             | string         | The real hostname of the user.
ident            | string         | The username (ident) of the user.
modes            | array[string]  | Flag and parameter modes which are set on the user.
nick             | string         | The nickname of the user.
nickchanged      | int            | The time at which the user last changed their nickname.
real             | string         | The real name of the user.
server           | string         | The server that the user is connected to.
signon           | int            | The time at which the user connected to the network.
tls              | bool           | Whether the user is connected using TLS (SSL).
uid              | string or null | The unique immutable identifier of the user or null if the IRCd does not use UIDs.
vhost            | string or null | The virtual host of the user or null if they have no vhost.
vident           | string or null | The virtual ident (username) of the user or null if they have no vident.

#### Example

```json
{
	"account": {
		"display": "nick1",
		"opertype": "Services Root",
		"uniqueid": "17183514657819486040"
	},
	"address":  "127.0.0.1",
	"channels": ["@#chan1", "#chan2"],
	"chost": "localhost",
	"fingerprint": null,
	"host": "localhost",
	"id": "9TSAAAAAA",
	"ident": "user1",
	"modes": ["+r"],
	"nick": "nick1",
	"nickchanged": 1740408318,
	"real": "An IRC User",
	"server": "irc.example.com",
	"signon": 1740408296,
	"tls": true,
	"vhost": "staff.example.com",
	"vident": null,
}
```

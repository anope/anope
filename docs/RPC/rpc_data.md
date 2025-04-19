# Anope `rpc_data` RPC interface

## `anope.listChannels`

Lists all channels that exist on the network.

### Parameters

Index | Description
----- | -----------
0     | If specified then the level of detail to retrieve. Can be set to "full" to retrieve all detail or "name" to just retrieve the channel names. Defaults to "name".

### Errors

Code   | Description
------ | -----------
-32099 | The specified detail level does not exist.

### Result

Returns an array of channel names.

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
created     | uint          | The UNIX time at which the channel was originally created.
listmodes   | map           | List modes which are set on the channel keyed by the mode character.
modes       | array[string] | Flag and parameter modes which are set on the channel.
name        | string        | The name of the channel.
registered  | boolean       | Whether the channel is registered.
topic       | map or null   | The channel topic or null if no topic is set.
topic.setat | uint          | The time at which the topic was set.
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
0     | If specified then the level of detail to retrieve. Can be set to "full" to retrieve all detail or "name" to just retrieve the services operator nicknames. Defaults to "name".

### Errors

Code   | Description
------ | -----------
-32099 | The specified detail level does not exist.

### Result

Returns an array of services operator names.

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
opertype            | map                   | The oper type associated with the services operator opertype.
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
0     | If specified then the level of detail to retrieve. Can be set to "full" to retrieve all detail or "name" to just retrieve the server names. Defaults to "name".

### Errors

Code   | Description
------ | -----------
-32099 | The specified detail level does not exist.

### Result

Returns an array of server names.

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
0     | If specified then the level of detail to retrieve. Can be set to "full" to retrieve all detail or "name" to just retrieve the user nicknames. Defaults to "name".

### Errors

Code   | Description
------ | -----------
-32099 | The specified detail level does not exist.

### Result

Returns an array of user nicknames.


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
nickchanged      | uint           | The time at which the user last changed their nickname.
real             | string         | The real name of the user.
server           | string         | The server that the user is connected to.
signon           | uint           | The time at which the user connected to the network.
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
	"vhost": "staff.example.com",
	"vident": null,
}
```

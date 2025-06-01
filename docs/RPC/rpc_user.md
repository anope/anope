# Anope `rpc_user` RPC interface

## `anope.checkCredentials`

Checks whether the specified credentials are valid.

### Parameters

Index | Description
----- | -----------
0     | A nickname belonging to the account to check.
1     | The password for the specified account.

### Errors

Code   | Description
------ | -----------
-32099 | The specified account does not exist.
-32098 | The specified password is not correct.
-32097 | The specified account is suspended.

### Result

Returns a map containing basic information about the account. More information about the account can be found by calling [the `anope.account` event using the value from the `uniqueid` field (requires the rpc_data module)](./rpc_user.md).

Key       | Type    | Description
---       | ----    | -----------
account   | string  | The display nickname of the account.
confirmed | boolean | Whether the account has been confirmed.
uniqueid  | uint    | The unique immutable identifier of the account.

#### Example

```json
{
	"account": "foo",
	"confirmed": true,
	"uniqueid": 11085415958920757000,
}
```

## `anope.identify`

Identifies an IRC user to the specified account.

### Parameters

Index | Description
----- | -----------
0     | Either an account identifier or nickname belonging to the account to identify to.
1     | The nickname of the IRC user to identify to the account.

### Errors

Code   | Description
------ | -----------
-32099 | The specified account does not exist.
-32098 | The specified IRC user does not exist.

### Result

This procedure returns no result.

## `anope.listCommands`

Lists all commands that exist on the network.

### Parameters

Index | Description
----- | -----------
0...n | The nicknames of the services to list commands for. If none are specified then all commands are returned.

### Errors

Code   | Description
------ | -----------
-32098 | The specified service does not exist.

### Result

Returns a map containing information about the available commands.

Key                   | Type           | Description
---                   | ----           | -----------
\*                    | map            | A key-value map of services to the commands that exist on them.
\*.\*                 | string         | A key-value map of commands to information about the commands.
\*.\*.group           | string or null | The group that the command belongs to or null if the command is not grouped.
\*.\*.hidden          | boolean        | Whether the command is visible in the help output.
\*.\*.maxparams       | uint or null   | The maximum number of parameters that the command accepts or null if there is no limit.
\*.\*.minparams       | uint           | The minimum number of parameters that the command accepts.
\*.\*.permission      | string or null | The services operator permission required to execute the command or null if no permissions are required.
\*.\*.requiresaccount | boolean        | Whether a caller must be logged into an account to execute the command.
\*.\*.requiresuser    | boolean        | Whether an IRC user is required to execute the command.

#### Example

```json
{
	"Global": {
		"GLOBAL": {
			"group": null,
			"hidden": false,
			"maxparams": 1,
			"minparams": 0,
			"permission": "global/global",
			"requiresaccount": true,
			"requiresuser": false
		},
		"HELP": {
			"group": null,
			"hidden": false,
			"maxparams": null,
			"minparams": 0,
			"permission": null,
			"requireaccount": false,
			"requireuser": false
		},
		"QUEUE": {
			"group": null,
			"hidden": false,
			"maxparams": 2,
			"minparams": 1,
			"permission": "global/queue",
			"requireaccount": true,
			"requireuser": false
		},
		"SERVER": {
			"group": null,
			"hidden": false,
			"maxparams": 2,
			"minparams": 1,
			"permission": "global/server",
			"requireaccount": true,
			"requireuser": false
		}
	}
}
```

## `anope.commands`

Executes the specified command.

### Parameters

Index | Description
----- | -----------
0     | If non-empty then the account to execute the command as.
1     | The service which the command exists on.
2...n | The the command to execute and any parameters to pass to it.

### Errors

Code   | Description
------ | -----------
-32099 | The specified account does not exist.
-32098 | The specified service does not exist.
-32097 | The specified command does not exist.

### Result

Returns an array of messages returned by the command.

#### Example

```json
[
  "Global commands:",
  "    GLOBAL    Send a message to all users",
  "    HELP      Displays this list and give information about commands",
  "    QUEUE     Manages your pending message queue.",
  "    SERVER    Send a message to all users on a server"
]
```

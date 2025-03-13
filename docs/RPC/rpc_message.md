# Anope `rpc_message` RPC interface

## `anope.messageNetwork`

Sends a message to all users on the network.

### Parameters

Index | Description
----- | -----------
0+    | One or more messages to send.

### Errors

Code   | Description
------ | -----------
-32099 | The global service is not available.

### Result

This procedure returns no result.

## `anope.messageServer`

Sends a message to all users on the specified server.

### Parameters

Index | Description
----- | -----------
0     | The name of the server to message users on.
1+    | One or more messages to send.

### Errors

Code   | Description
------ | -----------
-32099 | The global service is not available.
-32098 | The specified server does not exist.

### Result

This procedure returns no result.

## `anope.messageUser`

Sends a message to the specified user.

### Parameters

Index | Description
----- | -----------
0     | The source pseudoclient to send the message from.
1     | The target user to send the message to.
2+    | One or more messages to send.

### Errors

Code   | Description
------ | -----------
-32099 | The specified source does not exist.
-32098 | The specified target does not exist.

### Result

This procedure returns no result.


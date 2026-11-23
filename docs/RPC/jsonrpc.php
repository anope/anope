<?php
// SPDX-License-Identifier: CC0-1.0
declare(strict_types=1);

/** Implements methods for accessing an Anope JSON-RPC server. */
class AnopeRPC
{
	/** @var string The RPC host base URL. */
	public string $host;

	/** @var string The bearer token for authorizing with the RPC interface. */
	public string $token;

	/**
	 * Initializes a new AnopeRPC instance with the specified RPC host.
	 *
	 * @param string $host The RPC host base URL.
	 * @param string $token The bearer token for authorizing with the RPC interface.
	 */
	public function __construct(string $host, string $token = "")
	{
		$this->host = $host;
		$this->token = $token;
	}

	/**
	 * Executes an arbitrary RPC query.
	 *
	 * @param string $method The name of the method to execute.
	 * @param mixed ...$params The parameters to pass to the method.
	 * @return mixed The result of the RPC query.
	 * @throws RuntimeException If the HTTP request or the RPC call fails.
	 */
	public function run(string $method, mixed ...$params): mixed
	{
		$body = json_encode([
			"jsonrpc" => "2.0",
			"method" => $method,
			"params" => array_map([self::class, "stringify"], $params),
			"id" => bin2hex(random_bytes(8)),
		], JSON_THROW_ON_ERROR);

		$headers = ["Content-Type: application/json"];
		if ($this->token !== "") {
			$headers[] = "Authorization: Bearer " . base64_encode($this->token);
		}

		$ch = curl_init($this->host);
		curl_setopt_array($ch, [
			CURLOPT_POST => true,
			CURLOPT_POSTFIELDS => $body,
			CURLOPT_HTTPHEADER => $headers,
			CURLOPT_RETURNTRANSFER => true,
		]);
		$response = curl_exec($ch);
		if ($response === false) {
			$error = curl_error($ch);
			curl_close($ch);
			throw new RuntimeException("HTTP request failed: {$error}");
		}
		$status = curl_getinfo($ch, CURLINFO_RESPONSE_CODE);
		curl_close($ch);

		if ($status < 200 || $status >= 300) {
			throw new RuntimeException("HTTP returned {$status}");
		}

		$json = json_decode($response, true, 512, JSON_THROW_ON_ERROR);
		if (is_array($json) && array_key_exists("error", $json)) {
			$code = $json["error"]["code"] ?? "?";
			$message = $json["error"]["message"] ?? "";
			throw new RuntimeException("JSON-RPC returned {$code}: {$message}");
		}
		if (is_array($json) && array_key_exists("result", $json)) {
			return $json["result"];
		}
		return null;
	}

	/** Converts a parameter to a string the same way JavaScript's toString() would. */
	private static function stringify(mixed $value): string
	{
		if (is_bool($value)) {
			return $value ? "true" : "false";
		}
		if (is_array($value)) {
			return implode(",", array_map([self::class, "stringify"], $value));
		}
		return (string) $value;
	}

	/**
	 * Retrieves a list of accounts.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param string $detail The level of detail to request.
	 * @return array A list of accounts.
	 */
	public function listAccounts(string $detail = "name"): mixed
	{
		return $this->run("anope.listAccounts", $detail);
	}

	/**
	 * Retrieves information about the specified account.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param string $name The name of the account.
	 * @return array Information about the account.
	 */
	public function account(string $name): mixed
	{
		return $this->run("anope.account", $name);
	}

	/**
	 * Retrieves a list of channels.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param string $detail The level of detail to request.
	 * @return array A list of channels.
	 */
	public function listChannels(string $detail = "name"): mixed
	{
		return $this->run("anope.listChannels", $detail);
	}

	/**
	 * Retrieves information about the specified channel.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param string $name The name of the channel.
	 * @return array Information about the channel.
	 */
	public function channel(string $name): mixed
	{
		return $this->run("anope.channel", $name);
	}

	/**
	 * Retrieves a list of services operators.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param string $detail The level of detail to request.
	 * @return array A list of services operators.
	 */
	public function listOpers(string $detail = "name"): mixed
	{
		return $this->run("anope.listOpers", $detail);
	}

	/**
	 * Retrieves information about the specified services operator.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param string $name The name of the services operator.
	 * @return array Information about the services operator.
	 */
	public function oper(string $name): mixed
	{
		return $this->run("anope.oper", $name);
	}

	/**
	 * Retrieves a list of servers.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param string $detail The level of detail to request.
	 * @return array A list of servers.
	 */
	public function listServers(string $detail = "name"): mixed
	{
		return $this->run("anope.listServers", $detail);
	}

	/**
	 * Retrieves information about the specified server.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param string $name The name of the server.
	 * @return array Information about the server.
	 */
	public function server(string $name): mixed
	{
		return $this->run("anope.server", $name);
	}

	/**
	 * Retrieves a list of users.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param string $detail The level of detail to request.
	 * @return array A list of users.
	 */
	public function listUsers(string $detail = "name"): mixed
	{
		return $this->run("anope.listUsers", $detail);
	}

	/**
	 * Retrieves information about the specified user.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param string $nick The nick of the user.
	 * @return array Information about the user.
	 */
	public function user(string $nick): mixed
	{
		return $this->run("anope.user", $nick);
	}

	/**
	 * Sends a message to every user on the network.
	 *
	 * Requires the rpc_message module to be loaded.
	 *
	 * @param string ...$messages One or more messages to send.
	 */
	public function messageNetwork(string ...$messages): mixed
	{
		return $this->run("anope.messageNetwork", ...$messages);
	}

	/**
	 * Sends a message to every user on the specified server.
	 *
	 * Requires the rpc_message module to be loaded.
	 *
	 * @param string $server The name of the server.
	 * @param string ...$messages One or more messages to send.
	 */
	public function messageServer(string $server, string ...$messages): mixed
	{
		return $this->run("anope.messageServer", $server, ...$messages);
	}

	/**
	 * Sends a message to the specified user.
	 *
	 * Requires the rpc_message module to be loaded.
	 *
	 * @param string $source The source pseudoclient to send the message from.
	 * @param string $target The target user to send the message to.
	 * @param string ...$messages One or more messages to send.
	 */
	public function messageUser(string $source, string $target, string ...$messages): mixed
	{
		return $this->run("anope.messageUser", $source, $target, ...$messages);
	}

	/**
	 * Checks whether the specified credentials are valid.
	 *
	 * Requires the rpc_user module to be loaded.
	 *
	 * @param string $account A nickname belonging to the account to check.
	 * @param string $password The password for the specified account.
	 * @return array Basic information about the account.
	 */
	public function checkCredentials(string $account, string $password): mixed
	{
		return $this->run("anope.checkCredentials", $account, $password);
	}

	/**
	 * Identifies an IRC user to the specified account.
	 *
	 * Requires the rpc_user module to be loaded.
	 *
	 * @param string $account Either an account identifier or nickname belonging to the account to
	 *                        identify to.
	 * @param string $user The nickname of the IRC user to identify to the account.
	 */
	public function identify(string $account, string $user): mixed
	{
		return $this->run("anope.identify", $account, $user);
	}

	/**
	 * Lists all commands that exist on the network.
	 *
	 * Requires the rpc_user module to be loaded.
	 *
	 * @param string ...$services The nicknames of the services to list commands for.
	 * @return array Information about the available commands.
	 */
	public function listCommands(string ...$services): mixed
	{
		return $this->run("anope.listCommands", ...$services);
	}

	/**
	 * Executes a command on the network.
	 *
	 * Requires the rpc_user module to be loaded.
	 *
	 * @param string $account If non-empty then the account to execute the command as.
	 * @param string $service The service which the command exists on.
	 * @param string ...$command The command to execute and any parameters to pass to it.
	 * @return array The result of executing the command.
	 */
	public function command(string $account, string $service, string ...$command): mixed
	{
		return $this->run("anope.command", $account, $service, ...$command);
	}
}

/*
$arpc = new AnopeRPC("http://127.0.0.1:8080/jsonrpc");
try {
	$servers = $arpc->listServers("full");
	print_r($servers);
} catch (Throwable $e) {
	echo $e->getMessage(), PHP_EOL;
}
*/

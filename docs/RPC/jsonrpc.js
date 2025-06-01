// SPDX-License-Identifier: CC0-1.0
"use strict";

/** Implements methods for accessing an Anope JSON-RPC server. */
class AnopeRPC {

	/**
	 * Initializes a new AnopeRPC instance with the specified RPC host.
	 *
	 * @param {string} host The RPC host base URL.
	 * @param {string} token The bearer token for authorizing with the RPC interface.
	 */
	constructor(host, token = "") {
		this.host = host;
		this.token = token;
	}

	/**
	 * Executes an arbitrary RPC query.
	 *
	 * @param {string} method The name of the method to execute.
	 * @param {...*} params The parameters pass to the method.
	 * @returns {*} The result of the RPC query.
	 */
	async run(method, ...params) {
		const body = JSON.stringify({
			"jsonrpc": "2.0",
			"method": method,
			"params": params.map((p) => p.toString()),
			"id": Math.random().toString(36).slice(2)
		});
		const headers = new Headers();
		if (this.token) {
			headers.append("Authorization", `Bearer ${btoa(this.token)}`);
		}
		const response = await fetch(this.host, {
			method: 'POST',
			headers: headers,
			body: body
		});
		if (!response.ok) {
			throw new Error(`HTTP returned ${response.status}`)
		}
		const json = await response.json();
		if ("error" in json) {
			throw new Error(`JSON-RPC returned ${json.error.code}: ${json.error.message}`)
		}
		if ("result" in json) {
			return json.result;
		}
		return null;
	}

	/**
	 * Retrieves a list of accounts.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param {string} detail The level of detail to request.
	 * @returns {(array|object)} A list of accounts.
	 */
	listAccounts(detail = "name") {
		return this.run("anope.listAccounts", detail);
	}

	/**
	 * Retrieves information about the specified account.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param {string} name The name of the account.
	 * @returns {object} An object containing information about the account.
	 */
	account(name) {
		return this.run("anope.account", name);
	}

	/**
	 * Retrieves a list of channels.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param {string} detail The level of detail to request.
	 * @returns {(array|object)} A list of channels.
	 */
	listChannels(detail = "name") {
		return this.run("anope.listChannels", detail);
	}

	/**
	 * Retrieves information about the specified channel.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param {string} name The name of the channel.
	 * @returns {object} An object containing information about the channel.
	 */
	channel(name) {
		return this.run("anope.channel", name);
	}

	/**
	 * Retrieves a list of services operators.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param {string} detail The level of detail to request.
	 * @returns {(array|object)} A list of services operators.
	 */
	listOpers(detail = "name") {
		return this.run("anope.listOpers", detail);
	}

	/**
	 * Retrieves information about the specified services operator.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param {string} name The name of the services operator.
	 * @returns {object} An object containing information about the services operator.
	 */
	oper(name) {
		return this.run("anope.oper", name);
	}

	/**
	 * Retrieves a list of servers.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param {string} detail The level of detail to request.
	 * @returns {(array|object)} A list of servers.
	 */
	listServers(detail = "name") {
		return this.run("anope.listServers", detail);
	}

	/**
	 * Retrieves information about the specified server.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param {string} name The name of the server.
	 * @returns {object} An object containing information about the server.
	 */
	server(name) {
		return this.run("anope.server", name);
	}

	/**
	 * Retrieves a list of users.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param {string} detail The level of detail to request.
	 * @returns {(array|object)} A list of users.
	 */
	listUsers(detail = "name") {
		return this.run("anope.listUsers", detail);
	}

	/**
	 * Retrieves information about the specified user.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @param {string} nick The nick of the user.
	 * @returns {object} An object containing information about the user.
	 */
	user(nick) {
		return this.run("anope.user", nick);
	}

	/**
	 * Sends a message to every user on the network.
	 *
	 * Requires the rpc_message module to be loaded.
	 *
	 * @param {...*} messages One or more messages to send.
	 */
	messageNetwork(...messages) {
		return this.run("anope.messageNetwork", ...messages);
	}

	/**
	 * Sends a message to every user on the specified server.
	 *
	 * Requires the rpc_message module to be loaded.
	 *
	 * @param {string} name The name of the server.
	 * @param {...*} messages One or more messages to send.
	 */
	messageServer(server, ...messages) {
		return this.run("anope.messageServer", server, ...messages);
	}

	/**
	 * Sends a message to the specified user.
	 *
	 * Requires the rpc_message module to be loaded.
	 *
	 * @param {string} source The source pseudoclient to send the message from.
	 * @param {string} target The target user to send the message to.
	 * @param {...*} messages One or more messages to send.
	 */
	messageUser(source, target, ...messages) {
		return this.run("anope.messageUser", source, target, ...messages);
	}

	/**
	 * Checks whether the specified credentials are valid.
	 *
	 * Requires the rpc_user module to be loaded.
	 *
	 * @param {string} account A nickname belonging to the account to check.
	 * @param {string} password  The password for the specified account.
	 * @returns {object} An object containing basic information about the account.
	 */
	checkCredentials(account, password) {
		return this.run("anope.checkCredentials", account, password);
	}

	/**
	 * Identifies an IRC user to the specified account.
	 *
	 * Requires the rpc_user module to be loaded.
	 *
	 * @param {string} account Either an account identifier or nickname belonging to the account to
	 *                         identify to.
	 * @param {string} password The nickname of the IRC user to identify to the account.
	 */
	identify(account, user) {
		return this.run("anope.identify", account, user);
	}

	/**
	 * Lists all commands that exist on the network.
	 *
	 * Requires the rpc_user module to be loaded.
	 *
	 * @param {...*} services The nicknames of the services to list commands for.
	 * @returns {object} An object containing information about the available commands.
	 */
	listCommands(...services) {
		return this.run("anope.listCommands", ...services);
	}

	/**
	 * Lists all commands that exist on the network.
	 *
	 * Requires the rpc_user module to be loaded.
	 *
	 * @param {string} account If non-empty then the account to execute the command as.
	 * @param {string} service The service which the command exists on.
	 * @param {...*} command The the command to execute and any parameters to pass to it.
	 * @returns {object} An object containing information about the available commands.
	 */
	command(account, service, ...command) {
		return this.run("anope.command", account, service, ...command);
	}
}

/*
const arpc = new AnopeRPC("http://127.0.0.1:8080/jsonrpc");
arpc.listServers("full").then(servers => {
	console.log(servers);
}).catch (error => {
	console.log(error);
});
*/

// SPDX-License-Identifier: CC0-1.0
"use strict";

/** Implements methods for accessing an Anope JSON-RPC server. */
class AnopeRPC {

	/**
	 * Initializes a new AnopeRPC instance with the specified RPC host.
	 *
	 * @param {string} The RPC host base URL.
	 */
	constructor(host) {
		this.host = host;
	}

	/**
	 * Executes an arbitrary RPC query.
	 *
	 * @param {string} method The name of the method to execute.
	 * @param {...*} params The parameters pass to the method.
	 * @returns {*} The result of the RPC query.
	 */
	async run(method, ...params) {
		const request = JSON.stringify({
			"jsonrpc": "2.0",
			"method": method,
			"params": params,
			"id": Math.random().toString(36).slice(2)
		});
		const response = await fetch(this.host, {
			method: 'POST',
			body: request
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
	 * Retrieves a list of channels.
	 *
	 * Requires the rpc_data module to be loaded.
	 *
	 * @returns {array} An array of channel names.
	 */
	listChannels() {
		return this.run("anope.listChannels");
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
	 * @returns {array} An array of channel names.
	 */
	listOpers() {
		return this.run("anope.listOpers");
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
	 * @returns {array} An array of servers names.
	 */
	listServers() {
		return this.run("anope.listServers");
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
	 * @returns {array} An array of channel names.
	 */
	listUsers() {
		return this.run("anope.listUsers");
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
		return this.run("anope.messageServer", source, target, ...messages);
	}
}

/*
const arpc = new AnopeRPC("http://127.0.0.1:8080/jsonrpc");
arpc.listServers().then(servers => {
	console.log(servers);
}).catch (error => {
	console.log(error);
});
*/

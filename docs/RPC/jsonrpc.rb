# SPDX-License-Identifier: CC0-1.0

require "base64"
require "json"
require "net/http"

# Implements methods for accessing an Anope JSON-RPC server.
class AnopeRPC

	# The RPC host base URL.
	attr_accessor :host

	# The bearer token for authorizing with the RPC interface.
	attr_accessor :token

	# Initializes a new AnopeRPC instance with the specified RPC host.
	#
	# @param host [String] The RPC host base URL.
	# @param token [String] The bearer token for authorizing with the RPC interface.
	def initialize(host, token = nil)
		@host = URI(host)
		@token = token
	end

	# Executes an arbitrary RPC query.
	#
	# @param method [String] The name of the method to execute.
	# @param params [Object] The parameters pass to the method.
	# @return [Object] The result of the RPC query.
	private def run(method, *params)
		body = {
			jsonrpc: "2.0",
			method: method,
			params: params.map(&:to_s),
			id: rand(36**16).to_s(36)
		}.to_json

		headers = {}
		headers["Authorization"] = "Bearer #{Base64.strict_encode64(@token)}" if @token

		response = Net::HTTP.post(@host, body, headers)
		raise "HTTP returned #{response.status}" unless response.is_a?(Net::HTTPSuccess)

		json = JSON.parse(response.body, symbolize_names: true)
		raise "JSON-RPC returned #{json[:error][:code]}: #{json[:error][:message]}" if json.key?(:error)

		json.dig(:result)
	end

	# Retrieves a list of accounts.
	#
	# Requires the rpc_data module to be loaded.
	#
	# @param detail [Symbol] detail The level of detail to request.
	# @return [Array] If the detail level is set to :name then an array of account names.
	# @return [Hash] If the detail level is set to :full then a hash of information about the
	#                accounts.
	def list_accounts(detail = :name)
		self.run("anope.listAccounts", detail)
	end

	# Retrieves information about the specified account.
	#
	# Requires the rpc_data module to be loaded.
	#
	# @param name [String] The name of the account.
	# @return [Hash] A hash containing information about the account.
	def account(name)
		self.run("anope.account", name)
	end

	# Retrieves a list of channels.
	#
	# Requires the rpc_data module to be loaded.
	#
	# @param detail [Symbol] detail The level of detail to request.
	# @return [Array] If the detail level is set to :name then an array of channel names.
	# @return [Hash] If the detail level is set to :full then a hash of information about the
	#                channels.
	def list_channels(detail = :name)
		self.run("anope.listChannels", detail)
	end

	# Retrieves information about the specified channel.
	#
	# Requires the rpc_data module to be loaded.
	#
	# @param name [String] The name of the channel.
	# @return [Hash] A hash containing information about the channel.
	def channel(name)
		self.run("anope.channel", name)
	end

	# Retrieves a list of services operators.
	#
	# Requires the rpc_data module to be loaded.
	#
	# @param detail [Symbol] detail The level of detail to request.
	# @return [Array] If the detail level is set to :name then an array of services operator names.
	# @return [Hash] If the detail level is set to :full then a hash of information about the
	#                services operators.
	def list_opers(detail = :name)
		self.run("anope.listOpers", detail)
	end

	# Retrieves information about the specified services operator.
	#
	# Requires the rpc_data module to be loaded.
	#
	# @param name [String] The name of the services operator.
	# @return [Hash] A hash containing information about the services operator.
	def oper(name)
		self.run("anope.oper", name)
	end

	# Retrieves a list of servers.
	#
	# Requires the rpc_data module to be loaded.
	#
	# @param detail [Symbol] detail The level of detail to request.
	# @return [Array] If the detail level is set to :name then an array of server names.
	# @return [Hash] If the detail level is set to :full then a hash of information about the
	#                servers.
	def list_servers(detail = :name)
		self.run("anope.listServers", detail)
	end

	# Retrieves information about the specified server.
	#
	# Requires the rpc_data module to be loaded.
	#
	# @param name [String] The name of the server.
	# @return [Hash] A hash containing information about the server.
	def server(name)
		self.run("anope.server", name)
	end

	# Retrieves a list of users.
	#
	# Requires the rpc_data module to be loaded.
	#
	# @param detail [Symbol] detail The level of detail to request.
	# @return [Array] If the detail level is set to :name then an array of user nicknames.
	# @return [Hash] If the detail level is set to :full then a hash of information about the
	#                users.
	def list_users(detail = :name)
		self.run("anope.listUsers", detail)
	end

	# Retrieves information about the specified user.
	#
	# Requires the rpc_data module to be loaded.
	#
	# @param name [String] The name of the user.
	# @return [Hash] A hash containing information about the user.
	def user(name)
		self.run("anope.user", name)
	end

	# Sends a message to every user on the network.
	#
	# Requires the rpc_message module to be loaded.
	#
	# @param messages [Array<String>] One or more messages to send.
	def message_network(*messages)
		self.run("anope.messageNetwork", *messages)
	end

	# Sends a message to every user on the specified server.
	#
	# Requires the rpc_message module to be loaded.
	#
	# @param name [String] The name of the server.
	# @param messages [Array<String>] One or more messages to send.
	def message_server(server, *messages)
		self.run("anope.messageServer", server, *messages)
	end

	# Sends a message to the specified user.
	#
	# Requires the rpc_message module to be loaded.
	#
	# @param source [String] The source pseudoclient to send the message from.
	# @param target [String] The target user to send the message to.
	# @param messages [Array<String>] One or more messages to send.
	def message_user(source, target, *messages)
		self.run("anope.messageUser", source, target, *messages)
	end
end

=begin
arpc = AnopeRPC.new("http://127.0.0.1:8080/jsonrpc")
begin
	pp arpc.list_servers(:full)
rescue StandardError => err
	STDERR.puts err
end
=end

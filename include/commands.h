/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "service.h"
#include "anope.h"
#include "channels.h"
#include "language.h"
#include "modules/nickserv.h"// XXX clang

struct CommandGroup
{
	Anope::string name, description;
};

/* Used in ServiceBot::commands */
struct CommandInfo
{
	typedef Anope::map<CommandInfo> map;

	/* Service name of the command */
	Anope::string name;
	/* User visible name */
	Anope::string cname;
	/* Permission required to execute the command */
	Anope::string permission;
	/* Group this command is in */
	Anope::string group;
	/* whether or not to hide this command in help output */
	bool hide = false;
	/* Only used with fantasy */
	bool prepend_channel = false;
};

/* Where the replies from commands go to. User inheits from this and is the normal
 * source of a CommandReply
 */
struct CoreExport CommandReply
{
	virtual ~CommandReply() { }
	virtual void SendMessage(const MessageSource &, const Anope::string &msg) anope_abstract;
};

/* The source for a command */
class CoreExport CommandSource
{
	/* The nick executing the command */
	Anope::string nick;
	/* User executing the command, may be NULL */
	Reference<User> u;
	/* Command info being executed */
	CommandInfo command;
	/* whether or not this is an override as determined by the command */
	bool override = false;
 public:
	/* The account executing the command */
	Reference<NickServ::Account> nc;
	/* Where the reply should go */
	CommandReply *reply;
	/* Channel the command was executed on (fantasy) */
	Reference<Channel> c;
	/* The service this command is on */
	Reference<ServiceBot> service;

	CommandSource(const Anope::string &n, User *user, NickServ::Account *core, CommandReply *reply, ServiceBot *bi);

	const Anope::string &GetNick() const;
	User *GetUser();
	NickServ::Account *GetAccount();
	Anope::string GetSource();

	const Anope::string &GetCommand() const;
	void SetCommand(const Anope::string &);

	const Anope::string &GetPermission() const;

	const CommandInfo &GetCommandInfo() const;
	void SetCommandInfo(const CommandInfo &);

	ChanServ::AccessGroup AccessFor(ChanServ::Channel *ci);
	bool IsFounder(ChanServ::Channel *ci);

	template<typename... Args>
	void Reply(const char *message, Args&&... args)
	{
		const char *translated_message = Language::Translate(this->nc, message);
		Reply(Anope::Format(translated_message, std::forward<Args>(args)...));
	}

	void Reply(const Anope::string &message);

	bool HasCommand(const Anope::string &cmd);
	bool HasPriv(const Anope::string &cmd);
	bool IsServicesOper();
	bool IsOper();

	bool HasOverridePriv(const Anope::string &priv);
	bool HasOverrideCommand(const Anope::string &priv);
	bool IsOverride() const;
};

/** Every services command is a class, inheriting from Command.
 */
class CoreExport Command : public Service
{
	Anope::string desc;
	std::vector<Anope::string> syntax;
	/* Allow unregistered users to use this command */
	bool allow_unregistered = false;
	/* Command requires that a user is executing it */
	bool require_user = false;

 public:
	static constexpr const char *NAME = "Command";

 	/* Maximum paramaters accepted by this command */
	size_t max_params;
	/* Minimum parameters required to use this command */
	size_t min_params;

	/* Module which owns us */
	Module *module;

	Logger logger;

	/** Create a new command.
	 * @param owner The owner of the command
	 * @param sname The command name
	 * @param min_params The minimum number of parameters the parser will require to execute this command
	 * @param max_params The maximum number of parameters the parser will create, after max_params, all will be combined into the last argument.
	 * NOTE: If max_params is not set (default), there is no limit to the max number of params.
	 */
	Command(Module *owner, const Anope::string &sname, size_t min_params, size_t max_params = 0);

	void SetDesc(const Anope::string &d);

	void ClearSyntax();
	void SetSyntax(const Anope::string &s);

	void AllowUnregistered(bool b);
	void RequireUser(bool b);

	void SendSyntax(CommandSource &);
	bool AllowUnregistered() const;
	bool RequireUser() const;

 	/** Get the command description
	 * @param source The source wanting the command description
	 * @return The commands description
	 */
 	virtual const Anope::string GetDesc(CommandSource &source) const;

	/** Execute this command.
	 * @param source The source
	 * @param params Command parameters
	 */
	virtual void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_abstract;

	/** Called when HELP is requsted for the client this command is on.
	 * @param source The source
	 */
	virtual void OnServHelp(CommandSource &source);

	/** Requested when the user is requesting help on this command. Help on this command should be sent to the user.
	 * @param source The source
	 * @param subcommand The subcommand the user is requesting help on, or an empty string. (e.g. /ns help set foo bar lol gives a subcommand of "FOO BAR LOL")
	 * @return true if help was provided to the user, false otherwise.
	 */
	virtual bool OnHelp(CommandSource &source, const Anope::string &subcommand);

	/** Requested when the user provides bad syntax to this command (not enough params, etc).
	 * @param source The source
	 * @param subcommand The subcommand the user tried to use
	 */
	virtual void OnSyntaxError(CommandSource &source, const Anope::string &subcommand);

	/** Runs a command
	 * @param source The source of the command
	 * @param message The full message to run, the command is at the beginning of the message
	 */
	static void Run(CommandSource &source, const Anope::string &message);

	void Run(CommandSource &source, const Anope::string &, const CommandInfo &, std::vector<Anope::string> &params);

	/** Looks up a command name from the service name.
	 * Note that if the same command exists multiple places this will return the first one encountered
	 * @param command_service The command service to lookup, eg, nickserv/register
	 * @param bot If found, is set to the bot the command is on, eg NickServ
	 * @param name If found, is set to the comand name, eg REGISTER
	 * @return true if the given command service exists
	 */
	static bool FindCommandFromService(const Anope::string &command_service, ServiceBot* &bi, Anope::string &name);
};


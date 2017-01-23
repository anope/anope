/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2016 Anope Team <team@anope.org>
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

#include "services.h"
#include "anope.h"
#include "service.h"
#include "servers.h"
#include "users.h"
#include "messages.h"

class IRCMessage;

/* Encapsultes the IRCd protocol we are speaking. */
class CoreExport IRCDProto : public Service
{
	Anope::string proto_name;

public:
	static constexpr const char *NAME = "ircdproto";

	/* Modes used by default by our clients */
	Anope::string DefaultPseudoclientModes = "+io";
	/* Can we force change a users's nick? */
	bool CanSVSNick = false;
	/* Can we force join or part users? */
	bool CanSVSJoin = false;
	/* Can we set vhosts/vidents on users? */
	bool CanSetVHost = false, CanSetVIdent = false;
	/* Can we ban specific gecos from being used? */
	bool CanSNLine = false;
	/* Can we ban specific nicknames from being used? */
	bool CanSQLine = false;
	/* Can we ban sepcific channel names from being used? */
	bool CanSQLineChannel = false;
	/* Can we ban by IP? */
	bool CanSZLine = false;
	/* Can we place temporary holds on specific nicknames? */
	bool CanSVSHold = false;
	/* See ns_cert */
	bool CanCertFP = false;
	/* Whether this IRCd requires unique IDs for each user or server. See TS6/P10. */
	bool RequiresID = false;
	/* If this IRCd has unique ids, whether the IDs and nicknames are ambiguous */
	bool AmbiguousID = false;
	/* The maximum number of modes we are allowed to set with one MODE command */
	unsigned int MaxModes = 3;
	/* The maximum number of bytes a line may have */
	unsigned int MaxLine = 512;

	IRCDProto(Module *creator, const Anope::string &proto_name);
	virtual ~IRCDProto();

	const Anope::string &GetProtocolName() const;

	virtual void Parse(const Anope::string &, Anope::string &, Anope::string &, std::vector<Anope::string> &);
	virtual Anope::string Format(IRCMessage &);

	virtual void SendCTCPReply(const MessageSource &, const Anope::string &dest, const Anope::string &buf);
	virtual void SendNumeric(int numeric, User *dest, IRCMessage &);

	/* Retrieves the next free UID or SID */
	virtual Anope::string UID_Retrieve() { return ""; }
	virtual Anope::string SID_Retrieve() { return ""; }


	virtual void SendAction(const MessageSource &source, const Anope::string &dest, const Anope::string &message);

	/** Used to introduce ourselves to our uplink.
	 */
	virtual void Handshake() anope_abstract;

	/** Called right before we begin our burst, after we have handshaked successfully with the uplink.
	 * At this point none of our servers, users, or channels exist on the uplink
	 */
	virtual void SendBOB() { }
	virtual void SendEOB() { }

	virtual bool IsNickValid(const Anope::string &);
	virtual bool IsChannelValid(const Anope::string &);
	virtual bool IsIdentValid(const Anope::string &);
	virtual bool IsHostValid(const Anope::string &);
	virtual bool IsExtbanValid(const Anope::string &) { return false; }

	/** Retrieve the maximum number of list modes settable on this channel
	 * Defaults to Config->ListSize
	 */
	virtual unsigned int GetMaxListFor(Channel *c);

	virtual Anope::string NormalizeMask(const Anope::string &mask);

	template<typename T, typename... Args> void Send(Args&&... args)
	{
		ServiceReference<MessageSender<T>> sender(T::NAME);

		if (!sender)
		{
			const char *const name = T::NAME;
			Anope::Logger.Debug("No message sender for type {0}", name);
			return;
		}

		static_cast<T*>(static_cast<MessageSender<T>*>(sender))->Send(std::forward<Args>(args)...);
	}

	/* Templated functions which overload the normal functions to provide format strings */
	template<typename... Args> void SendKill(const MessageSource &source, User *user, const Anope::string &reason, Args&&... args)
	{
		Send<messages::Kill>(source, user, Anope::Format(reason, std::forward<Args>(args)...));
	}

	template<typename... Args> void SendMode(const MessageSource &source, Channel *chan, const Anope::string &modes, Args&&... args)
	{
		Send<messages::ModeChannel>(source, chan, Anope::Format(modes, std::forward<Args>(args)...));
	}

	template<typename... Args> void SendMode(const MessageSource &source, User *u, const Anope::string &modes, Args&&... args)
	{
		Send<messages::ModeUser>(source, u, Anope::Format(modes, std::forward<Args>(args)...));
	}

	template<typename... Args> void SendKick(const MessageSource &source, Channel *chan, User *user, const Anope::string &reason, Args&&... args)
	{
		Send<messages::Kick>(source, chan, user, Anope::Format(reason, std::forward<Args>(args)...));
	}

	template<typename... Args> void SendNotice(const MessageSource &source, const Anope::string &dest, const Anope::string &message, Args&&... args)
	{
		Send<messages::Notice>(source, dest, Anope::Format(message, std::forward<Args>(args)...));
	}

	template<typename... Args> void SendPrivmsg(const MessageSource &source, const Anope::string &dest, const Anope::string &message, Args&&... args)
	{
		Send<messages::Privmsg>(source, dest, Anope::Format(message, std::forward<Args>(args)...));
	}

	template<typename... Args> void SendAction(const MessageSource &source, const Anope::string &dest, const Anope::string &message, Args&&... args)
	{
		SendAction(source, dest, Anope::Format(message, std::forward<Args>(args)...));
	}

	template<typename... Args> void SendCTCPReply(const MessageSource &source, const Anope::string &dest, const Anope::string &message, Args&&... args)
	{
		SendCTCPReply(source, dest, Anope::Format(message, std::forward<Args>(args)...));
	}

	template<typename... Args> void SendWallops(const MessageSource &source, const Anope::string &msg, Args&&... args)
	{
		Send<messages::Wallops>(source, Anope::Format(msg, std::forward<Args>(args)...));
	}

	template<typename... Args> void SendNumeric(int numeric, User *dest, Args&&... args);

	template<typename... Args> void SendQuit(User *u, const Anope::string &reason, Args&&... args)
	{
		Send<messages::Quit>(u, Anope::Format(reason, std::forward<Args>(args)...));
	}

	template<typename... Args> void SendPart(User *u, Channel *chan, const Anope::string &reason, Args&&... args)
	{
		Send<messages::Part>(u, chan, Anope::Format(reason, std::forward<Args>(args)...));
	}
};

class CoreExport MessageSource
{
	Anope::string source;
	User *u = nullptr;
	Server *s = nullptr;

 public:
	MessageSource(const Anope::string &);
	MessageSource(User *u);
	MessageSource(Server *s);
	const Anope::string &GetName() const;
	const Anope::string &GetUID() const;
	const Anope::string &GetSource() const;
	User *GetUser() const;
	ServiceBot *GetBot() const;
	Server *GetServer() const;
};

enum IRCDMessageFlag
{
	IRCDMESSAGE_SOFT_LIMIT,
	IRCDMESSAGE_REQUIRE_SERVER,
	IRCDMESSAGE_REQUIRE_USER
};

class CoreExport IRCDMessage : public Service
{
	Anope::string name;
	unsigned int param_count;
	std::set<IRCDMessageFlag> flags;
 public:
	static constexpr const char *NAME = "ircdmessage";

	IRCDMessage(Module *owner, const Anope::string &n, unsigned int p = 0);
	unsigned int GetParamCount() const;
	virtual void Run(MessageSource &, const std::vector<Anope::string> &params) anope_abstract;

	void SetFlag(IRCDMessageFlag f) { flags.insert(f); }
	bool HasFlag(IRCDMessageFlag f) const { return flags.count(f); }
};

extern CoreExport IRCDProto *IRCD;

class IRCMessage
{
	MessageSource source;
	Anope::string command;
	std::vector<Anope::string> parameters;

 public:
	template<typename... Args>
	IRCMessage(const MessageSource &_source, const Anope::string &_command, Args&&... args)
		: source(_source)
		, command(_command)
	{
		parameters.reserve(sizeof...(args));

		Push(std::forward<Args>(args)...);
	}

	template<typename T, typename... Args>
	void Push(const T& t, Args&&... args)
	{
		parameters.push_back(stringify(t));
		
		Push(std::forward<Args>(args)...);
	}

	void Push() { }

	void TokenizeAndPush(const Anope::string &buf, char sepToken = ' ')
	{
		sepstream sep(buf, sepToken);
		Anope::string token;
		while (sep.GetToken(token))
			Push(token);
	}

	const MessageSource &GetSource() const
	{
		return source;
	}

	void SetSource(const MessageSource &_source)
	{
		this->source = _source;
	}

	const Anope::string &GetCommand() const
	{
		return command;
	}

	const std::vector<Anope::string> &GetParameters() const
	{
		return parameters;
	}
};

template<typename... Args>
void IRCDProto::SendNumeric(int numeric, User *dest, Args&&... args)
{
	Anope::string numstr = stringify(numeric);
	if (numeric < 10)
		numstr.insert(numstr.begin(), '0');
	if (numeric < 100)
		numstr.insert(numstr.begin(), '0');

	IRCMessage message(Me, numstr, dest->GetUID());
	message.Push(std::forward<Args>(args)...);
	SendNumeric(numeric, dest, message);
}

/*
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

#include "services.h"
#include "anope.h"
#include "service.h"
#include "modes.h"

/** Thrown when a protocol error happens. */
class CoreExport ProtocolException final
	: public ModuleException
{
public:
	ProtocolException(const Anope::string &message)
		: ModuleException(message)
	{
	}
};

/* Encapsulates the IRCd protocol we are speaking. */
class CoreExport IRCDProto
	: public Service
{
	Anope::string proto_name;

protected:
	IRCDProto(Module *creator, const Anope::string &proto_name);
public:
	/** Retrieves the protocol name. */
	const Anope::string &GetProtocolName() const { return proto_name; }

	virtual ~IRCDProto();

	virtual void SendNotice(const MessageSource &source, const Anope::string &dest, const Anope::string &msg, const Anope::map<Anope::string> &tags = {});
	virtual void SendPrivmsg(const MessageSource &source, const Anope::string &dest, const Anope::string &msg, const Anope::map<Anope::string> &tags = {});
	virtual void SendTagmsg(const MessageSource &source, const Anope::string &dest, const Anope::map<Anope::string> &tags);

	/** Parses an incoming message from the IRC server.
	 * @param message The message to parse.
	 * @param tags The location to store tags.
	 * @param source The location to store the source.
	 * @param command The location to store the command name.
	 * @param params The location to store the parameters.
	 * @return If the message was well formed then true; otherwise, false.
	 */
	virtual bool Parse(const Anope::string &message, Anope::map<Anope::string> &tags, Anope::string &source, Anope::string &command, std::vector<Anope::string> &params);

	/* Formats an outgoing message so it can be sent to the IRC server.
	 * @param message The location to store the formatted message.
	 * @param tags IRCv3 message tags.
	 * @param source Source of the message.
	 * @param command Command name.
	 * @param params Any extra parameters.
	 * @return If the message was formatted then true; otherwise, false.
	 */
	virtual bool Format(Anope::string &message, const Anope::map<Anope::string> &tags, const MessageSource &source, const Anope::string &command, const std::vector<Anope::string> &params);

	/* Modes used by default by our clients */
	Anope::string DefaultPseudoclientModes = "+io";

	/* Can we force change a users's nick? */
	bool CanSVSNick = false;

	/* Can we force join or part users? */
	bool CanSVSJoin = false;

	/** Can we force servers to remove opers? */
	bool CanSVSNOOP = false;

	/* Can we set vhosts on users? */
	bool CanSetVHost = false;

	/* Can we set vidents on users? */
	bool CanSetVIdent = false;

	/* Can we ban specific gecos from being used? */
	bool CanSNLine = false;

	/* Can we ban specific nicknames from being used? */
	bool CanSQLine = false;

	/* Can we ban specific channel names from being used? */
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

	/** Can we ask the server to remove list modes matching a user? */
	std::set<Anope::string> CanClearModes;

	/** Can we send tag messages? */
	bool CanTagMessage = false;

	/* The maximum length of a channel name. */
	size_t MaxChannel = 0;

	/* The maximum length of a hostname. */
	size_t MaxHost = 0;

	/* The maximum number of bytes a line may have */
	size_t MaxLine = 512;

	/* The maximum number of modes we are allowed to set with one MODE command */
	size_t MaxModes = 3;

	/* The maximum length of a nickname. */
	size_t MaxNick = 0;

	/* The maximum length of a username. */
	size_t MaxUser = 0;


	/* Retrieves the next free UID or SID */
	virtual Anope::string UID_Retrieve();
	virtual Anope::string SID_Retrieve();

	/** Extracts a timestamp from a string. */
	virtual time_t ExtractTimestamp(const Anope::string &str);

	/** Sends an error to the uplink before disconnecting.
	 * @param reason The error message.
	 */
	virtual void SendError(const Anope::string &reason);

	/** Sets the server in NOOP mode. If NOOP mode is enabled, no users
	 * will be able to oper on the server.
	 * @param s The server
	 * @param mode Whether to turn NOOP on or off
	 */
	virtual void SendSVSNOOP(const Server *s, bool mode) { }

	/** Sets the topic on a channel
	 * @param bi The bot to set the topic from
	 * @param c The channel to set the topic on. The topic being set is Channel::topic
	 */
	virtual void SendTopic(const MessageSource &, Channel *);

	/** Sets a vhost on a user.
	 * @param u The user
	 * @param vident The ident to set
	 * @param vhost The vhost to set
	 */
	virtual void SendVHost(User *u, const Anope::string &vident, const Anope::string &vhost) { }
	virtual void SendVHostDel(User *) { }

	/** Sets an akill. This is a recursive function that can be called multiple times
	 * for the same xline, but for different users, if the xline is not one that can be
	 * enforced by the IRCd, such as a nick/user/host/realname combination ban.
	 * @param u The user affected by the akill, if known
	 * @param x The akill
	 */
	virtual void SendAkill(User *, XLine *) = 0;
	virtual void SendAkillDel(const XLine *) = 0;

	/* Realname ban */
	virtual void SendSGLine(User *, const XLine *) { }
	virtual void SendSGLineDel(const XLine *) { }

	/* IP ban */
	virtual void SendSZLine(User *u, const XLine *) { }
	virtual void SendSZLineDel(const XLine *) { }

	/* Nick ban (and sometimes channel) */
	virtual void SendSQLine(User *, const XLine *x) { }
	virtual void SendSQLineDel(const XLine *x) { }

	virtual void SendKill(const MessageSource &source, const Anope::string &target, const Anope::string &reason);

	/** Kills a user
	 * @param source Who is doing the kill
	 * @param user The user to be killed
	 * @param msg Kill reason
	 */
	virtual void SendSVSKill(const MessageSource &source, User *user, const Anope::string &msg);

	virtual void SendMode(const MessageSource &source, Channel *chan, const ModeManager::Change &change);
	virtual void SendModeInternal(const MessageSource &source, Channel *chan, const Anope::string &modes, const std::vector<Anope::string> &values);

	virtual void SendMode(const MessageSource &source, User *u, const ModeManager::Change &change);
	virtual void SendModeInternal(const MessageSource &source, User *u, const Anope::string &modes, const std::vector<Anope::string> &values);

	/** Introduces a client to the rest of the network
	 * @param u The client to introduce
	 */
	virtual void SendClientIntroduction(User *u) = 0;

	virtual void SendKick(const MessageSource &source, const Channel *chan, User *user, const Anope::string &msg);

	virtual void SendGlobalNotice(BotInfo *bi, const Server *dest, const Anope::string &msg) = 0;
	virtual void SendGlobalPrivmsg(BotInfo *bi, const Server *desc, const Anope::string &msg) = 0;

	virtual void SendContextNotice(BotInfo *bi, User *target, Channel *context, const Anope::string &msg);
	virtual void SendContextPrivmsg(BotInfo *bi, User *target, Channel *context, const Anope::string &msg);

	virtual void SendQuit(User *u, const Anope::string &msg = "", const Anope::string &opermsg = "");
	virtual void SendPing(const Anope::string &servname, const Anope::string &who);
	virtual void SendPong(const Anope::string &servname, const Anope::string &who);

	/** Joins one of our users to a channel.
	 * @param u The user to join
	 * @param c The channel to join the user to
	 * @param status The status to set on the user after joining. This may or may not already internally
	 * be set on the user. This may include the modes in the join, but will usually place them on the mode
	 * stacker to be set "soon".
	 */
	virtual void SendJoin(User *u, Channel *c, const ChannelStatus *status) = 0;
	virtual void SendPart(User *u, const Channel *chan, const Anope::string &msg);

	/** Force joins a user that isn't ours to a channel.
	 * @param bi The source of the message
	 * @param u The user to join
	 * @param chan The channel to join the user to
	 * @param key Channel key
	 */
	virtual void SendSVSJoin(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &key) { }

	/** Force parts a user that isn't ours from a channel.
	 * @param source The source of the message
	 * @param u The user to part
	 * @param chan The channel to part the user from
	 * @param param part reason, some IRCds don't support this
	 */
	virtual void SendSVSPart(const MessageSource &source, User *u, const Anope::string &chan, const Anope::string &param) { }

	virtual void SendInvite(const MessageSource &source, const Channel *c, User *u);
	virtual void SendGlobops(const MessageSource &source, const Anope::string &msg);

	/** Sends a nick change of one of our clients.
	 */
	virtual void SendNickChange(User *u, const Anope::string &newnick);

	/** Forces a nick change of a user that isn't ours (SVSNICK)
	 */
	virtual void SendForceNickChange(User *u, const Anope::string &newnick, time_t when);

	/** Used to introduce ourselves to our uplink. Usually will SendServer(Me) and any other
	 * initial handshake requirements.
	 */
	virtual void SendConnect() = 0;

	/** Called right before we begin our burst, after we have handshaked successfully with the uplink.
	 * At this point none of our servers, users, or channels exist on the uplink
	 */
	virtual void SendBOB() { }
	virtual void SendEOB() { }

	virtual void SendSVSHold(const Anope::string &, time_t) { }
	virtual void SendSVSHoldDel(const Anope::string &) { }

	virtual void SendSWhois(const MessageSource &, const Anope::string &, const Anope::string &) { }

	/** Introduces a server to the uplink
	 */
	virtual void SendServer(const Server *) = 0;
	virtual void SendSquit(Server *, const Anope::string &message);

	virtual void SendNumericInternal(int numeric, const Anope::string &dest, const std::vector<Anope::string> &params);
	template <typename... Args>
	void SendNumeric(int numeric, const Anope::string &dest, Args &&...args)
	{
		SendNumericInternal(numeric, dest, { Anope::ToString(args)... });
	}

	virtual void SendLogin(User *u, NickAlias *na) = 0;
	virtual void SendLogout(User *u) = 0;

	/** Send a channel creation message to the uplink.
	 * On most TS6 IRCds this is a SJOIN with no nick
	 */
	virtual void SendChannel(Channel *c) = 0;

	/** Make the user an IRC operator
	 * Normally this is a simple +o, though some IRCds require us to send the oper type
	 */
	virtual void SendOper(User *u);

	virtual void SendClearModes(const MessageSource &user, Channel *c, User* u, const Anope::string &mode) { }

	virtual bool IsNickValid(const Anope::string &);
	virtual bool IsChannelValid(const Anope::string &);
	virtual bool IsIdentValid(const Anope::string &);
	virtual bool IsHostValid(const Anope::string &);
	virtual bool IsExtbanValid(const Anope::string &) { return false; }
	virtual bool IsTagValid(const Anope::string &, const Anope::string &) { return false; }

	/** Retrieve the maximum number of list modes settable on this channel
	 * Defaults to Config->ListSize
	 */
	virtual size_t GetMaxListFor(Channel *c, ChannelMode *cm);

	virtual Anope::string NormalizeMask(const Anope::string &mask);

};

class CoreExport MessageSource final
{
	Anope::string source;
	User *u = nullptr;
	Server *s = nullptr;

public:
	explicit MessageSource(const Anope::string &);
	MessageSource(User *u);
	MessageSource(Server *s);
	const Anope::string &GetName() const;
	const Anope::string &GetSource() const;
	User *GetUser() const;
	BotInfo *GetBot() const;
	Server *GetServer() const;
};


/** Base class for protocol module message handlers. */
class CoreExport IRCDMessage
	: public Service
{
public:
	/** An enumeration of potential flags a command can have. */
	enum Flag
		: uint8_t
	{
		/** The parameter count is a minimum instead of an exact limit. */
		FLAG_SOFT_LIMIT,

		/** The message must come from a server. */
		FLAG_REQUIRE_SERVER,

		/** The message must come from a user. */
		FLAG_REQUIRE_USER,

		/** The highest flag possible. */
		FLAG_MAX,
	};

protected:
	/** The name of the message (e.g. PRIVMSG). */
	const Anope::string name;

	/** The number of parameters this command takes. */
	const size_t param_count;

	/** The flags that are set on the command. */
	std::bitset<FLAG_MAX> flags;

public:
	IRCDMessage(Module *o, const Anope::string &n, size_t pc = 0);

	/** Retrieves the parameter count. */
	inline size_t GetParamCount() const { return param_count; }

	/** Runs the handler for this message.
	 * @param source Entity that sent the message.
	 * @param params Message parameters
	 * @param tags Message tags
	 */
	virtual void Run(MessageSource &source, const std::vector<Anope::string> &params, const Anope::map<Anope::string> &tags) = 0;

	/** Sets the flags for this message. */
	inline void SetFlag(Flag flag, bool value = true) { flags.set(flag, value); }

	/** Determines if a flag is set. */
	inline bool HasFlag(Flag flag) const { return flags[flag]; }
};

/** MessageTokenizer allows tokens in the IRC wire format to be read from a string */
class CoreExport MessageTokenizer final
{
private:
	/** The message we are parsing tokens from. */
	Anope::string message;

	/** The current position within the message. */
	Anope::string::size_type position = 0;

public:
	/** Create a tokenstream and fill it with the provided data. */
	MessageTokenizer(const Anope::string &msg);

	/** Retrieve the next \<middle> token in the message.
	 * @param token The next token available, or an empty string if none remain.
	 * @return True if a token was retrieved; otherwise, false.
	 */
	bool GetMiddle(Anope::string &token);

	/** Retrieve the next \<trailing> token in the message.
	 * @param token The next token available, or an empty string if none remain.
	 * @return True if a token was retrieved; otherwise, false.
	 */
	bool GetTrailing(Anope::string &token);
};

extern CoreExport IRCDProto *IRCD;

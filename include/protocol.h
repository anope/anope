/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "services.h"
#include "anope.h"
#include "service.h"

/* Encapsultes the IRCd protocol we are speaking. */
class CoreExport IRCDProto
{
	Anope::string proto_name;

 protected:
 	IRCDProto(const Anope::string &proto_name);
 public:
	virtual ~IRCDProto();

	virtual void SendSVSKillInternal(const BotInfo *, User *, const Anope::string &);
	virtual void SendModeInternal(const BotInfo *, const Channel *, const Anope::string &);
	virtual void SendModeInternal(const BotInfo *, const User *, const Anope::string &) = 0;
	virtual void SendKickInternal(const BotInfo *, const Channel *, const User *, const Anope::string &);
	virtual void SendMessageInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &buf);
	virtual void SendNoticeInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &msg);
	virtual void SendPrivmsgInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &buf);
	virtual void SendQuitInternal(const User *u, const Anope::string &buf);
	virtual void SendPartInternal(const BotInfo *bi, const Channel *chan, const Anope::string &buf);
	virtual void SendGlobopsInternal(const BotInfo *source, const Anope::string &buf);
	virtual void SendCTCPInternal(const BotInfo *bi, const Anope::string &dest, const Anope::string &buf);
	virtual void SendNumericInternal(int numeric, const Anope::string &dest, const Anope::string &buf);

	const Anope::string &GetProtocolName();
	/* Modes used by default by our clients */
	Anope::string DefaultPseudoclientModes;
	/* Can we force change a users's nick */
	bool CanSVSNick;
	/* Can we set vhosts/vidents on users? */
	bool CanSetVHost, CanSetVIdent;
	/* Can we ban specific gecos from being used? */
	bool CanSNLine;
	/* Can we ban specific nicknames from being used? */
	bool CanSQLine;
	/* Can we ban sepcific channel names from being used? */
	bool CanSQLineChannel;
	/* Can we ban by IP? */
	bool CanSZLine;
	/* Can we place temporary holds on specific nicknames? */
	bool CanSVSHold;
	/* See os_oline */
	bool CanSVSO;
	/* See ns_cert */
	bool CanCertFP;
	/* Whether this IRCd requires unique IDs for each user or server. See TS6/P10. */
	bool RequiresID;
	/* The maximum number of modes we are allowed to set with one MODE command */
	unsigned MaxModes;

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
	virtual void SendTopic(BotInfo *, Channel *);

	/** Sets a vhost on a user.
	 * @param u The user
	 * @param vident The ident to set
	 * @param vhost The vhost to set
	 */
	virtual void SendVhost(User *u, const Anope::string &vident, const Anope::string &vhost) { }
	virtual void SendVhostDel(User *) { }

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

	/** Kills a user
	 * @param source The client used to kill the user, if any
	 * @param user The user to be killed
	 * @param fmt Kill reason
	 */
	virtual void SendSVSKill(const BotInfo *source, User *user, const char *fmt, ...);

	virtual void SendMode(const BotInfo *bi, const Channel *dest, const char *fmt, ...);
	virtual void SendMode(const BotInfo *bi, const User *u, const char *fmt, ...);

	/** Introduces a client to the rest of the network
	 * @param u The client to introduce
	 */
	virtual void SendClientIntroduction(const User *u) = 0;

	virtual void SendKick(const BotInfo *bi, const Channel *chan, const User *user, const char *fmt, ...);

	/* Sends a message using SendPrivmsg or SendNotice, depending on the default message method. */
	virtual void SendMessage(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendNotice(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendPrivmsg(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendAction(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendCTCP(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);

	virtual void SendGlobalNotice(const BotInfo *bi, const Server *dest, const Anope::string &msg) = 0;
	virtual void SendGlobalPrivmsg(const BotInfo *bi, const Server *desc, const Anope::string &msg) = 0;

	virtual void SendQuit(const User *u, const char *fmt, ...);
	virtual void SendPing(const Anope::string &servname, const Anope::string &who);
	virtual void SendPong(const Anope::string &servname, const Anope::string &who);

	/** Joins one of our users to a channel.
	 * @param u The user to join
	 * @param c The channel to join the user to
	 * @param status The status to set on the user after joining. This may or may not already internally
	 * be set on the user. This may include the modes in the join, but will usually place them on the mode
	 * stacker to be set "soon".
	 */
	virtual void SendJoin(const User *u, Channel *c, const ChannelStatus *status) = 0;
	virtual void SendPart(const BotInfo *bi, const Channel *chan, const char *fmt, ...);

	/** Force joins a user that isn't ours to a channel.
	 * @param bi The source of the message
	 * @param nick The user to join
	 * @param chan The channel to join the user to
	 * @param param Channel key?
	 */
	virtual void SendSVSJoin(const BotInfo *bi, const Anope::string &nick, const Anope::string &chan, const Anope::string &param) { }

	virtual void SendInvite(const BotInfo *bi, const Channel *c, const User *u);
	virtual void SendGlobops(const BotInfo *source, const char *fmt, ...);

	/** Sets oper flags on a user, currently only supported by Unreal
	 */
	virtual void SendSVSO(const BotInfo *, const Anope::string &, const Anope::string &) { }

	/** Sends a nick change of one of our clients.
	 */
	virtual void SendNickChange(const User *u, const Anope::string &newnick);

	/** Forces a nick change of a user that isn't ours (SVSNICK)
	 */
	virtual void SendForceNickChange(const User *u, const Anope::string &newnick, time_t when);

	/** Used to introduce ourselves to our uplink. Usually will SendServer(Me) and any other
	 * initial handshake requirements.
	 */
	virtual void SendConnect() = 0;
	
	/** Called right before we begin our burst, after we have handshaked successfully with the uplink/
	 * At this point none of our servesr, users, or channels exist on the uplink
	 */
	virtual void SendBOB() { }
	virtual void SendEOB() { }

	virtual void SendSVSHold(const Anope::string &) { }
	virtual void SendSVSHoldDel(const Anope::string &) { }

	virtual void SendSWhois(const BotInfo *bi, const Anope::string &, const Anope::string &) { }

	/** Introduces a server to the uplink
	 */
	virtual void SendServer(const Server *) = 0;
	virtual void SendSquit(Server *, const Anope::string &message);

	virtual void SendNumeric(int numeric, const Anope::string &dest, const char *fmt, ...);

	virtual void SendLogin(User *u) = 0;
	virtual void SendLogout(User *u) = 0;

	/** Send a channel creation message to the uplink.
	 * On most TS6 IRCds this is a SJOIN with no nick
	 */
	virtual void SendChannel(Channel *c) { }

	/** Make the user an IRC operator
	 * Normally this is a simple +o, though some IRCds require us to send the oper type
	 */
	virtual void SendOper(User *u);

	virtual bool IsNickValid(const Anope::string &);
	virtual bool IsChannelValid(const Anope::string &);
	virtual bool IsIdentValid(const Anope::string &);
	virtual bool IsHostValid(const Anope::string &);
};

enum IRCDMessageFlag
{
	IRCDMESSAGE_SOFT_LIMIT,
	IRCDMESSAGE_REQUIRE_SERVER,
	IRCDMESSAGE_REQUIRE_USER
};

class CoreExport MessageSource
{
	Anope::string source;
	User *u;
	Server *s;

 public:
	MessageSource(const Anope::string &);
	MessageSource(User *u);
	MessageSource(Server *s);
	const Anope::string GetName();
	const Anope::string &GetSource();
	User *GetUser();
	Server *GetServer();
};

class CoreExport IRCDMessage : public Flags<IRCDMessageFlag>, public Service
{
	Anope::string name;
	unsigned param_count;
 public:
	IRCDMessage(Module *owner, const Anope::string &n, unsigned p = 0);
	unsigned GetParamCount() const;
	virtual void Run(MessageSource &, const std::vector<Anope::string> &params) = 0;
};

extern CoreExport IRCDProto *IRCD;

#endif // PROTOCOL_H

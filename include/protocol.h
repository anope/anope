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
 *
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "services.h"
#include "anope.h"

class CoreExport IRCDProto
{
	Anope::string proto_name;

	IRCDProto() { }
 protected:
 	IRCDProto(const Anope::string &proto_name);

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
 public:
	virtual ~IRCDProto();

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

	virtual void SendSVSNOOP(const Server *, bool) { }
	virtual void SendTopic(BotInfo *, Channel *, const Anope::string &, const Anope::string &, time_t &);
	virtual void SendVhostDel(User *) { }
	virtual void SendAkill(User *, XLine *) = 0;
	virtual void SendAkillDel(const XLine *) = 0;
	virtual void SendSVSKill(const BotInfo *source, User *user, const char *fmt, ...);
	virtual void SendMode(const BotInfo *bi, const Channel *dest, const char *fmt, ...);
	virtual void SendMode(const BotInfo *bi, const User *u, const char *fmt, ...);
	virtual void SendClientIntroduction(const User *u) = 0;
	virtual void SendKick(const BotInfo *bi, const Channel *chan, const User *user, const char *fmt, ...);
	virtual void SendMessage(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendNotice(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendAction(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendPrivmsg(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendGlobalNotice(const BotInfo *bi, const Server *dest, const Anope::string &msg) = 0;
	virtual void SendGlobalPrivmsg(const BotInfo *bi, const Server *desc, const Anope::string &msg) = 0;

	virtual void SendQuit(const User *u, const char *fmt, ...);
	virtual void SendPing(const Anope::string &servname, const Anope::string &who);
	virtual void SendPong(const Anope::string &servname, const Anope::string &who);
	virtual void SendJoin(const User *, Channel *, const ChannelStatus *) = 0;
	virtual void SendSQLineDel(const XLine *x) { }
	virtual void SendInvite(const BotInfo *bi, const Channel *c, const User *u);
	virtual void SendPart(const BotInfo *bi, const Channel *chan, const char *fmt, ...);
	virtual void SendGlobops(const BotInfo *source, const char *fmt, ...);
	virtual void SendSQLine(User *, const XLine *x) { }
	virtual void SendSquit(Server *, const Anope::string &message);
	virtual void SendSVSO(const BotInfo *, const Anope::string &, const Anope::string &) { }
	virtual void SendChangeBotNick(const BotInfo *bi, const Anope::string &newnick);
	virtual void SendForceNickChange(const User *u, const Anope::string &newnick, time_t when);
	virtual void SendVhost(User *, const Anope::string &, const Anope::string &) { }
	virtual void SendConnect() = 0;
	virtual void SendSVSHold(const Anope::string &) { }
	virtual void SendSVSHoldDel(const Anope::string &) { }
	virtual void SendSGLineDel(const XLine *) { }
	virtual void SendSZLineDel(const XLine *) { }
	virtual void SendSZLine(User *u, const XLine *) { }
	virtual void SendSGLine(User *, const XLine *) { }
	virtual void SendCTCP(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendSVSJoin(const BotInfo *bi, const Anope::string &, const Anope::string &, const Anope::string &) { }
	virtual void SendSWhois(const BotInfo *bi, const Anope::string &, const Anope::string &) { }
	virtual void SendBOB() { }
	virtual void SendEOB() { }
	virtual void SendServer(const Server *) = 0;
	virtual bool IsNickValid(const Anope::string &) { return true; }
	virtual bool IsChannelValid(const Anope::string &);
	virtual void SendNumeric(int numeric, const Anope::string &dest, const char *fmt, ...);
	virtual void SendLogin(User *u) = 0;
	virtual void SendLogout(User *u) = 0;

	/** Send a channel creation message to the uplink.
	 * On most TS6 IRCds this is a SJOIN with no nick
	 */
	virtual void SendChannel(Channel *c) { }
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

class CoreExport IRCDMessage : public Flags<IRCDMessageFlag, 3>
{
	static std::map<Anope::string, std::vector<IRCDMessage *> > messages;

	Anope::string name;
	unsigned param_count;
 public:
 	static const std::vector<IRCDMessage *> *Find(const Anope::string &name);

	IRCDMessage(const Anope::string &n, unsigned p = 0);
	~IRCDMessage();
	unsigned GetParamCount() const;
	virtual bool Run(MessageSource &, const std::vector<Anope::string> &params) = 0;
};

extern CoreExport IRCDProto *ircdproto;

#endif // PROTOCOL_H

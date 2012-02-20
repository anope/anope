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

/* Protocol tweaks */

struct IRCDVar
{
	const char *name;				/* Name of the IRCd command */
	const char *pseudoclient_mode;	/* Mode used by BotServ Bots */
	int svsnick;					/* Supports SVSNICK */
	int vhost;						/* Supports vhost */
	int snline;						/* Supports SNline */
	int sqline;						/* Supports SQline */
	int szline;						/* Supports SZline */
	int join2msg;					/* Join 2 Message */
	int chansqline;					/* Supports Channel Sqlines */
	int quitonkill;					/* IRCD sends QUIT when kill */
	int vident;						/* Supports vidents */
	int svshold;					/* Supports svshold */
	int tsonmode;					/* Timestamp on mode changes */
	int omode;						/* On the fly o:lines */
	int umode;						/* change user modes */
	int knock_needs_i;				/* Check if we needed +i when setting NOKNOCK */
	int svsmode_ucmode;				/* Can remove User Channel Modes with SVSMODE */
	int sglineenforce;
	int ts6;						/* ircd is TS6 */
	const char *globaltldprefix;	/* TLD prefix used for Global */
	unsigned maxmodes;				/* Max modes to send per line */
	int certfp;					/* IRCd sends a SSL users certificate fingerprint */
};


class CoreExport IRCDProto
{
 protected:
	virtual void SendSVSKillInternal(const BotInfo *, const User *, const Anope::string &);
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
	virtual ~IRCDProto() { }

	virtual void SendSVSNOOP(const Server *, bool) { }
	virtual void SendTopic(BotInfo *, Channel *);
	virtual void SendVhostDel(User *) { }
	virtual void SendAkill(User *, const XLine *) = 0;
	virtual void SendAkillDel(const XLine *) = 0;
	virtual void SendSVSKill(const BotInfo *source, const User *user, const char *fmt, ...);
	virtual void SendMode(const BotInfo *bi, const Channel *dest, const char *fmt, ...);
	virtual void SendMode(const BotInfo *bi, const User *u, const char *fmt, ...);
	virtual void SendClientIntroduction(const User *u) = 0;
	virtual void SendKick(const BotInfo *bi, const Channel *chan, const User *user, const char *fmt, ...);
	virtual void SendMessage(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendNotice(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendAction(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendPrivmsg(const BotInfo *bi, const Anope::string &dest, const char *fmt, ...);
	virtual void SendGlobalNotice(const BotInfo *bi, const Server *dest, const Anope::string &msg);
	virtual void SendGlobalPrivmsg(const BotInfo *bi, const Server *desc, const Anope::string &msg);

	virtual void SendQuit(const User *u, const char *fmt, ...);
	virtual void SendPing(const Anope::string &servname, const Anope::string &who);
	virtual void SendPong(const Anope::string &servname, const Anope::string &who);
	virtual void SendJoin(User *, Channel *, const ChannelStatus *) = 0;
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

class CoreExport IRCdMessage
{
 public:
	virtual bool On436(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnAway(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnJoin(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnKick(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnKill(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnMode(const Anope::string &, const std::vector<Anope::string> &) = 0;
	virtual bool OnUID(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnNick(const Anope::string &, const std::vector<Anope::string> &) = 0;
	virtual bool OnPart(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnPing(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnPrivmsg(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnQuit(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnServer(const Anope::string &, const std::vector<Anope::string> &) = 0;
	virtual bool OnSQuit(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnTopic(const Anope::string &, const std::vector<Anope::string> &) = 0;
	virtual bool OnWhois(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnCapab(const Anope::string &, const std::vector<Anope::string> &);
	virtual bool OnSJoin(const Anope::string &, const std::vector<Anope::string> &) = 0;
	virtual bool OnError(const Anope::string &, const std::vector<Anope::string> &);
};

extern IRCDVar *ircd;
extern IRCDProto *ircdproto;
extern IRCdMessage *ircdmessage;

extern void pmodule_ircd_proto(IRCDProto *);
extern void pmodule_ircd_var(IRCDVar *ircdvar);
extern void pmodule_ircd_message(IRCdMessage *message);

#endif // PROTOCOL_H

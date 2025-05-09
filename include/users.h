/*
 *
 * (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * (C) 2003-2025 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#pragma once

#include "anope.h"
#include "modes.h"
#include "extensible.h"
#include "serialize.h"
#include "commands.h"
#include "account.h"
#include "sockets.h"

typedef Anope::unordered_map<User *> user_map;

extern CoreExport user_map UserListByNick, UserListByUID;

extern CoreExport int OperCount;
extern CoreExport unsigned MaxUserCount;
extern CoreExport time_t MaxUserTime;

/* Online user and channel data. */
class CoreExport User
	: public virtual Base
	, public Extensible
	, public CommandReply
{
	/* true if the user was quit or killed */
	bool quit;
	/* Users that are in the process of quitting */
	static std::list<User *> quitting_users;

public:
	typedef std::map<Anope::string, ModeData> ModeList;
protected:
	Anope::string vident;
	Anope::string ident;
	Anope::string uid;
	/* Map of user modes and the params this user has (if any) */
	ModeList modes;
	/* NickCore account the user is currently logged in as, if they are logged in */
	Serialize::Reference<NickCore> nc;

	/* # of invalid password attempts */
	unsigned invalid_pw_count;
	/* Time of last invalid password */
	time_t invalid_pw_time;


public: // XXX: exposing a tiny bit too much
	/* User's current nick */
	Anope::string nick;

	/* User's real hostname */
	Anope::string host;
	/* User's virtual hostname */
	Anope::string vhost;
	/* User's cloaked hostname */
	Anope::string chost;
	/* Realname */
	Anope::string realname;
	/* SSL Fingerprint */
	Anope::string fingerprint;
	/* User's IP */
	sockaddrs ip;
	/* Server user is connected to */
	Server *server;
	/* When the user signed on. Set on connect and never modified. */
	time_t signon;
	/* Timestamp of the nick. Updated when the nick changes. */
	time_t timestamp;
	/* Is the user as super admin? */
	bool super_admin;
	/* The away message of the user */
	Anope::string awaymsg;
	/* The time the user went away */
	time_t awaytime = 0;

	/* Channels the user is in */
	typedef std::map<Channel *, ChanUserContainer *> ChanUserList;
	ChanUserList chans;

	/* Last time this user sent a memo command used */
	time_t lastmemosend;
	/* Last time this user registered */
	time_t lastnickreg;
	/* Last time this user sent an email */
	time_t lastmail;

protected:
	/** Create a new user object, initialising necessary fields and
	 * adds it to the hash
	 *
	 * @param snick The nickname of the user.
	 * @param sident The username of the user
	 * @param shost The hostname of the user
	 * @param svhost The vhost of the user
	 * @param sip The ip of the user
	 * @param sserver The server of the user
	 * @param srealname The realname/gecos of the user
	 * @param ts User's timestamp
	 * @param smodes User's modes
	 * @param suid The unique identifier of the user.
	 * @param nc The account the user is identified as, if any
	 */
	User(const Anope::string &snick, const Anope::string &sident, const Anope::string &shost, const Anope::string &svhost, const Anope::string &sip, Server *sserver, const Anope::string &srealname, time_t ts, const Anope::string &smodes, const std::vector<Anope::string> &smodeparams, const Anope::string &suid, NickCore *nc);

	/** Destroy a user.
	 */
	virtual ~User();

public:
	static User *OnIntroduce(const Anope::string &snick, const Anope::string &sident, const Anope::string &shost, const Anope::string &svhost, const Anope::string &sip, Server *sserver, const Anope::string &srealname, time_t ts, const Anope::string &smodes, const Anope::string &suid, NickCore *nc, const std::vector<Anope::string> &smodeparams = {});

	/** Update the nickname of a user record accordingly, should be
	 * called from ircd protocol.
	 * @param newnick The new username
	 * @param ts The time the nick was changed, User::timestamp will be updated to this.
	 */
	void ChangeNick(const Anope::string &newnick, time_t ts = Anope::CurTime);

	/** Update the displayed (vhost) of a user record.
	 * This is used (if set) instead of real host.
	 * @param host The new displayed host to give the user.
	 */
	void SetDisplayedHost(const Anope::string &host);

	/** Get the displayed vhost of a user record.
	 * @return The displayed vhost of the user, where ircd-supported, or the user's real host.
	 */
	const Anope::string &GetDisplayedHost() const;

	/** Update the cloaked host of a user
	 * @param host The cloaked host
	 */
	void SetCloakedHost(const Anope::string &newhost);

	/** Get the cloaked host of a user
	 * @return The cloaked host
	 */
	const Anope::string &GetCloakedHost() const;

	/** Retrieves the UID of the user, if set, else the nick.
	 * @return The UID of the user.
	 */
	const Anope::string &GetUID() const;

	/** Update the displayed ident (username) of a user record.
	 * @param ident The new ident to give this user.
	 */
	void SetVIdent(const Anope::string &ident);

	/** Get the displayed ident (username) of this user.
	 * @return The displayed ident of this user.
	 */
	const Anope::string &GetVIdent() const;

	/** Update the real ident (username) of a user record.
	 * @param ident The new ident to give this user.
	 * NOTE: Where possible, you should use the Get/SetVIdent() equivalents.
	 */
	void SetIdent(const Anope::string &ident);

	/** Get the real ident (username) of this user.
	 * @return The displayed ident of this user.
	 * NOTE: Where possible, you should use the Get/SetVIdent() equivalents.
	 */
	const Anope::string &GetIdent() const;

	/** Get the full mask (nick!ident@realhost) of a user
	 */
	Anope::string GetMask() const;

	/** Get the full display mask (nick!vident@vhost/chost)
	 */
	Anope::string GetDisplayedMask() const;

	/** Updates the realname of the user record.
	 */
	void SetRealname(const Anope::string &realname);

	/**
	 * Send a message (notice or privmsg, depending on settings) to a user
	 * @param source Sender
	 * @param fmt Format of the Message
	 * @param ... any number of parameters
	 */
	void SendMessage(BotInfo *source, const char *fmt, ...) ATTR_FORMAT(3, 4);
	void SendMessage(BotInfo *source, int count, const char *singular, const char *plural, ...) ATTR_FORMAT(5, 6);
	void SendMessage(BotInfo *source, const Anope::string &msg) override;
	void SendMessage(CommandSource &source, const Anope::string &msg) override;

	/** Identify the user to a nick.
	 * updates last_seen, logs the user in,
	 * send messages, checks for mails, set vhost and more
	 * @param na the nick to identify to, should be the same as
	 * the user's current nick
	 */
	void Identify(NickAlias *na);

	/** Login the user to an account
	 * @param core The account
	 */
	void Login(NickCore *core);

	/** Logout the user
	 */
	void Logout();

	/** Get the account the user is logged in using
	 * @return The account or NULL
	 */
	NickCore *Account() const;

	/** Get the account nick the user is logged in using
	 * @return The account nick or NULL
	 */
	NickAlias *AccountNick() const;

	/** Check if the user is identified for their nick
	 * @param check_nick True to check if the user is identified to the nickname they are on too
	 * @return true or false
	 */
	bool IsIdentified(bool check_nick = false) const;

	/** Check if the user is connected securely.
	 * @return True if the user is connected securely; otherwise, false.
	 */
	bool IsSecurelyConnected() const;

	/** Check if the user is a services oper
	 * @return true if they are an oper
	 */
	bool IsServicesOper();

	/** Check whether this user has access to run the given command string.
	  * @param cmdstr The string to check, e.g. botserv/set/private.
	  * @return True if this user may run the specified command, false otherwise.
	  */
	bool HasCommand(const Anope::string &cmdstr);

	/** Check whether this user has access to the given special permission.
	  * @param privstr The priv to check for, e.g. users/auspex.
	  * @return True if this user has the specified priv, false otherwise.
	  */
	bool HasPriv(const Anope::string &privstr);

	/** Update the last usermask stored for a user. */
	void UpdateHost();

	/** Update the away state for a user. */
	void SetAway(const Anope::string &msg = "", time_t ts = 0);

	/** Determines whether this user is away. */
	auto IsAway() const { return awaymsg.empty(); }

	/** Check if the user has a mode
	 * @param name Mode name
	 * @return true or false
	 */
	bool HasMode(const Anope::string &name) const;

	/** Set a mode internally on the user, the IRCd is not informed
	 * @param setter who/what is setting the mode
	 * @param um The user mode
	 * @param data Data about the mode.
	 */
	void SetModeInternal(const MessageSource &setter, UserMode *um, const  ModeData &data = {});

	/** Remove a mode internally on the user, the IRCd is not informed
	 * @param setter who/what is setting the mode
	 * @param um The user mode
	 */
	void RemoveModeInternal(const MessageSource &setter, UserMode *um);

	/** Set a mode on the user
	 * @param bi The client setting the mode
	 * @param um The user mode
	 * @param data Data about the mode
	 */
	void SetMode(BotInfo *bi, UserMode *um, const ModeData &data = {});

	/** Set a mode on the user
	 * @param bi The client setting the mode
	 * @param name The mode name
	 * @param data Data about the mode
	 */
	void SetMode(BotInfo *bi, const Anope::string &name, const ModeData &data = {});

	/** Remove a mode on the user
	 * @param bi The client setting the mode
	 * @param um The user mode
	 * @param param Optional param for the mode
	 */
	void RemoveMode(BotInfo *bi, UserMode *um, const Anope::string &param = "");

	/** Remove a mode from the user
	 * @param bi The client setting the mode
	 * @param name The mode name
	 * @param param Optional param for the mode
	 */
	void RemoveMode(BotInfo *bi, const Anope::string &name, const Anope::string &param = "");

	/** Set a string of modes on a user
	 * @param bi The client setting the modes
	 * @param umodes The modes
	 */
	void SetModes(BotInfo *bi, const char *umodes, ...) ATTR_FORMAT(3, 4);
	void SetModes(BotInfo *bi, const Anope::string &umodes);

	/** Set a string of modes on a user internally
	 * @param setter who/what is setting the mode
	 * @param umodes The mode letters
	 * @param umodeparams The mode values
	 */
	void SetModesInternal(const MessageSource &source, const Anope::string &umodes, const std::vector<Anope::string> &umodeparams = {});

	/** Get modes set for this user.
	 * @return A string of modes set on the user
	 */
	Anope::string GetModes() const;

	const ModeList &GetModeList() const;

	/** Find the channel container for Channel c that the user is on
	 * This is preferred over using FindUser in Channel, as there are usually more users in a channel
	 * than channels a user is in
	 * @param c The channel
	 * @return The channel container, or NULL
	 */
	ChanUserContainer *FindChannel(Channel *c) const;

	/** Check if the user is protected from kicks and negative mode changes
	 * @return true or false
	 */
	bool IsProtected();

	/** Kill a user
	 * @param source The user/server doing the kill
	 * @param reason The reason for the kill
	 */
	void Kill(const MessageSource &source, const Anope::string &reason);

	/** Process a kill for a user
	 * @param source The user/server doing the kill
	 * @param reason The reason for the kill
	 */
	void KillInternal(const MessageSource &source, const Anope::string &reason);

	/** Processes a quit for the user, and marks them as quit
	 * @param reason The reason for the quit
	 */
	void Quit(const Anope::string &reason = "");

	bool Quitting() const;

	/* Returns a mask that will most likely match any address the
	 * user will have from that location.  For IP addresses, wildcards the
	 * last octet (e.g. 35.1.1.1 -> 35.1.1.*). for named addresses, wildcards
	 * the leftmost part of the name unless the  name only contains two parts.
	 * If the username begins with a ~, replace with *.
	 */
	Anope::string Mask() const;

	/** Notes the usage of an incorrect password. If too many
	 * incorrect passwords are used the user might be killed.
	 * @return true if the user was killed
	 */
	bool BadPassword();

	/** Determines whether this user should receive a PRIVMSG instead of a NOTICE. */
	bool ShouldPrivmsg() const;

	/** Finds a user by nick, or possibly UID
	 * @param name The nick, or possibly UID, to lookup
	 * @param nick_only set to true to only look up by nick, not UID
	 * @return the user, if they exist
	 */
	static User *Find(const Anope::string &name, bool nick_only = false);

	/** Quits all users who are pending to be quit
	 */
	static void QuitUsers();
};

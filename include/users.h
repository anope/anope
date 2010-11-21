/*
 * Copyright (C) 2008-2010 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#ifndef USERS_H
#define USERS_H

extern CoreExport patricia_tree<User, std::equal_to<ci::string> > UserListByNick;
extern CoreExport patricia_tree<User> UserListByUID;

class CoreExport ChannelStatus : public Flags<ChannelModeName, CMODE_END * 2>
{
 public:
	Anope::string BuildCharPrefixList() const;
	Anope::string BuildModePrefixList() const;
};

struct ChannelContainer
{
	Channel *chan;
	ChannelStatus *Status;

	ChannelContainer(Channel *c) : chan(c) { }
	virtual ~ChannelContainer() { }
};

typedef std::list<ChannelContainer *> UChannelList;

/* Online user and channel data. */
class CoreExport User : public Extensible
{
 protected:
	Anope::string vident;
	Anope::string ident;
	Anope::string uid;
	bool OnAccess; /* If the user is on the access list of the nick theyre on */
	Flags<UserModeName, UMODE_END * 2> modes; /* Bitset of mode names the user has set on them */
	std::map<UserModeName, Anope::string> Params; /* Map of user modes and the params this user has */
	NickCore *nc; /* NickCore account the user is currently loggged in as */

 public: // XXX: exposing a tiny bit too much
	Anope::string nick;		/* User's current nick */

	Anope::string host;		/* User's real hostname */
	Anope::string vhost;		/* User's virtual hostname */
	Anope::string chost;		/* User's cloaked hostname */
	Anope::string realname;		/* Realname */
	sockaddrs ip;			/* User's IP */
	Server *server;			/* Server user is connected to */
	time_t timestamp;		/* Timestamp of the nick */
	time_t my_signon;		/* When did _we_ see the user? */

	int isSuperAdmin;		/* is SuperAdmin on or off? */

	/* Channels the user is in */
	UChannelList chans;

	unsigned short invalid_pw_count;	/* # of invalid password attempts */
	time_t invalid_pw_time;				/* Time of last invalid password */

	time_t lastmemosend;	/* Last time MS SEND command used */
	time_t lastnickreg;		/* Last time NS REGISTER cmd used */
	time_t lastmail;		/* Last time this user sent a mail */

	/****************************************************************/

	/** Create a new user object, initialising necessary fields and
	 * adds it to the hash
	 *
	 * @param snick The nickname of the user.
	 * @param sident The username of the user
	 * @param shost The hostname of the user
	 * @param suid The unique identifier of the user.
	 */
	User(const Anope::string &snick, const Anope::string &sident, const Anope::string &shost, const Anope::string &suid);

	/** Destroy a user.
	 */
	virtual ~User();

	/** Update the nickname of a user record accordingly, should be
	 * called from ircd protocol.
	 */
	void SetNewNick(const Anope::string &newnick);

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

	/** Retrieves the UID of the user, where applicable, if set.
	 * This is not used on some IRCds, but is for a lot e.g. P10, TS6 protocols.
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
	 * NOTE: Where possible, you should use the Get/SetVIdent() equivilants.
	 */
	void SetIdent(const Anope::string &ident);

	/** Get the real ident (username) of this user.
	 * @return The displayed ident of this user.
	 * NOTE: Where possible, you should use the Get/SetVIdent() equivilants.
	 */
	const Anope::string &GetIdent() const;

	/** Get the full mask ( nick!ident@realhost ) of a user
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
	 * @param source Sender nick
	 * @param fmt Format of the Message
	 * @param ... any number of parameters
	 */
	void SendMessage(const Anope::string &source, const char *fmt, ...);
	virtual void SendMessage(const Anope::string &source, const Anope::string &msg);

	/** Send a language string message to a user
	 * @param source Sender
	 * @param message The message num
	 * @param ... parameters
	 */
	void SendMessage(BotInfo *source, LanguageString message, ...);

	/** Collide a nick
	 * See the comment in users.cpp
	 * @param na The nick
	 */
	void Collide(NickAlias *na);

	/** Login the user to a NickCore
	 * @param core The account the user is useing
	 */
	void Login(NickCore *core);

	/** Logout the user
	 */
	void Logout();

	/** Get the account the user is logged in using
	 * @return The account or NULL
	 */
	virtual NickCore *Account();

	/** Check if the user is identified for their nick
	 * @param CheckNick True to check if the user is identified to the nickname they are on too
	 * @return true or false
	 */
	virtual bool IsIdentified(bool CheckNick = false);

	/** Check if the user is recognized for their nick (on the nicks access list)
	 * @param CheckSecure Only returns true if the user has secure off
	 * @return true or false
	 */
	virtual bool IsRecognized(bool CheckSecure = false);

	/** Update the last usermask stored for a user, and check to see if they are recognized
	 */
	void UpdateHost();

	/** Check if the user has a mode
	 * @param Name Mode name
	 * @return true or false
	 */
	bool HasMode(UserModeName Name) const;

	/** Set a mode internally on the user, the IRCd is not informed
	 * @param um The user mode
	 * @param Param The param, if there is one
	 */
	void SetModeInternal(UserMode *um, const Anope::string &Param = "");

	/** Remove a mode internally on the user, the IRCd is not informed
	 * @param um The user mode
	 */
	void RemoveModeInternal(UserMode *um);

	/** Set a mode on the user
	 * @param bi The client setting the mode
	 * @param um The user mode
	 * @param Param Optional param for the mode
	 */
	void SetMode(BotInfo *bi, UserMode *um, const Anope::string &Param = "");

	/** Set a mode on the user
	 * @param bi The client setting the mode
	 * @param Name The mode name
	 * @param Param Optional param for the mode
	 */
	void SetMode(BotInfo *bi, UserModeName Name, const Anope::string &Param = "");

	/** Remove a mode on the user
	 * @param bi The client setting the mode
	 * @param um The user mode
	 */
	void RemoveMode(BotInfo *bi, UserMode *um);

	/** Remove a mode from the user
	 * @param bi The client setting the mode
	 * @param Name The mode name
	 */
	void RemoveMode(BotInfo *bi, UserModeName Name);

	/** Set a string of modes on a user
	 * @param bi The client setting the modes
	 * @param umodes The modes
	 */
	void SetModes(BotInfo *bi, const char *umodes, ...);

	/** Set a string of modes on a user internally
	 * @param umodes The modes
	 */
	void SetModesInternal(const char *umodes, ...);

	/** Find the channel container for Channel c that the user is on
	 * This is preferred over using FindUser in Channel, as there are usually more users in a channel
	 * than channels a user is in
	 * @param c The channel
	 * @return The channel container, or NULL
	 */
	ChannelContainer *FindChannel(const Channel *c);

	/** Check if the user is protected from kicks and negative mode changes
	 * @return true or false
	 */
	bool IsProtected() const;
};

#endif // USERS_H

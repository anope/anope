/*
 * Copyright (C) 2008-2010 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2010 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 *
 * $Id$
 *
 */

struct ChannelContainer
{
	Channel *chan;
	Flags<ChannelModeName> *Status;

	ChannelContainer(Channel *c) : chan(c) { }
	virtual ~ChannelContainer() { }
};

typedef std::list<ChannelContainer *> UChannelList;

/* Online user and channel data. */
class CoreExport User : public Extensible
{
 protected:
	std::string vident;
	std::string ident;
	std::string uid;
	bool OnAccess;	/* If the user is on the access list of the nick theyre on */
	std::bitset<128> modes; /* Bitset of mode names the user has set on them */
	std::map<UserModeName, std::string> Params; /* Map of user modes and the params this user has */
	NickCore *nc; /* NickCore account the user is currently loggged in as */

 public: // XXX: exposing a tiny bit too much
	User *next, *prev;

	std::string nick;	/* User's current nick */

	char *host;		/* User's real hostname 	*/
	char *hostip;		/* User's IP number			 */
	char *vhost;		/* User's virtual hostname 	*/
	std::string chost;	/* User's cloaked hostname */
	char *realname;		/* Realname 			*/
	Server *server;		/* Server user is connected to  */
	time_t timestamp;	/* Timestamp of the nick 	*/
	time_t my_signon;	/* When did _we_ see the user?  */

	int isSuperAdmin;	/* is SuperAdmin on or off? */

	/* Channels the user is in */
	UChannelList chans;

	unsigned short invalid_pw_count;	/* # of invalid password attempts */
	time_t invalid_pw_time;	/* Time of last invalid password */

	time_t lastmemosend;	/* Last time MS SEND command used */
	time_t lastnickreg;	/* Last time NS REGISTER cmd used */
	time_t lastmail;	/* Last time this user sent a mail */


	/****************************************************************/

	/** Create a new user object, initialising necessary fields and
	 * adds it to the hash
	 *
	 * @param nick The nickname of the user.
	 * @param uid The unique identifier of the user.
	 */
	User(const std::string &nick, const std::string &uid);

	/** Destroy a user.
	 */
	~User();

	/** Update the nickname of a user record accordingly, should be
	 * called from ircd protocol.
	 */
	virtual void SetNewNick(const std::string &newnick);

	/** Update the displayed (vhost) of a user record.
	 * This is used (if set) instead of real host.
	 * @param host The new displayed host to give the user.
	 */
	void SetDisplayedHost(const std::string &host);

	/** Get the displayed vhost of a user record.
	 * @return The displayed vhost of the user, where ircd-supported, or the user's real host.
	 */
	const std::string GetDisplayedHost() const;

	/** Update the cloaked host of a user
	 * @param host The cloaked host
	 */
	void SetCloakedHost(const std::string &newhost);

	/** Get the cloaked host of a user
	 * @return The cloaked host
	 */
	const std::string &GetCloakedHost() const;

	/** Retrieves the UID of the user, where applicable, if set.
	 * This is not used on some IRCds, but is for a lot e.g. P10, TS6 protocols.
	 * @return The UID of the user.
	 */
	const std::string &GetUID() const;

	/** Update the displayed ident (username) of a user record.
	 * @param ident The new ident to give this user.
	 */
	void SetVIdent(const std::string &ident);

	/** Get the displayed ident (username) of this user.
	 * @return The displayed ident of this user.
	 */
	const std::string &GetVIdent() const;

	/** Update the real ident (username) of a user record.
	 * @param ident The new ident to give this user.
	 * NOTE: Where possible, you should use the Get/SetVIdent() equivilants.
	 */
	void SetIdent(const std::string &ident);

	/** Get the real ident (username) of this user.
	 * @return The displayed ident of this user.
	 * NOTE: Where possible, you should use the Get/SetVIdent() equivilants.
	 */
	const std::string &GetIdent() const;

	/** Get the full mask ( nick!ident@realhost ) of a user
	 */
	const std::string GetMask();

	/** Updates the realname of the user record.
	 */
	void SetRealname(const std::string &realname);

	/**
	 * Send a message (notice or privmsg, depending on settings) to a user
	 * @param source Sender nick
	 * @param fmt Format of the Message
	 * @param ... any number of parameters
	 */
	virtual void SendMessage(const std::string &source, const char *fmt, ...);
	virtual void SendMessage(const std::string &source, const std::string &msg);

	/** Collide a nick
	 * See the comment in users.cpp
	 * @param na The nick
	 */
	void Collide(NickAlias *na);

	/** Check if the user should become identified because
	 * their svid matches the one stored in their nickcore
	 * @param svid Services id
	 */
	void CheckAuthenticationToken(const char *svid);

	/** Auto identify the user to the given accountname.
	 * @param account Display nick of account
	 */
	void AutoID(const std::string &account);

	/** Login the user to a NickCore
	 * @param core The account the user is useing
	 */
	void Login(NickCore *core);

	/** Logout the user
	 */
	void Logout();

	/** Get the account the user is logged in using
	 * @reurn The account or NULL
	 */
	virtual NickCore *Account() const;

	/** Check if the user is identified for their nick
	 * @return true or false
	 */
	virtual const bool IsIdentified() const;

	/** Check if the user is recognized for their nick (on the nicks access list)
	 * @return true or false
	 */
	virtual const bool IsRecognized() const;

	/** Update the last usermask stored for a user, and check to see if they are recognized
	 */
	void UpdateHost();

       /** Check if the user has a mode
        * @param Name Mode name
        * @return true or false
        */
	const bool HasMode(UserModeName Name) const;

	/** Set a mode internally on the user, the IRCd is not informed
	 * @param um The user mode
	 * @param Param The param, if there is one
	 */
	void SetModeInternal(UserMode *um, const std::string &Param = "");

	/** Remove a mode internally on the user, the IRCd is not informed
	 * @param um The user mode
	 */
	void RemoveModeInternal(UserMode *um);

	/** Set a mode on the user
	 * @param bi The client setting the mode
	 * @param um The user mode
	 * @param Param Optional param for the mode
	 */
	void SetMode(BotInfo *bi, UserMode *um, const std::string &Param = "");

	/** Set a mode on the user
	 * @param bi The client setting the mode
	 * @param Name The mode name
	 * @param Param Optional param for the mode
	 */
	void SetMode(BotInfo *bi, UserModeName Name, const std::string &Param = "");

	/* Set a mode on the user
	 * @param bi The client setting the mode
	 * @param ModeChar The mode char
	 * @param param Optional param for the mode
	 */
	void SetMode(BotInfo *bi, char ModeChar, const std::string &Param = "");

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

	/** Remove a mode from the user
	 * @param bi The client setting the mode
	 * @param ModeChar The mode char
	 */
	void RemoveMode(BotInfo *bi, char ModeChar);

	/** Set a string of modes on a user
	 * @param bi The client setting the mode
	 * @param modes The modes
	 */
	void SetModes(BotInfo *bi, const char *modes, ...);

	/** Find the channel container for Channel c that the user is on
	 * This is preferred over using FindUser in Channel, as there are usually more users in a channel
	 * than channels a user is in
	 * @param c The channel
	 * @return The channel container, or NULL
	 */
	ChannelContainer *FindChannel(Channel *c);
};


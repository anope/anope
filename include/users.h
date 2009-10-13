/*
 * Copyright (C) 2008-2009 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008-2009 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 *
 * $Id$
 *
 */

struct u_chanlist {
	struct u_chanlist *next, *prev;
	Channel *chan;
	int16 status;		/* Associated flags; see CSTATUS_* below. */
};

struct u_chaninfolist {
	struct u_chaninfolist *next, *prev;
	ChannelInfo *chan;
};

/* Online user and channel data. */
class CoreExport User : public Extensible
{
 private:
	std::string vident;
	std::string ident;
	std::string uid;
	bool OnAccess;	/* If the user is on the access list of the nick theyre on */

 public: // XXX: exposing a tiny bit too much
	User *next, *prev;

	char nick[NICKMAX];

	char *host;		/* User's real hostname 	*/
	char *hostip;		/* User's IP number			 */
	char *vhost;		/* User's virtual hostname 	*/
	std::string chost;	/* User's cloaked hostname */
	char *realname;		/* Realname 			*/
	Server *server;		/* Server user is connected to  */
	time_t timestamp;	/* Timestamp of the nick 	*/
	time_t my_signon;	/* When did _we_ see the user?  */
	std::bitset<128> modes;

	NickCore *nc;

	int isSuperAdmin;	/* is SuperAdmin on or off? */

	struct u_chanlist *chans;	/* Channels user has joined */
	struct u_chaninfolist *founder_chans;	/* Channels user has identified for */

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
	void SetNewNick(const std::string &newnick);

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

	/** Updates the realname of the user record.
	 */
	void SetRealname(const std::string &realname);

	/**
	 * Send a message (notice or privmsg, depending on settings) to a user
	 * @param source Sender nick
	 * @param fmt Format of the Message
	 * @param ... any number of parameters
	 * @return void
	 */
	void SendMessage(const char *source, const char *fmt, ...);
	void SendMessage(const char *source, const std::string &msg);

	/** Check if the user should become identified because
	 * their svid matches the one stored in their nickcore
	 * @param svid Services id
	 */
	void CheckAuthenticationToken(const char *svid);

	/** Auto identify the user to the given accountname.
	 * @param account Display nick of account
	 */
	void AutoID(const char *acc);

	/** Check if the user is recognized for their nick (on the nicks access list)
	 * @return true or false
	 */
	const bool IsRecognized() const;

	/** Update the last usermask stored for a user, and check to see if they are recognized
	 */
	void UpdateHost();
       
       /** Check if the user has a mode
        * @param Name Mode name
        * @return true or false
        */
       const bool HasMode(UserModeName Name) const;
 
       /** Set a mode on the user
        * @param Name The mode name
        */
       void SetMode(UserModeName Name);
 
       /* Set a mode on the user
        * @param ModeChar The mode char
        */
       void SetMode(char ModeChar);
 
       /** Remove a mode from the user
        * @param Name The mode name
        */
       void RemoveMode(UserModeName Name);

	/** Remove a mode from the user
        * @param ModeChar The mode char
        */
       void RemoveMode(char ModeChar);
};


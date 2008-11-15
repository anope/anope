/*
 * Copyright (C) 2008 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008 Anope Team <info@anope.org>
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
class User : public Extensible
{
 public: // XXX: exposing a tiny bit too much
	User *next, *prev;

	char nick[NICKMAX];

	char *username;		/* ident			*/
	char *host;		/* User's real hostname 	*/
	char *hostip;		/* User's IP number			 */
	char *vhost;		/* User's virtual hostname 	*/
	std::string chost;	/* User's cloaked hostname */
	char *vident;		/* User's virtual ident 	*/
	char *realname;		/* Realname 			*/
	Server *server;		/* Server user is connected to  */
	char *nickTrack;	/* Nick Tracking 		*/
	time_t timestamp;	/* Timestamp of the nick 	*/
	time_t my_signon;	/* When did _we_ see the user?  */
	time_t svid;		/* Services ID 			*/
	uint32 mode;		/* See below 			*/
	char *uid;		/* Univeral ID			*/

	NickAlias *na;

	ModuleData *moduleData;	/* defined for it, it should allow the module Add/Get */

	int isSuperAdmin;	/* is SuperAdmin on or off? */

	struct u_chanlist *chans;	/* Channels user has joined */
	struct u_chaninfolist *founder_chans;	/* Channels user has identified for */

	short invalid_pw_count;	/* # of invalid password attempts */
	time_t invalid_pw_time;	/* Time of last invalid password */

	time_t lastmemosend;	/* Last time MS SEND command used */
	time_t lastnickreg;	/* Last time NS REGISTER cmd used */
	time_t lastmail;	/* Last time this user sent a mail */


	/****************************************************************/

	/** Create a new user object, initialising necessary fields and
	 * adds it to the hash
	 *
	 * @parameter nick The nickname of the user account.
	 */
	User(const std::string &nick);

	/** Destroy a user.
	 */
	~User();

	/** Update the nickname of a user record accordingly, should be
	 * called from ircd protocol.
	 */
	void SetNewNick(const std::string &newnick);

	/** Update the displayed (vhost) of a user record.
	 * This is used (if set) instead of real host.
	 */
	void SetDisplayedHost(const std::string &host);

	/** Update the displayed ident (username) of a user record.
	 */
	void SetIdent(const std::string &ident);

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
};


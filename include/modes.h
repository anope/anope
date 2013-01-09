/* Mode support
 *
 * Copyright (C) 2008-2011 Adam <Adam@anope.org>
 * Copyright (C) 2008-2013 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#ifndef MODES_H
#define MODES_H

#include "anope.h"
#include "base.h"

/** All of the valid user mode names
 */
enum UserModeName
{
	UMODE_BEGIN,

	UMODE_SERV_ADMIN, UMODE_BOT, UMODE_CO_ADMIN, UMODE_FILTER, UMODE_HIDEOPER, UMODE_NETADMIN,
	UMODE_REGPRIV, UMODE_PROTECTED, UMODE_NOCTCP, UMODE_WEBTV, UMODE_WEBIRC, UMODE_WHOIS, UMODE_ADMIN, UMODE_DEAF,
	UMODE_GLOBOPS, UMODE_HELPOP, UMODE_INVIS, UMODE_OPER, UMODE_PRIV, UMODE_GOD, UMODE_REGISTERED,
	UMODE_SNOMASK, UMODE_VHOST, UMODE_WALLOPS, UMODE_CLOAK, UMODE_SSL, UMODE_SOFTCALLERID, UMODE_CALLERID,
	UMODE_COMMONCHANS, UMODE_HIDDEN, UMODE_STRIPCOLOR, UMODE_INVISIBLE_OPER, UMODE_RESTRICTED, UMODE_HIDEIDLE,

	UMODE_END
};

/** All of the valid channel mode names
 */
enum ChannelModeName
{
	CMODE_BEGIN,

	/* Channel modes */
	CMODE_BLOCKCOLOR, CMODE_FLOOD, CMODE_INVITE, CMODE_KEY, CMODE_LIMIT, CMODE_MODERATED, CMODE_NOEXTERNAL,
	CMODE_PRIVATE, CMODE_REGISTERED, CMODE_SECRET, CMODE_TOPIC, CMODE_AUDITORIUM, CMODE_SSL, CMODE_ADMINONLY,
	CMODE_NOCTCP, CMODE_FILTER, CMODE_NOKNOCK, CMODE_REDIRECT, CMODE_REGMODERATED, CMODE_NONICK, CMODE_OPERONLY,
	CMODE_NOKICK, CMODE_REGISTEREDONLY, CMODE_STRIPCOLOR, CMODE_NONOTICE, CMODE_NOINVITE, CMODE_ALLINVITE,
	CMODE_BLOCKCAPS, CMODE_PERM, CMODE_NICKFLOOD, CMODE_JOINFLOOD, CMODE_DELAYEDJOIN, CMODE_NOREJOIN,
	CMODE_BANDWIDTH,

	/* b/e/I */
	CMODE_BAN, CMODE_EXCEPT,
	CMODE_INVITEOVERRIDE,

	/* v/h/o/a/q */
	CMODE_VOICE, CMODE_HALFOP, CMODE_OP,
	CMODE_PROTECT, CMODE_OWNER,

	CMODE_END
};

/** The different types of modes
*/
enum ModeType
{
	/* Regular mode */
	MODE_REGULAR,
	/* b/e/I */
	MODE_LIST,
	/* k/l etc */
	MODE_PARAM,
	/* v/h/o/a/q */
	MODE_STATUS
};

/* Classes of modes, Channel modes and User modes
 */
enum ModeClass
{
	MC_CHANNEL,
	MC_USER
};

/** This class is the basis of all modes in Anope
 */
class CoreExport Mode : public Base
{
 public:
	/* Class of mode this is (user/channel) */
	ModeClass mclass;
	/* Mode char for this, eg 'b' */
	char mchar;
	/* Type of mode this is, eg MODE_LIST */
	ModeType type;

	/** constructor
	 * @param mclass The type of mode this is
	 * @param mc The mode char
	 * @param type The mode type
	 */
	Mode(ModeClass mclass, char mc, ModeType type);
	virtual ~Mode();
};

/** This class is a user mode, all user modes use this/inherit from this
 */
class CoreExport UserMode : public Mode
{
 public:
	/* Mode name */
	UserModeName name;

	/** constructor
	 * @param name The mode name
	 * @param mc The mode char
	 */
	UserMode(UserModeName name, char mc);
	virtual ~UserMode();

	/** Returns the mode name as a string
	 */
	const Anope::string NameAsString();
};

class CoreExport UserModeParam : public UserMode
{
 public:
 	/** constructor
	 * @param name The mode name
	 * @param mc The mode char
	 */
	UserModeParam(UserModeName name, char mc);

	/** Check if the param is valid
	 * @param value The param
	 * @return true or false
	 */
	virtual bool IsValid(const Anope::string &value) const { return true; }
};

/** This class is a channel mode, all channel modes use this/inherit from this
 */
class CoreExport ChannelMode : public Mode
{
 public:
	/* Mode name */
	ChannelModeName name;

	/** constructor
	 * @param name The mode name
	 * @param mc The mode char
	 */
	ChannelMode(ChannelModeName name, char mc);
	virtual ~ChannelMode();

	/** Can a user set this mode, used for mlock
	 * NOTE: User CAN be NULL, this is for checking if it can be locked with defcon
	 * @param u The user, or NULL
	 */
	virtual bool CanSet(User *u) const;

	/** Returns the mode name as a string
	 */
	const Anope::string NameAsString();
};


/** This is a mode for lists, eg b/e/I. These modes should inherit from this
 */
class CoreExport ChannelModeList : public ChannelMode
{
 public:
	/** constructor
	 * @param name The mode name
	 * @param mc The mode char
	 */
	ChannelModeList(ChannelModeName name, char mc);

	/** destructor
	 */
	virtual ~ChannelModeList();

	/** Is the mask valid
	 * @param mask The mask
	 * @return true for yes, false for no
	 */
	virtual bool IsValid(const Anope::string &mask) const { return true; }

	/** Checks if mask affects user
	 * Should only be used for extbans or other weird ircd-specific things.
	 * @param u The user
	 * @param e The entry to match against
	 * @return true on match
	 */
	virtual bool Matches(const User *u, const Entry *e) { return false; }

	/** Called when a mask is added to a channel
	 * @param chan The channel
	 * @param mask The mask
	 */
	virtual void OnAdd(Channel *chan, const Anope::string &mask) { }

	/** Called when a mask is removed from a channel
	 * @param chan The channel
	 * @param mask The mask
	 */
	virtual void OnDel(Channel *chan, const Anope::string &mask) { }
};

/** This is a mode with a paramater, eg +k/l. These modes should use/inherit from this
*/
class CoreExport ChannelModeParam : public ChannelMode
{
 public:
	/** constructor
	 * @param name The mode name
	 * @param mc The mode char
	 * @param minus_no_arg true if this mode sends no arg when unsetting
	 */
	ChannelModeParam(ChannelModeName name, char mc, bool minus_no_arg = false);

	/** destructor
	 */
	virtual ~ChannelModeParam();

	/* Should we send an arg when unsetting this mode? */
	bool minus_no_arg;

	/** Is the param valid
	 * @param value The param
	 * @return true for yes, false for no
	 */
	virtual bool IsValid(const Anope::string &value) const { return true; }
};

/** This is a mode that is a channel status, eg +v/h/o/a/q.
*/
class CoreExport ChannelModeStatus : public ChannelMode
{
 public:
	/* The symbol, eg @ % + */
	char Symbol;
	/* The "level" of the mode, used to compare with other modes.
	 * Used so we know op > halfop > voice etc.
	 */
	unsigned short Level;

	/** constructor
	 * @param name The mode name
	 * @param mc The mode char
	 * @param mSymbol The symbol for the mode, eg @ % 
	 * @param mLevel A level for the mode, which is usually determined by the PREFIX capab
	 */
	ChannelModeStatus(ChannelModeName name, char mc, char mSymbol, unsigned short mLevel = 0);

	/** destructor
	 */
	virtual ~ChannelModeStatus();
};

/* The status a user has on a channel (+v, +h, +o) etc */
class CoreExport ChannelStatus : public Flags<ChannelModeName>
{
 public:
	Anope::string BuildCharPrefixList() const;
	Anope::string BuildModePrefixList() const;
};

/** Channel mode +k (key)
 */
class CoreExport ChannelModeKey : public ChannelModeParam
{
 public:
	ChannelModeKey(char mc) : ChannelModeParam(CMODE_KEY, mc) { }

	bool IsValid(const Anope::string &value) const anope_override;
};

/** This class is used for channel mode +A (Admin only)
 * Only opers can mlock it
 */
class CoreExport ChannelModeAdmin : public ChannelMode
{
 public:
	ChannelModeAdmin(char mc) : ChannelMode(CMODE_ADMINONLY, mc) { }

	/* Opers only */
	bool CanSet(User *u) const anope_override;
};

/** This class is used for channel mode +O (Opers only)
 * Only opers can mlock it
 */
class CoreExport ChannelModeOper : public ChannelMode
{
 public:
	ChannelModeOper(char mc) : ChannelMode(CMODE_OPERONLY, mc) { }

	/* Opers only */
	bool CanSet(User *u) const anope_override;
};

/** This class is used for channel mode +r (registered channel)
 * No one may mlock r
 */
class CoreExport ChannelModeRegistered : public ChannelMode
{
 public:
	ChannelModeRegistered(char mc) : ChannelMode(CMODE_REGISTERED, mc) { }

	/* No one mlocks +r */
	bool CanSet(User *u) const anope_override;
};

class StackerInfo
{
 public:
	/* Modes to be added */
	std::list<std::pair<Mode *, Anope::string> > AddModes;
	/* Modes to be deleted */
	std::list<std::pair<Mode *, Anope::string> > DelModes;
	/* Bot this is sent from */
	const BotInfo *bi;

	/** Add a mode to this object
	 * @param mode The mode
	 * @param set true if setting, false if unsetting
	 * @param param The param for the mode
	 */
	void AddMode(Mode *mode, bool set, const Anope::string &param);
};

/** This is the mode manager
 * It contains functions for adding modes to Anope so Anope can track them
 * and do things such as MLOCK.
 * This also contains a mode stacker that will combine multiple modes and set
 * them on a channel or user at once
 */
class CoreExport ModeManager
{
 protected:
	/* List of pairs of user/channels and their stacker info */
	static std::map<User *, StackerInfo *> UserStackerObjects;
	static std::map<Channel *, StackerInfo *> ChannelStackerObjects;

	/** Build a list of mode strings to send to the IRCd from the mode stacker
	 * @param info The stacker info for a channel or user
	 * @return a list of strings
	 */
	static std::list<Anope::string> BuildModeStrings(StackerInfo *info);

 public:
 	/* List of all modes Anope knows about */
	static std::vector<ChannelMode *> ChannelModes;
	static std::vector<UserMode *> UserModes;

	/* Number of generic channel and user modes we are tracking */
	static unsigned GenericChannelModes, GenericUserModes;
	/* Default channel mode lock */
	static std::list<std::pair<ChannelModeName, Anope::string> > ModeLockOn;
	static std::list<ChannelModeName> ModeLockOff;
	/* Default modes bots have on channels */
	static ChannelStatus DefaultBotModes;

	/** Add a user mode to Anope
	 * @param um A UserMode or UserMode derived class
	 * @return true on success, false on error
	 */
	static bool AddUserMode(UserMode *um);

	/** Add a channel mode to Anope
	 * @param cm A ChannelMode or ChannelMode derived class
	 * @return true on success, false on error
	 */
	static bool AddChannelMode(ChannelMode *cm);

	/** Remove a user mode from Anope
	 * @param um A UserMode to remove
	 */
	static void RemoveUserMode(UserMode *um);

	/** Remove a channel mode from Anope
	 * @param um A ChanneMode to remove
	 */
	static void RemoveChannelMode(ChannelMode *cm);

	/** Find a channel mode
	 * @param mode The mode
	 * @return The mode class
	 */
	static ChannelMode *FindChannelModeByChar(char mode);

	/** Find a user mode
	 * @param mode The mode
	 * @return The mode class
	 */
	static UserMode *FindUserModeByChar(char mode);

	/** Find a channel mode
	 * @param name The modename
	 * @return The mode class
	 */
	static ChannelMode *FindChannelModeByName(ChannelModeName name);

	/** Find a user mode
	 * @param name The modename
	 * @return The mode class
	 */
	static UserMode *FindUserModeByName(UserModeName name);

	/** Find channel mode by string
	 * @param name The mode name
	 * @return The mode
	 */
	static ChannelMode *FindChannelModeByString(const Anope::string &name);

	/** Find user mode by string
	 * @param name The mode name
	 * @return The mode
	 */
	static UserMode *FindUserModeByString(const Anope::string &name);

	/** Gets the channel mode char for a symbol (eg + returns v)
	 * @param symbol The symbol
	 * @return The char
	 */
	static char GetStatusChar(char symbol);

	/** Add a mode to the stacker to be set on a channel
	 * @param bi The client to set the modes from
	 * @param c The channel
	 * @param cm The channel mode
	 * @param set true for setting, false for removing
	 * @param param The param, if there is one
	 */
	static void StackerAdd(const BotInfo *bi, Channel *c, ChannelMode *cm, bool set, const Anope::string &param = "");

	/** Add a mode to the stacker to be set on a user
	 * @param bi The client to set the modes from
	 * @param u The user
	 * @param um The user mode
	 * @param set true for setting, false for removing
	 * @param param The param, if there is one
	 */
	static void StackerAdd(const BotInfo *bi, User *u, UserMode *um, bool set, const Anope::string &param = "");

	/** Process all of the modes in the stacker and send them to the IRCd to be set on channels/users
	 */
	static void ProcessModes();

	/** Delete a user, channel, or mode from the stacker
	 */
	static void StackerDel(User *u);
	static void StackerDel(Channel *c);
	static void StackerDel(Mode *m);

	/** Updates the default mode locks and default bot modes
	 * @param config The configuration to read from. This is often called
	 * during a config reload.
	 */
	static void UpdateDefaultMLock(ServerConfig *config);
};

/** Entry flags
 */
enum EntryType
{
	ENTRYTYPE_NONE,
	ENTRYTYPE_CIDR,
	ENTRYTYPE_NICK_WILD,
	ENTRYTYPE_NICK,
	ENTRYTYPE_USER_WILD,
	ENTRYTYPE_USER,
	ENTRYTYPE_HOST_WILD,
	ENTRYTYPE_HOST
};

/** Represents a mask set on a channel (b/e/I)
 */
class CoreExport Entry : public Flags<EntryType>
{
	ChannelModeName modename;

 public:
	unsigned char cidr_len;
	Anope::string mask;
	Anope::string nick, user, host;

	/** Constructor
 	 * @param mode What mode this host is for - can be CMODE_BEGIN for unknown/no mode
	 * @param host A full nick!ident@host/cidr mask
	 */
	Entry(ChannelModeName mode, const Anope::string &host);

	/** Get the banned mask for this entry
	 * @return The mask
	 */
	const Anope::string GetMask();

	/** Check if this entry matches a user
	 * @param u The user
	 * @param full True to match against a users real host and IP
	 * @return true on match
	 */
	bool Matches(const User *u, bool full = false) const;
};

#endif // MODES_H

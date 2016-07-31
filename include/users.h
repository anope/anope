/*
 * Anope IRC Services
 *
 * Copyright (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2009-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "anope.h"
#include "modes.h"
#include "extensible.h"
#include "serialize.h"
#include "commands.h"
#include "sockets.h"

typedef Anope::hash_map<User *> user_map;

extern CoreExport user_map UserListByNick, UserListByUID;

extern CoreExport int OperCount;

enum class UserType
{
	USER,
	//LOCAL_USER,
	BOT
};

/* Online user and channel data. */
class CoreExport User : public virtual Base, public virtual Extensible, public CommandReply
{
	/* true if the user was quit or killed */
	bool quit;
	/* Users that are in the process of quitting */
	static std::list<User *> quitting_users;

 public:
	typedef std::map<Anope::string, Anope::string> ModeList;
 protected:
	Anope::string vident;
	Anope::string ident;
	Anope::string uid;
	/* If the user is on the access list of the nick they're on */
	bool on_access;
	/* Map of user modes and the params this user has (if any) */
	ModeList modes;
	/* NickServ::Account account the user is currently loggged in as, if they are logged in */
	Serialize::Reference<NickServ::Account> nc;

	/* # of invalid password attempts */
	unsigned short invalid_pw_count;
	/* Time of last invalid password */
	time_t invalid_pw_time;


 public:
	UserType type = UserType::USER;

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
	User(const Anope::string &snick, const Anope::string &sident, const Anope::string &shost, const Anope::string &svhost, const Anope::string &sip, Server *sserver, const Anope::string &srealname, time_t ts, const Anope::string &smodes, const Anope::string &suid, NickServ::Account *nc);

	/** Destroy a user.
	 */
	virtual ~User();

 public:
	static User* OnIntroduce(const Anope::string &snick, const Anope::string &sident, const Anope::string &shost, const Anope::string &svhost, const Anope::string &sip, Server *sserver, const Anope::string &srealname, time_t ts, const Anope::string &smodes, const Anope::string &suid, NickServ::Account *nc);

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
	 * NOTE: Where possible, you should use the Get/SetVIdent() equivilants.
	 */
	void SetIdent(const Anope::string &ident);

	/** Get the real ident (username) of this user.
	 * @return The displayed ident of this user.
	 * NOTE: Where possible, you should use the Get/SetVIdent() equivilants.
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
	 * @param message Format of the Message
	 * @param args any number of parameters
	 */
	template<typename... Args>
	void SendMessage(const MessageSource &source, const char *message, Args&&... args)
	{
		const char *translated_message = Language::Translate(this->Account(), message);
		SendMessage(source, Anope::Format(translated_message, std::forward<Args>(args)...));
	}
	void SendMessage(const MessageSource &, const Anope::string &msg) override;

	/** Identify the user to a nick.
	 * updates last_seen, logs the user in,
	 * send messages, checks for mails, set vhost and more
	 * @param na the nick to identify to, should be the same as
	 * the user's current nick
	 */
	void Identify(NickServ::Nick *na);

	/** Login the user to an account
	 * @param core The account
	 */
	void Login(NickServ::Account *core);

	/** Logout the user
	 */
	void Logout();

	/** Get the account the user is logged in using
	 * @return The account or NULL
	 */
	NickServ::Account *Account() const;

	/** Check if the user is identified for their nick
	 * @param check_nick True to check if the user is identified to the nickname they are on too
	 * @return true or false
	 */
	bool IsIdentified(bool check_nick = false) const;

	/** Check if the user is recognized for their nick (on the nicks access list)
	 * @param check_secure Only returns true if the user has secure off
	 * @return true or false
	 */
	bool IsRecognized(bool check_secure = true) const;

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

	/** Update the last usermask stored for a user, and check to see if they are recognized
	 */
	void UpdateHost();

	/** Check if the user has a mode
	 * @param name Mode name
	 * @return true or false
	 */
	bool HasMode(const Anope::string &name) const;

	/** Set a mode internally on the user, the IRCd is not informed
	 * @param setter who/what is setting the mode
	 * @param um The user mode
	 * @param Param The param, if there is one
	 */
	void SetModeInternal(const MessageSource &setter, UserMode *um, const Anope::string &param = "");

	/** Remove a mode internally on the user, the IRCd is not informed
	 * @param setter who/what is setting the mode
	 * @param um The user mode
	 */
	void RemoveModeInternal(const MessageSource &setter, UserMode *um);

	/** Set a mode on the user
	 * @param bi The client setting the mode
	 * @param um The user mode
	 * @param Param Optional param for the mode
	 */
	void SetMode(ServiceBot *bi, UserMode *um, const Anope::string &param = "");

	/** Set a mode on the user
	 * @param bi The client setting the mode
	 * @param name The mode name
	 * @param Param Optional param for the mode
	 */
	void SetMode(ServiceBot *bi, const Anope::string &name, const Anope::string &param = "");

	/** Remove a mode on the user
	 * @param bi The client setting the mode
	 * @param um The user mode
	 * @param param Optional param for the mode
	 */
	void RemoveMode(ServiceBot *bi, UserMode *um, const Anope::string &param = "");

	/** Remove a mode from the user
	 * @param bi The client setting the mode
	 * @param name The mode name
	 * @param param Optional param for the mode
	 */
	void RemoveMode(ServiceBot *bi, const Anope::string &name, const Anope::string &param = "");

	/** Set a string of modes on a user
	 * @param bi The client setting the modes
	 * @param umodes The modes
	 */
	void SetModes(ServiceBot *bi, const char *umodes, ...);

	/** Set a string of modes on a user internally
	 * @param setter who/what is setting the mode
	 * @param umodes The modes
	 */
	void SetModesInternal(const MessageSource &source, const char *umodes, ...);

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
	 * appropriate subnet mask (e.g. 35.1.1.1 -> 35.*; 128.2.1.1 -> 128.2.*);
	 * for named addresses, wildcards the leftmost part of the name unless the
	 * name only contains two parts.  If the username begins with a ~, delete
	 * it.
	 */
	Anope::string Mask() const;

	/** Notes the usage of an incorrect password. If too many
	 * incorrect passwords are used the user might be killed.
	 * @return true if the user was killed
	 */
	bool BadPassword();

	/** Finds a user by nick, or possibly UID
	 * @param name The nick, or possibly UID, to lookup
	 * @param nick_only set to true to only look up by nick, not UID
	 * @return the user, if they exist
	 */
	static User* Find(const Anope::string &name, bool nick_only = false);

	/** Quits all users who are pending to be quit
	 */
	static void QuitUsers();
};

class LocalUser : public User
{
 public:
	using User::User;

	/* Last time this user said something */
	time_t lastmsg = 0;
};


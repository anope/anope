/*
 * Anope IRC Services
 *
 * Copyright (C) 2016 Adam <Adam@anope.org>
 * Copyright (C) 2014-2016 Anope Team <team@anope.org>
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

#include "service.h"
#include "base.h"

/** Possible return types from events.
 */
enum EventReturn
{
	EVENT_STOP,
	EVENT_CONTINUE,
	EVENT_ALLOW
};

class Events : public Service
{
 public:
	Events(Module *o, const Anope::string &ename) : Service(o, ename) { }
};

/* uninstantiated */
template<typename EventHandler, typename Func, typename Return, typename... Args>
struct Caller;

/* specialization for void */
template<typename EventHandler, typename Func, typename... Args>
struct Caller<EventHandler, Func, void, Args...>
{
	void operator()(const std::vector<EventHandler *> &handlers, Func func, Args&&... args)
	{
		for (EventHandler *eh : handlers)
			(eh->*func)(std::forward<Args>(args)...);
	}
};

/* specialization for EventReturn */
template<typename EventHandler, typename Func, typename... Args>
struct Caller<EventHandler, Func, EventReturn, Args...>
{
	EventReturn operator()(const std::vector<EventHandler *> &handlers, Func func, Args&&... args)
	{
		EventReturn ret = EVENT_CONTINUE;

		for (EventHandler *eh : handlers)
		{
			ret = (eh->*func)(std::forward<Args>(args)...);
			if (ret != EVENT_CONTINUE)
				return ret;
		}

		return ret;
	}
};

template<typename EventHandler>
class EventHook;

template<typename EventHandler>
class EventDispatcher
{
	static_assert(std::is_base_of<Events, EventHandler>::value, "");

	ServiceReferenceList<EventHandler> handlers;

 public:
	EventDispatcher(const Anope::string &name) : handlers(name) { }

	template<typename Func, typename... Args>
	auto Dispatch(Func func, Args&&... args) -> decltype(((static_cast<EventHandler*>(nullptr))->*func)(args...))
	{
		const std::vector<EventHandler *> h = this->handlers.GetServices();
		return Caller<EventHandler, Func, decltype(((static_cast<EventHandler*>(nullptr))->*func)(args...)), Args...>()(h, func, std::forward<Args>(args)...);
	}
};

template<typename EventHandler>
class EventHook : public EventHandler
{
	static_assert(std::is_base_of<Events, EventHandler>::value, "");

 public:
	enum class Priority
	{
		FIRST,
		LAST
	}
	priority;

	EventHook(Module *creator) : EventHook(creator, Priority::LAST) { }

	EventHook(Module *creator, Priority p)
		: EventHandler(creator, EventHandler::NAME)
		, priority(p)
	{
#warning "priority doesnt work"
	}
};

class EventManager
{
	Anope::hash_map<EventDispatcher<Events> *> cache;

	template<typename T>
	EventDispatcher<T> *GetDispatcher(const Anope::string &name)
	{
		auto it = cache.find(name);
		if (it != cache.end())
			return reinterpret_cast<EventDispatcher<T> *>(it->second);

		auto dispatcher = new EventDispatcher<T>(name);
		cache[name] = reinterpret_cast<EventDispatcher<Events> *>(dispatcher);
		return dispatcher;
	}

	static EventManager *eventManager;

 public:
	template<
		typename Type,
		typename Function,
		typename... Args
	>
	auto Dispatch(Function Type::*func, Args&&... args) -> decltype(((static_cast<Type*>(nullptr))->*func)(args...))
	{
		static_assert(std::is_base_of<Events, Type>::value, "");

		EventDispatcher<Type> *dispatcher = GetDispatcher<Type>(Type::NAME);
		return dispatcher->Dispatch(func, std::forward<Args>(args)...);
	}

	static void Init();
	static EventManager *Get();
};

namespace Event
{
	struct CoreExport PreUserKicked : Events
	{
		static constexpr const char *NAME = "preuserkicked";

		using Events::Events;
		
		/** Called before a user has been kicked from a channel.
		 * @param source The kicker
		 * @param cu The user, channel, and status of the user being kicked
		 * @param kickmsg The reason for the kick.
		 * @return EVENT_STOP to stop the kick, which can only be done if source is 'Me' or from me
		 */
		virtual EventReturn OnPreUserKicked(const MessageSource &source, ChanUserContainer *cu, const Anope::string &kickmsg) anope_abstract;
	};

	struct CoreExport UserKicked : Events
	{
		static constexpr const char *NAME = "userkicked";

		using Events::Events;

		/** Called when a user has been kicked from a channel.
		 * @param source The kicker
		 * @param target The user being kicked
		 * @param channel The channel the user was kicked from, which may no longer exist
		 * @param status The status the kicked user had on the channel before they were kicked
		 * @param kickmsg The reason for the kick.
		 */
		virtual void OnUserKicked(const MessageSource &source, User *target, const Anope::string &channel, ChannelStatus &status, const Anope::string &kickmsg) anope_abstract;
	};

	struct CoreExport PreBotAssign : Events
	{
		static constexpr const char *NAME = "prebotassign";

		using Events::Events;

		/** Called before a bot is assigned to a channel.
		 * @param sender The user assigning the bot
		 * @param ci The channel the bot is to be assigned to.
		 * @param bi The bot being assigned.
		 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to deny the assign.
		 */
		virtual EventReturn OnPreBotAssign(User *sender, ChanServ::Channel *ci, ServiceBot *bi) anope_abstract;
	};

	struct CoreExport BotAssign : Events
	{
		static constexpr const char *NAME = "botassign";

		using Events::Events;

		/** Called when a bot is assigned ot a channel
		 */
		virtual void OnBotAssign(User *sender, ChanServ::Channel *ci, ServiceBot *bi) anope_abstract;
	};

	struct CoreExport BotUnAssign : Events
	{
		static constexpr const char *NAME = "botunassign";

		using Events::Events;

		/** Called before a bot is unassigned from a channel.
		 * @param sender The user unassigning the bot
		 * @param ci The channel the bot is being removed from
		 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to deny the unassign.
		 */
		virtual EventReturn OnBotUnAssign(User *sender, ChanServ::Channel *ci) anope_abstract;
	};

	struct CoreExport UserConnect : Events
	{
		static constexpr const char *NAME = "userconnect";

		using Events::Events;

		/** Called when a new user connects to the network.
		 * @param u The connecting user.
		 * @param exempt set to true/is true if the user should be excepted from bans etc
		 */
		virtual void OnUserConnect(User *u, bool &exempt) anope_abstract;
	};

	struct CoreExport NewServer : Events
	{
		static constexpr const char *NAME = "newserver";

		using Events::Events;

		/** Called when a new server connects to the network.
		 * @param s The server that has connected to the network
		 */
		virtual void OnNewServer(Server *s) anope_abstract;
	};

	struct CoreExport UserNickChange : Events
	{
		static constexpr const char *NAME = "usernickchange";

		using Events::Events;

		/** Called after a user changed the nick
		 * @param u The user.
		 * @param oldnick The old nick of the user
		 */
		virtual void OnUserNickChange(User *u, const Anope::string &oldnick) anope_abstract;
	};

	struct CoreExport PreCommand : Events
	{
		static constexpr const char *NAME = "precommand";

		using Events::Events;

		/** Called before a command is due to be executed.
		 * @param source The source of the command
		 * @param command The command the user is executing
		 * @param params The parameters the user is sending
		 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
		 */
		virtual EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) anope_abstract;
	};

	struct CoreExport PostCommand : Events
	{
		static constexpr const char *NAME = "postcommand";

		using Events::Events;

		/** Called after a command has been executed.
		 * @param source The source of the command
		 * @param command The command the user executed
		 * @param params The parameters the user sent
		 */
		virtual void OnPostCommand(CommandSource &source, Command *command, const std::vector<Anope::string> &params) anope_abstract;
	};

	struct CoreExport LoadDatabase : Events
	{
		static constexpr const char *NAME = "loaddatabase";

		using Events::Events;

		/** Called when the databases are loaded
		 * @return EVENT_CONTINUE to let other modules continue loading, EVENT_STOP to stop
		 */
		virtual EventReturn OnLoadDatabase() anope_abstract;
	};

	struct CoreExport Encrypt : Events
	{
		static constexpr const char *NAME = "encrypt";

		using Events::Events;

		/** Called when anope needs to check passwords against encryption
		 *  see src/encrypt.c for detailed informations
		 */
		virtual EventReturn OnEncrypt(const Anope::string &src, Anope::string &dest) anope_abstract;
	};

	struct CoreExport CreateBot : Events
	{
		static constexpr const char *NAME = "createbot";

		using Events::Events;

		/** Called when a bot is created or destroyed
		 */
		virtual void OnCreateBot(ServiceBot *bi) anope_abstract;
	};

	struct CoreExport DelBot : Events
	{
		static constexpr const char *NAME = "delbot";

		using Events::Events;
		
		virtual void OnDelBot(ServiceBot *bi) anope_abstract;
	};

	struct CoreExport PrePartChannel : Events
	{
		static constexpr const char *NAME = "prepartchannel";

		using Events::Events;

		/** Called before a user parts a channel
		 * @param u The user
		 * @param c The channel
		 */
		virtual void OnPrePartChannel(User *u, Channel *c) anope_abstract;
	};

	struct CoreExport PartChannel : Events
	{
		static constexpr const char *NAME = "partchannel";

		using Events::Events;

		/** Called when a user parts a channel
		 * @param u The user
		 * @param c The channel, may be NULL if the channel no longer exists
		 * @param channel The channel name
		 * @param msg The part reason
		 */
		virtual void OnPartChannel(User *u, Channel *c, const Anope::string &channel, const Anope::string &msg) anope_abstract;
	};

	struct CoreExport LeaveChannel : Events
	{
		static constexpr const char *NAME = "leavechannel";

		using Events::Events;

		/** Called when a user leaves a channel.
		 * From either parting, being kicked, or quitting/killed!
		 * @param u The user
		 * @param c The channel
		 */
		virtual void OnLeaveChannel(User *u, Channel *c) anope_abstract;
	};

	struct CoreExport JoinChannel : Events
	{
		static constexpr const char *NAME = "joinchannel";

		using Events::Events;

		/** Called after a user joins a channel
		 * If this event triggers the user is allowed to be in the channel, and will
		 * not be kicked for restricted/akick/forbidden, etc. If you want to kick the user,
		 * use the CheckKick event instead.
		 * @param u The user
		 * @param channel The channel
		 */
		virtual void OnJoinChannel(User *u, Channel *c) anope_abstract;
	};

	struct CoreExport TopicUpdated : Events
	{
		static constexpr const char *NAME = "topicupdated";

		using Events::Events;

		/** Called when a new topic is set
		 * @param source
		 * @param c The channel
		 * @param setter The user who set the new topic
		 * @param topic The new topic
		 */
		virtual void OnTopicUpdated(User *source, Channel *c, const Anope::string &user, const Anope::string &topic) anope_abstract;
	};

	struct CoreExport PreServerConnect : Events
	{
		static constexpr const char *NAME = "preserverconnect";

		using Events::Events;

		/** Called before Anope connecs to its uplink
		 */
		virtual void OnPreServerConnect() anope_abstract;
	};

	struct CoreExport ServerConnect : Events
	{
		static constexpr const char *NAME = "serverconnect";

		using Events::Events;

		/** Called when Anope connects to its uplink
		 */
		virtual void OnServerConnect() anope_abstract;
	};

	struct CoreExport PreUplinkSync : Events
	{
		static constexpr const char *NAME = "preuplinksync";

		using Events::Events;

		/** Called when we are almost done synching with the uplink, just before we send the EOB
		 */
		virtual void OnPreUplinkSync(Server *serv) anope_abstract;
	};

	struct CoreExport ServerDisconnect : Events
	{
		static constexpr const char *NAME = "serverdisconnect";

		using Events::Events;

		/** Called when Anope disconnects from its uplink, before it tries to reconnect
		 */
		virtual void OnServerDisconnect() anope_abstract;
	};

	struct CoreExport Restart : Events
	{
		static constexpr const char *NAME = "restart";

		using Events::Events;

		/** Called when services restart
		*/
		virtual void OnRestart() anope_abstract;
	};

	struct CoreExport Shutdown : Events
	{
		static constexpr const char *NAME = "shutdown";

		using Events::Events;

		/** Called when services shutdown
		 */
		virtual void OnShutdown() anope_abstract;
	};

	struct CoreExport AddXLine : Events
	{
		static constexpr const char *NAME = "addxline";

		using Events::Events;

		/** Called before a XLine is added
		 * @param source The source of the XLine
		 * @param x The XLine
		 * @param xlm The xline manager it was added to
		 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
		 */
		virtual EventReturn OnAddXLine(CommandSource &source, const XLine *x, XLineManager *xlm) anope_abstract;
	};

	struct CoreExport DelXLine : Events
	{
		static constexpr const char *NAME = "delxline";

		using Events::Events;

		/** Called before a XLine is deleted
		 * @param source The source of the XLine
		 * @param x The XLine
		 * @param xlm The xline manager it was deleted from
		 */
		virtual void OnDelXLine(CommandSource &source, const XLine *x, XLineManager *xlm) anope_abstract;
	};

	struct CoreExport IsServicesOperEvent : Events
	{
		static constexpr const char *NAME = "isservicesoper";

		using Events::Events;

		/** Called when a user is checked for whether they are a services oper
		 * @param u The user
		 * @return EVENT_ALLOW to allow, anything else to deny
		 */
		virtual EventReturn IsServicesOper(User *u) anope_abstract;
	};

	struct CoreExport ServerQuit : Events
	{
		static constexpr const char *NAME = "serverquit";

		using Events::Events;

		/** Called when a server quits
		 * @param server The server
		 */
		virtual void OnServerQuit(Server *server) anope_abstract;
	};

	struct CoreExport UserQuit : Events
	{
		static constexpr const char *NAME = "userquit";

		using Events::Events;

		/** Called when a user quits, or is killed
		 * @param u The user
		 * @param msg The quit message
		 */
		virtual void OnUserQuit(User *u, const Anope::string &msg) anope_abstract;
	};

	struct CoreExport PreUserLogoff : Events
	{
		static constexpr const char *NAME = "preuserlogoff";

		using Events::Events;

		/** Called when a user is quit, before and after being internally removed from
		 * This is different from OnUserQuit, which takes place at the time of the quit.
		 * This happens shortly after when all message processing is finished.
		 * all lists (channels, user list, etc)
		 * @param u The user
		 */
		virtual void OnPreUserLogoff(User *u) anope_abstract;
	};

	struct CoreExport PostUserLogoff : Events
	{
		static constexpr const char *NAME = "postuserlogoff";

		using Events::Events;

		virtual void OnPostUserLogoff(User *u) anope_abstract;
	};

	struct CoreExport AccessDel : Events
	{
		static constexpr const char *NAME = "accessdel";

		using Events::Events;

		/** Called after an access entry is deleted from a channel
		 * @param ci The channel
		 * @param source The source of the command
		 * @param access The access entry that was removed
		 */
		virtual void OnAccessDel(ChanServ::Channel *ci, CommandSource &source, ChanServ::ChanAccess *access) anope_abstract;
	};

	struct CoreExport AccessAdd : Events
	{
		static constexpr const char *NAME = "accessadd";

		using Events::Events;

		/** Called when access is added
		 * @param ci The channel
		 * @param source The source of the command
		 * @param access The access changed
		 */
		virtual void OnAccessAdd(ChanServ::Channel *ci, CommandSource &source, ChanServ::ChanAccess *access) anope_abstract;
	};

	struct CoreExport AccessClear : Events
	{
		static constexpr const char *NAME = "accessclear";

		using Events::Events;
		
		/** Called when the access list is cleared
		 * @param ci The channel
		 * @param u The user who cleared the access
		 */
		virtual void OnAccessClear(ChanServ::Channel *ci, CommandSource &source) anope_abstract;
	};

	struct CoreExport ChanRegistered : Events
	{
		static constexpr const char *NAME = "chanregistered";

		using Events::Events;

		/** Called when a channel is registered
		 * @param ci The channel
		 */
		virtual void OnChanRegistered(ChanServ::Channel *ci) anope_abstract;
	};

	struct CoreExport DelChan : Events
	{
		static constexpr const char *NAME = "delchan";

		using Events::Events;

		/** Called when a channel is being deleted, for any reason
		 * @param ci The channel
		 */
		virtual void OnDelChan(ChanServ::Channel *ci) anope_abstract;
	};

	struct CoreExport ChannelCreate : Events
	{
		static constexpr const char *NAME = "channelcreate";

		using Events::Events;

		/** Called when a new channel is created
		 * Note that this channel may not be introduced to the uplink at this point.
		 * @param c The channel
		 */
		virtual void OnChannelCreate(Channel *c) anope_abstract;
	};

	struct CoreExport ChannelDelete : Events
	{
		static constexpr const char *NAME = "channeldelete";

		using Events::Events;

		/** Called when a channel is deleted
		 * @param c The channel
		 */
		virtual void OnChannelDelete(Channel *c) anope_abstract;
	};

	struct CoreExport CheckKick : Events
	{
		static constexpr const char *NAME = "checkkick";

		using Events::Events;

		/** Called after a user join a channel when we decide whether to kick them or not
		 * @param u The user
		 * @param c The channel
		 * @param kick Set to true to kick
		 * @param mask The mask to ban, if any
		 * @param reason The reason for the kick
		 * @return EVENT_STOP to prevent the user from joining by kicking/banning the user
		 */
		virtual EventReturn OnCheckKick(User *u, Channel *c, Anope::string &mask, Anope::string &reason) anope_abstract;
	};

	struct CoreExport CheckPriv : Events
	{
		static constexpr const char *NAME = "checkpriv";

		using Events::Events;

		/** Checks if access has the channel privilege 'priv'.
		 * @param access THe access struct
		 * @param priv The privilege being checked for
		 * @return EVENT_ALLOW for yes, EVENT_STOP to stop all processing
		 */
		virtual EventReturn OnCheckPriv(const ChanServ::ChanAccess *access, const Anope::string &priv) anope_abstract;
	};

	struct CoreExport GroupCheckPriv : Events
	{
		static constexpr const char *NAME = "groupcheckpriv";

		using Events::Events;

		/** Check whether an access group has a privilege
		 * @param group The group
		 * @param priv The privilege
		 * @return MOD_ALLOW to allow, MOD_STOP to stop
		 */
		virtual EventReturn OnGroupCheckPriv(const ChanServ::AccessGroup *group, const Anope::string &priv) anope_abstract;
	};

	struct CoreExport NickIdentify : Events
	{
		static constexpr const char *NAME = "nickidentify";

		using Events::Events;

		/** Called when a user identifies to a nick
		 * @param u The user
		 */
		virtual void OnNickIdentify(User *u) anope_abstract;
	};

	struct CoreExport UserLogin : Events
	{
		static constexpr const char *NAME = "userlogin";

		using Events::Events;

		/** Called when a user is logged into an account
		 * @param u The user
		 */
		virtual void OnUserLogin(User *u) anope_abstract;
	};

	struct CoreExport NickLogout : Events
	{
		static constexpr const char *NAME = "nicklogout";

		using Events::Events;

		/** Called when a nick logs out
		 * @param u The nick
		 */
		virtual void OnNickLogout(User *u) anope_abstract;
	};

	struct CoreExport DelNick : Events
	{
		static constexpr const char *NAME = "delnick";

		using Events::Events;

		/** Called on delnick()
		 * @param na pointer to the nickalias
		 */
		virtual void OnDelNick(NickServ::Nick *na) anope_abstract;
	};

	struct CoreExport DelCore : Events
	{
		static constexpr const char *NAME = "delcore";

		using Events::Events;

		/** Called on delcore()
		 * @param nc pointer to the NickServ::Account
		 */
		virtual void OnDelCore(NickServ::Account *nc) anope_abstract;
	};

	struct CoreExport ChangeCoreDisplay : Events
	{
		static constexpr const char *NAME = "changecoredisplay";

		using Events::Events;

		/** Called on change_core_display()
		 * @param nc pointer to the NickServ::Account
		 * @param newdisplay the new display
		 */
		virtual void OnChangeCoreDisplay(NickServ::Account *nc, const Anope::string &newdisplay) anope_abstract;
	};

	struct CoreExport NickClearAccess : Events
	{
		static constexpr const char *NAME = "nickclearaccess";

		using Events::Events;

		/** called from NickServ::Account::ClearAccess()
		 * @param nc pointer to the NickServ::Account
		 */
		virtual void OnNickClearAccess(NickServ::Account *nc) anope_abstract;
	};

	struct CoreExport NickAddAccess : Events
	{
		static constexpr const char *NAME = "nickaddaccess";

		using Events::Events;

		/** Called when a user adds an entry to their access list
		 * @param nc The nick
		 * @param entry The entry
		 */
		virtual void OnNickAddAccess(NickServ::Account *nc, const Anope::string &entry) anope_abstract;
	};

	struct CoreExport NickEraseAccess : Events
	{
		static constexpr const char *NAME = "nickeraseaccess";

		using Events::Events;

		/** Called from NickServ::Account::EraseAccess()
		 * @param nc pointer to the NickServ::Account
		 * @param entry The access mask
		 */
		virtual void OnNickEraseAccess(NickServ::Account *nc, const Anope::string &entry) anope_abstract;
	};

	struct CoreExport CheckAuthentication : Events
	{
		static constexpr const char *NAME = "checkauthentication";

		using Events::Events;

		/** Check whether a username and password is correct
		 * @param u The user trying to identify, if applicable.
		 * @param req The login request
		 */
		virtual void OnCheckAuthentication(User *u, NickServ::IdentifyRequest *req) anope_abstract;
	};

	struct CoreExport Fingerprint : Events
	{
		static constexpr const char *NAME = "fingerprint";

		using Events::Events;

		/** Called when we get informed about a users SSL fingerprint
		 *  when we call this, the fingerprint should already be stored in the user struct
		 * @param u pointer to the user
		 */
		virtual void OnFingerprint(User *u) anope_abstract;
	};

	struct CoreExport UserAway : Events
	{
		static constexpr const char *NAME = "useraway";

		using Events::Events;

		/** Called when a user becomes (un)away
		 * @param message The message, is .empty() if unaway
		 */
		virtual void OnUserAway(User *u, const Anope::string &message) anope_abstract;
	};

	struct CoreExport Invite : Events
	{
		static constexpr const char *NAME = "invite";

		using Events::Events;

		/** Called when a user invites one of our users to a channel
		 * @param source The user doing the inviting
		 * @param c The channel the user is inviting to
		 * @param targ The user being invited
		 */
		virtual void OnInvite(User *source, Channel *c, User *targ) anope_abstract;
	};

	struct CoreExport SetVhost : Events
	{
		static constexpr const char *NAME = "setvhost";

		using Events::Events;

		/** Called when a vhost is set
		 * @param na The nickalias of the vhost
		 */
		virtual void OnSetVhost(NickServ::Nick *na) anope_abstract;
	};

	struct CoreExport SetDisplayedHost : Events
	{
		static constexpr const char *NAME = "setdisplayedhost";

		using Events::Events;

		/** Called when a users host changes
		 * @param u The user
		 */
		virtual void OnSetDisplayedHost(User *) anope_abstract;
	};

	struct CoreExport ChannelModeSet : Events
	{
		static constexpr const char *NAME = "channelmodeset";

		using Events::Events;

		/** Called when a mode is set on a channel
		 * @param c The channel
		 * @param setter The user or server that is setting the mode
		 * @param mode The mode
		 * @param param The mode param, if there is one
		 * @return EVENT_STOP to make mlock/secureops etc checks not happen
		 */
		virtual EventReturn OnChannelModeSet(Channel *c, const MessageSource &setter, ChannelMode *mode, const Anope::string &param) anope_abstract;
	};

	struct CoreExport ChannelModeUnset : Events
	{
		static constexpr const char *NAME = "channelmodeunset";

		using Events::Events;

		/** Called when a mode is unset on a channel
		 * @param c The channel
		 * @param setter The user or server that is unsetting the mode
		 * @param mode The mode
		 * @param param The mode param, if there is one
		 * @return EVENT_STOP to make mlock/secureops etc checks not happen
		 */
		virtual EventReturn OnChannelModeUnset(Channel *c, const MessageSource &setter, ChannelMode *mode, const Anope::string &param) anope_abstract;
	};

	struct CoreExport UserModeSet : Events
	{
		static constexpr const char *NAME = "usermodeset";

		using Events::Events;

		/** Called when a mode is set on a user
		 * @param setter who/what is setting the mode
		 * @param u The user
		 * @param mname The mode name
		 */
		virtual void OnUserModeSet(const MessageSource &setter, User *u, const Anope::string &mname) anope_abstract;
	};

	struct CoreExport UserModeUnset : Events
	{
		static constexpr const char *NAME = "usermodeunset";

		using Events::Events;

		/** Called when a mode is unset from a user
		 * @param setter who/what is setting the mode
		 * @param u The user
		 * @param mname The mode name
		 */
		virtual void OnUserModeUnset(const MessageSource &setter, User *u, const Anope::string &mname) anope_abstract;
	};

	struct CoreExport ChannelModeAdd : Events
	{
		static constexpr const char *NAME = "channelmodeadd";

		using Events::Events;

		/** Called when a channel mode is introducted into Anope
		 * @param cm The mode
		 */
		virtual void OnChannelModeAdd(ChannelMode *cm) anope_abstract;
	};

	struct CoreExport UserModeAdd : Events
	{
		static constexpr const char *NAME = "usermodeadd";

		using Events::Events;

		/** Called when a user mode is introducted into Anope
		 * @param um The mode
		 */
		virtual void OnUserModeAdd(UserMode *um) anope_abstract;
	};

	struct CoreExport ModuleLoad : Events
	{
		static constexpr const char *NAME = "moduleload";

		using Events::Events;

		/** Called after a module is loaded
		 * @param u The user loading the module, can be NULL
		 * @param m The module
		 */
		virtual void OnModuleLoad(User *u, Module *m) anope_abstract;
	};

	struct CoreExport ModuleUnload : Events
	{
		static constexpr const char *NAME = "moduleunload";

		using Events::Events;

		/** Called before a module is unloaded
		 * @param u The user, can be NULL
		 * @param m The module
		 */
		virtual void OnModuleUnload(User *u, Module *m) anope_abstract;
	};

	struct CoreExport ServerSync : Events
	{
		static constexpr const char *NAME = "serversync";

		using Events::Events;

		/** Called when a server is synced
		 * @param s The server, can be our uplink server
		 */
		virtual void OnServerSync(Server *s) anope_abstract;
	};

	struct CoreExport UplinkSync : Events
	{
		static constexpr const char *NAME = "uplinksync";

		using Events::Events;

		/** Called when we sync with our uplink
		 * @param s Our uplink
		 */
		virtual void OnUplinkSync(Server *s) anope_abstract;
	};

	struct CoreExport BotPrivmsg : Events
	{
		static constexpr const char *NAME = "botprivmsg";

		using Events::Events;

		/** Called when we receive a PRIVMSG for one of our clients
		 * @param u The user sending the PRIVMSG
		 * @param bi The target of the PRIVMSG
		 * @param message The message
		 * @return EVENT_STOP to halt processing
		 */
		virtual EventReturn OnBotPrivmsg(User *u, ServiceBot *bi, Anope::string &message) anope_abstract;
	};

	struct CoreExport BotNotice : Events
	{
		static constexpr const char *NAME = "botnotice";

		using Events::Events;

		/** Called when we receive a NOTICE for one of our clients
		 * @param u The user sending the NOTICE
		 * @param bi The target of the NOTICE
		 * @param message The message
		 */
		virtual void OnBotNotice(User *u, ServiceBot *bi, Anope::string &message) anope_abstract;
	};

	struct CoreExport Privmsg : Events
	{
		static constexpr const char *NAME = "privmsg";

		using Events::Events;

		/** Called when we receive a PRIVMSG for a registered channel we are in
		 * @param u The source of the message
		 * @param c The channel
		 * @param msg The message
		 */
		virtual void OnPrivmsg(User *u, Channel *c, Anope::string &msg) anope_abstract;
	};

	struct CoreExport Log : Events
	{
		static constexpr const char *NAME = "log";

		using Events::Events;

		/** Called when a message is logged
		 * @param l The log message
		 */
		virtual void OnLog(Logger *l) anope_abstract;
	};

	struct CoreExport LogMessage : Events
	{
		static constexpr const char *NAME = "logmessage";

		using Events::Events;

		/** Called when a log message is actually logged to a given log info
		 * The message has already passed validation checks by the LogInfo
		 * @param li The loginfo whee the message is being logged
		 * @param l The log message
		 * @param msg The final formatted message, derived from 'l'
		 */
		virtual void OnLogMessage(LogInfo *li, const Logger *l, const Anope::string &msg) anope_abstract;
	};

	struct CoreExport CheckModes : Events
	{
		static constexpr const char *NAME = "checkmodes";

		using Events::Events;

		/** Called when a channels modes are being checked to see if they are allowed,
		 * mostly to ensure mlock/+r are set.
		 * @param c The channel
		 */
		virtual void OnCheckModes(Reference<Channel> &c) anope_abstract;
	};

	struct CoreExport ChannelSync : Events
	{
		static constexpr const char *NAME = "channelsync";

		using Events::Events;

		/** Called when a channel is synced.
		 * Channels are synced after a sjoin is finished processing
		 * for a newly created channel to set the correct modes, topic,
		 * set.
		 */
		virtual void OnChannelSync(Channel *c) anope_abstract;
	};

	struct CoreExport SetCorrectModes : Events
	{
		static constexpr const char *NAME = "setcorrectmodes";

		using Events::Events;

		/** Called to set the correct modes on the user on the given channel
		 * @param user The user
		 * @param chan The channel
		 * @param access The user's access on the channel
		 * @param give_modes If giving modes is desired
		 * @param take_modes If taking modes is desired
		 */
		virtual void OnSetCorrectModes(User *user, Channel *chan, ChanServ::AccessGroup &access, bool &give_modes, bool &take_modes) anope_abstract;
	};

	struct CoreExport Message : Events
	{
		static constexpr const char *NAME = "message";

		using Events::Events;

		/** Called whenever a message is received from the uplink
		 * @param source The source of the message
		 * @param command The command being executed
		 * @param params Parameters
		 * @return EVENT_STOP to prevent the protocol module from processing this message
		 */
		virtual EventReturn OnMessage(MessageSource &source, Anope::string &command, std::vector<Anope::string> &param) anope_abstract;
	};

	struct CoreExport CanSet : Events
	{
		static constexpr const char *NAME = "canset";

		using Events::Events;

		/** Called to determine if a chnanel mode can be set by a user
		 * @param u The user
		 * @param cm The mode
		 */
		virtual EventReturn OnCanSet(User *u, const ChannelMode *cm) anope_abstract;
	};

	struct CoreExport CheckDelete : Events
	{
		static constexpr const char *NAME = "checkdelete";

		using Events::Events;

		virtual EventReturn OnCheckDelete(Channel *) anope_abstract;
	};

	struct CoreExport ExpireTick : Events
	{
		static constexpr const char *NAME = "expiretick";

		using Events::Events;

		/** Called every options:expiretimeout seconds. Should be used to expire nicks,
		 * channels, etc.
		 */
		virtual void OnExpireTick() anope_abstract;
	};

	struct CoreExport SerializeEvents : Events
	{
		static constexpr const char *NAME = "serialize";

		using Events::Events;
		
		virtual EventReturn OnSerializeList(Serialize::TypeBase *type, std::vector<Serialize::ID> &ids) anope_abstract;

		virtual EventReturn OnSerializeFind(Serialize::TypeBase *type, Serialize::FieldBase *field, const Anope::string &value, Serialize::ID &id) anope_abstract;

		virtual EventReturn OnSerializeGet(Serialize::Object *object, Serialize::FieldBase *field, Anope::string &value) anope_abstract;

		virtual EventReturn OnSerializeGetSerializable(Serialize::Object *object, Serialize::FieldBase *field, Anope::string &type, Serialize::ID &id) anope_abstract;

		virtual EventReturn OnSerializeGetRefs(Serialize::Object *object, Serialize::TypeBase *type, std::vector<Serialize::Edge> &) anope_abstract;

		virtual EventReturn OnSerializeDeref(Serialize::ID value, Serialize::TypeBase *type) anope_abstract;

		virtual EventReturn OnSerializableGetId(Serialize::ID &id) anope_abstract;

		virtual void OnSerializableDelete(Serialize::Object *) anope_abstract;

		virtual EventReturn OnSerializeSet(Serialize::Object *object, Serialize::FieldBase *field, const Anope::string &value) anope_abstract;

		virtual EventReturn OnSerializeSetSerializable(Serialize::Object *object, Serialize::FieldBase *field, Serialize::Object *value) anope_abstract;

		virtual EventReturn OnSerializeUnset(Serialize::Object *object, Serialize::FieldBase *field) anope_abstract;

		virtual EventReturn OnSerializeUnsetSerializable(Serialize::Object *object, Serialize::FieldBase *field) anope_abstract;

		virtual EventReturn OnSerializeHasField(Serialize::Object *object, Serialize::FieldBase *field) anope_abstract;

		virtual void OnSerializableCreate(Serialize::Object *) anope_abstract;
	};
}

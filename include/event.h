/*
 *
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#pragma once

#include "service.h"
#include "modules.h"

struct Events
{
	virtual ~Events() { }
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
class EventHandlers : public Service
{
	std::vector<EventHandler *> handlers;
	friend class EventHook<EventHandler>;

 public:
	EventHandlers(Module *o, const Anope::string &ename) : Service(o, "EventHandlers", ename)
	{
	}

	template<typename Func, typename... Args>
	auto operator()(Func func, Args&&... args) -> decltype(((static_cast<EventHandler*>(nullptr))->*func)(args...))
	{
		return Caller<EventHandler, Func, decltype(((static_cast<EventHandler*>(nullptr))->*func)(args...)), Args...>()(this->handlers, func, std::forward<Args>(args)...);
	}
};

template<typename EventHandler>
class EventHandlersReference
{
	ServiceReference<EventHandlers<EventHandler>> ref;
 public:
	EventHandlersReference(const Anope::string &name) : ref("EventHandlers", name) { }

	explicit operator bool()
	{
		return !!ref;
	}

	template<typename Func, typename... Args>
	auto operator()(Func func, Args&&... args) -> decltype(((static_cast<EventHandler*>(nullptr))->*func)(args...))
	{
		return (**ref)(func, std::forward<Args>(args)...);
	}
};

template<typename EventHandler>
class EventHook : public EventHandler
{

	struct ServiceReferenceListener : ServiceReference<EventHandlers<EventHandler>>
	{
		EventHook<EventHandler> *eh;

		ServiceReferenceListener(EventHook<EventHandler> *t, const Anope::string &sname) : ServiceReference<EventHandlers<EventHandler>>("EventHandlers", sname), eh(t)
		{
		}

		void OnAcquire() override
		{
			if (std::find((*this)->handlers.begin(), (*this)->handlers.end(), eh) == (*this)->handlers.end())
			{
				if (eh->priority == Priority::LAST)
					(*this)->handlers.push_back(eh);
				else
					(*this)->handlers.insert((*this)->handlers.begin(), eh);
			}
		}
	} handlers;

 public:
	enum class Priority
	{
		FIRST,
		LAST
	}
	priority;

	EventHook(const Anope::string &name, Priority p = Priority::LAST) : handlers(this, name)
	{
		handlers.Check();
	}

	~EventHook()
	{
		if (!handlers)
			return;

		auto it = std::find(handlers->handlers.begin(), handlers->handlers.end(), this);
		if (it != handlers->handlers.end())
			handlers->handlers.erase(it);
	}
};

namespace Event
{
	struct CoreExport PreUserKicked : Events
	{
		/** Called before a user has been kicked from a channel.
		 * @param source The kicker
		 * @param cu The user, channel, and status of the user being kicked
		 * @param kickmsg The reason for the kick.
		 */
		virtual void OnPreUserKicked(const MessageSource &source, ChanUserContainer *cu, const Anope::string &kickmsg) anope_abstract;
	};
	extern CoreExport EventHandlers<PreUserKicked> OnPreUserKicked;

	struct CoreExport UserKicked : Events
	{
		/** Called when a user has been kicked from a channel.
		 * @param source The kicker
		 * @param target The user being kicked
		 * @param channel The channel the user was kicked from, which may no longer exist
		 * @param status The status the kicked user had on the channel before they were kicked
		 * @param kickmsg The reason for the kick.
		 */
		virtual void OnUserKicked(const MessageSource &source, User *target, const Anope::string &channel, ChannelStatus &status, const Anope::string &kickmsg) anope_abstract;
	};
	extern CoreExport EventHandlers<UserKicked> OnUserKicked;

	struct CoreExport PreBotAssign : Events
	{
		/** Called before a bot is assigned to a channel.
		 * @param sender The user assigning the bot
		 * @param ci The channel the bot is to be assigned to.
		 * @param bi The bot being assigned.
		 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to deny the assign.
		 */
		virtual EventReturn OnPreBotAssign(User *sender, ChannelInfo *ci, BotInfo *bi) anope_abstract;
	};
	extern CoreExport EventHandlers<PreBotAssign> OnPreBotAssign;

	struct CoreExport BotAssign : Events
	{
		/** Called when a bot is assigned ot a channel
		 */
		virtual void OnBotAssign(User *sender, ChannelInfo *ci, BotInfo *bi) anope_abstract;
	};
	extern CoreExport EventHandlers<BotAssign> OnBotAssign;

	struct CoreExport BotUnAssign : Events
	{
		/** Called before a bot is unassigned from a channel.
		 * @param sender The user unassigning the bot
		 * @param ci The channel the bot is being removed from
		 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to deny the unassign.
		 */
		virtual EventReturn OnBotUnAssign(User *sender, ChannelInfo *ci) anope_abstract;
	};
	extern CoreExport EventHandlers<BotUnAssign> OnBotUnAssign;

	struct CoreExport UserConnect : Events
	{
		/** Called when a new user connects to the network.
		 * @param u The connecting user.
		 * @param exempt set to true/is true if the user should be excepted from bans etc
		 */
		virtual void OnUserConnect(User *u, bool &exempt) anope_abstract;
	};
	extern CoreExport EventHandlers<UserConnect> OnUserConnect;

	struct CoreExport NewServer : Events
	{
		/** Called when a new server connects to the network.
		 * @param s The server that has connected to the network
		 */
		virtual void OnNewServer(Server *s) anope_abstract;
	};
	extern CoreExport EventHandlers<NewServer> OnNewServer;

	struct CoreExport UserNickChange : Events
	{
		/** Called after a user changed the nick
		 * @param u The user.
		 * @param oldnick The old nick of the user
		 */
		virtual void OnUserNickChange(User *u, const Anope::string &oldnick) anope_abstract;
	};
	extern CoreExport EventHandlers<UserNickChange> OnUserNickChange;


	struct CoreExport PreCommand : Events
	{
		/** Called before a command is due to be executed.
		 * @param source The source of the command
		 * @param command The command the user is executing
		 * @param params The parameters the user is sending
		 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
		 */
		virtual EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) anope_abstract;
	};
	extern CoreExport EventHandlers<PreCommand> OnPreCommand;

	struct CoreExport PostCommand : Events
	{
		/** Called after a command has been executed.
		 * @param source The source of the command
		 * @param command The command the user executed
		 * @param params The parameters the user sent
		 */
		virtual void OnPostCommand(CommandSource &source, Command *command, const std::vector<Anope::string> &params) anope_abstract;
	};
	extern CoreExport EventHandlers<PostCommand> OnPostCommand;

	struct CoreExport SaveDatabase : Events
	{
		/** Called when the databases are saved
		 */
		virtual void OnSaveDatabase() anope_abstract;
	};
	extern CoreExport EventHandlers<SaveDatabase> OnSaveDatabase;

	struct CoreExport LoadDatabase : Events
	{
		/** Called when the databases are loaded
		 * @return EVENT_CONTINUE to let other modules continue loading, EVENT_STOP to stop
		 */
		virtual EventReturn OnLoadDatabase() anope_abstract;
	};
	extern CoreExport EventHandlers<LoadDatabase> OnLoadDatabase;

	struct CoreExport Encrypt : Events
	{
		/** Called when anope needs to check passwords against encryption
		 *  see src/encrypt.c for detailed informations
		 */
		virtual EventReturn OnEncrypt(const Anope::string &src, Anope::string &dest) anope_abstract;
	};
	extern CoreExport EventHandlers<Encrypt> OnEncrypt;

	struct CoreExport Decrypt : Events
	{
		virtual EventReturn OnDecrypt(const Anope::string &hashm, const Anope::string &src, Anope::string &dest) anope_abstract;
	};
	extern CoreExport EventHandlers<Decrypt> OnDecrypt;



	struct CoreExport CreateBot : Events
	{
		/** Called when a bot is created or destroyed
		 */
		virtual void OnCreateBot(BotInfo *bi) anope_abstract;
	};
	extern CoreExport EventHandlers<CreateBot> OnCreateBot;

	struct CoreExport DelBot : Events
	{
		virtual void OnDelBot(BotInfo *bi) anope_abstract;
	};
	extern CoreExport EventHandlers<DelBot> OnDelBot;

	struct CoreExport BotKick : Events
	{
		/** Called before a bot kicks a user
		 * @param bi The bot sending the kick
		 * @param c The channel the user is being kicked on
		 * @param u The user being kicked
		 * @param reason The reason
		 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
		 */
		virtual EventReturn OnBotKick(BotInfo *bi, Channel *c, User *u, const Anope::string &reason) anope_abstract;
	};
	extern CoreExport EventHandlers<BotKick> OnBotKick;

	struct CoreExport PrePartChannel : Events
	{
		/** Called before a user parts a channel
		 * @param u The user
		 * @param c The channel
		 */
		virtual void OnPrePartChannel(User *u, Channel *c) anope_abstract;
	};
	extern CoreExport EventHandlers<PrePartChannel> OnPrePartChannel;

	struct CoreExport PartChannel : Events
	{
		/** Called when a user parts a channel
		 * @param u The user
		 * @param c The channel, may be NULL if the channel no longer exists
		 * @param channel The channel name
		 * @param msg The part reason
		 */
		virtual void OnPartChannel(User *u, Channel *c, const Anope::string &channel, const Anope::string &msg) anope_abstract;
	};
	extern CoreExport EventHandlers<PartChannel> OnPartChannel;

	struct CoreExport LeaveChannel : Events
	{
		/** Called when a user leaves a channel.
		 * From either parting, being kicked, or quitting/killed!
		 * @param u The user
		 * @param c The channel
		 */
		virtual void OnLeaveChannel(User *u, Channel *c) anope_abstract;
	};
	extern CoreExport EventHandlers<LeaveChannel> OnLeaveChannel;

	struct CoreExport JoinChannel : Events
	{
		/** Called after a user joins a channel
		 * If this event triggers the user is allowed to be in the channel, and will
		 * not be kicked for restricted/akick/forbidden, etc. If you want to kick the user,
		 * use the CheckKick event instead.
		 * @param u The user
		 * @param channel The channel
		 */
		virtual void OnJoinChannel(User *u, Channel *c) anope_abstract;
	};
	extern CoreExport EventHandlers<JoinChannel> OnJoinChannel;

	struct CoreExport TopicUpdated : Events
	{
		/** Called when a new topic is set
		 * @param c The channel
		 * @param setter The user who set the new topic
		 * @param topic The new topic
		 */
		virtual void OnTopicUpdated(Channel *c, const Anope::string &user, const Anope::string &topic) anope_abstract;
	};
	extern CoreExport EventHandlers<TopicUpdated> OnTopicUpdated;

	struct CoreExport PreServerConnect : Events
	{
		/** Called before Anope connecs to its uplink
		 */
		virtual void OnPreServerConnect() anope_abstract;
	};
	extern CoreExport EventHandlers<PreServerConnect> OnPreServerConnect;

	struct CoreExport ServerConnect : Events
	{
		/** Called when Anope connects to its uplink
		 */
		virtual void OnServerConnect() anope_abstract;
	};
	extern CoreExport EventHandlers<ServerConnect> OnServerConnect;

	struct CoreExport PreUplinkSync : Events
	{
		/** Called when we are almost done synching with the uplink, just before we send the EOB
		 */
		virtual void OnPreUplinkSync(Server *serv) anope_abstract;
	};
	extern CoreExport EventHandlers<PreUplinkSync> OnPreUplinkSync;

	struct CoreExport ServerDisconnect : Events
	{
		/** Called when Anope disconnects from its uplink, before it tries to reconnect
		 */
		virtual void OnServerDisconnect() anope_abstract;
	};
	extern CoreExport EventHandlers<ServerDisconnect> OnServerDisconnect;

	struct CoreExport Restart : Events
	{
		/** Called when services restart
		*/
		virtual void OnRestart() anope_abstract;
	};
	extern CoreExport EventHandlers<Restart> OnRestart;

	struct CoreExport Shutdown : Events
	{
		/** Called when services shutdown
		 */
		virtual void OnShutdown() anope_abstract;
	};
	extern CoreExport EventHandlers<Shutdown> OnShutdown;

	struct CoreExport AddXLine : Events
	{
		/** Called before a XLine is added
		 * @param source The source of the XLine
		 * @param x The XLine
		 * @param xlm The xline manager it was added to
		 * @return EVENT_CONTINUE to let other modules decide, EVENT_STOP to halt the command and not process it
		 */
		virtual EventReturn OnAddXLine(CommandSource &source, const XLine *x, XLineManager *xlm) anope_abstract;
	};
	extern CoreExport EventHandlers<AddXLine> OnAddXLine;

	struct CoreExport DelXLine : Events
	{
		/** Called before a XLine is deleted
		 * @param source The source of the XLine
		 * @param x The XLine
		 * @param xlm The xline manager it was deleted from
		 */
		virtual void OnDelXLine(CommandSource &source, const XLine *x, XLineManager *xlm) anope_abstract;
	};
	extern CoreExport EventHandlers<DelXLine> OnDelXLine;

	struct CoreExport IsServicesOperEvent : Events
	{
		/** Called when a user is checked for whether they are a services oper
		 * @param u The user
		 * @return EVENT_ALLOW to allow, anything else to deny
		 */
		virtual EventReturn IsServicesOper(User *u) anope_abstract;
	};
	extern CoreExport EventHandlers<IsServicesOperEvent> OnIsServicesOper;

	struct CoreExport ServerQuit : Events
	{
		/** Called when a server quits
		 * @param server The server
		 */
		virtual void OnServerQuit(Server *server) anope_abstract;
	};
	extern CoreExport EventHandlers<ServerQuit> OnServerQuit;

	struct CoreExport UserQuit : Events
	{
		/** Called when a user quits, or is killed
		 * @param u The user
		 * @param msg The quit message
		 */
		virtual void OnUserQuit(User *u, const Anope::string &msg) anope_abstract;
	};
	extern CoreExport EventHandlers<UserQuit> OnUserQuit;

	struct CoreExport PreUserLogoff : Events
	{
		/** Called when a user is quit, before and after being internally removed from
		 * This is different from OnUserQuit, which takes place at the time of the quit.
		 * This happens shortly after when all message processing is finished.
		 * all lists (channels, user list, etc)
		 * @param u The user
		 */
		virtual void OnPreUserLogoff(User *u) anope_abstract;
	};
	extern CoreExport EventHandlers<PreUserLogoff> OnPreUserLogoff;

	struct CoreExport PostUserLogoff : Events
	{
		virtual void OnPostUserLogoff(User *u) anope_abstract;
	};
	extern CoreExport EventHandlers<PostUserLogoff> OnPostUserLogoff;

	struct CoreExport AccessDel : Events
	{
		/** Called after an access entry is deleted from a channel
		 * @param ci The channel
		 * @param source The source of the command
		 * @param access The access entry that was removed
		 */
		virtual void OnAccessDel(ChannelInfo *ci, CommandSource &source, ChanAccess *access) anope_abstract;
	};
	extern CoreExport EventHandlers<AccessDel> OnAccessDel;

	struct CoreExport AccessAdd : Events
	{
		/** Called when access is added
		 * @param ci The channel
		 * @param source The source of the command
		 * @param access The access changed
		 */
		virtual void OnAccessAdd(ChannelInfo *ci, CommandSource &source, ChanAccess *access) anope_abstract;
	};
	extern CoreExport EventHandlers<AccessAdd> OnAccessAdd;

	struct CoreExport AccessClear : Events
	{
		/** Called when the access list is cleared
		 * @param ci The channel
		 * @param u The user who cleared the access
		 */
		virtual void OnAccessClear(ChannelInfo *ci, CommandSource &source) anope_abstract;
	};
	extern CoreExport EventHandlers<AccessClear> OnAccessClear;

	struct CoreExport ChanRegistered : Events
	{
		/** Called when a channel is registered
		 * @param ci The channel
		 */
		virtual void OnChanRegistered(ChannelInfo *ci) anope_abstract;
	};
	extern CoreExport EventHandlers<ChanRegistered> OnChanRegistered;


	struct CoreExport CreateChan : Events
	{
		/** Called when a channel is being created, for any reason
		 * @param ci The channel
		 */
		virtual void OnCreateChan(ChannelInfo *ci) anope_abstract;
	};
	extern CoreExport EventHandlers<CreateChan> OnCreateChan;

	struct CoreExport DelChan : Events
	{
		/** Called when a channel is being deleted, for any reason
		 * @param ci The channel
		 */
		virtual void OnDelChan(ChannelInfo *ci) anope_abstract;
	};
	extern CoreExport EventHandlers<DelChan> OnDelChan;

	struct CoreExport ChannelCreate : Events
	{
		/** Called when a new channel is created
		 * Note that this channel may not be introduced to the uplink at this point.
		 * @param c The channel
		 */
		virtual void OnChannelCreate(Channel *c) anope_abstract;
	};
	extern CoreExport EventHandlers<ChannelCreate> OnChannelCreate;

	struct CoreExport ChannelDelete : Events
	{
		/** Called when a channel is deleted
		 * @param c The channel
		 */
		virtual void OnChannelDelete(Channel *c) anope_abstract;
	};
	extern CoreExport EventHandlers<ChannelDelete> OnChannelDelete;


	struct CoreExport CheckKick : Events
	{
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
	extern CoreExport EventHandlers<CheckKick> OnCheckKick;

	struct CoreExport CheckPriv : Events
	{
		/** Checks if access has the channel privilege 'priv'.
		 * @param access THe access struct
		 * @param priv The privilege being checked for
		 * @return EVENT_ALLOW for yes, EVENT_STOP to stop all processing
		 */
		virtual EventReturn OnCheckPriv(const ChanAccess *access, const Anope::string &priv) anope_abstract;
	};
	extern CoreExport EventHandlers<CheckPriv> OnCheckPriv;

	struct CoreExport GroupCheckPriv : Events
	{
		/** Check whether an access group has a privilege
		 * @param group The group
		 * @param priv The privilege
		 * @return MOD_ALLOW to allow, MOD_STOP to stop
		 */
		virtual EventReturn OnGroupCheckPriv(const AccessGroup *group, const Anope::string &priv) anope_abstract;
	};
	extern CoreExport EventHandlers<GroupCheckPriv> OnGroupCheckPriv;

	struct CoreExport NickIdentify : Events
	{
		/** Called when a user identifies to a nick
		 * @param u The user
		 */
		virtual void OnNickIdentify(User *u) anope_abstract;
	};
	extern CoreExport EventHandlers<NickIdentify> OnNickIdentify;

	struct CoreExport UserLogin : Events
	{
		/** Called when a user is logged into an account
		 * @param u The user
		 */
		virtual void OnUserLogin(User *u) anope_abstract;
	};
	extern CoreExport EventHandlers<UserLogin> OnUserLogin;

	struct CoreExport NickLogout : Events
	{
		/** Called when a nick logs out
		 * @param u The nick
		 */
		virtual void OnNickLogout(User *u) anope_abstract;
	};
	extern CoreExport EventHandlers<NickLogout> OnNickLogout;

	struct CoreExport DelNick : Events
	{
		/** Called on delnick()
		 * @ param na pointer to the nickalias
		 */
		virtual void OnDelNick(NickAlias *na) anope_abstract;
	};
	extern CoreExport EventHandlers<DelNick> OnDelNick;

	struct CoreExport NickCoreCreate : Events
	{
		/** Called when a nickcore is created
		 * @param nc The nickcore
		 */
		virtual void OnNickCoreCreate(NickCore *nc) anope_abstract;
	};
	extern CoreExport EventHandlers<NickCoreCreate> OnNickCoreCreate;

	struct CoreExport DelCore : Events
	{
		/** Called on delcore()
		 * @param nc pointer to the NickCore
		 */
		virtual void OnDelCore(NickCore *nc) anope_abstract;
	};
	extern CoreExport EventHandlers<DelCore> OnDelCore;

	struct CoreExport ChangeCoreDisplay : Events
	{
		/** Called on change_core_display()
		 * @param nc pointer to the NickCore
		 * @param newdisplay the new display
		 */
		virtual void OnChangeCoreDisplay(NickCore *nc, const Anope::string &newdisplay) anope_abstract;
	};
	extern CoreExport EventHandlers<ChangeCoreDisplay> OnChangeCoreDisplay;

	struct CoreExport NickClearAccess : Events
	{
		/** called from NickCore::ClearAccess()
		 * @param nc pointer to the NickCore
		 */
		virtual void OnNickClearAccess(NickCore *nc) anope_abstract;
	};
	extern CoreExport EventHandlers<NickClearAccess> OnNickClearAccess;

	struct CoreExport NickAddAccess : Events
	{
		/** Called when a user adds an entry to their access list
		 * @param nc The nick
		 * @param entry The entry
		 */
		virtual void OnNickAddAccess(NickCore *nc, const Anope::string &entry) anope_abstract;
	};
	extern CoreExport EventHandlers<NickAddAccess> OnNickAddAccess;

	struct CoreExport NickEraseAccess : Events
	{
		/** Called from NickCore::EraseAccess()
		 * @param nc pointer to the NickCore
		 * @param entry The access mask
		 */
		virtual void OnNickEraseAccess(NickCore *nc, const Anope::string &entry) anope_abstract;
	};
	extern CoreExport EventHandlers<NickEraseAccess> OnNickEraseAccess;



	struct CoreExport CheckAuthentication : Events
	{
		/** Check whether a username and password is correct
		 * @param u The user trying to identify, if applicable.
		 * @param req The login request
		 */
		virtual void OnCheckAuthentication(User *u, IdentifyRequest *req) anope_abstract;
	};
	extern CoreExport EventHandlers<CheckAuthentication> OnCheckAuthentication;

	struct CoreExport Fingerprint : Events
	{
		/** Called when we get informed about a users SSL fingerprint
		 *  when we call this, the fingerprint should already be stored in the user struct
		 * @param u pointer to the user
		 */
		virtual void OnFingerprint(User *u) anope_abstract;
	};
	extern CoreExport EventHandlers<Fingerprint> OnFingerprint;

	struct CoreExport UserAway : Events
	{
		/** Called when a user becomes (un)away
		 * @param message The message, is .empty() if unaway
		 */
		virtual void OnUserAway(User *u, const Anope::string &message) anope_abstract;
	};
	extern CoreExport EventHandlers<UserAway> OnUserAway;

	struct CoreExport Invite : Events
	{
		/** Called when a user invites one of our users to a channel
		 * @param source The user doing the inviting
		 * @param c The channel the user is inviting to
		 * @param targ The user being invited
		 */
		virtual void OnInvite(User *source, Channel *c, User *targ) anope_abstract;
	};
	extern CoreExport EventHandlers<Invite> OnInvite;

	struct CoreExport SetVhost : Events
	{
		/** Called when a vhost is set
		 * @param na The nickalias of the vhost
		 */
		virtual void OnSetVhost(NickAlias *na) anope_abstract;
	};
	extern CoreExport EventHandlers<SetVhost> OnSetVhost;

	struct CoreExport SetDisplayedHost : Events
	{
		/** Called when a users host changes
		 * @param u The user
		 */
		virtual void OnSetDisplayedHost(User *) anope_abstract;
	};
	extern CoreExport EventHandlers<SetDisplayedHost> OnSetDisplayedHost;

	struct CoreExport ChannelModeSet : Events
	{
		/** Called when a mode is set on a channel
		 * @param c The channel
		 * @param setter The user or server that is setting the mode
		 * @param mode The mode
		 * @param param The mode param, if there is one
		 * @return EVENT_STOP to make mlock/secureops etc checks not happen
		 */
		virtual EventReturn OnChannelModeSet(Channel *c, MessageSource &setter, ChannelMode *mode, const Anope::string &param) anope_abstract;
	};
	extern CoreExport EventHandlers<ChannelModeSet> OnChannelModeSet;

	struct CoreExport ChannelModeUnset : Events
	{
		/** Called when a mode is unset on a channel
		 * @param c The channel
		 * @param setter The user or server that is unsetting the mode
		 * @param mode The mode
		 * @param param The mode param, if there is one
		 * @return EVENT_STOP to make mlock/secureops etc checks not happen
		 */
		virtual EventReturn OnChannelModeUnset(Channel *c, MessageSource &setter, ChannelMode *mode, const Anope::string &param) anope_abstract;
	};
	extern CoreExport EventHandlers<ChannelModeUnset> OnChannelModeUnset;

	struct CoreExport UserModeSet : Events
	{
		/** Called when a mode is set on a user
		 * @param setter who/what is setting the mode
		 * @param u The user
		 * @param mname The mode name
		 */
		virtual void OnUserModeSet(const MessageSource &setter, User *u, const Anope::string &mname) anope_abstract;
	};
	extern CoreExport EventHandlers<UserModeSet> OnUserModeSet;

	struct CoreExport UserModeUnset : Events
	{
		/** Called when a mode is unset from a user
		 * @param setter who/what is setting the mode
		 * @param u The user
		 * @param mname The mode name
		 */
		virtual void OnUserModeUnset(const MessageSource &setter, User *u, const Anope::string &mname) anope_abstract;
	};
	extern CoreExport EventHandlers<UserModeUnset> OnUserModeUnset;

	struct CoreExport ChannelModeAdd : Events
	{
		/** Called when a channel mode is introducted into Anope
		 * @param cm The mode
		 */
		virtual void OnChannelModeAdd(ChannelMode *cm) anope_abstract;
	};
	extern CoreExport EventHandlers<ChannelModeAdd> OnChannelModeAdd;

	struct CoreExport UserModeAdd : Events
	{
		/** Called when a user mode is introducted into Anope
		 * @param um The mode
		 */
		virtual void OnUserModeAdd(UserMode *um) anope_abstract;
	};
	extern CoreExport EventHandlers<UserModeAdd> OnUserModeAdd;

	struct CoreExport ModuleLoad : Events
	{
		/** Called after a module is loaded
		 * @param u The user loading the module, can be NULL
		 * @param m The module
		 */
		virtual void OnModuleLoad(User *u, Module *m) anope_abstract;
	};
	extern CoreExport EventHandlers<ModuleLoad> OnModuleLoad;

	struct CoreExport ModuleUnload : Events
	{
		/** Called before a module is unloaded
		 * @param u The user, can be NULL
		 * @param m The module
		 */
		virtual void OnModuleUnload(User *u, Module *m) anope_abstract;
	};
	extern CoreExport EventHandlers<ModuleUnload> OnModuleUnload;

	struct CoreExport ServerSync : Events
	{
		/** Called when a server is synced
		 * @param s The server, can be our uplink server
		 */
		virtual void OnServerSync(Server *s) anope_abstract;
	};
	extern CoreExport EventHandlers<ServerSync> OnServerSync;

	struct CoreExport UplinkSync : Events
	{
		/** Called when we sync with our uplink
		 * @param s Our uplink
		 */
		virtual void OnUplinkSync(Server *s) anope_abstract;
	};
	extern CoreExport EventHandlers<UplinkSync> OnUplinkSync;

	struct CoreExport BotPrivmsg : Events
	{
		/** Called when we receive a PRIVMSG for one of our clients
		 * @param u The user sending the PRIVMSG
		 * @param bi The target of the PRIVMSG
		 * @param message The message
		 * @return EVENT_STOP to halt processing
		 */
		virtual EventReturn OnBotPrivmsg(User *u, BotInfo *bi, Anope::string &message) anope_abstract;
	};
	extern CoreExport EventHandlers<BotPrivmsg> OnBotPrivmsg;

	struct CoreExport BotNotice : Events
	{
		/** Called when we receive a NOTICE for one of our clients
		 * @param u The user sending the NOTICE
		 * @param bi The target of the NOTICE
		 * @param message The message
		 */
		virtual void OnBotNotice(User *u, BotInfo *bi, Anope::string &message) anope_abstract;
	};
	extern CoreExport EventHandlers<BotNotice> OnBotNotice;

	struct CoreExport Privmsg : Events
	{
		/** Called when we receive a PRIVMSG for a registered channel we are in
		 * @param u The source of the message
		 * @param c The channel
		 * @param msg The message
		 */
		virtual void OnPrivmsg(User *u, Channel *c, Anope::string &msg) anope_abstract;
	};
	extern CoreExport EventHandlers<Privmsg> OnPrivmsg;

	struct CoreExport Log : Events
	{
		/** Called when a message is logged
		 * @param l The log message
		 */
		virtual void OnLog(::Log *l) anope_abstract;
	};
	extern CoreExport EventHandlers<Log> OnLog;

	struct CoreExport LogMessage : Events
	{
		/** Called when a log message is actually logged to a given log info
		 * The message has already passed validation checks by the LogInfo
		 * @param li The loginfo whee the message is being logged
		 * @param l The log message
		 * @param msg The final formatted message, derived from 'l'
		 */
		virtual void OnLogMessage(LogInfo *li, const ::Log *l, const Anope::string &msg) anope_abstract;
	};
	extern CoreExport EventHandlers<LogMessage> OnLogMessage;

	struct CoreExport CheckModes : Events
	{
		/** Called when a channels modes are being checked to see if they are allowed,
		 * mostly to ensure mlock/+r are set.
		 * @param c The channel
		 */
		virtual void OnCheckModes(Reference<Channel> &c) anope_abstract;
	};
	extern CoreExport EventHandlers<CheckModes> OnCheckModes;

	struct CoreExport ChannelSync : Events
	{
		/** Called when a channel is synced.
		 * Channels are synced after a sjoin is finished processing
		 * for a newly created channel to set the correct modes, topic,
		 * set.
		 */
		virtual void OnChannelSync(Channel *c) anope_abstract;
	};
	extern CoreExport EventHandlers<ChannelSync> OnChannelSync;

	struct CoreExport SetCorrectModes : Events
	{
		/** Called to set the correct modes on the user on the given channel
		 * @param user The user
		 * @param chan The channel
		 * @param access The user's access on the channel
		 * @param give_modes If giving modes is desired
		 * @param take_modes If taking modes is desired
		 */
		virtual void OnSetCorrectModes(User *user, Channel *chan, AccessGroup &access, bool &give_modes, bool &take_modes) anope_abstract;
	};
	extern CoreExport EventHandlers<SetCorrectModes> OnSetCorrectModes;

	struct CoreExport SerializeCheck : Events
	{
		virtual void OnSerializeCheck(Serialize::Type *) anope_abstract;
	};
	extern CoreExport EventHandlers<SerializeCheck> OnSerializeCheck;

	struct CoreExport SerializableConstruct : Events
	{
		virtual void OnSerializableConstruct(Serializable *) anope_abstract;
	};
	extern CoreExport EventHandlers<SerializableConstruct> OnSerializableConstruct;

	struct CoreExport SerializableDestruct : Events
	{
		virtual void OnSerializableDestruct(Serializable *) anope_abstract;
	};
	extern CoreExport EventHandlers<SerializableDestruct> OnSerializableDestruct;

	struct CoreExport SerializableUpdate : Events
	{
		virtual void OnSerializableUpdate(Serializable *) anope_abstract;
	};
	extern CoreExport EventHandlers<SerializableUpdate> OnSerializableUpdate;

	struct CoreExport SerializeTypeCreate : Events
	{
		virtual void OnSerializeTypeCreate(Serialize::Type *) anope_abstract;
	};
	extern CoreExport EventHandlers<SerializeTypeCreate> OnSerializeTypeCreate;

	struct CoreExport Message : Events
	{
		/** Called whenever a message is received from the uplink
		 * @param source The source of the message
		 * @param command The command being executed
		 * @param params Parameters
		 * @return EVENT_STOP to prevent the protocol module from processing this message
		 */
		virtual EventReturn OnMessage(MessageSource &source, Anope::string &command, std::vector<Anope::string> &param) anope_abstract;
	};
	extern CoreExport EventHandlers<Message> OnMessage;

	struct CoreExport CanSet : Events
	{
		/** Called to determine if a chnanel mode can be set by a user
		 * @param u The user
		 * @param cm The mode
		 */
		virtual EventReturn OnCanSet(User *u, const ChannelMode *cm) anope_abstract;
	};
	extern CoreExport EventHandlers<CanSet> OnCanSet;

	struct CoreExport CheckDelete : Events
	{
		virtual EventReturn OnCheckDelete(Channel *) anope_abstract;
	};
	extern CoreExport EventHandlers<CheckDelete> OnCheckDelete;

	struct CoreExport ExpireTick : Events
	{
		/** Called every options:expiretimeout seconds. Should be used to expire nicks,
		 * channels, etc.
		 */
		virtual void OnExpireTick() anope_abstract;
	};
	extern CoreExport EventHandlers<ExpireTick> OnExpireTick;

}


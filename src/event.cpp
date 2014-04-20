/* Modular support
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#include "services.h"
#include "event.h"

using namespace Event;

EventHandlers<PreUserKicked> Event::OnPreUserKicked(nullptr, "OnPreUserKicked");
EventHandlers<UserKicked> Event::OnUserKicked(nullptr, "OnUserKicked");
EventHandlers<PreBotAssign> Event::OnPreBotAssign(nullptr, "OnPreBotAssign");
EventHandlers<BotAssign> Event::OnBotAssign(nullptr, "OnBotAssign");
EventHandlers<BotUnAssign> Event::OnBotUnAssign(nullptr, "OnBotUnAssign");
EventHandlers<UserConnect> Event::OnUserConnect(nullptr, "OnUserConnect");
EventHandlers<NewServer> Event::OnNewServer(nullptr, "OnNewServer");
EventHandlers<UserNickChange> Event::OnUserNickChange(nullptr, "OnUserNickChange");
EventHandlers<PreCommand> Event::OnPreCommand(nullptr, "OnPreCommand");
EventHandlers<PostCommand> Event::OnPostCommand(nullptr, "OnPostCommand");
EventHandlers<SaveDatabase> Event::OnSaveDatabase(nullptr, "OnSaveDatabase");
EventHandlers<LoadDatabase> Event::OnLoadDatabase(nullptr, "OnLoadDatabase");
EventHandlers<Encrypt> Event::OnEncrypt(nullptr, "OnEncrypt");
EventHandlers<Decrypt> Event::OnDecrypt(nullptr, "OnDecrypt");
EventHandlers<CreateBot> Event::OnCreateBot(nullptr, "OnCreateBot");
EventHandlers<DelBot> Event::OnDelBot(nullptr, "OnDelBot");
EventHandlers<BotKick> Event::OnBotKick(nullptr, "OnBotKick");
EventHandlers<PrePartChannel> Event::OnPrePartChannel(nullptr, "OnPrePartChannel");
EventHandlers<PartChannel> Event::OnPartChannel(nullptr, "OnPartChannel");
EventHandlers<LeaveChannel> Event::OnLeaveChannel(nullptr, "OnLeaveChannel");
EventHandlers<JoinChannel> Event::OnJoinChannel(nullptr, "OnJoinChannel");
EventHandlers<TopicUpdated> Event::OnTopicUpdated(nullptr, "OnTopicUpdated");
EventHandlers<PreServerConnect> Event::OnPreServerConnect(nullptr, "OnPreServerConnect");
EventHandlers<ServerConnect> Event::OnServerConnect(nullptr, "OnServerConnect");
EventHandlers<PreUplinkSync> Event::OnPreUplinkSync(nullptr, "OnPreUplinkSync");
EventHandlers<ServerDisconnect> Event::OnServerDisconnect(nullptr, "OnServerDisconnect");
EventHandlers<Restart> Event::OnRestart(nullptr, "OnRestart");
EventHandlers<Shutdown> Event::OnShutdown(nullptr, "OnShutdown");
EventHandlers<AddXLine> Event::OnAddXLine(nullptr, "OnAddXLine");
EventHandlers<DelXLine> Event::OnDelXLine(nullptr, "OnDelXLine");
EventHandlers<IsServicesOperEvent> Event::OnIsServicesOper(nullptr, "OnIsServicesOper");
EventHandlers<ServerQuit> Event::OnServerQuit(nullptr, "OnServerQuit");
EventHandlers<UserQuit> Event::OnUserQuit(nullptr, "OnUserQuit");
EventHandlers<PreUserLogoff> Event::OnPreUserLogoff(nullptr, "OnPreUserLogoff");
EventHandlers<PostUserLogoff> Event::OnPostUserLogoff(nullptr, "OnPostUserLogoff");
EventHandlers<AccessDel> Event::OnAccessDel(nullptr, "OnAccessDel");
EventHandlers<AccessAdd> Event::OnAccessAdd(nullptr, "OnAccessAdd");
EventHandlers<AccessClear> Event::OnAccessClear(nullptr, "OnAccessClear");
EventHandlers<ChanRegistered> Event::OnChanRegistered(nullptr, "OnChanRegistered");
EventHandlers<CreateChan> Event::OnCreateChan(nullptr, "OnCreateChan");
EventHandlers<DelChan> Event::OnDelChan(nullptr, "OnDelChan");
EventHandlers<ChannelCreate> Event::OnChannelCreate(nullptr, "OnChannelCreate");
EventHandlers<ChannelDelete> Event::OnChannelDelete(nullptr, "OnChannelDelete");
EventHandlers<CheckKick> Event::OnCheckKick(nullptr, "OnCheckKick");
EventHandlers<CheckPriv> Event::OnCheckPriv(nullptr, "OnCheckPriv");
EventHandlers<GroupCheckPriv> Event::OnGroupCheckPriv(nullptr, "OnGroupCheckPriv");
EventHandlers<NickIdentify> Event::OnNickIdentify(nullptr, "OnNickIdentify");
EventHandlers<UserLogin> Event::OnUserLogin(nullptr, "OnUserLogin");
EventHandlers<NickLogout> Event::OnNickLogout(nullptr, "OnNickLogout");
EventHandlers<DelNick> Event::OnDelNick(nullptr, "OnDelNick");
EventHandlers<NickCoreCreate> Event::OnNickCoreCreate(nullptr, "OnNickCoreCreate");
EventHandlers<DelCore> Event::OnDelCore(nullptr, "OnDelCore");
EventHandlers<ChangeCoreDisplay> Event::OnChangeCoreDisplay(nullptr, "OnChangeCoreDisplay");
EventHandlers<NickClearAccess> Event::OnNickClearAccess(nullptr, "OnNickClearAccess");
EventHandlers<NickAddAccess> Event::OnNickAddAccess(nullptr, "OnNickAddAccess");
EventHandlers<NickEraseAccess> Event::OnNickEraseAccess(nullptr, "OnNickEraseAccess");
EventHandlers<CheckAuthentication> Event::OnCheckAuthentication(nullptr, "OnCheckAuthentication");
EventHandlers<Fingerprint> Event::OnFingerprint(nullptr, "OnFingerprint");
EventHandlers<UserAway> Event::OnUserAway(nullptr, "OnUserAway");
EventHandlers<Invite> Event::OnInvite(nullptr, "OnInvite");
EventHandlers<SetVhost> Event::OnSetVhost(nullptr, "OnSetVhost");
EventHandlers<SetDisplayedHost> Event::OnSetDisplayedHost(nullptr, "OnSetDisplayedHost");
EventHandlers<ChannelModeSet> Event::OnChannelModeSet(nullptr, "OnChannelModeSet");
EventHandlers<ChannelModeUnset> Event::OnChannelModeUnset(nullptr, "OnChannelModeUnset");
EventHandlers<UserModeSet> Event::OnUserModeSet(nullptr, "OnUserModeSet");
EventHandlers<UserModeUnset> Event::OnUserModeUnset(nullptr, "OnUserModeUnset");
EventHandlers<ChannelModeAdd> Event::OnChannelModeAdd(nullptr, "OnChannelModeAdd");
EventHandlers<UserModeAdd> Event::OnUserModeAdd(nullptr, "OnUserModeAdd");
EventHandlers<ModuleLoad> Event::OnModuleLoad(nullptr, "OnModuleLoad");
EventHandlers<ModuleUnload> Event::OnModuleUnload(nullptr, "OnModuleUnload");
EventHandlers<ServerSync> Event::OnServerSync(nullptr, "OnServerSync");
EventHandlers<UplinkSync> Event::OnUplinkSync(nullptr, "OnUplinkSync");
EventHandlers<BotPrivmsg> Event::OnBotPrivmsg(nullptr, "OnBotPrivmsg");
EventHandlers<BotNotice> Event::OnBotNotice(nullptr, "OnBotNotice");
EventHandlers<Privmsg> Event::OnPrivmsg(nullptr, "OnPrivmsg");
EventHandlers<Event::Log> Event::OnLog(nullptr, "OnLog");
EventHandlers<LogMessage> Event::OnLogMessage(nullptr, "OnLogMessage");
EventHandlers<CheckModes> Event::OnCheckModes(nullptr, "OnCheckModes");
EventHandlers<ChannelSync> Event::OnChannelSync(nullptr, "OnChannelSync");
EventHandlers<SetCorrectModes> Event::OnSetCorrectModes(nullptr, "OnSetCorrectModes");
EventHandlers<SerializeCheck> Event::OnSerializeCheck(nullptr, "OnSerializeCheck");
EventHandlers<SerializableConstruct> Event::OnSerializableConstruct(nullptr, "OnSerializableConstruct");
EventHandlers<SerializableDestruct> Event::OnSerializableDestruct(nullptr, "OnSerializableDestruct");
EventHandlers<SerializableUpdate> Event::OnSerializableUpdate(nullptr, "OnSerializableUpdate");
EventHandlers<SerializeTypeCreate> Event::OnSerializeTypeCreate(nullptr, "OnSerializeTypeCreate");
EventHandlers<Message> Event::OnMessage(nullptr, "OnMessage");
EventHandlers<CanSet> Event::OnCanSet(nullptr, "OnCanSet");
EventHandlers<CheckDelete> Event::OnCheckDelete(nullptr, "OnCheckDelete");
EventHandlers<ExpireTick> Event::OnExpireTick(nullptr, "OnExpireTick");

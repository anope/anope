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

EventHandlers<PreUserKicked> Event::OnPreUserKicked(nullptr);
EventHandlers<UserKicked> Event::OnUserKicked(nullptr);
EventHandlers<PreBotAssign> Event::OnPreBotAssign(nullptr);
EventHandlers<BotAssign> Event::OnBotAssign(nullptr);
EventHandlers<BotUnAssign> Event::OnBotUnAssign(nullptr);
EventHandlers<UserConnect> Event::OnUserConnect(nullptr);
EventHandlers<NewServer> Event::OnNewServer(nullptr);
EventHandlers<UserNickChange> Event::OnUserNickChange(nullptr);
EventHandlers<PreCommand> Event::OnPreCommand(nullptr);
EventHandlers<PostCommand> Event::OnPostCommand(nullptr);
EventHandlers<SaveDatabase> Event::OnSaveDatabase(nullptr);
EventHandlers<LoadDatabase> Event::OnLoadDatabase(nullptr);
EventHandlers<Encrypt> Event::OnEncrypt(nullptr);
EventHandlers<Decrypt> Event::OnDecrypt(nullptr);
EventHandlers<CreateBot> Event::OnCreateBot(nullptr);
EventHandlers<DelBot> Event::OnDelBot(nullptr);
EventHandlers<PrePartChannel> Event::OnPrePartChannel(nullptr);
EventHandlers<PartChannel> Event::OnPartChannel(nullptr);
EventHandlers<LeaveChannel> Event::OnLeaveChannel(nullptr);
EventHandlers<JoinChannel> Event::OnJoinChannel(nullptr);
EventHandlers<TopicUpdated> Event::OnTopicUpdated(nullptr);
EventHandlers<PreServerConnect> Event::OnPreServerConnect(nullptr);
EventHandlers<ServerConnect> Event::OnServerConnect(nullptr);
EventHandlers<PreUplinkSync> Event::OnPreUplinkSync(nullptr);
EventHandlers<ServerDisconnect> Event::OnServerDisconnect(nullptr);
EventHandlers<Restart> Event::OnRestart(nullptr);
EventHandlers<Shutdown> Event::OnShutdown(nullptr);
EventHandlers<AddXLine> Event::OnAddXLine(nullptr);
EventHandlers<DelXLine> Event::OnDelXLine(nullptr);
EventHandlers<IsServicesOperEvent> Event::OnIsServicesOper(nullptr);
EventHandlers<ServerQuit> Event::OnServerQuit(nullptr);
EventHandlers<UserQuit> Event::OnUserQuit(nullptr);
EventHandlers<PreUserLogoff> Event::OnPreUserLogoff(nullptr);
EventHandlers<PostUserLogoff> Event::OnPostUserLogoff(nullptr);
EventHandlers<AccessDel> Event::OnAccessDel(nullptr);
EventHandlers<AccessAdd> Event::OnAccessAdd(nullptr);
EventHandlers<AccessClear> Event::OnAccessClear(nullptr);
EventHandlers<ChanRegistered> Event::OnChanRegistered(nullptr);
EventHandlers<CreateChan> Event::OnCreateChan(nullptr);
EventHandlers<DelChan> Event::OnDelChan(nullptr);
EventHandlers<ChannelCreate> Event::OnChannelCreate(nullptr);
EventHandlers<ChannelDelete> Event::OnChannelDelete(nullptr);
EventHandlers<CheckKick> Event::OnCheckKick(nullptr);
EventHandlers<CheckPriv> Event::OnCheckPriv(nullptr);
EventHandlers<GroupCheckPriv> Event::OnGroupCheckPriv(nullptr);
EventHandlers<NickIdentify> Event::OnNickIdentify(nullptr);
EventHandlers<UserLogin> Event::OnUserLogin(nullptr);
EventHandlers<NickLogout> Event::OnNickLogout(nullptr);
EventHandlers<DelNick> Event::OnDelNick(nullptr);
EventHandlers<NickCoreCreate> Event::OnNickCoreCreate(nullptr);
EventHandlers<DelCore> Event::OnDelCore(nullptr);
EventHandlers<ChangeCoreDisplay> Event::OnChangeCoreDisplay(nullptr);
EventHandlers<NickClearAccess> Event::OnNickClearAccess(nullptr);
EventHandlers<NickAddAccess> Event::OnNickAddAccess(nullptr);
EventHandlers<NickEraseAccess> Event::OnNickEraseAccess(nullptr);
EventHandlers<CheckAuthentication> Event::OnCheckAuthentication(nullptr);
EventHandlers<Fingerprint> Event::OnFingerprint(nullptr);
EventHandlers<UserAway> Event::OnUserAway(nullptr);
EventHandlers<Invite> Event::OnInvite(nullptr);
EventHandlers<SetVhost> Event::OnSetVhost(nullptr);
EventHandlers<SetDisplayedHost> Event::OnSetDisplayedHost(nullptr);
EventHandlers<ChannelModeSet> Event::OnChannelModeSet(nullptr);
EventHandlers<ChannelModeUnset> Event::OnChannelModeUnset(nullptr);
EventHandlers<UserModeSet> Event::OnUserModeSet(nullptr);
EventHandlers<UserModeUnset> Event::OnUserModeUnset(nullptr);
EventHandlers<ChannelModeAdd> Event::OnChannelModeAdd(nullptr);
EventHandlers<UserModeAdd> Event::OnUserModeAdd(nullptr);
EventHandlers<ModuleLoad> Event::OnModuleLoad(nullptr);
EventHandlers<ModuleUnload> Event::OnModuleUnload(nullptr);
EventHandlers<ServerSync> Event::OnServerSync(nullptr);
EventHandlers<UplinkSync> Event::OnUplinkSync(nullptr);
EventHandlers<BotPrivmsg> Event::OnBotPrivmsg(nullptr);
EventHandlers<BotNotice> Event::OnBotNotice(nullptr);
EventHandlers<Privmsg> Event::OnPrivmsg(nullptr);
EventHandlers<Event::Log> Event::OnLog(nullptr);
EventHandlers<LogMessage> Event::OnLogMessage(nullptr);
EventHandlers<CheckModes> Event::OnCheckModes(nullptr);
EventHandlers<ChannelSync> Event::OnChannelSync(nullptr);
EventHandlers<SetCorrectModes> Event::OnSetCorrectModes(nullptr);
EventHandlers<Message> Event::OnMessage(nullptr);
EventHandlers<CanSet> Event::OnCanSet(nullptr);
EventHandlers<CheckDelete> Event::OnCheckDelete(nullptr);
EventHandlers<ExpireTick> Event::OnExpireTick(nullptr);
EventHandlers<SerializeEvents> Event::OnSerialize(nullptr);

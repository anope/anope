/* Prototypes and external variable declarations.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 *
 */

#define EVENT_START "start"
#define EVENT_STOP  "stop"

#define EVENT_DB_SAVING "db_saving"
#define EVENT_DB_BACKUP "db_backup"
#define EVENT_NEWNICK   "newnick"
#define EVENT_BOT_UNASSIGN "bot_unassign"
#define EVENT_BOT_JOIN "bot_join"
#define EVENT_BOT_CREATE "bot_create"
#define EVENT_BOT_CHANGE "bot_change"
#define EVENT_BOT_FANTASY "bot_command"
#define EVENT_BOT_FANTASY_NO_ACCESS "bot_command_no_access"
#define EVENT_BOT_DEL "bot_del"
#define EVENT_BOT_ASSIGN "bot_assign"
#define EVENT_BOT_KICK "bot_kick"
#define EVENT_BOT_BAN "bot_ban"
#define EVENT_TOPIC_UPDATED "chan_topic_updated"
#define EVENT_CHAN_EXPIRE "chan_expire"
#define EVENT_CHAN_REGISTERED "chan_registered"
#define EVENT_CHAN_DROP "chan_dropped"
#define EVENT_CHAN_FORBIDDEN "chan_forbidden"
#define EVENT_CHAN_SUSPENDED "chan_suspended"
#define EVENT_CHAN_UNSUSPEND "chan_unsuspend"
#define EVENT_CONNECT "connect"
#define EVENT_DB_EXPIRE "db_expire"
#define EVENT_RESTART "restart"
#define EVENT_RELOAD "reload"
#define EVENT_SHUTDOWN "shutdown"
#define EVENT_SIGNAL "signal"
#define EVENT_NICK_REGISTERED "nick_registered"
#define EVENT_NICK_REQUESTED "nick_requested"
#define EVENT_NICK_DROPPED "nick_dropped"
#define EVENT_NICK_FORBIDDEN "nick_forbidden"
#define EVENT_NICK_EXPIRE "nick_expire"
#define EVENT_CHANGE_NICK "change_nick"
#define EVENT_USER_LOGOFF "user_logoff"
#define EVENT_NICK_GHOSTED "nick_ghosted"
#define EVENT_NICK_RECOVERED "nick_recovered"
#define EVENT_GROUP "nick_group"
#define EVENT_NICK_IDENTIFY "nick_id"
#define EVENT_SERVER_SQUIT "server_squit"
#define EVENT_SERVER_CONNECT "server_connect"
#define EVENT_DEFCON_LEVEL "defcon_level"
#define EVENT_NICK_SUSPENDED "nick_suspended"
#define EVENT_NICK_UNSUSPEND "nick_unsuspend"
#define EVENT_JOIN_CHANNEL "join_channel"
#define EVENT_PART_CHANNEL "part_channel"
#define EVENT_ACCESS_ADD "access_add"
#define EVENT_ACCESS_CHANGE "access_change"
#define EVENT_ACCESS_DEL "access_del"
#define EVENT_ACCESS_CLEAR "access_clear"
#define EVENT_NICK_LOGOUT "nick_logout"
#define EVENT_CHAN_KICK "chan_kick"
#define EVENT_MODLOAD "modload"
#define EVENT_MODUNLOAD "modunload"
#define EVENT_ADDCOMMAND "addcommand"
#define EVENT_DELCOMMAND "delcommand"

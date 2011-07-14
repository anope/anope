/* Prototypes and external variable declarations.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifndef EXTERN_H
#define EXTERN_H

#define E extern CoreExport
#define EI extern DllExport

#include "hashcomp.h"

/* IRC Variables */

E IRCDVar *ircd;
E IRCDProto *ircdproto;
E IRCdMessage *ircdmessage;

/**** actions.c ****/

E bool bad_password(User *u);
E void common_unban(ChannelInfo *ci, User *u, bool full = false);

/**** botserv.c ****/

E BotInfo *findbot(const Anope::string &nick);

E void bot_raw_ban(User *requester, ChannelInfo *ci, const Anope::string &nick, const Anope::string &reason);
E void bot_raw_kick(User *requester, ChannelInfo *ci, const Anope::string &nick, const Anope::string &reason);

/**** channels.c ****/


E Channel *findchan(const Anope::string &chan);

E User *nc_on_chan(Channel *c, const NickCore *nc);

E Anope::string get_xop_level(int level);

E void do_cmode(const Anope::string &source, const Anope::string &channel, const Anope::string &modes, const Anope::string &ts);
E void do_join(const Anope::string &source, const Anope::string &channels, const Anope::string &ts);
E void do_kick(const Anope::string &source, const Anope::string &channel, const Anope::string &users, const Anope::string &reason);
E void do_part(const Anope::string &source, const Anope::string &channels, const Anope::string &reason);

E void chan_set_correct_modes(User *user, Channel *c, int give_modes);

/**** chanserv.c ****/

E LevelInfo levelinfo[];


E void reset_levels(ChannelInfo *ci);

E void check_modes(Channel *c);

E ChannelInfo *cs_findchan(const Anope::string &chan);
E int check_access(User *user, ChannelInfo *ci, int what);
E bool IsFounder(User *user, ChannelInfo *ci);
E void update_cs_lastseen(User *user, ChannelInfo *ci);
E int get_idealban(ChannelInfo *ci, User *u, Anope::string &ret);

E int levelinfo_maxwidth;

/**** config.c ****/

E ConfigurationFile services_conf;
E ServerConfig *Config;

/**** encrypt.c ****/
E void enc_encrypt(const Anope::string &src, Anope::string &dest);
E bool enc_decrypt(const Anope::string &src, Anope::string &dest);

/**** hostserv.c  ****/

/**** init.c ****/

E void introduce_user(const Anope::string &user);
E bool GetCommandLineArgument(const Anope::string &name, char shortname = 0);
E bool GetCommandLineArgument(const Anope::string &name, char shortname, Anope::string &param);
E void Init(int ac, char **av);

/**** ircd.c ****/
E void pmodule_ircd_proto(IRCDProto *);
E void pmodule_ircd_var(IRCDVar *ircdvar);
E void pmodule_ircd_message(IRCdMessage *message);

/**** language.cpp ****/
E std::vector<Anope::string> languages;
E std::vector<Anope::string> domains;
E void InitLanguages();
E const char *translate(const char *string);
E const char *translate(User *u, const char *string);
E const char *translate(NickCore *nc, const char *string);
E const char *anope_gettext(const char *lang, const char *string);

/**** main.c ****/

E Anope::string services_dir;
E Anope::string services_bin;
E int debug;
E bool readonly;
E bool nofork;
E bool nothird;
E bool noexpire;
E bool protocoldebug;

E bool quitting;
E bool restarting;
E Anope::string quitmsg;
E time_t start_time;

E ConnectionSocket *UplinkSock;
E int CurrentUplink;

E void save_databases();
E void sighandler(int signum);

/**** messages.cpp ****/

E void init_core_messages();

E bool OnStats(const Anope::string &source, const std::vector<Anope::string> &);
E bool OnTime(const Anope::string &source, const std::vector<Anope::string> &);
E bool OnVersion(const Anope::string &source, const std::vector<Anope::string> &);

E bool On436(const Anope::string &, const std::vector<Anope::string> &);
E bool OnAway(const Anope::string &, const std::vector<Anope::string> &);
E bool OnJoin(const Anope::string &, const std::vector<Anope::string> &);
E bool OnKick(const Anope::string &, const std::vector<Anope::string> &);
E bool OnKill(const Anope::string &, const std::vector<Anope::string> &);
E bool OnMode(const Anope::string &, const std::vector<Anope::string> &);
E bool OnNick(const Anope::string &, const std::vector<Anope::string> &);
E bool OnUID(const Anope::string &, const std::vector<Anope::string> &);
E bool OnPart(const Anope::string &, const std::vector<Anope::string> &);
E bool OnPing(const Anope::string &, const std::vector<Anope::string> &);
E bool OnPrivmsg(const Anope::string &, const std::vector<Anope::string> &);
E bool OnQuit(const Anope::string &, const std::vector<Anope::string> &);
E bool OnServer(const Anope::string &, const std::vector<Anope::string> &);
E bool OnSQuit(const Anope::string &, const std::vector<Anope::string> &);
E bool OnTopic(const Anope::string &, const std::vector<Anope::string> &);
E bool OnWhois(const Anope::string &, const std::vector<Anope::string> &);
E bool OnCapab(const Anope::string &, const std::vector<Anope::string> &);
E bool OnSJoin(const Anope::string &, const std::vector<Anope::string> &);
E bool OnError(const Anope::string &, const std::vector<Anope::string> &);

/**** misc.c ****/

E bool IsFile(const Anope::string &filename);
E int toupper(char);
E int tolower(char);

E time_t dotime(const Anope::string &s);
E Anope::string duration(const time_t &seconds, NickCore *nc = NULL);
E Anope::string expire_left(NickCore *nc, time_t expires);
E Anope::string do_strftime(const time_t &t, NickCore *nc = NULL, bool short_output = false);
E bool doValidHost(const Anope::string &host, int type);

E bool isValidHost(const Anope::string &host, int type);
E bool isvalidchar(char c);

E Anope::string myStrGetToken(const Anope::string &str, char dilim, int token_number);
E Anope::string myStrGetTokenRemainder(const Anope::string &str, char dilim, int token_number);
E int myNumToken(const Anope::string &str, char dilim);
E bool nickIsServices(const Anope::string &nick, bool bot);

E void add_entropy_userkeys();
E void rand_init();
E unsigned char getrandom8();
E uint16 getrandom16();
E uint32 getrandom32();

E std::list<Anope::string> BuildStringList(const Anope::string &, char = ' ');
E std::vector<Anope::string> BuildStringVector(const Anope::string &, char = ' ');

E bool str_is_wildcard(const Anope::string &str);
E bool str_is_pure_wildcard(const Anope::string &str);
E Anope::string normalizeBuffer(const Anope::string &);

/**** modes.cpp ****/
/* Number of generic modes we support */
E unsigned GenericChannelModes, GenericUserModes;
E std::multimap<ChannelModeName, ModeLock> def_mode_locks;
E void SetDefaultMLock(ServerConfig *config);

/**** nickserv.c ****/

E void change_core_display(NickCore *nc);
E void change_core_display(NickCore *nc, const Anope::string &newdisplay);

E NickAlias *findnick(const Anope::string &nick);
E NickCore *findcore(const Anope::string &nick);
E bool is_on_access(const User *u, const NickCore *nc);

/**** process.c ****/

E void process(const Anope::string &buf);

/**** send.c ****/

E void send_cmd(const Anope::string &source, const char *fmt, ...) FORMAT(printf, 2, 3);

E void notice_server(const Anope::string &source, const Server *s, const char *fmt, ...) FORMAT(printf, 3, 4);

/**** sockets.cpp ****/

E int32 TotalRead;
E int32 TotalWritten;
E SocketIO normalSocketIO;

/**** users.c ****/

E int32 opcnt;
E uint32 maxusercnt, usercnt;
E time_t maxusertime;

E User *finduser(const Anope::string &nick);

E User *do_nick(const Anope::string &source, const Anope::string &nick, const Anope::string &username, const Anope::string &host, const Anope::string &server, const Anope::string &realname, time_t ts, const Anope::string &ip, const Anope::string &vhost, const Anope::string &uid, const Anope::string &modes);

E void do_umode(const Anope::string &user, const Anope::string &modes);
E void do_kill(User *user, const Anope::string &reason);

E bool matches_list(Channel *c, User *user, ChannelModeName mode);

E Anope::string create_mask(User *u);

#endif /* EXTERN_H */

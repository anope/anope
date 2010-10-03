/* Prototypes and external variable declarations.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifndef EXTERN_H
#define EXTERN_H

#define E extern CoreExport
#define EI extern DllExport

#include "hashcomp.h"

E void ModuleRunTimeDirCleanUp();

/* IRC Variables */

E IRCDVar *ircd;
E IRCDProto *ircdproto;

/**** actions.c ****/

E void kill_user(const Anope::string &source, const Anope::string &user, const Anope::string &reason);
E bool bad_password(User *u);
E void common_unban(ChannelInfo *ci, const Anope::string &nick);

E BotInfo *BotServ;
E BotInfo *ChanServ;
E BotInfo *Global;
E BotInfo *HostServ;
E BotInfo *MemoServ;
E BotInfo *NickServ;
E BotInfo *OperServ;

/**** botserv.c ****/

E void get_botserv_stats(long *nrec, long *memuse);
E void bs_init();
E void botchanmsgs(User *u, ChannelInfo *ci, const Anope::string &buf);
E BotInfo *findbot(const Anope::string &nick);

/** Finds a pseudoclient, given a UID. Useful for TS6 protocol modules.
 * @param uid The UID to search for
 * @return The pseudoclient structure, or NULL if one could not be found
 */
E Anope::string normalizeBuffer(const Anope::string &);

E void bot_raw_ban(User *requester, ChannelInfo *ci, const Anope::string &nick, const Anope::string &reason);
E void bot_raw_kick(User *requester, ChannelInfo *ci, const Anope::string &nick, const Anope::string &reason);
E void bot_raw_mode(User *requester, ChannelInfo *ci, const Anope::string &mode, const Anope::string &nick);

/**** channels.c ****/

E void get_channel_stats(long *nrec, long *memuse);

E Channel *findchan(const Anope::string &chan);

E void ChanSetInternalModes(Channel *c, int ac, const char **av, User *setter = NULL); /* this is dieing soon anyway */

E User *nc_on_chan(Channel *c, const NickCore *nc);

E int get_access_level(ChannelInfo *ci, NickAlias *na);
E int get_access_level(ChannelInfo *ci, NickCore *nc);
E Anope::string get_xop_level(int level);

E void do_cmode(const Anope::string &source, int ac, const char **av);
E void do_join(const Anope::string &source, int ac, const char **av);
E void do_kick(const Anope::string &source, int ac, const char **av);
E void do_part(const Anope::string &source, int ac, const char **av);
E void MassChannelModes(BotInfo *bi, const Anope::string &modes);

E void chan_set_correct_modes(User *user, Channel *c, int give_modes);

E Entry *entry_create(const Anope::string &mask);
E Entry *entry_add(EList *list, const Anope::string &mask);
E void entry_delete(EList *list, Entry *e);
E EList *list_create();
E int entry_match(Entry *e, const Anope::string &nick, const Anope::string &user, const Anope::string &host, uint32 ip);
E int entry_match_mask(Entry *e, const Anope::string &mask, uint32 ip);
E Entry *elist_match(EList *list, const Anope::string &nick, const Anope::string &user, const Anope::string &host, uint32 ip);
E Entry *elist_match_mask(EList *list, const Anope::string &mask, uint32 ip);
E Entry *elist_match_user(EList *list, User *u);
E Entry *elist_find_mask(EList *list, const Anope::string &mask);
E long get_memuse(EList *list);

inline BotInfo *whosends(ChannelInfo *ci)
{
	if (!ci || !ci->bi || !ci->c || !ci->botflags.HasFlag(BS_SYMBIOSIS) || !ci->c->FindUser(ci->bi))
		return ChanServ ? ChanServ : NickServ;
	return ci->bi;
}

/**** chanserv.c ****/

E LevelInfo levelinfo[];

E void get_chanserv_stats(long *nrec, long *memuse);

E void reset_levels(ChannelInfo *ci);
E void cs_init();
E void expire_chans();
E void cs_remove_nick(const NickCore *nc);

E void check_modes(Channel *c);
E int check_valid_admin(User *user, Channel *chan, int servermode);
E int check_valid_op(User *user, Channel *chan, int servermode);

E ChannelInfo *cs_findchan(const Anope::string &chan);
E int check_access(User *user, ChannelInfo *ci, int what);
E bool IsFounder(User *user, ChannelInfo *ci);
E int get_access(User *user, ChannelInfo *ci);
E void update_cs_lastseen(User *user, ChannelInfo *ci);
E int get_idealban(ChannelInfo *ci, User *u, Anope::string &ret);
E AutoKick *is_stuck(ChannelInfo *ci, const Anope::string &mask);
E void stick_mask(ChannelInfo *ci, AutoKick *akick);
E void stick_all(ChannelInfo *ci);

E int levelinfo_maxwidth;
E Anope::string get_mlock_modes(ChannelInfo *ci, int complete);

/**** config.c ****/

E Anope::string services_conf;
E ServerConfig *Config;

/* hostserv.c */
E void do_on_id(User *u);
E void HostServSyncVhosts(NickAlias *na);

/**** encrypt.c ****/
E int enc_encrypt(const Anope::string &src, Anope::string &dest);
E int enc_decrypt(const Anope::string &src, Anope::string &dest);
E int enc_check_password(Anope::string &plaintext, Anope::string &password);

/**** hostserv.c  ****/
E void get_hostserv_stats(long *nrec, long *memuse);
E void hostserv_init();

/**** init.c ****/

E void introduce_user(const Anope::string &user);
E bool GetCommandLineArgument(const Anope::string &name, char shortname = 0);
E bool GetCommandLineArgument(const Anope::string &name, char shortname, Anope::string &param);
E void Init(int ac, char **av);
E Uplink *uplink_server;

/**** ircd.c ****/
E void pmodule_ircd_proto(IRCDProto *);
E void pmodule_ircd_var(IRCDVar *ircdvar);

/**** language.cpp ****/
E std::vector<Anope::string> languages;
E void InitLanguages();
E const Anope::string GetString(Anope::string language, LanguageString string);
E const Anope::string GetString(LanguageString string);
E const Anope::string GetString(const NickCore *nc, LanguageString string);
E const Anope::string GetString(const User *u, LanguageString string);
E Anope::string language_strings[LANG_STRING_COUNT];
E void SyntaxError(BotInfo *bi, User *u, const Anope::string &command, LanguageString message);

/*** logger.cpp ***/
E void InitLogChannels(ServerConfig *);

/**** main.c ****/

E Anope::string services_dir;
E int debug;
E bool readonly;
E bool nofork;
E bool nothird;
E bool noexpire;
E bool protocoldebug;

E bool quitting;
E bool shutting_down;
E Anope::string quitmsg;
E bool save_data;
E time_t start_time;

E ConnectionSocket *UplinkSock;

E void save_databases();
E void expire_all();
E void sighandler(int signum);
E void do_restart_services();

/* The socket to our uplink */
class UplinkSocket : public ConnectionSocket
{
 public:
	UplinkSocket(bool ipv6 = false);

	virtual ~UplinkSocket();

	bool Read(const Anope::string &buf);
};

/**** memory.c ****/

E void *scalloc(long elsize, long els);
E void *srealloc(void *oldptr, long newsize);

/**** memoserv.c ****/

E void ms_init();
E void rsend_notify(User *u, Memo *m, const Anope::string &chan);
E void check_memos(User *u);
E MemoInfo *getmemoinfo(const Anope::string &name, bool &ischan, bool &isforbid);
E void memo_send(User *u, const Anope::string &name, const Anope::string &text, int z);
E bool delmemo(MemoInfo *mi, int num);

/**** messages.c ****/

E int m_nickcoll(const Anope::string &user);
E int m_away(const Anope::string &source, const Anope::string &msg);
E int m_kill(const Anope::string &nick, const Anope::string &msg);
E int m_motd(const Anope::string &source);
E int m_privmsg(const Anope::string &source, const Anope::string &receiver, const Anope::string &message);
E int m_stats(const Anope::string &source, int ac, const char **av);
E int m_whois(const Anope::string &source, const Anope::string &who);
E int m_time(const Anope::string &source, int ac, const char **av);
E int m_version(const Anope::string &source, int ac, const char **av);

/**** misc.c ****/

E bool IsFile(const Anope::string &filename);
E int toupper(char);
E int tolower(char);
E char *strscpy(char *d, const char *s, size_t len);
#ifndef HAVE_STRLCPY
E size_t strlcpy(char *, const char *, size_t);
#endif
#ifndef HAVE_STRLCAT
E size_t strlcat(char *, const char *, size_t);
#endif
E char *strnrepl(char *s, int32 size, const char *old, const char *nstr);
E const char *merge_args(int argc, char **argv);
E const char *merge_args(int argc, const char **argv);

E time_t dotime(const Anope::string &s);
E Anope::string duration(const NickCore *nc, time_t seconds);
E Anope::string expire_left(const NickCore *nc, time_t expires);
E Anope::string do_strftime(const time_t &t);
E bool doValidHost(const Anope::string &host, int type);

E bool isValidHost(const Anope::string &host, int type);
E bool isvalidchar(char c);

E Anope::string myStrGetToken(const Anope::string &str, char dilim, int token_number);
E Anope::string myStrGetTokenRemainder(const Anope::string &str, char dilim, int token_number);
E int myNumToken(const Anope::string &str, char dilim);
E void doCleanBuffer(char *str);
E void EnforceQlinedNick(const Anope::string &nick, const Anope::string &killer);
E bool nickIsServices(const Anope::string &nick, bool bot);

E void add_entropy_userkeys();
E void rand_init();
E unsigned char getrandom8();
E uint16 getrandom16();
E uint32 getrandom32();

E char *str_signed(unsigned char *str);

E std::list<Anope::string> BuildStringList(const Anope::string &, char = ' ');
E std::vector<Anope::string> BuildStringVector(const Anope::string &, char = ' ');

E uint32 cidr_to_netmask(uint16 cidr);
E uint16 netmask_to_cidr(uint32 mask);

E bool str_is_wildcard(const Anope::string &str);
E bool str_is_pure_wildcard(const Anope::string &str);

E bool str_is_cidr(const Anope::string &str, uint32 &ip, uint32 &mask, Anope::string &host);

/**** modes.cpp ****/
/* Number of generic modes we support */
E unsigned GenericChannelModes, GenericUserModes;
E Flags<ChannelModeName, CMODE_END * 2> DefMLockOn;
E Flags<ChannelModeName, CMODE_END * 2> DefMLockOff;
E std::map<ChannelModeName, Anope::string> DefMLockParams;
E void SetDefaultMLock(ServerConfig *config);

/**** nickserv.c ****/

E NickRequest *findrequestnick(const Anope::string &nick);
E void get_aliases_stats(long &count, long &mem);
E void get_core_stats(long &count, long &mem);
E void change_core_display(NickCore *nc);
E void change_core_display(NickCore *nc, const Anope::string &newdisplay);
E int do_setmodes(User *u);

E void ns_init();
E int validate_user(User *u);
E void expire_nicks();
E void expire_requests();
E NickAlias *findnick(const Anope::string &nick);
E NickCore *findcore(const Anope::string &nick);
E bool is_on_access(const User *u, const NickCore *nc);

/**** process.c ****/

E int allow_ignore;
E std::list<IgnoreData *> ignore;

E void add_ignore(const Anope::string &nick, time_t delta);
E IgnoreData *get_ignore(const Anope::string &nick);
E int delete_ignore(const Anope::string &nick);
E int clear_ignores();

E int split_buf(char *buf, const char ***argv, int colon_special);
E void process(const Anope::string &buf);

/**** send.c ****/

E void send_cmd(const Anope::string &source, const char *fmt, ...) FORMAT(printf, 2, 3);

E void notice_server(const Anope::string &source, const Server *s, const char *fmt, ...) FORMAT(printf, 3, 4);

/**** sessions.c ****/

E std::vector<Exception *> exceptions;

E void get_session_stats(long &count, long &mem);
E void get_exception_stats(long &count, long &mem);

E void add_session(const Anope::string &nick, const Anope::string &host, const Anope::string &hostip);
E void del_session(const Anope::string &host);

E void expire_exceptions();

E Session *findsession(const Anope::string &host);

E Exception *find_host_exception(const Anope::string &host);
E Exception *find_hostip_exception(const Anope::string &host, const Anope::string &hostip);
E int exception_add(User *u, const Anope::string &mask, int limit, const Anope::string &reason, const Anope::string &who, time_t expires);

/**** sockets.cpp ****/

E SocketEngineBase *SocketEngine;
E int32 TotalRead;
E int32 TotalWritten;
E SocketIO normalSocketIO;

/**** users.c ****/

E int32 opcnt;
E uint32 maxusercnt, usercnt;
E time_t maxusertime;

E void get_user_stats(long &count, long &mem);

E User *finduser(const Anope::string &nick);

E Anope::string TS6SID;

E User *do_nick(const Anope::string &source, const Anope::string &nick, const Anope::string &username, const Anope::string &host, const Anope::string &server, const Anope::string &realname, time_t ts, const Anope::string &ip, const Anope::string &vhost, const Anope::string &uid, const Anope::string &modes);

E void do_umode(const Anope::string &source, int ac, const char **av);
E void do_quit(const Anope::string &source, int ac, const char **av);
E void do_kill(const Anope::string &source, const Anope::string &reason);

E bool is_oper(User *user);

E bool is_excepted(ChannelInfo *ci, User *user);
E bool is_excepted_mask(ChannelInfo *ci, const Anope::string &mask);

E bool match_usermask(const Anope::string &mask, User *user);
E Anope::string create_mask(User *u);

/******************************************************************************/

E void b64_encode(const Anope::string &src, Anope::string &target);
E void b64_decode(const Anope::string &src, Anope::string &target);

#ifdef _WIN32
E Anope::string GetWindowsVersion();
E bool SupportedWindowsVersion();
#endif

#endif /* EXTERN_H */

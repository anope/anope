/* Prototypes and external variable declarations.
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id: extern.h,v 1.65 2004/03/13 13:55:59 dane Exp $ 
 *
 */

#ifndef EXTERN_H
#define EXTERN_H

#include "slist.h"

#define E extern


/**** modules.c ****/
E void moduleCallBackRun(void);

/**** actions.c ****/

E void change_user_mode(User *u, char *modes, char *arg);
E void kill_user(const char *source, const char *user, const char *reason);
E void bad_password(User *u);

/**** botserv.c ****/

E BotInfo *botlists[256];

E void get_botserv_stats(long *nrec, long *memuse);

E void bs_init(void);
E void botserv(User *u, char *buf);
E void botmsgs(User *u, BotInfo *bi, char *buf);
E void botchanmsgs(User *u, ChannelInfo *ci, char *buf);
E void load_bs_dbase(void);
E void save_bs_dbase(void);
E void save_bs_rdb_dbase(void);

E BotInfo *findbot(char *nick);
E void bot_join(ChannelInfo *ci);
E void bot_rejoin_all(BotInfo *bi);

/**** channels.c ****/

E Channel *chanlist[1024];
E CBMode cbmodes[128];
E CBModeInfo cbmodeinfos[];

E void get_channel_stats(long *nrec, long *memuse);
E Channel *findchan(const char *chan);
E Channel *firstchan(void);
E Channel *nextchan(void);

E void chan_deluser(User *user, Channel *c);

E int is_on_chan(Channel *c, User *u);
E User *nc_on_chan(Channel *c, NickCore *nc);

E char *chan_get_modes(Channel *chan, int complete, int plus);
E void chan_set_modes(const char *source, Channel *chan, int ac, char **av, int check);

E int chan_get_user_status(Channel *chan, User *user);
E int chan_has_user_status(Channel *chan, User *user, int16 status);
E void chan_remove_user_status(Channel *chan, User *user, int16 status);
E void chan_set_user_status(Channel *chan, User *user, int16 status);

E int   get_access_level(ChannelInfo *ci, NickAlias *na);
E char *get_xop_level(int level);

E void do_cmode(const char *source, int ac, char **av);
E void do_join(const char *source, int ac, char **av);
E void do_kick(const char *source, int ac, char **av);
E void do_part(const char *source, int ac, char **av);
E void do_sjoin(const char *source, int ac, char **av);
E void do_topic(const char *source, int ac, char **av);
E void do_mass_mode(char *modes);
#define whosends(ci) ((!(ci) || !((ci)->botflags & BS_SYMBIOSIS) || !(ci)->bi || !(ci)->c || (ci)->c->usercount < BSMinUsers) ? s_ChanServ : (ci)->bi->nick)

/**** chanserv.c ****/

E ChannelInfo *chanlists[256];
E CSModeUtil csmodeutils[];

E void listchans(int count_only, const char *chan);
E void get_chanserv_stats(long *nrec, long *memuse);

E void cs_init(void);
E void chanserv(User *u, char *buf);
E void load_cs_dbase(void);
E void save_cs_dbase(void);
E void save_cs_rdb_dbase(void);
E void expire_chans(void);
E void cs_remove_nick(const NickCore *nc);
E void cs_remove_bot(const BotInfo *bi);

E void check_modes(Channel *c);
#ifdef IRC_ULTIMATE3
E int check_valid_admin(User *user, Channel *chan, int servermode);
#endif
E int check_valid_op(User *user, Channel *chan, int servermode);
E int check_should_op(User *user, const char *chan);
E int check_should_voice(User *user, const char *chan);
#ifdef HAS_HALFOP
E int check_should_halfop(User *user, const char *chan);
#endif
#if defined(IRC_UNREAL) || defined(IRC_VIAGRA)
E int check_should_owner(User *user, const char *chan);
E int check_should_protect(User *user, const char *chan);
#endif
#ifdef IRC_ULTIMATE3
E int check_should_protect(User *user, const char *chan);
#endif
E int check_kick(User *user, char *chan);
E void record_topic(const char *chan);
E void restore_topic(const char *chan);
E int check_topiclock(Channel *c, time_t topic_time);

E ChannelInfo *cs_findchan(const char *chan);
E int check_access(User *user, ChannelInfo *ci, int what);
E int is_founder(User *user, ChannelInfo *ci);
E int get_access(User *user, ChannelInfo *ci);
E ChanAccess *get_access_entry(NickCore *nc, ChannelInfo *ci);
E void update_cs_lastseen(User *user, ChannelInfo *ci);
E int get_idealban(ChannelInfo *ci, User *u, char *ret, int retlen);
E AutoKick *is_stuck(ChannelInfo *ci, char *mask);
E void stick_mask(ChannelInfo *ci, AutoKick *akick);
E void stick_all(ChannelInfo *ci);

#ifdef HAS_FMODE
E char *cs_get_flood(ChannelInfo *ci);
E void cs_set_flood(ChannelInfo *ci, char *value);
#endif
E char *cs_get_key(ChannelInfo *ci);
E void cs_set_key(ChannelInfo *ci, char *value);
E char *cs_get_limit(ChannelInfo *ci);
E void cs_set_limit(ChannelInfo *ci, char *value);
#ifdef HAS_LMODE
E char *cs_get_redirect(ChannelInfo *ci);
E void cs_set_redirect(ChannelInfo *ci, char *value);
#endif

/**** compat.c ****/

#if !HAVE_SNPRINTF
# if BAD_SNPRINTF
#  define snprintf my_snprintf
# endif
# define vsnprintf my_vsnprintf
E int vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
E int snprintf(char *buf, size_t size, const char *fmt, ...);
#endif
#if !HAVE_STRICMP && !HAVE_STRCASECMP
E int stricmp(const char *s1, const char *s2);
E int strnicmp(const char *s1, const char *s2, size_t len);
#endif
#if !HAVE_STRDUP
E char *strdup(const char *s);
#endif
#if !HAVE_STRSPN
E size_t strspn(const char *s, const char *accept);
#endif
#if !HAVE_STRERROR
E char *strerror(int errnum);
#endif
#if !HAVE_STRSIGNAL
char *strsignal(int signum);
#endif


/**** config.c ****/

E char *RemoteServer;
E int   RemotePort;
E char *RemotePassword;
E char *RemoteServer2;
E int   RemotePort2;
E char *RemotePassword2;
E char *RemoteServer3;
E int   RemotePort3;
E char *RemotePassword3;
E char *LocalHost;
E int   LocalPort;

E char *ServerName;
E char *ServerDesc;
E char *ServiceUser;
E char *ServiceHost;

E char *HelpChannel;
E char *LogChannel;
E char **NetworkDomains;
E int  DomainNumber;
E char *NetworkName;

E char *s_NickServ;
E char *s_ChanServ;
E char *s_MemoServ;
E char *s_BotServ;
E char *s_HelpServ;
E char *s_OperServ;
E char *s_GlobalNoticer;
E char *s_IrcIIHelp;
E char *s_DevNull;
E char *desc_NickServ;
E char *desc_ChanServ;
E char *desc_MemoServ;
E char *desc_BotServ;
E char *desc_HelpServ;
E char *desc_OperServ;
E char *desc_GlobalNoticer;
E char *desc_IrcIIHelp;
E char *desc_DevNull;

E char *HostDBName;
E char *desc_HostServ;
E char *s_HostServ;
E void load_hs_dbase(void);
E void save_hs_dbase(void);
E void save_hs_rdb_dbase(void);
E int do_on_id(User *u);
E void delHostCore(char *nick);
E void hostserv(User *u, char *buf);

E char *s_NickServAlias;
E char *s_ChanServAlias;
E char *s_MemoServAlias;
E char *s_BotServAlias;
E char *s_HelpServAlias;
E char *s_OperServAlias;
E char *s_GlobalNoticerAlias;
E char *s_DevNullAlias;
E char *s_HostServAlias;
E char *desc_NickServAlias;
E char *desc_ChanServAlias;
E char *desc_MemoServAlias;
E char *desc_BotServAlias;
E char *desc_HelpServAlias;
E char *desc_OperServAlias;
E char *desc_GlobalNoticerAlias;
E char *desc_DevNullAlias;
E char *desc_HostServAlias;

E char *PIDFilename;
E char *MOTDFilename;
E char *NickDBName;
E char *PreNickDBName;
E char *ChanDBName;
E char *BotDBName;
E char *OperDBName;
E char *AutokillDBName;
E char *NewsDBName;

E int   NoBackupOkay;
E int   StrictPasswords;
E int   BadPassLimit;
E int   BadPassTimeout;
E int   UpdateTimeout;
E int   ExpireTimeout;
E int   ReadTimeout;
E int   WarningTimeout;
E int   TimeoutCheck;
E int   KeepLogs;
E int   KeepBackups;
E int   ForceForbidReason;
E int   UsePrivmsg;
E int   DumpCore;
E int   LogUsers;

E char **HostSetters;
E int HostNumber;

E int   UseMail;
E char *SendMailPath;
E char *SendFrom;
E int   RestrictMail;
E int   MailDelay;
E int  DontQuoteAddresses;

E int   ProxyDetect;
E int   ProxyThreads;
E char *ProxyMessage[8];
E int   ProxyCheckWingate;
E int   ProxyCheckSocks4;
E int   ProxyCheckSocks5;
E int   ProxyCheckHTTP1;
E int   ProxyCheckHTTP2;
E int   ProxyCheckHTTP3;
E int   ProxyTimeout;
E char *ProxyTestServer;
E int   ProxyTestPort;
E int   ProxyExpire;
E int   ProxyCacheExpire;
E char *ProxyAkillReason;
E int   WallProxy;
E int   ProxyMax;

E int   NSDefFlags;
E int   NSDefLanguage;
E int   NSRegDelay;
E int   NSExpire;
E int   NSRExpire;
E int   NSForceEmail;
E int   NSMaxAliases;
E int   NSAccessMax;
E char *NSEnforcerUser;
E char *NSEnforcerHost;
E int   NSReleaseTimeout;
E int   NSAllowKillImmed;
E int   NSNoGroupChange;
E int   NSListOpersOnly;
E int   NSListMax;
E char *NSGuestNickPrefix;
E int   NSSecureAdmins;
E int   NSStrictPrivileges;
E int   NSEmailReg;
E int   NSModeOnID;
E int   NSRestrictGetPass;

E int   CSDefFlags;
E int   CSMaxReg;
E int   CSExpire;
E int   CSDefBantype;
E int   CSAccessMax;
E int   CSAutokickMax;
E char *CSAutokickReason;
E int   CSInhabit;
E int   CSListOpersOnly;
E int   CSListMax;
E int   CSRestrictGetPass;
E int   CSOpersOnly;

E int   MSMaxMemos;
E int   MSSendDelay;
E int   MSNotifyAll;

E int   BSDefFlags;
E int   BSKeepData;
E int   BSMinUsers;
E int   BSBadWordsMax;
E int   BSSmartJoin;
E int   BSGentleBWReason;

E int   HideStatsO;
E int   GlobalOnCycle;
E int   AnonymousGlobal;
E char *GlobalOnCycleMessage;
E char *GlobalOnCycleUP;
E char **ServicesRoots;
E int   RootNumber;
E int   LogMaxUsers;
E int   SuperAdmin;
E int   LogBot;
E int   AutokillExpiry;
E int   ChankillExpiry;
E int	SGLineExpiry;
E int   SQLineExpiry;
E int	SZLineExpiry;
E int   AkillOnAdd;
E int   DisableRaw;
E int   WallOper;
E int   WallBadOS;
E int   WallOSGlobal;
E int   WallOSMode;
E int   WallOSClearmodes;
E int   WallOSKick;
E int   WallOSAkill;
E int   WallOSSGLine;
E int   WallOSSQLine;
E int   WallOSSZLine;
E int   WallOSNoOp;
E int   WallOSJupe;
E int   WallOSRaw;
E int   WallAkillExpire;
E int   WallSGLineExpire;
E int   WallSQLineExpire;
E int   WallSZLineExpire;
E int   WallExceptionExpire;
E int   WallDrop;
E int   WallForbid;
E int   WallGetpass;
E int   WallSetpass;
E int   CheckClones;
E int   CloneMinUsers;
E int   CloneMaxDelay;
E int   CloneWarningDelay;
E int   KillClones;
E int   AddAkiller;

E char **ModulesAutoload;
E int ModulesNumber;
E char **ModulesDelayedAutoload;
E int ModulesDelayedNumber;


E int   KillClonesAkillExpire;

E int   LimitSessions;
E int   DefSessionLimit;
E int   ExceptionExpiry;
E int   MaxSessionKill;
E int   MaxSessionLimit;
E int   SessionAutoKillExpiry;
E char *ExceptionDBName;
E char *SessionLimitDetailsLoc;
E char *SessionLimitExceeded;

#ifdef USE_RDB
E int rdb_init();
E int rdb_open();
E int rdb_close();
E int rdb_tag_table(char *table);
E int rdb_tag_table(char *table);
E int rdb_clear_table(char *table);
E int rdb_scrub_table(char *table, char *clause);
E int rdb_direct_query(char *query);
E int rdb_ns_set_display(char *newnick, char *oldnick);
E int rdb_cs_set_founder(char *channel, char *founder);
E int rdb_cs_deluser(char *nick);
E int rdb_cs_delchan(ChannelInfo * ci);
E void rdb_save_ns_core(NickCore * nc);
E void rdb_save_ns_alias(NickAlias * na);
E void rdb_save_ns_req(NickRequest * nr);
E void rdb_save_cs_info(ChannelInfo * ci);
E void rdb_save_bs_core(BotInfo * bi);
E void rdb_save_bs_rdb_core(BotInfo * bi);
E void rdb_save_hs_core(HostCore * hc);
E void rdb_save_os_db(unsigned int maxucnt, unsigned int maxutime,
                    SList * ak, SList * sgl, SList * sql, SList * szl,
                    HostCache * hc);
E void rdb_save_news(NewsItem * ni);
E void rdb_save_exceptions(Exception * e);
#endif

#ifdef USE_MYSQL
E char *MysqlHost;
E char *MysqlUser;
E char *MysqlPass;
E char *MysqlName;
E int MysqlPort;
E char *MysqlSock;
E char *MysqlSecure;
E int MysqlRetries;
E int MysqlRetryGap;
#endif

E int read_config(int reload);

E int DefConLevel;
E int DefCon[6];
E int checkDefCon(int level);
E void resetDefCon(int level);
E int DefConSessionLimit;
E char *DefConTimeOut;
E char *DefConAKILL;
E char *DefConChanModes;
E int GlobalOnDefcon;
E int GlobalOnDefconMore;
E char *DefconMessage;
E char *DefConAkillReason;
E char *DefConOffMessage;

/**** converter.c ****/

E int convert_ircservices_44(void);

/**** init.c ****/

E void introduce_user(const char *user);
E int init(int ac, char **av);
E int servernum;

/**** language.c ****/

E char **langtexts[NUM_LANGS];
E char *langnames[NUM_LANGS];
E int langlist[NUM_LANGS];

E void lang_init(void);
#define getstring(na,index) \
	(langtexts[((na)&&((NickAlias*)na)->nc&&!(((NickAlias*)na)->status & NS_VERBOTEN)?((NickAlias*)na)->nc->language:NSDefLanguage)][(index)])
#define getstring2(nc,index) \
	(langtexts[((nc)?((NickCore*)nc)->language:NSDefLanguage)][(index)])
E int strftime_lang(char *buf, int size, User *u, int format, struct tm *tm);
E void syntax_error(const char *service, User *u, const char *command, int msgnum);


/**** list.c ****/

E void do_listnicks(int ac, char **av);
E void do_listchans(int ac, char **av);


/**** log.c ****/

E int open_log(void);
E void close_log(void);
E void alog(const char *fmt, ...)		FORMAT(printf,1,2);
E void log_perror(const char *fmt, ...)		FORMAT(printf,1,2);
E void fatal(const char *fmt, ...)		FORMAT(printf,1,2);
E void fatal_perror(const char *fmt, ...)	FORMAT(printf,1,2);

/**** mail.c ****/

E MailInfo *MailBegin(User *u, NickCore *nc, char *subject, char *service);
E MailInfo *MailRegBegin(User *u, NickRequest *nr, char *subject, char *service);
E void MailEnd(MailInfo *mail);
E void MailReset(User *u, NickCore *nc);
E int MailValidate(const char *email);

/**** main.c ****/

E const char version_number[];
E const char version_build[];
E const char version_protocol[];
E const char version_flags[];

E char *services_dir;
E char *log_filename;
E int   debug;
E int   readonly;
E int   logchan;
E int   skeleton;
E int   nofork;
E int   forceload;
E int	noexpire;

#ifdef USE_RDB
E int   do_mysql;
#endif

E int   is44;

E int   quitting;
E int   delayed_quit;
E char *quitmsg;
E char  inbuf[BUFSIZE];
E int   servsock;
E int   save_data;
E int   got_alarm;
E time_t start_time;

E void save_databases(void);

/**** memory.c ****/

E void *smalloc(long size);
E void *scalloc(long elsize, long els);
E void *srealloc(void *oldptr, long newsize);
E char *sstrdup(const char *s);


/**** memoserv.c ****/

E void ms_init(void);
E void memoserv(User *u, char *buf);
E void check_memos(User *u);


/**** misc.c ****/

E char *strscpy(char *d, const char *s, size_t len);
E char *stristr(char *s1, char *s2);
E char *strnrepl(char *s, int32 size, const char *old, const char *new);

E char *merge_args(int argc, char **argv);

E int match_wild(const char *pattern, const char *str);
E int match_wild_nocase(const char *pattern, const char *str);

E int dotime(const char *s);
E char *duration(NickAlias *na, char *buf, int bufsize, time_t seconds);
E char *expire_left(NickAlias *na, char *buf, int len, time_t expires);

typedef int (*range_callback_t)(User *u, int num, va_list args);
E int process_numlist(const char *numstr, int *count_ret,
		range_callback_t callback, User *u, ...);

E int isValidHost(const char *host, int type);
E int isvalidchar(char c);

E char *myStrGetToken(const char *str, const char dilim, int token_number);
E char *myStrGetOnlyToken(const char *str, const char dilim,
        int token_number);
E char *myStrSubString(const char *src, int start, int end);
E char *myStrGetTokenRemainder(const char *str, const char dilim,
        int token_number);
E void doCleanBuffer(char *str);

/**** news.c ****/

E void get_news_stats(long *nrec, long *memuse);
E void load_news(void);
E void save_news(void);
E void save_rdb_news(void);
E void display_news(User *u, int16 type);
E int do_logonnews(User *u);
E int do_opernews(User *u);
E int do_randomnews(User *u);


/**** nickserv.c ****/

E NickAlias *nalists[1024];
E NickCore *nclists[1024];
E NickRequest *nrlists[1024];

E void listnicks(int count_only, const char *nick);
E void get_aliases_stats(long *nrec, long *memuse);
E void get_core_stats(long *nrec, long *memuse);

E void ns_init(void);
E void nickserv(User *u, char *buf);
E void load_ns_dbase(void);
E void load_ns_req_db(void);
E void save_ns_dbase(void);
E void save_ns_req_dbase(void);
E void save_ns_rdb_dbase(void);
E void save_ns_req_rdb_dbase(void);
E int validate_user(User *u);
E void cancel_user(User *u);
E int nick_identified(User *u);
E int nick_recognized(User *u);
E void expire_nicks(void);
E void expire_requests(void);
E int ns_do_register(User * u);

E int delnick(NickAlias *na);
E NickAlias *findnick(const char *nick);
E NickCore  *findcore(const char *nick);
E void clean_ns_timeouts(NickAlias *na);

/**** helpserv.c  ****/
E void helpserv(User *u, char *buf);
E void helpserv_init(void);

/**** hostserv.c  ****/
E void hostserv_init(void);

/**** operserv.c  ****/

E SList servadmins;
E SList servopers;

E void operserv(User *u, char *buf);
E void os_init(void);
E void load_os_dbase(void);
E void save_os_dbase(void);
E void save_os_rdb_dbase(void);

E void os_remove_nick(NickCore *nc);
E int is_services_root(User *u);
E int is_services_admin(User *u);
E int is_services_oper(User *u);
E int nick_is_services_admin(NickCore *nc);
E int nick_is_services_oper(NickCore *nc);

E int add_akill(User *u, char *mask, const char *by, const time_t expires, const char *reason);
E int check_akill(const char *nick, const char *username, const char *host, const char *vhost, const char *ip);
E void expire_akills(void);
E void oper_global(char *nick, char *fmt, ...);
#ifdef IRC_BAHAMUT
E int add_sgline(User *u, char *mask, const char *by, const time_t expires, const char *reason);
E int check_sgline(const char *nick, const char *realname);
E void expire_sglines(void);
#endif

E int add_sqline(User *u, char *mask, const char *by, const time_t expires, const char *reason);
E int check_sqline(const char *nick, int nick_change);
#ifdef IRC_BAHAMUT
E int check_chan_sqline(const char *chan);
#endif
E void expire_sqlines(void);

#ifdef IRC_BAHAMUT
E int add_szline(User *u, char *mask, const char *by, const time_t expires, const char *reason);
E void expire_szlines(void);
#endif

E void check_clones(User *user);

E void delete_ignore(const char *nick);

/**** process.c ****/

E int allow_ignore;
E IgnoreData *ignore[];

E void add_ignore(const char *nick, time_t delta);
E IgnoreData *get_ignore(const char *nick);

E int split_buf(char *buf, char ***argv, int colon_special);
E void process(void);

/**** protocol.c ****/

E void s_akill(char *user, char *host, char *who, time_t when, time_t expires, char *reason);
E void s_rakill(char *user, char *host);
E void s_sgline(char *mask, char *reason);
E void s_sqline(char *mask, char *reason);
E void s_svsnoop(char *server, int set);
E void s_szline(char *mask, char *reason);
E void s_unsgline(char *mask);
E void s_unsqline(char *mask);
E void s_unszline(char *mask);

/**** proxy.c ****/

E HostCache *hcache[1024];

E void get_proxy_stats(long *nrec, long *memuse);
E void ntoa(struct in_addr addr, char *ipaddr, int len);
E int proxy_check(char *nick, char *host, uint32 ip);
E void proxy_expire();
E int proxy_init(void);

E int do_cache(User *u);

/**** send.c ****/

E void send_cmd(const char *source, const char *fmt, ...)
	FORMAT(printf,2,3);
E void vsend_cmd(const char *source, const char *fmt, va_list args)
	FORMAT(printf,2,0);

E void wallops(const char *source, const char *fmt, ...)
	FORMAT(printf,2,3);

E void notice(const char *source, const char *dest, const char *fmt, ...)
	FORMAT(printf,3,4);
E void notice_user(const char *source, User *u, const char *fmt, ...)
	FORMAT(printf,3,4);

E void notice_list(const char *source, const char *dest, const char **text);
E void notice_lang(const char *source, User *dest, int message, ...);
E void notice_help(const char *source, User *dest, int message, ...);

E void privmsg(const char *source, const char *dest, const char *fmt, ...)
	FORMAT(printf,3,4);

/**** sessions.c ****/

E void get_session_stats(long *nrec, long *memuse);
E void get_exception_stats(long *nrec, long *memuse);

E int do_session(User *u);
E int add_session(const char *nick, const char *host);
E void del_session(const char *host);

E void load_exceptions(void);
E void save_exceptions(void);
E void save_rdb_exceptions(void);
E int do_exception(User *u);
E void expire_exceptions(void);

/**** sockutil.c ****/

E int32 total_read, total_written;
E int32 read_buffer_len(void);
E int32 write_buffer_len(void);

E int sgetc(int s);
E char *sgets(char *buf, int len, int s);
E char *sgets2(char *buf, int len, int s);
E int sread(int s, char *buf, int len);
E int sputs(char *str, int s);
E int sockprintf(int s, char *fmt,...);
E int conn(const char *host, int port, const char *lhost, int lport);
E void disconn(int s);

/**** users.c ****/

E User *userlist[1024];

E int32 usercnt, opcnt, maxusercnt;
E time_t maxusertime;

E void set_umode(User *user, int ac, char **av);

E void get_user_stats(long *nusers, long *memuse);
E User *finduser(const char *nick);
E User *firstuser(void);
E User *nextuser(void);

#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_ULTIMATE3) || defined(IRC_VIAGRA) || defined(IRC_PTLINK)
E void change_user_host(User *user, const char *host);
E void change_user_username(User *user, const char *username);
E void change_user_realname(User *user, const char *realname);
#endif

E User *do_nick(const char *source, char *nick, char *username, char *host, char *server, char *realname, time_t ts, uint32 svid, ...);
E void do_umode(const char *source, int ac, char **av);
E void do_quit(const char *source, int ac, char **av);
E void do_kill(const char *source, int ac, char **av);

E int is_oper(User *user);
E int is_protected(User *user);

#if defined(IRC_ULTIMATE) || defined(IRC_UNREAL) || defined(IRC_ULTIMATE3) || defined(IRC_VIAGRA) || defined(IRC_HYBRID)
E int is_excepted(ChannelInfo *ci, User *user);
E int is_excepted_mask(ChannelInfo *ci, char *mask);
#endif

E int match_usermask(const char *mask, User *user);
E void split_usermask(const char *mask, char **nick, char **user, char **host);
E char *create_mask(User *u);

#ifdef USE_MYSQL
/**** mysql.c ****/
E MYSQL       *mysql;
E MYSQL_RES   *mysql_res;
E MYSQL_FIELD *mysql_fields;
E MYSQL_ROW   mysql_row;

E int db_mysql_init();
E int db_mysql_open();
E int db_mysql_close();
E int db_mysql_query(char *sql);
E char *db_mysql_quote(char *sql);
E void db_mysql_save_ns_core(NickCore * nc);
E void db_mysql_save_ns_alias(NickAlias * na);
E void db_mysql_save_ns_req(NickRequest * nr);
E void db_mysql_save_cs_info(ChannelInfo * ci);
E void db_mysql_save_os_db(unsigned int maxucnt, unsigned int maxutime, SList *ak, SList *sgl, SList *sql, SList *szl, HostCache *hc);
E void db_mysql_save_news(NewsItem * ni);
E void db_mysql_save_exceptions(Exception * e);
E void db_mysql_save_hs_core(HostCore * hc);
E void db_mysql_save_bs_core(BotInfo * bi);
#endif


#endif	/* EXTERN_H */

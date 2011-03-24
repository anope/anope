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

#ifndef EXTERN_H
#define EXTERN_H

#ifndef _WIN32
#define E extern
#define EI extern
#else
#ifndef MODULE_COMPILE
#define E extern __declspec(dllexport)
#define EI extern __declspec(dllimport)
#else
#define E extern __declspec(dllimport)
#define EI extern __declspec(dllexport)
#endif
#endif

#include "slist.h"

E void ModuleRunTimeDirCleanUp(void);


E char *uplink;

/* IRC Variables */

E IRCDVar *ircd;
E IRCDCAPAB *ircdcap;
E char *flood_mode_char_set;
E char *flood_mode_char_remove;
E int UseTSMODE; /* hack to get around bahamut clones that don't send TSMODE */
EI unsigned long umodes[128];
E char csmodes[128];
E CMMode cmmodes[128];
E CBMode cbmodes[128];
E CBModeInfo *cbmodeinfos;
E CUMode cumodes[128];
E char *IRCDModule;
E IRCDProto ircdproto;

/**** actions.c ****/

E void kill_user(char *source, char *user, char *reason);
E void bad_password(User * u);
E void sqline(char *mask, char *reason);
E void common_unban(ChannelInfo * ci, char *nick);
E void common_unban_full(ChannelInfo * ci, char *nick, boolean full);
E void common_svsmode(User * u, char *modes, char *arg);

/**** botserv.c ****/

E BotInfo *botlists[256];
E int nbots;
E void get_botserv_stats(long *nrec, long *memuse);
E void bs_init(void);
E void botserv(User *u, char *buf);
E void botmsgs(User *u, BotInfo *bi, char *buf);
E void botchanmsgs(User *u, ChannelInfo *ci, char *buf);
E void load_bs_dbase(void);
E void save_bs_dbase(void);
E void save_bs_rdb_dbase(void);
E BotInfo *makebot(char *nick);
E BotInfo *findbot(char *nick);
E void bot_join(ChannelInfo *ci);
E void bot_rejoin_all(BotInfo *bi);
E char *normalizeBuffer(char *);
E void unassign(User * u, ChannelInfo * ci);
E void insert_bot(BotInfo * bi);

E void bot_raw_ban(User * requester, ChannelInfo * ci, char *nick, char *reason);
E void bot_raw_kick(User * requester, ChannelInfo * ci, char *nick, char *reason);
E void bot_raw_mode(User * requester, ChannelInfo * ci, char *mode, char *nick);

/**** channels.c ****/

E Channel *chanlist[1024];

E void add_ban(Channel * chan, char *mask);
E void chan_adduser2(User * user, Channel * c);
E void add_invite(Channel * chan, char *mask);
E void chan_delete(Channel * c);
E void del_ban(Channel * chan, char *mask);
E void chan_set_throttle(Channel * chan, char *value);
E void chan_set_key(Channel * chan, char *value);
E void set_limit(Channel * chan, char *value);
E void del_invite(Channel * chan, char *mask);
E char *get_key(Channel * chan);
E char *get_limit(Channel * chan);
E Channel *chan_create(char *chan, time_t ts);
E Channel *join_user_update(User * user, Channel * chan, char *name, time_t chants);

E void add_exception(Channel * chan, char *mask);
E void del_exception(Channel * chan, char *mask);
E char *get_flood(Channel * chan);
E void set_flood(Channel * chan, char *value);
E char *get_throttle(Channel * chan);
E void set_throttle(Channel * chan, char *value);
E char *get_redirect(Channel * chan);
E void set_redirect(Channel * chan, char *value);
E char *get_unkwn(Channel * chan);
E void set_unkwn(Channel *chan, char *value);


E void get_channel_stats(long *nrec, long *memuse);
E Channel *findchan(const char *chan);
E Channel *firstchan(void);
E Channel *nextchan(void);

E void chan_deluser(User * user, Channel * c);

E int is_on_chan(Channel * c, User * u);
E User *nc_on_chan(Channel * c, NickCore * nc);

E char *chan_get_modes(Channel * chan, int complete, int plus);
E void chan_set_modes(const char *source, Channel * chan, int ac,
                      char **av, int check);

E int chan_get_user_status(Channel * chan, User * user);
E int chan_has_user_status(Channel * chan, User * user, int16 status);
E void chan_remove_user_status(Channel * chan, User * user, int16 status);
E void chan_set_user_status(Channel * chan, User * user, int16 status);

E int get_access_level(ChannelInfo * ci, NickAlias * na);
E const char *get_xop_level(int level);

E void do_cmode(const char *source, int ac, char **av);
E void do_join(const char *source, int ac, char **av);
E void do_kick(const char *source, int ac, char **av);
E void do_part(const char *source, int ac, char **av);
E void do_sjoin(const char *source, int ac, char **av);
E void do_topic(const char *source, int ac, char **av);
E void do_mass_mode(char *modes);

E void chan_set_correct_modes(User * user, Channel * c, int give_modes);
E void restore_unsynced_topics(void);

E Entry *entry_create(char *mask);
E Entry *entry_add(EList *list, char *mask);
E void entry_delete(EList *list, Entry *e);
E EList *list_create();
E int entry_match(Entry *e, char *nick, char *user, char *host, uint32 ip);
E int entry_match_mask(Entry *e, char *mask, uint32 ip);
E Entry *elist_match(EList *list, char *nick, char *user, char *host, uint32 ip);
E Entry *elist_match_mask(EList *list, char *mask, uint32 ip);
E Entry *elist_match_user(EList *list, User *u);
E Entry *elist_match_user_full(EList *list, User *u, boolean full);
E Entry *elist_find_mask(EList *list, char *mask);
E long get_memuse(EList *list);


#define whosends(ci) ((!(ci) || !((ci)->botflags & BS_SYMBIOSIS) || !(ci)->bi || !(ci)->c || (ci)->c->usercount < BSMinUsers) ? s_ChanServ : (ci)->bi->nick)

/**** chanserv.c ****/

E ChannelInfo *chanlists[256];
E CSModeUtil csmodeutils[];
E LevelInfo levelinfo[];

E void listchans(int count_only, const char *chan);
E void get_chanserv_stats(long *nrec, long *memuse);

E int delchan(ChannelInfo * ci);
E void alpha_insert_chan(ChannelInfo * ci);
E void reset_levels(ChannelInfo * ci);
E void cs_init(void);
E void chanserv(User * u, char *buf);
E void load_cs_dbase(void);
E void save_cs_dbase(void);
E void save_cs_rdb_dbase(void);
E void expire_chans(void);
E void cs_remove_nick(const NickCore * nc);
E void cs_remove_bot(const BotInfo * bi);

E int is_real_founder(User * user, ChannelInfo * ci);

E void check_modes(Channel * c);
E int check_valid_admin(User * user, Channel * chan, int servermode);
E int check_valid_op(User * user, Channel * chan, int servermode);
E int check_should_op(User * user, char *chan);
E int check_should_voice(User * user, char *chan);
E int check_should_halfop(User * user, char *chan);
E int check_should_owner(User * user, char *chan);
E int check_should_protect(User * user, char *chan);
E int check_kick(User * user, char *chan, time_t chants);
E void record_topic(const char *chan);
E void restore_topic(char *chan);
E int check_topiclock(Channel * c, time_t topic_time);

E ChannelInfo *cs_findchan(const char *chan);
E int check_access(User * user, ChannelInfo * ci, int what);
E int is_founder(User * user, ChannelInfo * ci);
E int get_access(User * user, ChannelInfo * ci);
E ChanAccess *get_access_entry(NickCore * nc, ChannelInfo * ci);
E void update_cs_lastseen(User * user, ChannelInfo * ci);
E int get_idealban(ChannelInfo * ci, User * u, char *ret, int retlen);
E AutoKick *is_stuck(ChannelInfo * ci, char *mask);
E void stick_mask(ChannelInfo * ci, AutoKick * akick);
E void stick_all(ChannelInfo * ci);
E char *cs_get_flood(ChannelInfo * ci);
E void cs_set_flood(ChannelInfo * ci, char *value);
E char *cs_get_throttle(ChannelInfo * ci);
E void cs_set_throttle(ChannelInfo * ci, char *value);
E char *cs_get_key(ChannelInfo * ci);
E void cs_set_key(ChannelInfo * ci, char *value);
E char *cs_get_limit(ChannelInfo * ci);
E void cs_set_limit(ChannelInfo * ci, char *value);
E char *cs_get_redirect(ChannelInfo * ci);
E void cs_set_redirect(ChannelInfo * ci, char *value);
E char *cs_get_unkwn(ChannelInfo * ci);
E void cs_set_unkwn(ChannelInfo * ci, char *value);

E int levelinfo_maxwidth;
E ChannelInfo *makechan(const char *chan);
E int is_identified(User * user, ChannelInfo * ci);
E char *get_mlock_modes(ChannelInfo * ci, int complete);
E void CleanAccess(ChannelInfo *ci);

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
#ifdef _WIN32
char *sockstrerror(int error);
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
E char *NetworkName;
E int   NickLen;

E char *s_NickServ;
E char *s_ChanServ;
E char *s_MemoServ;
E char *s_BotServ;
E char *s_HelpServ;
E char *s_OperServ;
E char *s_GlobalNoticer;
E char *s_DevNull;
E char *desc_NickServ;
E char *desc_ChanServ;
E char *desc_MemoServ;
E char *desc_BotServ;
E char *desc_HelpServ;
E char *desc_OperServ;
E char *desc_GlobalNoticer;
E char *desc_DevNull;

E char *HostDBName;
E char *desc_HostServ;
E char *s_HostServ;
E void load_hs_dbase(void);
E void save_hs_dbase(void);
E void save_hs_rdb_dbase(void);
E int do_on_id(User * u);
E void delHostCore(char *nick);
E void hostserv(User * u, char *buf);

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
E int   UseStrictPrivMsg;
E int   DumpCore;
E int   LogUsers;
E int   NickRegDelay;
E int   UseSVSHOLD;
E int   UseSVS2MODE;
E int   RestrictOperNicks;
E int   UseTokens;
E int   NewsCount;
E char *Numeric;
E int   UnRestrictSAdmin;
E int   UseTS6;

E char **HostSetters;
E int HostNumber;

E int   UseMail;
E char *SendMailPath;
E char *SendFrom;
E int   RestrictMail;
E int   MailDelay;
E int  DontQuoteAddresses;
E int  ForkForMail;

E int   NSDefFlags;
E int   NSDefLanguage;
E int   NSRegDelay;
E int   NSResendDelay;
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
E int   NSNickTracking;
E int   NSAddAccessOnReg;

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
E int   MSMemoReceipt;

E int   BSDefFlags;
E int   BSKeepData;
E int   BSMinUsers;
E int   BSBadWordsMax;
E int   BSSmartJoin;
E int   BSGentleBWReason;
E int   BSCaseSensitive;
E char *BSFantasyCharacter;

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
E int   KillonSGline;
E int   KillonSQline;
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
E int   AddAkiller;

E int parse_directive(Directive * d, char *dir, int ac, char *av[MAXPARAMS], int linenum, int reload, char *s);

/**
 * Modules Stuff
 **/
E char **ModulesAutoload;
E int ModulesNumber;
E char **ModulesDelayedAutoload;
E int ModulesDelayedNumber;

E char **HostServCoreModules;
E int HostServCoreNumber;

E char **HelpServCoreModules;
E int HelpServCoreNumber;

E char **MemoServCoreModules;
E int MemoServCoreNumber;

E char **BotServCoreModules;
E int BotServCoreNumber;

E char **OperServCoreModules;
E int OperServCoreNumber;

E char **NickServCoreModules;
E int NickServCoreNumber;

E char **ChanServCoreModules;
E int ChanServCoreNumber;

E int   LimitSessions;
E int   DefSessionLimit;
E int   ExceptionExpiry;
E int   MaxSessionKill;
E int   MaxSessionLimit;
E int   SessionAutoKillExpiry;
E char *ExceptionDBName;
E char *SessionLimitDetailsLoc;
E char *SessionLimitExceeded;

E char *UlineServers;
E char **Ulines;
E int NumUlines;

#ifdef USE_RDB
E int rdb_init();
E int rdb_open();
E int rdb_close();
E char *rdb_quote(char *str);
E int rdb_tag_table(char *table);
E int rdb_tag_table_where(char *table, char *clause);
E int rdb_empty_table(char *table);
E int rdb_clean_table(char *table);
E int rdb_clean_table_where(char *table, char *clause);
E int rdb_scrub_table(char *table, char *clause);
E int rdb_direct_query(char *query);
E int rdb_ns_set_display(char *newnick, char *oldnick);
E int rdb_save_ns_core(NickCore * nc);
E int rdb_save_ns_alias(NickAlias * na);
E int rdb_save_ns_req(NickRequest * nr);
E int rdb_save_cs_info(ChannelInfo * ci);
E int rdb_save_bs_core(BotInfo * bi);
E int rdb_save_hs_core(HostCore * hc);
E int rdb_save_os_db(unsigned int maxucnt, unsigned int maxutime,
                    SList * ak, SList * sgl, SList * sql, SList * szl);
E int rdb_save_news(NewsItem * ni);
E int rdb_save_exceptions(Exception * e);
E int rdb_load_bs_dbase(void);
E int rdb_load_hs_dbase(void);
E int rdb_load_ns_dbase(void);
E int rdb_load_dbases(void);
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
E int UseRDB;
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

E long unsigned int UserKey1;
E long unsigned int UserKey2;     
E long unsigned int UserKey3;

/**** encrypt.c ****/
E char *EncModule;
E int enc_encrypt(const char *src, int len, char *dest, int size);
E int enc_encrypt_check_len(int passlen, int bufsize);
E int enc_decrypt(const char *src, char *dest, int size);
E int enc_check_password(const char *plaintext, const char *password);
E void encmodule_encrypt(int (*func)(const char *src, int len, char *dest, int size));
E void encmodule_encrypt_check_len(int (*func)(int passlen, int bufsize));
E void encmodule_decrypt(int (*func)(const char *src, char *dest, int size));
E void encmodule_check_password(int (*func)(const char *plaintext, const char *password));

/**** helpserv.c  ****/
E void helpserv(User * u, char *buf);
E void helpserv_init(void);

/**** hostserv.c  ****/
E void get_hostserv_stats(long *nrec, long *memuse);
E void hostserv_init(void);
E void addHostCore(char *nick, char *vIdent, char *vhost, char *creator, int32 tmp_time);
E char *getvIdent(char *nick);
E char *getvHost(char *nick);
E int is_host_remover(User * u);
E int is_host_setter(User *u);
E HostCore *hostCoreListHead();
E HostCore *findHostCore(HostCore * head, char *nick, boolean * found);
E HostCore *createHostCorelist(HostCore * next, char *nick, char *vIdent, char *vHost, char *creator, int32 tmp_time);
E HostCore *insertHostCore(HostCore * head, HostCore * prev, char *nick, char *vIdent, char *vHost, char *creator, int32 tmp_time);
E HostCore *deleteHostCore(HostCore * head, HostCore * prev);
E void set_lastmask(User * u);

/**** init.c ****/

E void introduce_user(const char *user);
E int init_primary(int ac, char **av);
E int init_secondary(int ac, char **av);
E void init_tertiary();
E int servernum;

/**** ircd.c ****/
E void pmodule_set_mod_current_buffer(void (*func) (int ac, char **av));
E void pmodule_cmd_svsnoop(void (*func) (char *server, int set));
E void pmodule_cmd_remove_akill(void (*func) (char *user, char *host));
E void pmodule_cmd_topic(void (*func) (char *whosets, char *chan, char *whosetit, char *topic, time_t when));
E void pmodule_cmd_vhost_off(void (*func) (User * u));
E void pmodule_cmd_akill(void (*func) (char *user, char *host, char *who, time_t when, time_t expires, char *reason));
E void pmodule_cmd_svskill(void (*func) (char *source, char *user, char *buf));
E void pmodule_cmd_svsmode(void (*func) (User * u, int ac, char **av));
E void pmodule_cmd_372(void (*func) (char *source, char *msg));
E void pmodule_cmd_372_error(void (*func) (char *source));
E void pmodule_cmd_375(void (*func) (char *source));
E void pmodule_cmd_376(void (*func) (char *source));
E void pmodule_cmd_nick(void (*func) (char *nick, char *name, char *modes));
E void pmodule_cmd_guest_nick(void (*func) (char *nick, char *user, char *host, char *real, char *modes));
E void pmodule_cmd_mode(void (*func) (char *source, char *dest, char *buf));
E void pmodule_cmd_bot_nick(void (*func) (char *nick, char *user, char *host, char *real, char *modes));
E void pmodule_cmd_kick(void (*func) (char *source, char *chan, char *user, char *buf));
E void pmodule_cmd_notice_ops(void (*func) (char *source, char *dest, char *buf));
E void pmodule_cmd_notice(void (*func) (char *source, char *dest, char *buf));
E void pmodule_cmd_notice2(void (*func) (char *source, char *dest, char *msg));
E void pmodule_cmd_privmsg(void (*func) (char *source, char *dest, char *buf));
E void pmodule_cmd_privmsg2(void (*func) (char *source, char *dest, char *msg));
E void pmodule_cmd_serv_notice(void (*func) (char *source, char *dest, char *msg));
E void pmodule_cmd_serv_privmsg(void (*func) (char *source, char *dest, char *msg));
E void pmodule_cmd_bot_chan_mode(void (*func) (char *nick, char *chan));
E void pmodule_cmd_351(void (*func) (char *source));
E void pmodule_cmd_quit(void (*func) (char *source, char *buf));
E void pmodule_cmd_pong(void (*func) (char *servname, char *who));
E void pmodule_cmd_join(void (*func) (char *user, char *channel, time_t chantime));
E void pmodule_cmd_unsqline(void (*func) (char *user));
E void pmodule_cmd_invite(void (*func) (char *source, char *chan, char *nick));
E void pmodule_cmd_part(void (*func) (char *nick, char *chan, char *buf));
E void pmodule_cmd_391(void (*func) (char *source, char *timestr));
E void pmodule_cmd_250(void (*func) (char *buf));
E void pmodule_cmd_307(void (*func) (char *buf));
E void pmodule_cmd_311(void (*func) (char *buf));
E void pmodule_cmd_312(void (*func) (char *buf));
E void pmodule_cmd_317(void (*func) (char *buf));
E void pmodule_cmd_219(void (*func) (char *source, char *letter));
E void pmodule_cmd_401(void (*func) (char *source, char *who));
E void pmodule_cmd_318(void (*func) (char *source, char *who));
E void pmodule_cmd_242(void (*func) (char *buf));
E void pmodule_cmd_243(void (*func) (char *buf));
E void pmodule_cmd_211(void (*func) (char *buf));
E void pmodule_cmd_global(void (*func) (char *source, char *buf));
E void pmodule_cmd_global_legacy(void (*func) (char *source, char *fmt));
E void pmodule_cmd_sqline(void (*func) (char *mask, char *reason));
E void pmodule_cmd_squit(void (*func) (char *servname, char *message));
E void pmodule_cmd_svso(void (*func) (char *source, char *nick, char *flag));
E void pmodule_cmd_chg_nick(void (*func) (char *oldnick, char *newnick));
E void pmodule_cmd_svsnick(void (*func) (char *source, char *guest, time_t when));
E void pmodule_cmd_vhost_on(void (*func) (char *nick, char *vIdent, char *vhost));
E void pmodule_cmd_connect(void (*func) (int servernum));
E void pmodule_cmd_bob(void (*func) ());
E void pmodule_cmd_svshold(void (*func) (char *nick));
E void pmodule_cmd_release_svshold(void (*func) (char *nick));
E void pmodule_cmd_unsgline(void (*func) (char *mask));
E void pmodule_cmd_unszline(void (*func) (char *mask));
E void pmodule_cmd_szline(void (*func) (char *mask, char *reason, char *whom));
E void pmodule_cmd_sgline(void (*func) (char *mask, char *reason));
E void pmodule_cmd_unban(void (*func) (char *name, char *nick));
E void pmodule_cmd_svsmode_chan(void (*func) (char *name, char *mode, char *nick));
E void pmodule_cmd_svid_umode(void (*func) (char *nick, time_t ts));
E void pmodule_cmd_nc_change(void (*func) (User * u));
E void pmodule_cmd_svid_umode2(void (*func) (User * u, char *ts));
E void pmodule_cmd_svid_umode3(void (*func) (User * u, char *ts));
E void pmodule_cmd_ctcp(void (*func) (char *source, char *dest, char *buf));
E void pmodule_cmd_svsjoin(void (*func) (char *source, char *nick, char *chan, char *param));
E void pmodule_cmd_svspart(void (*func) (char *source, char *nick, char *chan));
E void pmodule_cmd_swhois(void (*func) (char *source, char *who, char *mask));
E void pmodule_cmd_eob(void (*func) ());
E void pmodule_cmd_jupe(void (*func) (char *jserver, char *who, char *reason));
E void pmodule_set_umode(void (*func) (User * user, int ac, char **av));
E void pmodule_valid_nick(int (*func) (char *nick));
E void pmodule_valid_chan(int (*func) (char *chan));
E void pmodule_flood_mode_check(int (*func) (char *value));
E void pmodule_jointhrottle_mode_check(int (*func) (char *value));
E void pmodule_ircd_var(IRCDVar * ircdvar);
E void pmodule_ircd_cap(IRCDCAPAB * cap);
E void pmodule_ircd_version(char *version);
E void pmodule_ircd_cbmodeinfos(CBModeInfo * modeinfos);
E void pmodule_ircd_cumodes(CUMode modes[128]);
E void pmodule_ircd_flood_mode_char_set(char *mode);
E void pmodule_ircd_flood_mode_char_remove(char *mode);
E void pmodule_ircd_cbmodes(CBMode modes[128]);
E void pmodule_ircd_cmmodes(CMMode modes[128]);
E void pmodule_ircd_csmodes(char mode[128]);
E void pmodule_ircd_useTSMode(int use);
E void pmodule_invis_umode(int mode);
E void pmodule_oper_umode(int mode);
E void pmodule_invite_cmode(int mode);
E void pmodule_secret_cmode(int mode);
E void pmodule_private_cmode(int mode);
E void pmodule_key_mode(int mode);
E void pmodule_limit_mode(int mode);
E void pmodule_permchan_mode(int mode);

E int anope_get_secret_mode();
E int anope_get_invite_mode();
E int anope_get_key_mode();
E int anope_get_limit_mode();
E int anope_get_private_mode();
E int anope_get_invis_mode();
E int anope_get_oper_mode();
E int anope_get_permchan_mode();

/**** language.c ****/

E char **langtexts[NUM_LANGS];
E char *langnames[NUM_LANGS];
E int langlist[NUM_LANGS];

E void lang_init(void);
#define getstring(na,index) \
	(langtexts[((na)&&((NickAlias*)na)->nc&&!(((NickAlias*)na)->status & NS_VERBOTEN)?((NickAlias*)na)->nc->language:NSDefLanguage)][(index)])
#define getstring2(nc,index) \
	(langtexts[((nc)?((NickCore*)nc)->language:NSDefLanguage)][(index)])
E int strftime_lang(char *buf, int size, User * u, int format,
                    struct tm *tm);
E void syntax_error(char *service, User * u, const char *command,
                    int msgnum);


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
E MailInfo *MailMemoBegin(NickCore * nc);
E void MailEnd(MailInfo *mail);
E void MailReset(User *u, NickCore *nc);
E int MailValidate(const char *email);

/**** main.c ****/

E const char version_number[];
E const char version_number_dotted[];
E const char version_build[];
E char *version_protocol;
E const char version_flags[];

E char *services_dir;
E char *log_filename;
E int   debug;
E int   readonly;
E int   logchan;
E int   skeleton;
E int   nofork;
E int   forceload;
E int   nothird;
E int	noexpire;
E int   protocoldebug;

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
E void expire_all(void);
E void do_backtrace(int show_segheader);
E void sighandler(int signum);
E void do_restart_services(void);

/**** memory.c ****/

E void *smalloc(long size);
E void *scalloc(long elsize, long els);
E void *srealloc(void *oldptr, long newsize);
E char *sstrdup(const char *s);


/**** memoserv.c ****/

E void ms_init(void);
E void memoserv(User * u, char *buf);
E void check_memos(User * u);
E MemoInfo *getmemoinfo(const char *name, int *ischan, int *isforbid);
E void memo_send(User * u, char *name, char *text, int z);
E void memo_send_from(User * u, char *name, char *text, int z, char *source);
E int delmemo(MemoInfo * mi, int num);

/**** messages.c ****/

E int m_nickcoll(char *user);
E int m_away(char *source, char *msg);
E int m_kill(char *nick, char *msg);
E int m_motd(char *source);
E int m_privmsg(char *source, char *receiver, char *msg);
E int m_stats(char *source, int ac, char **av);
E int m_whois(char *source, char *who);
E int m_time(char *source, int ac, char **av);
E int m_version(char *source, int ac, char **av);


/**** misc.c ****/

E int toupper(char);
E int tolower(char);
E char *strscpy(char *d, const char *s, size_t len);
#ifndef HAVE_STRLCPY
E size_t strlcpy(char *, const char *, size_t);
#endif
#ifndef HAVE_STRLCAT
E size_t strlcat(char *, const char *, size_t);
#endif
E char *stristr(char *s1, char *s2);
E char *strnrepl(char *s, int32 size, const char *old, const char *new);
E char *merge_args(int argc, char **argv);
E int match_wild(const char *pattern, const char *str);
E int match_wild_nocase(const char *pattern, const char *str);
E int dotime(const char *s);
E char *duration(NickAlias * na, char *buf, int bufsize, time_t seconds);
E char *expire_left(NickAlias * na, char *buf, int len, time_t expires);
E void protocol_debug(char *source, char *cmd, int argc, char **argv);
E int doValidHost(const char *host, int type);

typedef int (*range_callback_t) (User * u, int num, va_list args);
E int process_numlist(const char *numstr, int *count_ret,
                      range_callback_t callback, User * u, ...);

E int isValidHost(const char *host, int type);
E int isvalidchar(const char c);

E char *myStrGetToken(const char *str, const char dilim, int token_number);
E char *myStrGetOnlyToken(const char *str, const char dilim,
        int token_number);
E char *myStrSubString(const char *src, int start, int end);
E char *myStrGetTokenRemainder(const char *str, const char dilim,
        int token_number);
E char *stripModePrefix(const char *str);
E int myNumToken(const char *str, const char dilim);
E void doCleanBuffer(char *str);
E void EnforceQlinedNick(char *nick, char *killer);
E int nickIsServices(char *nick, int bot);

E void add_entropy_userkeys(void);
E void rand_init(void);
E unsigned char getrandom8(void);
E u_int16_t getrandom16(void);
E u_int32_t getrandom32(void);

E char *str_signed(unsigned char *str);

E void ntoa(struct in_addr addr, char *ipaddr, int len);

E char **buildStringList(char *src, int *number);
E void binary_to_hex(unsigned char *bin, char *hex, int length);

E uint32 cidr_to_netmask(uint16 cidr);
E uint16 netmask_to_cidr(uint32 mask);

E int str_is_wildcard(const char *str);
E int str_is_pure_wildcard(const char *str);

E uint32 str_is_ip(char *str);
E int str_is_cidr(char *str, uint32 * ip, uint32 * mask, char **host);


/**** modules.c ****/
E void modules_core_init(int number, char **list);
E void modules_unload_all(boolean fini, boolean unload_proto);	/* Read warnings near function source */
E void moduleCallBackRun(void);
E void moduleCleanStruct(ModuleData **moduleData);
E void ModuleDatabaseBackup(char *dbname);
E void ModuleRemoveBackups(char *dbname);

/**** news.c ****/

E int32 nnews, news_size;
E NewsItem *news;
E void get_news_stats(long *nrec, long *memuse);
E void load_news(void);
E void save_news(void);
E void save_rdb_news(void);
E void display_news(User * u, int16 type);
E int do_logonnews(User * u);
E int do_opernews(User * u);
E int do_randomnews(User * u);

/**** nickserv.c ****/

E NickAlias *nalists[1024];
E NickCore *nclists[1024];
E NickRequest *nrlists[1024];
E NickRequest *findrequestnick(const char *nick);
E int delnickrequest(NickRequest * nr);
E unsigned int guestnum;
E void insert_requestnick(NickRequest * nr);
E void alpha_insert_alias(NickAlias * na);
E void insert_core(NickCore * nc);
E void listnicks(int count_only, const char *nick);
E void get_aliases_stats(long *nrec, long *memuse);
E void get_core_stats(long *nrec, long *memuse);
E void collide(NickAlias * na, int from_timeout);
E void del_ns_timeout(NickAlias * na, int type);
E void change_core_display(NickCore * nc, char *newdisplay);
E void release(NickAlias * na, int from_timeout);
E int do_setmodes(User * u);
E int should_mode_change(int16 status, int16 mode);

E void ns_init(void);
E void nickserv(User * u, char *buf);
E void load_ns_dbase(void);
E void load_ns_req_db(void);
E void save_ns_dbase(void);
E void save_ns_req_dbase(void);
E void save_ns_rdb_dbase(void);
E void save_ns_req_rdb_dbase(void);
E int validate_user(User * u);
E void cancel_user(User * u);
E int nick_identified(User * u);
E int nick_recognized(User * u);
E void expire_nicks(void);
E void expire_requests(void);
EI int ns_do_register(User * u);
E int delnick(NickAlias * na);
E NickAlias *findnick(const char *nick);
E NickCore  *findcore(const char *nick);
E void clean_ns_timeouts(NickAlias * na);
E void nsStartNickTracking(User * u);
E void nsStopNickTracking(User * u);
E int nsCheckNickTracking(User *u);

E int group_identified(User * u, NickCore * nc);
E int is_on_access(User * u, NickCore * nc);

/**** operserv.c  ****/

E SList akills, sglines, sqlines, szlines;
E SList servadmins;
E SList servopers;

E int DefConModesSet;
E uint32 DefConModesOn;
E uint32 DefConModesOff;
E ChannelInfo DefConModesCI;

E void operserv(User *u, char *buf);
E void os_init(void);
E void load_os_dbase(void);
E void save_os_dbase(void);
E void save_os_rdb_dbase(void);

E void os_remove_nick(NickCore *nc);
E int is_services_root(User *u);
E int is_services_admin(User *u);
E int is_services_oper(User *u);
E int nick_is_services_root(NickCore * nc);
E int nick_is_services_admin(NickCore *nc);
E int nick_is_services_oper(NickCore *nc);

E int add_akill(User *u, char *mask, const char *by, const time_t expires, const char *reason);
E int check_akill(char *nick, const char *username, const char *host, const char *vhost, const char *ip);
E void expire_akills(void);
E void oper_global(char *nick, char *fmt, ...);

E int add_sgline(User *u, char *mask, const char *by, const time_t expires, const char *reason);
E int check_sgline(char *nick, const char *realname);
E void expire_sglines(void);

E int add_sqline(User *u, char *mask, const char *by, const time_t expires, const char *reason);
E int check_sqline(char *nick, int nick_change);
E void expire_sqlines(void);
E int check_chan_sqline(const char *chan);

E int add_szline(User * u, char *mask, const char *by,
                 const time_t expires, const char *reason);
E void expire_szlines(void);
E int check_szline(char *nick, char *ip);

E Server *server_global(Server * s, char *msg);

E int OSOpersOnly; 
E time_t DefContimer;
E void runDefCon(void);
E int defconParseModeString(const char *str);

/**** process.c ****/

E int allow_ignore;
E IgnoreData *ignore;

E void add_ignore(const char *nick, time_t delta);
E IgnoreData *get_ignore(const char *nick);
E int delete_ignore(const char *nick);
E int clear_ignores();

E int split_buf(char *buf, char ***argv, int colon_special);
E void process(void);

/**** send.c ****/

E void send_cmd(const char *source, const char *fmt, ...)
	FORMAT(printf,2,3);
E void vsend_cmd(const char *source, const char *fmt, va_list args)
	FORMAT(printf,2,0);

E void notice_server(char *source, Server * s, char *fmt, ...)
	FORMAT(printf,3,4);
E void notice_user(char *source, User *u, const char *fmt, ...)
	FORMAT(printf,3,4);

E void notice_list(char *source, char *dest, char **text);
E void notice_lang(char *source, User *dest, int message, ...);
E void notice_help(char *source, User *dest, int message, ...);


/**** servers.c ****/

E Server *servlist;
E Server *me_server;
E Server *serv_uplink;
E uint32 uplink_capab;
E CapabInfo capab_info[];

E Server *first_server(int flags);
E Server *next_server(int flags);

E int is_ulined(char *server);
E int is_sync(Server *server);

E Server *new_server(Server * uplink, const char *name, const char *desc,
                   uint16 flags, char *suid);

E Server *findserver(Server *s, const char *name);

E void do_server(const char *source, char *servername, char *hops, char *descript, char *numeric);
E void do_squit(const char *source, int ac, char **av);
E void capab_parse(int ac, char **av);
E int anope_check_sync(const char *name);

E void finish_sync(Server *serv, int sync_links);

E void ts6_uid_init(void);
E void ts6_uid_increment(unsigned int slot);
E char *ts6_uid_retrieve(void);

/**** sessions.c ****/

E Exception *exceptions;
E int16 nexceptions;

E Session *sessionlist[1024];
E int32 nsessions;

E void get_session_stats(long *nrec, long *memuse);
E void get_exception_stats(long *nrec, long *memuse);

E int do_session(User *u);
E int add_session(char *nick, char *host, char *hostip);
E void del_session(const char *host);

E void load_exceptions(void);
E void save_exceptions(void);
E void save_rdb_exceptions(void);
E int do_exception(User *u);
E void expire_exceptions(void);

E Session *findsession(const char *host);

E Exception *find_host_exception(const char *host);
E Exception *find_hostip_exception(const char *host, const char *hostip);
E int exception_add(User * u, const char *mask, const int limit,
                         const char *reason, const char *who,
                         const time_t expires);

/**** slist.c ****/
E int slist_add(SList *slist, void *item);
E void slist_clear(SList *slist, int free);
E int slist_delete(SList *slist, int index);
E int slist_delete_range(SList *slist, char *range, slist_delcheckcb_t cb, ...);
E int slist_enum(SList *slist, char *range, slist_enumcb_t cb, ...);
E int slist_full(SList *slist);
E int slist_indexof(SList *slist, void *item);
E void slist_init(SList *slist);
E void slist_pack(SList *slist);
E int slist_remove(SList *slist, void *item);
E int slist_setcapacity(SList *slist, int16 capacity);

/**** sockutil.c ****/

E int32 total_read, total_written;
E int32 read_buffer_len(void);
E int32 write_buffer_len(void);

E int sgetc(ano_socket_t s);
E char *sgets(char *buf, int len, ano_socket_t s);
E char *sgets2(char *buf, int len, ano_socket_t s);
E int sread(ano_socket_t s, char *buf, int len);
E int sputs(char *str, ano_socket_t s);
E int sockprintf(ano_socket_t s, char *fmt, ...);
E int conn(const char *host, int port, const char *lhost, int lport);
E void disconn(ano_socket_t s);

/**** users.c ****/

E User *userlist[1024];

E int32 usercnt, opcnt;
E uint32 maxusercnt;
E time_t maxusertime;

E void delete_user(User *user);

E void get_user_stats(long *nusers, long *memuse);
E User *finduser(const char *nick);
E User *firstuser(void);
E User *nextuser(void);

E User *find_byuid(const char *uid);
E User *first_uid(void);
E User *next_uid(void);
E Uid *new_uid(const char *nick, char *uid);
E Uid *find_uid(const char *nick);
E Uid *find_nickuid(const char *uid);
E Server *findserver_uid(Server * s, const char *name);
E char *TS6SID;
E char *TS6UPLINK;

E void update_host(User * user);
E void change_user_host(User * user, const char *host);
E void change_user_username(User * user, const char *username);
E void change_user_realname(User * user, const char *realname);

E User *do_nick(const char *source, char *nick, char *username, char *host,
              char *server, char *realname, time_t ts, uint32 svid, uint32 ip, char *vhost, char *uid);

E void do_umode(const char *source, int ac, char **av);
E void do_umode2(const char *source, int ac, char **av);
E void do_quit(const char *source, int ac, char **av);
E void do_kill(char *source, char *reason);

E int is_oper(User * user);
E int is_protected(User * user);

E int is_excepted(ChannelInfo * ci, User * user);
E int is_excepted_mask(ChannelInfo * ci, char *mask);

E int match_usermask(const char *mask, User * user);
E int match_usermask_full(const char *mask, User * user, boolean full);
E int match_userip(const char *mask, User * user, char *host);
E void split_usermask(const char *mask, char **nick, char **user,
                      char **host);
E char *create_mask(User * u);

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
E char *db_mysql_quote_buffer(char *sql, int size);
E int db_mysql_try(const char *fmt, ...);
E int db_mysql_save_ns_core(NickCore * nc);
E int db_mysql_save_ns_alias(NickAlias * na);
E int db_mysql_save_ns_req(NickRequest * nr);
E int db_mysql_save_cs_info(ChannelInfo * ci);
E int db_mysql_save_os_db(unsigned int maxucnt, unsigned int maxutime,
                           SList * ak, SList * sgl, SList * sql,
                           SList * szl);
E int db_mysql_save_news(NewsItem * ni);
E int db_mysql_save_exceptions(Exception * e);
E int db_mysql_save_hs_core(HostCore * hc);
E int db_mysql_save_bs_core(BotInfo * bi);
E int db_mysql_load_bs_dbase(void);
E int db_mysql_load_hs_dbase(void);
E int db_mysql_load_ns_dbase(void);
E int db_mysql_load_ns_req_dbase(void);
E int db_mysql_load_cs_dbase(void);
E int db_mysql_load_os_dbase(void);
E int db_mysql_load_exceptions(void);
E int db_mysql_load_news(void);
E unsigned int mysql_rand(void);
#endif

E void privmsg(char *source, char *dest, const char *fmt, ...);
E void notice(char *source, char *dest, const char *fmt, ...);

/******************************************************************************/

E int anope_set_mod_current_buffer(int ac, char **av);

E void anope_cmd_211(const char *fmt, ...);                          	  		  /* 211 */
E void anope_cmd_219(char *source, char *who); 			  	  		  /* 219 */
E void anope_cmd_242(const char *fmt, ...);                          	  		  /* 242 */
E void anope_cmd_243(const char *fmt, ...);                          	  		  /* 243 */
E void anope_cmd_250(const char *fmt, ...);			  	  		  /* 250 */
E void anope_cmd_307(const char *fmt, ...);			    	  		  /* 307 */
E void anope_cmd_311(const char *fmt, ...);                          	  		  /* 311 */
E void anope_cmd_312(const char *fmt, ...);                          	  		  /* 312 */
E void anope_cmd_317(const char *fmt, ...);                          	  		  /* 317 */
E void anope_cmd_318(char *source, char *who);           		  	  		  /* 318 */
E void anope_cmd_351(char *source);				  	  		  /* 351 */
E void anope_cmd_372(char *source, char *msg);			 	  		  /* 372 */
E void anope_cmd_372_error(char *source);				  	  		  /* 372 */
E void anope_cmd_375(char *source);				 	  		  /* 375 */
E void anope_cmd_376(char *source);				 	  		  /* 376 */
E void anope_cmd_391(char *source, char *timestr);                             		  /* 391 */
E void anope_cmd_401(char *source, char *who);			  	  		  /* 401 */
E void anope_cmd_akill(char *user, char *host, char *who, time_t when, time_t expires, char *reason); /* AKILL */
E void anope_cmd_vhost_on(char *nick, char *vIdent, char *vhost);    			  /* CHGHOST + CHGIDENT */
E void anope_cmd_vhost_off(User *u);
E void anope_cmd_connect(int servernum);                             	           	  /* Connect */
E void anope_cmd_bob();
E void anope_cmd_global(char *source, const char *fmt, ...);         	  		  /* GLOBOPS */
E void anope_cmd_invite(char *source, char *chan, char *nick);       	  		  /* INVITE */
E void anope_cmd_join(char *user, char *channel, time_t chantime);   	  		  /* JOIN */
E void anope_cmd_kick(char *source, char *chan, char *user, const char *fmt, ...);		  /* KICK */
E void anope_cmd_mode(char *source, char *dest, const char *fmt, ...);   	  		  /* MODE */
E void anope_cmd_unban(char *name, char *nick);				  		  			      /* MODE -b */
E void anope_cmd_bot_chan_mode(char *nick, char *chan);			  		  /* MODE BotServ */
E void anope_cmd_nick(char *nick, char *name, char *mode);			  		  /* NICK */
E void anope_cmd_chg_nick(char *oldnick, char *newnick);             	  		  /* NICK */
E void anope_cmd_bot_nick(char *nick, char *user,char *host,char *real,char *modes);	  /* NICK */
E void anope_cmd_guest_nick(char *nick, char *user,char *host,char *real,char *modes);	  /* NICK */
E void anope_cmd_notice(char *source, char *dest, const char *fmt, ...);     		  /* NOTICE */
E void anope_cmd_notice_ops(char *source, char *dest, const char *fmt, ...); 		  /* NOTICE */
E void anope_cmd_notice2(char *source, char *dest, char *msg);		  		  /* NOTICE */
E void anope_cmd_serv_notice(char *source, char *dest, char *msg);		  		  /* NOTICE */
E void anope_cmd_part(char *nick, char *chan, const char *fmt, ...); 	  		  /* PART */
E void anope_cmd_pong(char *servname, char *who);                    	  		  /* PONG */
E void anope_cmd_privmsg(char *source, char *dest, const char *fmt, ...);    		  /* PRIVMSG */
E void anope_cmd_action(char *source, char *dest, const char *fmt, ...);    		  /* PRIVMSG */
E void anope_cmd_privmsg2(char *source, char *dest, char *msg);		  		  /* PRIVMSG */
E void anope_cmd_serv_privmsg(char *source, char *dest, char *msg);	  		  /* PRIVMSG */
E void anope_cmd_quit(char *source, const char *fmt, ...);           	  		  /* QUIT */
E void anope_cmd_remove_akill(char *user, char *host);			  		  /* RAKILL */
E void anope_cmd_sgline(char *mask, char *reason);			  	  		  /* SGLINE */
E void anope_cmd_sqline(char *mask, char *reason);                   	  		  /* SQLINE */
E void anope_cmd_szline(char *mask, char *reason, char *whom);				  /* SZLINE */
E void anope_cmd_squit(char *servname, char *message);               	  		  /* SQUIT  */
E void anope_cmd_svshold(char *nick);				  	  		  /* SVSHOLD */
E void anope_cmd_release_svshold(char *nick);				  		  /* SVSHOLD */
E void anope_cmd_svsjoin(char *source, char *nick,char *chan, char *param);          	  /* SVSJOIN */
E void anope_cmd_svskill(char *source,char *user, const char *fmt, ...);     		  /* SVSKILL */
E void anope_cmd_svsmode(User * u, int ac, char **av);   	        	  		  /* SVSMODE */
E void anope_cmd_svsmode_chan(char *name, char *mode, char *nick);				  /* SVSMODE */
E void anope_cmd_svsnick(char *nick,char *newnick, time_t when);     	  		  /* SVSNICK */
E void anope_cmd_svsnoop(char *server, int set);			  	  		  /* SVSNOOP */
E void anope_cmd_svso(char *source,char *nick, char *flag);          	  		  /* SVSO   */
E void anope_cmd_svspart(char *source, char *nick,char *chan);          	  		  /* SVSPART   */
E void anope_cmd_swhois(char *source, char *who, char *mask);	  	  		  /* SWHOIS */
E void anope_cmd_topic(char *whosets, char *chan, char *whosetit, char *topic, time_t when); /* TOPIC */
E void anope_cmd_unsgline(char *mask);				  	  		  /* UNSGLINE */
E void anope_cmd_unsqline(char *user);                               	  		  /* UNSQLINE */
E void anope_cmd_unszline(char *mask);				  	  		  /* UNSZLINE */
E void anope_cmd_eob();									  /* EOB - end of burst */
E void anope_cmd_ctcp(char *source, char *dest, const char *fmt, ...);   	  		  /* CTCP */

EI int anope_event_482(char *source, int ac, char **av);
EI int anope_event_436(char *source, int ac, char **av);
EI int anope_event_away(char *source, int ac, char **av);
EI int anope_event_ping(char *source, int ac, char **av);
EI int anope_event_motd(char *source, int ac, char **av);
EI int anope_event_join(char *source, int ac, char **av);
EI int anope_event_kick(char *source, int ac, char **av);
EI int anope_event_kill(char *source, int ac, char **av);
EI int anope_event_mode(char *source, int ac, char **av);
EI int anope_event_tmode(char *source, int ac, char **av);
EI int anope_event_quit(char *source, int ac, char **av);
EI int anope_event_squit(char *source, int ac, char **av);
EI int anope_event_topic(char *source, int ac, char **av);
EI int anope_event_whois(char *source, int ac, char **av);
EI int anope_event_part(char *source, int ac, char **av);
EI int anope_event_server(char *source, int ac, char **av);
EI int anope_event_sid(char *source, int ac, char **av);
EI int anope_event_nick(char *source, int ac, char **av);
EI int anope_event_bmask(char *source, int ac, char **av);
EI int anope_event_gnotice(char *source, int ac, char **av);
EI int anope_event_privmsg(char *source, int ac, char **av);
EI int anope_event_capab(char *source, int ac, char **av);
EI int anope_event_sjoin(char *source, int ac, char **av);
EI int anope_event_cs(char *source, int ac, char **av);
EI int anope_event_hs(char *source, int ac, char **av);
EI int anope_event_ms(char *source, int ac, char **av);
EI int anope_event_ns(char *source, int ac, char **av);
EI int anope_event_os(char *source, int ac, char **av);
EI int anope_event_vs(char *source, int ac, char **av);
EI int anope_event_svinfo(char *source, int ac, char **av);
EI int anope_event_chghost(char *source, int ac, char **av);
EI int anope_event_sethost(char *source, int ac, char **av);
EI int anope_event_chgident(char *source, int ac, char **av);
EI int anope_event_setident(char *source, int ac, char **av);
EI int anope_event_chgname(char *source, int ac, char **av);
EI int anope_event_setname(char *source, int ac, char **av);
EI int anope_event_svsinfo(char *source, int ac, char **av);
EI int anope_event_snick(char *source, int ac, char **av);
EI int anope_event_vhost(char *source, int ac, char **av);
EI int anope_event_tkl(char *source, int ac, char **av);
EI int anope_event_eos(char *source, int ac, char **av);
EI int anope_event_eob(char *source, int ac, char **av);
EI int anope_event_pass(char *source, int ac, char **av);
EI int anope_event_netinfo(char *source, int ac, char **av);
EI int anope_event_error(char *source, int ac, char **av);
EI int anope_event_netctrl(char *source, int ac, char **av);
EI int anope_event_notice(char *source, int ac, char **av);
EI int anope_event_snotice(char *source, int ac, char **av);
EI int anope_event_sqline(char *source, int ac, char **av);
EI int anope_event_smo(char *source, int ac, char **av);
EI int anope_event_myid(char *source, int ac, char **av);
EI int anope_event_vctrl(char *source, int ac, char **av);
EI int anope_event_tctrl(char *source, int ac, char **av);
EI int anope_event_snetinfo(char *source, int ac, char **av);
EI int anope_event_umode2(char *source, int ac, char **av);
EI int anope_event_globops(char *source, int ac, char **av);
EI int anope_event_swhois(char *source, int ac, char **av);
EI int anope_event_burst(char *source, int ac, char **av);
EI int anope_event_luserslock(char *source, int ac, char **av);
EI int anope_event_admin(char *source, int ac, char **av);
EI int anope_event_credits(char *source, int ac, char **av);
EI int anope_event_rehash(char *source, int ac, char **av);
EI int anope_event_sdesc(char *source, int ac, char **av);
EI int anope_event_netglobal(char *source, int ac, char **av);
EI int anope_event_invite(char *source, int ac, char **av);
E int anope_event_null(char *source, int ac, char **av);

E void anope_set_umode(User * user, int ac, char **av);
E void anope_cmd_svid_umode(char *nick, time_t ts);
E void anope_cmd_svid_umode2(User *u, char *ts);
E void anope_cmd_svid_umode3(User *u, char *ts);
E void anope_cmd_nc_change(User *u);
E int anope_flood_mode_check(char *value);
E int anope_jointhrottle_mode_check(char *value);

E void anope_cmd_jupe(char *jserver, char *who, char *reason);

E void anope_cmd_global_legacy(char *source, char *fmt);
E void wallops(char *source, const char *fmt, ...);

E int anope_valid_nick(char *nick);
E int anope_valid_chan(char *chan);

E char *common_get_vident(User *u);
E char *common_get_vhost(User *u);
E char *send_token(char *token1, char *token2);
E char *base64enc(long i);
E long base64dec(char *b64);
E long base64dects(char *ts);
E int b64_encode(char *src, size_t srclength, char *target, size_t targsize);
E int b64_decode(char *src, char *target, size_t targsize);
E char *encode_ip(unsigned char *ip);
E int decode_ip(char *buf);

#define Anope_Free(x)       if ((x) != NULL) free(x)

E char *host_resolve(char *host);

E void event_message_process(char *eventbuf);
E void eventprintf(char *fmt, ...);
E void event_process_hook(const char *name, int argc, char **argv);
E void send_event(const char *name, int argc, ...);

#ifdef _WIN32
E char *GetWindowsVersion(void) ;
E int SupportedWindowsVersion(void);
#endif

#endif	/* EXTERN_H */

/* Prototypes and external variable declarations.
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */

#ifndef EXTERN_H
#define EXTERN_H

#define E extern CoreExport
#define EI extern DllExport

#include "slist.h"

E void ModuleRunTimeDirCleanUp();


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
E IRCDProto *ircdproto;

/**** actions.c ****/

E void kill_user(const char *source, const char *user, const char *reason);
E void bad_password(User * u);
E void sqline(char *mask, char *reason);
E void common_unban(ChannelInfo * ci, char *nick);
E void common_svsmode(User * u, const char *modes, const char *arg);

/**** botserv.c ****/

E BotInfo *botlists[256];
E int nbots;
E void get_botserv_stats(long *nrec, long *memuse);
E void bs_init();
E void botserv(User *u, char *buf);
E void botmsgs(User *u, BotInfo *bi, char *buf);
E void botchanmsgs(User *u, ChannelInfo *ci, char *buf);
E void load_bs_dbase();
E void save_bs_dbase();
E BotInfo *findbot(const char *nick);

/** Finds a pseudoclient, given a UID. Useful for TS6 protocol modules.
 * @param uid The UID to search for
 * @return The pseudoclient structure, or NULL if one could not be found
 */
E BotInfo *findbot_byuid(const char *uid);
E void bot_join(ChannelInfo *ci);
E char *normalizeBuffer(const char *);
E void insert_bot(BotInfo * bi);

E void bot_raw_ban(User * requester, ChannelInfo * ci, char *nick, const char *reason);
E void bot_raw_kick(User * requester, ChannelInfo * ci, char *nick, const char *reason);
E void bot_raw_mode(User * requester, ChannelInfo * ci, const char *mode, char *nick);

/**** channels.c ****/

E Channel *chanlist[1024];

E void add_ban(Channel * chan, const char *mask);
E void chan_adduser2(User * user, Channel * c);
E void add_invite(Channel * chan, const char *mask);
E void chan_delete(Channel * c);
E void del_ban(Channel * chan, const char *mask);
E void chan_set_key(Channel * chan, const char *value);
E void set_limit(Channel * chan, const char *value);
E void del_invite(Channel * chan, const char *mask);
E char *get_key(Channel * chan);
E char *get_limit(Channel * chan);
E Channel *chan_create(const char *chan, time_t ts);
E Channel *join_user_update(User * user, Channel * chan, const char *name, time_t chants);

E void add_exception(Channel * chan, const char *mask);
E void del_exception(Channel * chan, const char *mask);
E char *get_flood(Channel * chan);
E void set_flood(Channel * chan, const char *value);
E char *get_redirect(Channel * chan);
E void set_redirect(Channel * chan, const char *value);


E void get_channel_stats(long *nrec, long *memuse);
E Channel *findchan(const char *chan);
E Channel *firstchan();
E Channel *nextchan();

E void chan_deluser(User * user, Channel * c);

E int is_on_chan(Channel * c, User * u);
E User *nc_on_chan(Channel * c, NickCore * nc);

E char *chan_get_modes(Channel * chan, int complete, int plus);
E void chan_set_modes(const char *source, Channel * chan, int ac,
					  const char **av, int check);

E int chan_get_user_status(Channel * chan, User * user);
E int chan_has_user_status(Channel * chan, User * user, int16 status);
E void chan_remove_user_status(Channel * chan, User * user, int16 status);
E void chan_set_user_status(Channel * chan, User * user, int16 status);

E int get_access_level(ChannelInfo * ci, NickAlias * na);
E const char *get_xop_level(int level);

E void do_cmode(const char *source, int ac, const char **av);
E void do_join(const char *source, int ac, const char **av);
E void do_kick(const char *source, int ac, const char **av);
E void do_part(const char *source, int ac, const char **av);
E void do_sjoin(const char *source, int ac, const char **av);
E void do_topic(const char *source, int ac, const char **av);
E void do_mass_mode(char *modes);

E void chan_set_correct_modes(User * user, Channel * c, int give_modes);
E void restore_unsynced_topics();

E Entry *entry_create(char *mask);
E Entry *entry_add(EList *list, const char *mask);
E void entry_delete(EList *list, Entry *e);
E EList *list_create();
E int entry_match(Entry *e, const char *nick, const char *user, const char *host, uint32 ip);
E int entry_match_mask(Entry *e, const char *mask, uint32 ip);
E Entry *elist_match(EList *list, const char *nick, const char *user, const char *host, uint32 ip);
E Entry *elist_match_mask(EList *list, const char *mask, uint32 ip);
E Entry *elist_match_user(EList *list, User *u);
E Entry *elist_find_mask(EList *list, const char *mask);
E long get_memuse(EList *list);


#define whosends(ci) ((!(ci) || !((ci)->botflags & BS_SYMBIOSIS) || !(ci)->bi || !(ci)->c || (ci)->c->usercount < BSMinUsers) ? findbot(s_ChanServ) : (ci)->bi)

/**** chanserv.c ****/

E ChannelInfo *chanlists[256];
E CSModeUtil csmodeutils[];
E LevelInfo levelinfo[];

E void get_chanserv_stats(long *nrec, long *memuse);

E int delchan(ChannelInfo * ci);
E void alpha_insert_chan(ChannelInfo * ci);
E void reset_levels(ChannelInfo * ci);
E void cs_init();
E void chanserv(User * u, char *buf);
E void load_cs_dbase();
E void save_cs_dbase();
E void expire_chans();
E void cs_remove_nick(const NickCore * nc);

E int is_real_founder(User * user, ChannelInfo * ci);

E void check_modes(Channel * c);
E int check_valid_admin(User * user, Channel * chan, int servermode);
E int check_valid_op(User * user, Channel * chan, int servermode);
E int check_should_op(User * user, char *chan);
E int check_should_voice(User * user, char *chan);
E int check_should_halfop(User * user, char *chan);
E int check_should_owner(User * user, char *chan);
E int check_should_protect(User * user, char *chan);
E int check_kick(User * user, const char *chan, time_t chants);
E void record_topic(const char *chan);
E void restore_topic(const char *chan);
E int check_topiclock(Channel * c, time_t topic_time);

E ChannelInfo *cs_findchan(const char *chan);
E int check_access(User * user, ChannelInfo * ci, int what);
E int is_founder(User * user, ChannelInfo * ci);
E int get_access(User * user, ChannelInfo * ci);
E ChanAccess *get_access_entry(NickCore * nc, ChannelInfo * ci);
E void update_cs_lastseen(User * user, ChannelInfo * ci);
E int get_idealban(ChannelInfo * ci, User * u, char *ret, int retlen);
E AutoKick *is_stuck(ChannelInfo * ci, const char *mask);
E void stick_mask(ChannelInfo * ci, AutoKick * akick);
E void stick_all(ChannelInfo * ci);
E char *cs_get_flood(ChannelInfo * ci);
E void cs_set_flood(ChannelInfo * ci, const char *value);
E char *cs_get_key(ChannelInfo * ci);
E void cs_set_key(ChannelInfo * ci, const char *value);
E char *cs_get_limit(ChannelInfo * ci);
E void cs_set_limit(ChannelInfo * ci, const char *value);
E char *cs_get_redirect(ChannelInfo * ci);
E void cs_set_redirect(ChannelInfo * ci, const char *value);

E int levelinfo_maxwidth;
E ChannelInfo *makechan(const char *chan);
E int is_identified(User * user, ChannelInfo * ci);
E char *get_mlock_modes(ChannelInfo * ci, int complete);

/**** compat.c ****/

#if !HAVE_STRICMP && !HAVE_STRCASECMP
E int stricmp(const char *s1, const char *s2);
E int strnicmp(const char *s1, const char *s2, size_t len);
#endif
#ifdef _WIN32
char *sockstrerror(int error);
#endif

/**** config.c ****/

E ServerConfig serverConfig;

E std::list<Uplink *> Uplinks;
E char *LocalHost;
E unsigned LocalPort;

E char *ServerName;
E char *ServerDesc;
E char *ServiceUser;
E char *ServiceHost;

E char *HelpChannel;
E char *LogChannel;
E char *NetworkName;
E unsigned NickLen;

E char *s_NickServ;
E char *s_ChanServ;
E char *s_MemoServ;
E char *s_BotServ;
E char *s_OperServ;
E char *s_GlobalNoticer;
E char *desc_NickServ;
E char *desc_ChanServ;
E char *desc_MemoServ;
E char *desc_BotServ;
E char *desc_OperServ;
E char *desc_GlobalNoticer;

E char *HostDBName;
E char *desc_HostServ;
E char *s_HostServ;
E void load_hs_dbase();
E void save_hs_dbase();
E int do_on_id(User * u);
E void delHostCore(const char *nick);
E void hostserv(User * u, char *buf);

E char *PIDFilename;
E char *MOTDFilename;
E char *NickDBName;
E char *PreNickDBName;
E char *ChanDBName;
E char *BotDBName;
E char *OperDBName;
E char *NewsDBName;

E bool  NoBackupOkay;
E bool  StrictPasswords;
E unsigned BadPassLimit;
E time_t BadPassTimeout;
E time_t UpdateTimeout;
E time_t ExpireTimeout;
E time_t ReadTimeout;
E time_t WarningTimeout;
E time_t TimeoutCheck;
E int   KeepLogs;
E int   KeepBackups;
E bool  ForceForbidReason;
E bool  UsePrivmsg;
E bool  UseStrictPrivMsg;
E bool  DumpCore;
E bool  LogUsers;
E unsigned NickRegDelay;
E bool   RestrictOperNicks;
E unsigned NewsCount;
E char *Numeric;

E char **HostSetters;
E int HostNumber;

E bool  UseMail;
E char *SendMailPath;
E char *SendFrom;
E bool  RestrictMail;
E time_t MailDelay;
E bool  DontQuoteAddresses;

E int   NSDefFlags;
E unsigned NSDefLanguage;
E time_t NSRegDelay;
E time_t NSResendDelay;
E time_t NSExpire;
E time_t NSRExpire;
E bool  NSForceEmail;
E int   NSMaxAliases;
E unsigned NSAccessMax;
E char *NSEnforcerUser;
E char *NSEnforcerHost;
E time_t NSReleaseTimeout;
E bool  NSAllowKillImmed;
E bool  NSNoGroupChange;
E bool  NSListOpersOnly;
E unsigned NSListMax;
E char *NSGuestNickPrefix;
E bool  NSSecureAdmins;
E bool  NSStrictPrivileges;
E bool  NSEmailReg;
E bool  NSModeOnID;
E bool  NSRestrictGetPass;
E bool  NSAddAccessOnReg;

E int   CSDefFlags;
E unsigned CSMaxReg;
E time_t CSExpire;
E int   CSDefBantype;
E unsigned CSAccessMax;
E unsigned CSAutokickMax;
E char *CSAutokickReason;
E time_t CSInhabit;
E bool  CSListOpersOnly;
E unsigned CSListMax;
E bool  CSRestrictGetPass;
E bool  CSOpersOnly;

E unsigned MSMaxMemos;
E time_t MSSendDelay;
E bool  MSNotifyAll;
E unsigned MSMemoReceipt;

E int   BSDefFlags;
E time_t BSKeepData;
E unsigned BSMinUsers;
E unsigned BSBadWordsMax;
E bool  BSSmartJoin;
E bool  BSGentleBWReason;
E bool  BSCaseSensitive;
E char *BSFantasyCharacter;

E bool  HideStatsO;
E bool  GlobalOnCycle;
E bool  AnonymousGlobal;
E char *GlobalOnCycleMessage;
E char *GlobalOnCycleUP;
E char **ServicesRoots;
E int   RootNumber;
E bool  LogMaxUsers;
E bool  SuperAdmin;
E bool  LogBot;
E time_t AutokillExpiry;
E time_t ChankillExpiry;
E time_t SGLineExpiry;
E time_t SQLineExpiry;
E time_t SZLineExpiry;
E bool  AkillOnAdd;
E bool  KillonSGline;
E bool  KillonSQline;
E bool  WallOper;
E bool  WallBadOS;
E bool  WallOSGlobal;
E bool  WallOSMode;
E bool  WallOSClearmodes;
E bool  WallOSKick;
E bool  WallOSAkill;
E bool  WallOSSGLine;
E bool  WallOSSQLine;
E bool  WallOSSZLine;
E bool  WallOSNoOp;
E bool  WallOSJupe;
E bool  WallAkillExpire;
E bool  WallSGLineExpire;
E bool  WallSQLineExpire;
E bool  WallSZLineExpire;
E bool  WallExceptionExpire;
E bool  WallDrop;
E bool  WallForbid;
E bool  WallGetpass;
E bool  WallSetpass;
E bool  AddAkiller;

/**
 * Modules Stuff
 **/
E char **ModulesAutoload;
E int ModulesNumber;
E char **ModulesDelayedAutoload;
E int ModulesDelayedNumber;

E char **HostServCoreModules;
E int HostServCoreNumber;

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

E bool  LimitSessions;
E unsigned DefSessionLimit;
E time_t ExceptionExpiry;
E int   MaxSessionKill;
E unsigned MaxSessionLimit;
E time_t SessionAutoKillExpiry;
E char *ExceptionDBName;
E char *SessionLimitDetailsLoc;
E char *SessionLimitExceeded;

E char **Ulines;
E int NumUlines;

E int read_config(int reload);

E int DefConLevel;
E int DefCon[6];
E int checkDefCon(int level);
E void resetDefCon(int level);
E int DefConSessionLimit;
E time_t DefConTimeOut;
E time_t DefConAKILL;
E char *DefConChanModes;
E bool GlobalOnDefcon;
E bool GlobalOnDefconMore;
E char *DefconMessage;
E char *DefConAkillReason;
E char *DefConOffMessage;

E long unsigned int UserKey1;
E long unsigned int UserKey2;
E long unsigned int UserKey3;
/**** converter.c ****/

E int convert_ircservices_44();

/**** encrypt.c ****/
E char *EncModule;
E void initEncryption();
E int enc_encrypt(const char *src, int len, char *dest, int size);
E int enc_encrypt_in_place(char *buf, int size);
E int enc_encrypt_check_len(int passlen, int bufsize);
E int enc_decrypt(const char *src, char *dest, int size);
E int enc_check_password(const char *plaintext, const char *password);
E void encmodule_encrypt(int (*func)(const char *src, int len, char *dest, int size));
E void encmodule_encrypt_in_place(int (*func)(char *buf, int size));
E void encmodule_encrypt_check_len(int (*func)(int passlen, int bufsize));
E void encmodule_decrypt(int (*func)(const char *src, char *dest, int size));
E void encmodule_check_password(int (*func)(const char *plaintext, const char *password));

/**** hostserv.c  ****/
E void get_hostserv_stats(long *nrec, long *memuse);
E void hostserv_init();
E void addHostCore(const char *nick, char *vIdent, char *vhost, const char *creator, int32 tmp_time);
E char *getvIdent(char *nick);
E char *getvHost(char *nick);
E int is_host_remover(User * u);
E int is_host_setter(User *u);
E HostCore *hostCoreListHead();
E HostCore *findHostCore(HostCore * head, const char *nick, bool *found);
E HostCore *createHostCorelist(HostCore * next, const char *nick, char *vIdent, char *vHost, const char *creator, int32 tmp_time);
E HostCore *insertHostCore(HostCore * head, HostCore * prev, const char *nick, char *vIdent, char *vHost, const char *creator, int32 tmp_time);
E HostCore *deleteHostCore(HostCore * head, HostCore * prev);
E void set_lastmask(User * u);

/**** init.c ****/

E void introduce_user(const char *user);
E int init_primary(int ac, char **av);
E int init_secondary(int ac, char **av);
E Uplink *uplink_server;

/**** ircd.c ****/
E void pmodule_ircd_proto(IRCDProto *);
E void pmodule_ircd_var(IRCDVar * ircdvar);
E void pmodule_ircd_cap(IRCDCAPAB * cap);
E void pmodule_ircd_version(const char *version);
E void pmodule_ircd_cbmodeinfos(CBModeInfo * modeinfos);
E void pmodule_ircd_cumodes(CUMode modes[128]);
E void pmodule_ircd_flood_mode_char_set(const char *mode);
E void pmodule_ircd_flood_mode_char_remove(const char *mode);
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

E int anope_get_secret_mode();
E int anope_get_invite_mode();
E int anope_get_key_mode();
E int anope_get_limit_mode();
E int anope_get_private_mode();
E int anope_get_invis_mode();
E int anope_get_oper_mode();

/**** language.c ****/

E char **langtexts[NUM_LANGS];
E char *langnames[NUM_LANGS];
E int langlist[NUM_LANGS];

E void lang_init();
E int strftime_lang(char *buf, int size, User * u, int format, struct tm *tm);
E void syntax_error(char *service, User * u, const char *command, int msgnum);
E const char *getstring(NickAlias *na, int index);
E const char *getstring(NickCore *nc, int index);
E const char *getstring(User *nc, int index);
E const char *getstring(int index);


/**** log.c ****/

E int open_log();
E void close_log();
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

E std::string services_dir;
E const char *log_filename;
E int   debug;
E int   readonly;
E int   logchan;
E int   nofork;
E int   forceload;
E int   nothird;
E int	noexpire;
E int   protocoldebug;

E int   is44;
E int   quitting;
E int   delayed_quit;
E const char *quitmsg;
E char  inbuf[BUFSIZE];
E int   servsock;
E int   save_data;
E int   got_alarm;
E time_t start_time;

E void save_databases();
E void expire_all();
E void sighandler(int signum);
E void do_restart_services();

/**** memory.c ****/

E void *smalloc(long size);
E void *scalloc(long elsize, long els);
E void *srealloc(void *oldptr, long newsize);
E char *sstrdup(const char *s);


/**** memoserv.c ****/

E void ms_init();
E void memoserv(User * u, char *buf);
E void check_memos(User * u);
E MemoInfo *getmemoinfo(const char *name, int *ischan, int *isforbid);
E void memo_send(User * u, const char *name, const char *text, int z);
E int delmemo(MemoInfo * mi, int num);

/**** messages.c ****/

E int m_nickcoll(const char *user);
E int m_away(const char *source, const char *msg);
E int m_kill(const char *nick, const char *msg);
E int m_motd(const char *source);
E int m_privmsg(const char *source, const char *receiver, const char *msg);
E int m_stats(const char *source, int ac, const char **av);
E int m_whois(const char *source, const char *who);
E int m_time(const char *source, int ac, const char **av);
E int m_version(const char *source, int ac, const char **av);


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
E char *strnrepl(char *s, int32 size, const char *old, const char *nstr);
E const char *merge_args(int argc, char **argv);
E const char *merge_args(int argc, const char **argv);

E int dotime(const char *s);
E const char *duration(NickCore *nc, char *buf, int bufsize, time_t seconds);
E const char *expire_left(NickCore *nc, char *buf, int len, time_t expires);
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
E void EnforceQlinedNick(const char *nick, const char *killer);
E int nickIsServices(const char *nick, int bot);

E void add_entropy_userkeys();
E void rand_init();
E unsigned char getrandom8();
E uint16 getrandom16();
E uint32 getrandom32();

E char *str_signed(unsigned char *str);

E void ntoa(struct in_addr addr, char *ipaddr, int len);

E char **buildStringList(const std::string &src, int *number);
E void binary_to_hex(unsigned char *bin, char *hex, int length);

E uint32 cidr_to_netmask(uint16 cidr);
E uint16 netmask_to_cidr(uint32 mask);

E int str_is_wildcard(const char *str);
E int str_is_pure_wildcard(const char *str);

E uint32 str_is_ip(char *str);
E int str_is_cidr(char *str, uint32 * ip, uint32 * mask, char **host);


/**** modules.c ****/
E void modules_unload_all(bool fini, bool unload_proto);	/* Read warnings near function source */
E void ModuleDatabaseBackup(const char *dbname);
E void ModuleRemoveBackups(const char *dbname);

/**** news.c ****/

/* Add news items. */
E int add_newsitem(User * u, const char *text, int16 type);
/* Delete news items. */
E int del_newsitem(int num, int16 type);
E int32 nnews, news_size;
E NewsItem *news;
E void get_news_stats(long *nrec, long *memuse);
E void load_news();
E void save_news();
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
E void get_aliases_stats(long *nrec, long *memuse);
E void get_core_stats(long *nrec, long *memuse);
E void collide(NickAlias * na, int from_timeout);
E void del_ns_timeout(NickAlias * na, int type);
E void change_core_display(NickCore * nc);
E void change_core_display(NickCore * nc, const char *newdisplay);
E void release(NickAlias * na, int from_timeout);
E int do_setmodes(User * u);
E int should_mode_change(int16 status, int16 mode);

E void ns_init();
E void nickserv(User * u, char *buf);
E void load_ns_dbase();
E void load_ns_req_db();
E void save_ns_dbase();
E void save_ns_req_dbase();
E int validate_user(User * u);
E void cancel_user(User * u);
E int nick_identified(User * u);
E int nick_recognized(User * u);
E void expire_nicks();
E void expire_requests();
EI int ns_do_register(User * u);
E int delnick(NickAlias * na);
E NickAlias *findnick(const char *nick);
E NickAlias *findnick(const std::string &nick);
E NickCore  *findcore(const char *nick);
E void clean_ns_timeouts(NickAlias * na);
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
E void os_init();
E void load_os_dbase();
E void save_os_dbase();

E void os_remove_nick(NickCore *nc);
E int is_services_root(User *u);
E int is_services_admin(User *u);
E int is_services_oper(User *u);
E int nick_is_services_root(NickCore * nc);
E int nick_is_services_admin(NickCore *nc);
E int nick_is_services_oper(NickCore *nc);

E int add_akill(User *u, const char *mask, const char *by, const time_t expires, const char *reason);
E int check_akill(const char *nick, const char *username, const char *host, const char *vhost, const char *ip);
E void expire_akills();
E void oper_global(char *nick, const char *fmt, ...);

E int add_sgline(User *u, const char *mask, const char *by, time_t expires, const char *reason);
E int check_sgline(const char *nick, const char *realname);
E void expire_sglines();

E int add_sqline(User *u, const char *mask, const char *by, time_t expires, const char *reason);
E int check_sqline(const char *nick, int nick_change);
E void expire_sqlines();
E int check_chan_sqline(const char *chan);

E int add_szline(User * u, const char *mask, const char *by,
				 time_t expires, const char *reason);
E void expire_szlines();
E int check_szline(const char *nick, char *ip);

E Server *server_global(Server * s, char *msg);

E bool OSOpersOnly;
E time_t DefContimer;
E void runDefCon();
E int defconParseModeString(const char *str);

/**** process.c ****/

E int allow_ignore;
E IgnoreData *ignore;

E void add_ignore(const char *nick, time_t delta);
E IgnoreData *get_ignore(const char *nick);
E int delete_ignore(const char *nick);
E int clear_ignores();

E int split_buf(char *buf, const char ***argv, int colon_special);
E void process();

/**** send.c ****/

E void send_cmd(const char *source, const char *fmt, ...) FORMAT(printf,2,3);
E void send_cmd(const std::string &source, const char *fmt, ...) FORMAT(printf,2,3);

E void notice_server(char *source, Server * s, const char *fmt, ...)
	FORMAT(printf,3,4);

E void notice_list(const char *source, const char *dest, char **text); // MARK_DEPRECATED;
E void notice_lang(const char *source, User *dest, int message, ...); // MARK_DEPRECATED;
E void notice_help(const char *source, User *dest, int message, ...); // MARK_DEPRECATED;


/**** servers.c ****/

E Server *servlist;
E Server *me_server;
E Server *serv_uplink;
E uint32 uplink_capab;
E CapabInfo capab_info[];

E Server *first_server(int flags);
E Server *next_server(int flags);

E int is_ulined(const char *server);
E int is_sync(Server *server);

E Server *new_server(Server * uplink, const char *name, const char *desc,
				   uint16 flags, const char *suid);

E Server *findserver(Server *s, const char *name);

E void do_server(const char *source, const char *servername, const char *hops, const char *descript, const char *numeric);
E void do_squit(const char *source, int ac, const char **av);
E void capab_parse(int ac, const char **av);
E int anope_check_sync(const char *name);

E void finish_sync(Server *serv, int sync_links);

E void ts6_uid_init();
E void ts6_uid_increment(unsigned int slot);
E const char *ts6_uid_retrieve();

/**** sessions.c ****/

E Exception *exceptions;
E int16 nexceptions;

E Session *sessionlist[1024];
E int32 nsessions;

E void get_session_stats(long *nrec, long *memuse);
E void get_exception_stats(long *nrec, long *memuse);

E int do_session(User *u);
E int add_session(const char *nick, const char *host, char *hostip);
E void del_session(const char *host);

E void load_exceptions();
E void save_exceptions();
E int do_exception(User *u);
E void expire_exceptions();

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
E int slist_delete_range(SList *slist, const char *range, slist_delcheckcb_t cb, ...);
E int slist_enum(SList *slist, const char *range, slist_enumcb_t cb, ...);
E int slist_full(SList *slist);
E int slist_indexof(SList *slist, void *item);
E void slist_init(SList *slist);
E void slist_pack(SList *slist);
E int slist_remove(SList *slist, void *item);
E int slist_setcapacity(SList *slist, int16 capacity);

/**** sockutil.c ****/

E int32 total_read, total_written;
E int32 read_buffer_len();
E int32 write_buffer_len();

E int sgetc(ano_socket_t s);
E char *sgets(char *buf, int len, ano_socket_t s);
E char *sgets2(char *buf, int len, ano_socket_t s);
E int sread(ano_socket_t s, char *buf, int len);
E int sputs(char *str, ano_socket_t s);
E int sockprintf(ano_socket_t s, const char *fmt, ...);
E int conn(const char *host, int port, const char *lhost, int lport);
E void disconn(ano_socket_t s);

/**** users.c ****/

E User *userlist[1024];

E int32 opcnt;
E uint32 maxusercnt, usercnt;
E time_t maxusertime;

E void get_user_stats(long *nusers, long *memuse);
E User *finduser(const char *nick);
E User *firstuser();
E User *nextuser();

E User *find_byuid(const char *uid);
E User *first_uid();
E User *next_uid();
E Server *findserver_uid(Server * s, const char *name);
E char *TS6SID;
E char *TS6UPLINK;

E void update_host(User * user);

E User *do_nick(const char *source, const char *nick, const char *username, const char *host,
			  const char *server, const char *realname, time_t ts, uint32 ip, const char *vhost, const char *uid);

E void do_umode(const char *source, int ac, const char **av);
E void do_quit(const char *source, int ac, const char **av);
E void do_kill(const char *source, const char *reason);

E int is_oper(User * user);
E int is_protected(User * user);

E int is_excepted(ChannelInfo * ci, User * user);
E int is_excepted_mask(ChannelInfo * ci, const char *mask);

E int match_usermask(const char *mask, User * user);
E void split_usermask(const char *mask, char **nick, char **user,
					  char **host);
E char *create_mask(User * u);


/******************************************************************************/

E const char* base64enc(long i);
E long base64dec(char *b64);
E long base64dects(const char *ts);
E int b64_encode(char *src, size_t srclength, char *target, size_t targsize);
E int b64_decode(const char *src, char *target, size_t targsize);
E const char* encode_ip(unsigned char *ip);
E int decode_ip(const char *buf);

E char *host_resolve(char *host);

E void event_process_hook(const char *name, int argc, char **argv);
E void send_event(const char *name, int argc, ...);

#ifdef _WIN32
E char *GetWindowsVersion() ;
E int SupportedWindowsVersion();
#endif

#endif	/* EXTERN_H */

/* Configuration file handling.
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

#include "services.h"

/*************************************************************************/

/* Configurable variables: */

char *IRCDModule;
char *EncModule;
char *RemoteServer;
int RemotePort;
char *RemotePassword;

char *RemoteServer2;
int RemotePort2;
char *RemotePassword2;

char *RemoteServer3;
int RemotePort3;
char *RemotePassword3;

char *LocalHost;
int LocalPort;

char *ServerName;
char *ServerDesc;
char *ServiceUser;
char *ServiceHost;
static char *temp_userhost;

char *HelpChannel;
char *LogChannel;
char *NetworkName;
int NickLen;

char *s_NickServ;
char *s_ChanServ;
char *s_MemoServ;
char *s_BotServ;
char *s_HelpServ;
char *s_OperServ;
char *s_GlobalNoticer;
char *s_DevNull;
char *desc_NickServ;
char *desc_ChanServ;
char *desc_MemoServ;
char *desc_BotServ;
char *desc_HelpServ;
char *desc_OperServ;
char *desc_GlobalNoticer;
char *desc_DevNull;

char *HostDBName;               /* Name of HostServ DB File */
char *s_HostServ;               /* HostServ Name */
char *desc_HostServ;            /* HostServ Description */

char *s_NickServAlias;
char *s_ChanServAlias;
char *s_MemoServAlias;
char *s_BotServAlias;
char *s_HelpServAlias;
char *s_OperServAlias;
char *s_GlobalNoticerAlias;
char *s_DevNullAlias;
char *s_HostServAlias;
char *desc_NickServAlias;
char *desc_ChanServAlias;
char *desc_MemoServAlias;
char *desc_BotServAlias;
char *desc_HelpServAlias;
char *desc_OperServAlias;
char *desc_GlobalNoticerAlias;
char *desc_DevNullAlias;
char *desc_HostServAlias;

char *PIDFilename;
char *MOTDFilename;
char *NickDBName;
char *PreNickDBName;
char *ChanDBName;
char *BotDBName;
char *OperDBName;
char *AutokillDBName;
char *NewsDBName;

char *HostSetter;
char **HostSetters;
int HostNumber = 0;             /* needs to be set to 0 */

int NoBackupOkay;
int StrictPasswords;
int BadPassLimit;
int BadPassTimeout;
int UpdateTimeout;
int ExpireTimeout;
int ReadTimeout;
int WarningTimeout;
int TimeoutCheck;
int KeepLogs;
int KeepBackups;
int ForceForbidReason;
int UsePrivmsg;
int UseStrictPrivMsg;
int DumpCore;
int LogUsers;
int NickRegDelay;
int UseSVSHOLD;
int UseTokens;
int UseSVS2MODE;
int NewsCount;

int UseMail;
char *SendMailPath;
char *SendFrom;
int RestrictMail;
int MailDelay;
int DontQuoteAddresses;
int ForkForMail;

static int NSDefNone;
char *NSGuestNickPrefix;
int NSAllowKillImmed;
int NSNoGroupChange;
int NSDefKill;
int NSDefKillQuick;
int NSDefSecure;
int NSDefPrivate;
int NSDefMsg;
int NSDefHideEmail;
int NSDefHideUsermask;
int NSDefHideQuit;
int NSDefMemoSignon;
int NSDefMemoReceive;
int NSDefFlags;
int NSDefLanguage;
int NSDefAutoop;
int NSRegDelay;
int NSResendDelay;
int NSExpire;
int NSRExpire;
int NSForceEmail;
int NSMaxAliases;
int NSAccessMax;
char *NSEnforcerUser;
char *NSEnforcerHost;
static char *temp_nsuserhost;
int NSReleaseTimeout;
int NSListOpersOnly;
int NSListMax;
int NSSecureAdmins;
int NSStrictPrivileges;
int NSEmailReg;
int NSModeOnID;
int NSRestrictGetPass;
int NSNickTracking;
int NSAddAccessOnReg;

int CSDefNone;
int CSDefKeepTopic;
int CSDefOpNotice;
int CSDefPeace;
int CSDefPrivate;
int CSDefRestricted;
int CSDefSecure;
int CSDefSecureOps;
int CSDefSecureFounder;
int CSDefSignKick;
int CSDefSignKickLevel;
int CSDefTopicLock;
int CSDefXOP;
int CSDefFlags;
int CSMaxReg;
int CSExpire;
int CSDefBantype;
int CSAccessMax;
int CSAutokickMax;
char *CSAutokickReason;
int CSInhabit;
int CSListOpersOnly;
int CSListMax;
int CSRestrictGetPass;
int CSOpersOnly;

int MSMaxMemos;
int MSSendDelay;
int MSNotifyAll;
int MSMemoReceipt;

int BSDefDontKickOps;
int BSDefDontKickVoices;
int BSDefFantasy;
int BSDefGreet;
int BSDefSymbiosis;
int BSDefFlags;
int BSKeepData;
int BSMinUsers;
int BSBadWordsMax;
int BSSmartJoin;
int BSGentleBWReason;
int BSCaseSensitive;
char *BSFantasyCharacter;

int HideStatsO;
int GlobalOnCycle;
int AnonymousGlobal;
int RestrictOperNicks;
char *GlobalOnCycleMessage;
char *GlobalOnCycleUP;
char *ServicesRoot;
char **ServicesRoots;
int RootNumber;
int SuperAdmin;
int LogBot;
int LogMaxUsers;
int DisableRaw;
int AutokillExpiry;
int ChankillExpiry;
int SGLineExpiry;
int SQLineExpiry;
int SZLineExpiry;
int AkillOnAdd;
int KillonSGline;
int KillonSQline;
int WallOper;
int WallBadOS;
int WallOSGlobal;
int WallOSMode;
int WallOSClearmodes;
int WallOSKick;
int WallOSAkill;
int WallOSSGLine;
int WallOSSQLine;
int WallOSSZLine;
int WallOSNoOp;
int WallOSJupe;
int WallOSRaw;
int WallAkillExpire;
int WallSGLineExpire;
int WallSQLineExpire;
int WallSZLineExpire;
int WallExceptionExpire;
int WallDrop;
int WallForbid;
int WallGetpass;
int WallSetpass;
int AddAkiller;

int LimitSessions;
int DefSessionLimit;
int ExceptionExpiry;
int MaxSessionKill;
int MaxSessionLimit;
int SessionAutoKillExpiry;
char *ExceptionDBName;
char *SessionLimitExceeded;
char *SessionLimitDetailsLoc;

int OSOpersOnly;

char *Modules;
char *ModulesDelayed;
char **ModulesAutoload;
int ModulesNumber;
int ModulesDelayedNumber;
char **ModulesDelayedAutoload;

/**
 * Core Module Stuff 
 **/
char *HostCoreModules;
char **HostServCoreModules;
int HostServCoreNumber;

char *MemoCoreModules;
char **MemoServCoreModules;
int MemoServCoreNumber;

char *HelpCoreModules;
char **HelpServCoreModules;
int HelpServCoreNumber;

char *BotCoreModules;
char **BotServCoreModules;
int BotServCoreNumber;

char *OperCoreModules;
char **OperServCoreModules;
int OperServCoreNumber;

char *NickCoreModules;
char **NickServCoreModules;
int NickServCoreNumber;

char *ChanCoreModules;
char **ChanServCoreModules;
int ChanServCoreNumber;


char *MysqlHost;
char *MysqlUser;
char *MysqlPass;
char *MysqlName;
int MysqlPort;
char *MysqlSecure;
char *MysqlSock;
int MysqlRetries = 0;
int MysqlRetryGap = 0;
int UseRDB = 0;

int DefConLevel;
int DefCon1;
int DefCon2;
int DefCon3;
int DefCon4;
int DefCon5;
int DefCon[6];
char *DefConTimeOut;
int DefConSessionLimit;
char *DefConAKILL;
char *DefConChanModes;
int GlobalOnDefcon;
int GlobalOnDefconMore;
char *DefConOffMessage;
char *DefconMessage;
char *DefConAkillReason;

long unsigned int UserKey1;
long unsigned int UserKey2;
long unsigned int UserKey3;

char *Numeric;

int UnRestrictSAdmin;

char *UlineServers;
char **Ulines;
int NumUlines;

int UseTS6;


/*************************************************************************/

/* Deprecated directive (dep_) and value checking (chk_) functions: */

/* Hey, there are no left! -GD */

/*************************************************************************/

Directive directives[] = {
    {"AkillOnAdd", {{PARAM_SET, PARAM_RELOAD, &AkillOnAdd}}},
    {"AutokillDB", {{PARAM_STRING, PARAM_RELOAD, &AutokillDBName}}},
    {"AutokillExpiry", {{PARAM_TIME, PARAM_RELOAD, &AutokillExpiry}}},
    {"ChankillExpiry", {{PARAM_TIME, PARAM_RELOAD, &ChankillExpiry}}},
    {"BadPassLimit", {{PARAM_POSINT, PARAM_RELOAD, &BadPassLimit}}},
    {"BadPassTimeout", {{PARAM_TIME, PARAM_RELOAD, &BadPassTimeout}}},
    {"BotCoreModules", {{PARAM_STRING, PARAM_RELOAD, &BotCoreModules}}},
    {"BotServDB", {{PARAM_STRING, PARAM_RELOAD, &BotDBName}}},
    {"BotServName", {{PARAM_STRING, 0, &s_BotServ},
                     {PARAM_STRING, 0, &desc_BotServ}}},
    {"BotServAlias", {{PARAM_STRING, 0, &s_BotServAlias},
                      {PARAM_STRING, 0, &desc_BotServAlias}}},
    {"BSBadWordsMax", {{PARAM_POSINT, PARAM_RELOAD, &BSBadWordsMax}}},
    {"BSDefDontKickOps", {{PARAM_SET, PARAM_RELOAD, &BSDefDontKickOps}}},
    {"BSDefDontKickVoices",
     {{PARAM_SET, PARAM_RELOAD, &BSDefDontKickVoices}}},
    {"BSDefGreet", {{PARAM_SET, PARAM_RELOAD, &BSDefGreet}}},
    {"BSDefFantasy", {{PARAM_SET, PARAM_RELOAD, &BSDefFantasy}}},
    {"BSDefSymbiosis", {{PARAM_SET, PARAM_RELOAD, &BSDefSymbiosis}}},
    {"BSCaseSensitive", {{PARAM_SET, PARAM_RELOAD, &BSCaseSensitive}}},
    {"BSFantasyCharacter",
     {{PARAM_STRING, PARAM_RELOAD, &BSFantasyCharacter}}},
    {"BSGentleBWReason", {{PARAM_SET, PARAM_RELOAD, &BSGentleBWReason}}},
    {"BSKeepData", {{PARAM_TIME, PARAM_RELOAD, &BSKeepData}}},
    {"BSMinUsers", {{PARAM_POSINT, PARAM_RELOAD, &BSMinUsers}}},
    {"BSSmartJoin", {{PARAM_SET, PARAM_RELOAD, &BSSmartJoin}}},
    {"HostServDB", {{PARAM_STRING, PARAM_RELOAD, &HostDBName}}},
    {"HostServName", {{PARAM_STRING, 0, &s_HostServ},
                      {PARAM_STRING, 0, &desc_HostServ}}},
    {"ChanCoreModules", {{PARAM_STRING, PARAM_RELOAD, &ChanCoreModules}}},
    {"ChanServDB", {{PARAM_STRING, PARAM_RELOAD, &ChanDBName}}},
    {"ChanServName", {{PARAM_STRING, 0, &s_ChanServ},
                      {PARAM_STRING, 0, &desc_ChanServ}}},
    {"ChanServAlias", {{PARAM_STRING, 0, &s_ChanServAlias},
                       {PARAM_STRING, 0, &desc_ChanServAlias}}},
    {"CSAccessMax", {{PARAM_POSINT, PARAM_RELOAD, &CSAccessMax}}},
    {"CSAutokickMax", {{PARAM_POSINT, PARAM_RELOAD, &CSAutokickMax}}},
    {"CSAutokickReason",
     {{PARAM_STRING, PARAM_RELOAD, &CSAutokickReason}}},
    {"CSDefBantype", {{PARAM_INT, PARAM_RELOAD, &CSDefBantype}}},
    {"CSDefNone", {{PARAM_SET, PARAM_RELOAD, &CSDefNone}}},
    {"CSDefKeepTopic", {{PARAM_SET, PARAM_RELOAD, &CSDefKeepTopic}}},
    {"CSDefOpNotice", {{PARAM_SET, PARAM_RELOAD, &CSDefOpNotice}}},
    {"CSDefPeace", {{PARAM_SET, PARAM_RELOAD, &CSDefPeace}}},
    {"CSDefPrivate", {{PARAM_SET, PARAM_RELOAD, &CSDefPrivate}}},
    {"CSDefRestricted", {{PARAM_SET, PARAM_RELOAD, &CSDefRestricted}}},
    {"CSDefSecure", {{PARAM_SET, PARAM_RELOAD, &CSDefSecure}}},
    {"CSDefSecureOps", {{PARAM_SET, PARAM_RELOAD, &CSDefSecureOps}}},
    {"CSDefSecureFounder",
     {{PARAM_SET, PARAM_RELOAD, &CSDefSecureFounder}}},
    {"CSDefSignKick", {{PARAM_SET, PARAM_RELOAD, &CSDefSignKick}}},
    {"CSDefSignKickLevel",
     {{PARAM_SET, PARAM_RELOAD, &CSDefSignKickLevel}}},
    {"CSDefTopicLock", {{PARAM_SET, PARAM_RELOAD, &CSDefTopicLock}}},
    {"CSDefXOP", {{PARAM_SET, PARAM_RELOAD, &CSDefXOP}}},
    {"CSExpire", {{PARAM_TIME, PARAM_RELOAD, &CSExpire}}},
    {"CSInhabit", {{PARAM_TIME, PARAM_RELOAD, &CSInhabit}}},
    {"CSListMax", {{PARAM_POSINT, PARAM_RELOAD, &CSListMax}}},
    {"CSListOpersOnly", {{PARAM_SET, PARAM_RELOAD, &CSListOpersOnly}}},
    {"CSMaxReg", {{PARAM_POSINT, 0, &CSMaxReg}}},
    {"CSRestrictGetPass", {{PARAM_SET, PARAM_RELOAD, &CSRestrictGetPass}}},
    {"CSOpersOnly", {{PARAM_SET, PARAM_RELOAD, &CSOpersOnly}}},
    {"DefSessionLimit", {{PARAM_POSINT, 0, &DefSessionLimit}}},
    {"DevNullName", {{PARAM_STRING, 0, &s_DevNull},
                     {PARAM_STRING, 0, &desc_DevNull}}},
    {"DevNullAlias", {{PARAM_STRING, 0, &s_DevNullAlias},
                      {PARAM_STRING, 0, &desc_DevNullAlias}}},
    {"DisableRaw", {{PARAM_SET, PARAM_RELOAD, &DisableRaw}}},
    {"DontQuoteAddresses",
     {{PARAM_SET, PARAM_RELOAD, &DontQuoteAddresses}}},
    {"ForkForMail", {{PARAM_SET, PARAM_RELOAD, &ForkForMail}}},
    {"DumpCore", {{PARAM_SET, 0, &DumpCore}}},
    {"DefConLevel", {{PARAM_INT, PARAM_RELOAD, &DefConLevel}}},
    {"DefCon1", {{PARAM_INT, PARAM_RELOAD, &DefCon1}}},
    {"DefCon2", {{PARAM_INT, PARAM_RELOAD, &DefCon2}}},
    {"DefCon3", {{PARAM_INT, PARAM_RELOAD, &DefCon3}}},
    {"DefCon4", {{PARAM_INT, PARAM_RELOAD, &DefCon4}}},
    {"DefConSessionLimit",
     {{PARAM_INT, PARAM_RELOAD, &DefConSessionLimit}}},
    {"DefConAkillExpire", {{PARAM_STRING, PARAM_RELOAD, &DefConAKILL}}},
    {"DefConChanModes", {{PARAM_STRING, PARAM_RELOAD, &DefConChanModes}}},
    {"DefConTimeOut", {{PARAM_STRING, PARAM_RELOAD, &DefConTimeOut}}},
    {"DefConAkillReason",
     {{PARAM_STRING, PARAM_RELOAD, &DefConAkillReason}}},
    {"DefConOffMessage",
     {{PARAM_STRING, PARAM_RELOAD, &DefConOffMessage}}},
    {"EncModule", {{PARAM_STRING, 0, &EncModule}}},
    {"ExceptionDB", {{PARAM_STRING, PARAM_RELOAD, &ExceptionDBName}}},
    {"ExceptionExpiry", {{PARAM_TIME, PARAM_RELOAD, &ExceptionExpiry}}},
    {"ExpireTimeout", {{PARAM_TIME, PARAM_RELOAD, &ExpireTimeout}}},
    {"ForceForbidReason", {{PARAM_SET, PARAM_RELOAD, &ForceForbidReason}}},
    {"GlobalName", {{PARAM_STRING, 0, &s_GlobalNoticer},
                    {PARAM_STRING, 0, &desc_GlobalNoticer}}},
    {"GlobalAlias", {{PARAM_STRING, 0, &s_GlobalNoticerAlias},
                     {PARAM_STRING, 0, &desc_GlobalNoticerAlias}}},
    {"HelpCoreModules", {{PARAM_STRING, PARAM_RELOAD, &HelpCoreModules}}},
    {"HelpChannel", {{PARAM_STRING, PARAM_RELOAD, &HelpChannel}}},
    {"HostCoreModules", {{PARAM_STRING, PARAM_RELOAD, &HostCoreModules}}},
    {"HostServAlias", {{PARAM_STRING, 0, &s_HostServAlias},
                       {PARAM_STRING, 0, &desc_HostServAlias}}},
    {"HostSetters", {{PARAM_STRING, PARAM_RELOAD, &HostSetter}}},
    {"IRCDModule", {{PARAM_STRING, 0, &IRCDModule}}},
    {"LogChannel", {{PARAM_STRING, PARAM_RELOAD, &LogChannel}}},
    {"LogBot", {{PARAM_SET, PARAM_RELOAD, &LogBot}}},
    {"HelpServName", {{PARAM_STRING, 0, &s_HelpServ},
                      {PARAM_STRING, 0, &desc_HelpServ}}},
    {"HelpServAlias", {{PARAM_STRING, 0, &s_HelpServAlias},
                       {PARAM_STRING, 0, &desc_HelpServAlias}}},
    {"KeepBackups", {{PARAM_INT, PARAM_RELOAD, &KeepBackups}}},
    {"KeepLogs", {{PARAM_INT, PARAM_RELOAD, &KeepLogs}}},
    {"KillonSGline", {{PARAM_SET, PARAM_RELOAD, &KillonSGline}}},
    {"KillonSQline", {{PARAM_SET, PARAM_RELOAD, &KillonSQline}}},
    {"AddAkiller", {{PARAM_SET, PARAM_RELOAD, &AddAkiller}}},
    {"LimitSessions", {{PARAM_SET, PARAM_FULLONLY, &LimitSessions}}},
    {"LocalAddress", {{PARAM_STRING, 0, &LocalHost},
                      {PARAM_PORT, PARAM_OPTIONAL, &LocalPort}}},
    {"LogUsers", {{PARAM_SET, PARAM_RELOAD, &LogUsers}}},
    {"SuperAdmin", {{PARAM_SET, PARAM_RELOAD, &SuperAdmin}}},
    {"LogMaxUsers", {{PARAM_SET, PARAM_RELOAD, &LogMaxUsers}}},
    {"MailDelay", {{PARAM_TIME, PARAM_RELOAD, &MailDelay}}},
    {"MaxSessionKill", {{PARAM_INT, PARAM_RELOAD, &MaxSessionKill}}},
    {"MaxSessionLimit", {{PARAM_POSINT, PARAM_RELOAD, &MaxSessionLimit}}},
    {"MemoCoreModules", {{PARAM_STRING, PARAM_RELOAD, &MemoCoreModules}}},
    {"MemoServName", {{PARAM_STRING, 0, &s_MemoServ},
                      {PARAM_STRING, 0, &desc_MemoServ}}},
    {"MemoServAlias", {{PARAM_STRING, 0, &s_MemoServAlias},
                       {PARAM_STRING, 0, &desc_MemoServAlias}}},
    {"MysqlHost", {{PARAM_STRING, PARAM_RELOAD, &MysqlHost}}},
    {"MysqlUser", {{PARAM_STRING, PARAM_RELOAD, &MysqlUser}}},
    {"MysqlPass", {{PARAM_STRING, PARAM_RELOAD, &MysqlPass}}},
    {"MysqlName", {{PARAM_STRING, PARAM_RELOAD, &MysqlName}}},
    {"MysqlPort", {{PARAM_PORT, PARAM_RELOAD, &MysqlPort}}},
    {"MysqlSecure", {{PARAM_STRING, PARAM_RELOAD, &MysqlSecure}}},
    {"MysqlSock", {{PARAM_STRING, PARAM_RELOAD, &MysqlSock}}},
    {"MysqlRetries", {{PARAM_POSINT, PARAM_RELOAD, &MysqlRetries}}},
    {"MysqlRetryGap", {{PARAM_POSINT, PARAM_RELOAD, &MysqlRetryGap}}},
    {"UseRDB", {{PARAM_SET, PARAM_RELOAD, &UseRDB}}},
    {"ModuleAutoload", {{PARAM_STRING, PARAM_RELOAD, &Modules}}},
    {"ModuleDelayedAutoload",
     {{PARAM_STRING, PARAM_RELOAD, &ModulesDelayed}}},
    {"MOTDFile", {{PARAM_STRING, PARAM_RELOAD, &MOTDFilename}}},
    {"MSMaxMemos", {{PARAM_POSINT, PARAM_RELOAD, &MSMaxMemos}}},
    {"MSNotifyAll", {{PARAM_SET, PARAM_RELOAD, &MSNotifyAll}}},
    {"MSSendDelay", {{PARAM_TIME, PARAM_RELOAD, &MSSendDelay}}},
    {"MSMemoReceipt", {{PARAM_POSINT, PARAM_RELOAD, &MSMemoReceipt}}},
    {"NetworkName", {{PARAM_STRING, PARAM_RELOAD, &NetworkName}}},
    {"NewsCount", {{PARAM_POSINT, PARAM_RELOAD, &NewsCount}}},
    {"NewsDB", {{PARAM_STRING, PARAM_RELOAD, &NewsDBName}}},
    {"NickLen", {{PARAM_POSINT, 0, &NickLen}}},
    {"NickservDB", {{PARAM_STRING, PARAM_RELOAD, &NickDBName}}},
    {"Numeric", {{PARAM_STRING, PARAM_RELOAD, &Numeric}}},
    {"PreNickServDB", {{PARAM_STRING, PARAM_RELOAD, &PreNickDBName}}},
    {"NSEmailReg", {{PARAM_SET, PARAM_RELOAD, &NSEmailReg}}},
    {"NickCoreModules", {{PARAM_STRING, PARAM_RELOAD, &NickCoreModules}}},
    {"NickRegDelay", {{PARAM_POSINT, PARAM_RELOAD, &NickRegDelay}}},
    {"NickServName", {{PARAM_STRING, 0, &s_NickServ},
                      {PARAM_STRING, 0, &desc_NickServ}}},
    {"NickServAlias", {{PARAM_STRING, 0, &s_NickServAlias},
                       {PARAM_STRING, 0, &desc_NickServAlias}}},
    {"NoBackupOkay", {{PARAM_SET, PARAM_RELOAD, &NoBackupOkay}}},
    {"NSAccessMax", {{PARAM_POSINT, PARAM_RELOAD, &NSAccessMax}}},
    {"NSAllowKillImmed", {{PARAM_SET, 0, &NSAllowKillImmed}}},
    {"NSDefHideEmail", {{PARAM_SET, PARAM_RELOAD, &NSDefHideEmail}}},
    {"NSDefHideQuit", {{PARAM_SET, PARAM_RELOAD, &NSDefHideQuit}}},
    {"NSDefHideUsermask", {{PARAM_SET, PARAM_RELOAD, &NSDefHideUsermask}}},
    {"NSDefKill", {{PARAM_SET, PARAM_RELOAD, &NSDefKill}}},
    {"NSDefKillQuick", {{PARAM_SET, PARAM_RELOAD, &NSDefKillQuick}}},
    {"NSDefLanguage", {{PARAM_POSINT, PARAM_RELOAD, &NSDefLanguage}}},
    {"NSDefMemoReceive", {{PARAM_SET, PARAM_RELOAD, &NSDefMemoReceive}}},
    {"NSDefMemoSignon", {{PARAM_SET, PARAM_RELOAD, &NSDefMemoSignon}}},
    {"NSDefMsg", {{PARAM_SET, PARAM_RELOAD, &NSDefMsg}}},
    {"NSDefNone", {{PARAM_SET, PARAM_RELOAD, &NSDefNone}}},
    {"NSDefPrivate", {{PARAM_SET, PARAM_RELOAD, &NSDefPrivate}}},
    {"NSDefSecure", {{PARAM_SET, PARAM_RELOAD, &NSDefSecure}}},
    {"NSDefAutoop", {{PARAM_SET, PARAM_RELOAD, &NSDefAutoop}}},
    {"NSEnforcerUser", {{PARAM_STRING, PARAM_RELOAD, &temp_nsuserhost}}},
    {"NSExpire", {{PARAM_TIME, PARAM_RELOAD, &NSExpire}}},
    {"NSRExpire", {{PARAM_TIME, PARAM_RELOAD, &NSRExpire}}},
    {"NSModeOnID", {{PARAM_SET, PARAM_RELOAD, &NSModeOnID}}},
    {"NSForceEmail", {{PARAM_SET, PARAM_RELOAD, &NSForceEmail}}},
    {"NSGuestNickPrefix",
     {{PARAM_STRING, PARAM_RELOAD, &NSGuestNickPrefix}}},
    {"NSListMax", {{PARAM_POSINT, PARAM_RELOAD, &NSListMax}}},
    {"NSListOpersOnly", {{PARAM_SET, PARAM_RELOAD, &NSListOpersOnly}}},
    {"NSMaxAliases", {{PARAM_INT, PARAM_RELOAD, &NSMaxAliases}}},
    {"NSNoGroupChange", {{PARAM_SET, PARAM_RELOAD, &NSNoGroupChange}}},
    {"NSRegDelay", {{PARAM_TIME, PARAM_RELOAD, &NSRegDelay}}},
    {"NSResendDelay", {{PARAM_TIME, PARAM_RELOAD, &NSResendDelay}}},
    {"NSReleaseTimeout", {{PARAM_TIME, PARAM_RELOAD, &NSReleaseTimeout}}},
    {"NSSecureAdmins", {{PARAM_SET, PARAM_RELOAD, &NSSecureAdmins}}},
    {"NSStrictPrivileges",
     {{PARAM_SET, PARAM_RELOAD, &NSStrictPrivileges}}},
    {"NSRestrictGetPass", {{PARAM_SET, PARAM_RELOAD, &NSRestrictGetPass}}},
    {"NSNickTracking", {{PARAM_SET, PARAM_RELOAD, &NSNickTracking}}},
    {"NSAddAccessOnReg", {{PARAM_SET, PARAM_RELOAD, &NSAddAccessOnReg}}},
    {"OperCoreModules", {{PARAM_STRING, PARAM_RELOAD, &OperCoreModules}}},
    {"OperServDB", {{PARAM_STRING, PARAM_RELOAD, &OperDBName}}},
    {"OperServName", {{PARAM_STRING, 0, &s_OperServ},
                      {PARAM_STRING, 0, &desc_OperServ}}},
    {"OperServAlias", {{PARAM_STRING, 0, &s_OperServAlias},
                       {PARAM_STRING, 0, &desc_OperServAlias}}},
    {"PIDFile", {{PARAM_STRING, 0, &PIDFilename}}},
    {"ReadTimeout", {{PARAM_TIME, PARAM_RELOAD, &ReadTimeout}}},
    {"RemoteServer", {{PARAM_STRING, 0, &RemoteServer},
                      {PARAM_PORT, 0, &RemotePort},
                      {PARAM_STRING, 0, &RemotePassword}}},
    {"RemoteServer2", {{PARAM_STRING, 0, &RemoteServer2},
                       {PARAM_PORT, 0, &RemotePort2},
                       {PARAM_STRING, 0, &RemotePassword2}}},
    {"RemoteServer3", {{PARAM_STRING, 0, &RemoteServer3},
                       {PARAM_PORT, 0, &RemotePort3},
                       {PARAM_STRING, 0, &RemotePassword3}}},
    {"RestrictMail", {{PARAM_SET, PARAM_RELOAD, &RestrictMail}}},
    {"RestrictOperNicks", {{PARAM_SET, PARAM_RELOAD, &RestrictOperNicks}}},
    {"SendMailPath", {{PARAM_STRING, PARAM_RELOAD, &SendMailPath}}},
    {"SendFrom", {{PARAM_STRING, PARAM_RELOAD, &SendFrom}}},
    {"ServerDesc", {{PARAM_STRING, 0, &ServerDesc}}},
    {"ServerName", {{PARAM_STRING, 0, &ServerName}}},
    {"ServicesRoot", {{PARAM_STRING, PARAM_RELOAD, &ServicesRoot}}},
    {"ServiceUser", {{PARAM_STRING, 0, &temp_userhost}}},
    {"SessionLimitDetailsLoc",
     {{PARAM_STRING, PARAM_RELOAD, &SessionLimitDetailsLoc}}},
    {"OSOpersOnly", {{PARAM_SET, PARAM_RELOAD, &OSOpersOnly}}},
    {"SessionLimitExceeded",
     {{PARAM_STRING, PARAM_RELOAD, &SessionLimitExceeded}}},
    {"SessionAutoKillExpiry",
     {{PARAM_TIME, PARAM_RELOAD, &SessionAutoKillExpiry}}},
    {"SGLineExpiry", {{PARAM_TIME, PARAM_RELOAD, &SGLineExpiry}}},
    {"SQLineExpiry", {{PARAM_TIME, PARAM_RELOAD, &SQLineExpiry}}},
    {"SZLineExpiry", {{PARAM_TIME, PARAM_RELOAD, &SZLineExpiry}}},
    {"HideStatsO", {{PARAM_SET, PARAM_RELOAD, &HideStatsO}}},
    {"GlobalOnCycle", {{PARAM_SET, PARAM_RELOAD, &GlobalOnCycle}}},
    {"AnonymousGlobal", {{PARAM_SET, PARAM_RELOAD, &AnonymousGlobal}}},
    {"GlobalOnCycleMessage",
     {{PARAM_STRING, PARAM_RELOAD, &GlobalOnCycleMessage}}},
    {"GlobalOnCycleUP", {{PARAM_STRING, PARAM_RELOAD, &GlobalOnCycleUP}}},
    {"StrictPasswords", {{PARAM_SET, PARAM_RELOAD, &StrictPasswords}}},
    {"TimeoutCheck", {{PARAM_TIME, PARAM_RELOAD, &TimeoutCheck}}},
    {"UpdateTimeout", {{PARAM_TIME, PARAM_RELOAD, &UpdateTimeout}}},
    {"UseMail", {{PARAM_SET, PARAM_RELOAD, &UseMail}}},
    {"UsePrivmsg", {{PARAM_SET, PARAM_RELOAD, &UsePrivmsg}}},
    {"UseStrictPrivMsg", {{PARAM_SET, PARAM_RELOAD, &UseStrictPrivMsg}}},
    {"UserKey1", {{PARAM_POSINT, PARAM_RELOAD, &UserKey1}}},
    {"UserKey2", {{PARAM_POSINT, PARAM_RELOAD, &UserKey2}}},
    {"UserKey3", {{PARAM_POSINT, PARAM_RELOAD, &UserKey3}}},
    {"UseSVSHOLD", {{PARAM_SET, PARAM_RELOAD, &UseSVSHOLD}}},
    {"UseSVS2MODE", {{PARAM_SET, PARAM_RELOAD, &UseSVS2MODE}}},
    {"UseTokens", {{PARAM_SET, 0, &UseTokens}}},
    {"UseTS6", {{PARAM_SET, 0, &UseTS6}}},
    {"UnRestrictSAdmin", {{PARAM_SET, PARAM_RELOAD, &UnRestrictSAdmin}}},
    {"WallAkillExpire", {{PARAM_SET, PARAM_RELOAD, &WallAkillExpire}}},
    {"WallBadOS", {{PARAM_SET, PARAM_RELOAD, &WallBadOS}}},
    {"WallDrop", {{PARAM_SET, PARAM_RELOAD, &WallDrop}}},
    {"WallExceptionExpire",
     {{PARAM_SET, PARAM_RELOAD, &WallExceptionExpire}}},
    {"WallForbid", {{PARAM_SET, PARAM_RELOAD, &WallForbid}}},
    {"WallGetpass", {{PARAM_SET, PARAM_RELOAD, &WallGetpass}}},
    {"WallOper", {{PARAM_SET, PARAM_RELOAD, &WallOper}}},
    {"WallOSAkill", {{PARAM_SET, PARAM_RELOAD, &WallOSAkill}}},
    {"WallOSClearmodes", {{PARAM_SET, PARAM_RELOAD, &WallOSClearmodes}}},
    {"WallOSGlobal", {{PARAM_SET, PARAM_RELOAD, &WallOSGlobal}}},
    {"WallOSKick", {{PARAM_SET, PARAM_RELOAD, &WallOSKick}}},
    {"WallOSJupe", {{PARAM_SET, PARAM_RELOAD, &WallOSJupe}}},
    {"WallOSMode", {{PARAM_SET, PARAM_RELOAD, &WallOSMode}}},
    {"WallOSNoOp", {{PARAM_SET, PARAM_RELOAD, &WallOSNoOp}}},
    {"WallOSRaw", {{PARAM_SET, PARAM_RELOAD, &WallOSRaw}}},
    {"WallOSSGLine", {{PARAM_SET, PARAM_RELOAD, &WallOSSGLine}}},
    {"WallOSSQLine", {{PARAM_SET, PARAM_RELOAD, &WallOSSQLine}}},
    {"WallOSSZLine", {{PARAM_SET, PARAM_RELOAD, &WallOSSZLine}}},
    {"WallSetpass", {{PARAM_SET, PARAM_RELOAD, &WallSetpass}}},
    {"WallSGLineExpire", {{PARAM_SET, PARAM_RELOAD, &WallSGLineExpire}}},
    {"WallSQLineExpire", {{PARAM_SET, PARAM_RELOAD, &WallSQLineExpire}}},
    {"WallSZLineExpire", {{PARAM_SET, PARAM_RELOAD, &WallSZLineExpire}}},
    {"WarningTimeout", {{PARAM_TIME, PARAM_RELOAD, &WarningTimeout}}},
    {"GlobalOnDefcon", {{PARAM_SET, PARAM_RELOAD, &GlobalOnDefcon}}},
    {"GlobalOnDefconMore",
     {{PARAM_SET, PARAM_RELOAD, &GlobalOnDefconMore}}},
    {"DefconMessage", {{PARAM_STRING, PARAM_RELOAD, &DefconMessage}}},
    {"UlineServers", {{PARAM_STRING, PARAM_RELOAD, &UlineServers}}},
};

/*************************************************************************/

/* Print an error message to the log (and the console, if open). */

void error(int linenum, const char *message, ...)
{
    char buf[4096];
    va_list args;

    va_start(args, message);
    vsnprintf(buf, sizeof(buf), message, args);
    va_end(args);

#ifndef NOT_MAIN
    if (linenum)
        alog("%s:%d: %s", SERVICES_CONF, linenum, buf);
    else
        alog("%s: %s", SERVICES_CONF, buf);
    if (!nofork && isatty(2)) {
#endif
        if (linenum)
            fprintf(stderr, "%s:%d: %s\n", SERVICES_CONF, linenum, buf);
        else
            fprintf(stderr, "%s: %s\n", SERVICES_CONF, buf);
#ifndef NOT_MAIN
    }
#endif
}

/*************************************************************************/

/* Parse a configuration line.  Return 1 on success; otherwise, print an
 * appropriate error message and return 0.  Destroys the buffer by side
 * effect.
 */

int parse_directive(Directive * d, char *dir, int ac, char *av[MAXPARAMS],
                    int linenum, int reload, char *s)
{
    int retval = 1;
    int i;
    long val;
    int optind;

    if (stricmp(dir, d->name) != 0)
        return 1;
    optind = 0;
    for (i = 0; i < MAXPARAMS && d->params[i].type != PARAM_NONE; i++) {
        if (reload && !(d->params[i].flags & PARAM_RELOAD))
            continue;

        if (d->params[i].type == PARAM_SET) {
            *(int *) d->params[i].ptr = 1;
            continue;
        }
#ifdef STREAMLINED
        if (d->params[i].flags & PARAM_FULLONLY) {
            error(linenum,
                  "Directive `%s' not available in STREAMLINED mode",
                  d->name);
            break;
        }
#endif

        /* Should we remove PARAM_DEPRECATED because it's
         * useless right now? -GD */
        if (d->params[i].type == PARAM_DEPRECATED) {
            void (*func) (void);
            error(linenum, "Deprecated directive `%s' used", d->name);
            func = (void (*)(void)) (d->params[i].ptr);
            func();             /* For clarity */
            continue;
        }
        if (optind >= ac) {
            if (!(d->params[i].flags & PARAM_OPTIONAL)) {
                error(linenum, "Not enough parameters for `%s'", d->name);
                retval = 0;
            }
            break;
        }
        switch (d->params[i].type) {
        case PARAM_INT:
            val = strtol(av[optind++], &s, 0);
            if (*s) {
                error(linenum,
                      "%s: Expected an integer for parameter %d",
                      d->name, optind);
                retval = 0;
                break;
            }
            *(int *) d->params[i].ptr = val;
            break;
        case PARAM_POSINT:
            val = strtol(av[optind++], &s, 0);
            if (*s || val <= 0) {
                error(linenum,
                      "%s: Expected a positive integer for parameter %d",
                      d->name, optind);
                retval = 0;
                break;
            }
            if (errno == ERANGE && val == LONG_MAX) {
                /* well the true top off is 2,147,483,647 but lets not give them the real top */
                error(linenum,
                      "%s: paramter %d is to large, reduce this value (0 to 2,147,483,646)",
                      d->name, optind);
            }
            *(int *) d->params[i].ptr = val;
            break;
        case PARAM_PORT:
            val = strtol(av[optind++], &s, 0);
            if (*s) {
                error(linenum,
                      "%s: Expected a port number for parameter %d",
                      d->name, optind);
                retval = 0;
                break;
            }
            if (val < 1 || val > 65535) {
                error(linenum,
                      "Port numbers must be in the range 1..65535");
                retval = 0;
                break;
            }
            *(int *) d->params[i].ptr = val;
            break;
        case PARAM_STRING:
/*	      if (reload && *(char **)d->params[i].ptr)
	      	free(*(char **)d->params[i].ptr); */
            *(char **) d->params[i].ptr = sstrdup(av[optind++]);
            if (!d->params[i].ptr) {
                error(linenum, "%s: Out of memory", d->name);
                return 0;
            }
            break;
        case PARAM_TIME:
            val = dotime(av[optind++]);
            if (val < 0) {
                error(linenum,
                      "%s: Expected a time value for parameter %d",
                      d->name, optind);
                retval = 0;
                break;
            }
            *(int *) d->params[i].ptr = val;
            break;
        default:
            error(linenum, "%s: Unknown type %d for param %d",
                  d->name, d->params[i].type, i + 1);
            retval = 0;         /* don't bother continuing--something's bizarre */
            break;
        }
    }
    return retval;;
}


int parse(char *buf, int linenum, int reload)
{
    char *s, *t, *dir;
    int n;
    int retval = 1;
    int ac = 0;
    char *av[MAXPARAMS];

    dir = strtok(buf, " \t\r\n");
    s = strtok(NULL, "");
    if (s) {
        while (isspace(*s))
            s++;
        while (*s) {
            if (ac >= MAXPARAMS) {
                error(linenum, "Warning: too many parameters (%d max)",
                      MAXPARAMS);
                break;
            }
            t = s;
            if (*s == '"') {
                t++;
                s++;
                while (*s && *s != '"') {
                    if (*s == '\\' && s[1] != 0)
                        s++;
                    s++;
                }
                if (!*s)
                    error(linenum,
                          "Warning: unterminated double-quoted string");
                else
                    *s++ = 0;
            } else {
                s += strcspn(s, " \t\r\n");
                if (*s)
                    *s++ = 0;
            }
            av[ac++] = t;
            while (isspace(*s))
                s++;
        }
    }

    if (!dir)
        return 1;

    for (n = 0; n < lenof(directives); n++) {
        Directive *d = &directives[n];
        retval = parse_directive(d, dir, ac, av, linenum, reload, s);
        if (!retval) {
            break;
        }
    }

    return retval;
}


/*************************************************************************/

#define CHECK(v) do {			\
    if (!v) {				\
	error(0, #v " missing");	\
	retval = 0;			\
    }					\
} while (0)

#define CHEK2(v,n) do {			\
    if (!v) {				\
	error(0, #n " missing");	\
	retval = 0;			\
    }					\
} while (0)

/* Read the entire configuration file.  If an error occurs while reading
 * the file or a required directive is not found, print and log an
 * appropriate error message and return 0; otherwise, return 1.
 *
 * If reload is 1, will reload the configuration file.
 *		--lara
 *
 */

int read_config(int reload)
{
    FILE *config;
    int linenum = 0, retval = 1;
    char buf[1024], *s;
    int defconCount = 0;

    if (reload) {
        int i, n;

        /* Reset all the reloadable settings */

        for (n = 0; n < lenof(directives); n++) {
            Directive *d = &directives[n];

            for (i = 0; i < MAXPARAMS && d->params[i].type != PARAM_NONE;
                 i++) {
                if (!(d->params[i].flags & PARAM_RELOAD))
                    continue;

                if (d->params[i].type == PARAM_SET
                    || d->params[i].type == PARAM_INT
                    || d->params[i].type == PARAM_POSINT
                    || d->params[i].type == PARAM_TIME) {
                    *(int *) d->params[i].ptr = 0;
                } else if (d->params[i].type == PARAM_STRING) {
                    if (*(char **) d->params[i].ptr)
                        free(*(char **) d->params[i].ptr);
                    (*(char **) d->params[i].ptr) = NULL;
                }
            }
        }
    }

    config = fopen(SERVICES_CONF, "r");
    if (!config) {
#ifndef NOT_MAIN
        log_perror("Can't open " SERVICES_CONF);
        if (!nofork && isatty(2)) {
#endif
            if (!reload)
                perror("Can't open " SERVICES_CONF);
            else
                alog("Can't open %s", SERVICES_CONF);
#ifndef NOT_MAIN
        }
#endif
        return 0;
    }
    while (fgets(buf, sizeof(buf), config)) {
        linenum++;
        if (*buf == '#' || *buf == '\r' || *buf == '\n')
            continue;
        if (!parse(buf, linenum, reload))
            retval = 0;
    }
    fclose(config);

    if (!reload) {
        CHECK(RemoteServer);
        CHECK(ServerName);
        CHECK(ServerDesc);

        if (RemoteServer3)
            CHECK(RemoteServer2);

        if (LocalHost && RemoteServer) {
            if ((!stricmp(LocalHost, RemoteServer))
                && LocalPort == RemotePort) {
                printf
                    ("\n*** LocalAddress and RemoteServer are set to use the same IP address\n"
                     "*** (%s) and port (%d). This would have resulted in errors.\n"
                     "*** Change the LocalAddress to bind to another port.\n",
                     RemoteServer, LocalPort);
                retval = 0;
            }
        }

        if (NickLen == 0) {
            alog("You have not defined the NickLen configuration directive. It is strongly");
            alog("advised that you do configure this correctly in your services.conf");
            NickLen = NICKMAX - 1;
        } else if ((NickLen < 1) || (NickLen >= NICKMAX)) {
            alog("NickLen has an invalid value; setting to %d",
                 (NICKMAX - 1));
            NickLen = NICKMAX - 1;
        }
    }

    CHECK(IRCDModule);
    CHECK(EncModule);

    CHECK(NetworkName);
    if (!reload) {
        CHEK2(temp_userhost, ServiceUser);
        CHEK2(s_NickServ, NickServName);
        CHEK2(s_ChanServ, ChanServName);
        CHEK2(s_MemoServ, MemoServName);
        CHEK2(s_HelpServ, HelpServName);
        CHEK2(s_OperServ, OperServName);
        CHEK2(s_GlobalNoticer, GlobalName);
        CHEK2(PIDFilename, PIDFile);
    }

    if (s_ChanServAlias) {
        if (!stricmp(s_ChanServ, s_ChanServAlias)) {
            printf
                ("\n*** ChanServ and ChanServ Alias are the same, this will cause errors\n");
            retval = 0;
        }
    }

    if (s_NickServAlias) {
        if (!stricmp(s_NickServ, s_NickServAlias)) {
            printf
                ("\n*** NickServ and NickServ Alias are the same, this will cause errors\n");
            retval = 0;
        }
    }

    if (s_OperServAlias) {
        if (!stricmp(s_OperServ, s_OperServAlias)) {
            printf
                ("\n*** OperServ and OperServ Alias are the same, this will cause errors\n");
            retval = 0;
        }
    }

    if (s_MemoServAlias) {
        if (!stricmp(s_MemoServ, s_MemoServAlias)) {
            printf
                ("\n*** MemoServ and MemoServ Alias are the same, this will cause errors\n");
            retval = 0;
        }
    }

    if (s_HelpServAlias) {
        if (!stricmp(s_HelpServ, s_HelpServAlias)) {
            printf
                ("\n*** HelpServ and HelpServ Alias are the same, this will cause errors\n");
            retval = 0;
        }
    }

    if (s_GlobalNoticerAlias) {
        if (!stricmp(s_GlobalNoticer, s_GlobalNoticerAlias)) {
            printf
                ("\n*** GlobalNoticer and GlobalNoticer Alias are the same, this will cause errors\n");
            retval = 0;
        }
    }


    CHEK2(MOTDFilename, MOTDFile);
    if (!reload) {
        CHEK2(NickDBName, NickServDB);
        CHEK2(ChanDBName, ChanServDB);
        CHEK2(OperDBName, OperServDB);
        CHEK2(NewsDBName, NewsDB);
        CHEK2(ExceptionDBName, ExceptionDB);
    }
    CHECK(UpdateTimeout);
    CHECK(ExpireTimeout);
    CHECK(ReadTimeout);
    CHECK(WarningTimeout);
    CHECK(TimeoutCheck);
    CHECK(NSAccessMax);
    CHEK2(temp_nsuserhost, NSEnforcerUser);
    CHECK(NSReleaseTimeout);
    CHECK(NSListMax);
    CHECK(CSAccessMax);
    CHECK(CSAutokickMax);
    CHECK(CSAutokickReason);
    CHECK(CSInhabit);
    CHECK(CSListMax);
    CHECK(ServicesRoot);
    CHECK(AutokillExpiry);
    CHECK(ChankillExpiry);
    CHECK(SGLineExpiry);
    CHECK(SQLineExpiry);
    CHECK(SZLineExpiry);

    if (!reload) {

        if (temp_userhost) {
            if (!(s = strchr(temp_userhost, '@'))) {
                error(0, "Missing `@' for ServiceUser");
            } else {
                *s++ = 0;
                ServiceUser = temp_userhost;
                ServiceHost = s;
            }
        }

    }

    if (temp_nsuserhost) {
        if (!(s = strchr(temp_nsuserhost, '@'))) {
            NSEnforcerUser = temp_nsuserhost;
            NSEnforcerHost = ServiceHost;
        } else {
            *s++ = 0;
            NSEnforcerUser = temp_nsuserhost;
            NSEnforcerHost = s;
        }
    }

    if (!NSDefNone &&
        !NSDefKill &&
        !NSDefKillQuick &&
        !NSDefSecure &&
        !NSDefPrivate &&
        !NSDefHideEmail &&
        !NSDefHideUsermask &&
        !NSDefHideQuit && !NSDefMemoSignon && !NSDefMemoReceive) {
        NSDefSecure = 1;
        NSDefMemoSignon = 1;
        NSDefMemoReceive = 1;
    }

    NSDefFlags = 0;
    if (!NSDefNone) {
        if (NSDefKill)
            NSDefFlags |= NI_KILLPROTECT;
        if (NSDefKillQuick)
            NSDefFlags |= NI_KILL_QUICK;
        if (NSDefSecure)
            NSDefFlags |= NI_SECURE;
        if (NSDefPrivate)
            NSDefFlags |= NI_PRIVATE;
        if (NSDefMsg) {
            if (!UsePrivmsg)
                alog("NSDefMsg can only be used when UsePrivmsg is set - unsetting NSDefMsg");
            else
                NSDefFlags |= NI_MSG;
        }
        if (NSDefHideEmail)
            NSDefFlags |= NI_HIDE_EMAIL;
        if (NSDefHideUsermask)
            NSDefFlags |= NI_HIDE_MASK;
        if (NSDefHideQuit)
            NSDefFlags |= NI_HIDE_QUIT;
        if (NSDefMemoSignon)
            NSDefFlags |= NI_MEMO_SIGNON;
        if (NSDefMemoReceive)
            NSDefFlags |= NI_MEMO_RECEIVE;
        if (!NSDefAutoop)
            NSDefFlags |= NI_AUTOOP;
    }

    if (!ServicesRoot) {
        error(0,
              "You must define the 'ServicesRoot' configuration directive");
        error(0,
              "in your services.conf file. This is a required setting that");
        error(0,
              "defines the main Administrative nick(s) Anope will obey.");
        retval = 0;
    }

    CHECK(NSGuestNickPrefix);   /* Add safety check */
    if (NSGuestNickPrefix && (strlen(NSGuestNickPrefix) > 21)) {
        error(0, "Value of NSGuestNickPrefix must be between 1 and 21");
        retval = 0;
    }

    CHECK(NSDefLanguage);
    if (NSDefLanguage) {
        NSDefLanguage--;
        if (NSDefLanguage < 0 || NSDefLanguage >= NUM_LANGS) {
            error(0, "Value of NSDefLanguage must be between 1 and %d",
                  USED_LANGS);
            retval = 0;
        }
    }

    if (!NewsCount) {
        NewsCount = 3;
    }

    if (reload) {
        if ((NSDefLanguage = langlist[NSDefLanguage]) < 0)
            NSDefLanguage = DEF_LANGUAGE;
    }

    if (CSDefBantype < 0 || CSDefBantype > 3) {
        error(0, "Value of CSDefBantype must be between 0 and 3 included");
        retval = 0;
    }

    if (!MysqlRetries || !MysqlRetryGap) {
        MysqlRetries = 5;
        MysqlRetryGap = 1;
    } else if (((MysqlRetries * MysqlRetryGap) > 60)
               || ((MysqlRetries * MysqlRetryGap) < 1)) {
        error(0,
              "MysqlRetries * MysqlRetryGap must be between 1 and 60, using standard values.");
        MysqlRetries = 5;
        MysqlRetryGap = 1;
    }

    if (!CSDefNone &&
        !CSDefKeepTopic &&
        !CSDefTopicLock &&
        !CSDefPrivate &&
        !CSDefRestricted &&
        !CSDefSecure &&
        !CSDefSecureOps &&
        !CSDefSecureFounder &&
        !CSDefSignKick && !CSDefSignKickLevel && !CSDefOpNotice) {
        CSDefKeepTopic = 1;
        CSDefSecure = 1;
        CSDefSecureFounder = 1;
        CSDefSignKick = 1;
    }

    CSDefFlags = 0;
    if (!CSDefNone) {
        if (CSDefKeepTopic)
            CSDefFlags |= CI_KEEPTOPIC;
        if (CSDefTopicLock)
            CSDefFlags |= CI_TOPICLOCK;
        if (CSDefPrivate)
            CSDefFlags |= CI_PRIVATE;
        if (CSDefRestricted)
            CSDefFlags |= CI_RESTRICTED;
        if (CSDefSecure)
            CSDefFlags |= CI_SECURE;
        if (CSDefSecureOps)
            CSDefFlags |= CI_SECUREOPS;
        if (CSDefSecureFounder)
            CSDefFlags |= CI_SECUREFOUNDER;
        if (CSDefSignKick)
            CSDefFlags |= CI_SIGNKICK;
        if (CSDefSignKickLevel)
            CSDefFlags |= CI_SIGNKICK_LEVEL;
        if (CSDefOpNotice)
            CSDefFlags |= CI_OPNOTICE;
        if (CSDefXOP)
            CSDefFlags |= CI_XOP;
        if (CSDefPeace)
            CSDefFlags |= CI_PEACE;
    }

    BSDefFlags = 0;
    if (BSDefDontKickOps)
        BSDefFlags |= BS_DONTKICKOPS;
    if (BSDefDontKickVoices)
        BSDefFlags |= BS_DONTKICKVOICES;
    if (BSDefGreet)
        BSDefFlags |= BS_GREET;
    if (BSDefFantasy)
        BSDefFlags |= BS_FANTASY;
    if (BSDefSymbiosis)
        BSDefFlags |= BS_SYMBIOSIS;

    /* Services Root building */

    if (ServicesRoot && !reload) {      /* Check to prevent segmentation fault if it's missing */
        RootNumber = 0;

        s = strtok(ServicesRoot, " ");
        do {
            if (s) {
                RootNumber++;
                ServicesRoots =
                    srealloc(ServicesRoots, sizeof(char *) * RootNumber);
                ServicesRoots[RootNumber - 1] = sstrdup(s);
            }
        } while ((s = strtok(NULL, " ")));
    }

    if (!RootNumber) {
        error(0, "No ServicesRoot defined");
        retval = 0;
    }

    /* Ulines */

    if (UlineServers) {
        NumUlines = 0;

        s = strtok(UlineServers, " ");
        do {
            if (s) {
                NumUlines++;
                Ulines = srealloc(Ulines, sizeof(char *) * NumUlines);
                Ulines[NumUlines - 1] = sstrdup(s);
            }
        } while ((s = strtok(NULL, " ")));
    }

    /* Host Setters building... :P */
    HostSetters = buildStringList(HostSetter, &HostNumber);

    /* Modules Autoload building... :P */
    ModulesAutoload = buildStringList(Modules, &ModulesNumber);
    ModulesDelayedAutoload =
        buildStringList(ModulesDelayed, &ModulesDelayedNumber);
    HostServCoreModules =
        buildStringList(HostCoreModules, &HostServCoreNumber);
    MemoServCoreModules =
        buildStringList(MemoCoreModules, &MemoServCoreNumber);
    HelpServCoreModules =
        buildStringList(HelpCoreModules, &HelpServCoreNumber);

    BotServCoreModules =
        buildStringList(BotCoreModules, &BotServCoreNumber);

    OperServCoreModules =
        buildStringList(OperCoreModules, &OperServCoreNumber);

    ChanServCoreModules =
        buildStringList(ChanCoreModules, &ChanServCoreNumber);

    NickServCoreModules =
        buildStringList(NickCoreModules, &NickServCoreNumber);


    if (LimitSessions) {
        CHECK(DefSessionLimit);
        CHECK(MaxSessionLimit);
        CHECK(ExceptionExpiry);

        if (MaxSessionKill && !SessionAutoKillExpiry)
            SessionAutoKillExpiry = 30 * 60;    /* 30 minutes */
    }

    if (s_BotServ) {
        CHEK2(BotDBName, BotServDB);
        CHECK(BSBadWordsMax);
        CHECK(BSMinUsers);
        CHECK(BSKeepData);
        if (s_BotServAlias) {
            if (!stricmp(s_BotServ, s_BotServAlias)) {
                printf
                    ("\n*** BotServ and BotServ Alias are the same, this will cause errors\n");
                retval = 0;
            }
        }
        if (!BSFantasyCharacter)
            BSFantasyCharacter = sstrdup("!");
        if (BSFantasyCharacter && (strlen(BSFantasyCharacter) > 1)) {
            printf
                ("*** BSFantasyCharacter is more than 1 character long. Only the first\n"
                 "*** character ('%c') will be used. The others will be ignored.\n",
                 *BSFantasyCharacter);
        }
    }

    if (s_HostServ) {
        CHEK2(s_HostServ, HostServName);
        CHEK2(HostDBName, HostServDB);

        if (s_HostServAlias) {
            if (!stricmp(s_HostServ, s_HostServAlias)) {
                printf
                    ("\n*** HostServ and HostServ Alias are the same, this will cause errors\n");
                retval = 0;
            }
        }
    }

    if (UseMail) {
        CHECK(SendMailPath);
        CHECK(SendFrom);
    }

    if (GlobalOnCycle) {
        if (!GlobalOnCycleMessage && !GlobalOnCycleUP) {
            alog("GlobalOnCycleMessage and GlobalOnCycleUP are not defined; disabling GlobalOnCycle");
            GlobalOnCycle = 0;
        }
    }

    /* Check the user keys */
    if ((UserKey1 == UserKey2) || (UserKey1 == UserKey3)
        || (UserKey3 == UserKey2))
        alog("Every UserKey must be different. It's for YOUR safety! Remember that!");

    /**
     * Check all DEFCON dependiencies...
     **/
    if (DefConLevel) {
        CHECK(DefCon1);
        CHECK(DefCon2);
        CHECK(DefCon3);
        CHECK(DefCon4);
        DefCon5 = 0;            /* ALWAYS have defcon 5 as normal operation */
        /* Build DefCon's */
        DefCon[0] = 0;
        DefCon[1] = DefCon1;
        DefCon[2] = DefCon2;
        DefCon[3] = DefCon3;
        DefCon[4] = DefCon4;
        DefCon[5] = DefCon5;
        for (defconCount = 1; defconCount <= 5; defconCount++) {        /* Check any defcon needed settings */
            if (DefCon[defconCount] & DEFCON_REDUCE_SESSION) {
                CHECK(DefConSessionLimit);
            }
            if (DefCon[defconCount] & DEFCON_AKILL_NEW_CLIENTS) {
                CHECK(DefConAKILL);
                CHECK(DefConAkillReason);
            }
            if (DefCon[defconCount] & DEFCON_FORCE_CHAN_MODES) {
                CHECK(DefConChanModes);
            }
        }
        if (GlobalOnDefconMore)
            CHECK(DefconMessage);
    }

    /**
     * If they try to enable any email registration option,
     * make sure they have everything else they need too...
     *
     * rob
     **/
    if (NSEmailReg) {
        CHEK2(PreNickDBName, PreNickServDB);
        CHECK(NSEmailReg);
        CHECK(NSRExpire);
        CHECK(UseMail);
        CHECK(NSForceEmail);
    } else {
        PreNickDBName = NULL;
        NSRExpire = 0;
    }

    if (!retval) {
        printf
            ("\n*** Support resources: Read through the services.conf self-contained \n*** documentation. Read the documentation files found in the 'docs' \n*** folder. Visit our portal located at http://www.anope.org/. Join \n*** our support channel on /server irc.anope.org channel #anope.\n\n");
    }

    return retval;
}


/*************************************************************************/

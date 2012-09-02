/* Prototypes and external variable declarations.
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#ifndef EXTERN_H
#define EXTERN_H

#include "modes.h"

#define E extern CoreExport
#define EI extern DllExport


/**** actions.c ****/

E bool bad_password(User *u);
E void common_unban(const ChannelInfo *ci, User *u, bool full = false);

/**** encrypt.c ****/

E void enc_encrypt(const Anope::string &src, Anope::string &dest);
E bool enc_decrypt(const Anope::string &src, Anope::string &dest);

/**** init.c ****/

E Anope::string conf_dir, db_dir, modules_dir, locale_dir, log_dir;

E void introduce_user(const Anope::string &user);
E bool GetCommandLineArgument(const Anope::string &name, char shortname = 0);
E bool GetCommandLineArgument(const Anope::string &name, char shortname, Anope::string &param);
E bool AtTerm();
E void Fork();
E void Init(int ac, char **av);

/**** language.cpp ****/

E std::vector<Anope::string> languages;
E std::vector<Anope::string> domains;
E void InitLanguages();
E const char *translate(const char *string);
E const char *translate(User *u, const char *string);
E const char *translate(const NickCore *nc, const char *string);
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
E int return_code;
E bool restarting;
E Anope::string quitmsg;
E time_t start_time;

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

E time_t dotime(const Anope::string &s);
E Anope::string duration(const time_t &seconds, const NickCore *nc = NULL);
E Anope::string expire_left(const NickCore *nc, time_t expires);
E Anope::string do_strftime(const time_t &t, const NickCore *nc = NULL, bool short_output = false);
E bool IsValidIdent(const Anope::string &ident);
E bool IsValidHost(const Anope::string &host);

E Anope::string myStrGetToken(const Anope::string &str, char dilim, int token_number);
E Anope::string myStrGetTokenRemainder(const Anope::string &str, char dilim, int token_number);
E int myNumToken(const Anope::string &str, char dilim);
E bool nickIsServices(const Anope::string &nick, bool bot);

E std::list<Anope::string> BuildStringList(const Anope::string &, char = ' ');
E std::vector<Anope::string> BuildStringVector(const Anope::string &, char = ' ');

E bool str_is_wildcard(const Anope::string &str);
E bool str_is_pure_wildcard(const Anope::string &str);
E Anope::string normalizeBuffer(const Anope::string &);

/**** modes.cpp ****/

/* Number of generic modes we support */
E unsigned GenericChannelModes, GenericUserModes;
E std::multimap<ChannelModeName, ModeLock *> def_mode_locks;
E void SetDefaultMLock(ServerConfig *config);

/**** process.c ****/

E void process(const Anope::string &buf);

#endif /* EXTERN_H */

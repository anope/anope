/* depricated.h
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

/*
   All of these functions were replaced in 1.7.6, you should move your modules
   to use the new functions
*/

#define change_user_mode(u, modes, arg) common_svsmode(u, modes, arg)
#define GetIdent(x) common_get_vident(x)
#define GetHost(x) common_get_vhost(x)

#define NEWNICK(nick,user,host,real,modes,qline) \
   anope_cmd_bot_nick(nick,user,host,real,modes)

#define s_akill(user, host, who, when, expires, reason) \
  anope_cmd_akill(user, host, who, when, expires, reason)

#define set_umode(user, ac, av) anope_set_umode(user, ac, av)

#define s_svsnoop(server, set) anope_cmd_svsnoop(server, set)

#define s_sqline(mask, reason) anope_cmd_sqline(mask, reason)

#define s_sgline(mask, reason) anope_cmd_sgline(mask, reason)
#define s_szline(mask, reason) anope_cmd_szline(mask, reason)
#define s_unsgline(mask) anope_cmd_unsgline(mask)
#define s_unsqline(mask) anope_cmd_unsqline(mask)
#define s_unszline(mask) anope_cmd_unszline(mask)

#define s_rakill(user, host) anope_cmd_remove_akill(user, host)


# define NICKSERV_MODE ircd->nickservmode
# define CHANSERV_MODE ircd->chanservmode
# define HOSTSERV_MODE ircd->hostservmode
# define MEMOSERV_MODE ircd->memoservmode
# define BOTSERV_MODE ircd->botservmode
# define HELPSERV_MODE ircd->helpservmode
# define OPERSERV_MODE ircd->oprservmode
# define DEVNULL_MODE ircd->devnullmode
# define GLOBAL_MODE ircd->globalmode
# define NICKSERV_ALIAS_MODE ircd->nickservaliasmode
# define CHANSERV_ALIAS_MODE ircd->chanservaliasmode
# define MEMOSERV_ALIAS_MODE ircd->memoservaliasmode
# define BOTSERV_ALIAS_MODE ircd->botservaliasmode
# define HELPSERV_ALIAS_MODE ircd->helpservaliasmode
# define OPERSERV_ALIAS_MODE ircd->operservaliasmode
# define DEVNULL_ALIAS_MODE ircd->devnullaliasmode
# define GLOBAL_ALIAS_MODE ircd->globalaliasmode
# define HOSTSERV_ALIAS_MODE ircd->hostservaliasmode
# define BOTSERV_BOTS_MODE ircd->botserv_bot_mode
#define CHAN_MAX_SYMBOL ircd->max_symbols
#define MODESTOREMOVE ircd->modestoremove

#ifdef IRC_HYBRID
# define HAS_HALFOP
# define HAS_EXCEPT
#endif
 
#ifdef IRC_VIAGRA
# define HAS_HALFOP
# define HAS_VHOST
# define HAS_VIDENT
# define HAS_EXCEPT
#endif

#ifdef IRC_BAHAMUT
# define HAS_NICKIP
# define HAS_EXCEPT
# define HAS_SVSHOLD                                                            
#endif
 
#ifdef IRC_RAGE2
# define HAS_HALFOP
# define HAS_EXCEPT
# define HAS_VHOST
# define HAS_NICKVHOST
#endif
 
#ifdef IRC_PTLINK
# define HAS_NICKVHOST
# define HAS_VHOST
# define HAS_FMODE
# define HAS_EXCEPT
#endif
 
#ifdef IRC_ULTIMATE2
# define IRC_ULTIMATE /* gotta do this for old mods */
# define HAS_FMODE
# define HAS_HALFOP
# define HAS_LMODE
# define HAS_VHOST
# define HAS_VIDENT
# define HAS_EXCEPT
#endif
 
#if defined(IRC_UNREAL31) || defined(IRC_UNREAL32)
# define IRC_UNREAL			/* gotta do this for old mods */
# define HAS_FMODE
# define HAS_HALFOP
# define HAS_LMODE
# define HAS_NICKVHOST
# define HAS_VHOST
# define HAS_VIDENT
# define HAS_EXCEPT
#endif
 
#ifdef IRC_ULTIMATE3
# define HAS_HALFOP
# define HAS_VHOST
# define HAS_NICKVHOST
# define HAS_VIDENT
# define HAS_EXCEPT
#endif



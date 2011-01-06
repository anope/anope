/* Declarations for command data.
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

 #include "modules.h"

/*************************************************************************/

/* Routines for looking up commands.  Command lists are arrays that must be
 * terminated with a NULL name.
 */

extern MDE Command *lookup_cmd(Command *list, char *name);
extern void run_cmd(char *service, User *u, Command *list,
		char *name);
extern void help_cmd(char *service, User *u, Command *list,
		char *name);
extern void do_run_cmd(char *service, User * u, Command *c,const char *cmd);
extern MDE void do_help_limited(char *service, User * u, Command * c);
extern void do_help_cmd(char *service, User * u, Command *c,const char *cmd);
extern MDE void mod_help_cmd(char *service, User *u, CommandHash *cmdTable[],const char *cmd);
extern MDE void mod_run_cmd(char *service, User *u, CommandHash *cmdTable[],const char *cmd);

/*************************************************************************/

/* Declarations for command data.
 *
 * (C) 2003-2008 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$ 
 *
 */

 #include "modules.h"

/*************************************************************************/

/* Routines for looking up commands.  Command lists are arrays that must be
 * terminated with a NULL name.
 */

extern Command *lookup_cmd(Command *list, char *name);
extern void run_cmd(char *service, User *u, Command *list,
		char *name);
extern void help_cmd(char *service, User *u, Command *list,
		char *name);
extern void do_run_cmd(char *service, User * u, Command *c,const char *cmd);
extern void do_help_cmd(char *service, User * u, Command *c,const char *cmd);
extern MDE void mod_help_cmd(char *service, User *u, CommandHash *cmdTable[],const char *cmd);
extern MDE void mod_run_cmd(char *service, User *u, CommandHash *cmdTable[],const char *cmd);

/*************************************************************************/

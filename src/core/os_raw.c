/* OperServ core functions
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
/*************************************************************************/

#include "module.h"

int do_raw(User * u);

/**
 * Create the command, and tell anope about it.
 * @param argc Argument count
 * @param argv Argument list
 * @return MOD_CONT to allow the module, MOD_STOP to stop it
 **/
int AnopeInit(int argc, char **argv)
{
    Command *c;

    moduleAddAuthor("Anope");
    moduleAddVersion(VERSION_STRING);
    moduleSetType(THIRD);

    c = createCommand("RAW", do_raw, is_services_root, OPER_HELP_RAW, -1,
                      -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);

    if (DisableRaw) {
		alog("[os_raw] Unloading because DisableRaw is enabled");
        return MOD_STOP;
    }
    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}


/**
 * The /os raw command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_raw(User * u)
{
    char *text = strtok(NULL, "");
    if (!text)
        syntax_error(s_OperServ, u, "RAW", OPER_RAW_SYNTAX);
    else {
        send_cmd(NULL, "%s", text);
        if (WallOSRaw) {
            char *kw = strtok(text, " ");
            while (kw && *kw == ':')
                kw = strtok(NULL, " ");
            anope_cmd_global(s_OperServ,
                             "\2%s\2 used RAW command for \2%s\2",
                             u->nick,
                             (kw ? kw : "\2non RFC compliant message\2"));
        }
        alog("%s used RAW command for %s", u->nick, text);
    }
    return MOD_CONT;
}

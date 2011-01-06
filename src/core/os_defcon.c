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

#ifdef _WIN32
extern MDE time_t DefContimer;
extern MDE void runDefCon(void);
#endif
int do_defcon(User * u);
void defcon_sendlvls(User * u);

void myOperServHelp(User * u);

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
    moduleSetType(CORE);

    c = createCommand("DEFCON", do_defcon, is_services_admin,
                      OPER_HELP_DEFCON, -1, -1, -1, -1);
    moduleAddCommand(OPERSERV, c, MOD_UNIQUE);

    moduleSetOperHelp(myOperServHelp);

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{

}


/**
 * Add the help response to anopes /os help output.
 * @param u The user who is requesting help
 **/
void myOperServHelp(User * u)
{
    if (is_services_admin(u)) {
        notice_lang(s_OperServ, u, OPER_HELP_CMD_DEFCON);
    }
}

/**
 * Defcon - A method of impelemting various stages of securty, the hope is this will help serives
 * protect a network during an attack, allowing admins to choose the precautions taken at each
 * level.
 *
 * /msg OperServ DefCon [level]
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 *
 **/
int do_defcon(User * u)
{
    char *lvl = strtok(NULL, " ");
    int newLevel = 0;
    char *langglobal;
    langglobal = getstring(NULL, DEFCON_GLOBAL);

    if (!DefConLevel) {         /* If we dont have a .conf setting! */
        notice_lang(s_OperServ, u, OPER_DEFCON_NO_CONF);
        return MOD_CONT;
    }

    if (!lvl) {
        notice_lang(s_OperServ, u, OPER_DEFCON_CHANGED, DefConLevel);
        defcon_sendlvls(u);
        return MOD_CONT;
    }
    newLevel = atoi(lvl);
    if (newLevel < 1 || newLevel > 5) {
	syntax_error(s_OperServ, u, "DEFCON", OPER_DEFCON_SYNTAX);
        return MOD_CONT;
    }
    DefConLevel = newLevel;
    send_event(EVENT_DEFCON_LEVEL, 1, lvl);
    DefContimer = time(NULL);
    notice_lang(s_OperServ, u, OPER_DEFCON_CHANGED, DefConLevel);
    defcon_sendlvls(u);
    alog("Defcon level changed to %d by Oper %s", newLevel, u->nick);
    anope_cmd_global(s_OperServ, getstring2(NULL, OPER_DEFCON_WALL),
                     u->nick, newLevel);
    /* Global notice the user what is happening. Also any Message that
       the Admin would like to add. Set in config file. */
    if (GlobalOnDefcon) {
        if ((DefConLevel == 5) && (DefConOffMessage)) {
            oper_global(NULL, "%s", DefConOffMessage);
        } else {
            oper_global(NULL, langglobal, DefConLevel);
        }
    }
    if (GlobalOnDefconMore) {
        if ((DefConOffMessage) && DefConLevel == 5) {
        } else {
            oper_global(NULL, "%s", DefconMessage);
        }
    }
    /* Run any defcon functions, e.g. FORCE CHAN MODE */
    runDefCon();
    return MOD_CONT;
}



/**
 * Send a message to the oper about which precautions are "active" for this level
 **/
void defcon_sendlvls(User * u)
{
    if (checkDefCon(DEFCON_NO_NEW_CHANNELS)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_CHANNELS);
    }
    if (checkDefCon(DEFCON_NO_NEW_NICKS)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_NICKS);
    }
    if (checkDefCon(DEFCON_NO_MLOCK_CHANGE)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_NO_MLOCK_CHANGE);
    }
    if (checkDefCon(DEFCON_FORCE_CHAN_MODES) && (DefConChanModes)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_FORCE_CHAN_MODES,
                    DefConChanModes);
    }
    if (checkDefCon(DEFCON_REDUCE_SESSION)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_REDUCE_SESSION,
                    DefConSessionLimit);
    }
    if (checkDefCon(DEFCON_NO_NEW_CLIENTS)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_CLIENTS);
    }
    if (checkDefCon(DEFCON_OPER_ONLY)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_OPER_ONLY);
    }
    if (checkDefCon(DEFCON_SILENT_OPER_ONLY)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_SILENT_OPER_ONLY);
    }
    if (checkDefCon(DEFCON_AKILL_NEW_CLIENTS)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_AKILL_NEW_CLIENTS);
    }
    if (checkDefCon(DEFCON_NO_NEW_MEMOS)) {
        notice_lang(s_OperServ, u, OPER_HELP_DEFCON_NO_NEW_MEMOS);
    }
}

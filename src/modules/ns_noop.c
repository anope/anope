/* ns_noop.c - Allows users to optionaly set autoop to off
 *
 * (C) 2003-2005 Anope Team
 * Contact us at info@anope.org
 *
 * Based on the original module by Rob <rob@anope.org>
 * Included in the Anope module pack since Anope 1.7.9
 * Anope Coder: DrStein <drstein@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Send bug reports to the Anope Coder instead of the module
 * author, because any changes since the inclusion into anope
 * are not supported by the original author.
 *
 */
/*************************************************************************/

#include "module.h"

#define AUTHOR "Rob"
#define VERSION "$Id$"

/* The name of the default database to save info to */
#define DEFAULT_DB_NAME "autoop.db"

/* Multi-language stuff */
#define LANG_NUM_STRINGS  8

#define AUTOOP_SYNTAX     0
#define AUTOOP_STATUS_ON  1
#define AUTOOP_STATUS_OFF 2
#define AUTOOP_NO_NICK    3
#define AUTOOP_ON         4
#define AUTOOP_OFF        5
#define AUTOOP_DESC       6
#define AUTOOP_HELP       7

/*************************************************************************/

User *currentUser;
int m_isIRCop = 0;

char *NSAutoOPDBName;

int myNickServAutoOpHelp(User * u);
void myNickServHelp(User * u);

int noop(User * u);
int mEventJoin(int argc, char **argv);
int setAutoOp(User * u);
int UnsetAutoOp(User * u);

int mLoadData(void);
int mSaveData(int argc, char **argv);
int mLoadConfig(int argc, char **argv);

void m_AddLanguages(void);
void delete_ignore(const char *nick);

/*************************************************************************/

/**
 * AnopeInit is called when the module is loaded
 * @param argc Argument count
 * @param argv Argument list
 * @return MOD_CONT to allow the module, MOD_STOP to stop it
 **/
int AnopeInit(int argc, char **argv)
{
    Command *c = NULL;
    EvtHook *hook = NULL;

    int status;

    NSAutoOPDBName = NULL;

    moduleAddAuthor(AUTHOR);
    moduleAddVersion(VERSION);
    moduleSetType(SUPPORTED);

    alog("ns_noop: Loading configuration directives");
    if (mLoadConfig(0, NULL))
        return MOD_STOP;

    c = createCommand("autoop", noop, NULL, -1, -1, -1, -1, -1);
    status = moduleAddCommand(NICKSERV, c, MOD_HEAD);

    hook = createEventHook(EVENT_JOIN_CHANNEL, mEventJoin);
    status = moduleAddEventHook(hook);

    hook = createEventHook(EVENT_DB_SAVING, mSaveData);
    status = moduleAddEventHook(hook);

    hook = createEventHook(EVENT_RELOAD, mLoadConfig);
    status = moduleAddEventHook(hook);

    moduleAddHelp(c, myNickServAutoOpHelp);
    moduleSetNickHelp(myNickServHelp);

    mLoadData();
    m_AddLanguages();

    return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{
    char *av[1];

    av[0] = sstrdup(EVENT_START);
    mSaveData(1, av);
    free(av[0]);

    if (NSAutoOPDBName)
        free(NSAutoOPDBName);
}

/*************************************************************************/

/**
 * Provide de user interface to set autoop on/off
 * @param u The user who executed this command
 * @return MOD_CONT
 **/
int noop(User * u)
{
    NickAlias *na = NULL;
    char *toggleStr = strtok(NULL, "");

    if (!nick_identified(u)) {
        moduleNoticeLang(s_NickServ, u, AUTOOP_NO_NICK);
    } else if (!toggleStr) {
        if ((na = findnick(u->nick))) {
            if (moduleGetData(&na->nc->moduleData, "autoop")) {
                moduleNoticeLang(s_NickServ, u, AUTOOP_STATUS_OFF);
            } else {
                moduleNoticeLang(s_NickServ, u, AUTOOP_STATUS_ON);
            }
        } else {
            moduleNoticeLang(s_NickServ, u, AUTOOP_SYNTAX);
        }
    } else {
        if (strcasecmp(toggleStr, "on") == 0) {
            setAutoOp(u);
            moduleNoticeLang(s_NickServ, u, AUTOOP_ON);
        } else if (strcasecmp(toggleStr, "off") == 0) {
            UnsetAutoOp(u);
            moduleNoticeLang(s_NickServ, u, AUTOOP_OFF);
        } else {
            moduleNoticeLang(s_NickServ, u, AUTOOP_SYNTAX);
        }
    }
    return MOD_CONT;
}

/**
 * Toggle on/off the autoop feature by adding or
 * deleting moduleData.
 * @param u The user who executed this command
 * @return 0 On success
 **/
int setAutoOp(User * u)
{
    NickAlias *na = NULL;

    if ((na = findnick(u->nick))) {
        /* Remove the module data */
        moduleDelData(&na->nc->moduleData, "autoop");
        /* NickCore not found! */
    } else {
        alog("ns_autoop.so: WARNING: Can not find NickAlias for %s",
             u->nick);
    }

    return 0;
}

int UnsetAutoOp(User * u)
{
    NickAlias *na = NULL;

    if ((na = findnick(u->nick))) {
        /* Add the module data to the user */
        moduleAddData(&na->nc->moduleData, "autoop", "off");
        /* NickCore not found! */
    } else {
        alog("ns_autoop.so: WARNING: Can not find NickAlias for %s",
             u->nick);
    }

    return 0;
}

/*************************************************************************/

/** 
 * Manage the JOIN_CHANNEL EVENT
 * @return MOD_CONT
 **/
int mEventJoin(int argc, char **argv)
{
    User *user;
    NickAlias *na = NULL;

    if (argc != 3)
        return MOD_CONT;

    user = finduser(argv[1]);
    /* Blame Rob if this user->na should be findnick(user->nick); -GD */
    if (user && (na = user->na)) {
        if (strcmp(argv[0], EVENT_START) == 0) {
            if (moduleGetData(&na->nc->moduleData, "autoop")) {
                currentUser = user;
                if (is_oper(user)) {
                    user->mode &= ~(anope_get_oper_mode());
                    m_isIRCop = 1;
                }
                add_ignore(user->nick, 120);
            }
        } else {
            /* Does the user have the autoop info in his moduleData? */
            if (moduleGetData(&na->nc->moduleData, "autoop")) {
                /* The most dirty solution ever! - doc */
                if (m_isIRCop)
                    user->mode |= anope_get_oper_mode();
                delete_ignore(user->nick);
            }
        }
    }

    return MOD_CONT;
}

/*************************************************************************/

/** 
 * Load data from the db file, and populate the autoop setting
 * @return 0 for success
 **/
int mLoadData(void)
{
    int ret = 0;
    int len = 0;

    char *name = NULL;

    NickAlias *na = NULL;
    FILE *in;

    /* will _never_ be this big thanks to the 512 limit of a message */
    char buffer[2000];
    if ((in = fopen(NSAutoOPDBName, "r")) == NULL) {
        alog("ns_noop: WARNING: Can not open database file! (it might not exist, this is not fatal)");
        ret = 1;
    } else {
        while (!feof(in)) {
            fgets(buffer, 1500, in);
            name = myStrGetToken(buffer, ' ', 0);
            if (name) {
                len = strlen(name);
                /* Take the \n from the end of the line */
                name[len - 1] = '\0';
                if ((na = findnick(name))) {
                    moduleAddData(&na->nc->moduleData, "autoop", "off");
                }
                free(name);
            }
        }
    }
    return ret;
}

/** 
 * Save all our data to our db file
 * First walk through the nick CORE list, and any nick core which has
 * the autoop setting, write to the file.
 * @return 0 for success
 **/
int mSaveData(int argc, char **argv)
{
    NickCore *nc = NULL;
    int i = 0;
    int ret = 0;
    FILE *out;

    if (argc >= 1) {
        if (stricmp(argv[0], EVENT_START) == 0) {
            alog("ns_noop: Saving the databases has started!");
            if ((out = fopen(NSAutoOPDBName, "w")) == NULL) {
                alog("ns_noop: ERROR: can not open the database file!");
                anope_cmd_global(s_NickServ,
                                 "ns_noop: ERROR: can not open the database file!");
                ret = 1;
            } else {
                for (i = 0; i < 1024; i++) {
                    for (nc = nclists[i]; nc; nc = nc->next) {
                        /* If we have any info on this user */
                        if (moduleGetData(&nc->moduleData, "autoop")) {
                            fprintf(out, "%s\n", nc->display);
                        }
                    }
                }
                fclose(out);
            }
        } else {
            alog("ns_noop: Saving the databases is complete");
            ret = 0;
        }
    }

    return ret;
}

/*************************************************************************/

/** 
 * Load the configuration directives from Services configuration file.
 * @return 0 for success
 **/
int mLoadConfig(int argc, char **argv)
{
    char *tmp = NULL;

    Directive d[] = {
        {"NSAutoOPDBName", {{PARAM_STRING, PARAM_RELOAD, &tmp}}},
    };

    moduleGetConfigDirective(d);

    if (NSAutoOPDBName)
        free(NSAutoOPDBName);

    if (tmp) {
        NSAutoOPDBName = tmp;
    } else {
        NSAutoOPDBName = sstrdup(DEFAULT_DB_NAME);
        alog("ns_noop: NSAutoOPDBName is not defined in Services configuration file, using default %s", NSAutoOPDBName);
    }

    if (!NSAutoOPDBName) {
        alog("ns_noop: FATAL: Can't read required configuration directives!");
        return MOD_STOP;
    } else {
        alog("ns_noop: Directive NSAutoOPDBName loaded (%s)...",
             NSAutoOPDBName);
    }

    return MOD_CONT;
}

/*************************************************************************/

/**
 *  Below are the help funcitons :)
 **/
void myNickServHelp(User * u)
{
    moduleNoticeLang(s_NickServ, u, AUTOOP_DESC);
}

int myNickServAutoOpHelp(User * u)
{
    moduleNoticeLang(s_NickServ, u, AUTOOP_SYNTAX);
    moduleNoticeLang(s_NickServ, u, AUTOOP_HELP, s_ChanServ);

    return MOD_CONT;
}

/*************************************************************************/

/**
 * manages the multilanguage stuff
 **/
void m_AddLanguages(void)
{
    /* English (US) */
    char *langtable_en_us[] = {
        /* AUTOOP_SYNTAX */
        "Syntax: AUTOOP [ON|OFF]",
        /* AUTOOP_STATUS_ON */
        "Your current AUTOOP setting is ON",
        /* AUTOOP_STATUS_OFF */
        "Your current AUTOOP setting is OFF",
        /* AUTOOP_NO_NICK */
        "Only registered and identified nicknames can set this option",
        /* AUTOOP_ON */
        "You will now be auto op'ed in channels when you join",
        /* AUTOOP_OFF */
        "You will no longer be auto op'ed in channels when you join them",
        /* AUTOOP_DESC */
        "    AUTOOP     Toggles auto-op'ing when joining rooms",
        /* AUTOOP_HELP */
        "When set to ON, this command will prevent %s setting any\n"
            "modes on you when you join any channel. This command requires\n"
            "you to be identified."
    };

    /* Spanish */
    char *langtable_es[] = {
        /* AUTOOP_SYNTAX */
        "Sintaxis: AUTOOP [ON|OFF]",
        /* AUTOOP_STATUS_ON */
        "Tu configuracion actual de AUTOOP es ON",
        /* AUTOOP_STATUS_OFF */
        "Tu configuracion actual de AUTOOP es OFF",
        /* AUTOOP_NO_NICK */
        "Solo nicknames registrados e identificados pueden usar esta opcion",
        /* AUTOOP_ON */
        "Recibiras OP automaticamente cuando entres a un canal.",
        /* AUTOOP_OFF */
        "Ya no recibiras OP automaticamente.",
        /* AUTOOP_DESC */
        "    AUTOOP     Cambia la opcion de auto-op cuando entras a un canal",
        /* AUTOOP_HELP */
        "Cuando esta en ON, evitaras que ChanServ/BotServ\n"
            "cambien tus modos en el canal que tengas acceso.\n",
        "(Debes estar identificado para usar esta opcion)"
    };

    /* Dutch */
    char *langtable_nl[] = {
        /* AUTOOP_SYNTAX */
        "Gebruik: AUTOOP [ON|OFF]",
        /* AUTOOP_STATUS_ON */
        "Je huidige AUTOOP instelling is AAN",
        /* AUTOOP_STATUS_OFF */
        "Je huidige AUTOOP instelling is UIT",
        /* AUTOOP_NO_NICK */
        "Alleen geregistreerd en geidentificeerde nicks kunnen deze optie gebruiken",
        /* AUTOOP_ON */
        "Je zal nu kanaal-op worden in kanalen wanneer je die binnen komt",
        /* AUTOOP_OFF */
        "Je zal nu geen kanaal-op meer worden in kanalen wanneer je die binnen komt",
        /* AUTOOP_DESC */
        "    AUTOOP     Zet auto-op aan of uit",
        /* AUTOOP_HELP */
        "Wanner dit aan (ON) staat, zal dit commando er voor zorgen dat\n"
            "%s geen kanaalmodes aan jou zal geven wanneer je een\n"
            "kanaal binnen komt. Voor dit command is het vereist dat je\n"
            "geidentificeerd bent."
    };

    moduleInsertLanguage(LANG_EN_US, LANG_NUM_STRINGS, langtable_en_us);
    moduleInsertLanguage(LANG_ES, LANG_NUM_STRINGS, langtable_es);
    moduleInsertLanguage(LANG_NL, LANG_NUM_STRINGS, langtable_nl);
}

/*************************************************************************/

/**
 * Deletes a nick from the ignore list
 * @param nick nickname to unignore.
 **/
void delete_ignore(const char *nick)
{
    IgnoreData *ign, *prev;
    IgnoreData **whichlist;

    if (!nick || !*nick) {
        return;
    }

    whichlist = &ignore[tolower(nick[0])];

    for (ign = *whichlist, prev = NULL; ign; prev = ign, ign = ign->next) {
        if (stricmp(ign->who, nick) == 0)
            break;
    }
    if (ign) {
        if (prev)
            prev->next = ign->next;
        else
            *whichlist = ign->next;
        free(ign);
        ign = NULL;
    }
}

/* EOF */

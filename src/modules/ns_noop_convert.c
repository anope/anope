/* ns_noop.c - Allows users to optionaly set autoop to off
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
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
#define VERSION VERSION_STRING

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

/*************************************************************************/

/**
 * AnopeInit is called when the module is loaded
 * @param argc Argument count
 * @param argv Argument list
 * @return MOD_CONT to allow the module, MOD_STOP to stop it
 **/
int AnopeInit(int argc, char **argv)
{
    NSAutoOPDBName = NULL;

    moduleAddAuthor(AUTHOR);
    moduleAddVersion(VERSION);
    moduleSetType(SUPPORTED);

    if (mLoadConfig(0, NULL))
        return MOD_STOP;

    mLoadData();
	
    alog("ns_noop_convert: Your auto-op database has been converted and this module will now");
    alog("ns_noop_convert: unload itself.  You can now remove this module from your config");

    return MOD_STOP;
}

/**
 * Unload the module
 **/
void AnopeFini(void)
{
    if (NSAutoOPDBName)
        free(NSAutoOPDBName);
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
        while (fgets(buffer, 1500, in)) {
            name = myStrGetToken(buffer, ' ', 0);
            if (name) {
                len = strlen(name);
                /* Take the \n from the end of the line */
                name[len - 1] = '\0';
                if ((na = findnick(name))) {
	    	    na->nc->flags |= NI_AUTOOP;
                }
                free(name);
            }
        }
        fclose(in);
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

/* EOF */

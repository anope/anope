#include "module.h"

#define AUTHOR "Anope"
#define VERSION "1"

/**
 * Default setting to be used if no confing value is found
 **/
#define DEFAULT_SETTING "moo"

int mShowSetting(User *u);
int mReadConfig(int argc, char **argv);

char *setting;

int AnopeInit(int argc, char **argv) {
    Command *c;	
    EvtHook *hook;
    int status = 0;

    setting = NULL; 
 
    mReadConfig(0,NULL);
    
    c = createCommand("SHOW",mShowSetting,NULL,-1,-1,-1,-1,-1);
    status = moduleAddCommand(HOSTSERV, c, MOD_HEAD);

    hook = createEventHook(EVENT_RELOAD, mReadConfig);
    status = moduleAddEventHook(hook);
    
    if(status!=MOD_ERR_OK) {
	return MOD_STOP;
    }
    return MOD_CONT;
}

/**
 * free the globals when we close
 **/
void AnopeFini(void) {
	if(setting) 
		free(setting);
}

/**
 * Simple function to show the user the current config setting
 **/
int mShowSetting(User *u) {
    notice(s_HostServ,u->nick,"Setting in use is [%s]",setting);
    return MOD_CONT;
}

/**
 * Load the config setting up, this will be called whenever
 * the EVENT_RELOAD event is recived.
 **/
int mReadConfig(int argc, char **argv) {
    char *tmp=NULL;
    Directive d[] = {{"HSConfigSetting", {{PARAM_STRING, PARAM_RELOAD, &tmp}}}};
    moduleGetConfigDirective(d);

    if(setting) {
	    free(setting);
    }
    if(tmp) {
	setting = tmp;
    } else {
	setting = sstrdup(DEFAULT_SETTING);
    }
    return MOD_CONT;
}


/* EOF */

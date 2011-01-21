/**
 * Simple module to show the usage of event messages and hooks
 * This module is an example, and has no useful purpose!
 *
 * Please visit http://modules.anope.org for useful modules! 
 *
 **/

#include "module.h"

#define AUTHOR "Anope"
#define VERSION VERSION_STRING

int my_nick(char *source, int ac, char **av);
int my_save(int argc, char **argv);
int do_moo(int argc, char **argv);

int AnopeInit(int argc, char **argv)
{
    EvtMessage *msg = NULL;
    EvtHook    *hook = NULL;
    int status;
    msg = createEventHandler("NICK", my_nick);
    status = moduleAddEventHandler(msg);

    hook = createEventHook(EVENT_DB_SAVING, my_save);
    status = moduleAddEventHook(hook);


    hook = createEventHook(EVENT_BOT_FANTASY, do_moo);
    status = moduleAddEventHook(hook);
    
    moduleAddAuthor(AUTHOR);
    moduleAddVersion(VERSION);
    return MOD_CONT;
}

void AnopeFini(void)
{
 /* unloading */
}

int my_nick(char *source, int ac, char **av)
{
  alog("Internal Event - nick is %s",av[0]);
  return MOD_CONT;
}

int my_save(int argc, char **argv)
{
  if(argc>=1) {
	  if (!stricmp(argv[0], EVENT_START)) {
	      alog("Saving the databases! has started");
	  } else {
	      alog("Saving the databases is complete");
	  }
  }
  return MOD_CONT;
}

/**
 * command to be called when a EVENT_BOT_FANTASY event is recived.
 * @param argc The paramater count for this event.
 * @param argv[0] The cmd used.
 * @param argv[1] The nick of the command user.
 * @param argv[2] The channel name the command was used in.
 * @param argv[3] The rest of the text after the command.
 * @return MOD_CONT or MOD_STOP
 **/
int do_moo(int argc, char **argv) {
    ChannelInfo *ci;
    if(argc>=3) { /* We need at least 3 arguments */
	if(stricmp(argv[0],"moo")==0) { /* is it meant for us? */
            if((ci = cs_findchan(argv[2]))) { /* channel should always exist */
                anope_cmd_privmsg(ci->bi->nick, ci->name, "%cACTION moos at %s %c",1,argv[1],1);
	        return MOD_STOP; /* We've dealt with it, don't let others */
            }
	} 
    }
    return MOD_CONT; /* guess it wasn't for us, pass it on */
}


/**
 * Simple module to show the usage of event messages and hooks
 * This module is an example, and has no useful purpose!
 *
 * Please visit http://modules.anope.org for useful modules! 
 *
 **/

#include "module.h"

#define AUTHOR "Anope"
#define VERSION "1.1"

int my_nick(char *source, int ac, char **av);
int my_save(char *message);

int AnopeInit(int argc, char **argv)
{
    EvtMessage *msg = NULL;
    EvtHook    *hook = NULL;
    int status;
    msg = createEventHandler("NICK", my_nick);
    status = moduleAddEventHandler(msg);

    hook = createEventHook(EVENT_DB_SAVING, my_save);
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

int my_save(char *message)
{
  if (!stricmp(message, EVENT_START)) {
      alog("Saving the databases! has started");
  } else {
      alog("Saving the databases is complete");
  }
  return MOD_CONT;
}


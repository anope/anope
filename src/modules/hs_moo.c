/**
 * Simple module to remove the Topiclock flag from _all_ channels and then unload itsself.
 **/
#include "module.h"

#define AUTHOR "Rob"									/* Set the Author for a modinfo reply */
#define VERSION "1.0"									/* Set the version for a modinfo reply */

int AnopeInit(int argc, char **argv)							/* This will be executed when the module is loaded */
{
	int i;
	ChannelInfo *ci;
	for (i = 0; i < 256; i++) {
		for (ci = chanlists[i]; ci; ci = ci->next) {
			ci->flags &= ~CI_TOPICLOCK;
		}
	}
	return MOD_STOP;
}

/* EOF */

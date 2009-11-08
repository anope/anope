#include "services.h"

NickAlias::NickAlias()
{
	next = prev = NULL;
	nick = last_quit = last_realname = last_usermask = NULL;
	time_registered = last_seen = 0;
	nc = NULL;
}


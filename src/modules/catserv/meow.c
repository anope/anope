#include "meow.h"

#include "catserv_extern.h"

int do_meow(User * u) {
	notice(s_CatServ, u->nick, "MEOW!");
	return MOD_STOP;
}


#include "purr.h"
#include "catserv_extern.h"

int do_purr(User * u)
{
    notice(s_CatServ, u->nick, "PURR!");
    return MOD_STOP;
}

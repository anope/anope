#include "catserv_messages.h"
#include "module.h"
#include "meow.h"
#include "purr.h"

void addMessageList(void)
{
    Command *c;
    c = createCommand("meow", do_meow, NULL, -1, -1, -1, -1, -1);
    moduleAddCommand(Catserv_cmdTable, c, MOD_UNIQUE);
    c = createCommand("purr", do_purr, NULL, -1, -1, -1, -1, -1);
    moduleAddCommand(Catserv_cmdTable, c, MOD_UNIQUE);
}


/* RDB functions.
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$ 
 *
 */
#include "services.h"

#ifdef USE_MYSQL
#define RDB_MYSQL(x) x
#else
#define RDB_MYSQL(x)
#endif

void rdb_init(void)
{
    RDB_MYSQL(db_mysql_init());
}

void rdb_end(void)
{
    RDB_MYSQL(db_mysql_end());
}

/* BotServ */
void rdb_bs_add_bot(BotInfo *bi)
{
    RDB_MYSQL(db_mysql_bs_add_bot(bi));
}

void rdb_bs_del_bot(BotInfo *bi)
{
    RDB_MYSQL(db_mysql_bs_del_bot(bi));
}

void rdb_bs_change_bot(BotInfo *bi, char *newnick, char *newuser, char *newhost, char *newreal)
{
    RDB_MYSQL(db_mysql_bs_chg_bot(bi, newnick, newuser, newhost, newreal));
}

void rdb_bs_change_bot_chancount(BotInfo *bi)
{
    RDB_MYSQL(db_mysql_bs_chg_bot_chancount(bi));
}

void rdb_bs_change_bot_flags(BotInfo *bi)
{
    RDB_MYSQL(db_mysql_bs_chg_bot_flags(bi));
}

void rdb_cs_add_access(ChannelInfo *ci, ChanAccess *access)
{
    RDB_MYSQL(db_mysql_cs_add_access(ci, access));
}

void rdb_cs_del_access(ChanAccess *access)
{
    ChannelInfo *ci, *found;
    ChanAccess *caccess;
    int i, y;
    /* Need to fetch channel manually since ChanAccess has no member for it */
    for (i = 0; i < 256; i++) {
        for (ci = chanlists[i]; ci; ci = ci->next) {
            for (caccess = ci->access, y = 0; y < ci->accesscount; caccess++, y++) {
                if (caccess == access) { found = ci; }
            }
        }
    }
    if (!found) return;
    RDB_MYSQL(db_mysql_cs_del_access(found, access));
}

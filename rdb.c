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

/*************************************************************************/

int rdb_init()
{

#ifdef USE_MYSQL
    return db_mysql_init();
#endif

}

/*************************************************************************/

int rdb_open()
{

#ifdef USE_MYSQL
    return do_mysql;            // db_mysql_open();
#endif

}

/*************************************************************************/

int rdb_close()
{

#ifdef USE_MYSQL
    return 1;                   // db_mysql_close();
#endif

}

/*************************************************************************/

int rdb_tag_table(char *table)
{
    static char buf[1024];

#ifdef USE_MYSQL
    snprintf(buf, sizeof(buf), "UPDATE %s SET active='0'", table);
    return db_mysql_query(buf);
#endif

    return 0;

}

/*************************************************************************/

int rdb_clear_table(char *table)
{
    static char buf[1024];

#ifdef USE_MYSQL
    snprintf(buf, sizeof(buf), "TRUNCATE TABLE %s", table);
    return db_mysql_query(buf);
#endif

    return 0;

}

/*************************************************************************/

int rdb_scrub_table(char *table, char *clause)
{

    static char buf[1024];

#ifdef USE_MYSQL
    snprintf(buf, sizeof(buf), "DELETE FROM %s WHERE %s", table, clause);
    return db_mysql_query(buf);
#endif

    return 0;

}

/*************************************************************************/

int rdb_direct_query(char *query)
{

#ifdef USE_MYSQL
    alog("Direct Query: %s", query);
    return db_mysql_query(query);
#endif

    return 0;

}

/*************************************************************************/

/* I still don't really like doing it this way, it should really be done
 * inside mysql.c and not here. So I'll revisit this later
 */
int rdb_ns_set_display(char *newnick, char *oldnick)
{
    static char buf[1024];

#ifdef USE_MYSQL
    /* Change the display on NS_CORE */
    snprintf(buf, sizeof(buf),
             "UPDATE anope_ns_core SET display='%s' WHERE display='%s'",
             newnick, oldnick);
    db_mysql_query(buf);

    /* Change the display on NS_ALIAS for all grouped nicks */
    snprintf(buf, sizeof(buf),
             "UPDATE anope_ns_alias SET display='%s' WHERE display='%s'",
             newnick, oldnick);
    db_mysql_query(buf);

    /* Change the display on ChanServ ACCESS list */
    snprintf(buf, sizeof(buf),
             "UPDATE anope_cs_access SET display='%s' WHERE display='%s'",
             newnick, oldnick);
    db_mysql_query(buf);

    /* Change the display on ChanServ AKICK list */
    snprintf(buf, sizeof(buf),
             "UPDATE anope_cs_access SET creator='%s' WHERE creator='%s'",
             newnick, oldnick);
    db_mysql_query(buf);

    /* Change the display on MemoServ sent memos */
    snprintf(buf, sizeof(buf),
             "UPDATE anope_ms_info SET sender='%s' WHERE sender='%s'",
             newnick, oldnick);
    db_mysql_query(buf);

    /* Change the display on MemoServ received memos */
    snprintf(buf, sizeof(buf),
             "UPDATE anope_ms_info SET receiver='%s' WHERE receiver='%s'",
             newnick, oldnick);
    db_mysql_query(buf);

    /* Need to do bwords and akills */

#endif

    return 0;
}

/*************************************************************************/

int rdb_cs_deluser(char *nick)
{
    static char buf[1024];

#ifdef USE_MYSQL
    snprintf(buf, sizeof(buf),
             "UPDATE anope_cs_info SET successor=NULL WHERE successor='%s'",
             nick);
    db_mysql_query(buf);

    snprintf(buf, sizeof(buf), "display='%s'", nick);
    rdb_scrub_table("anope_cs_access", buf);
    snprintf(buf, sizeof(buf), "creator='%s'", nick);
    rdb_scrub_table("anope_cs_akicks", buf);

    return 1;
#endif

    return 0;
}

/*************************************************************************/

int rdb_cs_delchan(ChannelInfo * ci)
{
    static char buf[1024];
    char *channel = ci->name;

#ifdef USE_MYSQL
    snprintf(buf, sizeof(buf),
             "UPDATE anope_cs_info SET successor=NULL WHERE name='%s'",
             channel);
    db_mysql_query(buf);

    snprintf(buf, sizeof(buf), "name='%s'", channel);
    rdb_scrub_table("anope_cs_info", buf);
    snprintf(buf, sizeof(buf), "receiver='%s' AND serv='CHAN'", channel);
    rdb_scrub_table("anope_ms_info", buf);
    snprintf(buf, sizeof(buf), "channel='%s'", channel);
    rdb_scrub_table("anope_cs_access", buf);
    rdb_scrub_table("anope_cs_akicks", buf);
    rdb_scrub_table("anope_cs_levels", buf);
    rdb_scrub_table("anope_cs_badwords", buf);
    if (ci->founder) {
        snprintf(buf, sizeof(buf),
                 "update anope_ns_core set channelcount=channelcount-1 where display='%s'",
                 ci->founder->display);
        db_mysql_query(buf);
    }

    return 1;
#endif

    return 0;
}

/*************************************************************************/

int rdb_cs_set_founder(char *channel, char *founder)
{
    static char buf[1024];

#ifdef USE_MYSQL
    snprintf(buf, sizeof(buf),
             "UPDATE anope_cs_info SET founder='%s', successor=NULL WHERE name='%s'",
             founder, channel);
    db_mysql_query(buf);

    snprintf(buf, sizeof(buf),
             "UPDATE anope_ns_core SET channelcount=channelcount+1 WHERE display='%s'",
             founder);
    db_mysql_query(buf);

    /* Do i need to scrub the access list for this channel ? */
    snprintf(buf, sizeof(buf), "display='%s' AND channel='%s'", founder,
             channel);
    rdb_scrub_table("anope_cs_access", buf);

    return 1;
#endif

    return 0;
}

/*************************************************************************/

void rdb_save_ns_core(NickCore * nc)
{

#ifdef USE_MYSQL
    db_mysql_save_ns_core(nc);
#endif

}

/*************************************************************************/

void rdb_save_ns_alias(NickAlias * na)
{

#ifdef USE_MYSQL
    db_mysql_save_ns_alias(na);
#endif

}

/*************************************************************************/

void rdb_save_ns_req(NickRequest * nr)
{

#ifdef USE_MYSQL
    db_mysql_save_ns_req(nr);
#endif

}

/*************************************************************************/

void rdb_save_cs_info(ChannelInfo * ci)
{

#ifdef USE_MYSQL
    db_mysql_save_cs_info(ci);
#endif

}

/*************************************************************************/

void rdb_save_bs_core(BotInfo * bi)
{

#ifdef USE_MYSQL
    db_mysql_save_bs_core(bi);
#endif

}

/*************************************************************************/

void rdb_save_hs_core(HostCore * hc)
{

#ifdef USE_MYSQL
    db_mysql_save_hs_core(hc);
#endif

}

/*************************************************************************/

void rdb_save_os_db(unsigned int maxucnt, unsigned int maxutime,
                    SList * ak, SList * sgl, SList * sql, SList * szl,
                    HostCache * hc)
{

#ifdef USE_MYSQL
    db_mysql_save_os_db(maxusercnt, maxusertime, ak, sgl, sql, szl, hc);
#endif

}

/*************************************************************************/

void rdb_save_news(NewsItem * ni)
{

#ifdef USE_MYSQL
    db_mysql_save_news(ni);
#endif

}

/*************************************************************************/

void rdb_load_bs_dbase(void)
{

#ifdef USE_MYSQL
    db_mysql_load_bs_dbase();
#endif

}

/*************************************************************************/

void rdb_load_hs_dbase(void)
{

#ifdef USE_MYSQL
    db_mysql_load_hs_dbase();
#endif

}

/*************************************************************************/

void rdb_load_ns_dbase(void)
{

#ifdef USE_MYSQL
    db_mysql_load_ns_dbase();
#endif
}

/*************************************************************************/

void rdb_load_news(void)
{
#ifdef USE_MYSQL
    db_mysql_load_news();
#endif
}

/*************************************************************************/

void rdb_load_exceptions(void)
{
#ifdef USE_MYSQL
    db_mysql_load_exceptions();
#endif
}

/*************************************************************************/

void rdb_load_cs_dbase(void)
{
#ifdef USE_MYSQL
    db_mysql_load_cs_dbase();
#endif
}

/*************************************************************************/

void rdb_load_os_dbase(void)
{
#ifdef USE_MYSQL
    db_mysql_load_os_dbase();
#endif
}

/*************************************************************************/

void rdb_load_ns_req_dbase(void)
{
#ifdef USE_MYSQL
    db_mysql_load_ns_req_dbase();
#endif
}

/*************************************************************************/

void rdb_load_dbases(void)
{
    if (!skeleton) {
        rdb_load_ns_dbase();
        if (debug)
            alog("RDB: Loaded NickServ DataBase (1/8)");
        if (s_HostServ) {
            rdb_load_hs_dbase();
            if (debug)
                alog("RDB: Loaded HostServ DataBase (2/8)");
        }
        if (s_BotServ) {
            rdb_load_bs_dbase();
            if (debug)
                alog("RDB: Loaded BotServ DataBase (3/8)");
        }
        rdb_load_cs_dbase();
        if (debug)
            alog("RDB: Loaded ChanServ DataBase (4/8)");
    }
    rdb_load_os_dbase();
    if (debug)
        alog("RDB: Loaded OperServ DataBase (5/8)");
    rdb_load_news();
    if (debug)
        alog("RDB: Loaded News DataBase (6/8)");
    rdb_load_exceptions();
    if (debug)
        alog("RDB: Loaded Exception Database (7/8)");
    if (PreNickDBName) {
        rdb_load_ns_req_dbase();
        if (debug)
            alog("RDB: Loaded PreNick DataBase (8/8)");
    } else {
        if (debug)
            alog("RDB: No need to load PreNickDB (8/8)");
    }
    alog("RDB: All DataBases loaded.");
}

/*************************************************************************/

void rdb_save_exceptions(Exception * e)
{

#ifdef USE_MYSQL
    db_mysql_save_exceptions(e);
#endif

}

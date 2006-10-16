/* RDB functions.
 *
 * (C) 2003-2005 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
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
    return do_mysql;            /* db_mysql_open(); */
#endif

}

/*************************************************************************/

int rdb_close()
{

#ifdef USE_MYSQL
    return 1;                   /* db_mysql_close(); */
#endif

}

/*************************************************************************/

char *rdb_quote(char *str)
{
#ifdef USE_MYSQL
    return db_mysql_quote(str);
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
    char *q_newnick;
    char *q_oldnick;

    q_newnick = rdb_quote(newnick);
    q_oldnick = rdb_quote(oldnick);

#ifdef USE_MYSQL
    /* Change the display on NS_CORE */
    snprintf(buf, sizeof(buf),
             "UPDATE anope_ns_core SET display='%s' WHERE display='%s'",
             q_newnick, q_oldnick);
    db_mysql_query(buf);

    /* Change the display on NS_ALIAS for all grouped nicks */
    snprintf(buf, sizeof(buf),
             "UPDATE anope_ns_alias SET display='%s' WHERE display='%s'",
             q_newnick, q_oldnick);
    db_mysql_query(buf);

    /* Change the display on ChanServ ACCESS list */
    snprintf(buf, sizeof(buf),
             "UPDATE anope_cs_access SET display='%s' WHERE display='%s'",
             q_newnick, q_oldnick);
    db_mysql_query(buf);

    /* Change the display on ChanServ AKICK list */
    snprintf(buf, sizeof(buf),
             "UPDATE anope_cs_akicks SET creator='%s' WHERE creator='%s'",
             q_newnick, q_oldnick);
    db_mysql_query(buf);

    /* Change the display on MemoServ sent memos */
    snprintf(buf, sizeof(buf),
             "UPDATE anope_ms_info SET sender='%s' WHERE sender='%s'",
             q_newnick, q_oldnick);
    db_mysql_query(buf);

    /* Change the display on MemoServ received memos */
    snprintf(buf, sizeof(buf),
             "UPDATE anope_ms_info SET receiver='%s' WHERE receiver='%s'",
             q_newnick, q_oldnick);
    db_mysql_query(buf);

    /* Need to do bwords and akills */

#endif

    free(q_newnick);
    free(q_oldnick);

    return 0;
}

/*************************************************************************/

int rdb_cs_deluser(char *nick)
{
    static char buf[1024];
    char *q_nick;

    q_nick = rdb_quote(nick);

#ifdef USE_MYSQL
    snprintf(buf, sizeof(buf),
             "UPDATE anope_cs_info SET successor='' WHERE successor='%s'",
             q_nick);
    db_mysql_query(buf);

    snprintf(buf, sizeof(buf), "display='%s'", q_nick);
    rdb_scrub_table("anope_cs_access", buf);
    snprintf(buf, sizeof(buf), "creator='%s'", q_nick);
    rdb_scrub_table("anope_cs_akicks", buf);

    free(q_nick);

    return 1;
#endif

    free(q_nick);

    return 0;
}

/*************************************************************************/

int rdb_cs_delchan(ChannelInfo * ci)
{
    static char buf[1024];
    char *q_channel;
    char *q_founder;

    q_channel = rdb_quote(ci->name);
    q_founder = rdb_quote(ci->founder->display);

#ifdef USE_MYSQL
    snprintf(buf, sizeof(buf),
             "UPDATE anope_cs_info SET successor='' WHERE name='%s'",
             q_channel);
    db_mysql_query(buf);

    snprintf(buf, sizeof(buf), "name='%s'", q_channel);
    rdb_scrub_table("anope_cs_info", buf);
    snprintf(buf, sizeof(buf), "receiver='%s' AND serv='CHAN'", q_channel);
    rdb_scrub_table("anope_ms_info", buf);
    snprintf(buf, sizeof(buf), "channel='%s'", q_channel);
    rdb_scrub_table("anope_cs_access", buf);
    rdb_scrub_table("anope_cs_akicks", buf);
    rdb_scrub_table("anope_cs_levels", buf);
    rdb_scrub_table("anope_cs_badwords", buf);
    if (ci->founder) {
        snprintf(buf, sizeof(buf),
                 "update anope_ns_core set channelcount=channelcount-1 where display='%s'",
                 q_founder);
        db_mysql_query(buf);
    }

    free(q_channel);
    free(q_founder);

    return 1;
#endif

    free(q_channel);
    free(q_founder);

    return 0;
}

/*************************************************************************/

int rdb_cs_set_founder(char *channel, char *founder)
{
    static char buf[1024];
    char *q_channel;
    char *q_founder;

    q_channel = rdb_quote(channel);
    q_founder = rdb_quote(founder);

#ifdef USE_MYSQL
    snprintf(buf, sizeof(buf),
             "UPDATE anope_cs_info SET founder='%s', successor='' WHERE name='%s'",
             q_founder, q_channel);
    db_mysql_query(buf);

    snprintf(buf, sizeof(buf),
             "UPDATE anope_ns_core SET channelcount=channelcount+1 WHERE display='%s'",
             q_founder);
    db_mysql_query(buf);

    /* Do i need to scrub the access list for this channel ? */
    snprintf(buf, sizeof(buf), "display='%s' AND channel='%s'", q_founder,
             q_channel);
    rdb_scrub_table("anope_cs_access", buf);

    free(q_channel);
    free(q_founder);

    return 1;
#endif

    free(q_channel);
    free(q_founder);

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
                    SList * ak, SList * sgl, SList * sql, SList * szl)
{

#ifdef USE_MYSQL
    db_mysql_save_os_db(maxusercnt, maxusertime, ak, sgl, sql, szl);
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
        anope_cmd_pong(ServerName, ServerName);
        if (debug)
            alog("debug: RDB Loaded NickServ DataBase (1/8)");
        if (s_HostServ) {
            rdb_load_hs_dbase();
            anope_cmd_pong(ServerName, ServerName);
            if (debug)
                alog("debug: RDB Loaded HostServ DataBase (2/8)");
        }
        if (s_BotServ) {
            rdb_load_bs_dbase();
            anope_cmd_pong(ServerName, ServerName);
            if (debug)
                alog("debug: RDB Loaded BotServ DataBase (3/8)");
        }
        rdb_load_cs_dbase();
        anope_cmd_pong(ServerName, ServerName);
        if (debug)
            alog("debug: RDB Loaded ChanServ DataBase (4/8)");
    }
    rdb_load_os_dbase();
    anope_cmd_pong(ServerName, ServerName);
    if (debug)
        alog("debug: RDB Loaded OperServ DataBase (5/8)");
    rdb_load_news();
    anope_cmd_pong(ServerName, ServerName);
    if (debug)
        alog("debug: RDB Loaded News DataBase (6/8)");
    rdb_load_exceptions();
    anope_cmd_pong(ServerName, ServerName);
    if (debug)
        alog("debug: RDB Loaded Exception Database (7/8)");
    if (PreNickDBName) {
        rdb_load_ns_req_dbase();
        anope_cmd_pong(ServerName, ServerName);
        if (debug)
            alog("debug: RDB Loaded PreNick DataBase (8/8)");
    } else {
        if (debug)
            alog("debug: RDB No need to load PreNickDB (8/8)");
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

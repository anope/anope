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

/* Initialize the current RDB database engine */
int rdb_init()
{

#ifdef USE_MYSQL
    return db_mysql_init();
#endif

    return 0;
}

/*************************************************************************/

/* Check if RDB can be used to load/save data */
int rdb_open()
{

#ifdef USE_MYSQL
    return do_mysql;            /* db_mysql_open(); */
#endif

    return 0;
}

/*************************************************************************/

/* Strictly spoken this should close the database. However, it's not too
 * efficient to close it every time after a write or so, so we just
 * pretend we closed it while in reality it's still open.
 */
int rdb_close()
{

#ifdef USE_MYSQL
    return 1;                   /* db_mysql_close(); */
#endif

    return 1;
}

/*************************************************************************/

/* Quote the string to be used in inclused for the current RDB database */
char *rdb_quote(char *str)
{
#ifdef USE_MYSQL
    return db_mysql_quote(str);
#endif

    return sstrdup(str);
}

/*************************************************************************/

/* Tag a table by setting the 'active' field to 0 for all rows. After an
 * update, all rows with active still 0 will be deleted; this is done to
 * easily delete old entries from the database.
 */
int rdb_tag_table(char *table)
{
    static char buf[1024];

#ifdef USE_MYSQL
    snprintf(buf, sizeof(buf), "UPDATE %s SET active='0'", table);
    return db_mysql_query(buf);
#endif

    return 0;

}

/* Be sure to quote all user input in the clause! */
int rdb_tag_table_where(char *table, char *clause)
{
    static char buf[1024];

#ifdef USE_MYSQL
    snprintf(buf, sizeof(buf), "UPDATE %s SET active='0' WHERE %s", table,
             clause);
    return db_mysql_query(buf);
#endif

    return 0;

}

/*************************************************************************/

/* Empty an entire database table */
int rdb_empty_table(char *table)
{
    static char buf[1024];

#ifdef USE_MYSQL
    snprintf(buf, sizeof(buf), "TRUNCATE TABLE %s", table);
    return db_mysql_query(buf);
#endif

    return 0;

}

/*************************************************************************/

/* Clean up a table with 'dirty' records (active = 0) */
int rdb_clean_table(char *table)
{
    static char buf[1024];

#ifdef USE_MYSQL
    snprintf(buf, sizeof(buf), "DELETE FROM %s WHERE active = 0", table);
    return db_mysql_query(buf);
#endif

    return 0;
}

/* Be sure to quote user input in the clause! */
int rdb_clean_table_where(char *table, char *clause)
{
    static char buf[1024];

#ifdef USE_MYSQL
    snprintf(buf, sizeof(buf), "DELETE FROM %s WHERE active = 0 AND (%s)",
             table, clause);
    return db_mysql_query(buf);
#endif

    return 0;
}

/*************************************************************************/

/* Delete specific records from a table. The clause is MySQL syntax, and
 * should be all quoted up nicely in the calling code.
 */
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

/* Execute a direct MySQL query. Do NOT forget to quote all user input! */
int rdb_direct_query(char *query)
{

#ifdef USE_MYSQL
    alog("Direct Query: %s", query);
    return db_mysql_query(query);
#endif

    return 0;

}

/*************************************************************************/

/* Update the needed tables when someone changes their display.
 * The original author didn't even like this (claimed it should be in
 * mysql.c), and i do agree muchly.
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

    /* Change the akills set on the person's nick */
    snprintf(buf, sizeof(buf),
             "UPDATE anope_cs_akicks SET dmask='%s' WHERE dmask='%s' AND flags & %d",
             q_newnick, q_oldnick, AK_ISNICK);
    db_mysql_query(buf);

    /* Chane the display on NickServ ACCESS list */
    snprintf(buf, sizeof(buf),
             "UPDATE anope_na_access SET display='%s' WHERE display='%s'",
             q_newnick, q_oldnick);
    db_mysql_query(buf);

    /* No need to update anope_cs_info here as it is updated when we
     * save the database.
     *
     * anope_hs_core is per nick, not per display; a changed display
     * won't change anything there
     */

#endif

    free(q_newnick);
    free(q_oldnick);

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
        if (debug)
            alog("debug: RDB Loaded NickServ DataBase (1/8)");
        if (s_HostServ) {
            rdb_load_hs_dbase();
            if (debug)
                alog("debug: RDB Loaded HostServ DataBase (2/8)");
        }
        if (s_BotServ) {
            rdb_load_bs_dbase();
            if (debug)
                alog("debug: RDB Loaded BotServ DataBase (3/8)");
        }
        rdb_load_cs_dbase();
        if (debug)
            alog("debug: RDB Loaded ChanServ DataBase (4/8)");
    }
    rdb_load_os_dbase();
    if (debug)
        alog("debug: RDB Loaded OperServ DataBase (5/8)");
    rdb_load_news();
    if (debug)
        alog("debug: RDB Loaded News DataBase (6/8)");
    rdb_load_exceptions();
    if (debug)
        alog("debug: RDB Loaded Exception Database (7/8)");
    if (PreNickDBName) {
        rdb_load_ns_req_dbase();
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

/* EOF */

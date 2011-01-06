/* RDB functions.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
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
    return db_mysql_open();     /* db_mysql_open(); */
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
#ifdef USE_MYSQL
    return db_mysql_try("UPDATE %s SET active = 0", table);
#endif

    return 0;

}

/* Be sure to quote all user input in the clause! */
int rdb_tag_table_where(char *table, char *clause)
{
#ifdef USE_MYSQL
    return db_mysql_try("UPDATE %s SET active = 0 WHERE %s", table,
                        clause);
#endif

    return 0;

}

/*************************************************************************/

/* Empty an entire database table */
int rdb_empty_table(char *table)
{
#ifdef USE_MYSQL
    return db_mysql_try("TRUNCATE TABLE %s", table);
#endif

    return 0;

}

/*************************************************************************/

/* Clean up a table with 'dirty' records (active = 0) */
int rdb_clean_table(char *table)
{
#ifdef USE_MYSQL
    return db_mysql_try("DELETE FROM %s WHERE active = 0", table);
#endif

    return 0;
}

/* Be sure to quote user input in the clause! */
int rdb_clean_table_where(char *table, char *clause)
{
#ifdef USE_MYSQL
    return db_mysql_try("DELETE FROM %s WHERE active = 0 AND (%s)", table,
                        clause);
#endif

    return 0;
}

/*************************************************************************/

/* Delete specific records from a table. The clause is MySQL syntax, and
 * should be all quoted up nicely in the calling code.
 */
int rdb_scrub_table(char *table, char *clause)
{
#ifdef USE_MYSQL
    return db_mysql_try("DELETE FROM %s WHERE %s", table, clause);
#endif

    return 0;

}

/*************************************************************************/

/* Execute a direct MySQL query. Do NOT forget to quote all user input!
 * NOTE: this ideally shouldn't be used, but that's probably a phase3 utopia
 */
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
    int ret = 0;
    char *q_newnick;
    char *q_oldnick;

    q_newnick = rdb_quote(newnick);
    q_oldnick = rdb_quote(oldnick);

#ifdef USE_MYSQL
    /* Change the display on NS_CORE */
    ret =
        db_mysql_try
        ("UPDATE anope_ns_core SET display = '%s' WHERE display = '%s'",
         q_newnick, q_oldnick);

    /* Change the display on NS_ALIAS for all grouped nicks */
    if (ret)
        ret =
            db_mysql_try
            ("UPDATE anope_ns_alias SET display='%s' WHERE display='%s'",
             q_newnick, q_oldnick);

    /* Change the display on ChanServ ACCESS list */
    if (ret)
        ret =
            db_mysql_try
            ("UPDATE anope_cs_access SET display='%s' WHERE display='%s'",
             q_newnick, q_oldnick);

    /* Change the display on ChanServ AKICK list */
    if (ret)
        ret =
            db_mysql_try
            ("UPDATE anope_cs_akicks SET creator='%s' WHERE creator='%s'",
             q_newnick, q_oldnick);

    /* Change the display on MemoServ sent memos -- is it required? */
    if (ret)
        ret =
            db_mysql_try
            ("UPDATE anope_ms_info SET sender='%s' WHERE sender='%s'",
             q_newnick, q_oldnick);

    /* Change the display on MemoServ received memos -- is it required? */
    if (ret)
        ret =
            db_mysql_try
            ("UPDATE anope_ms_info SET receiver='%s' WHERE receiver='%s'",
             q_newnick, q_oldnick);

    /* Change the akills set on the person's nick */
    if (ret)
        ret =
            db_mysql_try
            ("UPDATE anope_cs_akicks SET dmask='%s' WHERE dmask='%s' AND flags & %d",
             q_newnick, q_oldnick, AK_ISNICK);

    /* Change the display on NickServ ACCESS list */
    if (ret)
        ret =
            db_mysql_try
            ("UPDATE anope_ns_access SET display='%s' WHERE display='%s'",
             q_newnick, q_oldnick);

    /* No need to update anope_cs_info here as it is updated when we
     * save the database.
     *
     * anope_hs_core is per nick, not per display; a changed display
     * won't change anything there
     */

#endif

    free(q_newnick);
    free(q_oldnick);

    return ret;
}

/*************************************************************************/

int rdb_save_ns_core(NickCore * nc)
{

#ifdef USE_MYSQL
    return db_mysql_save_ns_core(nc);
#endif

    return 0;
}

/*************************************************************************/

int rdb_save_ns_alias(NickAlias * na)
{

#ifdef USE_MYSQL
    return db_mysql_save_ns_alias(na);
#endif

    return 0;
}

/*************************************************************************/

int rdb_save_ns_req(NickRequest * nr)
{

#ifdef USE_MYSQL
    return db_mysql_save_ns_req(nr);
#endif

    return 0;
}

/*************************************************************************/

int rdb_save_cs_info(ChannelInfo * ci)
{

#ifdef USE_MYSQL
    return db_mysql_save_cs_info(ci);
#endif

    return 0;
}

/*************************************************************************/

int rdb_save_bs_core(BotInfo * bi)
{

#ifdef USE_MYSQL
    return db_mysql_save_bs_core(bi);
#endif

    return 0;
}

/*************************************************************************/

int rdb_save_hs_core(HostCore * hc)
{

#ifdef USE_MYSQL
    return db_mysql_save_hs_core(hc);
#endif

    return 0;
}

/*************************************************************************/

int rdb_save_os_db(unsigned int maxucnt, unsigned int maxutime,
                   SList * ak, SList * sgl, SList * sql, SList * szl)
{

#ifdef USE_MYSQL
    return db_mysql_save_os_db(maxusercnt, maxusertime, ak, sgl, sql, szl);
#endif

    return 0;
}

/*************************************************************************/

int rdb_save_news(NewsItem * ni)
{

#ifdef USE_MYSQL
    return db_mysql_save_news(ni);
#endif

    return 0;
}

/*************************************************************************/

int rdb_load_bs_dbase(void)
{

#ifdef USE_MYSQL
    return db_mysql_load_bs_dbase();
#endif

    return 0;
}

/*************************************************************************/

int rdb_load_hs_dbase(void)
{

#ifdef USE_MYSQL
    return db_mysql_load_hs_dbase();
#endif

    return 0;
}

/*************************************************************************/

int rdb_load_ns_dbase(void)
{

#ifdef USE_MYSQL
    return db_mysql_load_ns_dbase();
#endif

    return 0;
}

/*************************************************************************/

int rdb_load_news(void)
{

#ifdef USE_MYSQL
    return db_mysql_load_news();
#endif

    return 0;
}

/*************************************************************************/

int rdb_load_exceptions(void)
{

#ifdef USE_MYSQL
    return db_mysql_load_exceptions();
#endif

    return 0;
}

/*************************************************************************/

int rdb_load_cs_dbase(void)
{

#ifdef USE_MYSQL
    return db_mysql_load_cs_dbase();
#endif

    return 0;
}

/*************************************************************************/

int rdb_load_os_dbase(void)
{

#ifdef USE_MYSQL
    return db_mysql_load_os_dbase();
#endif

    return 0;
}

/*************************************************************************/

int rdb_load_ns_req_dbase(void)
{

#ifdef USE_MYSQL
    return db_mysql_load_ns_req_dbase();
#endif

    return 0;
}

/*************************************************************************/

#define LOAD_DBASE(num, name, func) {\
	if (!func) {\
		alog("RDB unable to load %s database (%d/8) !!!", name, num);\
		return 0;\
	}\
	if (debug)\
		alog("debug: RDB Loaded %s DataBase (%d/8)", name, num);\
}

int rdb_load_dbases(void)
{
    if (!skeleton) {
        LOAD_DBASE(1, "NickServ", rdb_load_ns_dbase());

        if (s_HostServ) {
            LOAD_DBASE(2, "HostServ", rdb_load_hs_dbase());
        }

        if (s_BotServ) {
            LOAD_DBASE(3, "BotServ", rdb_load_bs_dbase());
        }

        LOAD_DBASE(4, "ChanServ", rdb_load_cs_dbase());
    }

    LOAD_DBASE(5, "OperServ", rdb_load_os_dbase());
    LOAD_DBASE(6, "News", rdb_load_news());
    LOAD_DBASE(7, "Exception", rdb_load_exceptions());

    if (PreNickDBName) {
        LOAD_DBASE(8, "PreNick", rdb_load_ns_req_dbase());
    } else if (debug) {
        alog("debug: RDB No need to load PreNickDB (8/8)");
    }

    alog("RDB: All DataBases loaded.");

    return 0;
}

/*************************************************************************/

int rdb_save_exceptions(Exception * e)
{

#ifdef USE_MYSQL
    return db_mysql_save_exceptions(e);
#endif

    return 0;
}

/* EOF */

/* MySQL functions.
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

#ifdef USE_MYSQL

MYSQL *mysql;
MYSQL_RES *mysql_res;
MYSQL_ROW mysql_row;
int do_mysql = 0;

void db_mysql_init(void)
{
    if (!MysqlPort) MysqlPort = 3306;

    if (!MysqlHost && !MysqlSock) {
        alog("MySQL is disabled.");
        do_mysql = 0;
        return;
    }
    if (!MysqlUser || !MysqlPass) {
        alog("Missing required config directives for MySQL.");
        do_mysql = 0;
        return;
    }

    mysql = mysql_init(NULL);
    if ((!mysql_real_connect(mysql, MysqlHost, MysqlUser, MysqlPass, MysqlName, MysqlPort, MysqlSock ? MysqlSock : NULL, 0))) {
        alog("Unable to connect to MySQL database:");
        alog(mysql_error(mysql));
        do_mysql = 0;
        return;
    }
    
    alog("MySQL enabled.");
    do_mysql = 1;
}

void db_mysql_end(void)
{
    if (do_mysql) {
        mysql_close(mysql);
        alog("MySQL connection closed.");
    }
}

char *db_mysql_quote(char *sql)
{
    char *quoted;
    int len;
    
    if (!do_mysql || !sql) return sstrdup("");
    
    len = strlen(sql);
    quoted = malloc((2*len + 1) * sizeof(char));
    mysql_real_escape_string(mysql, quoted, sql, len);
    return quoted;
}

int db_mysql_query(char *fmt, ...)
{
    va_list args;
    char *s, *sql;
    int result;
    
    if (!do_mysql) return MYSQL_ERR;
    
    va_start(args, fmt);
    
    sql = malloc(MAX_SQL_BUF);
    vsnprintf(sql, MAX_SQL_BUF, fmt, args);
    
    if (debug) {
        s = db_mysql_quote(sql);
        alog(s);
        free(s);
    }
    
    result = mysql_query(mysql, sql);
    if (result) {
        log_perror(mysql_error(mysql));
        return MYSQL_ERR;
    }
    va_end(args);
    return MYSQL_OK;
}

void db_mysql_store_result(void)
{
    mysql_res = mysql_store_result(mysql);
}

void db_mysql_free_res(void)
{
    mysql_free_result(mysql_res);
}

/* BotServ */
void db_mysql_bs_add_bot(BotInfo *bi)
{
    char *nick, *user, *host, *rname;
    
    nick = db_mysql_quote(bi->nick);
    user = db_mysql_quote(bi->user);
    host = db_mysql_quote(bi->host);
    rname = db_mysql_quote(bi->real);
    
    db_mysql_query("INSERT INTO `anope_bs_core` (`nick`,`user`,`host`,`rname`,`flags`,`created`,`chancount`) VALUES('%s','%s','%s','%s',%d,%d,%d)",nick,user,host,rname,bi->flags, bi->created, bi->chancount);
    
    free(nick);
    free(user);
    free(host);
    free(rname);
}

void db_mysql_bs_del_bot(BotInfo *bi)
{
    char *nick, *user, *host, *rname;
    
    nick = db_mysql_quote(bi->nick);
    user = db_mysql_quote(bi->user);
    host = db_mysql_quote(bi->host);
    rname = db_mysql_quote(bi->real);
    
    db_mysql_query("DELETE FROM `anope_bs_core` WHERE `nick`='%s' AND `user`='%s' AND `host`='%s' AND `rname`='%s'",nick,user,host,rname);
    
    free(nick);
    free(user);
    free(host);
    free(rname);    
}

void db_mysql_bs_chg_bot(BotInfo *bi, char *newnick, char *newuser, char *newhost, char *newreal)
{
    char *oldnick, *nick, *user, *host, *real, *sql;

    sql = malloc(MAX_SQL_BUF);
    snprintf(sql, MAX_SQL_BUF, "UPDATE `anope_bs_core` SET ");
    
    oldnick = db_mysql_quote(bi->nick);
    
    if (!newnick) newnick = bi->nick;
    if (!newuser) newuser = bi->user;
    if (!newhost) newhost = bi->host;
    if (!newreal) newreal = bi->real;
    
    nick = db_mysql_quote(newnick);
    strcat(sql,"`nick`='");
    strcat(sql,nick);
    strcat(sql,"',");
    free(nick);

    user = db_mysql_quote(newuser);
    strcat(sql,"`user`='");
    strcat(sql,user);
    strcat(sql,"',");
    free(user);

    host = db_mysql_quote(newhost);
    strcat(sql,"`host`='");
    strcat(sql,host);
    strcat(sql,"',");
    free(host);

    real = db_mysql_quote(newreal);
    strcat(sql,"`rname`='");
    strcat(sql,real);
    strcat(sql,"' ");
    free(real);
    
    strcat(sql,"WHERE `nick`='");
    strcat(sql,oldnick);
    strcat(sql,"'");
    db_mysql_query(sql);
    free(sql);
    free(oldnick);
}

void db_mysql_bs_chg_bot_chancount(BotInfo *bi)
{
    char *nick;
    
    nick = db_mysql_quote(bi->nick);
    db_mysql_query("UPDATE `anope_bs_core` SET `chancount` = %d WHERE `nick` = '%s'", bi->chancount, nick);
    free(nick);
}

void db_mysql_bs_chg_bot_flags(BotInfo *bi)
{
    char *nick;
    
    nick = db_mysql_quote(bi->nick);
    db_mysql_query("UPDATE `anope_bs_core` SET `flags` = %d WHERE `nick` = '%s'", bi->flags, nick);
    free(nick);
}

/* ChanServ */
void db_mysql_cs_add_access(ChannelInfo *ci, ChanAccess *access)
{
    char *display, *channel;

    display = db_mysql_quote(access->nc->display);
    channel = db_mysql_quote(ci->name);
    
    /* Check if the entry already exists */
    db_mysql_query("SELECT `ca_id` FROM `anope_cs_access` WHERE `channel` = '%s' AND `display` = '%s'",channel, display);
    db_mysql_store_result();
    
    if (mysql_num_rows(mysql_res) >= 1) {
        db_mysql_query("UPDATE `anope_cs_access` SET `in_use` = 1, `level` = %d, `last_seen` = %d WHERE `channel` = '%s' AND `display` = '%s'",access->level, access->last_seen, channel, display);
    } else {
        db_mysql_query("INSERT INTO `anope_cs_access` (`in_use`,`level`,`display`,`channel`,`last_seen`) VALUES (1, %d, '%s', '%s', %d)",access->level,display,channel,access->last_seen);
    }
    
    db_mysql_free_res();
    free(channel);
    free(display);
}

void db_mysql_cs_del_access(ChannelInfo *ci, ChanAccess *access)
{
    char *display, *channel;

    display = db_mysql_quote(access->nc->display);
    channel = db_mysql_quote(ci->name);
    
    db_mysql_query("DELETE FROM `anope_cs_access` WHERE `channel` = '%s' AND `display` = '%s'", channel, display);
    
    free(channel);
    free(display);
}
#endif /* USE_MYSQL */

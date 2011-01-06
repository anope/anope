
/* MySQL functions.
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

/* Database Global Variables */
MYSQL *mysql;                   /* MySQL Handler */
MYSQL_RES *mysql_res;           /* MySQL Result  */
MYSQL_FIELD *mysql_fields;      /* MySQL Fields  */
MYSQL_ROW mysql_row;            /* MySQL Row     */

int mysql_is_connected = 0;     /* Are we currently connected? */

/*************************************************************************/

/* Throw a mysql error into the logs. If severity is MYSQL_ERROR, we
 * also exit Anope...
 */
void db_mysql_error(int severity, char *msg)
{
    static char buf[512];

    if (mysql_error(mysql)) {
        snprintf(buf, sizeof(buf), "MySQL %s %s: %s", msg,
                 severity == MYSQL_WARNING ? "warning" : "error",
                 mysql_error(mysql));
    } else {
        snprintf(buf, sizeof(buf), "MySQL %s %s", msg,
                 severity == MYSQL_WARNING ? "warning" : "error");
    }

    log_perror(buf);

    if (severity == MYSQL_ERROR) {
        log_perror("MySQL FATAL error... aborting.");
        exit(0);
    }

}

/*************************************************************************/

/* Initialize the MySQL code */
int db_mysql_init()
{

    /* If the host is not defined, assume we don't want MySQL */
    if (!MysqlHost) {
        do_mysql = 0;
        alog("MySQL: has been disabled.");
        return 0;
    } else {
        do_mysql = 1;
        alog("MySQL: has been enabled.");
        alog("MySQL: client version %s.", mysql_get_client_info());
    }

    /* The following configuration options are required.
     * If missing disable MySQL to avoid any problems.
     */

    if ((do_mysql) && (!MysqlName || !MysqlUser)) {
        do_mysql = 0;
        alog("MySQL Error: Set all required configuration options.");
        return 0;
    }

    if (!db_mysql_open()) {
        do_mysql = 0;
        return 0;
    }

    return 1;
}

/*************************************************************************/

/* Open a connection to the mysql database. Return 0 on failure, or
 * 1 on success. If this succeeds, we're guaranteed of a working
 * mysql connection (unless something unexpected happens ofcourse...)
 */
int db_mysql_open()
{
    /* If MySQL is disabled, return 0 */
    if (!do_mysql)
        return 0;

    /* If we are reported to be connected, ping MySQL to see if we really are
     * still connected. (yes mysql_ping() returns 0 on success)
     */
    if (mysql_is_connected && !mysql_ping(mysql))
        return 1;

    mysql_is_connected = 0;

    mysql = mysql_init(NULL);
    if (mysql == NULL) {
        db_mysql_error(MYSQL_WARNING, "Unable to create mysql object");
        return 0;
    }

    if (!MysqlPort)
        MysqlPort = MYSQL_DEFAULT_PORT;

    if (!mysql_real_connect(mysql, MysqlHost, MysqlUser, MysqlPass, MysqlName, MysqlPort, MysqlSock, 0)) {
        log_perror("MySQL Error: Cant connect to MySQL: %s\n", mysql_error(mysql));
        return 0;
    }

    mysql_is_connected = 1;

    return 1;

}


/*************************************************************************/

/* Perform a MySQL query. Return 1 if the query succeeded and 0 if the
 * query failed. Before returning failure, re-try the query a few times
 * and die if it still fails.
 */
int db_mysql_query(char *sql)
{
    int lcv;

    if (!do_mysql)
        return 0;

    if (debug)
        alog("debug: MySQL: %s", sql);

    /* Try as many times as configured in MysqlRetries */
    for (lcv = 0; lcv < MysqlRetries; lcv++) {
        if (db_mysql_open() && (mysql_query(mysql, sql) == 0))
            return 1;

        /* If we get here, we could not run the query */
        log_perror("Unable to run query: %s\n", mysql_error(mysql));

        /* Wait for MysqlRetryGap seconds and try again */
        sleep(MysqlRetryGap);
    }

    /* Unable to run the query even after MysqlRetries tries */
    db_mysql_error(MYSQL_WARNING, "query");

    return 0;

}

/*************************************************************************/

/* Quote a string to be safely included in a query. The result of this
 * function is allocated; it MUST be freed by the caller.
 */
char *db_mysql_quote(char *sql)
{
    int slen;
    char *quoted;


    if (!sql)
        return sstrdup("");

    slen = strlen(sql);
    quoted = malloc((1 + (slen * 2)) * sizeof(char));

    mysql_real_escape_string(mysql, quoted, sql, slen);

    return quoted;

}

/**
 * Quote a buffer to be safely included in a query.
 * The result is allocated and needs to be freed by caller.
 **/
char *db_mysql_quote_buffer(char *sql, int size)
{
    char *ret;
    ret = scalloc((1 + 2 * size), sizeof(char));

    mysql_real_escape_string(mysql, ret, sql, size);

    return ret;
}

/*************************************************************************/

/* Close the MySQL database connection. */
int db_mysql_close()
{
    mysql_close(mysql);

    mysql_is_connected = 0;

    return 1;
}

/*************************************************************************/

/* Try to execute a query and issue a warning when failed. Return 1 on
 * success and 0 on failure.
 */
int db_mysql_try(const char *fmt, ...)
{
    va_list args;
    static char sql[MAX_SQL_BUF];

    va_start(args, fmt);
    vsnprintf(sql, MAX_SQL_BUF, fmt, args);
    va_end(args);

    if (!db_mysql_query(sql)) {
        log_perror("Can't create sql query: %s", sql);
        db_mysql_error(MYSQL_WARNING, "query");
        return 0;
    }

    return 1;
}

/*************************************************************************/

/**
 * Returns a string (buffer) to insert into a SQL query.
 * The string will, once evaluated by MySQL, result in the given pass
 * encoded in the encryption type selected for MysqlSecure
 * The result of this function is an allocated MySQL safe string.
 *
 * @param pass The buffer containing the password to secure.
 * @param size The size of the password-to-secure buffer.
 * @return Returns a MySQL safe string containing the password, hash or SQL instruction to encrypt the password. Needs to be free'd.
 **/
char *db_mysql_secure(char *pass, int size)
{
    char tmp_pass[PASSMAX];
    char *str, *tmp;
    unsigned bufsize = (2 * PASSMAX + 15);

    if (MysqlSecure)
    	bufsize += strlen(MysqlSecure);

    /* Initialize the buffer. Bug #86 */
    memset(tmp_pass, 0, PASSMAX);

    /* Return all zeros if no pass is set. */
    if (!pass)
        return NULL;

    str = scalloc(bufsize, sizeof(char));
    if (enc_decrypt(pass, tmp_pass, PASSMAX - 1) != 1) {
        /* We couldnt decrypt the pass... */
        /* Make sure the hash is MySQL safe.. */
        tmp = db_mysql_quote_buffer(pass, size);
        snprintf(str, bufsize, "'%s'", tmp);
        free(tmp);
    } else {                    /* if we could decrypt the pass */
        /* Make sure the pass itself pass is MySQL safe.. */
        tmp = db_mysql_quote_buffer(tmp_pass, strlen(tmp_pass));
        if ((!MysqlSecure) || (strcmp(MysqlSecure, "") == 0)) {
            snprintf(str, bufsize, "'%s'", tmp);
        } else if (strcmp(MysqlSecure, "des") == 0) {
            snprintf(str, bufsize, "ENCRYPT('%s')", tmp);
        } else if (strcmp(MysqlSecure, "md5") == 0) {
            snprintf(str, bufsize, "MD5('%s')", tmp);
        } else if (strcmp(MysqlSecure, "sha") == 0) {
            snprintf(str, bufsize, "SHA('%s')", tmp);
        } else {
            snprintf(str, bufsize, "ENCODE('%s','%s')", tmp, MysqlSecure);
        }
        free(tmp);
    }

    return str;
}

/*************************************************************************/

/*
 * NickServ Specific Secion
 */

/*************************************************************************/

/* Save the given NickRequest into the database
 * Return 1 on success, 0 on failure
 * These tables are tagged and will be cleaned:
 * - anope_ns_request
 */
int db_mysql_save_ns_req(NickRequest * nr)
{
    int ret;
    char *q_nick, *q_passcode, *q_password, *q_email;

    q_nick = db_mysql_quote(nr->nick);
    q_passcode = db_mysql_quote(nr->passcode);
    q_password = db_mysql_quote_buffer(nr->password, PASSMAX);
    q_email = db_mysql_quote(nr->email);

    ret = db_mysql_try("UPDATE anope_ns_request "
                       "SET passcode = '%s', password = '%s', email = '%s', requested = %d, active = 1 "
                       "WHERE nick = '%s'",
                       q_passcode, q_password, q_email, (int) nr->requested,
                       q_nick);

    if (ret && (mysql_affected_rows(mysql) == 0)) {
        ret = db_mysql_try("INSERT DELAYED INTO anope_ns_request "
                           "(nick, passcode, password, email, requested, active) "
                           "VALUES ('%s', '%s', '%s', '%s', %d, 1)",
                           q_nick, q_passcode, q_password, q_email,
                           (int) nr->requested);
    }

    free(q_nick);
    free(q_passcode);
    free(q_password);
    free(q_email);

    return ret;
}

/*************************************************************************/

/* Save the given NickCore into the database
 * Also save the access list and memo's for this user
 * Return 1 on success, 0 on failure
 * These tables are tagged and will be cleaned:
 * - anope_ns_core
 * - anope_ns_alias
 * - anope_ns_access
 * - anope_ms_info (serv='NICK')
 */
int db_mysql_save_ns_core(NickCore * nc)
{
    int ret;
    int i;
    char *q_display, *q_pass, *q_email, *q_greet, *q_url,
        *q_access, *q_sender, *q_text;

    q_display = db_mysql_quote(nc->display);
    q_email = db_mysql_quote(nc->email);
    q_greet = db_mysql_quote(nc->greet);
    q_url = db_mysql_quote(nc->url);

    /* First secure the pass, then make it MySQL safe.. - Viper */
    q_pass = db_mysql_secure(nc->pass, PASSMAX);
    if (!q_pass)
        fatal("Unable to encrypt password for MySQL");

    /* Let's take care of the core itself */
    /* Update the existing records */
    ret = db_mysql_try("UPDATE anope_ns_core "
                "SET pass = %s, email = '%s', greet = '%s', icq = %d, url = '%s', flags = %d, language = %d, "
                "accesscount = %d, memocount = %d, memomax = %d, channelcount = %d, channelmax = %d, active = 1 "
                "WHERE display = '%s'",
                q_pass, q_email, q_greet, nc->icq, q_url, nc->flags,
                nc->language, nc->accesscount, nc->memos.memocount,
                nc->memos.memomax, nc->channelcount, nc->channelmax,
                q_display);

    /* Our previous UPDATE affected no rows, therefore this is a new record */
    if (ret && (mysql_affected_rows(mysql) == 0))
        ret = db_mysql_try("INSERT DELAYED INTO anope_ns_core "
                    "(display, pass, email, greet, icq, url, flags, language, accesscount, memocount, memomax, channelcount, channelmax, active) "
                    "VALUES ('%s', %s, '%s', '%s', %d, '%s', %d, %d, %d, %d, %d, %d, %d, 1)",
                    q_display, q_pass, q_email, q_greet, nc->icq, q_url,
                    nc->flags, nc->language, nc->accesscount,
                    nc->memos.memocount, nc->memos.memomax,
                    nc->channelcount, nc->channelmax);

    /* Now let's do the access */
    for (i = 0; ret && (i < nc->accesscount); i++) {
        q_access = db_mysql_quote(nc->access[i]);
/**
        ret = db_mysql_try("UPDATE anope_ns_access "
                           "SET access = '%s' "
                           "WHERE display = '%s'",
                           q_access, q_display);

        if (ret && (mysql_affected_rows(mysql) == 0)) {**/

            ret = db_mysql_try("INSERT DELAYED INTO anope_ns_access "
                               "(display, access) "
                               "VALUES ('%s','%s')",
                               q_display, q_access);
/*        } */

        free(q_access);
    }

    /* Memos */
    for (i = 0; ret && (i < nc->memos.memocount); i++) {
        q_sender = db_mysql_quote(nc->memos.memos[i].sender);
        q_text = db_mysql_quote(nc->memos.memos[i].text);

        if (nc->memos.memos[i].id != 0)
        {
             ret = db_mysql_try("UPDATE anope_ms_info "
                           "SET receiver = '%s', number = %d, flags = %d, time = %d, sender = '%s', text = '%s', active = 1 "
                           "WHERE  nm_id = %d AND serv = 'NICK'",
                           q_display, nc->memos.memos[i].number,
                           nc->memos.memos[i].flags,
                           (int) nc->memos.memos[i].time, q_sender, q_text,
                           nc->memos.memos[i].id);
        }
        if (nc->memos.memos[i].id == 0 || (ret && (mysql_affected_rows(mysql) == 0))) {
            ret = db_mysql_try("INSERT INTO anope_ms_info "
                               "(receiver, number, flags, time, sender, text, serv, active) "
                               "VALUES ('%s', %d, %d, %d, '%s', '%s', 'NICK', 1)",
                               q_display, nc->memos.memos[i].number,
                               nc->memos.memos[i].flags,
                               (int) nc->memos.memos[i].time, q_sender,
                               q_text);

            /* This is to make sure we can UPDATE memos instead of TRUNCATE
             * the table each time and then INSERT them all again. Ideally
             * everything in core would have it's dbase-id stored, but that's
             * something for phase 3. -GD
             */
            if (ret)
                nc->memos.memos[i].id = mysql_insert_id(mysql);
        }

        free(q_sender);
        free(q_text);
    }

    free(q_display);
    free(q_pass);
    free(q_email);
    free(q_greet);
    free(q_url);

    return ret;
}


/*************************************************************************/

/* Save the given NickAlias into the database
 * Return 1 on success, 0 on failure
 * These tables are tagged and will be cleaned:
 * - anope_ns_core
 * - anope_ns_alias
 * - anope_ns_access
 * - anope_ms_info (serv='NICK')
 */
int db_mysql_save_ns_alias(NickAlias * na)
{
    int ret;
    char *q_nick, *q_lastmask, *q_lastrname, *q_lastquit, *q_display;

    q_nick = db_mysql_quote(na->nick);
    q_lastmask = db_mysql_quote(na->last_usermask);
    q_lastrname = db_mysql_quote(na->last_realname);
    q_lastquit = db_mysql_quote(na->last_quit);
    q_display = db_mysql_quote(na->nc->display);

    ret = db_mysql_try("UPDATE anope_ns_alias "
                       "SET last_usermask = '%s', last_realname = '%s', last_quit = '%s', time_registered = %d, last_seen = %d, status = %d, "
                       "    display = '%s', active = 1 "
                       "WHERE nick = '%s'",
                       q_lastmask, q_lastrname, q_lastquit,
                       (int) na->time_registered, (int) na->last_seen,
                       (int) na->status, q_display, q_nick);

    /* Our previous UPDATE affected no rows, therefore this is a new record */
    if (ret && (mysql_affected_rows(mysql) == 0)) {
        ret = db_mysql_try("INSERT DELAYED INTO anope_ns_alias "
                           "(nick, last_usermask, last_realname, last_quit, time_registered, last_seen, status, display, active) "
                           "VALUES ('%s', '%s', '%s', '%s', %d, %d, %d, '%s', 1)",
                           q_nick, q_lastmask, q_lastrname, q_lastquit,
                           (int) na->time_registered, (int) na->last_seen,
                           (int) na->status, q_display);
    }

    free(q_nick);
    free(q_lastmask);
    free(q_lastrname);
    free(q_lastquit);
    free(q_display);

    return ret;
}

/*************************************************************************/

/*
 * ChanServ Specific Secion
 */

/*************************************************************************/

/* Save the given ChannelInfo into the database
 * Also save the access list, levels, akicks, badwords, ttb, and memo's for this channel
 * Return 1 on success, 0 on failure
 * These tables are tagged and will be cleaned:
 * - anope_cs_info
 * - anope_cs_access
 * - anope_cs_levels
 * - anope_cs_akicks
 * - anope_cs_badwords
 * - anope_cs_ttb
 * - anope_ms_info (serv='CHAN')
 */
int db_mysql_save_cs_info(ChannelInfo * ci)
{
    int ret, i;
    char *q_name;
    char *q_founder;
    char *q_successor;
    char *q_pass;
    char *q_desc;
    char *q_url;
    char *q_email;
    char *q_lasttopic;
    char *q_lasttopicsetter;
    char *q_forbidby;
    char *q_forbidreason;
    char *q_mlock_key;
    char *q_mlock_flood;
    char *q_mlock_redirect;
    char *q_entrymsg;
    char *q_botnick;
    char *q_sender;
    char *q_text;
    char *q_accessdisp;
    char *q_akickdisp;
    char *q_akickreason;
    char *q_akickcreator;
    char *q_badwords;

    q_name = db_mysql_quote(ci->name);
    if (ci->founder) {
        q_founder = db_mysql_quote(ci->founder->display);
    } else {
        q_founder = db_mysql_quote("");
    }
    if (ci->successor) {
        q_successor = db_mysql_quote(ci->successor->display);
    } else {
        q_successor = db_mysql_quote("");
    }
    q_desc = db_mysql_quote(ci->desc);
    q_url = db_mysql_quote(ci->url);
    q_email = db_mysql_quote(ci->email);
    q_lasttopic = db_mysql_quote(ci->last_topic);
    q_lasttopicsetter = db_mysql_quote(ci->last_topic_setter);
    q_forbidby = db_mysql_quote(ci->forbidby);
    q_forbidreason = db_mysql_quote(ci->forbidreason);
    q_mlock_key = db_mysql_quote(ci->mlock_key);
    q_mlock_flood = db_mysql_quote(ci->mlock_flood);
    q_mlock_redirect = db_mysql_quote(ci->mlock_redirect);
    q_entrymsg = db_mysql_quote(ci->entry_message);
    if (ci->bi) {
        q_botnick = db_mysql_quote(ci->bi->nick);
    } else {
        q_botnick = db_mysql_quote("");
    }

    /* First secure the pass, then make it MySQL safe.. - Viper */
    q_pass = db_mysql_secure(ci->founderpass, PASSMAX);
    if (!q_pass)
        fatal("Unable to encrypt password for MySQL");

    /* Let's take care of the core itself */
    ret = db_mysql_try("UPDATE anope_cs_info "
                "SET founder = '%s', successor = '%s', founderpass = %s, descr = '%s', url = '%s', email = '%s', time_registered = %d, "
                "    last_used = %d, last_topic = '%s', last_topic_setter = '%s', last_topic_time = %d, flags = %d, forbidby = '%s', "
                "    forbidreason = '%s', bantype = %d, accesscount = %d, akickcount = %d, mlock_on = %d,  mlock_off = %d, mlock_limit = %d, "
                "    mlock_key = '%s', mlock_flood = '%s', mlock_redirect = '%s', entry_message = '%s', memomax = %d, botnick = '%s', botflags = %d, "
                "    bwcount = %d, capsmin = %d, capspercent = %d, floodlines = %d, floodsecs = %d, repeattimes = %d, active = 1 "
                "WHERE name = '%s'",
                q_founder, q_successor, q_pass, q_desc, q_url, q_email,
                (int) ci->time_registered, (int) ci->last_used,
                q_lasttopic, q_lasttopicsetter,
                (int) ci->last_topic_time, (int) ci->flags, q_forbidby,
                q_forbidreason, (int) ci->bantype,
                (int) ci->accesscount, (int) ci->akickcount,
                (int) ci->mlock_on, (int) ci->mlock_off,
                (int) ci->mlock_limit, q_mlock_key, q_mlock_flood,
                q_mlock_redirect, q_entrymsg, (int) ci->memos.memomax,
                q_botnick, (int) ci->botflags, (int) ci->bwcount,
                (int) ci->capsmin, (int) ci->capspercent,
                (int) ci->floodlines, (int) ci->floodsecs,
                (int) ci->repeattimes, q_name);

    /* Our previous UPDATE affected no rows, therefore this is a new record */
    if (ret && (mysql_affected_rows(mysql) == 0))
        ret = db_mysql_try("INSERT DELAYED INTO anope_cs_info "
                    "(name, founder, successor, founderpass, descr, url, email, time_registered, last_used,  last_topic, last_topic_setter, "
                    "    last_topic_time, flags, forbidby, forbidreason, bantype, accesscount, akickcount, mlock_on, mlock_off, mlock_limit, "
                    "    mlock_key, mlock_flood, mlock_redirect, entry_message, botnick, botflags, bwcount, capsmin, capspercent, floodlines, "
                    "    floodsecs, repeattimes, active) "
                    "VALUES ('%s', '%s', '%s', %s, '%s', '%s', '%s', %d, %d, '%s', '%s', %d, %d, '%s', '%s', %d, %d, %d, %d, %d, %d, '%s', '%s', "
                    "        '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, %d, 1)",
                    q_name, q_founder, q_successor, q_pass, q_desc,
                    q_url, q_email, (int) ci->time_registered,
                    (int) ci->last_used, q_lasttopic,
                    q_lasttopicsetter, (int) ci->last_topic_time,
                    (int) ci->flags, q_forbidby, q_forbidreason,
                    (int) ci->bantype, (int) ci->accesscount,
                    (int) ci->akickcount, (int) ci->mlock_on,
                    (int) ci->mlock_off, (int) ci->mlock_limit,
                    q_mlock_key, q_mlock_flood, q_mlock_redirect,
                    q_entrymsg, q_botnick, (int) ci->botflags,
                    (int) ci->bwcount, (int) ci->capsmin,
                    (int) ci->capspercent, (int) ci->floodlines,
                    (int) ci->floodsecs, (int) ci->repeattimes);

    /* Memos */
    for (i = 0; ret && (i < ci->memos.memocount); i++) {
        q_sender = db_mysql_quote(ci->memos.memos[i].sender);
        q_text = db_mysql_quote(ci->memos.memos[i].text);

        if (ci->memos.memos[i].id != 0)
        {
            ret = db_mysql_try("UPDATE anope_ms_info "
                           "SET receiver = '%s', number = %d, flags = %d, time = %d, sender = '%s', text = '%s', active = 1 "
                           "WHERE nm_id = %d AND serv = 'CHAN'",
                           q_name, ci->memos.memos[i].number,
                           ci->memos.memos[i].flags,
                           (int) ci->memos.memos[i].time, q_sender, q_text,
                           ci->memos.memos[i].id);
        }
        if (ci->memos.memos[i].id == 0 || (ret && (mysql_affected_rows(mysql) == 0))) {
            ret = db_mysql_try("INSERT INTO anope_ms_info "
                               "(receiver, number,flags, time, sender, text, serv, active) "
                               "VALUES ('%s', %d, %d, %d, '%s', '%s', 'CHAN', 1)",
                               q_name, ci->memos.memos[i].number,
                               ci->memos.memos[i].flags,
                               (int) ci->memos.memos[i].time, q_sender,
                               q_text);

            /* See comment at db_mysql_save_ns_core */
            if (ret)
            	ci->memos.memos[i].id = mysql_insert_id(mysql);
        }

        free(q_sender);
        free(q_text);
    }

    /* Access */
    for (i = 0; ret && (i < ci->accesscount); i++) {
        if (ci->access[i].in_use) {
            q_accessdisp = db_mysql_quote(ci->access[i].nc->display);

            ret = db_mysql_try("UPDATE anope_cs_access "
                               "SET in_use = %d, level = %d, last_seen = %d, active = 1 "
                               "WHERE channel = '%s' AND display = '%s'",
                               (int) ci->access[i].in_use,
                               (int) ci->access[i].level,
                               (int) ci->access[i].last_seen,
                               q_name, q_accessdisp);

            if (ret && (mysql_affected_rows(mysql) == 0)) {
                ret = db_mysql_try("INSERT DELAYED INTO anope_cs_access "
                                   "(channel, display, in_use, level, last_seen, active) "
                                   "VALUES ('%s', '%s', %d, %d, %d, 1)",
                                   q_name, q_accessdisp,
                                   (int) ci->access[i].in_use,
                                   (int) ci->access[i].level,
                                   (int) ci->access[i].last_seen);
            }

            free(q_accessdisp);
        }
    }

    /* Levels */
    for (i = 0; ret && (i < CA_SIZE); i++) {
        ret = db_mysql_try("UPDATE anope_cs_levels "
                           "SET level = %d, active = 1 "
                           "WHERE channel = '%s' AND position = %d",
                           (int) ci->levels[i], q_name, i);

        if (ret && (mysql_affected_rows(mysql) == 0)) {
            ret = db_mysql_try("INSERT DELAYED INTO anope_cs_levels "
                               "(channel, position, level, active) "
                               "VALUES ('%s', %d, %d, 1)",
                               q_name, i, (int) ci->levels[i]);
        }
    }

    /* Akicks */
    for (i = 0; ret && (i < ci->akickcount); i++) {
        if (ci->akick[i].flags & AK_USED) {
            if (ci->akick[i].flags & AK_ISNICK)
                q_akickdisp = db_mysql_quote(ci->akick[i].u.nc->display);
            else
                q_akickdisp = db_mysql_quote(ci->akick[i].u.mask);

            q_akickreason = db_mysql_quote(ci->akick[i].reason);
            q_akickcreator = db_mysql_quote(ci->akick[i].creator);
        } else {
            q_akickdisp = "";
            q_akickreason = "";
            q_akickcreator = "";
        }

        ret = db_mysql_try("UPDATE anope_cs_akicks "
                           "SET flags = %d, reason = '%s', creator = '%s', addtime = %d, active = 1 "
                           "WHERE channel = '%s' AND dmask = '%s'",
                           (int) ci->akick[i].flags, q_akickreason,
                           q_akickcreator, (ci->akick[i].flags & AK_USED ?
                                             (int) ci->akick[i].addtime : 0),
                           q_name, q_akickdisp);

        if (ret && (mysql_affected_rows(mysql) == 0)) {
            ret = db_mysql_try("INSERT DELAYED INTO anope_cs_akicks "
                               "(channel, dmask, flags, reason, creator, addtime, active) "
                               "VALUES ('%s', '%s', %d, '%s', '%s', %d, 1)",
                               q_name, q_akickdisp, (int) ci->akick[i].flags,
                               q_akickreason, q_akickcreator,
                               (ci->akick[i].flags & AK_USED ?
                                 (int) ci->akick[i].addtime : 0));
        }

        if (ci->akick[i].flags & AK_USED) {
            free(q_akickdisp);
            free(q_akickreason);
            free(q_akickcreator);
        }
    }

    /* Bad Words */
    for (i = 0; ret && (i < ci->bwcount); i++) {
        if (ci->badwords[i].in_use) {
            q_badwords = db_mysql_quote(ci->badwords[i].word);

            ret = db_mysql_try("UPDATE anope_cs_badwords "
                               "SET type = %d, active = 1 "
                               "WHERE channel = '%s' AND word = '%s'",
                               (int) ci->badwords[i].type, q_name,
                               q_badwords);

            if (ret && (mysql_affected_rows(mysql) == 0)) {
                ret = db_mysql_try("INSERT DELAYED INTO anope_cs_badwords "
                                   "(channel, word, type, active) "
                                   "VALUES ('%s', '%s', %d, 1)",
                                   q_name, q_badwords,
                                   (int) ci->badwords[i].type);
            }

            free(q_badwords);
        }
    }

    /* TTB's */
    for (i = 0; ret && (i < TTB_SIZE); i++) {
        ret = db_mysql_try("UPDATE anope_cs_ttb "
                           "SET value = %d, active = 1 "
                           "WHERE channel = '%s' AND ttb_id = %d",
                           ci->ttb[i], q_name, i);

        if (ret && (mysql_affected_rows(mysql) == 0)) {
            ret = db_mysql_try("INSERT DELAYED INTO anope_cs_ttb "
                               "(channel, ttb_id, value, active) "
                               "VALUES ('%s', %d, %d, 1)",
                               q_name, i, ci->ttb[i]);
        }
    }

    free(q_name);
    free(q_founder);
    free(q_successor);
    free(q_pass);
    free(q_desc);
    free(q_url);
    free(q_email);
    free(q_lasttopic);
    free(q_lasttopicsetter);
    free(q_mlock_key);
    free(q_mlock_flood);
    free(q_mlock_redirect);
    free(q_entrymsg);
    free(q_botnick);
    free(q_forbidby);
    free(q_forbidreason);

    return ret;
}

/*************************************************************************/


/*
 * OperServ Specific Section
 */

/*************************************************************************/

/* Save the OperServ database into MySQL
 * Return 1 on success, 0 on failure
 * These tables are tagged and will be cleaned:
 * - anope_os_akills
 * - anope_os_sglines
 * - anope_os_sqlines
 * - anope_os_szlines
 * These tables are emptied:
 * - anope_os_core
 */

int db_mysql_save_os_db(unsigned int maxucnt, unsigned int maxutime,
                         SList * ak, SList * sgl, SList * sql, SList * szl)
{
	int ret;
    int i;
    Akill *akl;
    SXLine *sl;
    char *q_user;
    char *q_host;
    char *q_mask;
    char *q_by;
    char *q_reason;


    /* First save the core info */
    ret = db_mysql_try("INSERT DELAYED INTO anope_os_core "
                       "(maxusercnt, maxusertime, akills_count, sglines_count, sqlines_count, szlines_count) "
                       "VALUES (%d, %d, %d, %d, %d, %d)",
                       maxucnt, maxutime, ak->count, sgl->count, sql->count,
                       szl->count);

    /* Next save all AKILLs */
    for (i = 0; ret && (i < ak->count); i++) {
        akl = ak->list[i];
        q_user = db_mysql_quote(akl->user);
        q_host = db_mysql_quote(akl->host);
        q_by = db_mysql_quote(akl->by);
        q_reason = db_mysql_quote(akl->reason);

        ret = db_mysql_try("UPDATE anope_os_akills "
                           "SET xby = '%s', reason = '%s', seton = %d, expire = %d, active = 1 "
                           "WHERE user = '%s' AND host = '%s'",
                           q_by, q_reason, (int) akl->seton,
                           (int) akl->expires, q_user, q_host);

        if (ret && (mysql_affected_rows(mysql) == 0)) {
            ret = db_mysql_try("INSERT DELAYED INTO anope_os_akills "
                               "(user, host, xby, reason, seton, expire, active) "
                               "VALUES ('%s', '%s', '%s', '%s', %d, %d, 1)",
                                q_user, q_host, q_by, q_reason,
                                (int) akl->seton, (int) akl->expires);
        }

        free(q_user);
        free(q_host);
        free(q_by);
        free(q_reason);
    }

    /* Time to save the SGLINEs */
    for (i = 0; ret && (i < sgl->count); i++) {
        sl = sgl->list[i];
        q_mask = db_mysql_quote(sl->mask);
        q_by = db_mysql_quote(sl->by);
        q_reason = db_mysql_quote(sl->reason);

        ret = db_mysql_try("UPDATE anope_os_sglines "
                           "SET xby = '%s', reason = '%s', seton = %d, expire = %d, active = 1 "
                           "WHERE mask = '%s'",
                           q_by, q_reason, (int) sl->seton, (int) sl->expires,
                           q_mask);

        if (ret && (mysql_affected_rows(mysql) == 0)) {
            ret = db_mysql_try("INSERT DELAYED INTO anope_os_sglines "
                               "(mask, xby, reason, seton, expire, active) "
                               "VALUES ('%s', '%s', '%s', %d, %d, 1)",
                               q_mask, q_by, q_reason, (int) sl->seton,
                               (int) sl->expires);
        }

        free(q_mask);
        free(q_by);
        free(q_reason);
    }

    /* Save the SQLINEs */
    for (i = 0; ret && (i < sql->count); i++) {
        sl = sql->list[i];

        q_mask = db_mysql_quote(sl->mask);
        q_by = db_mysql_quote(sl->by);
        q_reason = db_mysql_quote(sl->reason);

        ret = db_mysql_try("UPDATE anope_os_sqlines "
                           "SET xby = '%s', reason = '%s',  seton = %d, expire = %d, active = 1 "
                           "WHERE mask = '%s'",
                           q_by, q_reason, (int) sl->seton, (int) sl->expires,
                           q_mask);

        if (ret && (mysql_affected_rows(mysql) == 0)) {
            ret = db_mysql_try("INSERT DELAYED INTO anope_os_sqlines "
                               "(mask, xby, reason, seton, expire, active) "
                               "VALUES ('%s', '%s', '%s', %d, %d, 1)",
                               q_mask, q_by, q_reason, (int) sl->seton,
                               (int) sl->expires);
        }

        free(q_mask);
        free(q_by);
        free(q_reason);
    }

    /* Now save the SZLINEs */
    for (i = 0; ret && (i < szl->count); i++) {
        sl = szl->list[i];

        q_mask = db_mysql_quote(sl->mask);
        q_by = db_mysql_quote(sl->by);
        q_reason = db_mysql_quote(sl->reason);

        ret = db_mysql_try("UPDATE anope_os_szlines "
                           "SET xby = '%s', reason = '%s', seton = %d, expire = %d, active = 1 "
                           "WHERE mask = '%s'",
                           q_by, q_reason, (int) sl->seton, (int) sl->expires,
                           q_mask);

        if (ret && (mysql_affected_rows(mysql) == 0)) {
            ret = db_mysql_try("INSERT DELAYED INTO anope_os_szlines "
                               "(mask, xby, reason, seton, expire, active) "
                               "VALUES ('%s', '%s', '%s', %d, %d, 1)",
                               q_mask, q_by, q_reason, (int) sl->seton,
                               (int) sl->expires);
        }

        free(q_mask);
        free(q_by);
        free(q_reason);
    }

    return ret;
}

/*************************************************************************/

/* Save the given NewsItem
 * These tables are tagged and will be cleaned:
 * - anope_os_news
 */
int db_mysql_save_news(NewsItem * ni)
{
	int ret;
    char *q_text;
    char *q_who;

    q_text = db_mysql_quote(ni->text);
    q_who = db_mysql_quote(ni->who);

    ret = db_mysql_try("UPDATE anope_os_news "
                       "SET ntext = '%s', who = '%s', active = 1 "
                       "WHERE type = %d AND num = %d AND `time` = %d",
                       q_text, q_who, ni->type, ni->num, (int) ni->time);

    if (ret && (mysql_affected_rows(mysql) == 0)) {
        ret = db_mysql_try("INSERT DELAYED INTO anope_os_news "
                           "(type, num, ntext, who, `time`, active) "
                           "VALUES (%d, %d, '%s', '%s', %d, 1)",
                           ni->type, ni->num, q_text, q_who, (int) ni->time);
    }

    free(q_text);
    free(q_who);

    return ret;
}

/*************************************************************************/

/* Save the given Exception
 * These tables are tagged and will be cleaned:
 * - anope_os_exceptions
 */

int db_mysql_save_exceptions(Exception * e)
{
	int ret;
    char *q_mask;
    char *q_who;
    char *q_reason;

    q_mask = db_mysql_quote(e->mask);
    q_who = db_mysql_quote(e->who);
    q_reason = db_mysql_quote(e->reason);

    ret = db_mysql_try("UPDATE anope_os_exceptions "
                       "SET lim = %d, who = '%s', reason = '%s', `time` = %d, expires = %d, active = 1 "
                       "WHERE mask = '%s'",
                       e->limit, q_who, q_reason, (int) e->time,
                       (int) e->expires, q_mask);

    if (ret && (mysql_affected_rows(mysql)) == 0) {
        ret = db_mysql_try("INSERT DELAYED INTO anope_os_exceptions "
                           "(mask, lim, who, reason, `time`, expires, active) "
                           "VALUES ('%s', %d, '%s', '%s', %d, %d, 1)",
                           q_mask, e->limit, q_who, q_reason, (int) e->time,
                           (int) e->expires);
    }

    free(q_mask);
    free(q_who);
    free(q_reason);

    return ret;
}

/*************************************************************************/


/*
 * HostServ Specific Section
 */

/*************************************************************************/

/* Save the given HostCore
 * These tables are tagged and will be cleaned:
 * - anope_hs_core
 */

int db_mysql_save_hs_core(HostCore * hc)
{
	int ret;
    char *q_nick;
    char *q_ident;
    char *q_host;
    char *q_creator;

    q_nick = db_mysql_quote(hc->nick);
    q_ident = db_mysql_quote(hc->vIdent);
    q_host = db_mysql_quote(hc->vHost);
    q_creator = db_mysql_quote(hc->creator);

    ret = db_mysql_try("UPDATE anope_hs_core "
                       "SET vident = '%s', vhost = '%s', creator = '%s', `time` = %d, active = 1 "
                       "WHERE nick = '%s'",
                       q_ident, q_host, q_creator, (int) hc->time, q_nick);

    if (ret && (mysql_affected_rows(mysql) == 0)) {
        ret = db_mysql_try("INSERT DELAYED INTO anope_hs_core "
                           "(nick, vident, vhost, creator, `time`, active) "
                           "VALUES ('%s', '%s', '%s', '%s', %d, 1)",
                           q_nick, q_ident, q_host, q_creator,
                           (int) hc->time);
    }

    free(q_nick);
    free(q_ident);
    free(q_host);
    free(q_creator);

    return ret;
}

/*************************************************************************/

/*
 * BotServ Specific Section
 */

/*************************************************************************/

int db_mysql_save_bs_core(BotInfo * bi)
{
	int ret;
    char *q_nick;
    char *q_user;
    char *q_host;
    char *q_real;

    q_nick = db_mysql_quote(bi->nick);
    q_user = db_mysql_quote(bi->user);
    q_host = db_mysql_quote(bi->host);
    q_real = db_mysql_quote(bi->real);

    ret = db_mysql_try("UPDATE anope_bs_core "
                       "SET user = '%s', host = '%s', rname = '%s', flags = %d, created = %d, chancount = %d, active = 1 "
                       "WHERE nick = '%s'",
                       q_user, q_host, q_real, bi->flags, (int) bi->created,
                       bi->chancount, q_nick);

    if (ret && (mysql_affected_rows(mysql) == 0)) {
        ret = db_mysql_try("INSERT DELAYED INTO anope_bs_core "
                           "(nick, user, host, rname, flags, created, chancount, active) "
                           "VALUES ('%s', '%s', '%s', '%s', %d, %d, %d, 1)",
                           q_nick, q_user, q_host, q_real, bi->flags,
                           (int) bi->created, bi->chancount);
    }

    free(q_nick);
    free(q_user);
    free(q_host);
    free(q_real);

    return ret;
}

/*************************************************************************/
/*************************************************************************/

/* Some loading code! */

/*************************************************************************/
/*************************************************************************/

int db_mysql_load_bs_dbase(void)
{
	int ret;
    BotInfo *bi;

    if (!do_mysql)
        return 0;

    ret = db_mysql_try("SELECT nick, user, host, rname, flags, created, chancount "
                       "FROM anope_bs_core "
                       "WHERE active = 1");

    if (!ret)
        return 0;

    mysql_res = mysql_use_result(mysql);

    while ((mysql_row = mysql_fetch_row(mysql_res))) {
        bi = makebot(mysql_row[0]);
        bi->user = sstrdup(mysql_row[1]);
        bi->host = sstrdup(mysql_row[2]);
        bi->real = sstrdup(mysql_row[3]);
        bi->flags = strtol(mysql_row[4], (char **) NULL, 10);
        bi->created = strtol(mysql_row[5], (char **) NULL, 10);
        bi->chancount = strtol(mysql_row[6], (char **) NULL, 10);
    }

    mysql_free_result(mysql_res);

    return 1;
}

int db_mysql_load_hs_dbase(void)
{
	int ret;
	char *vident = NULL;
	int32 time;

	if (!do_mysql)
		return 0;

	ret = db_mysql_try("SELECT nick, vident, vhost, creator, `time` "
			"FROM anope_hs_core "
			"WHERE active = 1");

	if (!ret)
		return 0;

	mysql_res = mysql_use_result(mysql);

	while ((mysql_row = mysql_fetch_row(mysql_res)))
	{
		/* FIX: we can never allow an empty vident, it must *always* be NULL. */
		vident = mysql_row[1];
		if (!strcmp(vident, ""))
		{
			vident = NULL;
		}

		time = strtol(mysql_row[4], (char **) NULL, 10);
		addHostCore(mysql_row[0], NULL, mysql_row[2], mysql_row[3], time);
	}

	mysql_free_result(mysql_res);

	return 1;
}

int db_mysql_load_news(void)
{
	int ret;
    int i;

    if (!do_mysql)
        return 0;

    ret = db_mysql_try("SELECT type, num, ntext, who, `time` "
                       "FROM anope_os_news "
                       "WHERE active = 1");

    if (!ret)
        return 0;

    mysql_res = mysql_store_result(mysql);

    nnews = mysql_num_rows(mysql_res);
    if (nnews < 8)              /* 2^3 */
        news_size = 16;         /* 2^4 */
    else if (nnews >= 16384)    /* 2^14 */
        news_size = 32767;      /* 2^15 - 1 */
    else
        news_size = 2 * nnews;

    news = scalloc(news_size, sizeof(*news));

    i = 0;
    while ((mysql_row = mysql_fetch_row(mysql_res))) {
        news[i].type = strtol(mysql_row[0], (char **) NULL, 10);
        news[i].num = strtol(mysql_row[1], (char **) NULL, 10);
        news[i].text = sstrdup(mysql_row[2]);
        snprintf(news[i].who, NICKMAX, "%s", mysql_row[3]);
        news[i].time = strtol(mysql_row[4], (char **) NULL, 10);
        i++;
    }

    mysql_free_result(mysql_res);

    return 1;
}

int db_mysql_load_exceptions(void)
{
	int ret;
    int i;

    if (!do_mysql)
        return 0;

    ret = db_mysql_try("SELECT mask, lim, who, reason, `time`, expires "
                       "FROM anope_os_exceptions "
                       "WHERE active = 1");

    if (!ret)
        return 0;

    mysql_res = mysql_store_result(mysql);
    nexceptions = mysql_num_rows(mysql_res);
    exceptions = scalloc(nexceptions, sizeof(Exception));

    i = 0;
    while ((mysql_row = mysql_fetch_row(mysql_res))) {
        exceptions[i].mask = sstrdup(mysql_row[0]);
        exceptions[i].limit = strtol(mysql_row[1], (char **) NULL, 10);
        snprintf(exceptions[i].who, NICKMAX, "%s", mysql_row[2]);
        exceptions[i].reason = sstrdup(mysql_row[3]);
        exceptions[i].time = strtol(mysql_row[4], (char **) NULL, 10);
        exceptions[i].expires = strtol(mysql_row[5], (char **) NULL, 10);
        i++;
    }

    mysql_free_result(mysql_res);

    return 1;
}

int db_mysql_load_os_dbase(void)
{
	int ret;
    Akill *ak;
    SXLine *sl;
    int akc, sglc, sqlc, szlc;

    if (!do_mysql)
        return 0;

    ret = db_mysql_try("SELECT maxusercnt, maxusertime, akills_count, sglines_count, sqlines_count, szlines_count "
                       "FROM anope_os_core");

    if (!ret)
        return 0;

    mysql_res = mysql_use_result(mysql);

    if ((mysql_row = mysql_fetch_row(mysql_res))) {
        maxusercnt = strtol(mysql_row[0], (char **) NULL, 10);
        maxusertime = strtol(mysql_row[1], (char **) NULL, 10);
        /* I'm not too happy with the idea of storing thse counts in a field
         * instead of just using mysql_num_rows on the actual tables when
         * filling the data. For now this will do, but it's bound to give
         * problems sooner or later... (it probably does if you are looking
         * at this) -GD
         */
        akc = strtol(mysql_row[2], (char **) NULL, 10);
        sglc = strtol(mysql_row[3], (char **) NULL, 10);
        sqlc = strtol(mysql_row[4], (char **) NULL, 10);
        szlc = strtol(mysql_row[5], (char **) NULL, 10);
    } else {
        maxusercnt = 0;
        maxusertime = time(NULL);
        akc = 0;
        sglc = 0;
        sqlc = 0;
        szlc = 0;
    }

    mysql_free_result(mysql_res);


    /* Load the AKILLs */

    ret = db_mysql_try("SELECT user, host, xby, reason, seton, expire "
                       "FROM anope_os_akills "
                       "WHERE active = 1");

    if (!ret)
        return 0;

    mysql_res = mysql_use_result(mysql);
    slist_setcapacity(&akills, akc);

    while ((mysql_row = mysql_fetch_row(mysql_res))) {
        ak = scalloc(1, sizeof(Akill));
        ak->user = sstrdup(mysql_row[0]);
        ak->host = sstrdup(mysql_row[1]);
        ak->by = sstrdup(mysql_row[2]);
        ak->reason = sstrdup(mysql_row[3]);
        ak->seton = strtol(mysql_row[4], (char **) NULL, 10);
        ak->expires = strtol(mysql_row[5], (char **) NULL, 10);
        slist_add(&akills, ak);
    }

    mysql_free_result(mysql_res);


    /* Load the SGLINEs */

    ret = db_mysql_try("SELECT mask, xby, reason, seton, expire "
                       "FROM anope_os_sglines "
                       "WHERE active = 1");

    if (!ret)
        return 0;

    mysql_res = mysql_use_result(mysql);
    slist_setcapacity(&sglines, sglc);

    while ((mysql_row = mysql_fetch_row(mysql_res))) {
        sl = scalloc(1, sizeof(SXLine));
        sl->mask = sstrdup(mysql_row[0]);
        sl->by = sstrdup(mysql_row[1]);
        sl->reason = sstrdup(mysql_row[2]);
        sl->seton = strtol(mysql_row[3], (char **) NULL, 10);
        sl->expires = strtol(mysql_row[4], (char **) NULL, 10);
        slist_add(&sglines, sl);
    }

    mysql_free_result(mysql_res);


    /* Load the SQLINEs */

    ret = db_mysql_try("SELECT mask, xby, reason, seton, expire "
                       "FROM anope_os_sqlines "
                       "WHERE active = 1");

    if (!ret)
        return 0;

    mysql_res = mysql_use_result(mysql);
    slist_setcapacity(&sqlines, sqlc);

    while ((mysql_row = mysql_fetch_row(mysql_res))) {
        sl = scalloc(1, sizeof(SXLine));
        sl->mask = sstrdup(mysql_row[0]);
        sl->by = sstrdup(mysql_row[1]);
        sl->reason = sstrdup(mysql_row[2]);
        sl->seton = strtol(mysql_row[3], (char **) NULL, 10);
        sl->expires = strtol(mysql_row[4], (char **) NULL, 10);
        slist_add(&sqlines, sl);
    }

    mysql_free_result(mysql_res);


    /* Load the SZLINEs */

    ret = db_mysql_try("SELECT mask, xby, reason, seton, expire "
                       "FROM anope_os_szlines "
                       "WHERE active = 1");

    if (!ret)
        return 0;

    mysql_res = mysql_use_result(mysql);
    slist_setcapacity(&szlines, szlc);

    while ((mysql_row = mysql_fetch_row(mysql_res))) {
        sl = scalloc(1, sizeof(SXLine));
        sl->mask = sstrdup(mysql_row[0]);
        sl->by = sstrdup(mysql_row[1]);
        sl->reason = sstrdup(mysql_row[2]);
        sl->seton = strtol(mysql_row[3], (char **) NULL, 10);
        sl->expires = strtol(mysql_row[4], (char **) NULL, 10);
        slist_add(&szlines, sl);
    }

    mysql_free_result(mysql_res);

    return 1;
}

int db_mysql_load_cs_dbase(void)
{
    int ret;
    char *q_name;
    ChannelInfo *ci;
    int i;
    MYSQL_RES *res;
    MYSQL_ROW row;
    unsigned long *lengths;

    if (!do_mysql)
        return 0;

    ret = db_mysql_try("SELECT name, founder, successor, founderpass, descr, url, email, time_registered, last_used, last_topic, last_topic_setter, "
                       "       last_topic_time, flags, forbidby, forbidreason, bantype, accesscount, akickcount, mlock_on, mlock_off, mlock_limit, "
                       "       mlock_key, mlock_flood, mlock_redirect, entry_message, memomax, botnick, botflags, bwcount, capsmin, capspercent, floodlines, "
                       "       floodsecs, repeattimes "
                       "FROM anope_cs_info "
                       "WHERE active = 1");

    if (!ret)
        return 0;

    /* I'd really like to use mysql_use_result here, but it'd tie up with
     * all the queries being run inside each iteration... -GD
     */
    mysql_res = mysql_store_result(mysql);

    while (ret && (mysql_row = mysql_fetch_row(mysql_res))) {
        /* We need to get the length of the password.. - Viper */
        lengths = mysql_fetch_lengths(mysql_res);
        ci = scalloc(1, sizeof(ChannelInfo));

        /* Name, founder, successor, password */
        snprintf(ci->name, CHANMAX, "%s", mysql_row[0]);
        ci->founder = findcore(mysql_row[1]);
        if (mysql_row[2] && *(mysql_row[2]))
            ci->successor = findcore(mysql_row[2]);
        else
            ci->successor = NULL;

        /* Copy the password from what we got back from the DB and
         * keep in mind that lengths may vary. We should never
         * use more than we have. - Viper */
        memset(ci->founderpass, 0, PASSMAX);
        if (lengths[3] >= PASSMAX)
            memcpy(ci->founderpass, mysql_row[3], PASSMAX - 1);
        else
            memcpy(ci->founderpass, mysql_row[3], lengths[3]);

        /* Description, URL, email -- scalloc() initializes to 0/NULL */
        ci->desc = sstrdup(mysql_row[4]);
        if (mysql_row[5] && *(mysql_row[5]))
            ci->url = sstrdup(mysql_row[5]);
        if (mysql_row[6] && *(mysql_row[6]))
            ci->email = sstrdup(mysql_row[6]);

        /* Time registered, last used, last topic, last topic setter + time */
        ci->time_registered = strtol(mysql_row[7], (char **) NULL, 10);
        ci->last_used = strtol(mysql_row[8], (char **) NULL, 10);
        if (mysql_row[9] && *(mysql_row[9])) {
        	ci->last_topic = sstrdup(mysql_row[9]);
        	snprintf(ci->last_topic_setter, NICKMAX, "%s", mysql_row[10]);
        	ci->last_topic_time = strtol(mysql_row[11], (char **) NULL, 10);
        }

        /* Flags, forbidden by, forbid reason, bantype
         * NOTE: CI_INHABIT will be disabled in flags!!
         */
        ci->flags =
            strtol(mysql_row[12], (char **) NULL, 10) & ~CI_INHABIT;

        if (mysql_row[13] && *(mysql_row[13])) {
        	ci->forbidby = sstrdup(mysql_row[13]);
        	if (mysql_row[14] && *(mysql_row[14]))
        		ci->forbidreason = sstrdup(mysql_row[14]);
        }

        ci->bantype = strtol(mysql_row[15], (char **) NULL, 10);

        /* Accesscount, akickcount */
        ci->accesscount = strtol(mysql_row[16], (char **) NULL, 10);
        ci->akickcount = strtol(mysql_row[17], (char **) NULL, 10);

        /* Mlock: on, off, limit, key, flood, redirect */
        ci->mlock_on = strtol(mysql_row[18], (char **) NULL, 10);
        ci->mlock_off = strtol(mysql_row[19], (char **) NULL, 10);
        ci->mlock_limit = strtol(mysql_row[20], (char **) NULL, 10);
        if (mysql_row[21] && *(mysql_row[21])) {
        	ci->mlock_key = sstrdup(mysql_row[21]);
        	if (mysql_row[22] && *(mysql_row[22])) {
        		ci->mlock_flood = sstrdup(mysql_row[22]);
        	}
        	if (mysql_row[23] && *(mysql_row[23])) {
        		ci->mlock_redirect = sstrdup(mysql_row[23]);
        	}
        }

        /* MemoMax, entrymessage, botinfo, botflags, badwordcount */
        ci->memos.memomax = strtol(mysql_row[25], (char **) NULL, 10);
        if (mysql_row[24] && *(mysql_row[24]))
            ci->entry_message = sstrdup(mysql_row[24]);
        ci->bi = findbot(mysql_row[26]);
        ci->botflags = strtol(mysql_row[27], (char **) NULL, 10);
        ci->bwcount = strtol(mysql_row[28], (char **) NULL, 10);

        /* Capsmin, capspercent, floodlines, floodsecs, repeattimes */
        ci->capsmin = strtol(mysql_row[29], (char **) NULL, 10);
        ci->capspercent = strtol(mysql_row[30], (char **) NULL, 10);
        ci->floodlines = strtol(mysql_row[31], (char **) NULL, 10);
        ci->floodsecs = strtol(mysql_row[32], (char **) NULL, 10);
        ci->repeattimes = strtol(mysql_row[33], (char **) NULL, 10);


        /* Get info from other tables; we'll need the channel name */
        q_name = db_mysql_quote(ci->name);

        /* Get the LEVELS list */
        ret = db_mysql_try("SELECT position, level "
                           "FROM anope_cs_levels "
                           "WHERE channel = '%s' AND active = 1",
                           q_name);

        if (ret) {
            res = mysql_use_result(mysql);
            ci->levels = scalloc(CA_SIZE, sizeof(*ci->levels));
            reset_levels(ci);

            while ((row = mysql_fetch_row(res))) {
                i = strtol(row[0], (char **) NULL, 10);
                ci->levels[i] = strtol(row[1], (char **) NULL, 10);
            }

            mysql_free_result(res);
        }

        /* Get the channel ACCESS list */
        if (ret && (ci->accesscount > 0)) {
            ci->access = scalloc(ci->accesscount, sizeof(ChanAccess));

            ret = db_mysql_try("SELECT level, display, last_seen "
                               "FROM anope_cs_access "
                               "WHERE channel = '%s' AND in_use = 1 AND active = 1",
                               q_name);

            if (ret) {
                res = mysql_store_result(mysql);

                i = 0;
                while ((row = mysql_fetch_row(res))) {
                    ci->access[i].in_use = 1;
                    ci->access[i].level = strtol(row[0], (char **) NULL, 10);
                    ci->access[i].nc = findcore(row[1]);
                    if (!(ci->access[i].nc))
                        ci->access[i].in_use = 0;
                    ci->access[i].last_seen =
                        strtol(row[2], (char **) NULL, 10);
                    i++;
                }

                mysql_free_result(res);
            }
        }

        /* Get the channel AKICK list */
        if (ret && (ci->akickcount > 0)) {
            ci->akick = scalloc(ci->akickcount, sizeof(AutoKick));

            ret = db_mysql_try("SELECT flags, dmask, reason, creator, addtime "
                               "FROM anope_cs_akicks "
                               "WHERE channel = '%s' AND active = 1 AND (flags & %d) <> 0",
                               q_name, AK_USED);

            if (ret) {
                res = mysql_use_result(mysql);

                i = 0;
                while ((row = mysql_fetch_row(res))) {
                    ci->akick[i].flags = strtol(row[0], (char **) NULL, 10);
                    if (ci->akick[i].flags & AK_ISNICK) {
                        ci->akick[i].u.nc = findcore(row[1]);
                        if (!(ci->akick[i].u.nc))
                            ci->akick[i].flags &= ~AK_USED;
                    } else {
                        ci->akick[i].u.mask = sstrdup(row[1]);
                    }
                    if (row[2] && *(row[2])) {
                    	ci->akick[i].reason = sstrdup(row[2]);
                    }
                    ci->akick[i].creator = sstrdup(row[3]);
                    ci->akick[i].addtime = strtol(row[4], (char **) NULL, 10);
                    i++;
                }

                mysql_free_result(res);
            }
        }

        if (ret) {
            /* Get the channel memos */
            ret = db_mysql_try("SELECT nm_id, number, flags, time, sender, text "
                               "FROM anope_ms_info "
                               "WHERE receiver = '%s' AND serv = 'CHAN' AND active = 1",
                               q_name);

            if (ret) {
                res = mysql_store_result(mysql);
                ci->memos.memocount = mysql_num_rows(res);

                if (ci->memos.memocount > 0) {
                    Memo *memos;

                    memos = scalloc(ci->memos.memocount, sizeof(Memo));
                    ci->memos.memos = memos;

                    i = 0;
                    while ((row = mysql_fetch_row(res))) {
                        memos[i].id = strtol(row[0], (char **) NULL, 10);
                        memos[i].number = strtol(row[1], (char **) NULL, 10);
                        memos[i].flags = strtol(row[2], (char **) NULL, 10);
                        memos[i].time = strtol(row[3], (char **) NULL, 10);
                        snprintf(memos[i].sender, NICKMAX, "%s", row[4]);
                        memos[i].text = sstrdup(row[5]);
                        i++;
                    }
                }

                mysql_free_result(res);
            }
        }

        /* Get the TTB data */
        if (ret) {
            ci->ttb = scalloc(TTB_SIZE, sizeof(*ci->ttb));

            ret = db_mysql_try("SELECT ttb_id, value "
                               "FROM anope_cs_ttb "
                               "WHERE channel = '%s' AND active = 1",
                               q_name);

            if (ret) {
                res = mysql_use_result(mysql);

                while ((row = mysql_fetch_row(res))) {
                    i = strtol(row[0], (char **) NULL, 10);
                    /* Should we do a sanity check on the value of i? -GD */
                    ci->ttb[i] = strtol(row[1], (char **) NULL, 10);
                }

                mysql_free_result(res);
            }
        }

        /* Get the badwords */
        if (ret && (ci->bwcount > 0)) {
            ci->badwords = scalloc(ci->bwcount, sizeof(BadWord));

            ret = db_mysql_try("SELECT word, type "
                               "FROM anope_cs_badwords "
                               "WHERE channel = '%s' AND active = 1",
                               q_name);

            if (ret) {
                res = mysql_use_result(mysql);

                i = 0;
                while ((row = mysql_fetch_row(res))) {
                    ci->badwords[i].in_use = 1;
                    ci->badwords[i].word = sstrdup(row[0]);
                    ci->badwords[i].type = strtol(row[1], (char **) NULL, 10);
                    i++;
                }

                mysql_free_result(res);
            }
        }

        /* YAY! all done; free q_name and insert the channel */
        free(q_name);
        alpha_insert_chan(ci);
    }

    mysql_free_result(mysql_res);

    /* Check to be sure that all channels still have a founder. If not,
     * delete them. This code seems to be required in the old mysql code
     * so i'll leave it in just to be sure. I also wonder why they didn't
     * do that check up above immediately when it was known there was no
     * founder... -GD
     */
    for (i = 0; i < 256; i++) {
        ChannelInfo *next;
        for (ci = chanlists[i]; ci; ci = next) {
            next = ci->next;
            if (!(ci->flags & CI_VERBOTEN) && !ci->founder) {
                alog("%s: database load: Deleting founderless channel %s",
                     s_ChanServ, ci->name);
                delchan(ci);
            }
        }
    }

    return ret;
}

int db_mysql_load_ns_req_dbase(void)
{
    int ret;
    NickRequest *nr;
    unsigned long *lengths;

    if (!do_mysql)
        return 0;

    ret = db_mysql_try("SELECT nick, passcode, password, email, requested "
                       "FROM anope_ns_request "
                       "WHERE active = 1");

    if (ret) {
        mysql_res = mysql_use_result(mysql);

        while ((mysql_row = mysql_fetch_row(mysql_res))) {
            /* We need to get the length of the password.. - Viper */
            lengths = mysql_fetch_lengths(mysql_res);
            nr = scalloc(1, sizeof(NickRequest));

            nr->nick = sstrdup(mysql_row[0]);
            nr->passcode = sstrdup(mysql_row[1]);
            nr->email = sstrdup(mysql_row[3]);
            nr->requested = strtol(mysql_row[4], (char **) NULL, 10);

            /* Copy the password from what we got back from the DB and
             * keep in mind that lengths may vary. We should never
             * use more than we have. - Viper */
            memset(nr->password, 0, PASSMAX);
            if (lengths[2] >= PASSMAX)
                memcpy(nr->password, mysql_row[2], PASSMAX - 1);
            else
                memcpy(nr->password, mysql_row[2], lengths[2]);


            insert_requestnick(nr);
        }

        mysql_free_result(mysql_res);
    }

    return ret;
}

int db_mysql_load_ns_dbase(void)
{
    int ret;
    char *q_display;
    NickCore *nc;
    NickAlias *na;
    MYSQL_RES *res;
    MYSQL_ROW row;
    int i;
    unsigned long *lengths;


    if (!do_mysql)
        return 0;

    ret = db_mysql_try("SELECT display, pass, email, icq, url, flags, language, accesscount, memocount, memomax, channelcount, channelmax, greet "
                       "FROM anope_ns_core "
                       "WHERE active = 1");

    if (!ret)
        return 0;

    /* I'd really like to use mysql_use_result here, but it'd tie up with
     * all the queries being run inside each iteration... -GD
     */
    mysql_res = mysql_store_result(mysql);

    while (ret && (mysql_row = mysql_fetch_row(mysql_res))) {
        /* We need to get the length of the password.. - Viper */
        lengths = mysql_fetch_lengths(mysql_res);

        nc = scalloc(1, sizeof(NickCore));

        /* Display, password, email, ICQ, URL, flags */
        nc->display = sstrdup(mysql_row[0]);
        if (mysql_row[2] && *(mysql_row[2])) {
        	nc->email = sstrdup(mysql_row[2]);
        }
        nc->icq = strtol(mysql_row[3], (char **) NULL, 10);
        if (mysql_row[4] && *(mysql_row[4])) {
        	nc->url = sstrdup(mysql_row[4]);
        }
        nc->flags = strtol(mysql_row[5], (char **) NULL, 10);

        /* Copy the password from what we got back from the DB and
         * keep in mind that lengths may vary. We should never
         * use more than we have. - Viper */
        memset(nc->pass, 0, PASSMAX);
        if (lengths[1] >= PASSMAX)
            memcpy(nc->pass, mysql_row[1], PASSMAX - 1);
        else
            memcpy(nc->pass, mysql_row[1], lengths[1]);

        /* Language, accesscount, memocount, memomax */
        nc->language = strtol(mysql_row[6], (char **) NULL, 10);
        nc->accesscount = strtol(mysql_row[7], (char **) NULL, 10);
        nc->memos.memocount = strtol(mysql_row[8], (char **) NULL, 10);
        nc->memos.memomax = strtol(mysql_row[9], (char **) NULL, 10);

        /* Channelcount, channelmax, greet */
        nc->channelcount = strtol(mysql_row[10], (char **) NULL, 10);
        nc->channelmax = strtol(mysql_row[11], (char **) NULL, 10);
        if (mysql_row[12] && *(mysql_row[12]))
            nc->greet = sstrdup(mysql_row[12]);

        /* Don't allow KILL_IMMED if the config doesn't allow it */
        if (!NSAllowKillImmed)
            nc->flags &= ~NI_KILL_IMMED;

        /* Check if the current user is important enough to be added to
         * services admin or services oper lists
         */
        if (nc->flags & NI_SERVICES_ADMIN)
            slist_add(&servadmins, nc);
        if (nc->flags & NI_SERVICES_OPER)
            slist_add(&servopers, nc);

        /* Unset the SERVICES_ROOT flag; we will set it again later if this
         * user is really a services root (checked per NickAlias) -GD
         */
        nc->flags &= ~NI_SERVICES_ROOT;

        /* Get info from other tables; we'll need the display */
        q_display = db_mysql_quote(nc->display);

        /* Fill the accesslist */
        if (ret && (nc->accesscount > 0)) {
            nc->access = scalloc(nc->accesscount, sizeof(char *));

            ret = db_mysql_try("SELECT access "
                               "FROM anope_ns_access "
                               "WHERE display = '%s' AND active = 1",
                               q_display);

            if (ret) {
                res = mysql_use_result(mysql);

                i = 0;
                while ((row = mysql_fetch_row(res))) {
                    if (row[0] && *(row[0])) {
                        nc->access[i] = sstrdup(row[0]);
                        i++;
                    }
                }

		/* Versions previous to 1.8.3 have a problem with saving nick access lists to sql
		 * because of the way the table is set up, causing nc->accesscount to be higher
		 * than what is in the database, which causes numerious problems including crashes. - Adam
		 */
		if (nc->accesscount > i)
		{
			nc->accesscount = i;
			nc->access = srealloc(nc->access, sizeof(char *) * nc->accesscount);
		}

                mysql_free_result(res);
            }
        }

        /* Load the memos */
        if (ret && (nc->memos.memocount > 0)) {
            nc->memos.memos = scalloc(nc->memos.memocount, sizeof(Memo));

            ret = db_mysql_try("SELECT nm_id, number, flags, time, sender, text "
                               "FROM anope_ms_info "
                               "WHERE receiver = '%s' AND active = 1 AND serv = 'NICK' "
                               "ORDER BY number ASC",
                               q_display);

            if (ret) {
                res = mysql_use_result(mysql);

                i = 0;
                while ((row = mysql_fetch_row(res))) {
                    nc->memos.memos[i].id = strtol(row[0], (char **) NULL, 10);
                    nc->memos.memos[i].number = strtol(row[1], (char **) NULL, 10);
                    nc->memos.memos[i].flags = strtol(row[2], (char **) NULL, 10);
                    nc->memos.memos[i].time = strtol(row[3], (char **) NULL, 10);
                    snprintf(nc->memos.memos[i].sender, NICKMAX, "%s", row[4]);
                    nc->memos.memos[i].text = sstrdup(row[5]);

                    i++;
                }

                mysql_free_result(res);
            }
        }

        /* Done with the core; insert it */
        insert_core(nc);
    }

    mysql_free_result(mysql_res);

    if (!ret)
    	return 0;

    /* Load the nickaliases */
    ret = db_mysql_try("SELECT nick, display, time_registered, last_seen, status, last_usermask, last_realname, last_quit "
                       "FROM anope_ns_alias "
                       "WHERE active = 1");

    if (!ret)
        return 0;

    mysql_res = mysql_use_result(mysql);

    while ((mysql_row = mysql_fetch_row(mysql_res))) {
        /* First make sure this NickAlias has a NickCore; else we don't even
         * bother adding it to the aliases at all...
         */
        NickCore *nc;

        if (!(nc = findcore(mysql_row[1])))
            continue;

        na = scalloc(1, sizeof(NickAlias));

        /* nick, time_registered, last_seen, status
         * NOTE: remove NS_TEMPORARY from status on load
         */
        na->nick = sstrdup(mysql_row[0]);
        na->nc = nc;
        na->time_registered = strtol(mysql_row[2], (char **) NULL, 10);
        na->last_seen = strtol(mysql_row[3], (char **) NULL, 10);
        na->status =
            strtol(mysql_row[4], (char **) NULL, 10) & ~NS_TEMPORARY;

        /* last_usermask, last_realname, last_quit */
        na->last_usermask = sstrdup(mysql_row[5]);
        na->last_realname = sstrdup(mysql_row[6]);

        if (mysql_row[7] && *(mysql_row[7])) {
        	na->last_quit = sstrdup(mysql_row[7]);
        }

        /* Assign to the nickcore aliases */
        slist_add(&na->nc->aliases, na);

        /* Check if this user is a services root */
        for (i = 0; i < RootNumber; i++) {
            if (stricmp(ServicesRoots[i], na->nick) == 0)
                na->nc->flags |= NI_SERVICES_ROOT;
        }

        /* Last, but not least: insert the alias! */
        alpha_insert_alias(na);
    }

    mysql_free_result(mysql_res);

    return ret;
}

/* get random mysql number for the generator */
unsigned int mysql_rand(void)
{
    unsigned int num = 0;

    if (!do_mysql)
        return 0;

    db_mysql_try("SELECT RAND()");

    mysql_res = mysql_store_result(mysql);

    if (!(mysql_row = mysql_fetch_row(mysql_res))) {
        mysql_free_result(mysql_res);
        return 0;
    }

    num = UserKey3 * strtol(mysql_row[0], (char **) NULL, 10);

    mysql_free_result(mysql_res);

    return num;
}

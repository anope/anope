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

/* Database Global Variables */
MYSQL *mysql;                   /* MySQL Handler */
MYSQL_RES *mysql_res;           /* MySQL Result  */
MYSQL_FIELD *mysql_fields;      /* MySQL Fields  */
MYSQL_ROW mysql_row;            /* MySQL Row     */

/*************************************************************************/

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

int db_mysql_init()
{

    /* If the host is not defined, assume we don't want MySQL */
    if (!MysqlHost) {
        do_mysql = 0;
        alog("MySQL has been disabled.");
    } else {
        do_mysql = 1;
        alog("MySQL has been enabled.");
    }
    return 1;

}

/*************************************************************************/

int db_mysql_open()
{
    /* If MySQL is disabled, return 0 */
    if (!do_mysql)
        return 0;

    mysql = mysql_init(NULL);
    if (mysql == NULL)
        db_mysql_error(MYSQL_WARNING, "Unable to create mysql object");

    if (!MysqlPort)
        MysqlPort = 3306;

    if (MysqlSock) {
        if ((!mysql_real_connect
             (mysql, MysqlHost, MysqlUser, MysqlPass, MysqlName, MysqlPort,
              MysqlSock, 0))) {
            log_perror("Cant connect to MySQL: %s\n", mysql_error(mysql));
            return 0;
        }
    } else {
        if ((!mysql_real_connect
             (mysql, MysqlHost, MysqlUser, MysqlPass, MysqlName, MysqlPort,
              NULL, 0))) {
            log_perror("Cant connect to MySQL: %s\n", mysql_error(mysql));
            return 0;
        }
    }

    return 1;

}

/*************************************************************************/

int db_mysql_query(char *sql)
{

    int result, lcv;

    result = mysql_query(mysql, sql);

    if (result) {
        switch (mysql_errno(mysql)) {
        case CR_SERVER_GONE_ERROR:
        case CR_SERVER_LOST:

            for (lcv = 0; lcv < MysqlRetries; lcv++) {
                if (db_mysql_open()) {
                    result = mysql_query(mysql, sql);
                    return (result);
                }
                sleep(MysqlRetryGap);
            }

            /* If we get here, we could not connect. */
            log_perror("Unable to reconnect to database: %s\n",
                       mysql_error(mysql));
            db_mysql_error(MYSQL_ERROR, "connect");

            /* Never reached. */
            break;

        default:
            /* Unhandled error. */
            return (result);
        }
    }

    return (0);

}

/*************************************************************************/

char *db_mysql_quote(char *sql)
{
    int slen;
    char *quoted;

    if (!sql) {
        return sstrdup("");
    }

    slen = strlen(sql);
    quoted = malloc((1 + (slen * 2)) * sizeof(char));

    mysql_real_escape_string(mysql, quoted, sql, slen);
    return quoted;

}

/*************************************************************************/

/* I don't like using res here, maybe we can pass it as a param? */
int db_mysql_close()
{
    if (mysql_res)
        mysql_free_result(mysql_res);
    mysql_close(mysql);
    return 1;
}

/*************************************************************************/

/*
 * NickServ Specific Secion
 */

/*************************************************************************/
void db_mysql_save_ns_req(NickRequest * nr)
{
    char *qnick, *qpasscode, *qpassword, *qemail;
    char sqlcmd[MAX_SQL_BUF];

    qnick = db_mysql_quote(nr->nick);
    qpasscode = db_mysql_quote(nr->passcode);
    qpassword = db_mysql_quote(nr->password);
    qemail = db_mysql_quote(nr->email);

    snprintf(sqlcmd, MAX_SQL_BUF,
             "REPLACE anope_ns_request (nick,passcode,password,email,requested,active)"
             " VALUES ('%s','%s','%s','%s','%d','1')",
             qnick, qpasscode, qpassword, qemail, (int) nr->requested);
    if (db_mysql_query(sqlcmd)) {
        log_perror("Can't create sql query: %s", sqlcmd);
        db_mysql_error(MYSQL_WARNING, "query");
    }
    free(qnick);
    free(qpasscode);
    free(qpassword);
    free(qemail);
}

char *db_mysql_secure(char *pass)
{

    char epass[BUFSIZE];

    if (!pass) {
        snprintf(epass, sizeof(epass), "''");
    } else if ((!MysqlSecure) || (strcmp(MysqlSecure, "") == 0)) {
        snprintf(epass, sizeof(epass), "'%s'", pass);
    } else if (strcmp(MysqlSecure, "des") == 0) {
        snprintf(epass, sizeof(epass), "ENCRYPT('%s')", pass);
    } else if (strcmp(MysqlSecure, "md5") == 0) {
        snprintf(epass, sizeof(epass), "MD5('%s')", pass);
    } else if (strcmp(MysqlSecure, "sha") == 0) {
        snprintf(epass, sizeof(epass), "SHA('%s')", pass);
    } else {
        snprintf(epass, sizeof(epass), "ENCODE('%s','%s')", pass,
                 MysqlSecure);
    }

    return sstrdup(epass);

}

/*************************************************************************/
void db_mysql_save_ns_core(NickCore * nc)
{
    char sqlcmd[MAX_SQL_BUF];
    int j;
    char **access;
    Memo *memos;
    char *cnick, *cpass, *epass, *cemail, *cgreet, *curl, *caccess,
        *msender, *mtext;

    cnick = db_mysql_quote(nc->display);
    cpass = db_mysql_quote(nc->pass);
    cemail = db_mysql_quote(nc->email);
    cgreet = db_mysql_quote(nc->greet);
    curl = db_mysql_quote(nc->url);

    epass = db_mysql_secure(cpass);
    free(cpass);

    /* Let's take care of the core itself */
    /* Update the existing records */
    snprintf(sqlcmd, MAX_SQL_BUF,
             "UPDATE anope_ns_core SET pass=%s,email='%s',greet='%s',icq='%d',url='%s',flags='%d',"
             "language='%d',accesscount='%d',memocount='%d',memomax='%d',channelcount='%d'"
             ",channelmax='%d',active='1' WHERE display='%s'",
             epass, cemail, cgreet, nc->icq, curl, nc->flags,
             nc->language, nc->accesscount, nc->memos.memocount,
             nc->memos.memomax, nc->channelcount, nc->channelmax, cnick);
    if (db_mysql_query(sqlcmd)) {
        log_perror("Can't create sql query: %s", sqlcmd);
        db_mysql_error(MYSQL_WARNING, "query");
    }

    /* need to write a wrapper for mysql_affected_rows */
    /* Our previous UPDATE affected no rows, therefore this is a new record */
    if ((int) mysql_affected_rows(mysql) <= 0) {

        /* Let's take care of the core itself */
        snprintf(sqlcmd, MAX_SQL_BUF,
                 "INSERT DELAYED INTO anope_ns_core (display,pass,email,greet,icq,url,flags,"
                 "language,accesscount,memocount,memomax,channelcount,channelmax,active)"
                 " VALUES ('%s',%s,'%s','%s','%d','%s','%d','%d','%d','%d','%d','%d','%d','1')",
                 cnick, epass, cemail, cgreet, nc->icq, curl, nc->flags,
                 nc->language, nc->accesscount, nc->memos.memocount,
                 nc->memos.memomax, nc->channelcount, nc->channelmax);
        if (db_mysql_query(sqlcmd)) {
            log_perror("Can't create sql query: %s", sqlcmd);
            db_mysql_error(MYSQL_WARNING, "query");
        }
    }

    /* Now let's do the access */
    for (j = 0, access = nc->access; j < nc->accesscount; j++, access++) {
        caccess = db_mysql_quote(*access);
        snprintf(sqlcmd, MAX_SQL_BUF,
                 "INSERT DELAYED INTO anope_ns_access (display,access) VALUES ('%s','%s')",
                 cnick, caccess);
        if (db_mysql_query(sqlcmd)) {
            log_perror("Can't create sql query: %s", sqlcmd);
            db_mysql_error(MYSQL_WARNING, "query");
        }
        free(caccess);
    }

    /* And... memos */
    memos = nc->memos.memos;
    for (j = 0; j < nc->memos.memocount; j++, memos++) {
        msender = db_mysql_quote(memos->sender);
        mtext = db_mysql_quote(memos->text);
        snprintf(sqlcmd, MAX_SQL_BUF,
                 "INSERT DELAYED INTO anope_ms_info (receiver,number,flags,time,sender,text,serv)"
                 " VALUES ('%s','%d','%d','%d','%s','%s','NICK')",
                 cnick, memos->number, memos->flags,
                 (int) memos->time, msender, mtext);
        if (db_mysql_query(sqlcmd)) {
            log_perror("Can't create sql query: %s", sqlcmd);
            db_mysql_error(MYSQL_WARNING, "query");
        }
        free(msender);
        free(mtext);
    }

    free(cnick);
    free(epass);
    free(cemail);
    free(cgreet);
    free(curl);
}


/*************************************************************************/
void db_mysql_save_ns_alias(NickAlias * na)
{
    char sqlcmd[MAX_SQL_BUF];
    char *nnick, *nlmask, *nlrname, *nlquit, *nncnick;
    nnick = db_mysql_quote(na->nick);
    nlmask = db_mysql_quote(na->last_usermask);
    nlrname = db_mysql_quote(na->last_realname);
    nlquit = db_mysql_quote(na->last_quit);
    nncnick = db_mysql_quote(na->nc->display);
    snprintf(sqlcmd, MAX_SQL_BUF,
             "UPDATE anope_ns_alias SET last_usermask='%s',last_realname='%s',last_quit='%s',time_registered='%d',last_seen='%d',status='%d',display='%s',active='1' WHERE nick='%s'",
             nlmask, nlrname, nlquit, (int) na->time_registered,
             (int) na->last_seen, (int) na->status, nncnick, nnick);
    if (db_mysql_query(sqlcmd)) {
        log_perror("Can't create sql query: %s", sqlcmd);
        db_mysql_error(MYSQL_WARNING, "query");
    }
    /* Our previous UPDATE affected no rows, therefore this is a new record */
    if ((int) mysql_affected_rows(mysql) <= 0) {
        snprintf(sqlcmd, MAX_SQL_BUF,
                 "INSERT INTO anope_ns_alias (nick,last_usermask,last_realname,last_quit,time_registered,last_seen,status,display,active) VALUES ('%s','%s','%s','%s','%d','%d','%d','%s','1')",
                 nnick, nlmask, nlrname, nlquit, (int) na->time_registered,
                 (int) na->last_seen, (int) na->status, nncnick);
        if (db_mysql_query(sqlcmd)) {
            log_perror("Can't create sql query: %s", sqlcmd);
            db_mysql_error(MYSQL_WARNING, "query");
        }
    }

    free(nnick);
    free(nlmask);
    free(nlrname);
    free(nlquit);
    free(nncnick);

    return;
}

/*************************************************************************/

/*
 * ChanServ Specific Secion
 */

/*************************************************************************/
void db_mysql_save_cs_info(ChannelInfo * ci)
{
    char sqlcmd[MAX_SQL_BUF];
    int j, position;
    Memo *memos;
    char *ciname, *cifoundernick, *cisuccessornick, *cifounderpass,
        *cidesc, *ciurl, *ciemail, *cilasttopic, *cilasttopicsetter,
        *ciforbidby, *ciforbidreason, *cimlock_key, *cimlock_flood,
        *cimlock_redirect, *cientrymsg, *cibotnick, *msender, *mtext,
        *ciaccessdisp, *ciakickdisp, *ciakickreason, *ciakickcreator,
        *cbadwords, *efounderpass;

    ciname = db_mysql_quote(ci->name);
    cifoundernick =
        ci->founder ? db_mysql_quote(ci->founder->display) : "";
    cisuccessornick =
        ci->successor ? db_mysql_quote(ci->successor->display) : "";
    cifounderpass = db_mysql_quote(ci->founderpass);
    cidesc = db_mysql_quote(ci->desc);
    ciurl = db_mysql_quote(ci->url);
    ciemail = db_mysql_quote(ci->email);
    cilasttopic = db_mysql_quote(ci->last_topic);
    cilasttopicsetter = db_mysql_quote(ci->last_topic_setter);
    ciforbidby = db_mysql_quote(ci->forbidby);
    ciforbidreason = db_mysql_quote(ci->forbidreason);
    cimlock_key = db_mysql_quote(ci->mlock_key);
#ifdef HAS_FMODE
    cimlock_flood = db_mysql_quote(ci->mlock_flood);
#else
    cimlock_flood = NULL;
#endif
#ifdef HAS_LMODE
    cimlock_redirect = db_mysql_quote(ci->mlock_redirect);
#else
    cimlock_redirect = NULL;
#endif
    cientrymsg = db_mysql_quote(ci->entry_message);
    cibotnick = ci->bi ? db_mysql_quote(ci->bi->nick) : "";

    efounderpass = db_mysql_secure(cifounderpass);
    free(cifounderpass);

    /* Let's take care of the core itself */
    snprintf(sqlcmd, MAX_SQL_BUF,
             "UPDATE anope_cs_info SET founder='%s',successor='%s',founderpass=%s,"
             "descr='%s',url='%s',email='%s',time_registered='%d',last_used='%d',"
             "last_topic='%s',last_topic_setter='%s',last_topic_time='%d',flags='%d',"
             "forbidby='%s',forbidreason='%s',bantype='%d',accesscount='%d',"
             "akickcount='%d',mlock_on='%d',mlock_off='%d',mlock_limit='%d',"
             "mlock_key='%s',mlock_flood='%s',mlock_redirect='%s',entry_message='%s',"
             "memomax='%d',botnick='%s',botflags='%d',ttb='%d',bwcount='%d',"
             "capsmin='%d',capspercent='%d',floodlines='%d',floodsecs='%d',"
             "repeattimes='%d',active='1' WHERE name='%s'",
             cifoundernick,
             cisuccessornick,
             efounderpass, cidesc, ciurl, ciemail,
             (int) ci->time_registered, (int) ci->last_used,
             cilasttopic, cilasttopicsetter,
             (int) ci->last_topic_time, (int) ci->flags,
             ciforbidby, ciforbidreason, (int) ci->bantype,
             (int) ci->accesscount, (int) ci->akickcount,
             (int) ci->mlock_on, (int) ci->mlock_off,
             (int) ci->mlock_limit, cimlock_key,
#ifdef HAS_FMODE
             cimlock_flood,
#else
             "",
#endif
#ifdef HAS_LMODE
             cimlock_redirect,
#else
             "",
#endif
             cientrymsg,
             (int) ci->memos.memomax,
             cibotnick,
             (int) ci->botflags,
             (int) ci->ttb,
             (int) ci->bwcount,
             (int) ci->capsmin,
             (int) ci->capspercent,
             (int) ci->floodlines,
             (int) ci->floodsecs, (int) ci->repeattimes, ciname);
    if (db_mysql_query(sqlcmd)) {
        log_perror("Can't create sql query: %s", sqlcmd);
        db_mysql_error(MYSQL_WARNING, "query");
    }

    /* Our previous UPDATE affected no rows, therefore this is a new record */
    if ((int) mysql_affected_rows(mysql) <= 0) {
        snprintf(sqlcmd, MAX_SQL_BUF,
                 "INSERT DELAYED INTO anope_cs_info (name,founder,successor,founderpass,"
                 "descr,url,email,time_registered,last_used,last_topic,last_topic_setter"
                 ",last_topic_time,flags,forbidby,forbidreason,bantype,accesscount,akickcount"
                 ",mlock_on,mlock_off,mlock_limit,mlock_key,mlock_flood,mlock_redirect,"
                 "entry_message,botnick,botflags,bwcount,capsmin,capspercent,floodlines,"
                 "floodsecs,repeattimes,active) VALUES ('%s','%s','%s',%s,'%s','%s','%s'"
                 ",'%d','%d','%s','%s','%d','%d','%s','%s','%d','%d','%d','%d','%d','%d',"
                 "'%s','%s','%s','%s','%s','%d','%d','%d','%d','%d','%d','%d','1')",
                 ciname,
                 cifoundernick,
                 cisuccessornick,
                 efounderpass, cidesc, ciurl, ciemail,
                 (int) ci->time_registered, (int) ci->last_used,
                 cilasttopic, cilasttopicsetter,
                 (int) ci->last_topic_time, (int) ci->flags,
                 ciforbidby, ciforbidreason, (int) ci->bantype,
                 (int) ci->accesscount, (int) ci->akickcount,
                 (int) ci->mlock_on, (int) ci->mlock_off,
                 (int) ci->mlock_limit, cimlock_key,
#ifdef HAS_FMODE
                 cimlock_flood,
#else
                 "",
#endif
#ifdef HAS_LMODE
                 cimlock_redirect,
#else
                 "",
#endif
                 cientrymsg,
                 cibotnick,
                 (int) ci->botflags,
                 (int) ci->bwcount,
                 (int) ci->capsmin,
                 (int) ci->capspercent,
                 (int) ci->floodlines,
                 (int) ci->floodsecs, (int) ci->repeattimes);
        if (db_mysql_query(sqlcmd)) {
            log_perror("Can't create sql query: %s", sqlcmd);
            db_mysql_error(MYSQL_WARNING, "query");
        }
    }

    /* Memos */
    memos = ci->memos.memos;
    for (j = 0; j < ci->memos.memocount; j++, memos++) {
        msender = db_mysql_quote(memos->sender);
        mtext = db_mysql_quote(memos->text);
        snprintf(sqlcmd, MAX_SQL_BUF,
                 "INSERT DELAYED INTO anope_ms_info (receiver,number,flags,time,sender,text,serv)"
                 " VALUES ('%s','%d','%d','%d','%s','%s','CHAN')",
                 ciname, memos->number, memos->flags,
                 (int) memos->time, msender, mtext);
        if (db_mysql_query(sqlcmd)) {
            log_perror("Can't create sql query: %s", sqlcmd);
            db_mysql_error(MYSQL_WARNING, "query");
        }
        free(msender);
        free(mtext);
    }

    /* Access */
    for (j = 0; j < ci->accesscount; j++) {
        if (ci->access[j].in_use) {
            ciaccessdisp = db_mysql_quote(ci->access[j].nc->display);
            snprintf(sqlcmd, MAX_SQL_BUF,
                     "INSERT DELAYED INTO anope_cs_access (in_use,level,display,channel,last_seen)"
                     " VALUES ('%d','%d','%s','%s','%d')",
                     (int) ci->access[j].in_use, (int) ci->access[j].level,
                     ciaccessdisp, ciname, (int) ci->access[j].last_seen);
            if (db_mysql_query(sqlcmd)) {
                log_perror("Can't create sql query: %s", sqlcmd);
                db_mysql_error(MYSQL_WARNING, "query");
            }
            free(ciaccessdisp);
        }
    }

    /* Levels */
    position = 0;
    for (j = 0; j < CA_SIZE; j++) {
        snprintf(sqlcmd, MAX_SQL_BUF,
                 "INSERT DELAYED INTO anope_cs_levels (channel, position, level) VALUES ('%s','%d','%d')",
                 ciname, position++, (int) ci->levels[j]);
        if (db_mysql_query(sqlcmd)) {
            log_perror("Can't create sql query: %s", sqlcmd);
            db_mysql_error(MYSQL_WARNING, "query");
        }
    }

    /* Akicks */
    for (j = 0; j < ci->akickcount; j++) {
        ciakickdisp =
            ci->akick[j].flags & AK_USED ? ci->akick[j].
            flags & AK_ISNICK ? db_mysql_quote(ci->akick[j].u.nc->
                                               display) :
            db_mysql_quote(ci->akick[j].u.mask) : "";
        ciakickreason =
            ci->akick[j].flags & AK_USED ? db_mysql_quote(ci->akick[j].
                                                          reason) : "";
        ciakickcreator =
            ci->akick[j].flags & AK_USED ? db_mysql_quote(ci->akick[j].
                                                          creator) : "";
        snprintf(sqlcmd, MAX_SQL_BUF,
                 "INSERT DELAYED INTO anope_cs_akicks (channel, flags, dmask, reason, creator,"
                 " addtime) VALUES ('%s','%d','%s','%s','%s','%d')",
                 ciname, (int) ci->akick[j].flags, ciakickdisp,
                 ciakickreason, ciakickcreator,
                 ci->akick[j].flags & AK_USED ? (int) ci->akick[j].
                 addtime : 0);
        if (db_mysql_query(sqlcmd)) {
            log_perror("Can't create sql query: %s", sqlcmd);
            db_mysql_error(MYSQL_WARNING, "query");
        }
        if (ci->akick[j].flags & AK_USED) {
            free(ciakickdisp);
            free(ciakickreason);
            free(ciakickcreator);
        }
    }

    /* Bad Words */
    for (j = 0; j < ci->bwcount; j++) {
        if (ci->badwords[j].in_use) {
            cbadwords = db_mysql_quote(ci->badwords[j].word);
            snprintf(sqlcmd, MAX_SQL_BUF,
                     "INSERT DELAYED INTO anope_cs_badwords (channel, word, type)"
                     " VALUES ('%s','%s','%d')", ciname, cbadwords,
                     (int) ci->badwords[j].type);
            free(cbadwords);
            if (db_mysql_query(sqlcmd)) {
                log_perror("Can't create sql query: %s", sqlcmd);
                db_mysql_error(MYSQL_WARNING, "query");
            }
        }
    }

    free(ciname);
    if (!(ci->flags & CI_VERBOTEN)) {
        free(cifoundernick);
        if (strlen(cisuccessornick) > 0)
            free(cisuccessornick);
        free(efounderpass);
        free(cidesc);
        free(ciurl);
        free(ciemail);
        free(cilasttopic);
        free(cilasttopicsetter);
        free(cimlock_key);
        free(cimlock_flood);
        free(cimlock_redirect);
        free(cientrymsg);
        if (ci->bi)
            free(cibotnick);
    } else {
        free(ciforbidby);
        free(ciforbidreason);
    }

    return;
}

/*************************************************************************/


/*
 * OperServ Specific Section
 */

/*************************************************************************/
void db_mysql_save_os_db(unsigned int maxucnt, unsigned int maxutime,
                         SList * ak, SList * sgl, SList * sql, SList * szl,
                         HostCache * hc)
{
    char sqlcmd[MAX_SQL_BUF];
    Akill *t_ak;
    SXLine *t_sl;
    HostCache *t_hc;
    char *takuser, *takhost, *takby, *takreason, *tslmask, *tslby,
        *tslreason, *thchost;

    int i, j;

    rdb_clear_table("anope_os_core");

    snprintf(sqlcmd, MAX_SQL_BUF,
             "INSERT DELAYED INTO anope_os_core (maxusercnt,maxusertime,akills_count,"
             "sglines_count,sqlines_count,szlines_count) VALUES "
             "('%d','%d','%d','%d','%d','%d')", maxucnt, maxutime,
             ak->count, sgl->count, sql->count, szl->count);
    if (db_mysql_query(sqlcmd)) {
        log_perror("Can't create sql query: %s", sqlcmd);
        db_mysql_error(MYSQL_WARNING, "query");
    }

    /* now the akills saving */
    rdb_clear_table("anope_os_akills");

    j = ak->count;
    for (i = 0; i < j; i++) {
        t_ak = ak->list[i];
        takuser = db_mysql_quote(t_ak->user);
        takhost = db_mysql_quote(t_ak->host);
        takby = db_mysql_quote(t_ak->by);
        takreason = db_mysql_quote(t_ak->reason);
        snprintf(sqlcmd, MAX_SQL_BUF,
                 "INSERT DELAYED INTO anope_os_akills (user,host,xby,reason,seton,expire) VALUES ('%s','%s','%s','%s','%d','%d')",
                 takuser,
                 takhost,
                 takby, takreason, (int) t_ak->seton, (int) t_ak->expires);
        if (db_mysql_query(sqlcmd)) {
            log_perror("Can't create sql query: %s", sqlcmd);
            db_mysql_error(MYSQL_WARNING, "query");
        }
        free(takuser);
        free(takhost);
        free(takby);
        free(takreason);
    }

/* sglines save */
    rdb_clear_table("anope_os_sglines");

    j = sgl->count;
    for (i = 0; i < j; i++) {
        t_sl = sgl->list[i];
        tslmask = db_mysql_quote(t_sl->mask);
        tslby = db_mysql_quote(t_sl->by);
        tslreason = db_mysql_quote(t_sl->reason);
        snprintf(sqlcmd, MAX_SQL_BUF,
                 "INSERT DELAYED INTO anope_os_sglines (mask,xby,reason,seton,expire) VALUES"
                 " ('%s','%s','%s','%d','%d')",
                 tslmask,
                 tslby, tslreason, (int) t_sl->seton, (int) t_sl->expires);
        if (db_mysql_query(sqlcmd)) {
            log_perror("Can't create sql query: %s", sqlcmd);
            db_mysql_error(MYSQL_WARNING, "query");
        }
        free(tslmask);
        free(tslby);
        free(tslreason);
    }

/* sqlines save */
    rdb_clear_table("anope_os_sqlines");

    j = sql->count;
    for (i = 0; i < j; i++) {
        t_sl = sql->list[i];
        tslmask = db_mysql_quote(t_sl->mask);
        tslby = db_mysql_quote(t_sl->by);
        tslreason = db_mysql_quote(t_sl->reason);
        snprintf(sqlcmd, MAX_SQL_BUF,
                 "INSERT DELAYED INTO anope_os_sqlines (mask,xby,reason,seton,expire) VALUES ('%s','%s','%s','%d','%d')",
                 tslmask,
                 tslby, tslreason, (int) t_sl->seton, (int) t_sl->expires);
        if (db_mysql_query(sqlcmd)) {
            log_perror("Can't create sql query: %s", sqlcmd);
            db_mysql_error(MYSQL_WARNING, "query");
        }
        free(tslmask);
        free(tslby);
        free(tslreason);
    }

/* szlines save */
    rdb_clear_table("anope_os_szlines");

    j = szl->count;
    for (i = 0; i < j; i++) {
        t_sl = szl->list[i];
        tslmask = db_mysql_quote(t_sl->mask);
        tslby = db_mysql_quote(t_sl->by);
        tslreason = db_mysql_quote(t_sl->reason);
        snprintf(sqlcmd, MAX_SQL_BUF,
                 "INSERT DELAYED INTO anope_os_szlines (mask,xby,reason,seton,expire) VALUES"
                 " ('%s','%s','%s','%d','%d')",
                 tslmask,
                 tslby, tslreason, (int) t_sl->seton, (int) t_sl->expires);
        if (db_mysql_query(sqlcmd)) {
            log_perror("Can't create sql query: %s", sqlcmd);
            db_mysql_error(MYSQL_WARNING, "query");
        }
        free(tslmask);
        free(tslby);
        free(tslreason);
    }

/* and finally we save hcache */
    rdb_clear_table("anope_os_hcache");
    for (i = 0; i < 1024; i++) {
        for (t_hc = hcache[i]; t_hc; t_hc = t_hc->next) {
            /* Don't save in-progress scans */
            if (t_hc->status < HC_NORMAL)
                continue;
            thchost = db_mysql_quote(t_hc->host);
            snprintf(sqlcmd, MAX_SQL_BUF,
                     "INSERT DELAYED INTO anope_os_hcache (mask,status,used) VALUES ('%s','%d','%d')",
                     thchost, (int) t_hc->status, (int) t_hc->used);
            if (db_mysql_query(sqlcmd)) {
                log_perror("Can't create sql query: %s", sqlcmd);
                db_mysql_error(MYSQL_WARNING, "query");
            }
            free(thchost);
        }
    }

    return;
}

/*************************************************************************/
void db_mysql_save_news(NewsItem * ni)
{
    char sqlcmd[MAX_SQL_BUF];
    char *nitext, *niwho;
    nitext = db_mysql_quote(ni->text);
    niwho = db_mysql_quote(ni->who);
    snprintf(sqlcmd, MAX_SQL_BUF,
             "INSERT DELAYED INTO anope_os_news (type,num,ntext,who,`time`)"
             " VALUES ('%d','%d','%s','%s','%d')",
             ni->type, ni->num, nitext, niwho, (int) ni->time);
    if (db_mysql_query(sqlcmd)) {
        log_perror("Can't create sql query: %s", sqlcmd);
        db_mysql_error(MYSQL_WARNING, "query");
    }
    free(nitext);
    free(niwho);

    return;
}

/*************************************************************************/
void db_mysql_save_exceptions(Exception * e)
{
    char sqlcmd[MAX_SQL_BUF];
    char *emask, *ewho, *ereason;
    emask = db_mysql_quote(e->mask);
    ewho = db_mysql_quote(e->who);
    ereason = db_mysql_quote(e->reason);
    snprintf(sqlcmd, MAX_SQL_BUF,
             "INSERT DELAYED INTO anope_os_exceptions (mask,lim,who,reason,`time`,expires)"
             " VALUES ('%s','%d','%s','%s','%d','%d')",
             emask, e->limit, ewho,
             ereason, (int) e->time, (int) e->expires);
    if (db_mysql_query(sqlcmd)) {
        log_perror("Can't create sql query: %s", sqlcmd);
        db_mysql_error(MYSQL_WARNING, "query");
    }
    free(emask);
    free(ewho);
    free(ereason);
    return;
}

/*************************************************************************/


/*
 * HostServ Specific Section
 */

/*************************************************************************/
/* TODO: Add vident to tables! */
void db_mysql_save_hs_core(HostCore * hc)
{
    char sqlcmd[MAX_SQL_BUF];
    char *hcnick, *hcvident, *hcvhost, *hccreator;
    hcnick = db_mysql_quote(hc->nick);
    hcvident = db_mysql_quote(hc->vIdent);
    hcvhost = db_mysql_quote(hc->vHost);
    hccreator = db_mysql_quote(hc->creator);
    snprintf(sqlcmd, MAX_SQL_BUF,
             "INSERT DELAYED INTO anope_hs_core (nick,vident,vhost,creator,`time`)"
             " VALUES ('%s','%s','%s','%s','%d')",
             hcnick, hcvident, hcvhost, hccreator, (int) hc->time);
    if (db_mysql_query(sqlcmd)) {
        log_perror("Can't create sql query: %s", sqlcmd);
        db_mysql_error(MYSQL_WARNING, "query");
    }
    free(hcnick);
    free(hcvident);
    free(hcvhost);
    free(hccreator);

    return;
}

/*************************************************************************/

/*
 * HostServ Specific Section
 */

/*************************************************************************/
void db_mysql_save_bs_core(BotInfo * bi)
{
    char sqlcmd[MAX_SQL_BUF];
    char *binick, *biuser, *bihost, *bireal;
    binick = db_mysql_quote(bi->nick);
    biuser = db_mysql_quote(bi->user);
    bihost = db_mysql_quote(bi->host);
    bireal = db_mysql_quote(bi->real);
    snprintf(sqlcmd, MAX_SQL_BUF,
             "INSERT DELAYED INTO anope_bs_core (nick,user,host,rname,flags,created"
             ",chancount) VALUES ('%s','%s','%s','%s','%d','%d','%d')",
             binick, biuser,
             bihost, bireal, bi->flags, (int) bi->created, bi->chancount);
    if (db_mysql_query(sqlcmd)) {
        log_perror("Can't create sql query: %s", sqlcmd);
        db_mysql_error(MYSQL_WARNING, "query");
    }
    free(binick);
    free(biuser);
    free(bihost);
    free(bireal);
}

/*************************************************************************/

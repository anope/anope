
/* News functions.
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
#include "pseudo.h"

/*************************************************************************/

int32 nnews = 0;
int32 news_size = 0;
NewsItem *news = NULL;

/*************************************************************************/

/* List of messages for each news type.  This simplifies message sending. */

#define MSG_SYNTAX	0
#define MSG_LIST_HEADER	1
#define MSG_LIST_ENTRY	2
#define MSG_LIST_NONE	3
#define MSG_ADD_SYNTAX	4
#define MSG_ADD_FULL	5
#define MSG_ADDED	6
#define MSG_DEL_SYNTAX	7
#define MSG_DEL_NOT_FOUND 8
#define MSG_DELETED	9
#define MSG_DEL_NONE	10
#define MSG_DELETED_ALL	11
#define MSG_MAX		11

struct newsmsgs {
    int16 type;
    char *name;
    int msgs[MSG_MAX + 1];
};

struct newsmsgs msgarray[] = {
    {NEWS_LOGON, "LOGON",
     {NEWS_LOGON_SYNTAX,
      NEWS_LOGON_LIST_HEADER,
      NEWS_LOGON_LIST_ENTRY,
      NEWS_LOGON_LIST_NONE,
      NEWS_LOGON_ADD_SYNTAX,
      NEWS_LOGON_ADD_FULL,
      NEWS_LOGON_ADDED,
      NEWS_LOGON_DEL_SYNTAX,
      NEWS_LOGON_DEL_NOT_FOUND,
      NEWS_LOGON_DELETED,
      NEWS_LOGON_DEL_NONE,
      NEWS_LOGON_DELETED_ALL}
     },
    {NEWS_OPER, "OPER",
     {NEWS_OPER_SYNTAX,
      NEWS_OPER_LIST_HEADER,
      NEWS_OPER_LIST_ENTRY,
      NEWS_OPER_LIST_NONE,
      NEWS_OPER_ADD_SYNTAX,
      NEWS_OPER_ADD_FULL,
      NEWS_OPER_ADDED,
      NEWS_OPER_DEL_SYNTAX,
      NEWS_OPER_DEL_NOT_FOUND,
      NEWS_OPER_DELETED,
      NEWS_OPER_DEL_NONE,
      NEWS_OPER_DELETED_ALL}
     },
    {NEWS_RANDOM, "RANDOM",
     {NEWS_RANDOM_SYNTAX,
      NEWS_RANDOM_LIST_HEADER,
      NEWS_RANDOM_LIST_ENTRY,
      NEWS_RANDOM_LIST_NONE,
      NEWS_RANDOM_ADD_SYNTAX,
      NEWS_RANDOM_ADD_FULL,
      NEWS_RANDOM_ADDED,
      NEWS_RANDOM_DEL_SYNTAX,
      NEWS_RANDOM_DEL_NOT_FOUND,
      NEWS_RANDOM_DELETED,
      NEWS_RANDOM_DEL_NONE,
      NEWS_RANDOM_DELETED_ALL}
     }
};

static int *findmsgs(int16 type, char **typename)
{
    int i;
    for (i = 0; i < lenof(msgarray); i++) {
        if (msgarray[i].type == type) {
            if (typename)
                *typename = msgarray[i].name;
            return msgarray[i].msgs;
        }
    }
    return NULL;
}

/*************************************************************************/

/* Called by the main OperServ routine in response to a NEWS command. */
static void do_news(User * u, int16 type);

/* Lists all a certain type of news. */
static void do_news_list(User * u, int16 type, int *msgs);

/* Add news items. */
static void do_news_add(User * u, int16 type, int *msgs,
                        const char *typename);
static int add_newsitem(User * u, const char *text, int16 type);

/* Delete news items. */
static void do_news_del(User * u, int16 type, int *msgs,
                        const char *typename);
static int del_newsitem(int num, int16 type);

/*************************************************************************/
/****************************** Statistics *******************************/
/*************************************************************************/

void get_news_stats(long *nrec, long *memuse)
{
    long mem;
    int i;

    mem = sizeof(NewsItem) * news_size;
    for (i = 0; i < nnews; i++)
        mem += strlen(news[i].text) + 1;
    *nrec = nnews;
    *memuse = mem;
}

/*************************************************************************/
/*********************** News item loading/saving ************************/
/*************************************************************************/

#define SAFE(x) do {					\
    if ((x) < 0) {					\
	if (!forceload)					\
	    fatal("Read error on %s", NewsDBName);	\
	nnews = i;					\
	break;						\
    }							\
} while (0)

void load_news()
{
    dbFILE *f;
    int i;
    uint16 n;
    uint32 tmp32;

    if (!(f = open_db(s_OperServ, NewsDBName, "r", NEWS_VERSION)))
        return;
    switch (i = get_file_version(f)) {
    case 9:
    case 8:
    case 7:
        SAFE(read_int16(&n, f));
        nnews = n;
        if (nnews < 8)
            news_size = 16;
        else if (nnews >= 16384)
            news_size = 32767;
        else
            news_size = 2 * nnews;
        news = scalloc(sizeof(*news) * news_size, 1);
        if (!nnews) {
            close_db(f);
            return;
        }
        for (i = 0; i < nnews; i++) {
            SAFE(read_int16(&news[i].type, f));
            SAFE(read_int32(&news[i].num, f));
            SAFE(read_string(&news[i].text, f));
            SAFE(read_buffer(news[i].who, f));
            SAFE(read_int32(&tmp32, f));
            news[i].time = tmp32;
        }
        break;

    default:
        fatal("Unsupported version (%d) on %s", i, NewsDBName);
    }                           /* switch (ver) */

    close_db(f);
}

#undef SAFE

/*************************************************************************/

#define SAFE(x) do {						\
    if ((x) < 0) {						\
	restore_db(f);						\
	log_perror("Write error on %s", NewsDBName);		\
	if (time(NULL) - lastwarn > WarningTimeout) {		\
	    anope_cmd_global(NULL, "Write error on %s: %s", NewsDBName,	\
			strerror(errno));			\
	    lastwarn = time(NULL);				\
	}							\
	return;							\
    }								\
} while (0)

void save_news()
{
    dbFILE *f;
    int i;
    static time_t lastwarn = 0;

    if (!(f = open_db(s_OperServ, NewsDBName, "w", NEWS_VERSION)))
        return;
    SAFE(write_int16(nnews, f));
    for (i = 0; i < nnews; i++) {
        SAFE(write_int16(news[i].type, f));
        SAFE(write_int32(news[i].num, f));
        SAFE(write_string(news[i].text, f));
        SAFE(write_buffer(news[i].who, f));
        SAFE(write_int32(news[i].time, f));
    }
    close_db(f);
}

#undef SAFE

void save_rdb_news()
{
#ifdef USE_RDB
    int i;
    NewsItem *ni;

    if (!rdb_open())
        return;

    if (rdb_tag_table("anope_os_news") == 0) {
        alog("Unable to tag table 'anope_os_news' - News RDB save failed.");
        return;
    }
	
    for (i = 0; i < nnews; i++) {
        ni = &news[i];
        if (rdb_save_news(ni) == 0) {
            alog("Unable to save NewsItem %d - News RDB save failed.", ni->num);
            return;
        }
    }
	
    if (rdb_clean_table("anope_os_news") == 0) {
        alog("Unable to clean table 'anope_os_news' - News RDB save failed.");
        return;
    }
	
    rdb_close();
#endif
}

/*************************************************************************/
/***************************** News display ******************************/
/*************************************************************************/

void display_news(User * u, int16 type)
{
    int msg;

    if (type == NEWS_LOGON) {
        msg = NEWS_LOGON_TEXT;
    } else if (type == NEWS_OPER) {
        msg = NEWS_OPER_TEXT;
    } else if (type == NEWS_RANDOM) {
        msg = NEWS_RANDOM_TEXT;
    } else {
        alog("news: Invalid type (%d) to display_news()", type);
        return;
    }

    if (type == NEWS_RANDOM) {
        static int current_news = -1;
        int count = 0;

        if (!nnews)
            return;

        while (count++ < nnews) {
            if (++current_news >= nnews)
                current_news = 0;

            if (news[current_news].type == type) {
                struct tm *tm;
                char timebuf[64];

                tm = localtime(&news[current_news].time);
                strftime_lang(timebuf, sizeof(timebuf), u,
                              STRFTIME_SHORT_DATE_FORMAT, tm);
                notice_lang(s_GlobalNoticer, u, msg, timebuf,
                            news[current_news].text);

                return;
            }
        }
    } else {
        int i, count = 0;

        for (i = nnews - 1; i >= 0; i--) {
            if (count >= NewsCount)
                break;
            if (news[i].type == type)
                count++;
        }
        while (++i < nnews) {
            if (news[i].type == type) {
                struct tm *tm;
                char timebuf[64];

                tm = localtime(&news[i].time);
                strftime_lang(timebuf, sizeof(timebuf), u,
                              STRFTIME_SHORT_DATE_FORMAT, tm);
                notice_lang(s_GlobalNoticer, u, msg, timebuf,
                            news[i].text);
            }
        }
    }
}

/*************************************************************************/
/***************************** News editing ******************************/
/*************************************************************************/
/* Declared in extern.h */
int do_logonnews(User * u)
{
    do_news(u, NEWS_LOGON);
    return MOD_CONT;
}


/* Declared in extern.h */
int do_opernews(User * u)
{
    do_news(u, NEWS_OPER);
    return MOD_CONT;
}

/* Declared in extern.h */
int do_randomnews(User * u)
{
    do_news(u, NEWS_RANDOM);
    return MOD_CONT;
}

/*************************************************************************/

/* Main news command handling routine. */
void do_news(User * u, short type)
{
    int is_servadmin = is_services_admin(u);
    char *cmd = strtok(NULL, " ");
    char *typename;
    int *msgs;

    msgs = findmsgs(type, &typename);
    if (!msgs) {
        alog("news: Invalid type to do_news()");
        return;
    }

    if (!cmd)
        cmd = "";

    if (stricmp(cmd, "LIST") == 0) {
        do_news_list(u, type, msgs);
    } else if (stricmp(cmd, "ADD") == 0) {
        if (is_servadmin)
            do_news_add(u, type, msgs, typename);
        else
            notice_lang(s_OperServ, u, PERMISSION_DENIED);

    } else if (stricmp(cmd, "DEL") == 0) {
        if (is_servadmin)
            do_news_del(u, type, msgs, typename);
        else
            notice_lang(s_OperServ, u, PERMISSION_DENIED);

    } else {
        char buf[32];
        snprintf(buf, sizeof(buf), "%sNEWS", typename);
        syntax_error(s_OperServ, u, buf, msgs[MSG_SYNTAX]);
    }
}

/*************************************************************************/

/* Handle a {LOGON,OPER}NEWS LIST command. */

static void do_news_list(User * u, int16 type, int *msgs)
{
    int i, count = 0;
    char timebuf[64];
    struct tm *tm;

    for (i = 0; i < nnews; i++) {
        if (news[i].type == type) {
            if (count == 0)
                notice_lang(s_OperServ, u, msgs[MSG_LIST_HEADER]);
            tm = localtime(&news[i].time);
            strftime_lang(timebuf, sizeof(timebuf),
                          u, STRFTIME_DATE_TIME_FORMAT, tm);
            notice_lang(s_OperServ, u, msgs[MSG_LIST_ENTRY],
                        news[i].num, timebuf,
                        *news[i].who ? news[i].who : "<unknown>",
                        news[i].text);
            count++;
        }
    }
    if (count == 0)
        notice_lang(s_OperServ, u, msgs[MSG_LIST_NONE]);
    else {
        notice_lang(s_OperServ, u, END_OF_ANY_LIST, "News");
    }
}

/*************************************************************************/

/* Handle a {LOGON,OPER}NEWS ADD command. */

static void do_news_add(User * u, int16 type, int *msgs,
                        const char *typename)
{
    char *text = strtok(NULL, "");
	int n;

    if (!text) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%sNEWS", typename);
        syntax_error(s_OperServ, u, buf, msgs[MSG_ADD_SYNTAX]);
    } else {
        if (readonly) {
            notice_lang(s_OperServ, u, READ_ONLY_MODE);
			return;
		}
        n = add_newsitem(u, text, type);
        if (n < 0)
            notice_lang(s_OperServ, u, msgs[MSG_ADD_FULL]);
        else
            notice_lang(s_OperServ, u, msgs[MSG_ADDED], n);
    }
}


/* Actually add a news item.  Return the number assigned to the item, or -1
 * if the news list is full (32767 items).
 */

static int add_newsitem(User * u, const char *text, short type)
{
    int i, num;

    if (nnews >= 32767)
        return -1;

    if (nnews >= news_size) {
        if (news_size < 8)
            news_size = 8;
        else
            news_size *= 2;
        news = srealloc(news, sizeof(*news) * news_size);
    }
    num = 0;
    for (i = nnews - 1; i >= 0; i--) {
        if (news[i].type == type) {
            num = news[i].num;
            break;
        }
    }
    news[nnews].type = type;
    news[nnews].num = num + 1;
    news[nnews].text = sstrdup(text);
    news[nnews].time = time(NULL);
    strscpy(news[nnews].who, u->nick, NICKMAX);
    nnews++;
    return num + 1;
}

/*************************************************************************/

/* Handle a {LOGON,OPER}NEWS DEL command. */

static void do_news_del(User * u, int16 type, int *msgs,
                        const char *typename)
{
    char *text = strtok(NULL, " ");
    int i, num;

    if (!text) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%sNEWS", typename);
        syntax_error(s_OperServ, u, buf, msgs[MSG_DEL_SYNTAX]);
    } else {
        if (readonly) {
            notice_lang(s_OperServ, u, READ_ONLY_MODE);
			return;
		}
        if (stricmp(text, "ALL") != 0) {
            num = atoi(text);
            if (num > 0 && del_newsitem(num, type)) {
                notice_lang(s_OperServ, u, msgs[MSG_DELETED], num);
                /* Reset the order - #0000397 */
                for (i = 0; i < nnews; i++) {
                    if ((news[i].type == type) && (news[i].num > num))
                        news[i].num--;
                }
            } else
                notice_lang(s_OperServ, u, msgs[MSG_DEL_NOT_FOUND], num);
        } else {
            if (del_newsitem(0, type))
                notice_lang(s_OperServ, u, msgs[MSG_DELETED_ALL]);
            else
                notice_lang(s_OperServ, u, msgs[MSG_DEL_NONE]);
        }
    }
}


/* Actually delete a news item.  If `num' is 0, delete all news items of
 * the given type.  Returns the number of items deleted.
 */

static int del_newsitem(int num, short type)
{
    int i;
    int count = 0;

    for (i = 0; i < nnews; i++) {
        if (news[i].type == type && (num == 0 || news[i].num == num)) {
            free(news[i].text);
            count++;
            nnews--;
            if (i < nnews)
                memcpy(news + i, news + i + 1,
                       sizeof(*news) * (nnews - i));
            i--;
        }
    }
    return count;
}

/*************************************************************************/

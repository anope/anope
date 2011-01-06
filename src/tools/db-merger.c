/*
 * Anope Database Merger
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 *****
 *
 *   Based on the original IRC, db-merger.c
 *   (C) Copyright 2005-2006, Florian Schulze (Certus)
 *   Based on the original code of Anope, (C) 2003-2005 Anope Team
 *
 */

/*
 * Known issues:
 *    - When merging it is possible that ownership of a channel is transferred 
 *      to another nickgroup if the founder's main nick is dropped and granted
 *      to another user in the other database. This user will then become
 *      founder of the channel.
 *
 *    - The count of channels a nickgroup has registered is not recalculated
 *      after the merge. This means that if a channel was dropped, the owner may
 *      have an incorrect counter and may sooner reach the maximum number of
 *      allowed registered channels.
 *
 * Ideally, this should be rewritten from scratch to only merge after both
 * database sets are fully loaded, however since we will be migrating to
 * another database architecture, this is unlikely to be done for the
 * current "stable" DB architecture.
 *
 *  ~ Viper
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h>

#ifndef _WIN32
#include <unistd.h>
#else
#include "sysconf.h"
#include <windows.h>
#endif

/* Some SUN fixs */
#ifdef __sun
/* Solaris specific code, types that do not exist in Solaris'
 * sys/types.h
 **/
#undef u_int8_t
#undef u_int16_t
#undef u_int32_t
#undef u_int_64_t
#define u_int8_t uint8_t
#define u_int16_t uint16_t
#define u_int32_t uint32_t
#define u_int64_t uint64_t

#ifndef INADDR_NONE
#define INADDR_NONE (-1)
#endif

#endif


/* CONFIGURATION BLOCK */

#define NICK_DB_1      "nick1.db"
#define NICK_DB_2      "nick2.db"
#define NICK_DB_NEW "nick.db"

#define CHAN_DB_1      "chan1.db"
#define CHAN_DB_2      "chan2.db"
#define CHAN_DB_NEW "chan.db"

#define BOT_DB_1        "bot1.db"
#define BOT_DB_2        "bot2.db"
#define BOT_DB_NEW   "bot.db"

#define HOST_DB_1      "hosts1.db"
#define HOST_DB_2      "hosts2.db"
#define HOST_DB_NEW "hosts.db"

/* END OF CONFIGURATION BLOCK */

#ifndef _WIN32
#define C_LBLUE "\033[1;34m"
#define C_NONE "\033[m"
#else
#define C_LBLUE ""
#define C_NONE ""
#endif

#define getc_db(f)		(fgetc((f)->fp))
#define HASH(nick)	((tolower((nick)[0])&31)<<5 | (tolower((nick)[1])&31))
#define HASH2(chan)	((chan)[1] ? ((chan)[1]&31)<<5 | ((chan)[2]&31) : 0)
#define read_buffer(buf,f)	(read_db((f),(buf),sizeof(buf)) == sizeof(buf))
#define write_buffer(buf,f)	(write_db((f),(buf),sizeof(buf)) == sizeof(buf))
#define read_db(f,buf,len)	(fread((buf),1,(len),(f)->fp))
#define write_db(f,buf,len)	(fwrite((buf),1,(len),(f)->fp))
#define read_int8(ret,f)	((*(ret)=fgetc((f)->fp))==EOF ? -1 : 0)
#define write_int8(val,f)	(fputc((val),(f)->fp)==EOF ? -1 : 0)
#define SAFE(x) do { \
    if ((x) < 0) { \
        printf("Error, the database is broken, trying to continue... no guarantee.\n"); \
    } \
} while (0)
#define READ(x) do { \
    if ((x) < 0) { \
        printf("Error, the database is broken, trying to continue... no guarantee.\n"); \
        exit(0); \
    } \
} while (0)

typedef int16_t int16;
typedef u_int16_t uint16;
typedef int32_t int32;
typedef u_int32_t uint32;
typedef struct memo_ Memo;
typedef struct dbFILE_ dbFILE;
typedef struct nickalias_ NickAlias;
typedef struct nickcore_ NickCore;
typedef struct chaninfo_ ChannelInfo;
typedef struct botinfo_ BotInfo;
typedef struct badword_ BadWord;
typedef struct hostcore_ HostCore;

struct memo_ {
    uint32 number;      /* Index number -- not necessarily array position! */
    uint16 flags;            /* Flags */
    time_t time;          /* When was it sent? */
    char sender[32];   /* Name of the sender */
    char *text;
};

struct dbFILE_ {
    int mode;                   /* 'r' for reading, 'w' for writing */
    FILE *fp;                    /* The normal file descriptor */
    char filename[1024];  /* Name of the database file */
};

typedef struct {
    int16 memocount;  /* Current # of memos */
    int16 memomax;    /* Max # of memos one can hold*/
    Memo *memos;     /* Pointer to original memos */
} MemoInfo;

typedef struct {
    uint16 in_use;         /* 1 if this entry is in use, else 0 */
    int16 level;
    NickCore *nc;       /* Guaranteed to be non-NULL if in use, NULL if not */
    time_t last_seen;
} ChanAccess;

typedef struct {
    int16 in_use;         /* Always 0 if not in use */
    int16 is_nick;         /* 1 if a regged nickname, 0 if a nick!user@host mask */
    uint16 flags;
    union {
        char *mask;      /* Guaranteed to be non-NULL if in use, NULL if not */
        NickCore *nc;   /* Same */
    } u;
    char *reason;
    char *creator;
    time_t addtime;
} AutoKick;

struct nickalias_ {
    NickAlias *next, *prev;
    char *nick;                    /* Nickname */
    char *last_quit;             /* Last quit message */
    char *last_realname;     /* Last realname */
    char *last_usermask;     /* Last usermask */
    time_t time_registered;  /* When the nick was registered */
    time_t last_seen;           /* When it was seen online for the last time */
    uint16 status;                  /* See NS_* below */
    NickCore *nc;               /* I'm an alias of this */
};

struct nickcore_ {
    NickCore *next, *prev;

    char *display;           /* How the nick is displayed */
    char pass[32];               /* Password of the nicks */
    char *email;              /* E-mail associated to the nick */
    char *greet;              /* Greet associated to the nick */
    uint32 icq;                 /* ICQ # associated to the nick */
    char *url;                  /* URL associated to the nick */
    uint32 flags;                /* See NI_* below */
    uint16 language;        /* Language selected by nickname owner (LANG_*) */
    uint16 accesscount;     /* # of entries */
    char **access;          /* Array of strings */
    MemoInfo memos;     /* Memo information */
    uint16 channelcount;  /* Number of channels currently registered */
    uint16 channelmax;   /* Maximum number of channels allowed */
    int unused;                /* Used for nick collisions */
    int aliascount;            /* How many aliases link to us? Remove the core if 0 */
};

struct chaninfo_ {
    ChannelInfo *next, *prev;

    char name[64];                     /* Channel name */
    char *founder;                      /* Who registered the channel */
    char *successor;                   /* Who gets the channel if the founder nick is dropped or expires */
    char founderpass[32];            /* Channel password */
    char *desc;                           /* Description */
    char *url;                              /* URL */
    char *email;                          /* Email address */
    time_t time_registered;          /* When was it registered */
    time_t last_used;                   /* When was it used hte last time */
    char *last_topic;                       /* Last topic on the channel */
    char last_topic_setter[32];      /* Who set the last topic */
    time_t last_topic_time;           /* When the last topic was set */
    uint32 flags;                          /* Flags */
    char *forbidby;                      /* if forbidden: who did it */
    char *forbidreason;                /* if forbidden: why */
    int16 bantype;                       /* Bantype */
    int16 *levels;                        /* Access levels for commands */
    uint16 accesscount;                 /* # of pple with access */
    ChanAccess *access;             /* List of authorized users */
    uint16 akickcount;                    /* # of akicked pple */
    AutoKick *akick;                    /* List of users to kickban */
    uint32 mlock_on, mlock_off;   /* See channel modes below */
    uint32 mlock_limit;                /* 0 if no limit */
    char *mlock_key;                  /* NULL if no key */
    char *mlock_flood;                /* NULL if no +f */
    char *mlock_redirect;            /* NULL if no +L */
    char *entry_message;           /* Notice sent on entering channel */
    MemoInfo memos;                 /* Memos */
    char *bi;                               /* Bot used on this channel */
    uint32 botflags;                      /* BS_* below */
    int16 *ttb;                             /* Times to ban for each kicker */
    uint16 bwcount;                       /* Badword count */
    BadWord *badwords;             /* For BADWORDS kicker */
    int16 capsmin, capspercent;    /* For CAPS kicker */
    int16 floodlines, floodsecs;      /* For FLOOD kicker */
    int16 repeattimes;                 /* For REPEAT kicker */
};

struct botinfo_ {
    BotInfo *next, *prev;
    char *nick;            /* Nickname of the bot */
    char *user;            /* Its user name */
    char *host;            /* Its hostname */
    char *real;            /* Its real name */
    int16 flags;            /* Bot flags */
    time_t created;      /* Birth date */
    int16 chancount;	    /* Number of channels that use the bot. */
};

struct badword_ {
    uint16 in_use;
    char *word;
    uint16 type;
};

struct hostcore_ {
    HostCore *next, *last;
    char *nick;        /* Owner of the vHost */
    char *vIdent;    /* vIdent for the user */
    char *vHost;     /* Vhost for this user */
    char *creator;   /* Oper Nick of the oper who set the vhost */
    time_t time;      /* Date/Time vHost was set */
};

dbFILE *open_db_write(const char *service, const char *filename, int version);
dbFILE *open_db_read(const char *service, const char *filename, int version);
NickCore *findcore(const char *nick, int version);
NickAlias *findnick(const char *nick);
BotInfo *findbot(char *nick);
ChannelInfo *cs_findchan(const char *chan);
char *strscpy(char *d, const char *s, size_t len);
int write_file_version(dbFILE * f, uint32 version);
int mystricmp(const char *s1, const char *s2);
int delnick(NickAlias *na, int donttouchthelist);
int write_string(const char *s, dbFILE * f);
int write_ptr(const void *ptr, dbFILE * f);
int read_int16(int16 * ret, dbFILE * f);
int read_int32(int32 * ret, dbFILE * f);
int read_uint16(uint16 * ret, dbFILE * f);
int read_uint32(uint32 * ret, dbFILE * f);
int read_string(char **ret, dbFILE * f);
int write_int16(uint16 val, dbFILE * f);
int write_int32(uint32 val, dbFILE * f);
int read_ptr(void **ret, dbFILE * f);
int delcore(NickCore *nc);
void alpha_insert_chan(ChannelInfo * ci);
void insert_bot(BotInfo * bi);
void close_db(dbFILE * f);

ChannelInfo *chanlists[256];
NickAlias *nalists[1024];
NickCore *nclists[1024];
BotInfo *botlists[256];

int preferfirst = 0, prefersecond = 0, preferoldest = 0, prefernewest = 0;
int nonick = 0, nochan = 0, nobot = 0, nohost = 0;

int main(int argc, char *argv[])
{
    dbFILE *f;
    int i;
    NickCore *nc, *ncnext;
    HostCore *firsthc = NULL;

    printf("\n"C_LBLUE"DB Merger for Anope IRC Services"C_NONE"\n");

    if (argc >= 2) {
        if (!mystricmp(argv[1], "--PREFEROLDEST")) {
            printf("Preferring oldest database entries on collision.\n");
            preferoldest = 1;
        } else if (!mystricmp(argv[1], "--PREFERFIRST")) {
            printf("Preferring first database's entries on collision .\n");
            preferfirst = 1;
        } else if (!mystricmp(argv[1], "--PREFERSECOND")) {
            printf("Preferring second database's entries on collision.\n");
            prefersecond = 1;
        } else if (!mystricmp(argv[1], "--PREFERNEWEST")) {
            printf("Preferring newest database entries on collision.\n");
            prefernewest = 1;
        }
    }

    /* Section I: Nicks */
    /* Ia: First database */
    if ((f = open_db_read("NickServ", NICK_DB_1, 14))) {

        NickAlias *na, **nalast, *naprev;
        NickCore *nc, **nclast, *ncprev;
        int16 tmp16;
        int32 tmp32;
        int i, j, c;

        printf("Trying to merge nicks...\n");

        /* Nick cores */
        for (i = 0; i < 1024; i++) {
            nclast = &nclists[i];
            ncprev = NULL;

            while ((c = getc_db(f)) == 1) {
                if (c != 1) {
                    printf("Invalid format in %s.\n", NICK_DB_1);
                    exit(0);
                }

                nc = calloc(1, sizeof(NickCore));
                nc->aliascount = 0;
                nc->unused = 0;

                *nclast = nc;
                nclast = &nc->next;
                nc->prev = ncprev;
                ncprev = nc;

                READ(read_string(&nc->display, f));
                READ(read_buffer(nc->pass, f));
                READ(read_string(&nc->email, f));
                READ(read_string(&nc->greet, f));
                READ(read_uint32(&nc->icq, f));
                READ(read_string(&nc->url, f));
                READ(read_uint32(&nc->flags, f));
                READ(read_uint16(&nc->language, f));
                READ(read_uint16(&nc->accesscount, f));
                if (nc->accesscount) {
                    char **access;
                    access = calloc(sizeof(char *) * nc->accesscount, 1);
                    nc->access = access;
                    for (j = 0; j < nc->accesscount; j++, access++)
                        READ(read_string(access, f));
                }
                READ(read_int16(&nc->memos.memocount, f));
                READ(read_int16(&nc->memos.memomax, f));
                if (nc->memos.memocount) {
                    Memo *memos;
                    memos = calloc(sizeof(Memo) * nc->memos.memocount, 1);
                    nc->memos.memos = memos;
                    for (j = 0; j < nc->memos.memocount; j++, memos++) {
                        READ(read_uint32(&memos->number, f));
                        READ(read_uint16(&memos->flags, f));
                        READ(read_int32(&tmp32, f));
                        memos->time = tmp32;
                        READ(read_buffer(memos->sender, f));
                        READ(read_string(&memos->text, f));
                    }
                }
                READ(read_uint16(&nc->channelcount, f));
                READ(read_int16(&tmp16, f));
            } /* getc_db() */
            *nclast = NULL;
        } /* for() loop */

        /* Nick aliases */
        for (i = 0; i < 1024; i++) {
            char *s = NULL;

            nalast = &nalists[i];
            naprev = NULL;

            while ((c = getc_db(f)) == 1) {
                if (c != 1) {
                    printf("Invalid format in %s.\n", NICK_DB_1);
                    exit(0);
                }

                na = calloc(1, sizeof(NickAlias));

                READ(read_string(&na->nick, f));
                READ(read_string(&na->last_usermask, f));
                READ(read_string(&na->last_realname, f));
                READ(read_string(&na->last_quit, f));

                READ(read_int32(&tmp32, f));
                na->time_registered = tmp32;
                READ(read_int32(&tmp32, f));
                na->last_seen = tmp32;
                READ(read_uint16(&na->status, f));
                READ(read_string(&s, f));
                na->nc = findcore(s, 0);
                na->nc->aliascount++;
                free(s);

                *nalast = na;
                nalast = &na->next;
                na->prev = naprev;
                naprev = na;
            } /* getc_db() */
            *nalast = NULL;
        } /* for() loop */
        close_db(f); /* End of section Ia */
    } else
        nonick = 1;

    /* Ib: Second database */
    if (!nonick) {
        if ((f = open_db_read("NickServ", NICK_DB_2, 14))) {

            NickAlias *na, *naptr;
            NickCore *nc;
            int16 tmp16;
            int32 tmp32;
            int i, j, index, c;

            /* Nick cores */
            for (i = 0; i < 1024; i++) {

                while ((c = getc_db(f)) == 1) {
                    if (c != 1) {
                        printf("Invalid format in %s.\n", NICK_DB_2);
                        exit(0);
                    }

                    nc = calloc(1, sizeof(NickCore));
                    READ(read_string(&nc->display, f));
                    READ(read_buffer(nc->pass, f));
                    READ(read_string(&nc->email, f));

                    naptr = findnick(nc->display);
                    if (naptr)
                        nc->unused = 1;
                    else
                        nc->unused = 0;

                    nc->aliascount = 0;

                    index = HASH(nc->display);
                    nc->prev = NULL;
                    nc->next = nclists[index];
                    if (nc->next)
                        nc->next->prev = nc;
                    nclists[index] = nc;

                    READ(read_string(&nc->greet, f));
                    READ(read_uint32(&nc->icq, f));
                    READ(read_string(&nc->url, f));
                    READ(read_uint32(&nc->flags, f));
                    READ(read_uint16(&nc->language, f));
                    READ(read_uint16(&nc->accesscount, f));
                    if (nc->accesscount) {
                        char **access;
                        access = calloc(sizeof(char *) * nc->accesscount, 1);
                        nc->access = access;
                        for (j = 0; j < nc->accesscount; j++, access++)
                            READ(read_string(access, f));
                    }
                    READ(read_int16(&nc->memos.memocount, f));
                    READ(read_int16(&nc->memos.memomax, f));
                    if (nc->memos.memocount) {
                        Memo *memos;
                        memos = calloc(sizeof(Memo) * nc->memos.memocount, 1);
                        nc->memos.memos = memos;
                        for (j = 0; j < nc->memos.memocount; j++, memos++) {
                            READ(read_uint32(&memos->number, f));
                            READ(read_uint16(&memos->flags, f));
                            READ(read_int32(&tmp32, f));
                            memos->time = tmp32;
                            READ(read_buffer(memos->sender, f));
                            READ(read_string(&memos->text, f));
                        }
                    }
                    READ(read_uint16(&nc->channelcount, f));
                    READ(read_int16(&tmp16, f));
                } /* getc_db() */
            } /* for() loop */

            /* Nick aliases */
            for (i = 0; i < 1024; i++) {
                char *s = NULL;
                NickAlias *ptr, *prev, *naptr;

                while ((c = getc_db(f)) == 1) {
                    if (c != 1) {
                        printf("Invalid format in %s.\n", NICK_DB_2);
                        exit(0);
                    }

                    na = calloc(1, sizeof(NickAlias));

                    READ(read_string(&na->nick, f));
                    READ(read_string(&na->last_usermask, f));
                    READ(read_string(&na->last_realname, f));
                    READ(read_string(&na->last_quit, f));
                    READ(read_int32(&tmp32, f));
                    na->time_registered = tmp32;
                    READ(read_int32(&tmp32, f));
                    na->last_seen = tmp32;
                    READ(read_uint16(&na->status, f));
                    READ(read_string(&s, f));

                    naptr = findnick(na->nick);
                    if (naptr) { /* COLLISION! na = collision #2 = new (na->nc doesn't exist yet), naptr = collision #1 = old (naptr->nc exists) */
                        char input[1024];
                        NickCore *ncptr = findcore(na->nick, 1); /* Find core for #2, ncptr MUST exist since we've read all cores, if it doesn't eixst, we have a malformed db */;

                        if (!ncptr) { /* malformed */
                            printf("\n\n     WARNING! Malformed database. No nickcore for nick %s, droping it.\n\n\n", na->nick);
                            delnick(na, 1);
                        } else { /* not malformed */
                            if (!preferoldest && !preferfirst && !prefersecond && !prefernewest) {
                                printf("Nick collision for nick %s:\n\n", na->nick);
                                printf("Group 1: %s (%s)\n", naptr->nc->display, naptr->nc->email);
                                printf("Time registered: %s\n", ctime(&naptr->time_registered));
                                printf("Group 2: %s (%s)\n", ncptr->display, ncptr->email);
                                printf("Time registered: %s\n", ctime(&na->time_registered));
                                printf("What group do you want to keep? Enter the related group number \"1\" or \"2\".\n");
                            }

                            if (preferoldest) {
                                input[0] = (na->time_registered > naptr->time_registered) ? '1' : '2';
                            } else if (prefernewest) {
                                input[0] = (na->time_registered > naptr->time_registered) ? '2' : '1';
                            } else if (preferfirst) {
                                input[0] = '1';
                            } else if (prefersecond) {
                                input[0] = '2';
                            } else {
                                waiting_for_input:
                                scanf("%s", input);
                            }
                            if (input[0] == '1') { /* #2 isn't in the list yet, #1 is. free #2 alias. */
                                printf("Deleting nick alias %s (#2).\n", na->nick);
                                delnick(na, 1); /* free()s the alias without touching the list since na isn't in the list */
                            } else if (input[0] == '2') { /* get alias #1 out of the list, then free() it, then add #2 to the list */
                                printf("Deleting nick alias %s (#1).\n", naptr->nick); 
                                naptr->nc->aliascount--; /* tell the core it has one alias less */
                                delnick(naptr, 0); /* removes the alias from the list and free()s it */
                                na->nc = ncptr;
                                na->nc->aliascount++;
                                index = HASH(na->nick);
                                for (prev = NULL, ptr = nalists[index]; ptr && mystricmp(ptr->nick, na->nick) < 0; prev = ptr, ptr = ptr->next);
                                na->prev = prev;
                                na->next = ptr;
                                if (!prev)
                                    nalists[index] = na;
                                else
                                    prev->next = na;
                                if (ptr)
                                    ptr->prev = na;
                            } else {
                                printf("Invalid number, give us a valid one (1 or 2).\n");
                                goto waiting_for_input;
                            }
                        } /* not malformed */
                    } else { /* No collision, add the core pointer and put the alias in the list */
                        na->nc = findcore(s, 0);
                        if (!na->nc) {
                            printf("\n\n     WARNING! Malformed database. No nickcore for nick %s, droping it.\n\n\n", na->nick);
                            delnick(na, 1);
                        } else {
                            na->nc->aliascount++;
                            index = HASH(na->nick);
                            for (prev = NULL, ptr = nalists[index]; ptr && mystricmp(ptr->nick, na->nick) < 0; prev = ptr, ptr = ptr->next);
                            na->prev = prev;
                            na->next = ptr;
                            if (!prev)
                                nalists[index] = na;
                            else
                                prev->next = na;
                            if (ptr)
                                ptr->prev = na;
                        }
                    }
                    free(s);
                } /* getc_db() */
            } /* for() loop */
            close_db(f); /* End of section Ib */   
        } else
            nonick = 1;
    }

    /* CLEAN THE CORES */

    for (i = 0; i < 1024; i++) {
        for (nc = nclists[i]; nc; nc = ncnext) {
            ncnext = nc->next;
            if (nc->aliascount < 1) {
                printf("Deleting core %s (%s).\n", nc->display, nc->email); 
                delcore(nc);
            }
        }
    }

    /* Ic: Saving */
    if (!nonick) {
        if ((f = open_db_write("NickServ", NICK_DB_NEW, 14))) {

            NickAlias *na;
            NickCore *nc;
            char **access;
            Memo *memos;
            int i, j;

            /* Nick cores */
            for (i = 0; i < 1024; i++) {
                for (nc = nclists[i]; nc; nc = nc->next) {
                    SAFE(write_int8(1, f));
                    SAFE(write_string(nc->display, f));
                    SAFE(write_buffer(nc->pass, f));
                    SAFE(write_string(nc->email, f));
                    SAFE(write_string(nc->greet, f));
                    SAFE(write_int32(nc->icq, f));
                    SAFE(write_string(nc->url, f));
                    SAFE(write_int32(nc->flags, f));
                    SAFE(write_int16(nc->language, f));
                    SAFE(write_int16(nc->accesscount, f));
                    for (j = 0, access = nc->access; j < nc->accesscount; j++, access++)
                        SAFE(write_string(*access, f));

                    SAFE(write_int16(nc->memos.memocount, f));
                    SAFE(write_int16(nc->memos.memomax, f));
                    memos = nc->memos.memos;
                    for (j = 0; j < nc->memos.memocount; j++, memos++) {
                        SAFE(write_int32(memos->number, f));
                        SAFE(write_int16(memos->flags, f));
                        SAFE(write_int32(memos->time, f));
                        SAFE(write_buffer(memos->sender, f));
                        SAFE(write_string(memos->text, f));
                    }
                    SAFE(write_int16(nc->channelcount, f));
                    SAFE(write_int16(nc->channelmax, f));
                } /* for (nc) */
                SAFE(write_int8(0, f));
            } /* for (i) */

            /* Nick aliases */
            for (i = 0; i < 1024; i++) {
                for (na = nalists[i]; na; na = na->next) {
                    SAFE(write_int8(1, f));
                    SAFE(write_string(na->nick, f));
                    SAFE(write_string(na->last_usermask, f));
                    SAFE(write_string(na->last_realname, f));
                    SAFE(write_string(na->last_quit, f));
                    SAFE(write_int32(na->time_registered, f));
                    SAFE(write_int32(na->last_seen, f));
                    SAFE(write_int16(na->status, f));
                    SAFE(write_string(na->nc->display, f));

                } /* for (na) */
                SAFE(write_int8(0, f));
            } /* for (i) */
                close_db(f); /* End of section Ic */
                printf("Nick merging done. New database saved as %s.\n", NICK_DB_NEW);
        }
    } /* End of section I */

    /* Section II: Chans */
    /* IIa: First database */
    if ((f = open_db_read("ChanServ", CHAN_DB_1, 16))) {
        ChannelInfo *ci, **last, *prev;
        int c;

        printf("Trying to merge channels...\n");

        for (i = 0; i < 256; i++) {
            int16 tmp16;
            int32 tmp32;
            int n_levels;
            char *s;
            int n_ttb;
			/* Unused variable - why? -GD
            int J;
			*/

            last = &chanlists[i];
            prev = NULL;
        
            while ((c = getc_db(f)) == 1) {
                int j;

                if (c != 1) {
                    printf("Invalid format in %s.\n", CHAN_DB_1);
                    exit(0);
                }

                ci = calloc(sizeof(ChannelInfo), 1);
                *last = ci;
                last = &ci->next;
                ci->prev = prev;
                prev = ci;
                READ(read_buffer(ci->name, f));
                READ(read_string(&ci->founder, f));
                READ(read_string(&ci->successor, f));
                READ(read_buffer(ci->founderpass, f));
                READ(read_string(&ci->desc, f));
                if (!ci->desc)
                    ci->desc = strdup("");
                READ(read_string(&ci->url, f));
                READ(read_string(&ci->email, f));
                READ(read_int32(&tmp32, f));
                ci->time_registered = tmp32;
                READ(read_int32(&tmp32, f));
                ci->last_used = tmp32;
                READ(read_string(&ci->last_topic, f));
                READ(read_buffer(ci->last_topic_setter, f));
                READ(read_int32(&tmp32, f));
                ci->last_topic_time = tmp32;
                READ(read_uint32(&ci->flags, f));
                /* Temporary flags cleanup */
                ci->flags &= ~0x80000000;
                READ(read_string(&ci->forbidby, f));
                READ(read_string(&ci->forbidreason, f));
                READ(read_int16(&tmp16, f));
                ci->bantype = tmp16;
                READ(read_int16(&tmp16, f));
                n_levels = tmp16;
                ci->levels = calloc(36 * sizeof(*ci->levels), 1);
                for (j = 0; j < n_levels; j++) {
                    if (j < 36)
                        READ(read_int16(&ci->levels[j], f));
                    else
                        READ(read_int16(&tmp16, f));
                }
                READ(read_uint16(&ci->accesscount, f));
                if (ci->accesscount) {
                    ci->access = calloc(ci->accesscount, sizeof(ChanAccess));
                    for (j = 0; j < ci->accesscount; j++) {
                        READ(read_uint16(&ci->access[j].in_use, f));
                        if (ci->access[j].in_use) {
                            READ(read_int16(&ci->access[j].level, f));
                            READ(read_string(&s, f));
                            if (s) {
                                ci->access[j].nc = findcore(s, 0);
                                free(s);
                            }
                            if (ci->access[j].nc == NULL)
                                ci->access[j].in_use = 0;
                            READ(read_int32(&tmp32, f));
                            ci->access[j].last_seen = tmp32;
                        }
                    }
                } else {
                    ci->access = NULL;
                }
                READ(read_uint16(&ci->akickcount, f));
                if (ci->akickcount) {
                    ci->akick = calloc(ci->akickcount, sizeof(AutoKick));
                    for (j = 0; j < ci->akickcount; j++) {
                        SAFE(read_uint16(&ci->akick[j].flags, f));
                        if (ci->akick[j].flags & 0x0001) {
                            SAFE(read_string(&s, f));
                            if (ci->akick[j].flags & 0x0002) {
                                ci->akick[j].u.nc = findcore(s, 0);
                                if (!ci->akick[j].u.nc)
                                    ci->akick[j].flags &= ~0x0001;
                                free(s);
                            } else {
                                ci->akick[j].u.mask = s;
                            }
                            SAFE(read_string(&s, f));
                            if (ci->akick[j].flags & 0x0001)
                                ci->akick[j].reason = s;
                            else if (s)
                                free(s);
                            SAFE(read_string(&s, f));
                            if (ci->akick[j].flags & 0x0001) {
                                ci->akick[j].creator = s;
                            } else if (s) {
                                free(s);
                            }
                            SAFE(read_int32(&tmp32, f));
                            if (ci->akick[j].flags & 0x0001)
                                ci->akick[j].addtime = tmp32;
                        }
                    }
                } else {
                    ci->akick = NULL;
                }
                READ(read_uint32(&ci->mlock_on, f));
                READ(read_uint32(&ci->mlock_off, f));
                READ(read_uint32(&ci->mlock_limit, f));
                READ(read_string(&ci->mlock_key, f));
                READ(read_string(&ci->mlock_flood, f));
                READ(read_string(&ci->mlock_redirect, f));
                READ(read_int16(&ci->memos.memocount, f));
                READ(read_int16(&ci->memos.memomax, f));
                if (ci->memos.memocount) {
                    Memo *memos;
                    memos = calloc(sizeof(Memo) * ci->memos.memocount, 1);
                    ci->memos.memos = memos;
                    for (j = 0; j < ci->memos.memocount; j++, memos++) {
                        READ(read_uint32(&memos->number, f));
                        READ(read_uint16(&memos->flags, f));
                        READ(read_int32(&tmp32, f));
                        memos->time = tmp32;
                        READ(read_buffer(memos->sender, f));
                        READ(read_string(&memos->text, f));
                    }
                }
                READ(read_string(&ci->entry_message, f));

                /* BotServ options */
                READ(read_string(&ci->bi, f));
                READ(read_int32(&tmp32, f));
                ci->botflags = tmp32;
                READ(read_int16(&tmp16, f));
                n_ttb = tmp16;
                ci->ttb = calloc(2 * 8, 1);
                for (j = 0; j < n_ttb; j++) {
                    if (j < 8)
                        READ(read_int16(&ci->ttb[j], f));
                    else
                        READ(read_int16(&tmp16, f));
                }
                for (j = n_ttb; j < 8; j++)
                    ci->ttb[j] = 0;
                READ(read_int16(&tmp16, f));
                ci->capsmin = tmp16;
                READ(read_int16(&tmp16, f));
                ci->capspercent = tmp16;
                READ(read_int16(&tmp16, f));
                ci->floodlines = tmp16;
                READ(read_int16(&tmp16, f));
                ci->floodsecs = tmp16;
                READ(read_int16(&tmp16, f));
                ci->repeattimes = tmp16;

                READ(read_uint16(&ci->bwcount, f));
                if (ci->bwcount) {
                    ci->badwords = calloc(ci->bwcount, sizeof(BadWord));
                    for (j = 0; j < ci->bwcount; j++) {
                        SAFE(read_uint16(&ci->badwords[j].in_use, f));
                        if (ci->badwords[j].in_use) {
                            SAFE(read_string(&ci->badwords[j].word, f));
                            SAFE(read_uint16(&ci->badwords[j].type, f));
                        }
                    }
                } else {
                    ci->badwords = NULL;
                }
            } /* getc_db() */
            *last = NULL;
        } /* for() loop */
        close_db(f);
    } else
        nochan = 1;

    /* IIb: Second database */
    if (!nochan) {
        if ((f = open_db_read("ChanServ", CHAN_DB_2, 16))) {
            int c;

            for (i = 0; i < 256; i++) {
                int16 tmp16;
                int32 tmp32;
                int n_levels;
                char *s;
                int n_ttb;
				/* Unused variables - why? -GD
                char input[1024];
                NickAlias *na;
                int J;
				*/

                while ((c = getc_db(f)) == 1) {
                    ChannelInfo *ci = NULL, *ciptr = NULL;
                    int j;

                    if (c != 1) {
                        printf("Invalid format in %s.\n", CHAN_DB_2);
                        exit(0);
                    }

                    ci = calloc(sizeof(ChannelInfo), 1);
                    READ(read_buffer(ci->name, f));
                    READ(read_string(&ci->founder, f));
                    READ(read_string(&ci->successor, f));
                    READ(read_buffer(ci->founderpass, f));
                    READ(read_string(&ci->desc, f));
                    if (!ci->desc)
                        ci->desc = strdup("");
                    READ(read_string(&ci->url, f));
                    READ(read_string(&ci->email, f));
                    READ(read_int32(&tmp32, f));
                    ci->time_registered = tmp32;
                    READ(read_int32(&tmp32, f));
                    ci->last_used = tmp32;
                    READ(read_string(&ci->last_topic, f));
                    READ(read_buffer(ci->last_topic_setter, f));
                    READ(read_int32(&tmp32, f));
                    ci->last_topic_time = tmp32;
                    READ(read_uint32(&ci->flags, f));
                    /* Temporary flags cleanup */
                    ci->flags &= ~0x80000000;
                    READ(read_string(&ci->forbidby, f));
                    READ(read_string(&ci->forbidreason, f));
                    READ(read_int16(&tmp16, f));
                    ci->bantype = tmp16;
                    READ(read_int16(&tmp16, f));
                    n_levels = tmp16;
                    ci->levels = calloc(36 * sizeof(*ci->levels), 1);
                    for (j = 0; j < n_levels; j++) {
                        if (j < 36)
                            READ(read_int16(&ci->levels[j], f));
                        else
                            READ(read_int16(&tmp16, f));
                    }
                    READ(read_uint16(&ci->accesscount, f));
                    if (ci->accesscount) {
                        ci->access = calloc(ci->accesscount, sizeof(ChanAccess));
                        for (j = 0; j < ci->accesscount; j++) {
                            READ(read_uint16(&ci->access[j].in_use, f));
                            if (ci->access[j].in_use) {
                                READ(read_int16(&ci->access[j].level, f));
                                READ(read_string(&s, f));
                                if (s) {
                                    ci->access[j].nc = findcore(s, 0);
                                    free(s);
                                }
                                if (ci->access[j].nc == NULL)
                                    ci->access[j].in_use = 0;
                                READ(read_int32(&tmp32, f));
                                ci->access[j].last_seen = tmp32;
                            }
                        }
                    } else {
                        ci->access = NULL;
                    }
                    READ(read_uint16(&ci->akickcount, f));
                    if (ci->akickcount) {
                        ci->akick = calloc(ci->akickcount, sizeof(AutoKick));
                        for (j = 0; j < ci->akickcount; j++) {
                            SAFE(read_uint16(&ci->akick[j].flags, f));
                            if (ci->akick[j].flags & 0x0001) {
                                SAFE(read_string(&s, f));
                                if (ci->akick[j].flags & 0x0002) {
                                    ci->akick[j].u.nc = findcore(s, 0);
                                    if (!ci->akick[j].u.nc)
                                        ci->akick[j].flags &= ~0x0001;
                                    free(s);
                                } else {
                                    ci->akick[j].u.mask = s;
                                }
                                SAFE(read_string(&s, f));
                                if (ci->akick[j].flags & 0x0001)
                                    ci->akick[j].reason = s;
                                else if (s)
                                    free(s);
                                SAFE(read_string(&s, f));
                                if (ci->akick[j].flags & 0x0001) {
                                    ci->akick[j].creator = s;
                                } else if (s) {
                                    free(s);
                                }
                                SAFE(read_int32(&tmp32, f));
                                if (ci->akick[j].flags & 0x0001)
                                    ci->akick[j].addtime = tmp32;
                            }
                        }
                    } else {
                        ci->akick = NULL;
                    }
                    READ(read_uint32(&ci->mlock_on, f));
                    READ(read_uint32(&ci->mlock_off, f));
                    READ(read_uint32(&ci->mlock_limit, f));
                    READ(read_string(&ci->mlock_key, f));
                    READ(read_string(&ci->mlock_flood, f));
                    READ(read_string(&ci->mlock_redirect, f));
                    READ(read_int16(&ci->memos.memocount, f));
                    READ(read_int16(&ci->memos.memomax, f));
                    if (ci->memos.memocount) {
                        Memo *memos;
                        memos = calloc(sizeof(Memo) * ci->memos.memocount, 1);
                        ci->memos.memos = memos;
                        for (j = 0; j < ci->memos.memocount; j++, memos++) {
                            READ(read_uint32(&memos->number, f));
                            READ(read_uint16(&memos->flags, f));
                            READ(read_int32(&tmp32, f));
                            memos->time = tmp32;
                            READ(read_buffer(memos->sender, f));
                            READ(read_string(&memos->text, f));
                        }
                    }
                    READ(read_string(&ci->entry_message, f));

                    /* BotServ options */
                    READ(read_string(&ci->bi, f));
                    READ(read_int32(&tmp32, f));
                    ci->botflags = tmp32;
                    READ(read_int16(&tmp16, f));
                    n_ttb = tmp16;
                    ci->ttb = calloc(32, 1);
                    for (j = 0; j < n_ttb; j++) {
                        if (j < 8)
                            READ(read_int16(&ci->ttb[j], f));
                        else
                            READ(read_int16(&tmp16, f));
                    }
                    for (j = n_ttb; j < 8; j++)
                        ci->ttb[j] = 0;
                    READ(read_int16(&tmp16, f));
                    ci->capsmin = tmp16;
                    READ(read_int16(&tmp16, f));
                    ci->capspercent = tmp16;
                    READ(read_int16(&tmp16, f));
                    ci->floodlines = tmp16;
                    READ(read_int16(&tmp16, f));
                    ci->floodsecs = tmp16;
                    READ(read_int16(&tmp16, f));
                    ci->repeattimes = tmp16;

                    READ(read_uint16(&ci->bwcount, f));
                    if (ci->bwcount) {
                        ci->badwords = calloc(ci->bwcount, sizeof(BadWord));
                        for (j = 0; j < ci->bwcount; j++) {
                            SAFE(read_uint16(&ci->badwords[j].in_use, f));
                            if (ci->badwords[j].in_use) {
                                SAFE(read_string(&ci->badwords[j].word, f));
                                SAFE(read_uint16(&ci->badwords[j].type, f));
                            }
                        }
                    } else {
                        ci->badwords = NULL;
                    }
                    /* READING DONE */
                    ciptr = cs_findchan(ci->name);
                    if (ciptr) { /* COLLISION! ciptr = old = 1; ci = new = 2*/
                        char input[1024];

                        if (!preferoldest && !preferfirst && !prefersecond && !prefernewest) {
                            printf("Chan collision for channel %s:\n\n", ci->name);

                            printf("Owner of channel 1: %s (%s)\n", (ciptr->founder) ? ciptr->founder : "none", (ciptr->email) ? ciptr->email : "no valid email");
                            printf("Accesscount: %u\n", ciptr->accesscount);
                            if (ciptr->flags & 0x00000080) {
                                printf("Status: Channel is forbidden");
                            } else if (ciptr->flags & 0x00010000) {
                                printf("Status: Channel is suspended");
                            }
                            printf("Time registered: %s\n", ctime(&ciptr->time_registered));

                            printf("Owner of channel 2: %s (%s)\n", (ci->founder) ? ci->founder : "none", (ci->email) ? ci->email : "no valid email");
                            printf("Accesscount: %u\n", ci->accesscount);
                            if (ci->flags & 0x00000080) {
                                printf("Status: Channel is forbidden");
                            } else if (ci->flags & 0x00010000) {
                                printf("Status: Channel is suspended");
                            }
                            printf("Time registered: %s\n", ctime(&ci->time_registered));

                            printf("What channel do you want to keep? Enter the related number \"1\" or \"2\".\n");
                        }

                        if (preferoldest) {
                            input[0] = (ci->time_registered > ciptr->time_registered) ? '1' : '2';
                        } else if (prefernewest) {
                            input[0] = (ci->time_registered > ciptr->time_registered) ? '2' : '1';
                        } else if (preferfirst) {
                            input[0] = '1';
                        } else if (prefersecond) {
                            input[0] = '2';
                        } else {
                            waiting_for_input4:
                            scanf("%s", input);
                        }
                        if (input[0] == '1') { /* #2 isn't in the list yet, #1 is. -> free() #2 [ci] */
                            NickCore *nc = NULL;
                            int i;
                            printf("Deleting chan %s (#2).\n", ci->name);

                            if (ci->founder) {
                                nc = findcore(ci->founder, 0);
                                if (nc)
                                    nc->channelcount--;
                            }
                            free(ci->desc);
                            free(ci->founder);
                            free(ci->successor);
                            if (ci->url)
                                free(ci->url);
                            if (ci->email)
                                free(ci->email);
                            if (ci->last_topic)
                                free(ci->last_topic);
                            if (ci->forbidby)
                                free(ci->forbidby);
                            if (ci->forbidreason)
                                free(ci->forbidreason);
                            if (ci->mlock_key)
                                free(ci->mlock_key);
                            if (ci->mlock_flood)
                                free(ci->mlock_flood);
                            if (ci->mlock_redirect)
                                free(ci->mlock_redirect);
                            if (ci->entry_message)
                                free(ci->entry_message);
                            if (ci->access)
                                free(ci->access);
                            for (i = 0; i < ci->akickcount; i++) {
                                if (!(ci->akick[i].flags & 0x0002) && ci->akick[i].u.mask)
                                    free(ci->akick[i].u.mask);
                                if (ci->akick[i].reason)
                                    free(ci->akick[i].reason);
                                if (ci->akick[i].creator)
                                    free(ci->akick[i].creator);
                            }
                            if (ci->akick)
                                free(ci->akick);
                            if (ci->levels)
                                free(ci->levels);
                            if (ci->memos.memos) {
                                for (i = 0; i < ci->memos.memocount; i++) {
                                    if (ci->memos.memos[i].text)
                                        free(ci->memos.memos[i].text);
                                }
                                free(ci->memos.memos);
                            }
                            if (ci->ttb)
                                free(ci->ttb);
                            for (i = 0; i < ci->bwcount; i++) {
                                if (ci->badwords[i].word)
                                    free(ci->badwords[i].word);
                            }
                            if (ci->badwords)
                                free(ci->badwords);
                            if (ci->bi)
                                free(ci->bi);
                            free(ci);

                        } else if (input[0] == '2') { /* get #1 out of the list, free() it and add #2 to the list */
                            NickCore *nc = NULL;
                            printf("Deleting chan %s (#1).\n", ciptr->name);

                            if (ciptr->next)
                                ciptr->next->prev = ciptr->prev;
                            if (ciptr->prev)
                                ciptr->prev->next = ciptr->next;
                            else
                                chanlists[tolower(ciptr->name[1])] = ciptr->next;

                            if (ciptr->founder) {
                                nc = findcore(ciptr->founder, 0);
                                if (nc)
                                    nc->channelcount--;
                            }
                            free(ciptr->desc);
                            if (ciptr->url)
                                free(ciptr->url);
                            if (ciptr->email)
                                free(ciptr->email);
                            if (ciptr->last_topic)
                                free(ciptr->last_topic);
                            if (ciptr->forbidby)
                                free(ciptr->forbidby);
                            if (ciptr->forbidreason)
                                free(ciptr->forbidreason);
                            if (ciptr->mlock_key)
                                free(ciptr->mlock_key);
                            if (ciptr->mlock_flood)
                                free(ciptr->mlock_flood);
                            if (ciptr->mlock_redirect)
                                free(ciptr->mlock_redirect);
                            if (ciptr->entry_message)
                                free(ciptr->entry_message);
                            if (ciptr->access)
                                free(ciptr->access);
                            for (i = 0; i < ciptr->akickcount; i++) {
                                if (!(ciptr->akick[i].flags & 0x0002) && ciptr->akick[i].u.mask)
                                    free(ciptr->akick[i].u.mask);
                                if (ciptr->akick[i].reason)
                                    free(ciptr->akick[i].reason);
                                if (ciptr->akick[i].creator)
                                    free(ciptr->akick[i].creator);
                            }
                            if (ciptr->akick)
                                free(ciptr->akick);
                            if (ciptr->levels)
                                free(ciptr->levels);
                            if (ciptr->memos.memos) {
                                for (i = 0; i < ciptr->memos.memocount; i++) {
                                    if (ciptr->memos.memos[i].text)
                                        free(ciptr->memos.memos[i].text);
                                }
                                free(ciptr->memos.memos);
                            }
                            if (ciptr->ttb)
                                free(ciptr->ttb);
                            for (i = 0; i < ciptr->bwcount; i++) {
                                if (ciptr->badwords[i].word)
                                    free(ciptr->badwords[i].word);
                            }
                            if (ciptr->badwords)
                                free(ciptr->badwords);
                            free(ciptr);

                            alpha_insert_chan(ci);

                        } else {
                            printf("Invalid number, give us a valid one (1 or 2).\n");
                            goto waiting_for_input4;
                        }
                    } else { /* no collision, put the chan into the list */
                        alpha_insert_chan(ci);
                    }
                } /* getc_db() */
            } /* for() loop */
            close_db(f);
        } else
            nochan = 1;
    }

    /* IIc: Saving */
    if (!nochan) {
        if ((f = open_db_write("ChanServ", CHAN_DB_NEW, 16))) {
            ChannelInfo *ci;
            Memo *memos;
			/* Unused variable - why? -GD
            static time_t lastwarn = 0;
			*/

            for (i = 0; i < 256; i++) {
                int16 tmp16;
                for (ci = chanlists[i]; ci; ci = ci->next) {
                    int j;
                    SAFE(write_int8(1, f));
                    SAFE(write_buffer(ci->name, f));
                    if (ci->founder)
                        SAFE(write_string(ci->founder, f));
                    else
                        SAFE(write_string(NULL, f));
                    if (ci->successor)
                        SAFE(write_string(ci->successor, f));
                    else
                        SAFE(write_string(NULL, f));
                    SAFE(write_buffer(ci->founderpass, f));
                    SAFE(write_string(ci->desc, f));
                    SAFE(write_string(ci->url, f));
                    SAFE(write_string(ci->email, f));
                    SAFE(write_int32(ci->time_registered, f));
                    SAFE(write_int32(ci->last_used, f));
                    SAFE(write_string(ci->last_topic, f));
                    SAFE(write_buffer(ci->last_topic_setter, f));
                    SAFE(write_int32(ci->last_topic_time, f));
                    SAFE(write_int32(ci->flags, f));
                    SAFE(write_string(ci->forbidby, f));
                    SAFE(write_string(ci->forbidreason, f));
                    SAFE(write_int16(ci->bantype, f));
                    tmp16 = 36;
                    SAFE(write_int16(tmp16, f));
                    for (j = 0; j < 36; j++)
                        SAFE(write_int16(ci->levels[j], f));

                    SAFE(write_int16(ci->accesscount, f));
                    for (j = 0; j < ci->accesscount; j++) {
                        SAFE(write_int16(ci->access[j].in_use, f));
                        if (ci->access[j].in_use) {
                            SAFE(write_int16(ci->access[j].level, f));
                            SAFE(write_string(ci->access[j].nc->display, f));
                            SAFE(write_int32(ci->access[j].last_seen, f));
                        }
                    }
                    SAFE(write_int16(ci->akickcount, f));
                    for (j = 0; j < ci->akickcount; j++) {
                        SAFE(write_int16(ci->akick[j].flags, f));
                        if (ci->akick[j].flags & 0x0001) {
                            if (ci->akick[j].flags & 0x0002)
                                SAFE(write_string(ci->akick[j].u.nc->display, f));
                            else
                                SAFE(write_string(ci->akick[j].u.mask, f));
                            SAFE(write_string(ci->akick[j].reason, f));
                            SAFE(write_string(ci->akick[j].creator, f));
                            SAFE(write_int32(ci->akick[j].addtime, f));
                        }
                    }

                    SAFE(write_int32(ci->mlock_on, f));
                    SAFE(write_int32(ci->mlock_off, f));
                    SAFE(write_int32(ci->mlock_limit, f));
                    SAFE(write_string(ci->mlock_key, f));
                    SAFE(write_string(ci->mlock_flood, f));
                    SAFE(write_string(ci->mlock_redirect, f));
                    SAFE(write_int16(ci->memos.memocount, f));
                    SAFE(write_int16(ci->memos.memomax, f));
                    memos = ci->memos.memos;
                    for (j = 0; j < ci->memos.memocount; j++, memos++) {
                        SAFE(write_int32(memos->number, f));
                        SAFE(write_int16(memos->flags, f));
                        SAFE(write_int32(memos->time, f));
                        SAFE(write_buffer(memos->sender, f));
                        SAFE(write_string(memos->text, f));
                    }
                    SAFE(write_string(ci->entry_message, f));
                    if (ci->bi)
                        SAFE(write_string(ci->bi, f));
                    else
                        SAFE(write_string(NULL, f));
                    SAFE(write_int32(ci->botflags, f));
                    tmp16 = 8;
                    SAFE(write_int16(tmp16, f));
                    for (j = 0; j < 8; j++)
                        SAFE(write_int16(ci->ttb[j], f));
                    SAFE(write_int16(ci->capsmin, f));
                    SAFE(write_int16(ci->capspercent, f));
                    SAFE(write_int16(ci->floodlines, f));
                    SAFE(write_int16(ci->floodsecs, f));
                    SAFE(write_int16(ci->repeattimes, f));

                    SAFE(write_int16(ci->bwcount, f));
                    for (j = 0; j < ci->bwcount; j++) {
                        SAFE(write_int16(ci->badwords[j].in_use, f));
                        if (ci->badwords[j].in_use) {
                            SAFE(write_string(ci->badwords[j].word, f));
                            SAFE(write_int16(ci->badwords[j].type, f));
                        }
                    }
                } /* for (chanlists[i]) */
                SAFE(write_int8(0, f));
            } /* for (i) */
            close_db(f);
            printf("Chan merging done. New database saved as %s.\n", CHAN_DB_NEW);
        }
    }

    /* Section III: Bots */
    /* IIIa: First database */
    if ((f = open_db_read("Botserv", BOT_DB_1, 10))) {
        BotInfo *bi;
        int c;
        int32 tmp32;
        int16 tmp16;

        printf("Trying to merge bots...\n");

        while ((c = getc_db(f)) == 1) {
            if (c != 1) {
                printf("Invalid format in %s.\n", BOT_DB_1);
                exit(0);
            }

            bi = calloc(sizeof(BotInfo), 1);
            READ(read_string(&bi->nick, f));
            READ(read_string(&bi->user, f));
            READ(read_string(&bi->host, f));
            READ(read_string(&bi->real, f));
            SAFE(read_int16(&tmp16, f));
            bi->flags = tmp16;
            READ(read_int32(&tmp32, f));
            bi->created = tmp32;
            READ(read_int16(&tmp16, f));
            bi->chancount = tmp16;
            insert_bot(bi);
        }
    } else
        nobot = 1;

    /* IIIb: Second database */
    if (!nobot) {
        if ((f = open_db_read("Botserv", BOT_DB_2, 10))) {
            BotInfo *bi, *biptr;
            int c;
            int32 tmp32;
            int16 tmp16;
            char input[1024];

            while ((c = getc_db(f)) == 1) {
                if (c != 1) {
                    printf("Invalid format in %s.\n", BOT_DB_2);
                    exit(0);
                }

                bi = calloc(sizeof(BotInfo), 1);
                READ(read_string(&bi->nick, f));
                READ(read_string(&bi->user, f));
                READ(read_string(&bi->host, f));
                READ(read_string(&bi->real, f));
                SAFE(read_int16(&tmp16, f));
                bi->flags = tmp16;
                READ(read_int32(&tmp32, f));
                bi->created = tmp32;
                READ(read_int16(&tmp16, f));
                bi->chancount = tmp16;
                biptr = findbot(bi->nick);
                if (biptr) { /* BOT COLLISION! #1 is biptr-> (db1), #2 is bi-> (db2) */
                    if (!preferoldest && !preferfirst && !prefersecond && !prefernewest) {
                        printf("Bot collision for botnick %s:\n\n", biptr->nick);
                        printf("Bot 1: %s@%s (%s) is assigned to %d chans\n", biptr->user, biptr->host, biptr->real, biptr->chancount);
                        printf("Time registered: %s\n", ctime(&biptr->created));
                        printf("Bot 2: %s@%s (%s) is assigned to %d chans\n", bi->user, bi->host, bi->real, bi->chancount);
                        printf("Time registered: %s\n", ctime(&bi->created));
                        printf("What bot do you want to keep? Enter the related bot number \"1\" or \"2\".\n");
                    }

                    if (preferoldest) {
                        input[0] = (biptr->created > bi->created) ? '1' : '2';
                    } else if (prefernewest) {
                        input[0] = (biptr->created > bi->created) ? '2' : '1';
                    } else if (preferfirst) {
                        input[0] = '2';
                    } else if (prefersecond) {
                        input[0] = '1';
                    } else {
                        waiting_for_input3:
                        scanf("%s", input);
                    }
                    if (input[0] == '1') { /* free() bot #2 (bi) */
                        printf("Deleting Bot %s!%s@%s (%s) (#2).\n", bi->nick, bi->user, bi->host, bi->real);
                        free(bi->nick);
                        free(bi->user);
                        free(bi->host);
                        free(bi->real);
                        free(bi);
                    } else if (input[0] == '2') { /* get bot #1 (biptr) out of the list, free() it and add #2 to the list */
                        printf("Deleting Bot %s!%s@%s (%s) (#1).\n", biptr->nick, biptr->user, biptr->host, biptr->real);
                        if (biptr->next)
                            biptr->next->prev = biptr->prev;
                        if (biptr->prev)
                            biptr->prev->next = biptr->next;
                        else
                            botlists[tolower(*biptr->nick)] = biptr->next;
                        free(biptr->nick);
                        free(biptr->user);
                        free(biptr->host);
                        free(biptr->real);
                        free(biptr);
                        insert_bot(bi);
                    } else {
                        printf("Invalid number, give us a valid one (1 or 2).\n");
                        goto waiting_for_input3;
                    }
                } /* NO COLLISION (biptr) */
                else
                    insert_bot(bi);
            }
        } else
            nobot = 1;
    }

    /* IIIc: Saving */
    if (!nobot) {
        if ((f = open_db_write("Botserv", BOT_DB_NEW, 10))) {
            BotInfo *bi;
            for (i = 0; i < 256; i++) {
                for (bi = botlists[i]; bi; bi = bi->next) {
                    SAFE(write_int8(1, f));
                    SAFE(write_string(bi->nick, f));
                    SAFE(write_string(bi->user, f));
                    SAFE(write_string(bi->host, f));
                    SAFE(write_string(bi->real, f));
                    SAFE(write_int16(bi->flags, f));
                    SAFE(write_int32(bi->created, f));
                    SAFE(write_int16(bi->chancount, f));
                }
            }
            SAFE(write_int8(0, f));
            close_db(f);
            printf("Bot merging done. New database saved as %s.\n", BOT_DB_NEW);
        }
    } /* End of section III */

    /* Section IV: Hosts */
    /* IVa: First database */
    if ((f = open_db_read("HostServ", HOST_DB_1, 3))) {
        HostCore *hc;
        int c;
        int32 tmp32;

        printf("Trying to merge hosts...\n");

        while ((c = getc_db(f)) == 1) {
            if (c != 1) {
                printf("Invalid format in %s.\n", HOST_DB_1);
                exit(0);
            }
            hc = calloc(1, sizeof(HostCore));
            READ(read_string(&hc->nick, f));
            READ(read_string(&hc->vIdent, f));
            READ(read_string(&hc->vHost, f));
            READ(read_string(&hc->creator, f));
            READ(read_int32(&tmp32, f));
            hc->time = tmp32;
            hc->next = firsthc;
            if (firsthc)
                firsthc->last = hc;
            hc->last = NULL;
            firsthc = hc;
        }
    } else
        nohost = 1;

    /* IVb: Second database */
    if (!nohost) {
        if ((f = open_db_read("HostServ", HOST_DB_2, 3))) {
            HostCore *hc, *hcptr;
            char input[1024];
            int32 tmp32;
            int c;
            int collision = 0;

            while ((c = getc_db(f)) == 1) {
                if (c != 1) {
                    printf("Invalid format in %s.\n", HOST_DB_2);
                    exit(0);
                }
                collision = 0;
                hc = calloc(1, sizeof(HostCore));
                READ(read_string(&hc->nick, f));
                READ(read_string(&hc->vIdent, f));
                READ(read_string(&hc->vHost, f));
                READ(read_string(&hc->creator, f));
                READ(read_int32(&tmp32, f));
                hc->time = tmp32;

                for (hcptr = firsthc; hcptr; hcptr = hcptr->next) {
                    if (!mystricmp(hcptr->nick, hc->nick)) { /* COLLISION: #1 is from db1 (hcptr), #2 is from db2 (hc) */
                        collision++;
                        if (!preferoldest && !preferfirst && !prefersecond && !prefernewest) {
                            printf("vHost collision for nick %s:\n\n", hcptr->nick);
                            printf("vHost 1: %s (vIdent: %s)\n", hcptr->vHost, (hcptr->vIdent) ? hcptr->vIdent : "none");
                            printf("Time set: %s\n", ctime(&hcptr->time));
                            printf("vHost 2: %s (vIdent: %s)\n", hc->vHost, (hc->vIdent) ? hc->vIdent : "none");
                            printf("Time set: %s\n", ctime(&hc->time));
                            printf("What vhost do you want to keep? Enter the related number \"1\" or \"2\".\n");
                        }
                        if (preferoldest) {
                            input[0] = (hcptr->time > hc->time) ? '1' : '2';
                        } else if (prefernewest) {
                            input[0] = (hcptr->time > hc->time) ? '2' : '1';
                        } else if (preferfirst) {
                            input[0] = '1';
                        } else if (prefersecond) {
                            input[0] = '2';
                        } else {
                            waiting_for_input2:
                            scanf("%s", input);
                        }
                        if (input[0] == '2') { /* free() hcptr and get it out of the list, put hc into the list */
                            printf("Deleting vHost %s (vIdent: %s) for nick %s (#1).\n", hcptr->vHost, (hcptr->vIdent) ? hcptr->vIdent : "none", hcptr->nick);
                            free(hcptr->nick);
                            hcptr->nick = hc->nick;
                            free(hcptr->vHost);
                            hcptr->vHost = hc->vHost;
                            free(hcptr->vIdent);
                            hcptr->vIdent = hc->vIdent;
                            free(hcptr->creator);
                            hcptr->creator = hc->creator;
                            hcptr->time = hc->time;
                            free(hc);
                        } else if (input[0] == '1') { /* free() hc */
                            printf("Deleting vHost %s (vIdent: %s) for nick %s (#2).\n", hc->vHost, (hc->vIdent) ? hc->vIdent : "none", hc->nick);
                            free(hc->nick);
                            free(hc->vHost);
                            free(hc->vIdent);
                            free(hc->creator);
                            free(hc);
                        } else {
                            printf("Invalid number, give us a valid one (1 or 2).\n");
                            goto waiting_for_input2;
                        } /* input[0] */
                    } /* mystricmp */
                } /* for (hcptr...) */
                if (!collision) { /* No collision */
                    hc->next = firsthc;
                    if (firsthc)
                        firsthc->last = hc;
                    hc->last = NULL;
                    firsthc = hc;
                }
            } /* while */
        } else
            nohost = 1;
    }

    /* IVc: Saving */
    if (!nohost) {
        if ((f = open_db_write("HostServ", HOST_DB_NEW, 3))) {
            HostCore *hcptr;
            for (hcptr = firsthc; hcptr; hcptr = hcptr->next) {
                SAFE(write_int8(1, f));
                SAFE(write_string(hcptr->nick, f));
                SAFE(write_string(hcptr->vIdent, f));
                SAFE(write_string(hcptr->vHost, f));
                SAFE(write_string(hcptr->creator, f));
                SAFE(write_int32(hcptr->time, f));
            }
            SAFE(write_int8(0, f));
            close_db(f);
            printf("Host merging done. New database saved as %s.\n", HOST_DB_NEW);
        }
    } /* End of section IV */

    /* MERGING DONE \o/ HURRAY! */

    printf("\n\nMerging is now done. I give NO guarantee for your DBs.\n");
	return 0;
} /* End of main() */

/* Open a database file for reading and check for the version */
dbFILE *open_db_read(const char *service, const char *filename, int version)
{
    dbFILE *f;
    FILE *fp;
    int myversion;

    f = calloc(sizeof(*f), 1);
    if (!f) {
        printf("Can't allocate memory for %s database %s.\n", service, filename);
        exit(0);
    }
    strscpy(f->filename, filename, sizeof(f->filename));
    f->mode = 'r';
    fp = fopen(f->filename, "rb");
    if (!fp) {
        printf("Can't read %s database %s.\n", service, f->filename);
        free(f);
        return NULL;
    }
    f->fp = fp;
    myversion = fgetc(fp) << 24 | fgetc(fp) << 16 | fgetc(fp) << 8 | fgetc(fp);
    if (feof(fp)) {
        printf("Error reading version number on %s: End of file detected.\n", f->filename);
        exit(0);
    } else if (myversion < version) {
        printf("Unsuported database version (%d) on %s.\n", myversion, f->filename);
        exit(0);
    }
    return f;
}

/* Open a database file for reading and check for the version */
dbFILE *open_db_write(const char *service, const char *filename, int version)
{
    dbFILE *f;
    int fd;

    f = calloc(sizeof(*f), 1);
    if (!f) {
        printf("Can't allocate memory for %s database %s.\n", service, filename);
        exit(0);
    }
    strscpy(f->filename, filename, sizeof(f->filename));
    filename = f->filename;
#ifndef _WIN32
    unlink(filename);
#else
    DeleteFile(filename);
#endif
    f->mode = 'w';
#ifndef _WIN32
    fd = open(filename, O_WRONLY | O_CREAT | O_EXCL, 0666);
#else
    fd = open(filename, O_WRONLY | O_CREAT | O_EXCL | _O_BINARY, 0666);
#endif
    f->fp = fdopen(fd, "wb");   /* will fail and return NULL if fd < 0 */
    if (!f->fp || !write_file_version(f, version)) {
        printf("Can't write to %s database %s.\n", service, filename);
        if (f->fp) {
            fclose(f->fp);
#ifndef _WIN32
            unlink(filename);
#else
            DeleteFile(filename);
#endif
        }
        free(f);
        return NULL;
    }
    return f;
}

/* Close it */
void close_db(dbFILE * f)
{
    fclose(f->fp);
    free(f);
}

int read_int16(int16 * ret, dbFILE * f)
{
    int c1, c2;

    c1 = fgetc(f->fp);
    c2 = fgetc(f->fp);
    if (c1 == EOF || c2 == EOF)
        return -1;
    *ret = c1 << 8 | c2;
    return 0;
}

int read_uint16(uint16 * ret, dbFILE * f)
{
    int c1, c2;

    c1 = fgetc(f->fp);
    c2 = fgetc(f->fp);
    if (c1 == EOF || c2 == EOF)
        return -1;
    *ret = c1 << 8 | c2;
    return 0;
}


int write_int16(uint16 val, dbFILE * f)
{
    if (fputc((val >> 8) & 0xFF, f->fp) == EOF
        || fputc(val & 0xFF, f->fp) == EOF)
        return -1;
    return 0;
}

int read_int32(int32 * ret, dbFILE * f)
{
    int c1, c2, c3, c4;

    c1 = fgetc(f->fp);
    c2 = fgetc(f->fp);
    c3 = fgetc(f->fp);
    c4 = fgetc(f->fp);
    if (c1 == EOF || c2 == EOF || c3 == EOF || c4 == EOF)
        return -1;
    *ret = c1 << 24 | c2 << 16 | c3 << 8 | c4;
    return 0;
}

int read_uint32(uint32 * ret, dbFILE * f)
{
    int c1, c2, c3, c4;

    c1 = fgetc(f->fp);
    c2 = fgetc(f->fp);
    c3 = fgetc(f->fp);
    c4 = fgetc(f->fp);
    if (c1 == EOF || c2 == EOF || c3 == EOF || c4 == EOF)
        return -1;
    *ret = c1 << 24 | c2 << 16 | c3 << 8 | c4;
    return 0;
}

int write_int32(uint32 val, dbFILE * f)
{
    if (fputc((val >> 24) & 0xFF, f->fp) == EOF)
        return -1;
    if (fputc((val >> 16) & 0xFF, f->fp) == EOF)
        return -1;
    if (fputc((val >> 8) & 0xFF, f->fp) == EOF)
        return -1;
    if (fputc((val) & 0xFF, f->fp) == EOF)
        return -1;
    return 0;
}


int read_ptr(void **ret, dbFILE * f)
{
    int c;

    c = fgetc(f->fp);
    if (c == EOF)
        return -1;
    *ret = (c ? (void *) 1 : (void *) 0);
    return 0;
}

int write_ptr(const void *ptr, dbFILE * f)
{
    if (fputc(ptr ? 1 : 0, f->fp) == EOF)
        return -1;
    return 0;
}


int read_string(char **ret, dbFILE * f)
{
    char *s;
    uint16 len;

    if (read_uint16(&len, f) < 0)
        return -1;
    if (len == 0) {
        *ret = NULL;
        return 0;
    }
    s = calloc(len, 1);
    if (len != fread(s, 1, len, f->fp)) {
        free(s);
        return -1;
    }
    *ret = s;
    return 0;
}

int write_string(const char *s, dbFILE * f)
{
    uint32 len;

    if (!s)
        return write_int16(0, f);
    len = strlen(s);
    if (len > 65534)
        len = 65534;
    if (write_int16((uint16) (len + 1), f) < 0)
        return -1;
    if (len > 0 && fwrite(s, 1, len, f->fp) != len)
        return -1;
    if (fputc(0, f->fp) == EOF)
        return -1;
    return 0;
}

NickCore *findcore(const char *nick, int unused)
{
    NickCore *nc;

    for (nc = nclists[HASH(nick)]; nc; nc = nc->next) {
        if (!mystricmp(nc->display, nick))
            if ((nc->unused && unused) || (!nc->unused && !unused))
                return nc;
    }

    return NULL;
}

NickAlias *findnick(const char *nick)
{
    NickAlias *na;

    for (na = nalists[HASH(nick)]; na; na = na->next) {
        if (!mystricmp(na->nick, nick))
            return na;
    }

    return NULL;
}

int write_file_version(dbFILE * f, uint32 version)
{
    FILE *fp = f->fp;
    if (fputc(version >> 24 & 0xFF, fp) < 0 ||
        fputc(version >> 16 & 0xFF, fp) < 0 ||
        fputc(version >> 8 & 0xFF, fp) < 0 ||
        fputc(version & 0xFF, fp) < 0) {
            printf("Error writing version number on %s.\n", f->filename);
            exit(0);
    }
    return 1;
}

/* strscpy:  Copy at most len-1 characters from a string to a buffer, and
 *           add a null terminator after the last character copied.
 */

char *strscpy(char *d, const char *s, size_t len)
{
    char *d_orig = d;

    if (!len)
        return d;
    while (--len && (*d++ = *s++));
    *d = '\0';
    return d_orig;
}

int mystricmp(const char *s1, const char *s2)
{
    register int c;

    while ((c = tolower(*s1)) == tolower(*s2)) {
        if (c == 0)
            return 0;
        s1++;
        s2++;
    }
    if (c < tolower(*s2))
        return -1;
    return 1;
}

int delnick(NickAlias *na, int donttouchthelist)
{
    if (!donttouchthelist) {
        /* Remove us from the aliases list */
        if (na->next)
            na->next->prev = na->prev;
        if (na->prev)
            na->prev->next = na->next;
        else
            nalists[HASH(na->nick)] = na->next;
    }

    /* free() us */
    free(na->nick);
    if (na->last_usermask)
        free(na->last_usermask);
    if (na->last_realname)
        free(na->last_realname);
    if (na->last_quit)
        free(na->last_quit);
    free(na);
    return 1;
}

int delcore(NickCore *nc)
{
    int i;
    /* Remove the core from the list */
    if (nc->next)
        nc->next->prev = nc->prev;
    if (nc->prev)
        nc->prev->next = nc->next;
    else
        nclists[HASH(nc->display)] = nc->next;

    free(nc->display);
    if (nc->email)
        free(nc->email);
    if (nc->greet)
        free(nc->greet);
    if (nc->url)
        free(nc->url);
    if (nc->access) {
        for (i = 0; i < nc->accesscount; i++) {
            if (nc->access[i])
                free(nc->access[i]);
        }
        free(nc->access);
    }
    if (nc->memos.memos) {
        for (i = 0; i < nc->memos.memocount; i++) {
            if (nc->memos.memos[i].text)
                free(nc->memos.memos[i].text);
        }
        free(nc->memos.memos);
    }
    free(nc);
    return 1;
}

void insert_bot(BotInfo * bi)
{
    BotInfo *ptr, *prev;

    for (prev = NULL, ptr = botlists[tolower(*bi->nick)];
         ptr != NULL && mystricmp(ptr->nick, bi->nick) < 0;
         prev = ptr, ptr = ptr->next);
    bi->prev = prev;
    bi->next = ptr;
    if (!prev)
        botlists[tolower(*bi->nick)] = bi;
    else
        prev->next = bi;
    if (ptr)
        ptr->prev = bi;
}

BotInfo *findbot(char *nick)
{
    BotInfo *bi;

    for (bi = botlists[tolower(*nick)]; bi; bi = bi->next)
        if (!mystricmp(nick, bi->nick))
            return bi;

    return NULL;
}

ChannelInfo *cs_findchan(const char *chan)
{
    ChannelInfo *ci;
    for (ci = chanlists[tolower(chan[1])]; ci; ci = ci->next) {
        if (!mystricmp(ci->name, chan))
            return ci;
    }
    return NULL;
}

void alpha_insert_chan(ChannelInfo * ci)
{
    ChannelInfo *ptr, *prev;
    char *chan = ci->name;

    for (prev = NULL, ptr = chanlists[tolower(chan[1])];
         ptr != NULL && mystricmp(ptr->name, chan) < 0;
         prev = ptr, ptr = ptr->next);
    ci->prev = prev;
    ci->next = ptr;
    if (!prev)
        chanlists[tolower(chan[1])] = ci;
    else
        prev->next = ci;
    if (ptr)
        ptr->prev = ci;
}


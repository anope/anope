/* Database converters.
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
#include "datafiles.h"

/* Each converter will try to convert the databases to the format used
 * by ircservices-4.3 (that is compatible with Epona). 
 */

#ifdef USE_CONVERTER

/*************************************************************************/

#define IS43_VERSION 7

#define SAFER(x) do {					\
    if ((x) < 0) {					\
		fatal("Read error on %s", rdb);	\
		return 0;						\
    }							\
} while (0)

#define SAFEW(x) do {						\
    if ((x) < 0) {						\
		fatal("Write error on %s", wdb);		\
		return 0;					\
	}							\
} while (0)

/*************************************************************************/
/*****************************ircservices-4.4*****************************/
/*************************************************************************/

#ifdef IS44_CONVERTER

#define IS44_VERSION	8
#define IS44_CA_SIZE    13

int convert_ircservices_44(void)
{
    dbFILE *f, *g;              /* f for reading, g for writing */
    int c, i, j;
    char *rdb, *wdb;

    char *tmp;
    int16 tmp16;
    int32 tmp32;

    char nick[NICKMAX];
    char pass[PASSMAX];
    char chan[CHANMAX];

    /* NickServ database */

    if (rename("nick.db", "nick.db.old") == -1) {
        fatal("Converter: unable to rename nick.db to nick.db.old: %s",
              strerror(errno));
        return 0;
    }

    if (!(f = open_db(s_NickServ, "nick.db.old", "r", IS44_VERSION))) {
        fatal("Converter: unable to open nick.db.old in read access: %s",
              strerror(errno));
        return 0;
    }

    if (!(g = open_db(s_NickServ, NickDBName, "w", IS43_VERSION))) {
        fatal("Converter: unable to open %s in write access: %s",
              NickDBName, strerror(errno));
        return 0;
    }

    rdb = sstrdup("nick.db.old");
    wdb = sstrdup(NickDBName);

    get_file_version(f);

    for (i = 0; i < 256; i++) {
        while ((c = getc_db(f)) == 1) {
            if (c != 1)
                fatal("Invalid format in %s", wdb);

            SAFEW(write_int8(1, g));

            SAFER(read_buffer(nick, f));
            SAFEW(write_buffer(nick, g));
            SAFER(read_buffer(pass, f));
            SAFEW(write_buffer(pass, g));

            SAFER(read_string(&tmp, f));
            SAFEW(write_string(tmp, g));
            if (tmp)
                free(tmp);
            SAFER(read_string(&tmp, f));
            SAFEW(write_string(tmp, g));
            if (tmp)
                free(tmp);

            SAFER(read_string(&tmp, f));
            SAFEW(write_string(tmp, g));
            if (tmp)
                free(tmp);
            SAFER(read_string(&tmp, f));
            SAFEW(write_string(tmp, g));
            if (tmp)
                free(tmp);
            SAFER(read_string(&tmp, f));
            SAFEW(write_string(tmp, g));
            if (tmp)
                free(tmp);
            SAFER(read_int32(&tmp32, f));
            SAFEW(write_int32(tmp32, g));
            SAFER(read_int32(&tmp32, f));
            SAFEW(write_int32(tmp32, g));

            SAFER(read_int16(&tmp16, f));
            SAFEW(write_int16(tmp16, g));

            SAFER(read_string(&tmp, f));
            SAFEW(write_string(tmp, g));

            SAFER(read_int16(&tmp16, f));
            SAFEW(write_int16(tmp16, g));

            if (tmp) {
                if (tmp)
                    free(tmp);
                SAFER(read_int16(&tmp16, f));
                SAFEW(write_int16(tmp16, g));
            } else {
                int32 flags;
                int16 memocount, accesscount;

                SAFER(read_int32(&flags, f));
                tmp32 = (flags & ~0x10000000);
                SAFEW(write_int32(tmp32, g));

                /* Suspend stuff that we don't convert */
                if (flags & 0x10000000) {
                    SAFER(read_buffer(nick, f));
                    SAFER(read_string(&tmp, f));
                    if (tmp)
                        free(tmp);
                    SAFER(read_int32(&tmp32, f));
                    SAFER(read_int32(&tmp32, f));
                }

                SAFER(read_int16(&tmp16, f));
                accesscount = tmp16;
                SAFEW(write_int16(tmp16, g));

                if (accesscount) {
                    for (j = 0; j < accesscount; j++) {
                        SAFER(read_string(&tmp, f));
                        SAFEW(write_string(tmp, g));
                        if (tmp)
                            free(tmp);
                    }
                }

                SAFER(read_int16(&memocount, f));
                SAFEW(write_int16(memocount, g));
                SAFER(read_int16(&tmp16, f));
                SAFEW(write_int16(tmp16, g));

                if (memocount) {
                    for (j = 0; j < memocount; j++) {
                        SAFER(read_int32(&tmp32, f));
                        SAFEW(write_int32(tmp32, g));
                        SAFER(read_int16(&tmp16, f));
                        SAFEW(write_int16(tmp16, g));
                        SAFER(read_int32(&tmp32, f));
                        SAFEW(write_int32(tmp32, g));
                        SAFER(read_buffer(nick, f));
                        SAFEW(write_buffer(nick, g));
                        SAFER(read_string(&tmp, f));
                        SAFEW(write_string(tmp, g));
                        if (tmp)
                            free(tmp);
                    }
                }

                SAFER(read_int16(&tmp16, f));
                SAFEW(write_int16(tmp16, g));
                SAFER(read_int16(&tmp16, f));
                SAFEW(write_int16(tmp16, g));

                SAFER(read_int16(&tmp16, f));
                SAFEW(write_int16(tmp16, g));
            }
        }                       /* while (getc_db(f) != 0) */
        SAFEW(write_int8(0, g));
    }                           /* for (i) */

    close_db(f);
    close_db(g);

    free(rdb);
    free(wdb);

    /* ChanServ */

    rdb = sstrdup("chan.db.old");
    wdb = sstrdup(ChanDBName);

    if (rename("chan.db", rdb) == -1) {
        fatal("Converter: unable to rename chan.db to %s: %s", rdb,
              strerror(errno));
        return 0;
    }

    if (!(f = open_db(s_ChanServ, rdb, "r", IS44_VERSION))) {
        fatal("Converter: unable to open %s in read access: %s", rdb,
              strerror(errno));
        return 0;
    }

    if (!(g = open_db(s_ChanServ, wdb, "w", IS43_VERSION))) {
        fatal("Converter: unable to open %s in write access: %s", wdb,
              strerror(errno));
        return 0;
    }

    get_file_version(f);

    for (i = 0; i < 256; i++) {
        while ((c = getc_db(f)) != 0) {
            int16 n_levels, accesscount, akickcount, memocount;

            if (c != 1)
                fatal("Invalid format in %s", wdb);

            SAFEW(write_int8(1, g));

            SAFER(read_buffer(chan, f));
            SAFEW(write_buffer(chan, g));

            SAFER(read_string(&tmp, f));
            SAFEW(write_string(tmp, g));
            if (tmp)
                free(tmp);
            SAFER(read_string(&tmp, f));
            SAFEW(write_string(tmp, g));
            if (tmp)
                free(tmp);

            SAFER(read_buffer(pass, f));
            SAFEW(write_buffer(pass, g));
            SAFER(read_string(&tmp, f));
            SAFEW(write_string(tmp, g));
            if (tmp)
                free(tmp);

            SAFER(read_string(&tmp, f));
            SAFEW(write_string(tmp, g));
            if (tmp)
                free(tmp);
            SAFER(read_string(&tmp, f));
            SAFEW(write_string(tmp, g));
            if (tmp)
                free(tmp);

            SAFER(read_int32(&tmp32, f));
            SAFEW(write_int32(tmp32, g));
            SAFER(read_int32(&tmp32, f));
            SAFEW(write_int32(tmp32, g));

            SAFER(read_string(&tmp, f));
            SAFEW(write_string(tmp, g));
            if (tmp)
                free(tmp);
            SAFER(read_buffer(nick, f));
            SAFEW(write_buffer(nick, g));
            SAFER(read_int32(&tmp32, f));
            SAFEW(write_int32(tmp32, g));

            SAFER(read_int32(&tmp32, f));
            SAFEW(write_int32(tmp32, g));

            SAFER(read_int16(&n_levels, f));
            SAFEW(write_int16(n_levels, g));

            for (j = 0; j < n_levels; j++) {
                SAFER(read_int16(&tmp16, f));
                if (j < IS44_CA_SIZE)
                    SAFEW(write_int16(tmp16, g));
            }

            SAFER(read_int16(&accesscount, f));
            SAFEW(write_int16(accesscount, g));

            if (accesscount) {
                for (j = 0; j < accesscount; j++) {
                    SAFER(read_int16(&tmp16, f));
                    SAFEW(write_int16(tmp16, g));
                    if (tmp16) {
                        SAFER(read_int16(&tmp16, f));
                        SAFEW(write_int16(tmp16, g));
                        SAFER(read_string(&tmp, f));
                        SAFEW(write_string(tmp, g));
                        if (tmp)
                            free(tmp);
                    }
                }
            }

            SAFER(read_int16(&akickcount, f));
            SAFEW(write_int16(akickcount, g));

            if (akickcount) {
                for (j = 0; j < akickcount; j++) {
                    SAFER(read_int16(&tmp16, f));
                    SAFEW(write_int16(tmp16, g));
                    if (tmp16) {
                        SAFER(read_int16(&tmp16, f));
                        SAFEW(write_int16(tmp16, g));
                        SAFER(read_string(&tmp, f));
                        SAFEW(write_string(tmp, g));
                        if (tmp)
                            free(tmp);
                        SAFER(read_string(&tmp, f));
                        SAFEW(write_string(tmp, g));
                        if (tmp)
                            free(tmp);
                        SAFER(read_buffer(nick, f));
                    }
                }
            }

            SAFER(read_int16(&tmp16, f));
            SAFEW(write_int16(tmp16, g));
            SAFER(read_int16(&tmp16, f));
            SAFEW(write_int16(tmp16, g));
            SAFER(read_int32(&tmp32, f));
            SAFEW(write_int32(tmp32, g));
            SAFER(read_string(&tmp, f));
            SAFEW(write_string(tmp, g));
            if (tmp)
                free(tmp);

            SAFER(read_int16(&memocount, f));
            SAFEW(write_int16(memocount, g));
            SAFER(read_int16(&tmp16, f));
            SAFEW(write_int16(tmp16, g));

            if (memocount) {
                for (j = 0; j < memocount; j++) {
                    SAFER(read_int32(&tmp32, f));
                    SAFEW(write_int32(tmp32, g));
                    SAFER(read_int16(&tmp16, f));
                    SAFEW(write_int16(tmp16, g));
                    SAFER(read_int32(&tmp32, f));
                    SAFEW(write_int32(tmp32, g));
                    SAFER(read_buffer(nick, f));
                    SAFER(write_buffer(nick, g));
                    SAFER(read_string(&tmp, f));
                    SAFEW(write_string(tmp, g));
                    if (tmp)
                        free(tmp);
                }
            }

            SAFER(read_string(&tmp, f));
            SAFEW(write_string(tmp, g));
            if (tmp)
                free(tmp);
        }                       /* while (getc_db(f) != 0) */
        SAFEW(write_int8(0, g));
    }                           /* for (i) */

    close_db(f);
    close_db(g);

    free(rdb);
    free(wdb);

    return 1;
}

#endif

/*************************************************************************/

#endif

/*************************************************************************/

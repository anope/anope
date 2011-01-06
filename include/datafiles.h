/* Database file descriptor structure and file handling routine prototypes.
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

#ifndef DATAFILES_H
#define DATAFILES_H

#ifndef _WIN32
#include <sys/param.h>
#endif

/*************************************************************************/

typedef struct dbFILE_ dbFILE;
struct dbFILE_ {
    int mode;			/* 'r' for reading, 'w' for writing */
    FILE *fp;			/* The normal file descriptor */
    FILE *backupfp;		/* Open file pointer to a backup copy of
				 *    the database file (if non-NULL) */
    char filename[MAXPATHLEN];	/* Name of the database file */
    char backupname[MAXPATHLEN];	/* Name of the backup file */
};

/*************************************************************************/

/* Prototypes and macros: */

E void check_file_version(dbFILE *f);
E int get_file_version(dbFILE *f);
E int write_file_version(dbFILE *f, uint32 version);

E dbFILE *open_db(const char *service, const char *filename, const char *mode, uint32 version);
E void restore_db(dbFILE *f);	/* Restore to state before open_db() */
E void close_db(dbFILE *f);
E void backup_databases(void); 

#define read_db(f,buf,len)	(fread((buf),1,(len),(f)->fp))
#define write_db(f,buf,len)	(fwrite((buf),1,(len),(f)->fp))
#define getc_db(f)		(fgetc((f)->fp))

E int read_int16(uint16 *ret, dbFILE *f);
E int write_int16(uint16 val, dbFILE *f);
E int read_int32(uint32 *ret, dbFILE *f);
E int write_int32(uint32 val, dbFILE *f);
E int read_ptr(void **ret, dbFILE *f);
E int write_ptr(const void *ptr, dbFILE *f);
E int read_string(char **ret, dbFILE *f);
E int write_string(const char *s, dbFILE *f);

#define read_int8(ret,f)	((*(ret)=fgetc((f)->fp))==EOF ? -1 : 0)
#define write_int8(val,f)	(fputc((val),(f)->fp)==EOF ? -1 : 0)
#define read_buffer(buf,f)	(read_db((f),(buf),sizeof(buf)) == sizeof(buf))
#define write_buffer(buf,f)	(write_db((f),(buf),sizeof(buf)) == sizeof(buf))
#define read_buflen(buf,len,f)	(read_db((f),(buf),(len)) == (len))
#define write_buflen(buf,len,f)	(write_db((f),(buf),(len)) == (len))
#define read_variable(var,f)	(read_db((f),&(var),sizeof(var)) == sizeof(var))
#define write_variable(var,f)	(write_db((f),&(var),sizeof(var)) == sizeof(var))

/*************************************************************************/

#endif	/* DATAFILES_H */

/* os_ignore_db.c - Provides a database backend for OS IGNORE.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Included in the Anope module pack since Anope 1.7.23
 * Anope Coder: Viper <viper@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 */
/* ------------------------------------------------------------------------------- */

#include "module.h"

#define AUTHOR "Viper"
#define VERSION VERSION_STRING

/* Default database name */
#define DefIgnoreDB "os_ignore.db"
#define IGNOREDBVERSION 1

/* Database seperators */
#define SEPARATOR  '^'        /* End of a key, seperates keys from values */
#define BLOCKEND   '\n'         /* End of a block, e.g. a whole ignore */
#define VALUEEND   '\000'       /* End of a value */
#define SUBSTART   '\010'       /* Beginning of a new subblock, closed by a BLOCKEND */

/* Database reading return values */
#define DB_READ_SUCCESS   0
#define DB_READ_ERROR     1
#define DB_EOF_ERROR      2
#define DB_VERSION_ERROR  3
#define DB_READ_BLOCKEND  4
#define DB_READ_SUBSTART  5

#define DB_WRITE_SUCCESS  0
#define DB_WRITE_ERROR    1
#define DB_WRITE_NOVAL    2

/* Database Key, Value max length */
#define MAXKEYLEN 128
#define MAXVALLEN 1024

/* Structs */
typedef struct db_file_ DBFile;

struct db_file_ {
	FILE *fptr;             /* Pointer to the opened file */
	int db_version;         /* The db version of the datafiles (only needed for reading) */
	int core_db_version;    /* The current db version of this anope source */
	char service[256];      /* StatServ/etc. */
	char filename[256];     /* Filename of the database */
	char temp_name[262];    /* Temp filename of the database */
};


/* Variables */
char *IgnoreDB;

/* Functions */
int new_open_db_read(DBFile *dbptr, char **key, char **value);
int new_open_db_write(DBFile *dbptr);
void new_close_db(FILE *fptr, char **key, char **value);
int new_read_db_entry(char **key, char **value, FILE * fptr);
int new_write_db_entry(const char *key, DBFile *dbptr, const char *fmt, ...);
int new_write_db_endofblock(DBFile *dbptr);
void fill_db_ptr(DBFile *dbptr, int version, int core_version, char service[256], char filename[256]);

int save_ignoredb(int argc, char **argv);
int backup_ignoredb(int argc, char **argv);
void load_ignore_db(void);
void save_ignore_db(void);
void load_config(void);
int reload_config(int argc, char **argv);

/* ------------------------------------------------------------------------------- */

/**
 * AnopeInit is called when the module is loaded
 * @param argc Argument count
 * @param argv Argument list
 * @return MOD_CONT to allow the module, MOD_STOP to stop it
 **/
int AnopeInit(int argc, char **argv) {
	EvtHook *hook;
	IgnoreDB = NULL;

	moduleAddAuthor(AUTHOR);
	moduleAddVersion(VERSION);
	moduleSetType(SUPPORTED);

	hook = createEventHook(EVENT_RELOAD, reload_config);
	if (moduleAddEventHook(hook) != MOD_ERR_OK) {
		alog("[\002os_ignore_db\002] Can't hook to EVENT_RELOAD event");
		return MOD_STOP;
	}

	hook = createEventHook(EVENT_DB_SAVING, save_ignoredb);
	if (moduleAddEventHook(hook) != MOD_ERR_OK) {
		alog("[\002os_ignore_db\002] Can't hook to EVENT_DB_SAVING event");
		return MOD_STOP;
	}

	hook = createEventHook(EVENT_DB_BACKUP, backup_ignoredb);
	if (moduleAddEventHook(hook) != MOD_ERR_OK) {
		alog("[\002os_ignore_db\002] Can't hook to EVENT_DB_BACKUP event");
		return MOD_STOP;
	}

	load_config();
	/* Load the ignore database and re-add them to anopes ignorelist. */
	load_ignore_db();

	return MOD_CONT;
}

/**
 * Unload the module
 **/
void AnopeFini(void) {
	/* Save the ignore database before bailing out.. */
	save_ignore_db();
	
	if (IgnoreDB)
		free(IgnoreDB);
}

/* ------------------------------------------------------------------------------- */

void load_config(void) {
	int i;

	Directive confvalues[][1] = {
		{{"OSIgnoreDBName", {{PARAM_STRING, PARAM_RELOAD, &IgnoreDB}}}},
	};

	if (IgnoreDB)
		free(IgnoreDB);
	IgnoreDB = NULL;

	for (i = 0; i < 1; i++)
		moduleGetConfigDirective(confvalues[i]);

	if (!IgnoreDB)
		IgnoreDB = sstrdup(DefIgnoreDB);

	if (debug)
		alog("[os_ignore_db] debug: Set config vars: OSIgnoreDBName='%s'", IgnoreDB);
}

/**
 * Upon /os reload call the routines for reloading the configuration directives
 **/
int reload_config(int argc, char **argv) {
	if (argc >= 1) {
		if (!stricmp(argv[0], EVENT_START)) {
			load_config();
		}
	}
	return MOD_CONT;
}

/**
 * When anope saves her databases, we do the same.
 **/
int save_ignoredb(int argc, char **argv) {
	if ((argc >= 1) && (!stricmp(argv[0], EVENT_STOP)))
		save_ignore_db();

	return MOD_CONT;
}


/**
 * When anope backs her databases up, we do the same.
 **/
int backup_ignoredb(int argc, char **argv) {
	if ((argc >= 1) && (!stricmp(argv[0], EVENT_STOP))) {
		if (debug)
			alog("[os_ignore_db] debug: Backing up %s database...", IgnoreDB);
		ModuleDatabaseBackup(IgnoreDB);
	}
	return MOD_CONT;
}

/* ------------------------------------------------------------------------------- */

/**************************************************************************
 *                DataBase Handling
 **************************************************************************/

void load_ignore_db(void) {
	DBFile *dbptr = scalloc(1, sizeof(DBFile));
	char *key, *value, *mask = NULL;
	int retval = 0;
	time_t expiry_time;
	IgnoreData *ign;

	expiry_time = time(NULL);
	fill_db_ptr(dbptr, 0, IGNOREDBVERSION, s_OperServ, IgnoreDB);

	/* let's remove existing temp files here, because we only load dbs on startup */
	remove(dbptr->temp_name);

	/* Open the db, fill the rest of dbptr and allocate memory for key and value */
	if (new_open_db_read(dbptr, &key, &value)) {
		free(dbptr);
		return;                 /* Bang, an error occurred */
	}

	while (1) {
		/* read a new entry and fill key and value with it -Certus */
		retval = new_read_db_entry(&key, &value, dbptr->fptr);

		if (retval == DB_READ_ERROR) {
			new_close_db(dbptr->fptr, &key, &value);
			free(dbptr);
			return;

		} else if (retval == DB_EOF_ERROR) {
			new_close_db(dbptr->fptr, &key, &value);
			free(dbptr);
			return;
		} else if (retval == DB_READ_BLOCKEND) {        /* DB_READ_BLOCKEND */
			/* Check if we have everything to add the ignore.. 
			 * We shouldn't bother with already expired ignores either.. */
			if (mask && (expiry_time > time(NULL) || expiry_time == 0)) {
				/* We should check for double entries.. */
				for (ign = ignore; ign; ign = ign->next)
					if (!stricmp(ign->mask, mask))
						break;
 
 				if (!ign) {
					/* Create a fresh entry.. */
					ign = scalloc(sizeof(*ign), 1);
					ign->mask = sstrdup(mask);
					ign->time = expiry_time;
					ign->prev = NULL;
					ign->next = ignore;
					if (ignore)
						ignore->prev = ign;
					ignore = ign;
					if (debug)
						alog("[os_ignore_db] debug: Added new ignore entry for %s", mask);
				} else {
					/* Update time on existing entry. 
					 * The longest expiry time survives.. */
					if (expiry_time == 0 || ign->time == 0)
						ign->time = 0;
					else if (expiry_time > ign->time)
						ign->time = expiry_time;
				}
			}
			
			if (mask) free(mask);
			mask = NULL;
			expiry_time = time(NULL);
		} else {              /* DB_READ_SUCCESS */
			if (!*key || !*value)
				continue;

			/* mask */
			if (!stricmp(key, "m")) {
				if (mask)
					free(mask);
				mask = sstrdup(value);

			/* expiry time */
			} else if (!stricmp(key, "t")) {
				expiry_time = atoi(value);

			} else if (!stricmp(key, "IGNORE_DB_VERSION")) {
				if ((int)atoi(value) != IGNOREDBVERSION) {
					alog("[\002os_ignore_db\002] Database version does not match any database versions supported by this module.");
					alog("[\002os_ignore_db\002] Continuing with clean database...");
					break;
				}
			}
		} 					/* else */
	}					/* while */

	free(dbptr);
}


void save_ignore_db(void) {
	DBFile *dbptr = scalloc(1, sizeof(DBFile));
	time_t now;
	IgnoreData *ign, *next;

	now = time(NULL);
	fill_db_ptr(dbptr, 0, IGNOREDBVERSION, s_OperServ, IgnoreDB);

	/* time to backup the old db */
	rename(IgnoreDB, dbptr->temp_name);

	if (new_open_db_write(dbptr)) {
		rename(dbptr->temp_name, IgnoreDB);
		free(dbptr);
		return;                /* Bang, an error occurred */
	}

	/* Store the version of the DB in the DB as well...
	 * This will make stuff a lot easier if the database scheme needs to modified. */
	new_write_db_entry("IGNORE_DB_VERSION", dbptr, "%d", IGNOREDBVERSION);
	new_write_db_endofblock(dbptr);

	/* Go over the entire ignorelist, check whether each entry is still valid
	 * and write it to the database if it is.*/
	for (ign = ignore; ign; ign = next) {
		next = ign->next;

		if (ign->time != 0 && ign->time <= now) {
			if (debug)
				alog("[os_ignore_db] debug: Expiring ignore entry %s", ign->mask);
			if (ign->prev)
				ign->prev->next = ign->next;
			else if (ignore == ign)
				ignore = ign->next;
			if (ign->next)
				ign->next->prev = ign->prev;
			free(ign->mask);
			free(ign);
			ign = NULL;
		} else {
			new_write_db_entry("m", dbptr, "%s", ign->mask);
			new_write_db_entry("t", dbptr, "%d", ign->time);
			new_write_db_endofblock(dbptr);
		}
	}

	if (dbptr) {
		new_close_db(dbptr->fptr, NULL, NULL);  /* close file */
		remove(dbptr->temp_name);       /* saved successfully, no need to keep the old one */
		free(dbptr);           /* free the db struct */
	}
}


/* ------------------------------------------------------------------------------- */

/**************************************************************************
 *         Generic DataBase Functions  (Borrowed this from Trystan :-) )
 **************************************************************************/


int new_open_db_read(DBFile *dbptr, char **key, char **value) {
	*key = malloc(MAXKEYLEN);
	*value = malloc(MAXVALLEN);

	if (!(dbptr->fptr = fopen(dbptr->filename, "rb"))) {
		if (debug) {
			alog("debug: Can't read %s database %s : errno(%d)", dbptr->service,
				dbptr->filename, errno);
		}
		free(*key);
		*key = NULL;
		free(*value);
		*value = NULL;
		return DB_READ_ERROR;
	}
	dbptr->db_version = fgetc(dbptr->fptr) << 24 | fgetc(dbptr->fptr) << 16
		| fgetc(dbptr->fptr) << 8 | fgetc(dbptr->fptr);

	if (ferror(dbptr->fptr)) {
		if (debug) {
			alog("debug: Error reading version number on %s", dbptr->filename);
		}
		free(*key);
		*key = NULL;
		free(*value);
		*value = NULL;
		return DB_READ_ERROR;
	} else if (feof(dbptr->fptr)) {
		if (debug) {
			alog("debug: Error reading version number on %s: End of file detected",
				dbptr->filename);
		}
		free(*key);
		*key = NULL;
		free(*value);
		*value = NULL;
		return DB_EOF_ERROR;
	} else if (dbptr->db_version < 1) {
		if (debug) {
			alog("debug: Invalid version number (%d) on %s", dbptr->db_version, dbptr->filename);
		}
		free(*key);
		*key = NULL;
		free(*value);
		*value = NULL;
		return DB_VERSION_ERROR;
	}
	return DB_READ_SUCCESS;
}


int new_open_db_write(DBFile *dbptr) {
	if (!(dbptr->fptr = fopen(dbptr->filename, "wb"))) {
		if (debug) {
			alog("debug: %s Can't open %s database for writing", dbptr->service, dbptr->filename);
		}
		return DB_WRITE_ERROR;
	}

	if (fputc(dbptr->core_db_version >> 24 & 0xFF, dbptr->fptr) < 0 ||
		fputc(dbptr->core_db_version >> 16 & 0xFF, dbptr->fptr) < 0 ||
		fputc(dbptr->core_db_version >> 8 & 0xFF, dbptr->fptr) < 0 ||
		fputc(dbptr->core_db_version & 0xFF, dbptr->fptr) < 0) {
		if (debug) {
			alog("debug: Error writing version number on %s", dbptr->filename);
		}
		return DB_WRITE_ERROR;
	}
	return DB_WRITE_SUCCESS;
}


void new_close_db(FILE *fptr, char **key, char **value) {
	if (key && *key) {
		free(*key);
		*key = NULL;
	}
	if (value && *value) {
		free(*value);
		*value = NULL;
	}

	if (fptr) {
		fclose(fptr);
	}
}


int new_read_db_entry(char **key, char **value, FILE *fptr) {
	char *string = *key;
	int character;
	int i = 0;

	**key = '\0';
	**value = '\0';

	while (1) {
		if ((character = fgetc(fptr)) == EOF) { /* a problem occurred reading the file */
			if (ferror(fptr)) {
				return DB_READ_ERROR;   /* error! */
			}
			return DB_EOF_ERROR;        /* end of file */
		} else if (character == BLOCKEND) {     /* END OF BLOCK */
			return DB_READ_BLOCKEND;
		} else if (character == VALUEEND) {     /* END OF VALUE */
			string[i] = '\0';   /* end of value */
			return DB_READ_SUCCESS;
		} else if (character == SEPARATOR) {    /* END OF KEY */
			string[i] = '\0';   /* end of key */
			string = *value;    /* beginning of value */
			i = 0;              /* start with the first character of our value */
		} else {
			if ((i == (MAXKEYLEN - 1)) && (string == *key)) {   /* max key length reached, continuing with value */
				string[i] = '\0';       /* end of key */
				string = *value;        /* beginning of value */
				i = 0;          /* start with the first character of our value */
			} else if ((i == (MAXVALLEN - 1)) && (string == *value)) {  /* max value length reached, returning */
				string[i] = '\0';
				return DB_READ_SUCCESS;
			} else {
				string[i] = character;  /* read string (key or value) */
				i++;
			}
		}
	}
}


int new_write_db_entry(const char *key, DBFile *dbptr, const char *fmt, ...) {
	char string[MAXKEYLEN + MAXVALLEN + 2], value[MAXVALLEN];   /* safety byte :P */
	va_list ap;
	unsigned int length;

	if (!dbptr) {
		return DB_WRITE_ERROR;
	}

	va_start(ap, fmt);
	vsnprintf(value, MAXVALLEN, fmt, ap);
	va_end(ap);

	if (!stricmp(value, "(null)")) {
		return DB_WRITE_NOVAL;
	}
	snprintf(string, MAXKEYLEN + MAXVALLEN + 1, "%s%c%s", key, SEPARATOR, value);
	length = strlen(string);
	string[length] = VALUEEND;
	length++;

	if (fwrite(string, 1, length, dbptr->fptr) < length) {
		if (debug) {
			alog("debug: Error writing to %s", dbptr->filename);
		}
		new_close_db(dbptr->fptr, NULL, NULL);
		if (debug) {
			alog("debug: Restoring backup.");
		}
		remove(dbptr->filename);
		rename(dbptr->temp_name, dbptr->filename);
		free(dbptr);
		dbptr = NULL;
		return DB_WRITE_ERROR;
	}
	return DB_WRITE_SUCCESS;
}


int new_write_db_endofblock(DBFile *dbptr) {
	if (!dbptr) {
		return DB_WRITE_ERROR;
	}
	if (fputc(BLOCKEND, dbptr->fptr) == EOF) {
		if (debug) {
			alog("debug: Error writing to %s", dbptr->filename);
		}
		new_close_db(dbptr->fptr, NULL, NULL);
		return DB_WRITE_ERROR;
	}
	return DB_WRITE_SUCCESS;
}



void fill_db_ptr(DBFile *dbptr, int version, int core_version,
				char service[256], char filename[256]) {
	dbptr->db_version = version;
	dbptr->core_db_version = core_version;
	if (!service)
		strcpy(dbptr->service, service);
	else
		strcpy(dbptr->service, "");

	strcpy(dbptr->filename, filename);
	snprintf(dbptr->temp_name, 261, "%s.temp", filename);
	return;
}

/* EOF */

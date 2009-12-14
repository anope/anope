/* Database file handling routines.
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
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
#include "datafiles.h"
#include "modules.h"
#include <fcntl.h>

static int curday = 0;
static time_t lastwarn = 0;

/*************************************************************************/

/**
 * Renames a database
 *
 * @param name Database to name
 * @param ext Extention
 * @return void
 */
static void rename_database(const char *name, char *ext)
{

	char destpath[PATH_MAX];

	snprintf(destpath, sizeof(destpath), "backups/%s.%s", name, ext);
	if (rename(name, destpath) != 0) {
		alog("Backup of %s failed.", name);
		ircdproto->SendGlobops(Config.s_OperServ, "WARNING! Backup of %s failed.",
						 name);
	}
}

/*************************************************************************/

/**
 * Removes old databases
 *
 * @return void
 */
static void remove_backups()
{

	char ext[9];
	char path[PATH_MAX];

	time_t t;
	struct tm tm;

	time(&t);
	t -= (60 * 60 * 24 * Config.KeepBackups);
	tm = *localtime(&t);
	strftime(ext, sizeof(ext), "%Y%m%d", &tm);

	snprintf(path, sizeof(path), "backups/%s.%s", Config.NickDBName, ext);
	DeleteFile(path);
	
	snprintf(path, sizeof(path), "backups/%s.%s", Config.ChanDBName, ext);
	DeleteFile(path);
	
	snprintf(path, sizeof(path), "backups/%s.%s", Config.OperDBName, ext);
	DeleteFile(path);
	
	snprintf(path, sizeof(path), "backups/%s.%s", Config.NewsDBName, ext);
	DeleteFile(path);
	
	snprintf(path, sizeof(path), "backups/%s.%s", Config.ExceptionDBName, ext);
	DeleteFile(path);

	if (Config.s_BotServ) {
		snprintf(path, sizeof(path), "backups/%s.%s", Config.BotDBName, ext);
		DeleteFile(path);
	}
	if (Config.s_HostServ) {
		snprintf(path, sizeof(path), "backups/%s.%s", Config.HostDBName, ext);
		DeleteFile(path);
	}
	if (Config.NSEmailReg) {
		snprintf(path, sizeof(path), "backups/%s.%s", Config.PreNickDBName, ext);
		DeleteFile(path);
	}
}

/*************************************************************************/

/**
 * Handles database backups.
 *
 * @return void
 */
void backup_databases()
{

	time_t t;
	struct tm tm;

	if (!Config.KeepBackups) {
		return;
	}

	time(&t);
	tm = *localtime(&t);

	if (!curday) {
		curday = tm.tm_yday;
		return;
	}

	if (curday != tm.tm_yday) {

		char ext[9];


		alog("Backing up databases");
		FOREACH_MOD(I_OnBackupDatabase, OnBackupDatabase())

		remove_backups();

		curday = tm.tm_yday;
		strftime(ext, sizeof(ext), "%Y%m%d", &tm);

		rename_database(Config.NickDBName, ext);
		if (Config.s_BotServ) {
			rename_database(Config.BotDBName, ext);
		}
		rename_database(Config.ChanDBName, ext);
		if (Config.s_HostServ) {
			rename_database(Config.HostDBName, ext);
		}
		if (Config.NSEmailReg) {
			rename_database(Config.PreNickDBName, ext);
		}

		rename_database(Config.OperDBName, ext);
		rename_database(Config.NewsDBName, ext);
		rename_database(Config.ExceptionDBName, ext);
	}
}

/*************************************************************************/

void ModuleDatabaseBackup(const char *dbname)
{

	time_t t;
	struct tm tm;

	if (!Config.KeepBackups) {
		return;
	}

	time(&t);
	tm = *localtime(&t);

	if (!curday) {
		curday = tm.tm_yday;
		return;
	}

	if (curday != tm.tm_yday) {

		char ext[9];

		if (debug) {
			alog("Module Database Backing up %s", dbname);
		}
		ModuleRemoveBackups(dbname);
		curday = tm.tm_yday;
		strftime(ext, sizeof(ext), "%Y%m%d", &tm);
		rename_database(dbname, ext);
	}
}

/*************************************************************************/

void ModuleRemoveBackups(const char *dbname)
{
	char ext[9];
	char path[PATH_MAX];

	time_t t;
	struct tm tm;

	time(&t);
	t -= (60 * 60 * 24 * Config.KeepBackups);
	tm = *localtime(&t);
	strftime(ext, sizeof(ext), "%Y%m%d", &tm);

	snprintf(path, sizeof(path), "backups/%s.%s", dbname, ext);
	DeleteFile(path);
}

/*************************************************************************/

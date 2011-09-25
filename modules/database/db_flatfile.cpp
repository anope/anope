/*
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

Anope::string DatabaseFile;

class DBFlatFile : public Module
{
	/* Day the last backup was on */
	int LastDay;
	/* Backup file names */
	std::list<Anope::string> Backups;

	SerializableBase *find(const Anope::string &sname)
	{
		for (unsigned i = 0; i < serialized_types.size(); ++i)
			if (serialized_types[i]->serialize_name() == sname)
				return serialized_types[i];
		return NULL;
	}

 public:
	DBFlatFile(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnLoadDatabase, I_OnSaveDatabase };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		OnReload();

		LastDay = 0;
	}

	void BackupDatabase()
	{
		/* Do not backup a database that doesn't exist */
		if (!IsFile(DatabaseFile))
			return;

		time_t now = Anope::CurTime;
		tm *tm = localtime(&now);

		if (tm->tm_mday != LastDay)
		{
			LastDay = tm->tm_mday;
			Anope::string newname = "backups/" + DatabaseFile + "." + stringify(tm->tm_year) + stringify(tm->tm_mon) + stringify(tm->tm_mday);

			/* Backup already exists */
			if (IsFile(newname))
				return;

			Log(LOG_DEBUG) << "db_flatfile: Attemping to rename " << DatabaseFile << " to " << newname;
			if (rename(DatabaseFile.c_str(), newname.c_str()))
			{
				Log() << "Unable to back up database!";

				if (!Config->NoBackupOkay)
					quitting = true;

				return;
			}

			Backups.push_back(newname);

			if (Config->KeepBackups > 0 && Backups.size() > static_cast<unsigned>(Config->KeepBackups))
			{
				DeleteFile(Backups.front().c_str());
				Backups.pop_front();
			}
		}
	}

	void OnReload()
	{
		ConfigReader config;
		DatabaseFile = config.ReadValue("flatfile", "database", "anope.db", 0);
	}

	EventReturn OnLoadDatabase()
	{
		std::fstream db;
		db.open(DatabaseFile.c_str(), std::ios_base::in);

		if (!db.is_open())
		{
			Log() << "Unable to open " << DatabaseFile << " for reading!";
			return EVENT_CONTINUE;
		}

		SerializableBase *sb = NULL;
		SerializableBase::serialized_data data;
		std::multimap<SerializableBase *, SerializableBase::serialized_data> objects;
		for (Anope::string buf, token; std::getline(db, buf.str());)
		{
			spacesepstream sep(buf);

			if (!sep.GetToken(token))
				continue;

			if (token == "OBJECT" && sep.GetToken(token))
			{
				sb = this->find(token);
				data.clear();
			}
			else if (token == "DATA" && sb != NULL && sep.GetToken(token))
				data[token] << sep.GetRemaining();
			else if (token == "END" && sb != NULL)
			{
				objects.insert(std::make_pair(sb, data));

				sb = NULL;
				data.clear();
			}
		}

		for (unsigned i = 0; i < serialized_types.size(); ++i)
		{
			SerializableBase *stype = serialized_types[i];

			std::multimap<SerializableBase *, SerializableBase::serialized_data>::iterator it = objects.find(stype), it_end = objects.upper_bound(stype);
			if (it == objects.end())
				continue;
			for (; it != it_end; ++it)
				it->first->alloc(it->second);
		}

		db.close();

		return EVENT_STOP;
	}


	EventReturn OnSaveDatabase()
	{
		BackupDatabase();

		Anope::string tmp_db = DatabaseFile + ".tmp";
		if (IsFile(DatabaseFile))
			rename(DatabaseFile.c_str(), tmp_db.c_str());

		std::fstream db(DatabaseFile.c_str(), std::ios_base::out | std::ios_base::trunc);
		if (!db.is_open())
		{
			Log() << "Unable to open " << DatabaseFile << " for writing";
			if (IsFile(tmp_db))
				rename(tmp_db.c_str(), DatabaseFile.c_str());
			return EVENT_CONTINUE;
		}

		for (std::list<SerializableBase *>::iterator it = serialized_items.begin(), it_end = serialized_items.end(); it != it_end; ++it)
		{
			SerializableBase *base = *it;
			SerializableBase::serialized_data data = base->serialize();

			db << "OBJECT " << base->serialize_name() << "\n";
			for (SerializableBase::serialized_data::iterator dit = data.begin(), dit_end = data.end(); dit != dit_end; ++dit)
				db << "DATA " << dit->first << " " << dit->second.astr() << "\n";
			db << "END\n";
		}

		db.close();

		if (db.good() == false)
		{
			Log() << "Unable to write database";
			if (!Config->NoBackupOkay)
				quitting = true;
			if (IsFile(tmp_db))
				rename(tmp_db.c_str(), DatabaseFile.c_str());
		}
		else
			DeleteFile(tmp_db.c_str());

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(DBFlatFile)



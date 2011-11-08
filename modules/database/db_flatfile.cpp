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
				unlink(Backups.front().c_str());
				Backups.pop_front();
			}
		}
	}

	void OnReload()
	{
		ConfigReader config;
		DatabaseFile = config.ReadValue("db_flatfile", "database", "anope.db", 0);
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

		SerializeType *st = NULL;
		Serializable::serialized_data data;
		std::multimap<SerializeType *, Serializable::serialized_data> objects;
		for (Anope::string buf, token; std::getline(db, buf.str());)
		{
			spacesepstream sep(buf);

			if (!sep.GetToken(token))
				continue;

			if (token == "OBJECT" && sep.GetToken(token))
			{
				st = SerializeType::Find(token);
				data.clear();
			}
			else if (token == "DATA" && st != NULL && sep.GetToken(token))
				data[token] << sep.GetRemaining();
			else if (token == "END" && st != NULL)
			{
				objects.insert(std::make_pair(st, data));

				st = NULL;
				data.clear();
			}
		}

		const std::vector<Anope::string> type_order = SerializeType::GetTypeOrder();
		for (unsigned i = 0; i < type_order.size(); ++i)
		{
			SerializeType *stype = SerializeType::Find(type_order[i]);

			std::multimap<SerializeType *, Serializable::serialized_data>::iterator it = objects.find(stype), it_end = objects.upper_bound(stype);
			if (it == objects.end())
				continue;
			for (; it != it_end; ++it)
				it->first->Create(it->second);
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

		const std::list<Serializable *> &items = Serializable::GetItems();
		for (std::list<Serializable *>::const_iterator it = items.begin(), it_end = items.end(); it != it_end; ++it)
		{
			Serializable *base = *it;
			Serializable::serialized_data data = base->serialize();
	
			db << "OBJECT " << base->GetSerializeName() << "\n";
			for (Serializable::serialized_data::iterator dit = data.begin(), dit_end = data.end(); dit != dit_end; ++dit)
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
			unlink(tmp_db.c_str());

		return EVENT_CONTINUE;
	}
};

MODULE_INIT(DBFlatFile)



/*
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class SaveData : public Serialize::Data
{
 public:
 	Anope::string last;
	std::fstream *fs;

	SaveData() : fs(NULL) { }

	std::iostream& operator[](const Anope::string &key) anope_override
	{
		if (key != last)
		{
			*fs << "\nDATA " << key << " ";
			last = key;
		}

		return *fs;
	}
};

class LoadData : public Serialize::Data
{
 public:
 	std::fstream *fs;
	std::map<Anope::string, Anope::string> data;
	std::stringstream ss;
	bool read;

	LoadData() : fs(NULL), read(false) { }

	std::iostream& operator[](const Anope::string &key) anope_override
	{
		if (!read)
		{
			for (Anope::string token; std::getline(*this->fs, token.str());)
			{
				if (token.find("DATA ") != 0)
					break;

				size_t sp = token.find(' ', 5); // Skip DATA
				if (sp != Anope::string::npos)
					data[token.substr(5, sp - 5)] = token.substr(sp + 1);
			}

			read = true;
		}

		ss.clear();
		this->ss << this->data[key];
		return this->ss;
	}

	std::set<Anope::string> KeySet() const anope_override
	{
		std::set<Anope::string> keys;
		for (std::map<Anope::string, Anope::string>::const_iterator it = this->data.begin(), it_end = this->data.end(); it != it_end; ++it)
			keys.insert(it->first);
		return keys;
	}
	
	void Reset()
	{
		read = false;
		data.clear();
	}
};

class DBFlatFile : public Module, public Pipe
{
	Anope::string database_file;
	/* Day the last backup was on */
	int last_day;
	/* Backup file names */
	std::map<Anope::string, std::list<Anope::string> > backups;
	bool use_fork;
	bool loaded;

	void BackupDatabase()
	{
		tm *tm = localtime(&Anope::CurTime);

		if (tm->tm_mday != last_day)
		{
			last_day = tm->tm_mday;

			const std::vector<Anope::string> &type_order = Serialize::Type::GetTypeOrder();

			std::set<Anope::string> dbs;
			dbs.insert(database_file);

			for (unsigned i = 0; i < type_order.size(); ++i)
			{
				Serialize::Type *stype = Serialize::Type::Find(type_order[i]);

				if (stype && stype->GetOwner())
					dbs.insert("module_" + stype->GetOwner()->name + ".db");
			}


			for (std::set<Anope::string>::const_iterator it = dbs.begin(), it_end = dbs.end(); it != it_end; ++it)
			{
				const Anope::string &oldname = Anope::DataDir + "/" + *it;
				Anope::string newname = Anope::DataDir + "/backups/" + *it + "." + stringify(tm->tm_year) + "." + stringify(tm->tm_mon) + "." + stringify(tm->tm_mday);

				/* Backup already exists or no database to backup */
				if (Anope::IsFile(newname) || !Anope::IsFile(oldname))
					continue;

				Log(LOG_DEBUG) << "db_flatfile: Attemping to rename " << *it << " to " << newname;
				if (rename(oldname.c_str(), newname.c_str()))
				{
					Log(this) << "Unable to back up database " << *it << "!";

					if (!Config->NoBackupOkay)
						Anope::Quitting = true;

					continue;
				}

				backups[*it].push_back(newname);

				if (Config->KeepBackups > 0 && backups[*it].size() > static_cast<unsigned>(Config->KeepBackups))
				{
					unlink(backups[*it].front().c_str());
					backups[*it].pop_front();
				}
			}
		}
	}

 public:
	DBFlatFile(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE), last_day(0), use_fork(false), loaded(false)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnReload, I_OnLoadDatabase, I_OnSaveDatabase, I_OnSerializeTypeCreate };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

		OnReload();
	}

	void OnNotify() anope_override
	{
		char buf[512];
		int i = this->Read(buf, sizeof(buf) - 1);
		if (i <= 0)
			return;
		buf[i] = 0;

		if (!*buf)
		{
			Log(this) << "Finished saving databases";
			return;
		}

		Log(this) << "Error saving databases: " << buf;

		if (!Config->NoBackupOkay)
			Anope::Quitting = true;
	}

	void OnReload() anope_override
	{
		ConfigReader config;
		database_file = config.ReadValue("db_flatfile", "database", "anope.db", 0);
		use_fork = config.ReadFlag("db_flatfile", "fork", "no", 0);
	}

	EventReturn OnLoadDatabase() anope_override
	{
		const std::vector<Anope::string> &type_order = Serialize::Type::GetTypeOrder();
		std::set<Anope::string> tried_dbs;

		const Anope::string &db_name = Anope::DataDir + "/" + database_file;

		std::fstream fd(db_name.c_str(), std::ios_base::in);
		if (!fd.is_open())
		{
			Log(this) << "Unable to open " << db_name << " for reading!";
			return EVENT_STOP;
		}

		std::map<Anope::string, std::vector<std::streampos> > positions;

		for (Anope::string buf; std::getline(fd, buf.str());)
			if (buf.find("OBJECT ") == 0)
				positions[buf.substr(7)].push_back(fd.tellg());

		LoadData ld;
		ld.fs = &fd;

		for (unsigned i = 0; i < type_order.size(); ++i)
		{
			Serialize::Type *stype = Serialize::Type::Find(type_order[i]);
			if (!stype || stype->GetOwner())
				continue;
				
			std::vector<std::streampos> &pos = positions[stype->GetName()];

			for (unsigned j = 0; j < pos.size(); ++j)
			{
				fd.clear();
				fd.seekg(pos[j]);

				stype->Unserialize(NULL, ld);
				ld.Reset();
			}
		}

		fd.close();

		loaded = true;
		return EVENT_STOP;
	}


	EventReturn OnSaveDatabase() anope_override
	{
		BackupDatabase();

		int i = -1;
		if (use_fork)
		{
			i = fork();
			if (i > 0)
				return EVENT_CONTINUE;
			else if (i < 0)
				Log(this) << "Unable to fork for database save";
		}

		try
		{
			std::map<Module *, std::fstream *> databases;

			SaveData data;
			const std::list<Serializable *> &items = Serializable::GetItems();
			for (std::list<Serializable *>::const_iterator it = items.begin(), it_end = items.end(); it != it_end; ++it)
			{
				Serializable *base = *it;
				Serialize::Type *s_type = base->GetSerializableType();

				if (!s_type)
					continue;

				data.fs = databases[s_type->GetOwner()];

				if (!data.fs)
				{
					Anope::string db_name;
					if (s_type->GetOwner())
						db_name = Anope::DataDir + "/module_" + s_type->GetOwner()->name + ".db";
					else
						db_name = Anope::DataDir + "/" + database_file;

					if (Anope::IsFile(db_name))
						rename(db_name.c_str(), (db_name + ".tmp").c_str());

					data.fs = databases[s_type->GetOwner()] = new std::fstream(db_name.c_str(), std::ios_base::out | std::ios_base::trunc);

					if (!data.fs->is_open())
					{
						Log(this) << "Unable to open " << db_name << " for writing";
						continue;
					}
				}
				else if (!data.fs->is_open())
					continue;

				*data.fs << "OBJECT " << s_type->GetName();
				base->Serialize(data);
				*data.fs << "\nEND\n";
			}

			for (std::map<Module *, std::fstream *>::iterator it = databases.begin(), it_end = databases.end(); it != it_end; ++it)
			{
				std::fstream *f = it->second;
				const Anope::string &db_name = Anope::DataDir + "/" + (it->first ? (it->first->name + ".db") : database_file);

				if (!f->is_open() || !f->good())
				{
					this->Write("Unable to write database " + db_name);

					f->close();

					if (Anope::IsFile((db_name + ".tmp").c_str()))
						rename((db_name + ".tmp").c_str(), db_name.c_str());
				}
				else
				{
					f->close();
					unlink((db_name + ".tmp").c_str());
				}

				delete f;
			}
		}
		catch (...)
		{
			if (i)
				throw;
		}

		if (!i)
		{
			this->Notify();
			exit(0);
		}

		return EVENT_CONTINUE;
	}

	/* Load just one type. Done if a module is reloaded during runtime */
	void OnSerializeTypeCreate(Serialize::Type *stype) anope_override
	{
		if (!loaded)
			return;

		Anope::string db_name;
		if (stype->GetOwner())
			db_name = Anope::DataDir + "/module_" + stype->GetOwner()->name + ".db";
		else
			db_name = Anope::DataDir + "/" + database_file;

		std::fstream fd(db_name.c_str(), std::ios_base::in);
		if (!fd.is_open())
		{
			Log(this) << "Unable to open " << db_name << " for reading!";
			return;
		}

		LoadData ld;
		ld.fs = &fd;

		for (Anope::string buf; std::getline(fd, buf.str());)
		{
			if (buf == "OBJECT " + stype->GetName())
			{
				stype->Unserialize(NULL, ld);
				ld.Reset();
			}
		}

		fd.close();
	}
};

MODULE_INIT(DBFlatFile)



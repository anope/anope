/*
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

#ifndef _WIN32
#include <sys/wait.h>
#endif

class SaveData final
	: public Serialize::Data
{
public:
	Anope::string last;
	std::fstream *fs = nullptr;

	std::iostream &operator[](const Anope::string &key) override
	{
		if (key != last)
		{
			*fs << "\nDATA " << key << " ";
			last = key;
		}

		return *fs;
	}
};

class LoadData final
	: public Serialize::Data
{
public:
	std::fstream *fs = nullptr;
	uint64_t id = 0;
	std::map<Anope::string, Anope::string> data;
	std::stringstream ss;
	bool read = false;

	std::iostream &operator[](const Anope::string &key) override
	{
		if (!read)
		{
			for (Anope::string token; std::getline(*this->fs, token.str());)
			{
				if (token.find("ID ") == 0)
				{
					this->id = Anope::Convert<uint64_t>(token.substr(3), 0);
					continue;
				}
				else if (token.find("DATA ") != 0)
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

	size_t Hash() const override
	{
		size_t hash = 0;
		for (const auto &[_, value] : this->data)
			if (!value.empty())
				hash ^= Anope::hash_cs()(value);
		return hash;
	}

	void Reset()
	{
		id = 0;
		read = false;
		data.clear();
	}
};

class DBFlatFile final
	: public Module
	, public Pipe
{
	/* Day the last backup was on */
	int last_day = 0;
	/* Backup file names */
	std::map<Anope::string, std::list<Anope::string> > backups;
	bool loaded = false;

	int child_pid = -1;

	void BackupDatabase()
	{
		tm *tm = localtime(&Anope::CurTime);

		if (tm->tm_mday != last_day)
		{
			last_day = tm->tm_mday;

			std::set<Anope::string> dbs;
			dbs.insert(Config->GetModule(this).Get<const Anope::string>("database", "anope.db"));

			for (const auto &type_order : Serialize::Type::GetTypeOrder())
			{
				Serialize::Type *stype = Serialize::Type::Find(type_order);

				if (stype && stype->GetOwner())
					dbs.insert("module_" + stype->GetOwner()->name + ".db");
			}


			for (const auto &db : dbs)
			{
				const auto oldname = Anope::ExpandData(db);
				const auto newname = Anope::ExpandData("backups/" + db + "-" + Anope::ToString(tm->tm_year + 1900) + Anope::printf("-%02i-", tm->tm_mon + 1) + Anope::printf("%02i", tm->tm_mday));

				/* Backup already exists or no database to backup */
				if (Anope::IsFile(newname) || !Anope::IsFile(oldname))
					continue;

				Log(LOG_DEBUG) << "db_flatfile: Attempting to rename " << db << " to " << newname;
				if (rename(oldname.c_str(), newname.c_str()))
				{
					Anope::string err = Anope::LastError();
					Log(this) << "Unable to back up database " << db << " (" << err << ")!";

					if (!Config->GetModule(this).Get<bool>("nobackupokay"))
					{
						Anope::Quitting = true;
						Anope::QuitReason = "Unable to back up database " + db + " (" + err + ")";
					}

					continue;
				}

				backups[db].push_back(newname);

				unsigned keepbackups = Config->GetModule(this).Get<unsigned>("keepbackups");
				if (keepbackups > 0 && backups[db].size() > keepbackups)
				{
					unlink(backups[db].front().c_str());
					backups[db].pop_front();
				}
			}
		}
	}

public:
	DBFlatFile(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE | VENDOR)
	{

	}

#ifndef _WIN32
	void OnRestart() override
	{
		OnShutdown();
	}

	void OnShutdown() override
	{
		if (child_pid > -1)
		{
			Log(this) << "Waiting for child to exit...";

			int status;
			waitpid(child_pid, &status, 0);

			Log(this) << "Done";
		}
	}
#endif

	void OnNotify() override
	{
		char buf[512];
		int i = this->Read(buf, sizeof(buf) - 1);
		if (i <= 0)
			return;
		buf[i] = 0;

		child_pid = -1;

		if (!*buf)
		{
			Log(this) << "Finished saving databases";
			return;
		}

		Log(this) << "Error saving databases: " << buf;

		if (!Config->GetModule(this).Get<bool>("nobackupokay"))
			Anope::Quitting = true;
	}

	EventReturn OnLoadDatabase() override
	{
		std::set<Anope::string> tried_dbs;

		const auto db_name = Anope::ExpandData(Config->GetModule(this).Get<const Anope::string>("database", "anope.db"));

		std::fstream fd(db_name.c_str(), std::ios_base::in | std::ios_base::binary);
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

		for (const auto &type_order : Serialize::Type::GetTypeOrder())
		{
			Serialize::Type *stype = Serialize::Type::Find(type_order);
			if (!stype || stype->GetOwner())
				continue;

			for (const auto &position : positions[stype->GetName()])
			{
				fd.clear();
				fd.seekg(position);

				Serializable *obj = stype->Unserialize(NULL, ld);
				if (obj != NULL)
					obj->id = ld.id;
				ld.Reset();
			}
		}

		fd.close();

		loaded = true;
		return EVENT_STOP;
	}


	void OnSaveDatabase() override
	{
		if (child_pid > -1)
		{
			Log(this) << "Database save is already in progress!";
			return;
		}

		BackupDatabase();

		int i = -1;
#ifndef _WIN32
		if (!Anope::Quitting && Config->GetModule(this).Get<bool>("fork"))
		{
			i = fork();
			if (i > 0)
			{
				child_pid = i;
				return;
			}
			else if (i < 0)
				Log(this) << "Unable to fork for database save";
		}
#endif

		try
		{
			std::map<Module *, std::fstream *> databases;

			/* First open the databases of all of the registered types. This way, if we have a type with 0 objects, that database will be properly cleared */
			for (const auto &[_, s_type] : Serialize::Type::GetTypes())
			{
				if (databases[s_type->GetOwner()])
					continue;

				Anope::string db_name;
				if (s_type->GetOwner())
					db_name = Anope::ExpandData("module_" + s_type->GetOwner()->name + ".db");
				else
					db_name = Anope::ExpandData(Config->GetModule(this).Get<const Anope::string>("database", "anope.db"));

				std::fstream *fs = databases[s_type->GetOwner()] = new std::fstream((db_name + ".tmp").c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

				if (!fs->is_open())
					Log(this) << "Unable to open " << db_name << " for writing";
			}

			SaveData data;
			const std::list<Serializable *> &items = Serializable::GetItems();
			for (auto *base : items)
			{
				Serialize::Type *s_type = base->GetSerializableType();
				if (!s_type)
					continue;

				data.fs = databases[s_type->GetOwner()];
				if (!data.fs || !data.fs->is_open())
					continue;

				*data.fs << "OBJECT " << s_type->GetName();
				if (base->id)
					*data.fs << "\nID " << base->id;
				base->Serialize(data);
				*data.fs << "\nEND\n";
			}

			for (auto &[mod, f] : databases)
			{
				const auto db_name = Anope::ExpandData((mod ? (mod->name + ".db") : Config->GetModule(this).Get<const Anope::string>("database", "anope.db")));

				if (!f->is_open() || !f->good())
				{
					this->Write("Unable to write database " + db_name);

					f->close();
				}
				else
				{
					f->close();
#ifdef _WIN32
					/* Windows rename() fails if the file already exists. */
					remove(db_name.c_str());
#endif
					rename((db_name + ".tmp").c_str(), db_name.c_str());
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
	}

	/* Load just one type. Done if a module is reloaded during runtime */
	void OnSerializeTypeCreate(Serialize::Type *stype) override
	{
		if (!loaded)
			return;

		Anope::string db_name;
		if (stype->GetOwner())
			db_name = Anope::ExpandData("module_" + stype->GetOwner()->name + ".db");
		else
			db_name = Anope::ExpandData(Config->GetModule(this).Get<const Anope::string>("database", "anope.db"));

		std::fstream fd(db_name.c_str(), std::ios_base::in | std::ios_base::binary);
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

/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"

class DBFlatFile : public Module
	, public EventHook<Event::LoadDatabase>
	, public EventHook<Event::SaveDatabase>
{
	/* Day the last backup was on */
	int last_day;
	/* Backup file names */
	std::map<Anope::string, std::list<Anope::string> > backups;
	bool loaded;

	void BackupDatabase()
	{
		tm *tm = localtime(&Anope::CurTime);

		if (tm->tm_mday != last_day)
		{
			last_day = tm->tm_mday;

			std::set<Anope::string> dbs;
			dbs.insert(Config->GetModule(this)->Get<Anope::string>("database", "anope.db"));

			for (Serialize::TypeBase *stype : Serialize::TypeBase::GetTypes())
			{
				if (stype->GetOwner())
					dbs.insert("module_" + stype->GetOwner()->name + ".db");
			}


			for (std::set<Anope::string>::const_iterator it = dbs.begin(), it_end = dbs.end(); it != it_end; ++it)
			{
				const Anope::string &oldname = Anope::DataDir + "/" + *it;
				Anope::string newname = Anope::DataDir + "/backups/" + *it + "-" + stringify(tm->tm_year + 1900) + Anope::printf("-%02i-", tm->tm_mon + 1) + Anope::printf("%02i", tm->tm_mday);

				/* Backup already exists or no database to backup */
				if (Anope::IsFile(newname) || !Anope::IsFile(oldname))
					continue;

				Log(LOG_DEBUG) << "db_flatfile: Attempting to rename " << *it << " to " << newname;
				if (rename(oldname.c_str(), newname.c_str()))
				{
					Anope::string err = Anope::LastError();
					Log(this) << "Unable to back up database " << *it << " (" << err << ")!";

					if (!Config->GetModule(this)->Get<bool>("nobackupokay"))
					{
						Anope::Quitting = true;
						Anope::QuitReason = "Unable to back up database " + *it + " (" + err + ")";
					}

					continue;
				}

				backups[*it].push_back(newname);

				unsigned keepbackups = Config->GetModule(this)->Get<unsigned>("keepbackups");
				if (keepbackups > 0 && backups[*it].size() > keepbackups)
				{
					unlink(backups[*it].front().c_str());
					backups[*it].pop_front();
				}
			}
		}
	}

 public:
	DBFlatFile(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE | VENDOR)
		, EventHook<Event::LoadDatabase>(this)
		, EventHook<Event::SaveDatabase>(this)
		, last_day(0)
		, loaded(false)
	{

	}

	EventReturn OnLoadDatabase() override
	{
		const Anope::string &db_name = Anope::DataDir + "/" + Config->GetModule(this)->Get<Anope::string>("database", "anope.db");

		std::fstream fd(db_name.c_str(), std::ios_base::in | std::ios_base::binary);
		if (!fd.is_open())
		{
			Log(this) << "Unable to open " << db_name << " for reading!";
			return EVENT_STOP;
		}

		Serialize::TypeBase *type = nullptr;
		Serialize::Object *obj = nullptr;
		for (Anope::string buf; std::getline(fd, buf.str());)
		{
			if (buf.find("OBJECT ") == 0)
			{
				Anope::string t = buf.substr(7);
				if (obj)
					Log(LOG_DEBUG) << "obj != null but got OBJECT";
				if (type)
					Log(LOG_DEBUG) << "type != null but got OBJECT";
				type = Serialize::TypeBase::Find(t);
				obj = nullptr;
			}
			else if (buf.find("ID ") == 0)
			{
				if (!type || obj)
					continue;

				try
				{
					Serialize::ID id = convertTo<Serialize::ID>(buf.substr(3));
					obj = type->Require(id);
				}
				catch (const ConvertException &)
				{
					Log(LOG_DEBUG) << "Unable to parse object id " << buf.substr(3);
				}
			}
			else if (buf.find("DATA ") == 0)
			{
				if (!type)
					continue;

				if (!obj)
					obj = type->Create();

				size_t sp = buf.find(' ', 5); // Skip DATA
				if (sp == Anope::string::npos)
					continue;

				Anope::string key = buf.substr(5, sp - 5), value = buf.substr(sp + 1);

				Serialize::FieldBase *field = type->GetField(key);
				if (field)
					field->UnserializeFromString(obj, value);
			}
			else if (buf.find("END") == 0)
			{
				type = nullptr;
				obj = nullptr;
			}
		}

		fd.close();

		loaded = true;
		return EVENT_STOP;
	}


	void OnSaveDatabase() override
	{
		BackupDatabase();

		Anope::string db_name = Anope::DataDir + "/" + Config->GetModule(this)->Get<Anope::string>("database", "anope.db");

		if (Anope::IsFile(db_name))
			rename(db_name.c_str(), (db_name + ".tmp").c_str());

		std::fstream f(db_name.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

		if (!f.is_open())
		{
			Log(this) << "Unable to open " << db_name << " for writing";
		}
		else
		{
			for (std::pair<Serialize::ID, Serialize::Object *> p : Serialize::objects)
			{
				Serialize::Object *object = p.second;
				Serialize::TypeBase *s_type = object->GetSerializableType();

				f << "OBJECT " << s_type->GetName() << "\n";
				f << "ID " << object->id << "\n";
				for (Serialize::FieldBase *field : s_type->GetFields())
					if (field->HasFieldS(object)) // for ext
						f << "DATA " << field->serialize_name << " " << field->SerializeToString(object) << "\n";
				f << "END\n";
			}
		}

		if (!f.is_open() || !f.good())
		{
			f.close();
			rename((db_name + ".tmp").c_str(), db_name.c_str());
		}
		else
		{
			f.close();
			unlink((db_name + ".tmp").c_str());
		}
	}
};

MODULE_INIT(DBFlatFile)



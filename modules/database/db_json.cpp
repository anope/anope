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

#include <filesystem>
namespace fs = std::filesystem;

#include "yyjson/yyjson.c"

#include "module.h"

#define ANOPE_DATABASE_VERSION 1

// TODO:
// * forking into background

inline Anope::string yyjson_get_astr(yyjson_val *val)
{
	const auto *str = yyjson_get_str(val);
	return str ? str : "";
}

class Data final
	: public Serialize::Data
{
public:
	// If non-zero then the id of the database entry.
	Serializable::Id id = 0;

	// Data in this database entry.
	Anope::map<std::stringstream> data;

	// Used when writing data.
	Data(Serialize::Type *s_type, Serializable *obj)
	{
		if (obj->id)
			this->id = obj->id;
		s_type->Serialize(obj, *this);
	}

	// Used when reading data.
	Data(yyjson_val *elem)
	{
		size_t idx, max;
		yyjson_val *key, *value;
		yyjson_obj_foreach(elem, idx, max, key, value)
		{
			if (yyjson_get_type(key) != YYJSON_TYPE_STR)
				continue;

			auto akey = yyjson_get_astr(key);
			if (akey.equals_ci("@id"))
			{
				this->id = yyjson_get_uint(value);
				continue;
			}

			if (yyjson_is_bool(value))
				data[akey] << yyjson_get_bool(value);
			else if (yyjson_is_int(value))
				data[akey] << yyjson_get_int(value);
			else if (yyjson_is_null(value))
				data[akey];
			else if (yyjson_is_real(value))
				data[akey] << yyjson_get_real(value);
			else if (yyjson_is_str(value))
				data[akey] << yyjson_get_astr(value);
			else if (yyjson_is_uint(value))
				data[akey] << yyjson_get_uint(value);
		}
	}

	std::iostream &operator[](const Anope::string &key) override
	{
		return data[key];
	}
};

class DBJSON final
	: public Module
{
private:
	// Multimap from serializable type to serializable data.
	using DBData = Anope::multimap<Data>;

	// The databases which have already been loaded from disk.
	std::unordered_map<Module *, DBData> databases;

	// Whether OnLoadDatabase has been called yet.
	bool loaded = false;

	void CreateBackup(const Anope::string &backupdir, const fs::path &dbpath, size_t backups, const char *fmt, const char *glob)
	{
		if (!backups)
			return;

		char timebuf[16];
		strftime(timebuf, sizeof(timebuf), fmt, localtime(&Anope::CurTime));

		auto backupbase = Anope::Expand(backupdir, dbpath.filename().string()) + ".";
		auto backupname = backupbase + timebuf;

		std::error_code ec;
		Anope::string dbname = dbpath.string();
		Log(LOG_DEBUG) << "Copying " << dbname << " to " << backupname;
		if (!fs::copy_file(dbname.str(), backupname.str(), fs::copy_options::overwrite_existing, ec))
		{
			Log(this) << "Failed to copy " << dbname << " to " << backupname << ": " << ec.message();
			if (!Config->GetModule(this).Get<bool>("ignore_backup_failure"))
			{
				Anope::Quitting = true;
				Anope::QuitReason = "Failed to copy " + dbname + " to " + backupname + ": " + ec.message();
			}
		}

		// Delete older backups.
		std::set<Anope::string> old_backups;
		for (const auto &entry : fs::directory_iterator(backupdir.str(), ec))
		{
			Anope::string entryname = entry.path().string();
			if (entryname.compare(0, backupbase.length(), backupbase) != 0)
				continue; // Not one of our backups.

			if (!Anope::Match(entryname.substr(backupbase.length()), glob))
				continue; // Not this type of backup.

			old_backups.insert(entryname);
			if (old_backups.size() <= backups)
				continue;

			Log(LOG_DEBUG) << "Deleting expired backup " << *old_backups.begin();
			if (!fs::remove(old_backups.begin()->str(), ec))
			{
				Log(this) << "Failed to delete expired backup " << *old_backups.begin() << ": " << ec.message();
				break;
			}
			old_backups.erase(old_backups.begin());
		}
	}

	Anope::string GetDatabaseFile(Module *mod)
	{
		Anope::string filename;
		if (mod)
		{
			// We are reading the name of a module database.
			filename = Anope::Template(Config->GetModule(this).Get<Anope::string>("module_database", "{name}.module.json"), {
				{ "name", mod->name },
			});
		}
		else
		{
			// We are reading a the name of the core database.
			filename = Config->GetModule(this).Get<Anope::string>("database", "anope.json");
		}

		return Anope::ExpandData(filename);
	}

	void BackupDatabase(const Anope::string &dbname)
	{
		std::error_code ec;
		fs::path dbpath(dbname.str());
		if (!fs::exists(dbpath, ec) || ec)
			return; // Nothing to backup.

		auto &modconf = Config->GetModule(this);
		auto daily_backups = modconf.Get<size_t>("daily_backups", "7");
		auto monthly_backups = modconf.Get<size_t>("monthly_backups", "3");
		if (!daily_backups && !monthly_backups)
			return; // No backups.

		auto backupdir = Anope::ExpandData(modconf.Get<Anope::string>("backup_directory", "backups"));
		if (!fs::is_directory(backupdir.str(), ec) && !ec)
		{
			fs::create_directories(backupdir.str(), ec);
			if (ec)
			{
				Log(this) << "Failed to create backup directory: " << ec.message();
				if (!modconf.Get<bool>("ignore_backup_failure"))
				{
					Anope::Quitting = true;
					Anope::QuitReason = "Failed to create backup directory: " + ec.message();
				}
				return;
			}
		}

		CreateBackup(backupdir, dbpath, daily_backups, "%Y-%m-%d", "\?\?\?\?-\?\?-\?\?");
		CreateBackup(backupdir, dbpath, monthly_backups, "%Y-%m", "\?\?\?\?-\?\?");
	}

	void LoadType(Serialize::Type *s_type, DBData &data)
	{
		auto entries = data.equal_range(s_type->GetName());
		if (entries.first == entries.second)
			return;

		for (auto it = entries.first; it != entries.second; ++it)
			s_type->Unserialize(nullptr, it->second);
	}

	std::optional<DBData> ReadDatabase(const Anope::string &dbname)
	{
		yyjson_read_err errmsg;
		const auto flags = YYJSON_READ_ALLOW_TRAILING_COMMAS | YYJSON_READ_ALLOW_INVALID_UNICODE;
		auto *doc = yyjson_read_file(dbname.c_str(), flags, nullptr, &errmsg);
		if (!doc)
		{
			Log(this) << "Unable to read " << dbname << ": error #" << errmsg.code << ": " << errmsg.msg;
			return std::nullopt;
		}

		auto *root = yyjson_doc_get_root(doc);
		if (!yyjson_is_obj(root))
		{
			Log(this) << "Unable to read " << dbname << ": root element is not an object";
			return std::nullopt;
		}

		auto version = yyjson_get_uint(yyjson_obj_get(root, "version"));
		if (version && version != ANOPE_DATABASE_VERSION)
		{
			Log(this) << "Refusing to load an unsupported database version: " << version;
			return std::nullopt;
		}

		auto generator = yyjson_get_astr(yyjson_obj_get(root, "generator"));
		auto updated = yyjson_get_uint(yyjson_obj_get(root, "updated"));
		Log(LOG_DEBUG) << "Database " << dbname << " was generated on " << Anope::strftime(updated) << " by " << generator;

		auto *data = yyjson_obj_get(root, "data");
		if (!data || !yyjson_is_obj(data))
		{
			Log(this) << "Unable to read " << dbname << ": data element is missing or not an object";
			return std::nullopt;
		}

		DBData ret;

		size_t idx, max;
		yyjson_val *key, *val;
		yyjson_obj_foreach(data, idx, max, key, val)
		{
			if (!yyjson_is_str(key))
			{
				Log(this) << "Unable to read part of " << dbname << ": key of data element #" << idx << " is not a string";
				continue;
			}

			auto keystr = yyjson_get_astr(key);
			if (!yyjson_is_arr(val))
			{
				Log(this) << "Unable to read part of " << dbname << ": " << keystr << " value of data element #" << idx << " is not an array";
				continue;
			}

			Log(LOG_DEBUG) << "Loading " << yyjson_arr_size(val) << " " << keystr << " records";

			size_t idx, max;
			yyjson_val *elem;
			yyjson_arr_foreach(val, idx, max, elem)
			{
				Data ld(elem);
				ret.emplace(keystr, std::move(ld));
			}
		}

		yyjson_doc_free(doc);

		return ret;
	}

	void SaveType(yyjson_mut_doc *doc, yyjson_mut_val *obj, const Anope::string &typestr, const Data &data)
	{
		auto *type = yyjson_mut_obj_getn(obj, typestr.c_str(), typestr.length());
		if (!type || !yyjson_mut_is_arr(type))
		{
			// We haven't seen this element before.
			type = yyjson_mut_arr(doc);
			yyjson_mut_obj_add_val(doc, obj, typestr.c_str(), type);
		}

		auto *elem = yyjson_mut_obj(doc);
		if (data.id)
			yyjson_mut_obj_add_uint(doc, elem, "@id", data.id);

		for (const auto &[key, value] : data.data)
		{
			yyjson_mut_val *v;
			switch (data.GetType(key))
			{
				case Serialize::DataType::BOOL:
					v = yyjson_mut_bool(doc, Anope::Convert<bool>(value.str(), false));
					break;
				case Serialize::DataType::FLOAT:
					v = yyjson_mut_real(doc, Anope::Convert<double>(value.str(), 0.0));
					break;
				case Serialize::DataType::INT:
					v = yyjson_mut_int(doc, Anope::Convert<int64_t>(value.str(), 0));
					break;
				case Serialize::DataType::TEXT:
				{
					auto str = value.str();
					v = str.empty() ? yyjson_mut_null(doc) : yyjson_mut_strncpy(doc, str.c_str(), str.length());
					break;
				}
				case Serialize::DataType::UINT:
					v = yyjson_mut_uint(doc, Anope::Convert<uint64_t>(value.str(), 0));
					break;
			}

			auto *k = yyjson_mut_strncpy(doc, key.c_str(), key.length());
			yyjson_mut_obj_add(elem, k, v);
		}

		yyjson_mut_arr_add_val(type, elem);
	}

public:
	DBJSON(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, DATABASE | VENDOR)
	{
	}

	EventReturn OnLoadDatabase() override
	{
		auto dbname = GetDatabaseFile(nullptr);

		auto db = ReadDatabase(dbname);
		if (!db)
			return EVENT_STOP;

		for (const auto &type : Serialize::Type::GetTypeOrder())
		{
			auto *s_type = Serialize::Type::Find(type);
			if (s_type && !s_type->GetOwner())
				LoadType(s_type, db.value());
		}

		loaded = true;
		return EVENT_STOP;
	}

	void OnSaveDatabase() override
	{
		// Step 1: clear the old data.
		for (const auto &type : Serialize::Type::GetTypeOrder())
		{
			auto *s_type = Serialize::Type::Find(type);
			if (!s_type)
				continue; // Provider has been unloaded.

			auto it = databases.find(s_type->GetOwner());
			if (it == databases.end())
			{
				databases.emplace(s_type->GetOwner(), DBData());
				continue; // We just need to create for this type.
			}

			// As this type has been written before we need to clear the entries
			// from the previous writes so we can update it in the next loop. We
			// have to do it this way so we don't purge any entries for unloaded
			// modules.
			it->second.erase(s_type->GetName());
		}

		// Step 2: store the new data.
		for (auto *item : Serializable::GetItems())
		{
			auto *s_type = item->GetSerializableType();
			if (!s_type)
				continue; // Provider has been unloaded.

			// This should always be found because we create it in the previous step.
			auto it = databases.find(s_type->GetOwner());
			if (it != databases.end())
				it->second.emplace(s_type->GetName(), Data(s_type, item));
		}

		// Step 3: serialize to JSON.
		for (auto &[mod, database] : databases)
		{
			auto dbname = GetDatabaseFile(mod);
			BackupDatabase(dbname);

			auto *doc = yyjson_mut_doc_new(nullptr);

			auto *root = yyjson_mut_obj(doc);
			yyjson_mut_doc_set_root(doc, root);

			const auto generator = "Anope " + Anope::Version() + " " + Anope::VersionBuildString();
			yyjson_mut_obj_add_strncpy(doc, root, "generator", generator.c_str(), generator.length());
			yyjson_mut_obj_add_uint(doc, root, "version", ANOPE_DATABASE_VERSION);
			yyjson_mut_obj_add_int(doc, root, "updated", Anope::CurTime);

			auto *data = yyjson_mut_obj(doc);
			yyjson_mut_obj_add_val(doc, root, "data", data);

			for (const auto &[name, items] : database)
				SaveType(doc, data, name, items);

			Log(LOG_DEBUG) << "Writing " << dbname;

			yyjson_write_err errmsg;
			const auto flags = YYJSON_WRITE_ALLOW_INVALID_UNICODE | YYJSON_WRITE_NEWLINE_AT_END | YYJSON_WRITE_PRETTY;
			if (!yyjson_mut_write_file(dbname.c_str(), doc, flags, nullptr, &errmsg))
			{
				Log(this) << "Unable to write " << dbname << ": error #" << errmsg.code << ": " << errmsg.msg;
				// TODO: exit??? retry???
			}

			yyjson_mut_doc_free(doc);
		}
	}

	void OnSerializeTypeCreate(Serialize::Type *s_type) override
	{
		if (!loaded)
			return; // We are creating on boot.

		auto it = databases.find(s_type->GetOwner());
		if (it == databases.end())
		{
			// The database for this type hasn't been loaded yet.
			const auto dbname = GetDatabaseFile(s_type->GetOwner());

			auto db = ReadDatabase(dbname);
			if (!db)
				return; // Not much we can do here.

			it = databases.emplace(s_type->GetOwner(), std::move(db.value())).first;
		}

		LoadType(s_type, it->second);
	}
};

MODULE_INIT(DBJSON)

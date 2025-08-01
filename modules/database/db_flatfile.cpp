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

class LoadData final
	: public Serialize::Data
{
public:
	std::fstream *fs;
	Serializable::Id id = 0;
	std::map<Anope::string, Anope::string> data;
	std::stringstream ss;
	bool read = false;

	LoadData(std::fstream &fsref)
		: fs(&fsref)
	{
	}

	std::iostream &operator[](const Anope::string &key) override
	{
		if (!read)
		{
			for (Anope::string token; std::getline(*this->fs, token.str());)
			{
				if (token.find("ID ") == 0)
				{
					this->id = Anope::Convert<Serializable::Id>(token.substr(3), 0);
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
{
private:
	bool loaded = false;

public:
	DBFlatFile(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, DATABASE | DEPRECATED | VENDOR)
	{
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

		LoadData ld(fd);
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

		LoadData ld(fd);
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

// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"
#include "modules/sql.h"

class SQLLog final
	: public Module
{
	std::set<Anope::string> inited;
	Anope::string table;

public:
	SQLLog(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR | EXTRA)
	{
	}

	void OnReload(Configuration::Conf &conf) override
	{
		const auto &config = conf.GetModule(this);
		this->table = config.Get<const Anope::string>("table", "logs");
	}

	void OnLogMessage(LogInfo *li, const Log *l, const Anope::string &msg) override
	{
		Anope::string ref_name;
		ServiceReference<SQL::Provider> SQL;

		for (const auto &target : li->targets)
		{
			size_t sz = target.find("sql_log:");
			if (!sz)
			{
				ref_name = target.substr(8);
				SQL = ServiceReference<SQL::Provider>("SQL::Provider", ref_name);
				break;
			}
		}

		if (!SQL)
			return;

		if (!inited.count(ref_name))
		{
			inited.insert(ref_name);

			SQL::Query create("CREATE TABLE IF NOT EXISTS `" + table + "` ("
						"`date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,"
						"`type` varchar(64) NOT NULL,"
						"`user` varchar(64) NOT NULL,"
						"`acc` varchar(64) NOT NULL,"
						"`command` varchar(64) NOT NULL,"
						"`channel` varchar(64) NOT NULL,"
						"`msg` text NOT NULL"
					")");

			SQL->Run(NULL, create);
		}

		SQL::Query insert("INSERT INTO `" + table + "` (`type`,`user`,`acc`,`command`,`channel`,`msg`)"
			"VALUES (@type@, @user@, @acc@, @command@, @channel@, @msg@)");

		switch (l->type)
		{
			case LOG_ADMIN:
				insert.SetValue("type", "ADMIN");
				break;
			case LOG_OVERRIDE:
				insert.SetValue("type", "OVERRIDE");
				break;
			case LOG_COMMAND:
				insert.SetValue("type", "COMMAND");
				break;
			case LOG_SERVER:
				insert.SetValue("type", "SERVER");
				break;
			case LOG_CHANNEL:
				insert.SetValue("type", "CHANNEL");
				break;
			case LOG_USER:
				insert.SetValue("type", "USER");
				break;
			case LOG_MODULE:
				insert.SetValue("type", "MODULE");
				break;
			case LOG_NORMAL:
				insert.SetValue("type", "NORMAL");
				break;
			default:
				return;
		}

		insert.SetValue("user", l->u ? l->u->nick : "");
		insert.SetValue("acc", l->nc ? l->nc->display : "");
		insert.SetValue("command", l->c ? l->c->name : "");
		insert.SetValue("channel", l->ci ? l->ci->name : "");
		insert.SetValue("msg", msg);

		SQL->Run(NULL, insert);
	}
};

MODULE_INIT(SQLLog)

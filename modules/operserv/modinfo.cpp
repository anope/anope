/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
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

class CommandOSModInfo : public Command
{
 public:
	CommandOSModInfo(Module *creator) : Command(creator, "operserv/modinfo", 1, 1)
	{
		this->SetDesc(_("Info about a loaded module"));
		this->SetSyntax(_("\037modname\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &file = params[0];

		Log(LOG_ADMIN, source, this) << "on " << file;

		Module *m = ModuleManager::FindModule(file);
		if (m)
		{
			source.Reply(_("Module: \002{0}\002 Version: \002{1}\002 Author: \002{2}\002 Loaded: \002{3}\002"), m->name, !m->version.empty() ? m->version : "?", !m->author.empty() ? m->author : "Unknown", Anope::strftime(m->created, source.GetAccount()));
			if (Anope::Debug)
				source.Reply(_(" Loaded at: {0}"), Anope::printf("0x%x", m->handle));

			std::vector<Command *> commands = ServiceManager::Get()->FindServices<Command *>();
			for (Command *c : commands)
			{
				if (c->GetOwner() != m)
					continue;

				source.Reply(_("   Providing service: \002{0}\002"), c->GetName());

				for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
				{
					User *u = it->second;

					if (u->type != UserType::BOT)
						continue;

					ServiceBot *bi = anope_dynamic_static_cast<ServiceBot *>(u);

					for (CommandInfo::map::const_iterator cit = bi->commands.begin(), cit_end = bi->commands.end(); cit != cit_end; ++cit)
					{
						const Anope::string &c_name = cit->first;
						const CommandInfo &info = cit->second;
						if (info.name != c->GetName())
							continue;
						source.Reply(_("   Command \002{0}\002 on \002{1}\002 is linked to \002{2}\002"), c_name, bi->nick, c->GetName());
					}
				}
			}
		}
		else
			source.Reply(_("No information about module \002{0}\002 is available."), file);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("This command lists information about the specified loaded module."));
		return true;
	}
};

class CommandOSModList : public Command
{
 public:
	CommandOSModList(Module *creator) : Command(creator, "operserv/modlist", 0, 1)
	{
		this->SetDesc(_("List loaded modules"));
		this->SetSyntax("[all|third|vendor|extra|database|encryption|pseudoclient|protocol]");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &param = !params.empty() ? params[0] : "";

		if (!param.empty())
			Log(LOG_ADMIN, source, this) << "for " << param;
		else
			Log(LOG_ADMIN, source, this);

		bool third = false, vendor = false, extra = false, database = false, encryption = false, pseudoclient = false, protocol = false;

		if (param.equals_ci("all"))
			third = vendor = extra = database = encryption = pseudoclient = protocol = true;
		else if (param.equals_ci("third"))
			third = true;
		else if (param.equals_ci("vendor"))
			vendor = true;
		else if (param.equals_ci("extra"))
			extra = true;
		else if (param.equals_ci("database"))
			database = true;
		else if (param.equals_ci("encryption"))
			encryption = true;
		else if (param.equals_ci("pseudoclient"))
			pseudoclient = true;
		else if (param.equals_ci("protocol"))
			protocol = true;
		else
			third = extra = database = encryption = protocol = true;

		Module *protomod = ModuleManager::FindFirstOf(PROTOCOL);

		source.Reply(_("Current module list:"));

		int count = 0;
		for (std::list<Module *>::iterator it = ModuleManager::Modules.begin(), it_end = ModuleManager::Modules.end(); it != it_end; ++it)
		{
			Module *m = *it;

			bool show = false;
			Anope::string mtype;

			if (m->type & PROTOCOL)
			{
				show |= protocol;
				if (!mtype.empty())
					mtype += ", ";
				mtype += "Protocol";
			}
			if (m->type & PSEUDOCLIENT)
			{
				show |= pseudoclient;
				if (!mtype.empty())
					mtype += ", ";
				mtype += "Pseudoclient";
			}
			if (m->type & ENCRYPTION)
			{
				show |= encryption;
				if (!mtype.empty())
					mtype += ", ";
				mtype += "Encryption";
			}
			if (m->type & DATABASE)
			{
				show |= database;
				if (!mtype.empty())
					mtype += ", ";
				mtype += "Database";
			}
			if (m->type & EXTRA)
			{
				show |= extra;
				if (!mtype.empty())
					mtype += ", ";
				mtype += "Extra";
			}
			if (m->type & VENDOR)
			{
				show |= vendor;
				if (!mtype.empty())
					mtype += ", ";
				mtype += "Vendor";
			}
			if (m->type & THIRD)
			{
				show |= third;
				if (!mtype.empty())
					mtype += ", ";
				mtype += "Third";
			}

			if (!show)
				continue;
			else if (m->type & PROTOCOL && param.empty() && m != protomod)
				continue;

			++count;

			source.Reply(_("Module: \002{0}\002 [{1}] [{2}]"), m->name, m->version, mtype);
		}

		if (!count)
			source.Reply(_("No modules currently loaded matching that criteria."));
		else
			source.Reply(_("\002{0}\002 modules loaded."), count);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Lists currently loaded modules."));
		return true;
	}
};

class OSModInfo : public Module
{
	CommandOSModInfo commandosmodinfo;
	CommandOSModList commandosmodlist;

 public:
	OSModInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandosmodinfo(this)
		, commandosmodlist(this)
	{

	}
};

MODULE_INIT(OSModInfo)

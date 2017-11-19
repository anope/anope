/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
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

class CommandOSOper : public Command
{
	bool HasPrivs(CommandSource &source, OperType *ot) const
	{
		std::list<Anope::string> commands = ot->GetCommands(), privs = ot->GetPrivs();

		for (std::list<Anope::string>::iterator it = commands.begin(); it != commands.end(); ++it)
			if (!source.HasCommand(*it))
				return false;

		for (std::list<Anope::string>::iterator it = privs.begin(); it != privs.end(); ++it)
			if (!source.HasPriv(*it))
				return false;

		return true;
	}

 public:
	CommandOSOper(Module *creator) : Command(creator, "operserv/oper", 1, 3)
	{
		this->SetDesc(_("View and change Services Operators"));
		this->SetSyntax(_("ADD \037oper\037 \037type\037"));
		this->SetSyntax(_("DEL \037oper\037"));
		this->SetSyntax(_("INFO [\037type\037]"));
		this->SetSyntax("LIST");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &subcommand = params[0];

		if (subcommand.equals_ci("ADD") && params.size() > 2)
		{
			const Anope::string &oper = params[1];
			const Anope::string &otype = params[2];

			if (!source.HasPriv("operserv/oper/modify"))
			{
				source.Reply(_("Access denied. You do not have the operator privilege \002{0}\002."), "operserv/oper/modify");
				return;
			}

			NickServ::Nick *na = NickServ::FindNick(oper);
			if (na == NULL)
			{
				source.Reply(_("\002{0}\002 isn't currently online."), oper);
				return;
			}

			OperType *ot = OperType::Find(otype);
			if (ot == NULL)
			{
				source.Reply(_("Oper type \002{0}\002 has not been configured."), otype);
				return;
			}

			if (!HasPrivs(source, ot))
			{
				source.Reply(_("Access denied."));
				return;
			}

			Oper *o = na->GetAccount()->GetOper();
			if (o != nullptr)
			{
				o->Delete();
			}

			o = Serialize::New<Oper *>();
			o->SetName(na->GetAccount()->GetDisplay());
			o->SetType(ot);
			o->SetRequireOper(true);

			na->GetAccount()->SetOper(o);

			if (Anope::ReadOnly)
				source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

			logger.Admin(source, _("{source} used {command} to add {0} as an oper of type {1}"), na->GetNick(), ot->GetName());

			source.Reply("\002{0}\002 (\002{1}\002) added to the \002{2}\002 list.", na->GetNick(), na->GetAccount()->GetDisplay(), ot->GetName());
		}
		else if (subcommand.equals_ci("DEL") && params.size() > 1)
		{
			const Anope::string &oper = params[1];

			if (!source.HasPriv("operserv/oper/modify"))
			{
				source.Reply(_("Access denied. You do not have the operator privilege \002{0}\002."), "operserv/oper/modify");
				return;
			}

			NickServ::Nick *na = NickServ::FindNick(oper);
			if (na == nullptr || na->GetAccount() == nullptr)
			{
				source.Reply(_("\002{0}\002 isn't registered."), oper);
				return;
			}

			Oper *o = na->GetAccount()->GetOper();

			if (o == nullptr)
			{
				source.Reply(_("Nick \002{0}\002 is not a Services Operator."), oper);
				return;
			}

			if (!HasPrivs(source, o->GetType()))
			{
				source.Reply(_("Access denied."));
				return;
			}


			o->Delete();

			if (Anope::ReadOnly)
				source.Reply(_("Services are in read-only mode. Any changes made may not persist."));

			logger.Admin(source, _("{source} used {command} to remove {0}"), na->GetNick());

			source.Reply(_("Oper privileges removed from \002{0}\002 (\002{1}\002)."), na->GetNick(), na->GetAccount()->GetDisplay());
		}
		else if (subcommand.equals_ci("LIST"))
		{
			source.Reply(_("Name     Type"));
			for (NickServ::Account *nc : NickServ::service->GetAccountList())
			{
				Oper *oper = nc->GetOper();

				if (oper == nullptr)
					continue;

				source.Reply(Anope::printf("%-8s %s", oper->GetName().c_str(), oper->GetType()->GetName().c_str()));
				for (User *u : nc->users)
					source.Reply(_("   \002{0}\002 is online using this oper block."), u->nick);
			}
		}
		else if (subcommand.equals_ci("INFO"))
		{
			if (params.size() < 2)
			{
				source.Reply(_("Available opertypes:"));
				for (unsigned i = 0; i < Config->MyOperTypes.size(); ++i)
				{
					OperType *ot = Config->MyOperTypes[i];
					source.Reply("%s", ot->GetName().c_str());
				}
				return;
			}

			Anope::string fulltype = params[1];
			if (params.size() > 2)
				fulltype += " " + params[2];
			OperType *ot = OperType::Find(fulltype);
			if (ot == NULL)
			{
				source.Reply(_("Oper type \002{0}\002 has not been configured."), fulltype);
				return;
			}

			if (ot->GetCommands().empty())
			{
				source.Reply(_("Opertype \002{0}\002 has no allowed commands."), ot->GetName());
			}
			else
			{
				source.Reply(_("Available commands for \002{0}\002:"), ot->GetName());
				Anope::string buf;
				std::list<Anope::string> cmds = ot->GetCommands();
				for (std::list<Anope::string>::const_iterator it = cmds.begin(), it_end = cmds.end(); it != it_end; ++it)
				{
					buf += *it + " ";
					if (buf.length() > 400)
					{
						source.Reply("%s", buf.c_str());
						buf.clear();
					}
				}
				if (!buf.empty())
				{
					source.Reply("%s", buf.c_str());
					buf.clear();
				}
			}

			if (ot->GetPrivs().empty())
			{
				source.Reply(_("Opertype \002{0}\002 has no allowed privileges."), ot->GetName());
			}
			else
			{
				source.Reply(_("Available privileges for \002{0}\002:"), ot->GetName());
				Anope::string buf;
				std::list<Anope::string> privs = ot->GetPrivs();
				for (std::list<Anope::string>::const_iterator it = privs.begin(), it_end = privs.end(); it != it_end; ++it)
				{
					buf += *it + " ";
					if (buf.length() > 400)
					{
						source.Reply("%s", buf.c_str());
						buf.clear();
					}
				}
				if (!buf.empty())
				{
					source.Reply("%s", buf.c_str());
					buf.clear();
				}
			}

			if (!ot->modes.empty())
				source.Reply(_("Opertype \002{0}\002 receives modes \002{1}\002 once identified."), ot->GetName(), ot->modes);
		}
		else
		{
			this->OnSyntaxError(source);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Allows you to change and view Services Operators.\n"
				" Note that operators removed by this command but are still set in the configuration file are not permanently affected by this."));
		return true;
	}
};

class OSOper : public Module
{
	CommandOSOper commandosoper;

 public:
	OSOper(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandosoper(this)
	{
	}
};

MODULE_INIT(OSOper)

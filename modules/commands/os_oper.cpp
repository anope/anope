/* OperServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

struct MyOper : Oper, Serializable
{
	MyOper(const Anope::string &n, OperType *o) : Oper(n, o) { }

	Anope::string serialize_name() const
	{
		return "Oper";
	}

	serialized_data serialize()
	{
		serialized_data data;

		data["name"] << this->name;
		data["type"] << this->ot->GetName();

		return data;
	}

	static void unserialize(serialized_data &data)
	{
		OperType *ot = OperType::Find(data["type"].astr());
		if (ot == NULL)
			return;
		NickCore *nc = findcore(data["name"].astr());
		if (nc == NULL)
			return;

		nc->o = new MyOper(nc->display, ot);
		Log(LOG_NORMAL, "operserv/oper") << "Tied oper " << nc->display << " to type " << ot->GetName();
	}
};

class CommandOSOper : public Command
{
 public:
	CommandOSOper(Module *creator) : Command(creator, "operserv/oper", 1, 3)
	{
		this->SetDesc(_("View and change services operators"));
		this->SetSyntax(_("ADD \037oper\037 \037type\037"));
		this->SetSyntax(_("DEL [\037oper\037]"));
		this->SetSyntax(_("INFO [\037type\037]"));
		this->SetSyntax(_("LIST"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &subcommand = params[0];

		if (subcommand.equals_ci("ADD") && params.size() > 2)
		{
			const Anope::string &oper = params[1];
			const Anope::string &otype = params[2];

			NickAlias *na = findnick(oper);
			if (na == NULL)
				source.Reply(NICK_X_NOT_REGISTERED, oper.c_str());
			else if (na->nc->o)
				source.Reply(_("Nick \2%s\2 is already an operator."), na->nick.c_str());
			else
			{
				OperType *ot = OperType::Find(otype);
				if (ot == NULL)
					source.Reply(_("Oper type \2%s\2 has not been configured."), otype.c_str());
				else
				{
					na->nc->o = new MyOper(na->nc->display, ot);

					Log(LOG_ADMIN, source.u, this) << "ADD " << na->nick << " as type " << ot->GetName();
					source.Reply("%s (%s) added to the \2%s\2 list.", na->nick.c_str(), na->nc->display.c_str(), ot->GetName().c_str());
				}
			}
		}
		else if (subcommand.equals_ci("DEL") && params.size() > 1)
		{
			const Anope::string &oper = params[1];

			NickAlias *na = findnick(oper);
			if (na == NULL)
				source.Reply(NICK_X_NOT_REGISTERED, oper.c_str());
			else if (!na->nc || !na->nc->o)
				source.Reply(_("Nick \2%s\2 is not a services operator."), oper.c_str());
			else
			{
				delete na->nc->o;
				na->nc->o = NULL;

				Log(LOG_ADMIN, source.u, this) << "DEL " << na->nick;
				source.Reply(_("Oper privileges removed from %s (%s)."), na->nick.c_str(), na->nc->display.c_str());
			}
		}
		else if (subcommand.equals_ci("LIST"))
		{
			source.Reply(_("Name     Type"));
			for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
			{
				NickCore *nc = it->second;

				if (!nc->o)
					continue;

				source.Reply(_("%-8s %s"), nc->o->name.c_str(), nc->o->ot->GetName().c_str());
				if (nc->o->config)
					source.Reply(_("   This oper is configured in the configuration file."));
				for (std::list<User *>::iterator uit = nc->Users.begin(); uit != nc->Users.end(); ++uit)
				{
					User *u = *uit;
					source.Reply(_("   %s is online using this oper block."), u->nick.c_str());
				}
			}
		}
		else if (subcommand.equals_ci("INFO") && params.size() > 1)
		{
			Anope::string fulltype = params[1];
			if (params.size() > 2)
				fulltype += " " + params[2];
			OperType *ot = OperType::Find(fulltype);
			if (ot == NULL)	
				source.Reply(_("Oper type \2%s\2 has not been configured."), fulltype.c_str());
			else
			{
				if (ot->GetCommands().empty())
					source.Reply(_("Opertype \2%s\2 has no allowed commands."), ot->GetName().c_str());
				else
				{
					source.Reply(_("Available commands for \2%s\2:"), ot->GetName().c_str());
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
					source.Reply(_("Opertype \2%s\2 has no allowed privileges."), ot->GetName().c_str());
				else
				{
					source.Reply(_("Available privileges for \2%s\2:"), ot->GetName().c_str());
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
					source.Reply(_("Opertype \2%s\2 receives modes \2%s\2 once identifying."), ot->GetName().c_str(), ot->modes.c_str());
			}
		}
		else
			this->OnSyntaxError(source, subcommand);

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to change and view services operators.\n"
				"Note that operators removed by this command but are still set in\n"
				"the configuration file are not permanently affected by this."));
		return true;
	}
};

class OSOper : public Module
{
	SerializeType myoper_type;
	CommandOSOper commandosoper;

 public:
	OSOper(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		myoper_type("Oper", MyOper::unserialize), commandosoper(this)
	{
		this->SetAuthor("Anope");
	}

	~OSOper()
	{
		for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
		{
			NickCore *nc = it->second;

			if (nc->o && !nc->o->config)
				delete nc->o;
		}
	}
};

MODULE_INIT(OSOper)

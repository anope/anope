/* OperServ core functions
 *
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
#include "operserv.h"

class CommandOSStaff : public Command
{
 public:
	CommandOSStaff() : Command("STAFF", 0, 0, "operserv/staff")
	{
		this->SetDesc(_("Display Services staff and online status"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		source.Reply(_("On Level Nick"));

		for (unsigned i = 0; i < Config->Opers.size(); ++i)
		{
			Oper *o = Config->Opers[i];

			NickAlias *na = findnick(o->name);
			if (na)
			{
				NickCore *nc = na->nc;
				for (std::list<User *>::iterator uit = nc->Users.begin(); uit != nc->Users.end(); ++uit)
				{
					User *u2 = *uit;

					if (na->nick.equals_ci(u2->nick))
						source.Reply(_(" %c %s  %s"), '*', o->ot->GetName().c_str(), u2->nick.c_str());
					else
						source.Reply(_(" %c %s  %s [%s]"), '*', o->ot->GetName().c_str(), na->nick.c_str(), u2->nick.c_str());
				}
				if (nc->Users.empty())
					source.Reply(_(" %c %s  %s"), ' ', o->ot->GetName().c_str(), na->nick.c_str());
			}
		}

		source.Reply(_(END_OF_ANY_LIST), "Staff");
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002STAFF\002\n"
				"Displays all Services Staff nicks along with level\n"
				"and on-line status."));
		return true;
	}
};

class OSStaff : public Module
{
	CommandOSStaff commandosstaff;

 public:
	OSStaff(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!operserv)
			throw ModuleException("OperServ is not loaded!");

		this->AddCommand(operserv->Bot(), &commandosstaff);
	}
};

MODULE_INIT(OSStaff)

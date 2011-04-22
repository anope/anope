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

class CommandOSMode : public Command
{
 public:
	CommandOSMode() : Command("MODE", 2, 2, "operserv/mode")
	{
		this->SetDesc(_("Change channel or user modes"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &target = params[0];
		const Anope::string &modes = params[1];

		if (target[0] == '#')
		{
			Channel *c = findchan(target);
			if (!c)
				source.Reply(_(CHAN_X_NOT_IN_USE), target.c_str());
			else if (c->bouncy_modes)
				source.Reply(_("Services is unable to change modes. Are your servers' U:lines configured correctly?"));
			else
			{
				c->SetModes(operserv->Bot(), false, modes.c_str());

				Log(LOG_ADMIN, u, this) << modes << " on " << target;
			}
		}
		else
		{
			User *u2 = finduser(target);
			if (!u2)
				source.Reply(_(NICK_X_NOT_IN_USE), target.c_str());
			else
			{
				u2->SetModes(operserv->Bot(), "%s", modes.c_str());
				source.Reply(_("Changed usermodes of \002%s\002 to %s."), u2->nick.c_str(), modes.c_str());

				u2->SendMessage(operserv->Bot(), _("\002%s\002 changed your usermodes to %s."), u->nick.c_str(), modes.c_str());

				Log(LOG_ADMIN, u, this) << modes << " on " << target;
			}
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002MODE {\037channel\037|\037user\037} \037modes\037\002\n"
				" \n"
				"Allows Services operators to change modes for any channel or\n"
				"user. Parameters are the same as for the standard /MODE\n"
				"command."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "MODE", _("MODE {\037channel\037|\037user\037} \037modes\037"));
	}
};

class OSMode : public Module
{
	CommandOSMode commandosmode;

 public:
	OSMode(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		if (!operserv)
			throw ModuleException("OperServ is not loaded!");

		this->AddCommand(operserv->Bot(), &commandosmode);
	}
};

MODULE_INIT(OSMode)

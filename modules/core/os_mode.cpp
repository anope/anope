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

class CommandOSMode : public Command
{
 public:
	CommandOSMode() : Command("MODE", 2, 2, "operserv/mode")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &chan = params[0];
		const Anope::string &modes = params[1];
		Channel *c;

		if (!(c = findchan(chan)))
			source.Reply(LanguageString::CHAN_X_NOT_IN_USE, chan.c_str());
		else if (c->bouncy_modes)
			source.Reply(_("Services is unable to change modes. Are your servers' U:lines configured correctly?"));
		else
		{
			c->SetModes(OperServ, false, modes.c_str());

			if (Config->WallOSMode)
				ircdproto->SendGlobops(OperServ, "%s used MODE %s on %s", u->nick.c_str(), modes.c_str(), chan.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002MODE \037channel\037 \037modes\037\002\n"
				" \n"
				"Allows Services operators to set channel modes for any\n"
				"channel. Parameters are the same as for the standard /MODE\n"
				"command."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "MODE", _("MODE \037channel\037 \037modes\037"));
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    MODE        Change a channel's modes"));
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

		this->AddCommand(OperServ, &commandosmode);
	}
};

MODULE_INIT(OSMode)

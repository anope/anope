/* ChanServ core functions
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
#include "chanserv.h"

class CommandCSGetKey : public Command
{
 public:
	CommandCSGetKey() : Command("GETKEY", 1, 1)
	{
		this->SetDesc(_("Returns the key of the given channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];

		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (!check_access(u, ci, CA_GETKEY) && !u->HasCommand("chanserv/getkey"))
		{
			source.Reply(_(ACCESS_DENIED));
			return MOD_CONT;
		}

		Anope::string key;
		if (!ci->c || !ci->c->GetParam(CMODE_KEY, key))
		{
			source.Reply(_("The channel \002%s\002 has no key."), chan.c_str());
			return MOD_CONT;
		}

		bool override = !check_access(u, ci, CA_GETKEY);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci);

		source.Reply(_("Key for channel \002%s\002 is \002%s\002."), chan.c_str(), key.c_str());
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002GETKEY \037channel\037\002\n"
				" \n"
				"Returns the key of the given channel."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "GETKEY", _("GETKEY \037channel\037"));
	}
};

class CSGetKey : public Module
{
	CommandCSGetKey commandcsgetkey;

 public:
	CSGetKey(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!chanserv)
			throw ModuleException("ChanServ is not loaded!");

		this->AddCommand(chanserv->Bot(), &commandcsgetkey);
	}
};

MODULE_INIT(CSGetKey)

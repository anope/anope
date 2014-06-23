/* ChanServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandCSGetKey : public Command
{
 public:
	CommandCSGetKey(Module *creator) : Command(creator, "chanserv/getkey", 1, 1)
	{
		this->SetDesc(_("Returns the key of the given channel"));
		this->SetSyntax(_("\037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("GETKEY") && !source.HasCommand("chanserv/getkey"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "GETKEY", ci->name);
			return;
		}

		Anope::string key;
		if (!ci->c || !ci->c->GetParam("KEY", key))
		{
			source.Reply(_("Channel \002{0}\002 does not have a key."), ci->name);
			return;
		}

		bool override = !source.AccessFor(ci).HasPriv("GETKEY");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci);

		source.Reply(_("Key for channel \002{0}\002 is \002{1}\002."), ci->name, key);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Returns the key of \037channel\037.\n"
		               "\n"
		               "Use of this command requires the \002{0}\002 privilege on \037channel\037."),
		               "GETKEY");
		return true;
	}
};

class CSGetKey : public Module
{
	CommandCSGetKey commandcsgetkey;

 public:
	CSGetKey(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsgetkey(this)
	{

	}
};

MODULE_INIT(CSGetKey)

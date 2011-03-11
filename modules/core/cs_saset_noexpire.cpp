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

class CommandCSSASetNoexpire : public Command
{
 public:
	CommandCSSASetNoexpire() : Command("NOEXPIRE", 2, 2, "chanserv/saset/noexpire")
	{
		this->SetDesc(_("Prevent the channel from expiring"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSASetNoexpire");

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_NO_EXPIRE);
			source.Reply(_("Channel %s \002will not\002 expire."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_NO_EXPIRE);
			source.Reply(_("Channel %s \002will\002 expire."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "NOEXPIRE");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002SET \037channel\037 NOEXPIRE {ON | OFF}\002\n"
				" \n"
				"Sets whether the given channel will expire.  Setting this\n"
				"to ON prevents the channel from expiring."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		SyntaxError(source, "SET NOEXPIRE", _("SET \037channel\037 NOEXPIRE {ON | OFF}"));
	}
};

class CSSetNoexpire : public Module
{
	CommandCSSASetNoexpire commandcssasetnoexpire;

 public:
	CSSetNoexpire(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetnoexpire);
	}

	~CSSetNoexpire()
	{
		Command *c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetnoexpire);
	}
};

MODULE_INIT(CSSetNoexpire)

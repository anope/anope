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

class CommandCSSetDescription : public Command
{
 public:
	CommandCSSetDescription(const Anope::string &cpermission = "") : Command("DESC", 2, 2, cpermission)
	{
		this->SetDesc(_("Set the channel description"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetDescription");

		ci->desc = params[1];

		source.Reply(_("Description of %s changed to \002%s\002."), ci->name.c_str(), ci->desc.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002%s \037channel\037 DESC \037description\037\002\n"
				" \n"
				"Sets the description for the channel, which shows up with\n"
				"the \002LIST\002 and \002INFO\002 commands."), this->name.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SET", _(CHAN_SET_SYNTAX));
	}
};

class CommandCSSASetDescription : public CommandCSSetDescription
{
 public:
	CommandCSSASetDescription() : CommandCSSetDescription("chanserv/saset/description")
	{
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SASET", _(CHAN_SASET_SYNTAX));
	}
};

class CSSetDescription : public Module
{
	CommandCSSetDescription commandcssetdescription;
	CommandCSSASetDescription commandcssasetdescription;

 public:
	CSSetDescription(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssetdescription);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetdescription);
	}

	~CSSetDescription()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetdescription);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetdescription);
	}
};

MODULE_INIT(CSSetDescription)

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

class CommandCSSetBanType : public Command
{
 public:
	CommandCSSetBanType(const Anope::string &cpermission = "") : Command("BANTYPE", 2, 2, cpermission)
	{
		this->SetDesc("Set how Services make bans on the channel");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetBanType");

		try
		{
			ci->bantype = convertTo<int16>(params[1]);
			source.Reply(_("Ban type for channel %s is now #%d."), ci->name.c_str(), ci->bantype);
		}
		catch (const ConvertException &)
		{
			source.Reply(_("\002%s\002 is not a valid ban type."), params[1].c_str());
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002%s \037channel\037 BANTYPE \037bantype\037\002\n"
				" \n"
				"Sets the ban type that will be used by services whenever\n"
				"they need to ban someone from your channel.\n"
				" \n"
				"bantype is a number between 0 and 3 that means:\n"
				" \n"
				"0: ban in the form *!user@host\n"
				"1: ban in the form *!*user@host\n"
				"2: ban in the form *!*@host\n"
				"3: ban in the form *!*user@*.domain"), this->name.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SET", LanguageString::CHAN_SET_SYNTAX);
	}
};

class CommandCSSASetBanType : public CommandCSSetBanType
{
 public:
	CommandCSSASetBanType() : CommandCSSetBanType("chanserv/saset/bantype")
	{
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SASET", LanguageString::CHAN_SASET_SYNTAX);
	}
};

class CSSetBanType : public Module
{
	CommandCSSetBanType commandcssetbantype;
	CommandCSSASetBanType commandcssasetbantype;

 public:
	CSSetBanType(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssetbantype);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetbantype);
	}

	~CSSetBanType()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetbantype);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetbantype);
	}
};

MODULE_INIT(CSSetBanType)

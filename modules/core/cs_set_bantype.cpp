/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
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
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetBanType");

		Anope::string end;

		int16 bantype = convertTo<int16>(params[1], end, false);

		if (!end.empty() || bantype < 0 || bantype > 3)
			source.Reply(CHAN_SET_BANTYPE_INVALID, params[1].c_str());
		else
		{
			ci->bantype = bantype;
			source.Reply(CHAN_SET_BANTYPE_CHANGED, ci->name.c_str(), ci->bantype);
		}

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_BANTYPE, "SET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SET", CHAN_SET_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_SET_BANTYPE);
	}
};

class CommandCSSASetBanType : public CommandCSSetBanType
{
 public:
	CommandCSSASetBanType() : CommandCSSetBanType("chanserv/saset/bantype")
	{
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(CHAN_HELP_SET_BANTYPE, "SASET");
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SASET", CHAN_SASET_SYNTAX);
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

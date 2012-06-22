/* ChanServ core functions
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

class CommandCSSetBanType : public Command
{
 public:
	CommandCSSetBanType(Module *creator, const Anope::string &cname = "chanserv/set/bantype") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Set how Services make bans on the channel"));
		this->SetSyntax(_("\037channel\037 \037bantype\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		try
		{
			int16_t new_type = convertTo<int16_t>(params[1]);
			if (new_type < 0 || new_type > 3)
				throw ConvertException("Invalid range");
			ci->bantype = new_type;
			source.Reply(_("Ban type for channel %s is now #%d."), ci->name.c_str(), ci->bantype);
		}
		catch (const ConvertException &)
		{
			source.Reply(_("\002%s\002 is not a valid ban type."), params[1].c_str());
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sets the ban type that will be used by services whenever\n"
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
};

class CommandCSSASetBanType : public CommandCSSetBanType
{
 public:
	CommandCSSASetBanType(Module *creator) : CommandCSSetBanType(creator, "chanserv/saset/bantype")
	{
	}
};

class CSSetBanType : public Module
{
	CommandCSSetBanType commandcssetbantype;
	CommandCSSASetBanType commandcssasetbantype;

 public:
	CSSetBanType(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetbantype(this), commandcssasetbantype(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSSetBanType)

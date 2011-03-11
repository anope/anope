/* BotServ core functions
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

class CommandBSSay : public Command
{
 public:
	CommandBSSay() : Command("SAY", 2, 2)
	{
		this->SetDesc(_("Makes the bot say the given text on the given channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &text = params[1];

		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (!check_access(u, ci, CA_SAY))
		{
			source.Reply(_(ACCESS_DENIED));
			return MOD_CONT;
		}

		if (!ci->bi)
		{
			source.Reply(_(BOT_NOT_ASSIGNED));
			return MOD_CONT;
		}

		if (!ci->c || !ci->c->FindUser(ci->bi))
		{
			source.Reply(_(BOT_NOT_ON_CHANNEL), ci->name.c_str());
			return MOD_CONT;
		}

		if (text[0] == '\001')
		{
			this->OnSyntaxError(source, "");
			return MOD_CONT;
		}

		ircdproto->SendPrivmsg(ci->bi, ci->name, "%s", text.c_str());
		ci->bi->lastmsg = Anope::CurTime;

		// XXX need a way to find if someone is overriding this
		Log(LOG_COMMAND, u, this, ci) << text;

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002SAY \037channel\037 \037text\037\002\n"
				" \n"
				"Makes the bot say the given text on the given channel."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SAY", _("SAY \037channel\037 \037text\037"));
	}
};

class BSSay : public Module
{
	CommandBSSay commandbssay;

 public:
	BSSay(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(BotServ, &commandbssay);
	}
};

MODULE_INIT(BSSay)

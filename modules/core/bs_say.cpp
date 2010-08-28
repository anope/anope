/* BotServ core functions
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

class CommandBSSay : public Command
{
 public:
	CommandBSSay() : Command("SAY", 2, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci;

		Anope::string chan = params[0];
		Anope::string text = params[1];

		ci = cs_findchan(chan);

		if (!check_access(u, ci, CA_SAY))
		{
			notice_lang(Config->s_BotServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (!ci->bi)
		{
			notice_help(Config->s_BotServ, u, BOT_NOT_ASSIGNED);
			return MOD_CONT;
		}

		if (!ci->c || !ci->c->FindUser(ci->bi))
		{
			notice_lang(Config->s_BotServ, u, BOT_NOT_ON_CHANNEL, ci->name.c_str());
			return MOD_CONT;
		}

		if (text[0] == '\001')
		{
			this->OnSyntaxError(u, "");
			return MOD_CONT;
		}

		ircdproto->SendPrivmsg(ci->bi, ci->name, "%s", text.c_str());
		ci->bi->lastmsg = time(NULL);

		// XXX need a way to find if someone is overriding this
		Log(LOG_COMMAND, u, this, ci) << text;

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config->s_BotServ, u, BOT_HELP_SAY);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_BotServ, u, "SAY", BOT_SAY_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_BotServ, u, BOT_HELP_CMD_SAY);
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

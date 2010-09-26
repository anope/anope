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

class CommandBSAct : public Command
{
 public:
	CommandBSAct() : Command("ACT", 2, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci = cs_findchan(params[0]);
		Anope::string message = params[1];

		if (!check_access(u, ci, CA_SAY))
		{
			u->SendMessage(BotServ, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (!ci->bi)
		{
			u->SendMessage(BotServ, BOT_NOT_ASSIGNED);
			return MOD_CONT;
		}

		if (!ci->c || !ci->c->FindUser(ci->bi))
		{
			u->SendMessage(BotServ, BOT_NOT_ON_CHANNEL, ci->name.c_str());
			return MOD_CONT;
		}

		size_t i = 0;
		while ((i = message.find(1)) && i != Anope::string::npos)
			message.erase(i, 1);

		ircdproto->SendAction(ci->bi, ci->name, "%s", message.c_str());
		ci->bi->lastmsg = Anope::CurTime;

		// XXX Need to be able to find if someone is overriding this.
		Log(LOG_COMMAND, u, this, ci) << message;

		return MOD_CONT;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(BotServ, u, "ACT", BOT_ACT_SYNTAX);
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(BotServ, BOT_HELP_ACT);
		return true;
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(BotServ, BOT_HELP_CMD_ACT);
	}
};

class BSAct : public Module
{
	CommandBSAct commandbsact;

 public:
	BSAct(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(BotServ, &commandbsact);
	}
};

MODULE_INIT(BSAct)

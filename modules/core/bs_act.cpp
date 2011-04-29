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
#include "botserv.h"

class CommandBSAct : public Command
{
 public:
	CommandBSAct() : Command("ACT", 2, 2)
	{
		this->SetDesc(_("Makes the bot do the equivalent of a \"/me\" command"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Anope::string message = params[1];

		if (!check_access(u, ci, CA_SAY))
		{
			source.Reply(_(ACCESS_DENIED));
			return MOD_CONT;
		}

		if (!ci->bi)
		{
			source.Reply(_(BOT_NOT_ASSIGNED), Config->UseStrictPrivMsgString.c_str(), Config->s_BotServ.c_str());
			return MOD_CONT;
		}

		if (!ci->c || !ci->c->FindUser(ci->bi))
		{
			source.Reply(_(BOT_NOT_ON_CHANNEL), ci->name.c_str());
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

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "ACT", _("ACT \037channel\037 \037text\037"));
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002ACT \037channel\037 \037text\037\002\n"
				" \n"
				"Makes the bot do the equivalent of a \"/me\" command\n"
				"on the given channel using the given text."));
		return true;
	}
};

class BSAct : public Module
{
	CommandBSAct commandbsact;

 public:
	BSAct(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!botserv)
			throw ModuleException("BotServ is not loaded!");

		this->AddCommand(botserv->Bot(), &commandbsact);
	}
};

MODULE_INIT(BSAct)

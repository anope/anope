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
	CommandBSSay(Module *creator) : Command(creator, "botserv/say", 2, 2)
	{
		this->SetDesc(_("Makes the bot say the given text on the given channel"));
		this->SetSyntax(_("\037channel\037 \037text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &text = params[1];

		User *u = source.u;

		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!ci->AccessFor(u).HasPriv(CA_SAY))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (!ci->bi)
		{
			source.Reply(BOT_NOT_ASSIGNED);
			return;
		}

		if (!ci->c || !ci->c->FindUser(ci->bi))
		{
			source.Reply(BOT_NOT_ON_CHANNEL, ci->name.c_str());
			return;
		}

		if (text[0] == '\001')
		{
			this->OnSyntaxError(source, "");
			return;
		}

		ircdproto->SendPrivmsg(ci->bi, ci->name, "%s", text.c_str());
		ci->bi->lastmsg = Anope::CurTime;

		// XXX need a way to find if someone is overriding this
		Log(LOG_COMMAND, u, this, ci) << text;

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Makes the bot say the given text on the given channel."));
		return true;
	}
};

class CommandBSAct : public Command
{
 public:
	CommandBSAct(Module *creator) : Command(creator, "botserv/act", 2, 2)
	{
		this->SetDesc(_("Makes the bot do the equivalent of a \"/me\" command"));
		this->SetSyntax(_("\037channel\037 \037text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		Anope::string message = params[1];

		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!ci->AccessFor(u).HasPriv(CA_SAY))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (!ci->bi)
		{
			source.Reply(BOT_NOT_ASSIGNED);
			return;
		}

		if (!ci->c || !ci->c->FindUser(ci->bi))
		{
			source.Reply(BOT_NOT_ON_CHANNEL, ci->name.c_str());
			return;
		}

		size_t i = 0;
		while ((i = message.find(1)) && i != Anope::string::npos)
			message.erase(i, 1);

		ircdproto->SendAction(ci->bi, ci->name, "%s", message.c_str());
		ci->bi->lastmsg = Anope::CurTime;

		// XXX Need to be able to find if someone is overriding this.
		Log(LOG_COMMAND, u, this, ci) << message;

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Makes the bot do the equivalent of a \"/me\" command\n"
				"on the given channel using the given text."));
		return true;
	}
};

class BSControl : public Module
{
	CommandBSSay commandbssay;
	CommandBSAct commandbsact;

 public:
	BSControl(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandbssay(this), commandbsact(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(BSControl)

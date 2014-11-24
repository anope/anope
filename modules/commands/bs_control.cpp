/* BotServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandBSSay : public Command
{
 public:
	CommandBSSay(Module *creator) : Command(creator, "botserv/say", 2, 2)
	{
		this->SetDesc(_("Makes the bot say the given text on the given channel"));
		this->SetSyntax(_("\037channel\037 \037text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &text = params[1];

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("SAY") && !source.HasPriv("botserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SAY", ci->GetName());
			return;
		}

		if (!ci->GetBot())
		{
			source.Reply(_("There is no bot assigned to \002{0}\002. One must be assigned to the channel before this command can be used."), ci->GetName());
			ServiceBot *bi;
			Anope::string name;
			Command::FindCommandFromService("botserv/assign", bi, name);
			CommandInfo *help = source.service->FindCommand("generic/help");
			if (bi && help)
				source.Reply(_("See \002{msg}{service} {help} {command}\002 for information on assigning bots."),
				                "msg"_kw = Config->StrictPrivmsg, "service"_kw = bi->nick, "help"_kw = help->cname, "command"_kw = name);
			return;
		}

		if (!ci->c || !ci->c->FindUser(ci->GetBot()))
		{
			source.Reply(_("Bot \002{0}\002 is not on channel \002{1}\002."), ci->GetBot()->nick, ci->GetName());
			return;
		}

		if (text[0] == '\001')
		{
			this->OnSyntaxError(source, "");
			return;
		}

		IRCD->SendPrivmsg(ci->GetBot(), ci->GetName(), "%s", text.c_str());
		ci->GetBot()->lastmsg = Anope::CurTime;

		bool override = !source.AccessFor(ci).HasPriv("SAY");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to say: " << text;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Makes the bot say the given \037text\037 on \037channel\037.\n"
		               "\n"
		                "Example:\n"
		                "         {command} #anope mmm pie\n"
		                "         Makes the assigned service bot say \"mmm pie\"."));
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

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		Anope::string message = params[1];

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("SAY") && !source.HasPriv("botserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "SAY", ci->GetName());
			return;
		}

		if (!ci->GetBot())
		{
			source.Reply(_("There is no bot assigned to \002{0}\002. One must be assigned to the channel before this command can be used."), ci->GetName());
			ServiceBot *bi;
			Anope::string name;
			Command::FindCommandFromService("botserv/assign", bi, name);
			CommandInfo *help = source.service->FindCommand("generic/help");
			if (bi && help)
				source.Reply(_("See \002{msg}{service} {help} {command}\002 for information on assigning bots."),
				                "msg"_kw = Config->StrictPrivmsg, "service"_kw = bi->nick, "help"_kw = help->cname, "command"_kw = name);
			return;
		}

		if (!ci->c || !ci->c->FindUser(ci->GetBot()))
		{
			source.Reply(_("Bot \002{0}\002 is not on channel \002{1}\002."), ci->GetBot()->nick, ci->GetName());
			return;
		}

		message = message.replace_all_cs("\1", "");
		if (message.empty())
			return;

		IRCD->SendAction(ci->GetBot(), ci->GetName(), "%s", message.c_str());
		ci->GetBot()->lastmsg = Anope::CurTime;

		bool override = !source.AccessFor(ci).HasPriv("SAY");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to say: " << message;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Makes the assigned bot do the equivalent of a \"/me\" command on \037channel\037 using the given \037text\037.\n"
		                "\n"
		                "Example:\n"
		                "         {command} #anope slaps Cronus\n"
		                "         Shows the assigned service bot \"slapping\" Cronus."),
		                "command"_kw = source.command);
		return true;
	}
};

class BSControl : public Module
{
	CommandBSSay commandbssay;
	CommandBSAct commandbsact;

 public:
	BSControl(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandbssay(this), commandbsact(this)
	{

	}
};

MODULE_INIT(BSControl)

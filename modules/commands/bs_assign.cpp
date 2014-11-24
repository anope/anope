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
#include "modules/bs_info.h"

class CommandBSAssign : public Command
{
 public:
	CommandBSAssign(Module *creator) : Command(creator, "botserv/assign", 2, 2)
	{
		this->SetDesc(_("Assigns a bot to a channel"));
		this->SetSyntax(_("\037channel\037 \037nickname\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &nick = params[1];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, bot assignment is temporarily disabled."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		ServiceBot *bi = ServiceBot::Find(nick, true);
		if (!bi)
		{
			source.Reply(_("Bot \002{0}\002 does not exist."), nick);
			return;
		}

		ChanServ::AccessGroup access = source.AccessFor(ci);
 		if (!access.HasPriv("ASSIGN") && !source.HasPriv("botserv/administration"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "ASSIGN", ci->GetName());
			return;
		}

		if (ci->HasFieldS("BS_NOBOT"))
		{
			source.Reply(_("Access denied. \002{0}\002 may not have a bot assigned to it because a Services Operator has disallowed it."), ci->GetName());
			return;
		}

		if (bi->bi->GetOperOnly() && !source.HasPriv("botserv/administration"))
		{
			source.Reply(_("Access denied. Bot \002{0}\002 is for operators only."), bi->nick);
			return;
		}

		if (ci->GetBot() == bi)
		{
			source.Reply(_("Bot \002{0}\002 is already assigned to \002{1}\002."), ci->GetBot()->nick, ci->GetName());
			return;
		}

		bool override = !access.HasPriv("ASSIGN");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "for " << bi->nick;

		bi->Assign(source.GetUser(), ci);
		source.Reply(_("Bot \002{0}\002 has been assigned to \002{1}\002."), bi->nick, ci->GetName());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Assigns the bot \037nickname\037 to \037channel\037."
		               " You can then configure the bot for the channel so it fits your needs.\n"
		               "\n"
		               "Use of this command requires the \002{0}\002 privilege on \037channel\037."
		               "\n"
		               "Example:\n"
			       "         {command} #anope Botox\n"
			       "         Assigns the bot Botox to #anope.\n"),
		               "ASSIGN", "command"_kw = source.command);
		return true;
	}
};

class CommandBSUnassign : public Command
{
 public:
	CommandBSUnassign(Module *creator) : Command(creator, "botserv/unassign", 1, 1)
	{
		this->SetDesc(_("Unassigns a bot from a channel"));
		this->SetSyntax(_("\037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, bot assignment is temporarily disabled."));
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		ChanServ::AccessGroup access = source.AccessFor(ci);
		if (!source.HasPriv("botserv/administration") && !access.HasPriv("ASSIGN"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "ASSIGN", ci->GetName());
			return;
		}

		if (!ci->GetBot())
		{
			source.Reply(_("There is no bot assigned to \002{0}\002."), ci->GetName());
			return;
		}

		if (ci->HasFieldS("PERSIST") && !ModeManager::FindChannelModeByName("PERM"))
		{
			source.Reply(_("You can not unassign bots while persist is set on the channel."));
			return;
		}

		bool override = !access.HasPriv("ASSIGN");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "for " << ci->GetBot()->nick;

		ServiceBot *bi = ci->GetBot();
		bi->UnAssign(source.GetUser(), ci);
		source.Reply(_("Bot \002{0}\002 has been unassigned from \002{1}\002."), bi->nick, ci->GetName());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Unassigns a bot from \037channel\037."
		               "Bot configuration is kept, so you will always be able to reassign a bot later without losing your settings.\n"
		               "\n"
		               "Use of this command requires the \002{0}\002 privilege on \037channel\037."
		               "\n"
		               "Example:\n"
			       "         {command} #anope\n"
			       "         Unassigns the current bot from #anope.\n"),
		               "ASSIGN", "command"_kw = source.command);
		return true;
	}
};

class CommandBSSetNoBot : public Command
{
 public:
	CommandBSSetNoBot(Module *creator, const Anope::string &sname = "botserv/set/nobot") : Command(creator, sname, 2, 2)
	{
		this->SetDesc(_("Prevent a bot from being assigned to a channel"));
		this->SetSyntax(_("\037channel\037 {\037ON|OFF\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &value = params[1];

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
			return;
		}

		if (value.equals_ci("ON"))
		{
			Log(LOG_ADMIN, source, this, ci) << "to enable nobot";

			ci->SetS<bool>("BS_NOBOT", true);
			if (ci->GetBot())
				ci->GetBot()->UnAssign(source.GetUser(), ci);
			source.Reply(_("No-bot mode is now \002on\002 for \002{0}\002."), ci->GetName());
		}
		else if (value.equals_ci("OFF"))
		{
			Log(LOG_ADMIN, source, this, ci) << "to disable nobot";

			ci->UnsetS<bool>("BS_NOBOT");
			source.Reply(_("No-bot mode is now \002off\002 for \002{0}\002."), ci->GetName());
		}
		else
			this->OnSyntaxError(source, source.command);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("If no-bot is set on a channel, then the channel will be unabled to have a bot assigned to it."
		               " If a bot is already assigned to the channel, it is unassigned automatically when no-bot is enabled."
		               "\n"
		               "Example:\n"
			       "         {command} #anope on\n"
			       "         Prevents a service bot from being assigned to #anope.\n"),
		               "command"_kw = source.command);
		return true;
	}
};

class BSAssign : public Module
	, public EventHook<Event::Invite>
	, public EventHook<Event::ServiceBotEvent>
{
	Serialize::Field<BotInfo, bool> nobot;

	CommandBSAssign commandbsassign;
	CommandBSUnassign commandbsunassign;
	CommandBSSetNoBot commandbssetnobot;

 public:
	BSAssign(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, EventHook<Event::Invite>("OnInvite")
		, EventHook<Event::ServiceBotEvent>("OnServiceBotEvent")

		, nobot(this, botinfo, "BS_NOBOT")

		, commandbsassign(this)
		, commandbsunassign(this)
		, commandbssetnobot(this)
	{
	}

	void OnInvite(User *source, Channel *c, User *targ) override
	{
		ServiceBot *bi;
		if (Anope::ReadOnly || !c->ci || targ->server != Me || !(bi = ServiceBot::Find(targ->nick, true)))
			return;

		ChanServ::AccessGroup access = c->ci->AccessFor(source);
		if (!access.HasPriv("ASSIGN") && !source->HasPriv("botserv/administration"))
		{
			targ->SendMessage(bi, _("Access denied. You do not have privilege \002ASSIGN\002 on \002{0}\002."), c->ci->GetName());
			return;
		}

		if (nobot.HasExt(c->ci))
		{
			targ->SendMessage(bi, _("Access denied. \002{0}\002 may not have a bot assigned to it because a Services Operator has disallowed it."), c->ci->GetName());
			return;
		}

		if (bi->bi->GetOperOnly() && !source->HasPriv("botserv/administration"))
		{
			targ->SendMessage(bi, _("Access denied. Bot \002{0}\002 is for operators only."), bi->nick);
			return;
		}

		if (c->ci->GetBot() == bi)
		{
			targ->SendMessage(bi, _("Bot \002{0}\002 is already assigned to \002{1}\002."), bi->nick, c->ci->GetName());
			return;
		}

		bi->Assign(source, c->ci);
		targ->SendMessage(bi, _("Bot \002{0}\002 has been assigned to \002{1}\002."), bi->nick, c->ci->GetName());
	}

	void OnServiceBot(CommandSource &source, ServiceBot *bi, ChanServ::Channel *ci, InfoFormatter &info) override
	{
		if (nobot.HasExt(ci))
			info.AddOption(_("No bot"));
	}
};

MODULE_INIT(BSAssign)

/* ChanServ core functions
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

class CommandCSRegister : public Command
{
 public:
	CommandCSRegister(Module *creator) : Command(creator, "chanserv/register", 1, 2)
	{
		this->SetDesc(_("Register a channel"));
		this->SetSyntax(_("\037channel\037 [\037description\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &chdesc = params.size() > 1 ? params[1] : "";

		User *u = source.GetUser();
		NickServ::Account *nc = source.nc;

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, channel registration is temporarily disabled."));
			return;
		}

		if (nc->HasFieldS("UNCONFIRMED"))
		{
			source.Reply(_("You must confirm your account before you can register a channel."));
			return;
		}

		if (chan[0] == '&')
		{
			source.Reply(_("Local channels can not be registered."));
			return;
		}

		if (chan[0] != '#')
		{
			source.Reply(_("Please use the symbol of \002#\002 when attempting to register."));
			return;
		}

		if (!IRCD->IsChannelValid(chan))
		{
			source.Reply(_("Channel \002{0}\002 is not a valid channel."), chan);
			return;
		}

		Channel *c = Channel::Find(params[0]);
		if (!c && u)
		{
			source.Reply(_("Channel \002{0}\002 doesn't exist."), chan);
			return;
		}

		ChanServ::Channel *ci = ChanServ::Find(chan);
		if (ci)
		{
			source.Reply(_("Channel \002{0}\002 is already registered!"), chan);
			return;
		}

		if (c && !c->HasUserStatus(u, "OP"))
		{
			source.Reply(_("You must be a channel operator to register the channel."));
			return;
		}

		unsigned maxregistered = Config->GetModule("chanserv")->Get<unsigned>("maxregistered");
		if (maxregistered && nc->GetChannelCount() >= maxregistered && !source.HasPriv("chanserv/no-register-limit"))
		{
			if (nc->GetChannelCount() > maxregistered)
				source.Reply(_("Sorry, you have already exceeded your limit of \002{0}\002 channels."), maxregistered);
			else
				source.Reply(_("Sorry, you have already reached your limit of \002{0}\002 channels."), maxregistered);
			return;
		}

		if (!ChanServ::service)
			return;
		ci = ChanServ::channel.Create();
		ci->SetName(chan);
		ci->SetFounder(nc);
		ci->SetDesc(chdesc);

		if (c && !c->topic.empty())
		{
			ci->SetLastTopic(c->topic);
			ci->SetLastTopicSetter(c->topic_setter);
			ci->SetLastTopicTime(c->topic_time);
		}
		else
			ci->SetLastTopicSetter(source.service->nick);

		Log(LOG_COMMAND, source, this, ci);
		source.Reply(_("Channel \002{0}\002 registered under your account: \002{1}\002"), chan, nc->GetDisplay());

		/* Implement new mode lock */
		if (c)
		{
			c->CheckModes();
			if (u)
				c->SetCorrectModes(u, true);
		}

		Event::OnChanRegistered(&Event::ChanRegistered::OnChanRegistered, ci);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Registers a channel, which sets you as the founder and prevents other users from gaining unauthorized access in the channel."
		               " To use this command, you must first be a channel operator on \037channel\037."
		               " The description, which is optional, is a general description of the channel's purpose.\n"
		               "\n"
		               "When you register a channel, you are recorded as the founder of the channel."
		               " The channel founder is allowed to change all of the channel settings for the channel,"
		               " and will automatically be given channel operator status when entering the channel."));

		ServiceBot *bi;
		Anope::string cmd;
		CommandInfo *help = source.service->FindCommand("generic/help");
		if (Command::FindCommandFromService("chanserv/access", bi, cmd) && help)
			source.Reply(_("\n"
			               "See the \002{0}\002 command (\002{1}{2} {3} {0}\002) for information on giving a subset of these privileges to other users."),
			                cmd, Config->StrictPrivmsg, bi->nick, help->cname);

		return true;
	}
};


class CSRegister : public Module
{
	CommandCSRegister commandcsregister;

 public:
	CSRegister(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsregister(this)
	{
	}
};

MODULE_INIT(CSRegister)

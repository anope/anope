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
#include "modules/cs_akick.h"

class CommandCSStatus : public Command
{
public:
	CommandCSStatus(Module *creator) : Command(creator, "chanserv/status", 1, 2)
	{
		this->SetDesc(_("Find a user's status on a channel"));
		this->SetSyntax(_("\037channel\037 [\037user\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &channel = params[0];

		ChanServ::Channel *ci = ChanServ::Find(channel);
		if (ci == NULL)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), channel);
			return;
		}

		if (!source.AccessFor(ci).HasPriv("ACCESS_CHANGE") && !source.HasPriv("chanserv/auspex"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "ACCESS_CHANGE", ci->GetName());
			return;
		}

		Anope::string nick = source.GetNick();
		if (params.size() > 1)
			nick = params[1];

		ChanServ::AccessGroup ag;
		User *u = User::Find(nick, true);
		NickServ::Nick *na = NULL;
		if (u != NULL)
			ag = ci->AccessFor(u);
		else
		{
			na = NickServ::FindNick(nick);
			if (na != NULL)
				ag = ci->AccessFor(na->GetAccount());
		}

		if (ag.super_admin)
			source.Reply(_("\002{0}\002 is a super administrator."), nick);
		else if (ag.founder)
			source.Reply(_("\002{0}\002 is the founder of \002{1}\002."), nick, ci->GetName());
		else  if (ag.empty())
			source.Reply(_("\002{0}\002 has no access on \002{1}\002."), nick, ci->GetName());
		else
		{
			source.Reply(_("Access for \002{0}\002 on \002{1}\002:"), nick, ci->GetName());

			for (unsigned i = 0; i < ag.size(); ++i)
			{
				ChanServ::ChanAccess *acc = ag[i];

				source.Reply(_("\002{0}\002 matches access entry \002{1}\002, which has privilege \002{2}\002."), nick, acc->Mask(), acc->AccessSerialize());
			}
		}

		for (unsigned j = 0, end = ci->GetAkickCount(); j < end; ++j)
		{
			AutoKick *ak = ci->GetAkick(j);

			if (ak->GetAccount())
			{
				if (na && ak->GetAccount() == na->GetAccount())
					source.Reply(_("\002{0}\002 is on the auto kick list of \002{1}\002 ({2})."), na->GetAccount()->GetDisplay(), ci->GetName(), ak->GetReason());
			}
			else if (u != NULL)
			{
				Entry akick_mask("", ak->GetMask());
				if (akick_mask.Matches(u))
					source.Reply(_("\002{0}\002 matches auto kick entry \002{1}\002 on \002{2}\002 ({3})."), u->nick, ak->GetMask(), ci->GetName(), ak->GetReason());
			}
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("This command tells you what access \037user\037 has on \037channel\037."
		               "It will also tell you which access and auto kick entries match \037user\037.\n"
		               "\n"
		               "Use of this command requires the \002{0}\002 privilege on \037channel\037."),
		               "ACCESS_CHANGE");
		return true;
	}
};

class CSStatus : public Module
{
	CommandCSStatus commandcsstatus;

 public:
	CSStatus(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcsstatus(this)
	{
	}
};

MODULE_INIT(CSStatus)

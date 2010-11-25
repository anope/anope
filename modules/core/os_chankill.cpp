/* OperServ core functions
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

class CommandOSChanKill : public Command
{
 public:
	CommandOSChanKill() : Command("CHANKILL", 2, 3, "operserv/chankill")
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		Anope::string expiry, channel;
		time_t expires;
		unsigned last_param = 1;
		Channel *c;

		channel = params[0];
		if (!channel.empty() && channel[0] == '+')
		{
			expiry = channel;
			channel = params[1];
			last_param = 2;
		}

		expires = !expiry.empty() ? dotime(expiry) : Config->ChankillExpiry;
		if (!expiry.empty() && isdigit(expiry[expiry.length() - 1]))
			expires *= 86400;
		if (expires && expires < 60)
		{
			source.Reply(BAD_EXPIRY_TIME);
			return MOD_CONT;
		}
		else if (expires > 0)
			expires += Anope::CurTime;

		if (params.size() <= last_param)
		{
			this->OnSyntaxError(u, "");
			return MOD_CONT;
		}

		Anope::string reason = params[last_param];
		if (params.size() > last_param + 1)
			reason += params[last_param + 1];
		if (!reason.empty())
		{
			Anope::string realreason;
			if (Config->AddAkiller)
				realreason = "[" + u->nick + "] " + reason;
			else
				realreason = reason;

			if ((c = findchan(channel)))
			{
				for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
				{
					UserContainer *uc = *it++;

					if (is_oper(uc->user))
						continue;

					SGLine->Add(OperServ, u, "*@" + uc->user->host, expires, realreason);
					SGLine->Check(uc->user);
				}
				if (Config->WallOSAkill)
					ircdproto->SendGlobops(OperServ, "%s used CHANKILL on %s (%s)", u->nick.c_str(), channel.c_str(), realreason.c_str());
			}
			else
				source.Reply(CHAN_X_NOT_IN_USE, channel.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(OperServ, OPER_HELP_CHANKILL);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(OperServ, u, "CHANKILL", OPER_CHANKILL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(OperServ, OPER_HELP_CMD_CHANKILL);
	}
};

class OSChanKill : public Module
{
	CommandOSChanKill commandoschankill;

 public:
	OSChanKill(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandoschankill);
	}
};

MODULE_INIT(OSChanKill)

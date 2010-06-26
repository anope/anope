/* OperServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

class CommandOSChanKill : public Command
{
 public:
	CommandOSChanKill() : Command("CHANKILL", 2, 3, "operserv/chankill")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *expiry, *channel;
		char reason[BUFSIZE];
		time_t expires;
		unsigned last_param = 1;
		Channel *c;

		channel = params[0].c_str();
		if (channel && *channel == '+')
		{
			expiry = channel;
			channel = params[1].c_str();
			last_param = 2;
		}
		else
			expiry = NULL;

		expires = expiry ? dotime(expiry) : Config.ChankillExpiry;
		if (expiry && isdigit(expiry[strlen(expiry) - 1]))
			expires *= 86400;
		if (expires != 0 && expires < 60)
		{
			notice_lang(Config.s_OperServ, u, BAD_EXPIRY_TIME);
			return MOD_CONT;
		}
		else if (expires > 0)
			expires += time(NULL);

		if (params.size() <= last_param)
		{
			this->OnSyntaxError(u, "");
			return MOD_CONT;
		}
		snprintf(reason, sizeof(reason), "%s%s", params[last_param].c_str(), (params.size() > last_param + 1 ? params[last_param + 1].c_str() : ""));
		if (*reason)
		{

			std::string realreason;
			if (Config.AddAkiller)
				realreason = "[" + u->nick + "] " + std::string(reason);
			else
				realreason = reason;

			if ((c = findchan(channel)))
			{
				for (CUserList::iterator it = c->users.begin(); it != c->users.end();)
				{
					UserContainer *uc = *it++;

					if (is_oper(uc->user))
						continue;

					SGLine->Add(OperServ, u, ci::string("*@") + uc->user->host, expires, realreason);
					SGLine->Check(uc->user);
				}
				if (Config.WallOSAkill)
					ircdproto->SendGlobops(OperServ, "%s used CHANKILL on %s (%s)", u->nick.c_str(), channel, realreason.c_str());
			}
			else
				notice_lang(Config.s_OperServ, u, CHAN_X_NOT_IN_USE, channel);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_CHANKILL);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "CHANKILL", OPER_CHANKILL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_CHANKILL);
	}
};

class OSChanKill : public Module
{
 public:
	OSChanKill(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, new CommandOSChanKill());
	}
};

MODULE_INIT(OSChanKill)

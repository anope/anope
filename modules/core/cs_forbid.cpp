/* ChanServ core functions
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

class CommandCSForbid : public Command
{
 public:
	CommandCSForbid() : Command("FORBID", 1, 2, "chanserv/forbid")
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTEREDCHANNEL);
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		ChannelInfo *ci;
		Anope::string chan = params[0];
		Anope::string reason = params.size() > 1 ? params[1] : "";

		Channel *c;

		if (Config->ForceForbidReason && reason.empty())
		{
			SyntaxError(ChanServ, u, "FORBID", CHAN_FORBID_SYNTAX_REASON);
			return MOD_CONT;
		}

		if (chan[0] != '#')
		{
			u->SendMessage(ChanServ, CHAN_SYMBOL_REQUIRED);
			return MOD_CONT;
		}

		if (readonly)
		{
			u->SendMessage(ChanServ, READ_ONLY_MODE);
			return MOD_CONT;
		}

		if ((ci = cs_findchan(chan)))
			delete ci;

		ci = new ChannelInfo(chan);
		if (!ci)
		{
			 // this cant happen?
			//Alog() << Config->s_ChanServ << ": Valid FORBID for " << ci->name << " by " << u->nick << " failed";
			u->SendMessage(ChanServ, CHAN_FORBID_FAILED, chan.c_str());
			return MOD_CONT;
		}

		ci->SetFlag(CI_FORBIDDEN);
		ci->forbidby = u->nick;
		ci->forbidreason = reason;

		if ((c = ci->c))
		{
			/* Before banning everyone, it might be prudent to clear +e and +I lists..
			 * to prevent ppl from rejoining.. ~ Viper */
			std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> modes = c->GetModeList(CMODE_EXCEPT);
			for (; modes.first != modes.second; ++modes.first)
				c->RemoveMode(NULL, CMODE_EXCEPT, modes.first->second);
			modes = c->GetModeList(CMODE_INVITEOVERRIDE);
			for (; modes.first != modes.second; ++modes.first)
				c->RemoveMode(NULL, CMODE_INVITEOVERRIDE, modes.first->second);

			for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
			{
				UserContainer *uc = *it++;

				if (is_oper(uc->user))
					continue;

				c->Kick(ChanServ, uc->user, "%s", !reason.empty() ? reason.c_str() : GetString(uc->user, CHAN_FORBID_REASON).c_str());
			}
		}

		if (ircd->chansqline)
		{
			XLine x(chan, "Forbidden");
			ircdproto->SendSQLine(&x);
		}

		if (Config->WallForbid)
			ircdproto->SendGlobops(ChanServ, "\2%s\2 used FORBID on channel \2%s\2", u->nick.c_str(), ci->name.c_str());
		Log(LOG_ADMIN, u, this, ci) << (!ci->forbidreason.empty() ? ci->forbidreason : "No reason");

		u->SendMessage(ChanServ, CHAN_FORBID_SUCCEEDED, chan.c_str());

		FOREACH_MOD(I_OnChanForbidden, OnChanForbidden(ci));

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_SERVADMIN_HELP_FORBID);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "FORBID", CHAN_FORBID_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_FORBID);
	}
};

class CSForbid : public Module
{
	CommandCSForbid commandcsforbid;

 public:
	CSForbid(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsforbid);
	}
};

MODULE_INIT(CSForbid)

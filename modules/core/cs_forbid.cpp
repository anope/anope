/* ChanServ core functions
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

class CommandCSForbid : public Command
{
 public:
	CommandCSForbid() : Command("FORBID", 1, 2, "chanserv/forbid")
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTEREDCHANNEL);
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		const Anope::string &reason = params.size() > 1 ? params[1] : "";

		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (Config->ForceForbidReason && reason.empty())
		{
			SyntaxError(source, "FORBID", CHAN_FORBID_SYNTAX_REASON);
			return MOD_CONT;
		}

		if (chan[0] != '#')
		{
			source.Reply(CHAN_SYMBOL_REQUIRED);
			return MOD_CONT;
		}

		if (readonly)
		{
			source.Reply(READ_ONLY_MODE);
			return MOD_CONT;
		}

		if ((ci = cs_findchan(chan)))
			delete ci;

		ci = new ChannelInfo(chan);
		ci->SetFlag(CI_FORBIDDEN);
		ci->forbidby = u->nick;
		ci->forbidreason = reason;

		Channel *c = ci->c;
		if (c)
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

				if (uc->user->HasMode(UMODE_OPER))
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

		source.Reply(CHAN_FORBID_SUCCEEDED, chan.c_str());

		FOREACH_MOD(I_OnChanForbidden, OnChanForbidden(ci));

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_SERVADMIN_HELP_FORBID);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "FORBID", CHAN_FORBID_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_FORBID);
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

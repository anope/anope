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

class CommandCSInvite : public Command
{
 public:
	CommandCSInvite() : Command("INVITE", 1, 3)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Channel *c;
		ChannelInfo *ci;
		User *u2;

		if (!(c = findchan(chan)))
		{
			notice_lang(Config.s_ChanServ, u, CHAN_X_NOT_IN_USE, chan.c_str());
			return MOD_CONT;
		}

		ci = c->ci;

		if (!u || !check_access(u, ci, CA_INVITE))
		{
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (params.size() == 1)
			u2 = u;
		else
		{
			if (!(u2 = finduser(params[1])))
			{
				notice_lang(Config.s_ChanServ, u, NICK_X_NOT_IN_USE, params[1].c_str());
				return MOD_CONT;
			}
		}

		if (c->FindUser(u2))
			notice_lang(Config.s_ChanServ, u, CHAN_INVITE_ALREADY_IN, c->name.c_str());
		else
		{
			ircdproto->SendInvite(whosends(ci), chan, u2->nick);
			notice_lang(whosends(ci)->nick, u, CHAN_INVITE_OTHER_SUCCESS, u2->nick.c_str(), c->name.c_str());
			notice_lang(whosends(ci)->nick, u2, CHAN_INVITE_SUCCESS, c->name.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_INVITE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "INVITE", CHAN_INVITE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_INVITE);
	}
};

class CSInvite : public Module
{
	CommandCSInvite commandcsinvite;

 public:
	CSInvite(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsinvite);
	}
};

MODULE_INIT(CSInvite)

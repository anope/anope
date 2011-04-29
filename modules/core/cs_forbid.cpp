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
#include "chanserv.h"

class CommandCSForbid : public Command
{
 public:
	CommandCSForbid() : Command("FORBID", 1, 2, "chanserv/forbid")
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTEREDCHANNEL);
		this->SetDesc(_("Prevent a channel from being used"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];
		const Anope::string &reason = params.size() > 1 ? params[1] : "";

		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (Config->ForceForbidReason && reason.empty())
		{
			SyntaxError(source, "FORBID", _("FORBID \037channel\037 \037reason\037"));
			return MOD_CONT;
		}

		if (chan[0] != '#')
		{
			source.Reply(_(CHAN_SYMBOL_REQUIRED));
			return MOD_CONT;
		}

		if (readonly)
		{
			source.Reply(_(READ_ONLY_MODE));
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

				c->Kick(chanserv->Bot(), uc->user, "%s", !reason.empty() ? reason.c_str() : GetString(uc->user->Account(), "This channel has been forbidden.").c_str());
			}
		}

		if (ircd->chansqline)
		{
			XLine x(chan, "Forbidden");
			ircdproto->SendSQLine(NULL, &x);
		}

		Log(LOG_ADMIN, u, this, ci) << (!ci->forbidreason.empty() ? ci->forbidreason : "No reason");

		source.Reply(_("Channel \002%s\002 is now forbidden."), chan.c_str());

		FOREACH_MOD(I_OnChanForbidden, OnChanForbidden(ci));

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002FORBID \037channel\037 [\037reason\037]\002\n"
				" \n"
				"Disallows anyone from registering or using the given\n"
				"channel.  May be cancelled by dropping the channel.\n"
				" \n"
				"Reason may be required on certain networks."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "FORBID", _("FORBID \037channel\037 [\037reason\037]"));
	}
};

class CSForbid : public Module
{
	CommandCSForbid commandcsforbid;

 public:
	CSForbid(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!chanserv)
			throw ModuleException("ChanServ is not loaded!");

		this->AddCommand(chanserv->Bot(), &commandcsforbid);
	}
};

MODULE_INIT(CSForbid)

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

class CommandCSInvite : public Command
{
 public:
	CommandCSInvite() : Command("INVITE", 1, 3)
	{
		this->SetDesc(Anope::printf(_("Tells %s to invite you into a channel"), Config->s_ChanServ.c_str()));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];

		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Channel *c = ci->c;

		if (!(c = findchan(chan)))
		{
			source.Reply(_(CHAN_X_NOT_IN_USE), chan.c_str());
			return MOD_CONT;
		}

		ci = c->ci;

		if (!check_access(u, ci, CA_INVITE))
		{
			source.Reply(_(ACCESS_DENIED));
			return MOD_CONT;
		}

		User *u2;
		if (params.size() == 1)
			u2 = u;
		else
		{
			if (!(u2 = finduser(params[1])))
			{
				source.Reply(_(NICK_X_NOT_IN_USE), params[1].c_str());
				return MOD_CONT;
			}
		}

		// XXX need a check for override...
		Log(LOG_COMMAND, u, this, ci) << "for " << u2->nick;

		if (c->FindUser(u2))
			source.Reply(_("You are already in \002%s\002! "), c->name.c_str());
		else
		{
			ircdproto->SendInvite(ci->WhoSends(), chan, u2->nick);
			source.Reply(_("\002%s\002 has been invited to \002%s\002."), u2->nick.c_str(), c->name.c_str());
			u2->SendMessage(ci->WhoSends(), _("You have been invited to \002%s\002."), c->name.c_str());
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002INVITE \037channel\037\002\n"
				" \n"
				"Tells %s to invite you into the given channel.\n"
				" \n"
				"By default, limited to AOPs or those with level 5 and above\n"
				"on the channel."), Config->s_ChanServ.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "INVITE", _("INVITE \037channel\037"));
	}
};

class CSInvite : public Module
{
	CommandCSInvite commandcsinvite;

 public:
	CSInvite(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE)
	{
		this->SetAuthor("Anope");

		if (!chanserv)
			throw ModuleException("ChanServ is not loaded!");

		this->AddCommand(chanserv->Bot(), &commandcsinvite);
	}
};

MODULE_INIT(CSInvite)

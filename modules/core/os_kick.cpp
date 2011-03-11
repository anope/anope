/* OperServ core functions
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

class CommandOSKick : public Command
{
 public:
	CommandOSKick() : Command("KICK", 3, 3, "operserv/kick")
	{
		this->SetDesc(_("Kick a user from a channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &chan = params[0];
		const Anope::string &nick = params[1];
		const Anope::string &s = params[2];
		Channel *c;
		User *u2;

		if (!(c = findchan(chan)))
		{
			source.Reply(_(CHAN_X_NOT_IN_USE), chan.c_str());
			return MOD_CONT;
		}
		else if (c->bouncy_modes)
		{
			source.Reply(_("Services is unable to change modes. Are your servers' U:lines configured correctly?"));
			return MOD_CONT;
		}
		else if (!(u2 = finduser(nick)))
		{
			source.Reply(_(NICK_X_NOT_IN_USE), nick.c_str());
			return MOD_CONT;
		}

		c->Kick(OperServ, u2, "%s (%s)", u->nick.c_str(), s.c_str());
		Log(LOG_ADMIN, u, this) << "on " << u2->nick << " in " << c->name;
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002KICK \037channel\037 \037user\037 \037reason\037\002\n"
				" \n"
				"Allows staff to kick a user from any channel.\n"
				"Parameters are the same as for the standard /KICK\n"
				"command. The kick message will have the nickname of the\n"
				"IRCop sending the KICK command prepended; for example:\n"
				" \n"
				"*** SpamMan has been kicked off channel #my_channel by %s (Alcan (Flood))"), OperServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "KICK", _("KICK \037channel\037 \037user\037 \037reason\037"));
	}
};

class OSKick : public Module
{
	CommandOSKick commandoskick;

 public:
	OSKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandoskick);
	}
};

MODULE_INIT(OSKick)

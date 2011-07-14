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
	CommandOSKick(Module *creator) : Command(creator, "operserv/kick", 3, 3, "operserv/kick")
	{
		this->SetDesc(_("Kick a user from a channel"));
		this->SetSyntax(_("\037channel\037 \037user\037 \037reason\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &chan = params[0];
		const Anope::string &nick = params[1];
		const Anope::string &s = params[2];
		Channel *c;
		User *u2;

		if (!(c = findchan(chan)))
		{
			source.Reply(CHAN_X_NOT_IN_USE, chan.c_str());
			return;
		}
		else if (c->bouncy_modes)
		{
			source.Reply(_("Services is unable to change modes. Are your servers' U:lines configured correctly?"));
			return;
		}
		else if (!(u2 = finduser(nick)))
		{
			source.Reply(NICK_X_NOT_IN_USE, nick.c_str());
			return;
		}

		c->Kick(source.owner, u2, "%s (%s)", u->nick.c_str(), s.c_str());
		Log(LOG_ADMIN, u, this) << "on " << u2->nick << " in " << c->name;
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows staff to kick a user from any channel.\n"
				"Parameters are the same as for the standard /KICK\n"
				"command. The kick message will have the nickname of the\n"
				"IRCop sending the KICK command prepended; for example:\n"
				" \n"
				"*** SpamMan has been kicked off channel #my_channel by %s (Alcan (Flood))"), source.owner->nick.c_str());
		return true;
	}
};

class OSKick : public Module
{
	CommandOSKick commandoskick;

 public:
	OSKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandoskick(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandoskick);
	}
};

MODULE_INIT(OSKick)

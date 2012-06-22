/* NickServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandNSDrop : public Command
{
 public:
	CommandNSDrop(Module *creator) : Command(creator, "nickserv/drop", 0, 1)
	{
		this->SetDesc(_("Cancel the registration of a nickname"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		Anope::string nick = !params.empty() ? params[0] : "";

		if (readonly)
		{
			source.Reply(_("Sorry, nickname de-registration is temporarily disabled."));
			return;
		}

		NickAlias *na = findnick(!nick.empty() ? nick : source.GetNick());
		if (!na)
		{
			source.Reply(NICK_NOT_REGISTERED);
			return;
		}

		bool is_mine = source.nc == na->nc;
		Anope::string my_nick;
		if (is_mine && nick.empty())
			my_nick = na->nick;

		if (!is_mine && !source.HasPriv("nickserv/drop"))
			source.Reply(ACCESS_DENIED);
		else if (Config->NSSecureAdmins && !is_mine && na->nc->IsServicesOper())
			source.Reply(_("You may not drop other services operators nicknames."));
		else
		{
			if (readonly)
				source.Reply(READ_ONLY_MODE);

			FOREACH_MOD(I_OnNickDrop, OnNickDrop(source, na));

			Log(!is_mine ? LOG_OVERRIDE : LOG_COMMAND, source, this) << "to drop nickname " << na->nick << " (group: " << na->nc->display << ") (email: " << (!na->nc->email.empty() ? na->nc->email : "none") << ")";
			na->destroy();

			if (!is_mine)
			{
				source.Reply(_("Nickname \002%s\002 has been dropped."), nick.c_str());
			}
			else
			{
				if (!nick.empty())
					source.Reply(_("Nickname \002%s\002 has been dropped."), nick.c_str());
				else
					source.Reply(_("Your nickname has been dropped."));
			}
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		if (source.HasPriv("nickserv/drop"))
			source.Reply(_("Syntax: \002%s [\037nickname\037]\002\n"
					" \n"
					"Without a parameter, deletes your nickname.\n"
					" \n"
					"With a parameter, drops the named nick from the database.\n"
					"You may drop any nick within your group without any \n"
					"special privileges. Dropping any nick is limited to \n"
					"\002Services Operators\002."), source.command.c_str());
		else
			source.Reply(_("Syntax: \002%s [\037nickname\037 | \037password\037]\002\n"
					" \n"
					"Deltes your nickname.  A nick\n"
					"that has been dropped is free for anyone to re-register.\n"
					" \n"
					"You may drop a nick within your group by passing it\n"
					"as the \002nick\002 parameter.\n"
					" \n"
					"If you have a nickname registration pending but can not confirm\n"
					"it for any reason, you can cancel your registration by passing\n"
					"your password as the \002password\002 parameter.\n"
					" \n"
					"In order to use this command, you must first identify\n"
					"with your password."), source.command.c_str());

		return true;
	}
};

class NSDrop : public Module
{
	CommandNSDrop commandnsdrop;

 public:
	NSDrop(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnsdrop(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(NSDrop)

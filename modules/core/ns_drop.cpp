/* NickServ core functions
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

class CommandNSDrop : public Command
{
 public:
	CommandNSDrop() : Command("DROP", 0, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc("Cancel the registration of a nickname");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		Anope::string nick = !params.empty() ? params[0] : "";

		if (readonly)
		{
			source.Reply(_("Sorry, nickname de-registration is temporarily disabled."));
			return MOD_CONT;
		}

		NickAlias *na = findnick((u->Account() && !nick.empty() ? nick : u->nick));
		if (!na)
		{
			NickRequest *nr = findrequestnick(u->Account() && !nick.empty() ? nick : u->nick);
			if (nr && u->Account() && u->Account()->IsServicesOper())
			{
				Log(LOG_ADMIN, u, this) << "to drop nickname " << nr->nick << " (email: " << nr->email << ")";
				delete nr;
				source.Reply(_("Nickname \002%s\002 has been dropped."), nick.c_str());
			}
			else if (nr && !nick.empty())
			{
				int res = enc_check_password(nick, nr->password);
				if (res)
				{
					Log(LOG_COMMAND, u, this) << "to drop nick request " << nr->nick;
					source.Reply(_("Nickname \002%s\002 has been dropped."), nr->nick.c_str());
					delete nr;
				}
				else if (bad_password(u))
					return MOD_STOP;
				else
					source.Reply(LanguageString::PASSWORD_INCORRECT);
			}
			else
				source.Reply(LanguageString::NICK_NOT_REGISTERED);

			return MOD_CONT;
		}

		if (!u->Account())
		{
			source.Reply(LanguageString::NICK_IDENTIFY_REQUIRED, Config->s_NickServ.c_str());
			return MOD_CONT;
		}

		bool is_mine = u->Account() == na->nc;
		Anope::string my_nick;
		if (is_mine && nick.empty())
			my_nick = na->nick;

		if (!is_mine && !u->Account()->HasPriv("nickserv/drop"))
			source.Reply(LanguageString::ACCESS_DENIED);
		else if (Config->NSSecureAdmins && !is_mine && na->nc->IsServicesOper())
			source.Reply(LanguageString::ACCESS_DENIED);
		else
		{
			if (readonly)
				source.Reply(LanguageString::READ_ONLY_MODE);

			if (ircd->sqline && na->HasFlag(NS_FORBIDDEN))
			{
				XLine x(na->nick);
				ircdproto->SendSQLineDel(&x);
			}

			FOREACH_MOD(I_OnNickDrop, OnNickDrop(u, na));

			Log(!is_mine ? LOG_OVERRIDE : LOG_COMMAND, u, this) << "to drop nickname " << na->nick << " (group: " << na->nc->display << ") (email: " << (!na->nc->email.empty() ? na->nc->email : "none") << ")";
			delete na;

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

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		if (u->Account() && u->Account()->HasPriv("nickserv/drop"))
			source.Reply(_("Syntax: \002DROP [\037nickname\037]\002\n"
					" \n"
					"Without a parameter, drops your nickname from the\n"
					"%s database.\n"
					" \n"
					"With a parameter, drops the named nick from the database.\n"
					"You may drop any nick within your group without any \n"
					"special privileges. Dropping any nick is limited to \n"
					"\002Services Operators\002."), NickServ->nick.c_str());
		else
			source.Reply(_("Syntax: \002DROP [\037nickname\037 | \037password\037]\002\n"
					" \n"
					"Drops your nickname from the %s database.  A nick\n"
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
					"with your password (\002%R%s HELP IDENTIFY\002 for more\n"
					"information)."), NickServ->nick.c_str(), NickServ->nick.c_str());

		return true;
	}
};

class NSDrop : public Module
{
	CommandNSDrop commandnsdrop;

 public:
	NSDrop(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsdrop);
	}
};

MODULE_INIT(NSDrop)

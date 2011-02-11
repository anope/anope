/* HostServ core functions
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

class CommandHSSet : public Command
{
 public:
	CommandHSSet() : Command("SET", 2, 2, "hostserv/set")
	{
		this->SetDesc("Set the vhost of another user");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		Anope::string nick = params[0];
		Anope::string rawhostmask = params[1];
		Anope::string hostmask;

		Anope::string vIdent = myStrGetToken(rawhostmask, '@', 0); /* Get the first substring, @ as delimiter */
		if (!vIdent.empty())
		{
			rawhostmask = myStrGetTokenRemainder(rawhostmask, '@', 1); /* get the remaining string */
			if (rawhostmask.empty())
			{
				source.Reply(_("SET \002<nick>\002 \002<hostmask>\002."), Config->s_HostServ.c_str());
				return MOD_CONT;
			}
			if (vIdent.length() > Config->UserLen)
			{
				source.Reply(LanguageString::HOST_SET_IDENTTOOLONG, Config->UserLen);
				return MOD_CONT;
			}
			else
			{
				for (Anope::string::iterator s = vIdent.begin(), s_end = vIdent.end(); s != s_end; ++s)
					if (!isvalidchar(*s))
					{
						source.Reply(LanguageString::HOST_SET_IDENT_ERROR);
						return MOD_CONT;
					}
			}
			if (!ircd->vident)
			{
				source.Reply(LanguageString::HOST_NO_VIDENT);
				return MOD_CONT;
			}
		}
		if (rawhostmask.length() < Config->HostLen)
			hostmask = rawhostmask;
		else
		{
			source.Reply(LanguageString::HOST_SET_TOOLONG, Config->HostLen);
			return MOD_CONT;
		}

		if (!isValidHost(hostmask, 3))
		{
			source.Reply(LanguageString::HOST_SET_ERROR);
			return MOD_CONT;
		}

		NickAlias *na = findnick(nick);
		if ((na = findnick(nick)))
		{
			if (na->HasFlag(NS_FORBIDDEN))
			{
				source.Reply(LanguageString::NICK_X_FORBIDDEN, nick.c_str());
				return MOD_CONT;
			}

			Log(LOG_ADMIN, u, this) << "to set the vhost of " << na->nick << " to " << (!vIdent.empty() ? vIdent + "@" : "") << hostmask;

			na->hostinfo.SetVhost(vIdent, hostmask, u->nick);
			FOREACH_MOD(I_OnSetVhost, OnSetVhost(na));
			if (!vIdent.empty())
				source.Reply(_("vhost for \002%s\002 set to \002%s\002@\002%s\002."), nick.c_str(), vIdent.c_str(), hostmask.c_str());
			else
				source.Reply(_("vhost for \002%s\002 set to \002%s\002."), nick.c_str(), hostmask.c_str());
		}
		else
			source.Reply(LanguageString::NICK_X_NOT_REGISTERED, nick.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002SET\002 \002<nick>\002 \002<hostmask>\002.\n"
				"Sets the vhost for the given nick to that of the given\n"
				"hostmask.  If your IRCD supports vIdents, then using\n"
				"SET <nick> <ident>@<hostmask> set idents for users as \n"
				"well as vhosts."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "SET", _("SET \002<nick>\002 \002<hostmask>\002."));
	}
};

class HSSet : public Module
{
	CommandHSSet commandhsset;

 public:
	HSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(HostServ, &commandhsset);
	}
};

MODULE_INIT(HSSet)

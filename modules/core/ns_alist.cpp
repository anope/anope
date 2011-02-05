/* NickServ core functions
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

class CommandNSAList : public Command
{
 public:
	CommandNSAList() : Command("ALIST", 0, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		/*
		 * List the channels that the given nickname has access on
		 *
		 * /ns ALIST [level]
		 * /ns ALIST [nickname] [level]
		 *
		 * -jester
		 */

		Anope::string nick;

		NickAlias *na;

		int is_servadmin = u->Account()->IsServicesOper();
		unsigned lev_param = 0;

		if (!is_servadmin)
			/* Non service admins can only see their own levels */
			na = findnick(u->Account()->display);
		else
		{
			/* Services admins can request ALIST on nicks.
			 * The first argument for service admins must
			 * always be a nickname.
			 */
			nick = !params.empty() ? params[0] : "";
			lev_param = 1;

			/* If an argument was passed, use it as the nick to see levels
			 * for, else check levels for the user calling the command */
			na = findnick(!nick.empty() ? nick : u->nick);
		}

		/* If available, get level from arguments */
		Anope::string lev = params.size() > lev_param ? params[lev_param] : "";

		/* if a level was given, make sure it's an int for later */
		int min_level = ACCESS_INVALID;
		if (!lev.empty())
		{
			if (lev.equals_ci("FOUNDER"))
				min_level = ACCESS_FOUNDER;
			else if (lev.equals_ci("SOP"))
				min_level = ACCESS_SOP;
			else if (lev.equals_ci("AOP"))
				min_level = ACCESS_AOP;
			else if (lev.equals_ci("HOP"))
				min_level = ACCESS_HOP;
			else if (lev.equals_ci("VOP"))
				min_level = ACCESS_VOP;
			else
			{
				try
				{
					min_level = convertTo<int>(lev);
				}
				catch (const CoreException &) { }
			}
		}

		if (!na)
			u->SendMessage(NickServ, NICK_X_NOT_REGISTERED, nick.c_str());
		else if (na->HasFlag(NS_FORBIDDEN))
			u->SendMessage(NickServ, NICK_X_FORBIDDEN, na->nick.c_str());
		else if (min_level <= ACCESS_INVALID || min_level > ACCESS_FOUNDER)
			u->SendMessage(NickServ, CHAN_ACCESS_LEVEL_RANGE, ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
		else
		{
			int level;
			int chan_count = 0;
			int match_count = 0;

			u->SendMessage(NickServ, is_servadmin ? NICK_ALIST_HEADER_X : NICK_ALIST_HEADER, na->nick.c_str());

			for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
			{
				ChannelInfo *ci = it->second;

				if ((level = get_access_level(ci, na)))
				{
					++chan_count;

					if (min_level > level)
						continue;

					++match_count;

					if (ci->HasFlag(CI_XOP) || level == ACCESS_FOUNDER)
					{
						Anope::string xop = get_xop_level(level);

						u->SendMessage(NickServ, NICK_ALIST_XOP_FORMAT, match_count, ci->HasFlag(CI_NO_EXPIRE) ? '!' : ' ', ci->name.c_str(), xop.c_str(), !ci->desc.empty() ? ci->desc.c_str() : "");
					}
					else
						u->SendMessage(NickServ, NICK_ALIST_ACCESS_FORMAT, match_count, ci->HasFlag(CI_NO_EXPIRE) ? '!' : ' ', ci->name.c_str(), level, !ci->desc.empty() ? ci->desc.c_str() : "");
				}
			}

			u->SendMessage(NickServ, NICK_ALIST_FOOTER, match_count, chan_count);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		if (u->Account() && u->Account()->IsServicesOper())
			u->SendMessage(NickServ, NICK_SERVADMIN_HELP_ALIST);
		else
			u->SendMessage(NickServ, NICK_HELP_ALIST);

		return true;
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(NickServ, NICK_HELP_CMD_ALIST);
	}
};

class NSAList : public Module
{
	CommandNSAList commandnsalist;

 public:
	NSAList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(NickServ, &commandnsalist);
	}
};

MODULE_INIT(NSAList)

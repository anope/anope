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

class CommandNSAList : public Command
{
 public:
	CommandNSAList() : Command("ALIST", 0, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		/*
		 * List the channels that the given nickname has access on
		 *
		 * /ns ALIST [level]
		 * /ns ALIST [nickname] [level]
		 *
		 * -jester
		 */

		User *u = source.u;
		Anope::string nick;
		NickAlias *na;

		int min_level = 0;
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
				min_level = lev.is_number_only() ? convertTo<int>(lev) : ACCESS_INVALID;
		}

		if (!na)
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
		else if (na->HasFlag(NS_FORBIDDEN))
			source.Reply(NICK_X_FORBIDDEN, na->nick.c_str());
		else if (min_level <= ACCESS_INVALID || min_level > ACCESS_FOUNDER)
			source.Reply(CHAN_ACCESS_LEVEL_RANGE, ACCESS_INVALID + 1, ACCESS_FOUNDER - 1);
		else
		{
			int chan_count = 0;
			int match_count = 0;

			source.Reply(is_servadmin ? NICK_ALIST_HEADER_X : NICK_ALIST_HEADER, na->nick.c_str());

			for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; ++it)
			{
				ChannelInfo *ci = it->second;

				ChanAccess *access = ci->GetAccess(na->nc);
				if (access)
				{
					++chan_count;

					if (min_level > access->level)
						continue;

					++match_count;

					if (ci->HasFlag(CI_XOP) || access->level == ACCESS_FOUNDER)
					{
						Anope::string xop = get_xop_level(access->level);

						source.Reply(NICK_ALIST_XOP_FORMAT, match_count, ci->HasFlag(CI_NO_EXPIRE) ? '!' : ' ', ci->name.c_str(), xop.c_str(), !ci->desc.empty() ? ci->desc.c_str() : "");
					}
					else
						source.Reply(NICK_ALIST_ACCESS_FORMAT, match_count, ci->HasFlag(CI_NO_EXPIRE) ? '!' : ' ', ci->name.c_str(), access->level, !ci->desc.empty() ? ci->desc.c_str() : "");
				}
			}

			source.Reply(NICK_ALIST_FOOTER, match_count, chan_count);
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		User *u = source.u;
		if (u->Account() && u->Account()->IsServicesOper())
			source.Reply(NICK_SERVADMIN_HELP_ALIST);
		else
			source.Reply(NICK_HELP_ALIST);

		return true;
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(NICK_HELP_CMD_ALIST);
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

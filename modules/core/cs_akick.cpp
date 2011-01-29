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

/* Split a usermask up into its constitutent parts.  Returned strings are
 * malloc()'d, and should be free()'d when done with.  Returns "*" for
 * missing parts.
 */

static void split_usermask(const Anope::string &mask, Anope::string &nick, Anope::string &user, Anope::string &host)
{
	size_t ex = mask.find('!'), at = mask.find('@', ex == Anope::string::npos ? 0 : ex + 1);
	if (ex == Anope::string::npos)
	{
		if (at == Anope::string::npos)
		{
			nick = mask;
			user = host = "*";
		}
		else
		{
			nick = "*";
			user = mask.substr(0, at);
			host = mask.substr(at + 1);
		}
	}
	else
	{
		nick = mask.substr(0, ex);
		if (at == Anope::string::npos)
		{
			user = mask.substr(ex + 1);
			host = "*";
		}
		else
		{
			user = mask.substr(ex + 1, at - ex - 1);
			host = mask.substr(at + 1);
		}
	}
}

class AkickListCallback : public NumberList
{
 protected:
	CommandSource &source;
	bool SentHeader;
 public:
	AkickListCallback(CommandSource &_source, const Anope::string &numlist) : NumberList(numlist, false), source(_source), SentHeader(false)
	{
	}

	~AkickListCallback()
	{
		if (!SentHeader)
			source.Reply(CHAN_AKICK_NO_MATCH, source.ci->name.c_str());
	}

	virtual void HandleNumber(unsigned Number)
	{
		if (!Number || Number > source.ci->GetAkickCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(CHAN_AKICK_LIST_HEADER, source.ci->name.c_str());
		}

		DoList(source, Number - 1, source.ci->GetAkick(Number - 1));
	}

	static void DoList(CommandSource &source, unsigned index, AutoKick *akick)
	{
		source.Reply(CHAN_AKICK_LIST_FORMAT, index + 1, akick->HasFlag(AK_ISNICK) ? akick->nc->display.c_str() : akick->mask.c_str(), !akick->reason.empty() ? akick->reason.c_str() : GetString(source.u, NO_REASON).c_str());
	}
};

class AkickViewCallback : public AkickListCallback
{
 public:
	AkickViewCallback(CommandSource &_source, const Anope::string &numlist) : AkickListCallback(_source, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > source.ci->GetAkickCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(CHAN_AKICK_LIST_HEADER, source.ci->name.c_str());
		}

		DoList(source, Number - 1, source.ci->GetAkick(Number - 1));
	}

	static void DoList(CommandSource &source, unsigned index, AutoKick *akick)
	{
		User *u = source.u;
		Anope::string timebuf;

		if (akick->addtime)
			timebuf = do_strftime(akick->addtime);
		else
			timebuf = GetString(u, UNKNOWN);

		source.Reply(CHAN_AKICK_VIEW_FORMAT, index + 1, akick->HasFlag(AK_ISNICK) ? akick->nc->display.c_str() : akick->mask.c_str(), !akick->creator.empty() ? akick->creator.c_str() : GetString(u, UNKNOWN).c_str(), timebuf.c_str(), !akick->reason.empty() ? akick->reason.c_str() : GetString(u, NO_REASON).c_str());

		if (akick->last_used)
			source.Reply(CHAN_AKICK_LAST_USED, do_strftime(akick->last_used).c_str());
	}
};

class AkickDelCallback : public NumberList
{
	CommandSource &source;
	Command *c;
	unsigned Deleted;
 public:
	AkickDelCallback(CommandSource &_source, Command *_c, const Anope::string &list) : NumberList(list, true), source(_source), c(_c), Deleted(0)
	{
	}

	~AkickDelCallback()
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;
		bool override = !check_access(u, ci, CA_AKICK);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, c, ci) << "DEL on " << Deleted << " users";

		if (!Deleted)
			source.Reply(CHAN_AKICK_NO_MATCH, ci->name.c_str());
		else if (Deleted == 1)
			source.Reply(CHAN_AKICK_DELETED_ONE, ci->name.c_str());
		else
			source.Reply(CHAN_AKICK_DELETED_SEVERAL, Deleted, ci->name.c_str());
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > source.ci->GetAkickCount())
			return;

		++Deleted;
		source.ci->EraseAkick(Number - 1);
	}
};

class CommandCSAKick : public Command
{
	void DoAdd(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;

		Anope::string mask = params[2];
		Anope::string reason = params.size() > 3 ? params[3] : "";
		NickAlias *na = findnick(mask);
		NickCore *nc = NULL;
		AutoKick *akick;

		if (!na)
		{
			Anope::string nick, user, host;

			split_usermask(mask, nick, user, host);
			mask = nick + "!" + user + "@" + host;
		}
		else
		{
			if (na->HasFlag(NS_FORBIDDEN))
			{
				source.Reply(NICK_X_FORBIDDEN, mask.c_str());
				return;
			}

			nc = na->nc;
		}

		/* Check excepts BEFORE we get this far */
		if (ModeManager::FindChannelModeByName(CMODE_EXCEPT) && is_excepted_mask(ci, mask))
		{
			source.Reply(CHAN_EXCEPTED, mask.c_str(), ci->name.c_str());
			return;
		}

		/* Check whether target nick has equal/higher access
		* or whether the mask matches a user with higher/equal access - Viper */
		if (ci->HasFlag(CI_PEACE) && nc)
		{
			ChanAccess *nc_access = ci->GetAccess(nc), *u_access = ci->GetAccess(u);
			int16 nc_level = nc_access ? nc_access->level : 0, u_level = u_access ? u_access->level : 0;
			if (nc == ci->founder || nc_level >= u_level)
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
		}
		else if (ci->HasFlag(CI_PEACE))
		{
			/* Match against all currently online users with equal or
			 * higher access. - Viper */
			for (patricia_tree<User *, ci::ci_char_traits>::iterator it(UserListByNick); it.next();)
			{
				User *u2 = *it;

				ChanAccess *u2_access = ci->GetAccess(nc), *u_access = ci->GetAccess(u);
				int16 u2_level = u2_access ? u2_access->level : 0, u_level = u_access ? u_access->level : 0;
				Entry entry_mask(mask);

				if ((check_access(u2, ci, CA_FOUNDER) || u2_level >= u_level) && entry_mask.Matches(u2))
				{
					source.Reply(ACCESS_DENIED);
					return;
				}
			}

			/* Match against the lastusermask of all nickalias's with equal
			 * or higher access. - Viper */
			for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; ++it)
			{
				NickAlias *na2 = it->second;

				if (na2->HasFlag(NS_FORBIDDEN))
					continue;

				ChanAccess *na2_access = ci->GetAccess(na2->nc), *u_access = ci->GetAccess(u);
				int16 na2_level = na2_access ? na2_access->level : 0, u_level = u_access ? u_access->level : 0;
				if (na2->nc && (na2->nc == ci->founder || na2_level >= u_level))
				{
					Anope::string buf = na2->nick + "!" + na2->last_usermask;
					if (Anope::Match(buf, mask))
					{
						source.Reply(ACCESS_DENIED);
						return;
					}
				}
			 }
		}

		for (unsigned j = 0, end = ci->GetAkickCount(); j < end; ++j)
		{
			akick = ci->GetAkick(j);
			if (akick->HasFlag(AK_ISNICK) ? akick->nc == nc : mask.equals_ci(akick->mask))
			{
				source.Reply(CHAN_AKICK_ALREADY_EXISTS, akick->HasFlag(AK_ISNICK) ? akick->nc->display.c_str() : akick->mask.c_str(), ci->name.c_str());
				return;
			}
		}

		if (ci->GetAkickCount() >= Config->CSAutokickMax)
		{
			source.Reply(CHAN_AKICK_REACHED_LIMIT, Config->CSAutokickMax);
			return;
		}

		if (nc)
			akick = ci->AddAkick(u->nick, nc, reason);
		else
			akick = ci->AddAkick(u->nick, mask, reason);

		bool override = !check_access(u, ci, CA_AKICK);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "ADD " << mask << ": " << reason;

		FOREACH_MOD(I_OnAkickAdd, OnAkickAdd(u, ci, akick));

		source.Reply(CHAN_AKICK_ADDED, mask.c_str(), ci->name.c_str());

		this->DoEnforce(source);
	}

	void DoDel(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;

		const Anope::string &mask = params[2];
		AutoKick *akick;
		unsigned i, end;

		if (!ci->GetAkickCount())
		{
			source.Reply(CHAN_AKICK_LIST_EMPTY, ci->name.c_str());
			return;
		}

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AkickDelCallback list(source, this, mask);
			list.Process();
		}
		else
		{
			NickAlias *na = findnick(mask);
			NickCore *nc = na ? na->nc : NULL;

			for (i = 0, end = ci->GetAkickCount(); i < end; ++i)
			{
				akick = ci->GetAkick(i);

				if ((akick->HasFlag(AK_ISNICK) && akick->nc == nc) || (!akick->HasFlag(AK_ISNICK) && mask.equals_ci(akick->mask)))
					break;
			}

			if (i == ci->GetAkickCount())
			{
				source.Reply(CHAN_AKICK_NOT_FOUND, mask.c_str(), ci->name.c_str());
				return;
			}

			bool override = !check_access(u, ci, CA_AKICK);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "DEL " << mask;

			ci->EraseAkick(i);

			source.Reply(CHAN_AKICK_DELETED, mask.c_str(), ci->name.c_str());
		}
	}

	void DoList(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;

		const Anope::string &mask = params.size() > 2 ? params[2] : "";

		bool override = !check_access(u, ci, CA_AKICK);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "LIST";

		if (!ci->GetAkickCount())
		{
			source.Reply(CHAN_AKICK_LIST_EMPTY, ci->name.c_str());
			return;
		}

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AkickListCallback list(source, mask);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = ci->GetAkickCount(); i < end; ++i)
			{
				AutoKick *akick = ci->GetAkick(i);

				if (!mask.empty())
				{
					if (!akick->HasFlag(AK_ISNICK) && !Anope::Match(akick->mask, mask))
						continue;
					if (akick->HasFlag(AK_ISNICK) && !Anope::Match(akick->nc->display, mask))
						continue;
				}

				if (!SentHeader)
				{
					SentHeader = true;
					source.Reply(CHAN_AKICK_LIST_HEADER, ci->name.c_str());
				}

				AkickListCallback::DoList(source, i, akick);
			}

			if (!SentHeader)
				source.Reply(CHAN_AKICK_NO_MATCH, ci->name.c_str());
		}
	}

	void DoView(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;

		const Anope::string &mask = params.size() > 2 ? params[2] : "";

		bool override = !check_access(u, ci, CA_AKICK);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "VIEW";

		if (!ci->GetAkickCount())
		{
			source.Reply(CHAN_AKICK_LIST_EMPTY, ci->name.c_str());
			return;
		}

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AkickViewCallback list(source, mask);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = ci->GetAkickCount(); i < end; ++i)
			{
				AutoKick *akick = ci->GetAkick(i);

				if (!mask.empty())
				{
					if (!akick->HasFlag(AK_ISNICK) && !Anope::Match(akick->mask, mask))
						continue;
					if (akick->HasFlag(AK_ISNICK) && !Anope::Match(akick->nc->display, mask))
						continue;
				}

				if (!SentHeader)
				{
					SentHeader = true;
					source.Reply(CHAN_AKICK_LIST_HEADER, ci->name.c_str());
				}

				AkickViewCallback::DoList(source, i, akick);
			}

			if (!SentHeader)
				source.Reply(CHAN_AKICK_NO_MATCH, ci->name.c_str());
		}
	}

	void DoEnforce(CommandSource &source)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Channel *c = ci->c;
		int count = 0;

		if (!c)
		{
			source.Reply(CHAN_X_NOT_IN_USE, ci->name.c_str());
			return;
		}

		for (CUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end; )
		{
			UserContainer *uc = *it++;

			if (ci->CheckKick(uc->user))
				++count;
		}

		bool override = !check_access(u, ci, CA_AKICK);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "ENFORCE, affects " << count << " users";

		source.Reply(CHAN_AKICK_ENFORCE_DONE, ci->name.c_str(), count);
	}

	void DoClear(CommandSource &source)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;
		bool override = !check_access(u, ci, CA_AKICK);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "CLEAR";

		ci->ClearAkick();
		source.Reply(CHAN_AKICK_CLEAR, ci->name.c_str());
	}

 public:
	CommandCSAKick() : Command("AKICK", 2, 4)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Anope::string cmd = params[1];
		Anope::string mask = params.size() > 2 ? params[2] : "";

		User *u = source.u;
		ChannelInfo *ci = source.ci;

		if (mask.empty() && (cmd.equals_ci("ADD") || cmd.equals_ci("DEL")))
			this->OnSyntaxError(source, cmd);
		else if (!check_access(u, ci, CA_AKICK) && !u->Account()->HasPriv("chanserv/access/modify"))
			source.Reply(ACCESS_DENIED);
		else if (!cmd.equals_ci("LIST") && !cmd.equals_ci("VIEW") && !cmd.equals_ci("ENFORCE") && readonly)
			source.Reply(CHAN_AKICK_DISABLED);
		else if (cmd.equals_ci("ADD"))
			this->DoAdd(source, params);
		else if (cmd.equals_ci("DEL"))
			this->DoDel(source, params);
		else if (cmd.equals_ci("LIST"))
			this->DoList(source, params);
		else if (cmd.equals_ci("VIEW"))
			this->DoView(source, params);
		else if (cmd.equals_ci("ENFORCE"))
			this->DoEnforce(source);
		else if (cmd.equals_ci("CLEAR"))
			this->DoClear(source);
		else
			this->OnSyntaxError(source, "");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(CHAN_HELP_AKICK);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "AKICK", CHAN_AKICK_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(CHAN_HELP_CMD_AKICK);
	}
};

class CSAKick : public Module
{
	CommandCSAKick commandcsakick;

 public:
	CSAKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcsakick);
	}
};

MODULE_INIT(CSAKick)

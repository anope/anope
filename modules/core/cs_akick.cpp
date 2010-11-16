/* ChanServ core functions
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
	User *u;
	ChannelInfo *ci;
	bool SentHeader;
 public:
	AkickListCallback(User *_u, ChannelInfo *_ci, const Anope::string &numlist) : NumberList(numlist, false), u(_u), ci(_ci), SentHeader(false)
	{
	}

	~AkickListCallback()
	{
		if (!SentHeader)
			u->SendMessage(ChanServ, CHAN_AKICK_NO_MATCH, ci->name.c_str());
	}

	virtual void HandleNumber(unsigned Number)
	{
		if (!Number || Number > ci->GetAkickCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			u->SendMessage(ChanServ, CHAN_AKICK_LIST_HEADER, ci->name.c_str());
		}

		DoList(u, ci, Number - 1, ci->GetAkick(Number - 1));
	}

	static void DoList(User *u, ChannelInfo *ci, unsigned index, AutoKick *akick)
	{
		u->SendMessage(ChanServ, CHAN_AKICK_LIST_FORMAT, index + 1, akick->HasFlag(AK_ISNICK) ? akick->nc->display.c_str() :akick->mask.c_str(), !akick->reason.empty() ? akick->reason.c_str() : GetString(u, NO_REASON).c_str());
	}
};

class AkickViewCallback : public AkickListCallback
{
 public:
	AkickViewCallback(User *_u, ChannelInfo *_ci, const Anope::string &numlist) : AkickListCallback(_u, _ci, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > ci->GetAkickCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			u->SendMessage(ChanServ, CHAN_AKICK_LIST_HEADER, ci->name.c_str());
		}

		DoList(u, ci, Number - 1, ci->GetAkick(Number - 1));
	}

	static void DoList(User *u, ChannelInfo *ci, unsigned index, AutoKick *akick)
	{
		Anope::string timebuf;
		if (akick->addtime)
			timebuf = do_strftime(akick->addtime);
		else
			timebuf = GetString(u, UNKNOWN);

		u->SendMessage(ChanServ, CHAN_AKICK_VIEW_FORMAT, index + 1, akick->HasFlag(AK_ISNICK) ? akick->nc->display.c_str() : akick->mask.c_str(), !akick->creator.empty() ? akick->creator.c_str() : GetString(u, UNKNOWN).c_str(), timebuf.c_str(), !akick->reason.empty() ? akick->reason.c_str() : GetString(u, NO_REASON).c_str());

		if (akick->last_used)
			u->SendMessage(ChanServ, CHAN_AKICK_LAST_USED, do_strftime(akick->last_used).c_str());
	}
};

class AkickDelCallback : public NumberList
{
	User *u;
	ChannelInfo *ci;
	Command *c;
	unsigned Deleted;
 public:
	AkickDelCallback(User *_u, ChannelInfo *_ci, Command *_c, const Anope::string &list) : NumberList(list, true), u(_u), ci(_ci), c(_c), Deleted(0)
	{
	}

	~AkickDelCallback()
	{
		bool override = !check_access(u, ci, CA_AKICK);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, c, ci) << "DEL on " << Deleted << " users";

		if (!Deleted)
			u->SendMessage(ChanServ, CHAN_AKICK_NO_MATCH, ci->name.c_str());
		else if (Deleted == 1)
			u->SendMessage(ChanServ, CHAN_AKICK_DELETED_ONE, ci->name.c_str());
		else
			u->SendMessage(ChanServ, CHAN_AKICK_DELETED_SEVERAL, Deleted, ci->name.c_str());
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > ci->GetAkickCount())
			return;

		++Deleted;
		ci->EraseAkick(Number - 1);
	}
};

class CommandCSAKick : public Command
{
	void DoAdd(User *u, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
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
				u->SendMessage(ChanServ, NICK_X_FORBIDDEN, mask.c_str());
				return;
			}

			nc = na->nc;
		}

		/* Check excepts BEFORE we get this far */
		if (ModeManager::FindChannelModeByName(CMODE_EXCEPT) && is_excepted_mask(ci, mask))
		{
			u->SendMessage(ChanServ, CHAN_EXCEPTED, mask.c_str(), ci->name.c_str());
			return;
		}

		/* Check whether target nick has equal/higher access
		* or whether the mask matches a user with higher/equal access - Viper */
		if (ci->HasFlag(CI_PEACE) && nc)
		{
			if (nc == ci->founder || get_access_level(ci, nc) >= get_access(u, ci))
			{
				u->SendMessage(ChanServ, ACCESS_DENIED);
				return;
			}
		}
		else if (ci->HasFlag(CI_PEACE))
		{
			/* Match against all currently online users with equal or
			 * higher access. - Viper */
			for (patricia_tree<User>::const_iterator it = UserListByNick.begin(), it_end = UserListByNick.end(); it != it_end; ++it)
			{
				User *u2 = *it;

				if ((check_access(u2, ci, CA_FOUNDER) || get_access(u2, ci) >= get_access(u, ci)) && match_usermask(mask, u2))
				{
					u->SendMessage(ChanServ, ACCESS_DENIED);
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

				if (na2->nc && (na2->nc == ci->founder || get_access_level(ci, na2->nc) >= get_access(u, ci)))
				{
					Anope::string buf = na2->nick + "!" + na2->last_usermask;
					if (Anope::Match(buf, mask))
					{
						u->SendMessage(ChanServ, ACCESS_DENIED);
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
				u->SendMessage(ChanServ, CHAN_AKICK_ALREADY_EXISTS, akick->HasFlag(AK_ISNICK) ? akick->nc->display.c_str() : akick->mask.c_str(), ci->name.c_str());
				return;
			}
		}

		if (ci->GetAkickCount() >= Config->CSAutokickMax)
		{
			u->SendMessage(ChanServ, CHAN_AKICK_REACHED_LIMIT, Config->CSAutokickMax);
			return;
		}

		if (nc)
			akick = ci->AddAkick(u->nick, nc, reason);
		else
			akick = ci->AddAkick(u->nick, mask, reason);

		bool override = !check_access(u, ci, CA_AKICK);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "ADD " << mask << ": " << reason;

		FOREACH_MOD(I_OnAkickAdd, OnAkickAdd(u, ci, akick));

		u->SendMessage(ChanServ, CHAN_AKICK_ADDED, mask.c_str(), ci->name.c_str());

		this->DoEnforce(u, ci);
	}

	void DoDel(User *u, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string mask = params[2];
		AutoKick *akick;
		unsigned i, end;

		if (!ci->GetAkickCount())
		{
			u->SendMessage(ChanServ, CHAN_AKICK_LIST_EMPTY, ci->name.c_str());
			return;
		}

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AkickDelCallback list(u, ci, this, mask);
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
				u->SendMessage(ChanServ, CHAN_AKICK_NOT_FOUND, mask.c_str(), ci->name.c_str());
				return;
			}

			bool override = !check_access(u, ci, CA_AKICK);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "DEL " << mask;

			ci->EraseAkick(i);

			u->SendMessage(ChanServ, CHAN_AKICK_DELETED, mask.c_str(), ci->name.c_str());
		}
	}

	void DoList(User *u, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string mask = params.size() > 2 ? params[2] : "";

		bool override = !check_access(u, ci, CA_AKICK);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "LIST";

		if (!ci->GetAkickCount())
		{
			u->SendMessage(ChanServ, CHAN_AKICK_LIST_EMPTY, ci->name.c_str());
			return;
		}

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AkickListCallback list(u, ci, mask);
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
					u->SendMessage(ChanServ, CHAN_AKICK_LIST_HEADER, ci->name.c_str());
				}

				AkickListCallback::DoList(u, ci, i, akick);
			}

			if (!SentHeader)
				u->SendMessage(ChanServ, CHAN_AKICK_NO_MATCH, ci->name.c_str());
		}
	}

	void DoView(User *u, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		Anope::string mask = params.size() > 2 ? params[2] : "";

		bool override = !check_access(u, ci, CA_AKICK);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "VIEW";

		if (!ci->GetAkickCount())
		{
			u->SendMessage(ChanServ, CHAN_AKICK_LIST_EMPTY, ci->name.c_str());
			return;
		}

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AkickViewCallback list(u, ci, mask);
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
					u->SendMessage(ChanServ, CHAN_AKICK_LIST_HEADER, ci->name.c_str());
				}

				AkickViewCallback::DoList(u, ci, i, akick);
			}

			if (!SentHeader)
				u->SendMessage(ChanServ, CHAN_AKICK_NO_MATCH, ci->name.c_str());
		}
	}

	void DoEnforce(User *u, ChannelInfo *ci)
	{
		Channel *c = ci->c;
		int count = 0;

		if (!c)
		{
			u->SendMessage(ChanServ, CHAN_X_NOT_IN_USE, ci->name.c_str());
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

		u->SendMessage(ChanServ, CHAN_AKICK_ENFORCE_DONE, ci->name.c_str(), count);
	}

	void DoClear(User *u, ChannelInfo *ci)
	{
		bool override = !check_access(u, ci, CA_AKICK);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "CLEAR";

		ci->ClearAkick();
		u->SendMessage(ChanServ, CHAN_AKICK_CLEAR, ci->name.c_str());
	}

 public:
	CommandCSAKick() : Command("AKICK", 2, 4)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Anope::string cmd = params[1];
		Anope::string mask = params.size() > 2 ? params[2] : "";

		ChannelInfo *ci = cs_findchan(chan);

		if (mask.empty() && (cmd.equals_ci("ADD") || cmd.equals_ci("DEL")))
			this->OnSyntaxError(u, cmd);
		else if (!check_access(u, ci, CA_AKICK) && !u->Account()->HasPriv("chanserv/access/modify"))
			u->SendMessage(ChanServ, ACCESS_DENIED);
		else if (!cmd.equals_ci("LIST") && !cmd.equals_ci("VIEW") && !cmd.equals_ci("ENFORCE") && readonly)
			u->SendMessage(ChanServ, CHAN_AKICK_DISABLED);
		else if (cmd.equals_ci("ADD"))
			this->DoAdd(u, ci, params);
		else if (cmd.equals_ci("DEL"))
			this->DoDel(u, ci, params);
		else if (cmd.equals_ci("LIST"))
			this->DoList(u, ci, params);
		else if (cmd.equals_ci("VIEW"))
			this->DoView(u, ci, params);
		else if (cmd.equals_ci("ENFORCE"))
			this->DoEnforce(u, ci);
		else if (cmd.equals_ci("CLEAR"))
			this->DoClear(u, ci);
		else
			this->OnSyntaxError(u, "");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_AKICK);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "AKICK", CHAN_AKICK_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_AKICK);
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

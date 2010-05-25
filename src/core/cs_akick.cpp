/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */
/*************************************************************************/

#include "module.h"

/* Split a usermask up into its constitutent parts.  Returned strings are
 * malloc()'d, and should be free()'d when done with.  Returns "*" for
 * missing parts.
 */

static void split_usermask(const char *mask, const char **nick, const char **user,
					const char **host)
{
	char *mask2 = sstrdup(mask);

	*nick = strtok(mask2, "!");
	*user = strtok(NULL, "@");
	*host = strtok(NULL, "");
	/* Handle special case: mask == user@host */
	if (*nick && !*user && strchr(*nick, '@')) {
		*nick = NULL;
		*user = strtok(mask2, "@");
		*host = strtok(NULL, "");
	}
	if (!*nick)
		*nick = "*";
	if (!*user)
		*user = "*";
	if (!*host)
		*host = "*";
	*nick = sstrdup(*nick);
	*user = sstrdup(*user);
	*host = sstrdup(*host);
	delete [] mask2;
}

class AkickListCallback : public NumberList
{
 protected:
	User *u;
	ChannelInfo *ci;
	bool SentHeader;
 public:
	AkickListCallback(User *_u, ChannelInfo *_ci, const std::string &numlist) : NumberList(numlist, false), u(_u), ci(_ci), SentHeader(false)
	{
	}

	~AkickListCallback()
	{
		if (!SentHeader)
			notice_lang(Config.s_ChanServ, u, CHAN_AKICK_NO_MATCH, ci->name.c_str());
	}

	virtual void HandleNumber(unsigned Number)
	{
		if (Number > ci->GetAkickCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config.s_ChanServ, u, CHAN_AKICK_LIST_HEADER, ci->name.c_str());
		}

		DoList(u, ci, Number - 1, ci->GetAkick(Number - 1));
	}

	static void DoList(User *u, ChannelInfo *ci, unsigned index, AutoKick *akick)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_AKICK_LIST_FORMAT, index + 1,
			((akick->HasFlag(AK_ISNICK)) ? akick->nc->display : akick->mask.c_str()),
			(!akick->reason.empty() ? akick->reason.c_str() : getstring(u, NO_REASON)));
	}
};

class AkickViewCallback : public AkickListCallback
{
 public:
	AkickViewCallback(User *_u, ChannelInfo *_ci, const std::string &numlist) : AkickListCallback(u, ci, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (Number > ci->GetAkickCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config.s_ChanServ, u, CHAN_AKICK_LIST_HEADER, ci->name.c_str());
		}

		DoList(u, ci, Number - 1, ci->GetAkick(Number - 1));
	}

	static void DoList(User *u, ChannelInfo *ci, unsigned index, AutoKick *akick)
	{
		char timebuf[64];
		struct tm tm;

		memset(&timebuf, 0, sizeof(timebuf));

		if (akick->addtime)
		{
			tm = *localtime(&akick->addtime);
			strftime_lang(timebuf, sizeof(timebuf), u, STRFTIME_SHORT_DATE_FORMAT, &tm);
		}
		else
			snprintf(timebuf, sizeof(timebuf), "%s", getstring(u, UNKNOWN));

		notice_lang(Config.s_ChanServ, u, (akick->HasFlag(AK_STUCK) ? CHAN_AKICK_VIEW_FORMAT_STUCK : CHAN_AKICK_VIEW_FORMAT), index + 1,
			((akick->HasFlag(AK_ISNICK)) ? akick->nc->display : akick->mask.c_str()),
			!akick->creator.empty() ? akick->creator.c_str() : getstring(u, UNKNOWN), timebuf,
			(!akick->reason.empty() ? akick->reason.c_str() : getstring(u, NO_REASON)));

		if (akick->last_used)
		{
			char last_used[64];
			tm = *localtime(&akick->last_used);
			strftime_lang(last_used, sizeof(last_used), u, STRFTIME_SHORT_DATE_FORMAT, &tm);
			notice_lang(Config.s_ChanServ, u, CHAN_AKICK_LAST_USED, last_used);
		}
	}
};

class AkickDelCallback : public NumberList
{
	User *u;
	ChannelInfo *ci;
	unsigned Deleted;
 public:
	AkickDelCallback(User *_u, ChannelInfo *_ci, const std::string &list) : NumberList(list, true), u(_u), ci(_ci), Deleted(0)
	{
	}

	~AkickDelCallback()
	{
		if (!Deleted)
			notice_lang(Config.s_ChanServ, u, CHAN_AKICK_NO_MATCH, ci->name.c_str());
		else if (Deleted == 1)
			notice_lang(Config.s_ChanServ, u, CHAN_AKICK_DELETED_ONE, ci->name.c_str());
		else
			notice_lang(Config.s_ChanServ, u, CHAN_AKICK_DELETED_SEVERAL, Deleted, ci->name.c_str());
	}

	void HandleNumber(unsigned Number)
	{
		if (Number > ci->GetAkickCount())
			return;

		++Deleted;
		ci->EraseAkick(Number - 1);
	}
};

class CommandCSAKick : public Command
{
	void DoAdd(User *u, ChannelInfo *ci, const std::vector<ci::string> &params)
	{
		ci::string mask = params[2];
		ci::string reason = params.size() > 3 ? params[3] : "";
		NickAlias *na = findnick(mask.c_str());
		NickCore *nc = NULL;
		AutoKick *akick;

		if (!na)
		{
			const char *nick, *user, *host;

			split_usermask(mask.c_str(), &nick, &user, &host);
			mask = ci::string(nick) + "!" + user + "@" + host;
			delete [] nick;
			delete [] user;
			delete [] host;
		}
		else
		{
			 if (na->HasFlag(NS_FORBIDDEN))
			 {
				notice_lang(Config.s_ChanServ, u, NICK_X_FORBIDDEN, mask.c_str());
				return;
			 }

			 nc = na->nc;
		}

		/* Check excepts BEFORE we get this far */
		if (ModeManager::FindChannelModeByName(CMODE_EXCEPT))
		{
			 if (is_excepted_mask(ci, mask.c_str()))
			 {
				notice_lang(Config.s_ChanServ, u, CHAN_EXCEPTED, mask.c_str(), ci->name.c_str());
				return;
			 }
		}

		/* Check whether target nick has equal/higher access
		* or whether the mask matches a user with higher/equal access - Viper */
		if ((ci->HasFlag(CI_PEACE)) && nc)
		{
			 if ((nc == ci->founder) || (get_access_level(ci, nc) >= get_access(u, ci)))
			 {
				notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
				return;
			 }
		}
		else if ((ci->HasFlag(CI_PEACE)))
		{
			 /* Match against all currently online users with equal or
			  * higher access. - Viper */
			 for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
			 {
			 	User *u2 = it->second;
				
				if (check_access(u2, ci, CA_FOUNDER) || (get_access(u2, ci) >= get_access(u, ci)))
				{
					if (match_usermask(mask.c_str(), u2))
					{
						notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
						return;
					}
				}
			 }

			/* Match against the lastusermask of all nickalias's with equal
			 * or higher access. - Viper */
			for (nickalias_map::const_iterator it = NickAliasList.begin(); it != NickAliasList.end(); ++it)
			{
				NickAlias *na2 = it->second;
				
				if (na2->HasFlag(NS_FORBIDDEN))
					continue;

				if (na2->nc && ((na2->nc == ci->founder) || (get_access_level(ci, na2->nc) >= get_access(u, ci))))
				{
					char buf[BUFSIZE];

					snprintf(buf, BUFSIZE, "%s!%s", na2->nick, na2->last_usermask);
					if (Anope::Match(buf, mask.c_str(), false))
					{
						notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
						return;
					}
				}
			 }
		}

		for (unsigned j = 0; j < ci->GetAkickCount(); ++j)
		{
			akick = ci->GetAkick(j);
			if ((akick->HasFlag(AK_ISNICK)) ? akick->nc == nc : akick->mask == mask)
			{
				notice_lang(Config.s_ChanServ, u, CHAN_AKICK_ALREADY_EXISTS, (akick->HasFlag(AK_ISNICK)) ? akick->nc->display : akick->mask.c_str(), ci->name.c_str());
				return;
			}
		}


		if (ci->GetAkickCount() >= Config.CSAutokickMax)
		{
			 notice_lang(Config.s_ChanServ, u, CHAN_AKICK_REACHED_LIMIT, Config.CSAutokickMax);
			 return;
		}

		if (nc)
			akick = ci->AddAkick(u->nick, nc, !reason.empty() ? reason.c_str() : "");
		else
			akick = ci->AddAkick(u->nick, mask.c_str(), !reason.empty() ? reason.c_str() : "");

		FOREACH_MOD(I_OnAkickAdd, OnAkickAdd(u, ci, akick));

		notice_lang(Config.s_ChanServ, u, CHAN_AKICK_ADDED, mask.c_str(), ci->name.c_str());

		this->DoEnforce(u, ci, params);
	}

	void DoStick(User *u, ChannelInfo *ci, const std::vector<ci::string> &params)
	{
		NickAlias *na;
		NickCore *nc;
		ci::string mask = params[2];
		unsigned i;
		AutoKick *akick;

		if (!ci->GetAkickCount())
		{
		        notice_lang(Config.s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, ci->name.c_str());
		        return;
		}

		na = findnick(mask.c_str());
		nc = (na ? na->nc : NULL);

		for (i = 0; i < ci->GetAkickCount(); ++i)
		{
			akick = ci->GetAkick(i);

		        if (akick->HasFlag(AK_ISNICK))
		                continue;
			if (akick->mask == mask)
		                break;
		}

		if (i == ci->GetAkickCount())
		{
		        notice_lang(Config.s_ChanServ, u, CHAN_AKICK_NOT_FOUND, mask.c_str(), ci->name.c_str());
		        return;
		}

		akick->SetFlag(AK_STUCK);
		notice_lang(Config.s_ChanServ, u, CHAN_AKICK_STUCK, akick->mask.c_str(), ci->name.c_str());

		if (ci->c)
		        stick_mask(ci, akick);
	}

	void DoUnStick(User *u, ChannelInfo *ci, const std::vector<ci::string> &params)
	{
		NickAlias *na;
		NickCore *nc;
		AutoKick *akick;
		unsigned i;
		ci::string mask = params[2];

		if (!ci->GetAkickCount())
		{
		        notice_lang(Config.s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, ci->name.c_str());
		        return;
		}

		na = findnick(mask.c_str());
		nc = (na ? na->nc : NULL);

		for (i = 0; i < ci->GetAkickCount(); ++i)
		{
			akick = ci->GetAkick(i);

		        if (akick->HasFlag(AK_ISNICK))
		                continue;
			if (akick->mask == mask)
		                break;
		}

		if (i == ci->GetAkickCount())
		{
		        notice_lang(Config.s_ChanServ, u, CHAN_AKICK_NOT_FOUND, mask.c_str(), ci->name.c_str());
		        return;
		}

		akick->UnsetFlag(AK_STUCK);
		notice_lang(Config.s_ChanServ, u, CHAN_AKICK_UNSTUCK, akick->mask.c_str(), ci->name.c_str());
	}

	void DoDel(User *u, ChannelInfo *ci, const std::vector<ci::string> &params)
	{
		ci::string mask = params[2];
		AutoKick *akick;
		unsigned i;

		if (!ci->GetAkickCount())
		{
		        notice_lang(Config.s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, ci->name.c_str());
		        return;
		}

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (isdigit(*mask.c_str()) && strspn(mask.c_str(), "1234567890,-") == strlen(mask.c_str()))
			(new AkickDelCallback(u, ci, mask.c_str()))->Process();
		else
		{
		        NickAlias *na = findnick(mask.c_str());
		        NickCore *nc = (na ? na->nc : NULL);

			for (i = 0; i < ci->GetAkickCount(); ++i)
			{
				akick = ci->GetAkick(i);

				if (((akick->HasFlag(AK_ISNICK)) && akick->nc == nc)
					|| (!(akick->HasFlag(AK_ISNICK))
					&& akick->mask == mask))
					break;
			}

			if (i == ci->GetAkickCount())
			{
				notice_lang(Config.s_ChanServ, u, CHAN_AKICK_NOT_FOUND, mask.c_str(), ci->name.c_str());
				return;
			}

			ci->EraseAkick(i);

			notice_lang(Config.s_ChanServ, u, CHAN_AKICK_DELETED, mask.c_str(), ci->name.c_str());
		}
	}

	void DoList(User *u, ChannelInfo *ci, const std::vector<ci::string> &params)
	{
		ci::string mask = params.size() > 2 ? params[2] : "";

		if (!ci->GetAkickCount())
		{
		        notice_lang(Config.s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, ci->name.c_str());
		        return;
		}

		if (!mask.empty() && isdigit(*mask.c_str()) && strspn(mask.c_str(), "1234567890,-") == strlen(mask.c_str()))
			(new AkickListCallback(u, ci, mask.c_str()))->Process();
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0; i < ci->GetAkickCount(); ++i)
			{
				AutoKick *akick = ci->GetAkick(i);

		                if (!mask.empty())
				{
					if (!(akick->HasFlag(AK_ISNICK)) && !Anope::Match(akick->mask.c_str(), mask.c_str(), false))
					        continue;
					if ((akick->HasFlag(AK_ISNICK)) && !Anope::Match(akick->nc->display, mask.c_str(), false))
			        		continue;
	                	}
				
				if (!SentHeader)
				{
					SentHeader = true;
					notice_lang(Config.s_ChanServ, u, CHAN_AKICK_LIST_HEADER, ci->name.c_str());
				}

				AkickListCallback::DoList(u, ci, i, akick);
 		       }
		       
		       if (!SentHeader)
		       		notice_lang(Config.s_ChanServ, u, CHAN_AKICK_NO_MATCH, ci->name.c_str());
		}
	}

	void DoView(User *u, ChannelInfo *ci, const std::vector<ci::string> &params)
	{
		ci::string mask = params.size() > 2 ? params[2] : "";

		if (!ci->GetAkickCount())
		{
		        notice_lang(Config.s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, ci->name.c_str());
		        return;
		}

		if (!mask.empty() && isdigit(*mask.c_str()) && strspn(mask.c_str(), "1234567890,-") == strlen(mask.c_str()))
			(new AkickViewCallback(u, ci, mask.c_str()))->Process();
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0; i < ci->GetAkickCount(); ++i)
			{
				AutoKick *akick = ci->GetAkick(i);

		                if (!mask.empty())
				{
					if (!(akick->HasFlag(AK_ISNICK)) && !Anope::Match(akick->mask.c_str(), mask.c_str(), false))
					        continue;
					if ((akick->HasFlag(AK_ISNICK)) && !Anope::Match(akick->nc->display, mask.c_str(), false))
					        continue;
		                }

				if (!SentHeader)
				{
					SentHeader = true;
					notice_lang(Config.s_ChanServ, u, CHAN_AKICK_LIST_HEADER, ci->name.c_str());
				}

				AkickViewCallback::DoList(u, ci, i, akick);
		        }

			if (!SentHeader)
				notice_lang(Config.s_ChanServ, u, CHAN_AKICK_NO_MATCH, ci->name.c_str());
		}
	}

	void DoEnforce(User *u, ChannelInfo *ci, const std::vector<ci::string> &params)
	{
		Channel *c = ci->c;
		int count = 0;

		if (!c)
		{
		        notice_lang(Config.s_ChanServ, u, CHAN_X_NOT_IN_USE, ci->name.c_str());
		        return;
		}

		for (CUserList::iterator it = c->users.begin(); it != c->users.end();)
		{
			UserContainer *uc = *it++;

			if (ci->CheckKick(uc->user))
			{
				count++;
		        }
		}

		notice_lang(Config.s_ChanServ, u, CHAN_AKICK_ENFORCE_DONE, ci->name.c_str(), count);
	}

	void DoClear(User *u, ChannelInfo *ci, const std::vector<ci::string> &params)
	{
		ci->ClearAkick();
		notice_lang(Config.s_ChanServ, u, CHAN_AKICK_CLEAR, ci->name.c_str());
	}

 public:
	CommandCSAKick() : Command("AKICK", 2, 4)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ci::string chan = params[0];
		ci::string cmd = params[1];
		ci::string mask = params.size() > 2 ? params[2] : "";

		ChannelInfo *ci = cs_findchan(chan.c_str());

		if (mask.empty() && (cmd == "ADD" || cmd == "STICK" || cmd == "UNSTICK" || cmd == "DEL"))
			this->OnSyntaxError(u, cmd);
		else if (!check_access(u, ci, CA_AKICK) && !u->Account()->HasPriv("chanserv/access/modify"))
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
		else if (cmd != "LIST" && cmd != "VIEW" && cmd != "ENFORCE" && readonly)
			notice_lang(Config.s_ChanServ, u, CHAN_AKICK_DISABLED);
		else if (cmd == "ADD")
			this->DoAdd(u, ci, params);
		else if (cmd == "STICK")
			this->DoStick(u, ci, params);
		else if (cmd == "UNSTICK")
			this->DoUnStick(u, ci, params);
		else if (cmd == "DEL")
			this->DoDel(u, ci, params);
		else if (cmd == "LIST")
			this->DoList(u, ci, params);
		else if (cmd == "VIEW")
			this->DoView(u, ci, params);
		else if (cmd == "ENFORCE")
			this->DoEnforce(u, ci, params);
		else if (cmd == "CLEAR")
			this->DoClear(u, ci, params);
		else
			this->OnSyntaxError(u, "");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_AKICK);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "AKICK", CHAN_AKICK_SYNTAX);
	}
};




class CSAKick : public Module
{
 public:
	CSAKick(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);
		this->AddCommand(ChanServ, new CommandCSAKick());

		ModuleManager::Attach(I_OnChanServHelp, this);
	}
	void OnChanServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_AKICK);
	}
};

MODULE_INIT(CSAKick)

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
			source.Reply(_("No matching entries on %s autokick list."), source.ci->name.c_str());
	}

	virtual void HandleNumber(unsigned Number)
	{
		if (!Number || Number > source.ci->GetAkickCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(_("Autokick list for %s:"), source.ci->name.c_str());
		}

		DoList(source, Number - 1, source.ci->GetAkick(Number - 1));
	}

	static void DoList(CommandSource &source, unsigned index, AutoKick *akick)
	{
		source.Reply(_("  %3d %s (%s)"), index + 1, akick->HasFlag(AK_ISNICK) ? akick->nc->display.c_str() : akick->mask.c_str(), !akick->reason.empty() ? akick->reason.c_str() : _(NO_REASON));
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
			source.Reply(_("Autokick list for %s:"), source.ci->name.c_str());
		}

		DoList(source, Number - 1, source.ci->GetAkick(Number - 1));
	}

	static void DoList(CommandSource &source, unsigned index, AutoKick *akick)
	{
		Anope::string timebuf;

		if (akick->addtime)
			timebuf = do_strftime(akick->addtime);
		else
			timebuf = _(UNKNOWN);

		source.Reply(_(CHAN_AKICK_VIEW_FORMAT), index + 1, akick->HasFlag(AK_ISNICK) ? akick->nc->display.c_str() : akick->mask.c_str(), !akick->creator.empty() ? akick->creator.c_str() : UNKNOWN, timebuf.c_str(), !akick->reason.empty() ? akick->reason.c_str() : _(NO_REASON));

		if (akick->last_used)
			source.Reply(_("    Last used %s"), do_strftime(akick->last_used).c_str());
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
			source.Reply(_("No matching entries on %s autokick list."), ci->name.c_str());
		else if (Deleted == 1)
			source.Reply(_("Deleted 1 entry from %s autokick list."), ci->name.c_str());
		else
			source.Reply(_("Deleted %d entries from %s autokick list."), Deleted, ci->name.c_str());
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
				source.Reply(_(NICK_X_FORBIDDEN), mask.c_str());
				return;
			}

			nc = na->nc;
		}

		/* Check excepts BEFORE we get this far */
		if (ModeManager::FindChannelModeByName(CMODE_EXCEPT) && is_excepted_mask(ci, mask))
		{
			source.Reply(_(CHAN_EXCEPTED), mask.c_str(), ci->name.c_str());
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
				source.Reply(_(ACCESS_DENIED));
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
				Entry entry_mask(CMODE_BEGIN, mask);

				if ((check_access(u2, ci, CA_FOUNDER) || u2_level >= u_level) && entry_mask.Matches(u2))
				{
					source.Reply(_(ACCESS_DENIED));
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
						source.Reply(_(ACCESS_DENIED));
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
				source.Reply(_("\002%s\002 already exists on %s autokick list."), akick->HasFlag(AK_ISNICK) ? akick->nc->display.c_str() : akick->mask.c_str(), ci->name.c_str());
				return;
			}
		}

		if (ci->GetAkickCount() >= Config->CSAutokickMax)
		{
			source.Reply(_("Sorry, you can only have %d autokick masks on a channel."), Config->CSAutokickMax);
			return;
		}

		if (nc)
			akick = ci->AddAkick(u->nick, nc, reason);
		else
			akick = ci->AddAkick(u->nick, mask, reason);

		bool override = !check_access(u, ci, CA_AKICK);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "ADD " << mask << ": " << reason;

		FOREACH_MOD(I_OnAkickAdd, OnAkickAdd(u, ci, akick));

		source.Reply(_("\002%s\002 added to %s autokick list."), mask.c_str(), ci->name.c_str());

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
			source.Reply(_("%s autokick list is empty."), ci->name.c_str());
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
				source.Reply(_("\002%s\002 not found on %s autokick list."), mask.c_str(), ci->name.c_str());
				return;
			}

			bool override = !check_access(u, ci, CA_AKICK);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "DEL " << mask;

			ci->EraseAkick(i);

			source.Reply(_("\002%s\002 deleted from %s autokick list."), mask.c_str(), ci->name.c_str());
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
			source.Reply(_("%s autokick list is empty."), ci->name.c_str());
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
					source.Reply(_("Autokick list for %s:"), ci->name.c_str());
				}

				AkickListCallback::DoList(source, i, akick);
			}

			if (!SentHeader)
				source.Reply(_("No matching entries on %s autokick list."), ci->name.c_str());
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
			source.Reply(_("%s autokick list is empty."), ci->name.c_str());
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
					source.Reply(_("Autokick list for %s:"), ci->name.c_str());
				}

				AkickViewCallback::DoList(source, i, akick);
			}

			if (!SentHeader)
				source.Reply(_("No matching entries on %s autokick list."), ci->name.c_str());
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
			source.Reply(_(CHAN_X_NOT_IN_USE), ci->name.c_str());
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

		source.Reply(_("AKICK ENFORCE for \002%s\002 complete; \002%d\002 users were affected."), ci->name.c_str(), count);
	}

	void DoClear(CommandSource &source)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;
		bool override = !check_access(u, ci, CA_AKICK);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "CLEAR";

		ci->ClearAkick();
		source.Reply(_("Channel %s akick list has been cleared."), ci->name.c_str());
	}

 public:
	CommandCSAKick() : Command("AKICK", 2, 4)
	{
		this->SetDesc("Maintain the AutoKick list");
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
			source.Reply(_(ACCESS_DENIED));
		else if (!cmd.equals_ci("LIST") && !cmd.equals_ci("VIEW") && !cmd.equals_ci("ENFORCE") && readonly)
			source.Reply(_("Sorry, channel autokick list modification is temporarily disabled."));
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
		source.Reply(_("Syntax: \002AKICK \037channel\037 ADD {\037nick\037 | \037mask\037} [\037reason\037]\002\n"
				"        \002AKICK \037channel\037 DEL {\037nick\037 | \037mask\037 | \037entry-num\037 | \037list\037}\002\n"
				"        \002AKICK \037channel\037 LIST [\037mask\037 | \037entry-num\037 | \037list\037]\002\n"
				"        \002AKICK \037channel\037 VIEW [\037mask\037 | \037entry-num\037 | \037list\037]\002\n"
				"        \002AKICK \037channel\037 ENFORCE\002\n"
				"        \002AKICK \037channel\037 CLEAR\002\n"
				" \n"
				"Maintains the \002AutoKick list\002 for a channel.  If a user\n"
				"on the AutoKick list attempts to join the channel,\n"
				"%sS will ban that user from the channel, then kick\n"
				"the user.\n"
				" \n"
				"The \002AKICK ADD\002 command adds the given nick or usermask\n"
				"to the AutoKick list.  If a \037reason\037 is given with\n"
				"the command, that reason will be used when the user is\n"
				"kicked; if not, the default reason is \"You have been\n"
				"banned from the channel\".\n"
				"When akicking a \037registered nick\037 the nickserv account\n"
				"will be added to the akick list instead of the mask.\n"
				"All users within that nickgroup will then be akicked.\n"
				" \n"
				"The \002AKICK DEL\002 command removes the given nick or mask\n"
				"from the AutoKick list.  It does not, however, remove any\n"
				"bans placed by an AutoKick; those must be removed\n"
				"manually.\n"
				" \n"
				"The \002AKICK LIST\002 command displays the AutoKick list, or\n"
				"optionally only those AutoKick entries which match the\n"
				"given mask.\n"
				" \n"
				"The \002AKICK VIEW\002 command is a more verbose version of\n"
				"\002AKICK LIST\002 command.\n"
				" \n"
				"The \002AKICK ENFORCE\002 command causes %sS to enforce the\n"
				"current AKICK list by removing those users who match an\n"
				"AKICK mask.\n"
				" \n"
				"The \002AKICK CLEAR\002 command clears all entries of the\n"
				"akick list."), ChanServ->nick.c_str(), ChanServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "AKICK", _("AKICK \037channel\037 {ADD | DEL | LIST | VIEW | ENFORCE | CLEAR} [\037nick-or-usermask\037] [\037reason\037]"));
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

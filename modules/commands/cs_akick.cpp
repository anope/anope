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
	ChannelInfo *ci;
	bool SentHeader;
 public:
	AkickListCallback(CommandSource &_source, ChannelInfo *_ci, const Anope::string &numlist) : NumberList(numlist, false), source(_source), ci(_ci), SentHeader(false)
	{
	}

	~AkickListCallback()
	{
		if (!SentHeader)
			source.Reply(_("No matching entries on %s autokick list."), ci->name.c_str());
	}

	virtual void HandleNumber(unsigned Number)
	{
		if (!Number || Number > ci->GetAkickCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(_("Autokick list for %s:"), ci->name.c_str());
		}

		DoList(source, ci, Number - 1, ci->GetAkick(Number - 1));
	}

	static void DoList(CommandSource &source, ChannelInfo *ci, unsigned index, AutoKick *akick)
	{
		source.Reply(_("  %3d %s (%s)"), index + 1, akick->HasFlag(AK_ISNICK) ? akick->nc->display.c_str() : akick->mask.c_str(), !akick->reason.empty() ? akick->reason.c_str() : NO_REASON);
	}
};

class AkickViewCallback : public AkickListCallback
{
 public:
	AkickViewCallback(CommandSource &_source, ChannelInfo *_ci, const Anope::string &numlist) : AkickListCallback(_source, _ci, numlist)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > ci->GetAkickCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(_("Autokick list for %s:"), ci->name.c_str());
		}

		DoList(source, ci, Number - 1, ci->GetAkick(Number - 1));
	}

	static void DoList(CommandSource &source, ChannelInfo *ci, unsigned index, AutoKick *akick)
	{
		Anope::string timebuf;

		if (akick->addtime)
			timebuf = do_strftime(akick->addtime);
		else
			timebuf = UNKNOWN;

		source.Reply(CHAN_AKICK_VIEW_FORMAT, index + 1, akick->HasFlag(AK_ISNICK) ? akick->nc->display.c_str() : akick->mask.c_str(), !akick->creator.empty() ? akick->creator.c_str() : UNKNOWN, timebuf.c_str(), !akick->reason.empty() ? akick->reason.c_str() : _(NO_REASON));

		if (akick->last_used)
			source.Reply(_("    Last used %s"), do_strftime(akick->last_used).c_str());
	}
};

class AkickDelCallback : public NumberList
{
	CommandSource &source;
	ChannelInfo *ci;
	Command *c;
	unsigned Deleted;
 public:
	AkickDelCallback(CommandSource &_source, ChannelInfo *_ci, Command *_c, const Anope::string &list) : NumberList(list, true), source(_source), ci(_ci), c(_c), Deleted(0)
	{
	}

	~AkickDelCallback()
	{
		User *u = source.u;
		bool override = !ci->AccessFor(u).HasPriv("AKICK");
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
		if (!Number || Number > ci->GetAkickCount())
			return;

		++Deleted;
		ci->EraseAkick(Number - 1);
	}
};

class CommandCSAKick : public Command
{
	void DoAdd(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

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
			nc = na->nc;

		/* Check excepts BEFORE we get this far */
		if (ci->c)
		{
			std::pair<Channel::ModeList::iterator, Channel::ModeList::iterator> modes = ci->c->GetModeList(CMODE_EXCEPT);
			for (; modes.first != modes.second; ++modes.first)
			{
				if (Anope::Match(modes.first->second, mask))
				{
					source.Reply(CHAN_EXCEPTED, mask.c_str(), ci->name.c_str());
					return;
				}
			}
		}

		/* Check whether target nick has equal/higher access
		* or whether the mask matches a user with higher/equal access - Viper */
		if (ci->HasFlag(CI_PEACE) && nc)
		{
			AccessGroup nc_access = ci->AccessFor(nc), u_access = ci->AccessFor(u);
			if (nc == ci->GetFounder() || nc_access >= u_access)
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
		}
		else if (ci->HasFlag(CI_PEACE))
		{
			/* Match against all currently online users with equal or
			 * higher access. - Viper */
			for (Anope::insensitive_map<User *>::iterator it = UserListByNick.begin(), it_end = UserListByNick.end(); it != it_end; ++it)
			{
				User *u2 = it->second;

				AccessGroup nc_access = ci->AccessFor(nc), u_access = ci->AccessFor(u);
				Entry entry_mask(CMODE_BEGIN, mask);

				if ((ci->AccessFor(u2).HasPriv("FOUNDER") || nc_access >= u_access) && entry_mask.Matches(u2))
				{
					source.Reply(ACCESS_DENIED);
					return;
				}
			}

			/* Match against the lastusermask of all nickalias's with equal
			 * or higher access. - Viper */
			for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; ++it)
			{
				na = it->second;

				AccessGroup nc_access = ci->AccessFor(na->nc), u_access = ci->AccessFor(u);
				if (na->nc && (na->nc == ci->GetFounder() || nc_access >= u_access))
				{
					Anope::string buf = na->nick + "!" + na->last_usermask;
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

		bool override = !ci->AccessFor(u).HasPriv("AKICK");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "ADD " << mask << ": " << reason;

		FOREACH_MOD(I_OnAkickAdd, OnAkickAdd(u, ci, akick));

		source.Reply(_("\002%s\002 added to %s autokick list."), mask.c_str(), ci->name.c_str());

		this->DoEnforce(source, ci);
	}

	void DoDel(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

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
			AkickDelCallback list(source, ci, this, mask);
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

			bool override = !ci->AccessFor(u).HasPriv("AKICK");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "DEL " << mask;

			ci->EraseAkick(i);

			source.Reply(_("\002%s\002 deleted from %s autokick list."), mask.c_str(), ci->name.c_str());
		}
	}

	void DoList(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &mask = params.size() > 2 ? params[2] : "";

		bool override = !ci->AccessFor(u).HasPriv("AKICK");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "LIST";

		if (!ci->GetAkickCount())
		{
			source.Reply(_("%s autokick list is empty."), ci->name.c_str());
			return;
		}

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AkickListCallback list(source, ci, mask);
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

				AkickListCallback::DoList(source, ci, i, akick);
			}

			if (!SentHeader)
				source.Reply(_("No matching entries on %s autokick list."), ci->name.c_str());
		}
	}

	void DoView(CommandSource &source, ChannelInfo *ci, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &mask = params.size() > 2 ? params[2] : "";

		bool override = !ci->AccessFor(u).HasPriv("AKICK");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "VIEW";

		if (!ci->GetAkickCount())
		{
			source.Reply(_("%s autokick list is empty."), ci->name.c_str());
			return;
		}

		if (!mask.empty() && isdigit(mask[0]) && mask.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			AkickViewCallback list(source, ci, mask);
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

				AkickViewCallback::DoList(source, ci, i, akick);
			}

			if (!SentHeader)
				source.Reply(_("No matching entries on %s autokick list."), ci->name.c_str());
		}
	}

	void DoEnforce(CommandSource &source, ChannelInfo *ci)
	{
		User *u = source.u;
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

		bool override = !ci->AccessFor(u).HasPriv("AKICK");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "ENFORCE, affects " << count << " users";

		source.Reply(_("AKICK ENFORCE for \002%s\002 complete; \002%d\002 users were affected."), ci->name.c_str(), count);
	}

	void DoClear(CommandSource &source, ChannelInfo *ci)
	{
		User *u = source.u;
		bool override = !ci->AccessFor(u).HasPriv("AKICK");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "CLEAR";

		ci->ClearAkick();
		source.Reply(_("Channel %s akick list has been cleared."), ci->name.c_str());
	}

 public:
	CommandCSAKick(Module *creator) : Command(creator, "chanserv/akick", 2, 4)
	{
		this->SetDesc(_("Maintain the AutoKick list"));
		this->SetSyntax(_("\037channel\037 ADD {\037nick\037 | \037mask\037} [\037reason\037]"));
		this->SetSyntax(_("\037channel\037 DEL {\037nick\037 | \037mask\037 | \037entry-num\037 | \037list\037}"));
		this->SetSyntax(_("\037channel\037 LIST [\037mask\037 | \037entry-num\037 | \037list\037]"));
		this->SetSyntax(_("\037channel\037 VIEW [\037mask\037 | \037entry-num\037 | \037list\037]"));
		this->SetSyntax(_("\037channel\037 ENFORCE"));
		this->SetSyntax(_("\037channel\037 CLEAR"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Anope::string cmd = params[1];
		Anope::string mask = params.size() > 2 ? params[2] : "";

		User *u = source.u;

		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (mask.empty() && (cmd.equals_ci("ADD") || cmd.equals_ci("DEL")))
			this->OnSyntaxError(source, cmd);
		else if (!ci->AccessFor(u).HasPriv("AKICK") && !u->HasPriv("chanserv/access/modify"))
			source.Reply(ACCESS_DENIED);
		else if (!cmd.equals_ci("LIST") && !cmd.equals_ci("VIEW") && !cmd.equals_ci("ENFORCE") && readonly)
			source.Reply(_("Sorry, channel autokick list modification is temporarily disabled."));
		else if (cmd.equals_ci("ADD"))
			this->DoAdd(source, ci, params);
		else if (cmd.equals_ci("DEL"))
			this->DoDel(source, ci, params);
		else if (cmd.equals_ci("LIST"))
			this->DoList(source, ci, params);
		else if (cmd.equals_ci("VIEW"))
			this->DoView(source, ci, params);
		else if (cmd.equals_ci("ENFORCE"))
			this->DoEnforce(source, ci);
		else if (cmd.equals_ci("CLEAR"))
			this->DoClear(source, ci);
		else
			this->OnSyntaxError(source, "");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Maintains the \002AutoKick list\002 for a channel.  If a user\n"
				"on the AutoKick list attempts to join the channel,\n"
				"%s will ban that user from the channel, then kick\n"
				"the user.\n"
				" \n"
				"The \002AKICK ADD\002 command adds the given nick or usermask\n"
				"to the AutoKick list.  If a \037reason\037 is given with\n"
				"the command, that reason will be used when the user is\n"
				"kicked; if not, the default reason is \"You have been\n"
				"banned from the channel\".\n"
				"When akicking a \037registered nick\037 the nickserv account\n"
				"will be added to the akick list instead of the mask.\n"
				"All users within that nickgroup will then be akicked.\n"),
				source.owner->nick.c_str());
		source.Reply(_(
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
				"The \002AKICK ENFORCE\002 command causes %s to enforce the\n"
				"current AKICK list by removing those users who match an\n"
				"AKICK mask.\n"
				" \n"
				"The \002AKICK CLEAR\002 command clears all entries of the\n"
				"akick list."), source.owner->nick.c_str());
		return true;
	}
};

class CSAKick : public Module
{
	CommandCSAKick commandcsakick;

 public:
	CSAKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcsakick(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSAKick)

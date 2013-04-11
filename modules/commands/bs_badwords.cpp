/* BotServ core functions
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"


class BadwordsDelCallback : public NumberList
{
	CommandSource &source;
	ChannelInfo *ci;
	Command *c;
	unsigned deleted;
	bool override;
 public:
	BadwordsDelCallback(CommandSource &_source, ChannelInfo *_ci, Command *_c, const Anope::string &list) : NumberList(list, true), source(_source), ci(_ci), c(_c), deleted(0), override(false)
	{
		if (!source.AccessFor(ci).HasPriv("BADWORDS") && source.HasPriv("botserv/administration"))
			this->override = true;
	}

	~BadwordsDelCallback()
	{
		if (!deleted)
			source.Reply(_("No matching entries on %s bad words list."), ci->name.c_str());
		else if (deleted == 1)
			source.Reply(_("Deleted 1 entry from %s bad words list."), ci->name.c_str());
		else
			source.Reply(_("Deleted %d entries from %s bad words list."), deleted, ci->name.c_str());
	}

	void HandleNumber(unsigned Number) anope_override
	{
		if (!Number || Number > ci->GetBadWordCount())
			return;

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, c, ci) << "DEL " << ci->GetBadWord(Number - 1)->word;
		++deleted;
		ci->EraseBadWord(Number - 1);
	}
};

class CommandBSBadwords : public Command
{
 private:
	void DoList(CommandSource &source, ChannelInfo *ci, const Anope::string &word)
	{
		bool override = !source.AccessFor(ci).HasPriv("BADWORDS");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "LIST";
		ListFormatter list;

		list.AddColumn("Number").AddColumn("Word").AddColumn("Type");

		if (!ci->GetBadWordCount())
		{
			source.Reply(_("%s bad words list is empty."), ci->name.c_str());
			return;
		}
		else if (!word.empty() && word.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			class BadwordsListCallback : public NumberList
			{
				ListFormatter &list;
				ChannelInfo *ci;
			 public:
				BadwordsListCallback(ListFormatter &_list, ChannelInfo *_ci, const Anope::string &numlist) : NumberList(numlist, false), list(_list), ci(_ci)
				{
				}

				void HandleNumber(unsigned Number) anope_override
				{
					if (!Number || Number > ci->GetBadWordCount())
						return;

					const BadWord *bw = ci->GetBadWord(Number - 1);
					ListFormatter::ListEntry entry;
					entry["Number"] = stringify(Number);
					entry["Word"] = bw->word;
					entry["Type"] = bw->type == BW_SINGLE ? "(SINGLE)" : (bw->type == BW_START ? "(START)" : (bw->type == BW_END ? "(END)" : ""));
					this->list.AddEntry(entry);
				}
			}
			nl_list(list, ci, word);
			nl_list.Process();
		}
		else
		{
			for (unsigned i = 0, end = ci->GetBadWordCount(); i < end; ++i)
			{
				const BadWord *bw = ci->GetBadWord(i);

				if (!word.empty() && !Anope::Match(bw->word, word))
					continue;

				ListFormatter::ListEntry entry;
				entry["Number"] = stringify(i + 1);
				entry["Word"] = bw->word;
				entry["Type"] = bw->type == BW_SINGLE ? "(SINGLE)" : (bw->type == BW_START ? "(START)" : (bw->type == BW_END ? "(END)" : ""));
				list.AddEntry(entry);
			}
		}

		if (list.IsEmpty())
			source.Reply(_("No matching entries on %s bad words list."), ci->name.c_str());
		else
		{
			std::vector<Anope::string> replies;
			list.Process(replies);

			source.Reply(_("Bad words list for %s:"), ci->name.c_str());

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);

			source.Reply(_("End of bad words list."));
		}
	}

	void DoAdd(CommandSource &source, ChannelInfo *ci, const Anope::string &word)
	{
		size_t pos = word.rfind(' ');
		BadWordType bwtype = BW_ANY;
		Anope::string realword = word;

		if (pos != Anope::string::npos)
		{
			Anope::string opt = word.substr(pos + 1);
			if (!opt.empty())
			{
				if (opt.equals_ci("SINGLE"))
					bwtype = BW_SINGLE;
				else if (opt.equals_ci("START"))
					bwtype = BW_START;
				else if (opt.equals_ci("END"))
					bwtype = BW_END;
			}
			realword = word.substr(0, pos);
		}

		if (ci->GetBadWordCount() >= Config->BSBadWordsMax)
		{
			source.Reply(_("Sorry, you can only have %d bad words entries on a channel."), Config->BSBadWordsMax);
			return;
		}

		for (unsigned i = 0, end = ci->GetBadWordCount(); i < end; ++i)
		{
			const BadWord *bw = ci->GetBadWord(i);

			if (!bw->word.empty() && ((Config->BSCaseSensitive && realword.equals_cs(bw->word)) || (!Config->BSCaseSensitive && realword.equals_ci(bw->word))))
			{
				source.Reply(_("\002%s\002 already exists in %s bad words list."), bw->word.c_str(), ci->name.c_str());
				return;
			}
		}

		bool override = !source.AccessFor(ci).HasPriv("BADWORDS");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "ADD " << realword;
		ci->AddBadWord(realword, bwtype);

		source.Reply(_("\002%s\002 added to %s bad words list."), realword.c_str(), ci->name.c_str());

		return;
	}

	void DoDelete(CommandSource &source, ChannelInfo *ci, const Anope::string &word)
	{
		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (!word.empty() && isdigit(word[0]) && word.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			BadwordsDelCallback list(source, ci, this, word);
			list.Process();
		}
		else
		{
			unsigned i, end;
			const BadWord *badword;

			for (i = 0, end = ci->GetBadWordCount(); i < end; ++i)
			{
				badword = ci->GetBadWord(i);

				if (word.equals_ci(badword->word))
					break;
			}

			if (i == end)
			{
				source.Reply(_("\002%s\002 not found on %s bad words list."), word.c_str(), ci->name.c_str());
				return;
			}

			bool override = !source.AccessFor(ci).HasPriv("BADWORDS");
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "DEL " << badword->word;

			source.Reply(_("\002%s\002 deleted from %s bad words list."), badword->word.c_str(), ci->name.c_str());

			ci->EraseBadWord(i);
		}

		return;
	}

	void DoClear(CommandSource &source, ChannelInfo *ci)
	{
		bool override = !source.AccessFor(ci).HasPriv("BADWORDS");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "CLEAR";

		ci->ClearBadWords();
		source.Reply(_("Bad words list is now empty."));
		return;
	}
 public:
	CommandBSBadwords(Module *creator) : Command(creator, "botserv/badwords", 2, 3)
	{
		this->SetDesc(_("Maintains the bad words list"));
		this->SetSyntax(_("\037channel\037 ADD \037word\037 [\037SINGLE\037 | \037START\037 | \037END\037]"));
		this->SetSyntax(_("\037channel\037 DEL {\037word\037 | \037entry-num\037 | \037list\037}"));
		this->SetSyntax(_("\037channel\037 LIST [\037mask\037 | \037list\037]"));
		this->SetSyntax(_("\037channel\037 CLEAR"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &cmd = params[1];
		const Anope::string &word = params.size() > 2 ? params[2] : "";
		bool need_args = cmd.equals_ci("LIST") || cmd.equals_ci("CLEAR");

		if (!need_args && word.empty())
		{
			this->OnSyntaxError(source, cmd);
			return;
		}

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!source.AccessFor(ci).HasPriv("BADWORDS") && (!need_args || !source.HasPriv("botserv/administration")))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (Anope::ReadOnly)
		{
			source.Reply(_("Sorry, channel bad words list modification is temporarily disabled."));
			return;
		}

		if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, ci, word);
		else if (cmd.equals_ci("DEL"))
			return this->DoDelete(source, ci, word);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, ci, word);
		else if (cmd.equals_ci("CLEAR"))
			return this->DoClear(source, ci);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Maintains the \002bad words list\002 for a channel. The bad\n"
				"words list determines which words are to be kicked\n"
				"when the bad words kicker is enabled. For more information,\n"
				"type \002%s%s HELP KICK %s\002.\n"
				" \n"
				"The \002ADD\002 command adds the given word to the\n"
				"bad words list. If SINGLE is specified, a kick will be\n"
				"done only if a user says the entire word. If START is\n"
				"specified, a kick will be done if a user says a word\n"
				"that starts with \037word\037. If END is specified, a kick\n"
				"will be done if a user says a word that ends with\n"
				"\037word\037. If you don't specify anything, a kick will\n"
				"be issued every time \037word\037 is said by a user.\n"
				" \n"), Config->UseStrictPrivMsgString.c_str(), source.service->nick.c_str(), source.command.c_str());
		source.Reply(_("The \002DEL\002 command removes the given word from the\n"
				"bad words list.  If a list of entry numbers is given, those\n"
				"entries are deleted.  (See the example for LIST below.)\n"
				" \n"
				"The \002LIST\002 command displays the bad words list.  If\n"
				"a wildcard mask is given, only those entries matching the\n"
				"mask are displayed.  If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002#channel LIST 2-5,7-9\002\n"
				"      Lists bad words entries numbered 2 through 5 and\n"
				"      7 through 9.\n"
				" \n"
				"The \002CLEAR\002 command clears all entries of the\n"
				"bad words list."));
		return true;
	}
};

class BSBadwords : public Module
{
	CommandBSBadwords commandbsbadwords;

 public:
	BSBadwords(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandbsbadwords(this)
	{

	}
};

MODULE_INIT(BSBadwords)

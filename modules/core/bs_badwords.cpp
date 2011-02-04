/* BotServ core functions
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

class BadwordsListCallback : public NumberList
{
	CommandSource &source;
	bool SentHeader;
 public:
	BadwordsListCallback(CommandSource &_source, const Anope::string &list) : NumberList(list, false), source(_source), SentHeader(false)
	{
	}

	~BadwordsListCallback()
	{
		if (!SentHeader)
			source.Reply(_("No matching entries on %s bad words list."), source.ci->name.c_str());
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > source.ci->GetBadWordCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(_("Bad words list for %s:\n"
				"  Num   Word                           Type"), source.ci->name.c_str());
		}

		DoList(source, Number - 1, source.ci->GetBadWord(Number - 1));
	}

	static void DoList(CommandSource &source, unsigned Number, BadWord *bw)
	{
		source.Reply(_("  %3d   %-30s %s"), Number + 1, bw->word.c_str(), bw->type == BW_SINGLE ? "(SINGLE)" : (bw->type == BW_START ? "(START)" : (bw->type == BW_END ? "(END)" : "")));
	}
};

class BadwordsDelCallback : public NumberList
{
	CommandSource &source;
	Command *c;
	unsigned Deleted;
	bool override;
 public:
	BadwordsDelCallback(CommandSource &_source, Command *_c, const Anope::string &list) : NumberList(list, true), source(_source), c(_c), Deleted(0), override(false)
	{
		if (!check_access(source.u, source.ci, CA_BADWORDS) && source.u->Account()->HasPriv("botserv/administration"))
			this->override = true;
	}

	~BadwordsDelCallback()
	{
		if (!Deleted)
			source.Reply(_("No matching entries on %s bad words list."), source.ci->name.c_str());
		else if (Deleted == 1)
			source.Reply(_("Deleted 1 entry from %s bad words list."), source.ci->name.c_str());
		else
			source.Reply(_("Deleted %d entries from %s bad words list."), Deleted, source.ci->name.c_str());
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > source.ci->GetBadWordCount())
			return;

		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source.u, c, source.ci) << "DEL " << source.ci->GetBadWord(Number - 1)->word;
		++Deleted;
		source.ci->EraseBadWord(Number - 1);
	}
};

class CommandBSBadwords : public Command
{
 private:
	CommandReturn DoList(CommandSource &source, const Anope::string &word)
	{
		ChannelInfo *ci = source.ci;
		bool override = !check_access(source.u, ci, CA_BADWORDS);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source.u, this, ci) << "LIST";

		if (!ci->GetBadWordCount())
			source.Reply(_("%s bad words list is empty."), ci->name.c_str());
		else if (!word.empty() && word.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			BadwordsListCallback list(source, word);
			list.Process();
		}
		else
		{
			bool SentHeader = false;

			for (unsigned i = 0, end = ci->GetBadWordCount(); i < end; ++i)
			{
				BadWord *bw = ci->GetBadWord(i);

				if (!word.empty() && !Anope::Match(bw->word, word))
					continue;

				if (!SentHeader)
				{
					SentHeader = true;
					source.Reply(_("Bad words list for %s:\n"
						"  Num   Word                           Type"), ci->name.c_str());

				}

				BadwordsListCallback::DoList(source, i, bw);
			}

			if (!SentHeader)
				source.Reply(_("No matching entries on %s bad words list."), ci->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoAdd(CommandSource &source, const Anope::string &word)
	{
		ChannelInfo *ci = source.ci;
		size_t pos = word.rfind(' ');
		BadWordType type = BW_ANY;
		Anope::string realword = word;

		if (pos != Anope::string::npos)
		{
			Anope::string opt = word.substr(pos + 1);
			if (!opt.empty())
			{
				if (opt.equals_ci("SINGLE"))
					type = BW_SINGLE;
				else if (opt.equals_ci("START"))
					type = BW_START;
				else if (opt.equals_ci("END"))
					type = BW_END;
			}
			realword = word.substr(0, pos);
		}

		if (ci->GetBadWordCount() >= Config->BSBadWordsMax)
		{
			source.Reply(_("Sorry, you can only have %d bad words entries on a channel."), Config->BSBadWordsMax);
			return MOD_CONT;
		}

		for (unsigned i = 0, end = ci->GetBadWordCount(); i < end; ++i)
		{
			BadWord *bw = ci->GetBadWord(i);

			if (!bw->word.empty() && ((Config->BSCaseSensitive && realword.equals_cs(bw->word)) || (!Config->BSCaseSensitive && realword.equals_ci(bw->word))))
			{
				source.Reply(_("\002%s\002 already exists in %s bad words list."), bw->word.c_str(), ci->name.c_str());
				return MOD_CONT;
			}
		}

		bool override = !check_access(source.u, ci, CA_BADWORDS);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source.u, this, ci) << "ADD " << realword;
		ci->AddBadWord(realword, type);

		source.Reply(_("\002%s\002 added to %s bad words list."), realword.c_str(), ci->name.c_str());

		return MOD_CONT;
	}

	CommandReturn DoDelete(CommandSource &source, const Anope::string &word)
	{
		ChannelInfo *ci = source.ci;
		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (!word.empty() && isdigit(word[0]) && word.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			BadwordsDelCallback list(source, this, word);
			list.Process();
		}
		else
		{
			unsigned i, end;
			BadWord *badword;

			for (i = 0, end = ci->GetBadWordCount(); i < end; ++i)
			{
				badword = ci->GetBadWord(i);

				if (word.equals_ci(badword->word))
					break;
			}

			if (i == end)
			{
				source.Reply(_("\002%s\002 not found on %s bad words list."), word.c_str(), ci->name.c_str());
				return MOD_CONT;
			}

			bool override = !check_access(source.u, ci, CA_BADWORDS);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source.u, this, ci) << "DEL " << badword->word;
			ci->EraseBadWord(i);

			source.Reply(_("\002%s\002 deleted from %s bad words list."), badword->word.c_str(), ci->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(CommandSource &source)
	{
		ChannelInfo *ci = source.ci;
		bool override = !check_access(source.u, ci, CA_BADWORDS);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source.u, this, ci) << "CLEAR";

		ci->ClearBadWords();
		source.Reply(_("Bad words list is now empty."));
		return MOD_CONT;
	}
 public:
	CommandBSBadwords() : Command("BADWORDS", 2, 3)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &cmd = params[1];
		const Anope::string &word = params.size() > 2 ? params[2] : "";
		User *u = source.u;
		ChannelInfo *ci = source.ci;
		bool need_args = cmd.equals_ci("LIST") || cmd.equals_ci("CLEAR");

		if (!need_args && word.empty())
		{
			this->OnSyntaxError(source, cmd);
			return MOD_CONT;
		}

		if (!check_access(u, ci, CA_BADWORDS) && (!need_args || !u->Account()->HasPriv("botserv/administration")))
		{
			source.Reply(LanguageString::ACCESS_DENIED);
			return MOD_CONT;
		}

		if (readonly)
		{
			source.Reply(_("Sorry, channel bad words list modification is temporarily disabled."));
			return MOD_CONT;
		}

		if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, word);
		else if (cmd.equals_ci("DEL"))
			return this->DoDelete(source, word);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, word);
		else if (cmd.equals_ci("CLEAR"))
			return this->DoClear(source);
		else
			this->OnSyntaxError(source, "");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002BADWORDS \037channel\037 ADD \037word\037 [\037SINGLE\037 | \037START\037 | \037END\037]\002\n"
				"        \002BADWORDS \037channel\037 DEL {\037word\037 | \037entry-num\037 | \037list\037}\002\n"
				"        \002BADWORDS \037channel\037 LIST [\037mask\037 | \037list\037]\002\n"
				"        \002BADWORDS \037channel\037 CLEAR\002\n"
				" \n"
				"Maintains the \002bad words list\002 for a channel. The bad\n"
				"words list determines which words are to be kicked\n"
				"when the bad words kicker is enabled. For more information,\n"
				"type \002%R%S HELP KICK BADWORDS\002.\n"
				" \n"
				"The \002BADWORDS ADD\002 command adds the given word to the\n"
				"badword list. If SINGLE is specified, a kick will be\n"
				"done only if a user says the entire word. If START is \n"
				"specified, a kick will be done if a user says a word\n"
				"that starts with \037word\037. If END is specified, a kick\n"
				"will be done if a user says a word that ends with\n"
				"\037word\037. If you don't specify anything, a kick will\n"
				"be issued every time \037word\037 is said by a user.\n"
				" \n"
				"The \002BADWORDS DEL\002 command removes the given word from the\n"
				"bad words list.  If a list of entry numbers is given, those\n"
				"entries are deleted.  (See the example for LIST below.)\n"
				" \n"
				"The \002BADWORDS LIST\002 command displays the bad words list.  If\n"
				"a wildcard mask is given, only those entries matching the\n"
				"mask are displayed.  If a list of entry numbers is given,\n"
				"only those entries are shown; for example:\n"
				"   \002BADWORDS #channel LIST 2-5,7-9\002\n"
				"      Lists bad words entries numbered 2 through 5 and\n"
				"      7 through 9.\n"
				" \n"
				"The \002BADWORDS CLEAR\002 command clears all entries of the\n"
				"bad words list."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "BADWORDS", _("BADWORDS \037channel\037 {ADD|DEL|LIST|CLEAR} [\037word\037 | \037entry-list\037] [SINGLE|START|END]"));
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    BADWORDS       Maintains bad words list"));
	}
};

class BSBadwords : public Module
{
	CommandBSBadwords commandbsbadwords;

 public:
	BSBadwords(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(BotServ, &commandbsbadwords);
	}
};

MODULE_INIT(BSBadwords)

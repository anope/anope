/* BotServ core functions
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
			source.Reply(BOT_BADWORDS_NO_MATCH, source.ci->name.c_str());
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > source.ci->GetBadWordCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			source.Reply(BOT_BADWORDS_LIST_HEADER, source.ci->name.c_str());
		}

		DoList(source, Number - 1, source.ci->GetBadWord(Number - 1));
	}

	static void DoList(CommandSource &source, unsigned Number, BadWord *bw)
	{
		source.Reply(BOT_BADWORDS_LIST_FORMAT, Number + 1, bw->word.c_str(), bw->type == BW_SINGLE ? "(SINGLE)" : (bw->type == BW_START ? "(START)" : (bw->type == BW_END ? "(END)" : "")));
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
			source.Reply(BOT_BADWORDS_NO_MATCH, source.ci->name.c_str());
		else if (Deleted == 1)
			source.Reply(BOT_BADWORDS_DELETED_ONE, source.ci->name.c_str());
		else
			source.Reply(BOT_BADWORDS_DELETED_SEVERAL, Deleted, source.ci->name.c_str());
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
			source.Reply(BOT_BADWORDS_LIST_EMPTY, ci->name.c_str());
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
					source.Reply(BOT_BADWORDS_LIST_HEADER, ci->name.c_str());
				}

				BadwordsListCallback::DoList(source, i, bw);
			}

			if (!SentHeader)
				source.Reply(BOT_BADWORDS_NO_MATCH, ci->name.c_str());
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
			source.Reply(BOT_BADWORDS_REACHED_LIMIT, Config->BSBadWordsMax);
			return MOD_CONT;
		}

		for (unsigned i = 0, end = ci->GetBadWordCount(); i < end; ++i)
		{
			BadWord *bw = ci->GetBadWord(i);

			if (!bw->word.empty() && ((Config->BSCaseSensitive && realword.equals_cs(bw->word)) || (!Config->BSCaseSensitive && realword.equals_ci(bw->word))))
			{
				source.Reply(BOT_BADWORDS_ALREADY_EXISTS, bw->word.c_str(), ci->name.c_str());
				return MOD_CONT;
			}
		}

		bool override = !check_access(source.u, ci, CA_BADWORDS);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source.u, this, ci) << "ADD " << realword;
		ci->AddBadWord(realword, type);

		source.Reply(BOT_BADWORDS_ADDED, realword.c_str(), ci->name.c_str());

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
				source.Reply(BOT_BADWORDS_NOT_FOUND, word.c_str(), ci->name.c_str());
				return MOD_CONT;
			}

			bool override = !check_access(source.u, ci, CA_BADWORDS);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, source.u, this, ci) << "DEL " << badword->word;
			ci->EraseBadWord(i);

			source.Reply(BOT_BADWORDS_DELETED, badword->word.c_str(), ci->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(CommandSource &source)
	{
		ChannelInfo *ci = source.ci;
		bool override = !check_access(source.u, ci, CA_BADWORDS);
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source.u, this, ci) << "CLEAR";

		ci->ClearBadWords();
		source.Reply(BOT_BADWORDS_CLEAR);
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
			source.Reply(ACCESS_DENIED);
			return MOD_CONT;
		}

		if (readonly)
		{
			source.Reply(BOT_BADWORDS_DISABLED);
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
		source.Reply(BOT_HELP_BADWORDS);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "BADWORDS", BOT_BADWORDS_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(BOT_HELP_CMD_BADWORDS);
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

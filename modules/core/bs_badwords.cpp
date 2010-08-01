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
	User *u;
	ChannelInfo *ci;
	bool SentHeader;
 public:
	BadwordsListCallback(User *_u, ChannelInfo *_ci, const Anope::string &list) : NumberList(list, false), u(_u), ci(_ci), SentHeader(false)
	{
	}

	~BadwordsListCallback()
	{
		if (!SentHeader)
			notice_lang(Config.s_BotServ, u, BOT_BADWORDS_NO_MATCH, ci->name.c_str());
	}

	void HandleNumber(unsigned Number)
	{
		if (Number > ci->GetBadWordCount())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			notice_lang(Config.s_BotServ, u, BOT_BADWORDS_LIST_HEADER, ci->name.c_str());
		}

		DoList(u, ci, Number - 1, ci->GetBadWord(Number - 1));
	}

	static void DoList(User *u, ChannelInfo *ci, unsigned Number, BadWord *bw)
	{
		notice_lang(Config.s_BotServ, u, BOT_BADWORDS_LIST_FORMAT, Number + 1, bw->word.c_str(), bw->type == BW_SINGLE ? "(SINGLE)" : (bw->type == BW_START ? "(START)" : (bw->type == BW_END ? "(END)" : "")));
	}
};

class BadwordsDelCallback : public NumberList
{
	User *u;
	ChannelInfo *ci;
	unsigned Deleted;
 public:
	BadwordsDelCallback(User *_u, ChannelInfo *_ci, const Anope::string &list) : NumberList(list, true), u(_u), ci(_ci), Deleted(0)
	{
	}

	~BadwordsDelCallback()
	{
		if (!Deleted)
			notice_lang(Config.s_BotServ, u, BOT_BADWORDS_NO_MATCH, ci->name.c_str());
		else if (Deleted == 1)
			notice_lang(Config.s_BotServ, u, BOT_BADWORDS_DELETED_ONE, ci->name.c_str());
		else
			notice_lang(Config.s_BotServ, u, BOT_BADWORDS_DELETED_SEVERAL, Deleted, ci->name.c_str());
	}

	void HandleNumber(unsigned Number)
	{
		if (Number > ci->GetBadWordCount())
			return;

		++Deleted;
		ci->EraseBadWord(Number - 1);
	}
};

class CommandBSBadwords : public Command
{
 private:
	CommandReturn DoList(User *u, ChannelInfo *ci, const Anope::string &word)
	{
		if (!ci->GetBadWordCount())
			notice_lang(Config.s_BotServ, u, BOT_BADWORDS_LIST_EMPTY, ci->name.c_str());
		else if (!word.empty() && word.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			BadwordsListCallback list(u, ci, word);
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
					notice_lang(Config.s_BotServ, u, BOT_BADWORDS_LIST_HEADER, ci->name.c_str());
				}

				BadwordsListCallback::DoList(u, ci, i, bw);
			}

			if (!SentHeader)
				notice_lang(Config.s_BotServ, u, BOT_BADWORDS_NO_MATCH, ci->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoAdd(User *u, ChannelInfo *ci, const Anope::string &word)
	{
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

		if (ci->GetBadWordCount() >= Config.BSBadWordsMax)
		{
			notice_lang(Config.s_BotServ, u, BOT_BADWORDS_REACHED_LIMIT, Config.BSBadWordsMax);
			return MOD_CONT;
		}

		for (unsigned i = 0, end = ci->GetBadWordCount(); i < end; ++i)
		{
			BadWord *bw = ci->GetBadWord(i);

			if (!bw->word.empty() && ((Config.BSCaseSensitive && realword.equals_cs(bw->word)) || (!Config.BSCaseSensitive && realword.equals_ci(bw->word))))
			{
				notice_lang(Config.s_BotServ, u, BOT_BADWORDS_ALREADY_EXISTS, bw->word.c_str(), ci->name.c_str());
				return MOD_CONT;
			}
		}

		ci->AddBadWord(realword, type);

		notice_lang(Config.s_BotServ, u, BOT_BADWORDS_ADDED, realword.c_str(), ci->name.c_str());

		return MOD_CONT;
	}

	CommandReturn DoDelete(User *u, ChannelInfo *ci, const Anope::string &word)
	{
		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (!word.empty() && isdigit(word[0]) && word.find_first_not_of("1234567890,-") == Anope::string::npos)
		{
			BadwordsDelCallback list(u, ci, word);
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
				notice_lang(Config.s_BotServ, u, BOT_BADWORDS_NOT_FOUND, word.c_str(), ci->name.c_str());
				return MOD_CONT;
			}

			ci->EraseBadWord(i);

			notice_lang(Config.s_BotServ, u, BOT_BADWORDS_DELETED, badword->word.c_str(), ci->name.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u, ChannelInfo *ci)
	{
		ci->ClearBadWords();
		notice_lang(Config.s_BotServ, u, BOT_BADWORDS_CLEAR);
		return MOD_CONT;
	}
 public:
	CommandBSBadwords() : Command("BADWORDS", 2, 3)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Anope::string cmd = params[1];
		Anope::string word = params.size() > 2 ? params[2] : "";
		ChannelInfo *ci;
		bool need_args = cmd.equals_ci("LIST") || cmd.equals_ci("CLEAR");

		if (!need_args && word.empty())
		{
			this->OnSyntaxError(u, cmd);
			return MOD_CONT;
		}

		ci = cs_findchan(chan);

		if (!check_access(u, ci, CA_BADWORDS) && (!need_args || !u->Account()->HasPriv("botserv/administration")))
		{
			notice_lang(Config.s_BotServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (readonly)
		{
			notice_lang(Config.s_BotServ, u, BOT_BADWORDS_DISABLED);
			return MOD_CONT;
		}

		if (cmd.equals_ci("ADD"))
			return this->DoAdd(u, ci, word);
		else if (cmd.equals_ci("DEL"))
			return this->DoDelete(u, ci, word);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(u, ci, word);
		else if (cmd.equals_ci("CLEAR"))
			return this->DoClear(u, ci);
		else
			this->OnSyntaxError(u, "");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_BotServ, u, BOT_HELP_BADWORDS);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config.s_BotServ, u, "BADWORDS", BOT_BADWORDS_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_BotServ, u, BOT_HELP_CMD_BADWORDS);
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

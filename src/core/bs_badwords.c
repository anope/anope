/* BotServ core functions
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

int badwords_del_callback(User * u, int num, va_list args);
int badwords_list(User * u, int index, ChannelInfo * ci, int *sent_header);
int badwords_list_callback(User * u, int num, va_list args);

class CommandBSBadwords : public Command
{
 private:
	CommandReturn DoList(User *u, ChannelInfo *ci, const ci::string &word)
	{
		int sent_header = 0;

		if (!ci->GetBadWordCount())
		{
			notice_lang(Config.s_BotServ, u, BOT_BADWORDS_LIST_EMPTY, ci->name.c_str());
			return MOD_CONT;
		}
		if (!word.empty() && strspn(word.c_str(), "1234567890,-") == word.length())
		{
			process_numlist(word.c_str(), NULL, badwords_list_callback, u, ci, &sent_header);
		}
		else
		{
			for (unsigned i = 0; i < ci->GetBadWordCount(); i++)
			{
				BadWord *badword = ci->GetBadWord(i);

				if (!word.empty() && !Anope::Match(badword->word, word.c_str(), false))
					continue;

				badwords_list(u, i, ci, &sent_header);
			}
		}
		if (!sent_header)
			notice_lang(Config.s_BotServ, u, BOT_BADWORDS_NO_MATCH, ci->name.c_str());

		return MOD_CONT;
	}

	CommandReturn DoAdd(User *u, ChannelInfo *ci, const ci::string &word)
	{
		size_t pos = word.find_last_of(" ");
		BadWordType type = BW_ANY;
		ci::string realword = word;

		if (pos != ci::string::npos)
		{
			ci::string opt = ci::string(word, pos + 1);
			if (!opt.empty())
			{
				if (opt == "SINGLE")
					type = BW_SINGLE;
				else if (opt == "START")
					type = BW_START;
				else if (opt == "END")
					type = BW_END;
			}
			realword = ci::string(word, 0, pos);
		}

		if (ci->GetBadWordCount() >= Config.BSBadWordsMax)
		{
			notice_lang(Config.s_BotServ, u, BOT_BADWORDS_REACHED_LIMIT, Config.BSBadWordsMax);
			return MOD_CONT;
		}

		for (unsigned i = 0; i < ci->GetBadWordCount(); ++i)
		{
			BadWord *bw = ci->GetBadWord(i);

			if (!bw->word.empty() && (Config.BSCaseSensitive && !stricmp(bw->word.c_str(), realword.c_str())
				|| (!Config.BSCaseSensitive && bw->word == realword.c_str())))
			{
				notice_lang(Config.s_BotServ, u, BOT_BADWORDS_ALREADY_EXISTS, bw->word.c_str(), ci->name.c_str());
				return MOD_CONT;
			}
		}

		ci->AddBadWord(realword.c_str(), type);

		notice_lang(Config.s_BotServ, u, BOT_BADWORDS_ADDED, realword.c_str(), ci->name.c_str());

		return MOD_CONT;
	}

	CommandReturn DoDelete(User *u, ChannelInfo *ci, const ci::string &word)
	{
		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (!word.empty() && isdigit(word[0]) && strspn(word.c_str(), "1234567890,-") == word.length())
		{
			int count, deleted, last = -1;
			deleted = process_numlist(word.c_str(), &count, badwords_del_callback, u, ci, &last);
			
			if (!deleted)
			{
				if (count == 1)
				{
					notice_lang(Config.s_BotServ, u, BOT_BADWORDS_NO_SUCH_ENTRY, last, ci->name.c_str());
				}
				else
				{
					notice_lang(Config.s_BotServ, u, BOT_BADWORDS_NO_MATCH, ci->name.c_str());
				}
			}
			else if (deleted == 1)
			{
				notice_lang(Config.s_BotServ, u, BOT_BADWORDS_DELETED_ONE, ci->name.c_str());
			}
			else
			{
				notice_lang(Config.s_BotServ, u, BOT_BADWORDS_DELETED_SEVERAL, deleted, ci->name.c_str());
			}

			if (deleted)
				ci->CleanBadWords();
		}
		else
		{
			unsigned i;
			BadWord *badword;

			for (i = 0; i < ci->GetBadWordCount(); ++i)
			{
				badword = ci->GetBadWord(i);

				if (badword->word == word)
					break;
			}

			if (i == ci->GetBadWordCount())
			{
				notice_lang(Config.s_BotServ, u, BOT_BADWORDS_NOT_FOUND, word.c_str(), ci->name.c_str());
				return MOD_CONT;
			}
			
			ci->EraseBadWord(badword);

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

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		ci::string cmd = params[1];
		ci::string word = params.size() > 2 ? params[2].c_str() : "";
		ChannelInfo *ci;
		bool need_args = cmd == "LIST" || cmd == "CLEAR";

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

		if (cmd == "ADD")
			return this->DoAdd(u, ci, word);
		else if (cmd == "DEL")
			return this->DoDelete(u, ci, word);
		else if (cmd == "LIST")
			return this->DoList(u, ci, word);
		else if (cmd == "CLEAR")
			return this->DoClear(u, ci);
		else
			this->OnSyntaxError(u, "");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_BotServ, u, BOT_HELP_BADWORDS);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_BotServ, u, "BADWORDS", BOT_BADWORDS_SYNTAX);
	}
};

class BSBadwords : public Module
{
 public:
	BSBadwords(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);
		this->AddCommand(BotServ, new CommandBSBadwords);

		ModuleManager::Attach(I_OnBotServHelp, this);
	}
	void OnBotServHelp(User *u)
	{
		notice_lang(Config.s_BotServ, u, BOT_HELP_CMD_BADWORDS);
	}
};

int badwords_del_callback(User * u, int num, va_list args)
{
	ChannelInfo *ci = va_arg(args, ChannelInfo *);
	int *last = va_arg(args, int *);

	*last = num;

	if (num < 1 || num > ci->GetBadWordCount())
		return 0;

	ci->GetBadWord(num - 1)->InUse = false;

	return 1;
}

int badwords_list(User * u, int index, ChannelInfo * ci, int *sent_header)
{
	BadWord *bw = ci->GetBadWord(index);

	if (!*sent_header)
	{
		notice_lang(Config.s_BotServ, u, BOT_BADWORDS_LIST_HEADER, ci->name.c_str());
		*sent_header = 1;
	}

	notice_lang(Config.s_BotServ, u, BOT_BADWORDS_LIST_FORMAT, index + 1, bw->word.c_str(),
		((bw->type == BW_SINGLE) ? "(SINGLE)" : ((bw->type == BW_START) ? "(START)"
		: ((bw->type == BW_END) ? "(END)" : "")))
	);

	return 1;
}

int badwords_list_callback(User * u, int num, va_list args)
{
	ChannelInfo *ci = va_arg(args, ChannelInfo *);
	int *sent_header = va_arg(args, int *);

	if (num < 1 || num > ci->GetBadWordCount())
		return 0;

	return badwords_list(u, num - 1, ci, sent_header);
}

MODULE_INIT(BSBadwords)

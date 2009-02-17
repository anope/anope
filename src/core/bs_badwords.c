/* BotServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

void myBotServHelp(User * u);
int badwords_del_callback(User * u, int num, va_list args);
int badwords_list(User * u, int index, ChannelInfo * ci, int *sent_header);
int badwords_list_callback(User * u, int num, va_list args);

class CommandBSBadwords : public Command
{
 private:
	CommandReturn DoList(User *u, ChannelInfo *ci, const char *word)
	{
		int sent_header = 0;
		int i = 0;

		if (ci->bwcount == 0)
		{
			notice_lang(s_BotServ, u, BOT_BADWORDS_LIST_EMPTY, ci->name);
			return MOD_CONT;
		}
		if (word && strspn(word, "1234567890,-") == strlen(word))
		{
			process_numlist(word, NULL, badwords_list_callback, u, ci, &sent_header);
		}
		else
		{
			for (i = 0; i < ci->bwcount; i++)
			{
				if (!(ci->badwords[i].in_use))
					continue;
				if (word && ci->badwords[i].word
					&& !match_wild_nocase(word, ci->badwords[i].word))
					continue;
				badwords_list(u, i, ci, &sent_header);
			}
		}
		if (!sent_header)
			notice_lang(s_BotServ, u, BOT_BADWORDS_NO_MATCH, ci->name);

		return MOD_CONT;
	}

	CommandReturn DoAdd(User *u, ChannelInfo *ci, const char *word)
	{
		char *opt, *pos;
		int type = BW_ANY;
		unsigned i = 0;
		BadWord *bw;

		if (readonly)
		{
			notice_lang(s_BotServ, u, BOT_BADWORDS_DISABLED);
			return MOD_CONT;
		}

		pos = strrchr(const_cast<char *>(word), ' '); // XXX - Potentially unsafe cast
		if (pos)
		{
			opt = pos + 1;
			if (*opt)
			{
				if (!stricmp(opt, "SINGLE"))
					type = BW_SINGLE;
				else if (!stricmp(opt, "START"))
					type = BW_START;
				else if (!stricmp(opt, "END"))
					type = BW_END;
				if (type != BW_ANY)
					*pos = 0;
			}
		}

		for (bw = ci->badwords, i = 0; i < ci->bwcount; bw++, i++)
		{
			if (bw->word && ((BSCaseSensitive && (!strcmp(bw->word, word)))
							 || (!BSCaseSensitive
								 && (!stricmp(bw->word, word)))))
			{
				notice_lang(s_BotServ, u, BOT_BADWORDS_ALREADY_EXISTS,
							bw->word, ci->name);
				return MOD_CONT;
			}
		}

		for (i = 0; i < ci->bwcount; i++)
		{
			if (!ci->badwords[i].in_use)
				break;
		}
		if (i == ci->bwcount)
		{
			if (i < BSBadWordsMax)
			{
				ci->bwcount++;
				ci->badwords =
					static_cast<BadWord *>(srealloc(ci->badwords, sizeof(BadWord) * ci->bwcount));
			}
			else
			{
				notice_lang(s_BotServ, u, BOT_BADWORDS_REACHED_LIMIT,
							BSBadWordsMax);
				return MOD_CONT;
			}
		}
		bw = &ci->badwords[i];
		bw->in_use = 1;
		bw->word = sstrdup(word);
		bw->type = type;

		notice_lang(s_BotServ, u, BOT_BADWORDS_ADDED, bw->word, ci->name);
		return MOD_CONT;
	}

	CommandReturn DoDelete(User *u, ChannelInfo *ci, const char *word)
	{
		int deleted = 0, a, b;
		int i;
		BadWord *bw;

		/* Special case: is it a number/list?  Only do search if it isn't. */
		if (isdigit(*word) && strspn(word, "1234567890,-") == strlen(word))
		{
			int count, last = -1;
			deleted = process_numlist(word, &count, badwords_del_callback, u, ci,	&last);
			if (!deleted)
			{
				if (count == 1)
				{
					notice_lang(s_BotServ, u, BOT_BADWORDS_NO_SUCH_ENTRY,	last, ci->name);
				}
				else
				{
					notice_lang(s_BotServ, u, BOT_BADWORDS_NO_MATCH, ci->name);
				}
			}
			else if (deleted == 1)
			{
				notice_lang(s_BotServ, u, BOT_BADWORDS_DELETED_ONE,	ci->name);
			}
			else
			{
				notice_lang(s_BotServ, u, BOT_BADWORDS_DELETED_SEVERAL, deleted, ci->name);
			}
		}
		else
		{

			for (i = 0; i < ci->bwcount; i++)
			{
				if (!ci->badwords[i].in_use)
					break;
			}

			if (i == ci->bwcount)
			{
				notice_lang(s_BotServ, u, BOT_BADWORDS_NOT_FOUND, word, ci->name);
				return MOD_CONT;
			}

			bw = &ci->badwords[i];
			notice_lang(s_BotServ, u, BOT_BADWORDS_DELETED, bw->word, ci->name);
			if (bw->word)
				delete [] bw->word;
			bw->word = NULL;
			bw->in_use = 0;
			deleted = 1;
		}

		if (deleted) {
			/* Reordering - DrStein */
			for (b = 0; b < ci->bwcount; b++) {
				if (ci->badwords[b].in_use) {
					for (a = 0; a < ci->bwcount; a++) {
						if (a > b)
							break;
						if (!(ci->badwords[a].in_use)) {
							ci->badwords[a].in_use = ci->badwords[b].in_use;
							ci->badwords[a].type = ci->badwords[b].type;
							if (ci->badwords[b].word) {
								ci->badwords[a].word = sstrdup(ci->badwords[b].word);
								delete [] ci->badwords[b].word;
							}
							ci->badwords[b].word = NULL;
							ci->badwords[b].in_use = 0;
							break;
						}
					}
				}
			}
			/* After reordering only the entries at the end could still be empty.
			 * We ll free the places no longer in use... - Viper */
			for (i = ci->bwcount - 1; i >= 0; i--) {
				if (ci->badwords[i].in_use)
					break;
				ci->bwcount--;
			}
			ci->badwords =
				static_cast<BadWord *>(srealloc(ci->badwords,sizeof(BadWord) * ci->bwcount));
		}

		return MOD_CONT;
	}

	CommandReturn DoClear(User *u, ChannelInfo *ci, const char *word)
	{
		int i;
		for (i = 0; i < ci->bwcount; i++)
			if (ci->badwords[i].word)
				delete [] ci->badwords[i].word;

		free(ci->badwords);
		ci->badwords = NULL;
		ci->bwcount = 0;

		notice_lang(s_BotServ, u, BOT_BADWORDS_CLEAR);
		return MOD_CONT;
	}
 public:
	CommandBSBadwords() : Command("BADWORDS", 2, 3)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *cmd = params[1].c_str();
		const char *word = params.size() > 2 ? params[2].c_str() : NULL;
		ChannelInfo *ci;
		int need_args = (cmd && (!stricmp(cmd, "LIST") || !stricmp(cmd, "CLEAR")));

		if (!cmd || (need_args ? 0 : !word))
		{
			this->OnSyntaxError(u);
			return MOD_CONT;
		}

		if (!(ci = cs_findchan(chan)))
		{
			notice_lang(s_BotServ, u, CHAN_X_NOT_REGISTERED, chan);
			return MOD_CONT;
		}

		if (ci->flags & CI_FORBIDDEN)
		{
			notice_lang(s_BotServ, u, CHAN_X_FORBIDDEN, chan);
			return MOD_CONT;
		}

		if (!check_access(u, ci, CA_BADWORDS) && (!need_args || !u->nc->HasPriv("botserv/administration")))
		{
			notice_lang(s_BotServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}

		if (readonly)
		{
			notice_lang(s_BotServ, u, BOT_BADWORDS_DISABLED);
			return MOD_CONT;
		}

		if (stricmp(cmd, "ADD") == 0)
		{
			return this->DoAdd(u, ci, word);
		}
		else if (stricmp(cmd, "DEL") == 0)
		{
			return this->DoDelete(u, ci, word);
		}
		else if (stricmp(cmd, "LIST") == 0)
		{
			return this->DoList(u, ci, word);
		}
		else if (stricmp(cmd, "CLEAR") == 0)
		{
			return this->DoClear(u, ci, word);
		}
		else
		{
			this->OnSyntaxError(u);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_BotServ, u, BOT_HELP_BADWORDS);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_BotServ, u, "BADWORDS", BOT_BADWORDS_SYNTAX);
	}
};

class BSBadwords : public Module
{
 public:
	BSBadwords(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(BOTSERV, new CommandBSBadwords, MOD_UNIQUE);

		this->SetBotHelp(myBotServHelp);
	}
};


/**
 * Add the help response to Anopes /bs help output.
 * @param u The user who is requesting help
 **/
void myBotServHelp(User * u)
{
	notice_lang(s_BotServ, u, BOT_HELP_CMD_BADWORDS);
}

int badwords_del_callback(User * u, int num, va_list args)
{
	BadWord *bw;
	ChannelInfo *ci = va_arg(args, ChannelInfo *);
	int *last = va_arg(args, int *);

	*last = num;

	if (num < 1 || num > ci->bwcount)
		return 0;

	bw = &ci->badwords[num - 1];
	if (bw->word)
		delete [] bw->word;
	bw->word = NULL;
	bw->in_use = 0;

	return 1;
}

int badwords_list(User * u, int index, ChannelInfo * ci, int *sent_header)
{
	BadWord *bw = &ci->badwords[index];

	if (!bw->in_use)
		return 0;
	if (!*sent_header) {
		notice_lang(s_BotServ, u, BOT_BADWORDS_LIST_HEADER, ci->name);
		*sent_header = 1;
	}

	notice_lang(s_BotServ, u, BOT_BADWORDS_LIST_FORMAT, index + 1,
				bw->word,
				((bw->type ==
				  BW_SINGLE) ? "(SINGLE)" : ((bw->type ==
											  BW_START) ? "(START)"
											 : ((bw->type ==
												 BW_END) ? "(END)" : "")))
		);
	return 1;
}

int badwords_list_callback(User * u, int num, va_list args)
{
	ChannelInfo *ci = va_arg(args, ChannelInfo *);
	int *sent_header = va_arg(args, int *);
	if (num < 1 || num > ci->bwcount)
		return 0;
	return badwords_list(u, num - 1, ci, sent_header);
}

MODULE_INIT("bs_badwords", BSBadwords)

/* MemoServ core functions
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

int del_memo_callback(User *u, int num, va_list args);
void myMemoServHelp(User *u);

class CommandMSDel : public Command
{
 public:
	CommandMSDel() : Command("DEL", 0, 2)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		MemoInfo *mi;
		ChannelInfo *ci;
		const char *numstr = params.size() ? params[0].c_str() : NULL, *chan = NULL;
		int last, last0, i;
		char buf[BUFSIZE], *end;
		int delcount, count, left;

		if (numstr && *numstr == '#')
		{
			chan = numstr;
			numstr = params.size() > 1 ? params[1].c_str() : NULL;
			if (!(ci = cs_findchan(chan)))
			{
				notice_lang(s_MemoServ, u, CHAN_X_NOT_REGISTERED, chan);
				return MOD_CONT;
			}
			else if (readonly)
			{
				notice_lang(s_MemoServ, u, READ_ONLY_MODE);
				return MOD_CONT;
			}
			else if (ci->flags & CI_FORBIDDEN)
			{
				notice_lang(s_MemoServ, u, CHAN_X_FORBIDDEN, chan);
				return MOD_CONT;
			}
			else if (!check_access(u, ci, CA_MEMO))
			{
				notice_lang(s_MemoServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}
			mi = &ci->memos;
		}
		else
		{
			if (!nick_identified(u))
			{
				notice_lang(s_MemoServ, u, NICK_IDENTIFY_REQUIRED, s_NickServ);
				return MOD_CONT;
			}
			mi = &u->na->nc->memos;
		}
		if (!numstr || (!isdigit(*numstr) && stricmp(numstr, "ALL") && stricmp(numstr, "LAST")))
			this->OnSyntaxError(u);
		else if (mi->memos.empty())
		{
			if (chan)
				notice_lang(s_MemoServ, u, MEMO_X_HAS_NO_MEMOS, chan);
			else
				notice_lang(s_MemoServ, u, MEMO_HAVE_NO_MEMOS);
		}
		else
		{
			if (isdigit(*numstr))
			{
				/* Delete a specific memo or memos. */
				last = -1; /* Last memo deleted */
				last0 = -1; /* Beginning of range of last memos deleted */
				end = buf;
				left = sizeof(buf);
				delcount = process_numlist(numstr, &count, del_memo_callback, u, mi, &last, &last0, &end, &left);
				if (last != -1)
				{
					/* Some memos got deleted; tell them which ones. */
					if (delcount > 1)
					{
						if (last0 != last)
							end += snprintf(end, sizeof(buf) - (end - buf), ",%d-%d", last0, last);
						else
							end += snprintf(end, sizeof(buf) - (end - buf), ",%d", last);
						/* "buf+1" here because *buf == ',' */
						notice_lang(s_MemoServ, u, MEMO_DELETED_SEVERAL, buf + 1);
					}
					else
						notice_lang(s_MemoServ, u, MEMO_DELETED_ONE, last);
				}
				else
				{
					/* No memos were deleted.  Tell them so. */
					if (count == 1)
						notice_lang(s_MemoServ, u, MEMO_DOES_NOT_EXIST, atoi(numstr));
					else
						notice_lang(s_MemoServ, u, MEMO_DELETED_NONE);
				}
			}
			else if (!stricmp(numstr, "LAST"))
			{
				/* Delete last memo. */
				for (i = 0; i < mi->memos.size(); ++i)
					last = mi->memos[i]->number;
				delmemo(mi, last);
				notice_lang(s_MemoServ, u, MEMO_DELETED_ONE, last);
			}
			else
			{
				/* Delete all memos. */
				for (i = 0; i < mi->memos.size(); ++i)
				{
					delete [] mi->memos[i]->text;
					delete mi->memos[i];
				}
				mi->memos.clear();
				if (chan)
					notice_lang(s_MemoServ, u, MEMO_CHAN_DELETED_ALL, chan);
				else
					notice_lang(s_MemoServ, u, MEMO_DELETED_ALL);
			}

			/* Reset the order */
			for (i = 0; i < mi->memos.size(); ++i)
				mi->memos[i]->number = i + 1;
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_MemoServ, u, MEMO_HELP_DEL);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_MemoServ, u, "DEL", MEMO_DEL_SYNTAX);
	}
};

class MSDel : public Module
{
 public:
	MSDel(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(MEMOSERV, new CommandMSDel(), MOD_UNIQUE);
		this->SetMemoHelp(myMemoServHelp);
	}
};

/**
 * Add the help response to anopes /ms help output.
 * @param u The user who is requesting help
 **/
void myMemoServHelp(User *u)
{
	notice_lang(s_MemoServ, u, MEMO_HELP_CMD_DEL);
}

/**
 * Delete a single memo from a MemoInfo. callback function
 * @param u User Struct
 * @param int Number
 * @param va_list Variable Arguemtns
 * @return 1 if successful, 0 if it fails
 */
int del_memo_callback(User *u, int num, va_list args)
{
	MemoInfo *mi = va_arg(args, MemoInfo *);
	int *last = va_arg(args, int *);
	int *last0 = va_arg(args, int *);
	char **end = va_arg(args, char **);
	int *left = va_arg(args, int *);

	if (delmemo(mi, num))
	{
		if (num != (*last) + 1)
		{
			if (*last != -1)
			{
				int len;
				if (*last0 != *last)
					len = snprintf(*end, *left, ",%d-%d", *last0, *last);
				else
					len = snprintf(*end, *left, ",%d", *last);
				*end += len;
				*left -= len;
			}
			*last0 = num;
		}
		*last = num;
		return 1;
	}
	else
		return 0;
}

MODULE_INIT("ms_del", MSDel)

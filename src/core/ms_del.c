/* MemoServ core functions
 *
 * (C) 2003-2010 Anope Team
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

class CommandMSDel : public Command
{
 public:
	CommandMSDel() : Command("DEL", 0, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		MemoInfo *mi;
		ChannelInfo *ci;
		ci::string numstr = params.size() ? params[0] : "", chan;
		int last, last0, i;
		char buf[BUFSIZE], *end;
		int delcount, count, left;

		if (!numstr.empty() && numstr[0] == '#')
		{
			chan = numstr;
			numstr = params.size() > 1 ? params[1] : "";

			if (!(ci = cs_findchan(chan.c_str())))
			{
				notice_lang(Config.s_MemoServ, u, CHAN_X_NOT_REGISTERED, chan.c_str());
				return MOD_CONT;
			}
			else if (readonly)
			{
				notice_lang(Config.s_MemoServ, u, READ_ONLY_MODE);
				return MOD_CONT;
			}
			else if (!check_access(u, ci, CA_MEMO))
			{
				notice_lang(Config.s_MemoServ, u, ACCESS_DENIED);
				return MOD_CONT;
			}
			mi = &ci->memos;
		}
		else
		{
			mi = &u->nc->memos;
		}
		if (numstr.empty() || (!isdigit(numstr[0]) && numstr != "ALL" && numstr != "LAST"))
			this->OnSyntaxError(u, numstr);
		else if (mi->memos.empty())
		{
			if (!chan.empty())
				notice_lang(Config.s_MemoServ, u, MEMO_X_HAS_NO_MEMOS, chan.c_str());
			else
				notice_lang(Config.s_MemoServ, u, MEMO_HAVE_NO_MEMOS);
		}
		else
		{
			if (isdigit(numstr[0]))
			{
				/* Delete a specific memo or memos. */
				last = -1; /* Last memo deleted */
				last0 = -1; /* Beginning of range of last memos deleted */
				end = buf;
				left = sizeof(buf);
				delcount = process_numlist(numstr.c_str(), &count, del_memo_callback, u, mi, &last, &last0, &end, &left);
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
						notice_lang(Config.s_MemoServ, u, MEMO_DELETED_SEVERAL, buf + 1);
					}
					else
						notice_lang(Config.s_MemoServ, u, MEMO_DELETED_ONE, last);
				}
				else
				{
					/* No memos were deleted.  Tell them so. */
					if (count == 1)
						notice_lang(Config.s_MemoServ, u, MEMO_DOES_NOT_EXIST, atoi(numstr.c_str()));
					else
						notice_lang(Config.s_MemoServ, u, MEMO_DELETED_NONE);
				}
			}
			else if (numstr == "LAST")
			{
				/* Delete last memo. */
				for (i = 0; i < mi->memos.size(); ++i)
					last = mi->memos[i]->number;
				delmemo(mi, last);
				notice_lang(Config.s_MemoServ, u, MEMO_DELETED_ONE, last);
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
				if (!chan.empty())
					notice_lang(Config.s_MemoServ, u, MEMO_CHAN_DELETED_ALL, chan.c_str());
				else
					notice_lang(Config.s_MemoServ, u, MEMO_DELETED_ALL);
			}

			/* Reset the order */
			for (i = 0; i < mi->memos.size(); ++i)
				mi->memos[i]->number = i + 1;
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_MemoServ, u, MEMO_HELP_DEL);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_MemoServ, u, "DEL", MEMO_DEL_SYNTAX);
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
		this->AddCommand(MEMOSERV, new CommandMSDel());

		ModuleManager::Attach(I_OnMemoServHelp, this);
	}
	void OnMemoServHelp(User *u)
	{
		notice_lang(Config.s_MemoServ, u, MEMO_HELP_CMD_DEL);
	}
};

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

MODULE_INIT(MSDel)

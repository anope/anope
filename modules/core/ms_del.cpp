/* MemoServ core functions
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

class MemoDelCallback : public NumberList
{
	CommandSource &source;
	ChannelInfo *ci;
	MemoInfo *mi;
 public:
	MemoDelCallback(CommandSource &_source, ChannelInfo *_ci, MemoInfo *_mi, const Anope::string &list) : NumberList(list, true), source(_source), ci(_ci), mi(_mi)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > mi->memos.size())
			return;

		if (ci)
			FOREACH_MOD(I_OnMemoDel, OnMemoDel(ci, mi, mi->memos[Number - 1]));
		else
			FOREACH_MOD(I_OnMemoDel, OnMemoDel(source.u->Account(), mi, mi->memos[Number - 1]));

		mi->Del(Number - 1);
		source.Reply(MEMO_DELETED_ONE, Number);
	}
};

class CommandMSDel : public Command
{
 public:
	CommandMSDel() : Command("DEL", 0, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		MemoInfo *mi;
		ChannelInfo *ci = NULL;
		Anope::string numstr = !params.empty() ? params[0] : "", chan;

		if (!numstr.empty() && numstr[0] == '#')
		{
			chan = numstr;
			numstr = params.size() > 1 ? params[1] : "";

			if (!(ci = cs_findchan(chan)))
			{
				source.Reply(CHAN_X_NOT_REGISTERED, chan.c_str());
				return MOD_CONT;
			}
			else if (readonly)
			{
				source.Reply(READ_ONLY_MODE);
				return MOD_CONT;
			}
			else if (!check_access(u, ci, CA_MEMO))
			{
				source.Reply(ACCESS_DENIED);
				return MOD_CONT;
			}
			mi = &ci->memos;
		}
		else
			mi = &u->Account()->memos;
		if (numstr.empty() || (!isdigit(numstr[0]) && !numstr.equals_ci("ALL") && !numstr.equals_ci("LAST")))
			this->OnSyntaxError(u, numstr);
		else if (mi->memos.empty())
		{
			if (!chan.empty())
				source.Reply(MEMO_X_HAS_NO_MEMOS, chan.c_str());
			else
				source.Reply(MEMO_HAVE_NO_MEMOS);
		}
		else
		{
			if (isdigit(numstr[0]))
			{
				MemoDelCallback list(source, ci, mi, numstr);
				list.Process();
			}
			else if (numstr.equals_ci("LAST"))
			{
				/* Delete last memo. */
				if (ci)
					FOREACH_MOD(I_OnMemoDel, OnMemoDel(ci, mi, mi->memos[mi->memos.size() - 1]));
				else
					FOREACH_MOD(I_OnMemoDel, OnMemoDel(u->Account(), mi, mi->memos[mi->memos.size() - 1]));
				mi->Del(mi->memos[mi->memos.size() - 1]);
				source.Reply(MEMO_DELETED_ONE, mi->memos.size() + 1);
			}
			else
			{
				if (ci)
					FOREACH_MOD(I_OnMemoDel, OnMemoDel(ci, mi, NULL));
				else
					FOREACH_MOD(I_OnMemoDel, OnMemoDel(u->Account(), mi, NULL));
				/* Delete all memos. */
				for (unsigned i = 0, end = mi->memos.size(); i < end; ++i)
					delete mi->memos[i];
				mi->memos.clear();
				if (!chan.empty())
					source.Reply(MEMO_CHAN_DELETED_ALL, chan.c_str());
				else
					source.Reply(MEMO_DELETED_ALL);
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(MemoServ, MEMO_HELP_DEL);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(MemoServ, u, "DEL", MEMO_DEL_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(MemoServ, MEMO_HELP_CMD_DEL);
	}
};

class MSDel : public Module
{
	CommandMSDel commandmsdel;

 public:
	MSDel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, &commandmsdel);
	}
};

MODULE_INIT(MSDel)

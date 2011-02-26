/* MemoServ core functions
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
		source.Reply(_("Memo %d has been deleted."), Number);
	}
};

class CommandMSDel : public Command
{
 public:
	CommandMSDel() : Command("DEL", 0, 2)
	{
		this->SetDesc("Delete a memo or memos");
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
				source.Reply(_(CHAN_X_NOT_REGISTERED), chan.c_str());
				return MOD_CONT;
			}
			else if (readonly)
			{
				source.Reply(_(READ_ONLY_MODE));
				return MOD_CONT;
			}
			else if (!check_access(u, ci, CA_MEMO))
			{
				source.Reply(_(ACCESS_DENIED));
				return MOD_CONT;
			}
			mi = &ci->memos;
		}
		else
			mi = &u->Account()->memos;
		if (numstr.empty() || (!isdigit(numstr[0]) && !numstr.equals_ci("ALL") && !numstr.equals_ci("LAST")))
			this->OnSyntaxError(source, numstr);
		else if (mi->memos.empty())
		{
			if (!chan.empty())
				source.Reply(_(MEMO_X_HAS_NO_MEMOS), chan.c_str());
			else
				source.Reply(_(MEMO_HAVE_NO_MEMOS));
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
				source.Reply(_("Memo %d has been deleted."), mi->memos.size() + 1);
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
					source.Reply(_("All memos for channel %s have been deleted."), chan.c_str());
				else
					source.Reply(_("All of your memos have been deleted."));
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002DEL [\037channel\037] {\037num\037 | \037list\037 | LAST | ALL}\002\n"
				" \n"
				"Deletes the specified memo or memos. You can supply\n"
				"multiple memo numbers or ranges of numbers instead of a\n"
				"single number, as in the second example below.\n"
				" \n"
				"If \002LAST\002 is given, the last memo will be deleted.\n"
				"If \002ALL\002 is given, deletes all of your memos.\n"
				" \n"
				"Examples:\n"
				" \n"
				"   \002DEL 1\002\n"
				"      Deletes your first memo.\n"
				" \n"
				"   \002DEL 2-5,7-9\002\n"
				"      Deletes memos numbered 2 through 5 and 7 through 9."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "DEL", _("DEL [\037channel\037] {\037num\037 | \037list\037 | ALL}"));
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

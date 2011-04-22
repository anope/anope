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
#include "memoserv.h"

class MemoListCallback : public NumberList
{
	CommandSource &source;
	ChannelInfo *ci;
	const MemoInfo *mi;
	bool SentHeader;
 public:
	MemoListCallback(CommandSource &_source, ChannelInfo *_ci, const MemoInfo *_mi, const Anope::string &list) : NumberList(list, false), source(_source), ci(_ci), mi(_mi), SentHeader(false)
	{
	}

	void HandleNumber(unsigned Number)
	{
		if (!Number || Number > mi->memos.size())
			return;

		if (!SentHeader)
		{
			SentHeader = true;
			if (ci)
				source.Reply(_("Memos for %s. To read, type: \002%s%s READ %s \037num\037\002"), ci->name.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->s_MemoServ.c_str(), ci->name.c_str());
			else
				source.Reply(_("Memos for %s. To read, type: \002%s%s READ \037num\037\002"), source.u->nick.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->s_MemoServ.c_str());

			source.Reply(_(" Num  Sender            Date/Time"));
		}

		DoList(source, mi, Number - 1);
	}

	static void DoList(CommandSource &source, const MemoInfo *mi, unsigned index)
	{
		Memo *m = mi->memos[index];
		source.Reply(_("%c%3d  %-16s  %s"), (m->HasFlag(MF_UNREAD)) ? '*' : ' ', index + 1, m->sender.c_str(), do_strftime(m->time).c_str());
	}
};

class CommandMSList : public Command
{
 public:
	CommandMSList() : Command("LIST", 0, 2)
	{
		this->SetDesc(_("List your memos"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		Anope::string param = !params.empty() ? params[0] : "", chan;
		ChannelInfo *ci = NULL;
		const MemoInfo *mi;
		int i, end;

		if (!param.empty() && param[0] == '#')
		{
			chan = param;
			param = params.size() > 1 ? params[1] : "";

			if (!(ci = cs_findchan(chan)))
			{
				source.Reply(_(CHAN_X_NOT_REGISTERED), chan.c_str());
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
		if (!param.empty() && !isdigit(param[0]) && !param.equals_ci("NEW"))
			this->OnSyntaxError(source, param);
		else if (!mi->memos.size())
		{
			if (!chan.empty())
				source.Reply(_(MEMO_X_HAS_NO_MEMOS), chan.c_str());
			else
				source.Reply(_(MEMO_HAVE_NO_MEMOS));
		}
		else
		{
			if (!param.empty() && isdigit(param[0]))
			{
				MemoListCallback list(source, ci, mi, param);
				list.Process();
			}
			else
			{
				if (!param.empty())
				{
					for (i = 0, end = mi->memos.size(); i < end; ++i)
						if (mi->memos[i]->HasFlag(MF_UNREAD))
							break;
					if (i == end)
					{
						if (!chan.empty())
							source.Reply(_(MEMO_X_HAS_NO_NEW_MEMOS), chan.c_str());
						else
							source.Reply(_(MEMO_HAVE_NO_NEW_MEMOS));
						return MOD_CONT;
					}
				}

				bool SentHeader = false;

				for (i = 0, end = mi->memos.size(); i < end; ++i)
				{
					if (!param.empty() && !mi->memos[i]->HasFlag(MF_UNREAD))
						continue;

					if (!SentHeader)
					{
						SentHeader = true;
						if (ci)
							source.Reply(!param.empty() ? _("New memos for %s.  To read, type: \002%s%s READ %s \037num\037\002") : _("Memos for %s. To read, type: \002%sR%s READ %s \037num\037\002"), ci->name.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->s_MemoServ.c_str(), ci->name.c_str());
						else
							source.Reply(!param.empty() ? _("New memos for %s.  To read, type: \002%s%s READ \037num\037\002") : _("Memos for %s. To read, type: \002%s%s READ \037num\037\002"), u->nick.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->s_MemoServ.c_str());
						source.Reply(_(" Num  Sender            Date/Time"));
					}

					MemoListCallback::DoList(source, mi, i);
				}
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002LIST [\037channel\037] [\037list\037 | NEW]\002\n"
				" \n"
				"Lists any memos you currently have.  With \002NEW\002, lists only\n"
				"new (unread) memos. Unread memos are marked with a \"*\"\n"
				"to the left of the memo number. You can also specify a list\n"
				"of numbers, as in the example below:\n"
				"   \002LIST 2-5,7-9\002\n"
				"      Lists memos numbered 2 through 5 and 7 through 9."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "LIST", _("LIST [\037channel\037] [\037list\037 | NEW]"));
	}
};

class MSList : public Module
{
	CommandMSList commandmslist;

 public:
	MSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		if (!memoserv)
			throw ModuleException("MemoServ is not loaded!");

		this->AddCommand(memoserv->Bot(), &commandmslist);
	}
};

MODULE_INIT(MSList)

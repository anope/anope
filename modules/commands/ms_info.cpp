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

class CommandMSInfo : public Command
{
 public:
	CommandMSInfo(Module *creator) : Command(creator, "memoserv/info", 0, 1)
	{
		this->SetDesc(_("Displays information about your memos"));
		this->SetSyntax(_("[\037nick\037 | \037channel\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const MemoInfo *mi;
		NickAlias *na = NULL;
		ChannelInfo *ci = NULL;
		const Anope::string &nname = !params.empty() ? params[0] : "";
		int hardmax = 0;

		if (!nname.empty() && nname[0] != '#' && u->HasPriv("memoserv/info"))
		{
			na = findnick(nname);
			if (!na)
			{
				source.Reply(NICK_X_NOT_REGISTERED, nname.c_str());
				return;
			}
			mi = &na->nc->memos;
			hardmax = na->nc->HasFlag(NI_MEMO_HARDMAX) ? 1 : 0;
		}
		else if (!nname.empty() && nname[0] == '#')
		{
			if (!(ci = cs_findchan(nname)))
			{
				source.Reply(CHAN_X_NOT_REGISTERED, nname.c_str());
				return;
			}
			else if (!ci->HasPriv(u, CA_MEMO))
			{
				source.Reply(ACCESS_DENIED);
				return;
			}
			mi = &ci->memos;
			hardmax = ci->HasFlag(CI_MEMO_HARDMAX) ? 1 : 0;
		}
		else if (!nname.empty()) /* It's not a chan and we aren't services admin */
		{
			source.Reply(ACCESS_DENIED);
			return;
		}
		else
		{
			mi = &u->Account()->memos;
			hardmax = u->Account()->HasFlag(NI_MEMO_HARDMAX) ? 1 : 0;
		}

		if (!nname.empty() && (ci || na->nc != u->Account()))
		{
			if (mi->memos.empty())
				source.Reply(_("%s currently has no memos."), nname.c_str());
			else if (mi->memos.size() == 1)
			{
				if (mi->memos[0]->HasFlag(MF_UNREAD))
					source.Reply(_("%s currently has \0021\002 memo, and it has not yet been read."), nname.c_str());
				else
					source.Reply(_("%s currently has \0021\002 memo."), nname.c_str());
			}
			else
			{
				unsigned count = 0, i, end;
				for (i = 0, end = mi->memos.size(); i < end; ++i)
					if (mi->memos[i]->HasFlag(MF_UNREAD))
						++count;
				if (count == mi->memos.size())
					source.Reply(_("%s currently has \002%d\002 memos; all of them are unread."), nname.c_str(), count);
				else if (!count)
					source.Reply(_("%s currently has \002%d\002 memos."), nname.c_str(), mi->memos.size());
				else if (count == 1)
					source.Reply(_("%s currently has \002%d\002 memos, of which \0021\002 is unread."), nname.c_str(), mi->memos.size());
				else
					source.Reply(_("%s currently has \002%d\002 memos, of which \002%d\002 are unread."), nname.c_str(), mi->memos.size(), count);
			}
			if (!mi->memomax)
			{
				if (hardmax)
					source.Reply(_("%s's memo limit is \002%d\002, and may not be changed."), nname.c_str(), mi->memomax);
				else
					source.Reply(_("%s's memo limit is \002%d\002."), nname.c_str(), mi->memomax);
			}
			else if (mi->memomax > 0)
			{
				if (hardmax)
					source.Reply(_("%s's memo limit is \002%d\002, and may not be changed."), nname.c_str(), mi->memomax);
				else
					source.Reply(_("%s's memo limit is \002%d\002."), nname.c_str(), mi->memomax);
			}
			else
				source.Reply(_("%s has no memo limit."), nname.c_str());

			/* I ripped this code out of ircservices 4.4.5, since I didn't want
			   to rewrite the whole thing (it pisses me off). */
			if (na)
			{
				if (na->nc->HasFlag(NI_MEMO_RECEIVE) && na->nc->HasFlag(NI_MEMO_SIGNON))
					source.Reply(_("%s is notified of new memos at logon and when they arrive."), nname.c_str());
				else if (na->nc->HasFlag(NI_MEMO_RECEIVE))
					source.Reply(_("%s is notified when new memos arrive."), nname.c_str());
				else if (na->nc->HasFlag(NI_MEMO_SIGNON))
					source.Reply(_("%s is notified of news memos at logon."), nname.c_str());
				else
					source.Reply(_("%s is not notified of new memos."), nname.c_str());
			}
		}
		else /* !nname || (!ci || na->nc == u->Account()) */
		{
			if (mi->memos.empty())
				source.Reply(_("You currently have no memos."));
			else if (mi->memos.size() == 1)
			{
				if (mi->memos[0]->HasFlag(MF_UNREAD))
					source.Reply(_("You currently have \0021\002 memo, and it has not yet been read."));
				else
					source.Reply(_("You currently have \0021\002 memo."));
			}
			else
			{
				unsigned count = 0, i, end;
				for (i = 0, end = mi->memos.size(); i < end; ++i)
					if (mi->memos[i]->HasFlag(MF_UNREAD))
						++count;
				if (count == mi->memos.size())
					source.Reply(_("You currently have \002%d\002 memos; all of them are unread."), count);
				else if (!count)
					source.Reply(_("You currently have \002%d\002 memos."), mi->memos.size());
				else if (count == 1)
					source.Reply(_("You currently have \002%d\002 memos, of which \0021\002 is unread."), mi->memos.size());
				else
					source.Reply(_("You currently have \002%d\002 memos, of which \002%d\002 are unread."), mi->memos.size(), count);
			}

			if (!mi->memomax)
			{
				if (!u->IsServicesOper() && hardmax)
					source.Reply(_("Your memo limit is \0020\002; you will not receive any new memos.  You cannot change this limit."));
				else
					source.Reply(_("Your memo limit is \0020\002; you will not receive any new memos."));
			}
			else if (mi->memomax > 0)
			{
				if (!u->IsServicesOper() && hardmax)
					source.Reply(_("Your memo limit is \002%d\002, and may not be changed."), mi->memomax);
				else
					source.Reply(_("Your memo limit is \002%d\002."), mi->memomax);
			}
			else
				source.Reply(_("You have no limit on the number of memos you may keep."));

			/* Ripped too. But differently because of a seg fault (loughs) */
			if (u->Account()->HasFlag(NI_MEMO_RECEIVE) && u->Account()->HasFlag(NI_MEMO_SIGNON))
				source.Reply(_("You will be notified of new memos at logon and when they arrive."));
			else if (u->Account()->HasFlag(NI_MEMO_RECEIVE))
				source.Reply(_("You will be notified when new memos arrive."));
			else if (u->Account()->HasFlag(NI_MEMO_SIGNON))
				source.Reply(_("You will be notified of new memos at logon."));
			else
				source.Reply(_("You will not be notified of new memos."));
		}
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Without a parameter, displays information on the number of\n"
				"memos you have, how many of them are unread, and how many\n"
				"total memos you can receive.\n"
				" \n"
				"With a channel parameter, displays the same information for\n"
				"the given channel.\n"
				" \n"
				"With a nickname parameter, displays the same information\n"
				"for the given nickname.  This use limited to \002Services\n"
				"Operators\002."));

		return true;
	}
};

class MSInfo : public Module
{
	CommandMSInfo commandmsinfo;

 public:
	MSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandmsinfo(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandmsinfo);
	}
};

MODULE_INIT(MSInfo)

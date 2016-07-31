/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"

class CommandMSInfo : public Command
{
 public:
	CommandMSInfo(Module *creator) : Command(creator, "memoserv/info", 0, 1)
	{
		this->SetDesc(_("Displays information about your memos"));
		this->SetSyntax(_("[\037user\037 | \037channel\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		NickServ::Account *nc = source.nc;
		MemoServ::MemoInfo *mi;
		NickServ::Nick *na = NULL;
		ChanServ::Channel *ci = NULL;
		const Anope::string &nname = !params.empty() ? params[0] : "";
		bool hardmax;

		if (!nname.empty() && nname[0] != '#' && source.HasPriv("memoserv/info"))
		{
			na = NickServ::FindNick(nname);
			if (!na)
			{
				source.Reply(_("\002{0}\002 isn't registered."), nname);
				return;
			}
			mi = na->GetAccount()->GetMemos();
			hardmax = na->GetAccount()->HasFieldS("MEMO_HARDMAX");
		}
		else if (!nname.empty() && nname[0] == '#')
		{
			ci = ChanServ::Find(nname);
			if (!ci)
			{
				source.Reply(_("Channel \002{0}\002 isn't registered."), nname);
				return;
			}

			if (!source.AccessFor(ci).HasPriv("MEMO"))
			{
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "MEMO", ci->GetName());
				return;
			}

			mi = ci->GetMemos();
			hardmax = ci->HasFieldS("MEMO_HARDMAX");
		}
		else if (!nname.empty()) /* It's not a chan and we aren't an oper */
		{
			source.Reply(_("Access denied. You do not have the correct operator privilege to see the memo info of \002{0}\002."), nname);
			return;
		}
		else
		{
			mi = nc->GetMemos();
			hardmax = nc->HasFieldS("MEMO_HARDMAX");
		}
		if (!mi)
			return;

		auto memos = mi->GetMemos();

		if (!nname.empty() && (ci || na->GetAccount() != nc))
		{
			if (memos.empty())
				source.Reply(_("%s currently has no memos."), nname.c_str());
			else if (memos.size() == 1)
			{
				if (mi->GetMemo(0)->GetUnread())
					source.Reply(_("\002{0}\002 currently has \0021\002 memo, and it has not yet been read."), nname);
				else
					source.Reply(_("\002{0}\002 currently has \0021\002 memo."), nname);
			}
			else
			{
				unsigned count = 0, i, end;
				for (i = 0, end = memos.size(); i < end; ++i)
					if (mi->GetMemo(i)->GetUnread())
						++count;
				if (count == memos.size())
					source.Reply(_("\002{0}\002 currently has \002{1]\002 memos; all of them are unread."), nname, count);
				else if (!count)
					source.Reply(_("\002{0}\002 currently has \002{1}\002 memos."), nname, memos.size());
				else if (count == 1)
					source.Reply(_("\002{0}\002 currently has \002{1}\002 memos, of which \0021\002 is unread."), nname, memos.size());
				else
					source.Reply(_("\002{0}\002 currently has \002{1]\002 memos, of which \002{2}\002 are unread."), nname, memos.size(), count);
			}
			if (mi->GetMemoMax() >= 0)
			{
				if (hardmax)
					source.Reply(_("The memo limit of \002{0}\002 is \002{1}\002, and may not be changed."), nname, mi->GetMemoMax());
				else
					source.Reply(_("The memo limit of \002{0}\002 is \002{1}\002."), nname, mi->GetMemoMax());
			}
			else
				source.Reply(_("\002{0}\002 has no memo limit."), nname);

			if (na)
			{
				if (na->GetAccount()->HasFieldS("MEMO_RECEIVE") && na->GetAccount()->HasFieldS("MEMO_SIGNON"))
					source.Reply(_("\002{0}\002 is notified of new memos at logon and when they arrive."), nname);
				else if (na->GetAccount()->HasFieldS("MEMO_RECEIVE"))
					source.Reply(_("\002{0}\002 is notified when new memos arrive."), nname);
				else if (na->GetAccount()->HasFieldS("MEMO_SIGNON"))
					source.Reply(_("\002{0}\002 is notified of news memos at logon."), nname);
				else
					source.Reply(_("\002{0}\002 is not notified of new memos."), nname);
			}
		}
		else
		{
			if (memos.empty())
				source.Reply(_("You currently have no memos."));
			else if (memos.size() == 1)
			{
				if (mi->GetMemo(0)->GetUnread())
					source.Reply(_("You currently have \0021\002 memo, and it has not yet been read."));
				else
					source.Reply(_("You currently have \0021\002 memo."));
			}
			else
			{
				unsigned count = 0, i, end;
				for (i = 0, end = memos.size(); i < end; ++i)
					if (mi->GetMemo(i)->GetUnread())
						++count;
				if (count == memos.size())
					source.Reply(_("You currently have \002{0}\002 memos; all of them are unread."), count);
				else if (!count)
					source.Reply(_("You currently have \002{0}\002 memos."), memos.size());
				else if (count == 1)
					source.Reply(_("You currently have \002{0}\002 memos, of which \0021\002 is unread."), memos.size());
				else
					source.Reply(_("You currently have \002{0}\002 memos, of which \002{1}\002 are unread."), memos.size(), count);
			}

			if (!mi->GetMemoMax())
			{
				if (!source.IsServicesOper() && hardmax)
					source.Reply(_("Your memo limit is \0020\002; you will not receive any new memos. You cannot change this limit."));
				else
					source.Reply(_("Your memo limit is \0020\002; you will not receive any new memos."));
			}
			else if (mi->GetMemoMax() > 0)
			{
				if (!source.IsServicesOper() && hardmax)
					source.Reply(_("Your memo limit is \002{0}\002, and may not be changed."), mi->GetMemoMax());
				else
					source.Reply(_("Your memo limit is \002{0}\002."), mi->GetMemoMax());
			}
			else
				source.Reply(_("You have no limit on the number of memos you may keep."));

			if (nc->HasFieldS("MEMO_RECEIVE") && nc->HasFieldS("MEMO_SIGNON"))
				source.Reply(_("You will be notified of new memos at logon and when they arrive."));
			else if (nc->HasFieldS("MEMO_RECEIVE"))
				source.Reply(_("You will be notified when new memos arrive."));
			else if (nc->HasFieldS("MEMO_SIGNON"))
				source.Reply(_("You will be notified of new memos at logon."));
			else
				source.Reply(_("You will not be notified of new memos."));
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Without a parameter, displays information on the number of memos you have, how many of them are unread, and how many total memos you can receive."
		               " With a parameter, displays the same information for the given \037user\037 or \037channel\037, if you have the appropriate privilege.\n"
			       "\n"
			       "Use of this command on a channel requires the \002{0}\002 privilege on \037channel\037."),
		               source.command);

		return true;
	}
};

class MSInfo : public Module
{
	CommandMSInfo commandmsinfo;

 public:
	MSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandmsinfo(this)
	{

	}
};

MODULE_INIT(MSInfo)

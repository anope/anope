// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"

class CommandMSCancel final
	: public Command
{
public:
	CommandMSCancel(Module *creator) : Command(creator, "memoserv/cancel", 1, 1)
	{
		this->SetDesc(_("Cancel the last memo you sent"));
		this->SetSyntax(_("{\037nick\037 | \037channel\037}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const Anope::string &nname = params[0];

		bool ischan;
		MemoInfo *mi = MemoInfo::GetMemoInfo(nname, ischan);

		if (mi == NULL)
		{
			source.Reply(ischan ? CHAN_X_NOT_REGISTERED : NICK_X_NOT_REGISTERED, nname.c_str());
			return;
		}

		ChannelInfo *ci = NULL;
		NickAlias *na = NULL;
		if (ischan)
		{
			ci = ChannelInfo::Find(nname);

			if (ci == NULL)
				return; // can't happen
		}
		else
		{
			na = NickAlias::Find(nname);

			if (na == NULL)
				return; // can't happen
		}

		for (int i = mi->memos->size() - 1; i >= 0; --i)
		{
			Memo *m = mi->GetMemo(i);

			if (!m->unread)
				continue;

			NickAlias *sender = NickAlias::Find(m->sender);

			if (sender && sender->nc == source.GetAccount())
			{
				FOREACH_MOD(OnMemoDel, (ischan ? ci->name : na->nc->display, mi, m));
				mi->Del(i);
				source.Reply(_("Last memo to \002%s\002 has been cancelled."), (ischan ? ci->name : na->nc->display).c_str());
				return;
			}
		}

		source.Reply(_("No memo was cancelable."));
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Cancels the last memo you sent to the given nick or channel, "
			"provided it has not been read at the time you use the command."
		));
		return true;
	}
};

class MSCancel final
	: public Module
{
	CommandMSCancel commandmscancel;

public:
	MSCancel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandmscancel(this)
	{
	}
};

MODULE_INIT(MSCancel)

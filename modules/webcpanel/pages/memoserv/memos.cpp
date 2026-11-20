// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
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

#include "../../webcpanel.h"

WebCPanel::MemoServ::Memos::Memos(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::MemoServ::Memos::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	ChannelInfo *ci;
	const MemoInfo *mi;
	Memo *m;

	for (const auto &[_, ci2] : *RegisteredChannelList)
	{
		ci = ci2;

		if (ci->AccessFor(na->nc).HasPriv("MEMO"))
		{
			replacements["CHANNEL_NAMES"] = ci->name;
			replacements["ESCAPED_CHANNEL_NAMES"] = HTTP::URLEncode(ci->name);
		}
	}

	const Anope::string chname = WebPanel::GetRequestData(message.get_data, "channel");
	if (chname.empty())
	{
		replacements["MESSAGES"] = "No Channel specified, displaying the memos for your Nick";
		mi = &na->nc->memos;
	}
	else
	{
		ci = ChannelInfo::Find(chname);
		if (ci)
		{
			replacements["MESSAGES"] = "Displaying the memos for " + chname + ".";
			mi = &ci->memos;

			replacements["CHANNEL_NAME"] = ci->name;
			replacements["ESCAPED_CHANNEL_NAME"] = HTTP::URLEncode(ci->name);
		}
		else
		{
			replacements["MESSAGES"] = "Channel " + chname + " not found, displaying the memos for your nick";
			mi = &na->nc->memos;
		}
	}

	const Anope::string receiver = WebPanel::GetRequestData(message.post_data, "receiver");
	const Anope::string msg = WebPanel::GetRequestData(message.post_data, "message");
	if (!receiver.empty() && !msg.empty())
	{
		std::vector<Anope::string> params;
		params.push_back(receiver);
		params.push_back(msg);

		WebPanel::RunCommand(client, na->nc->display, na->nc, "MemoServ", "memoserv/send", params, replacements, "CMDR");
	}

	const Anope::string num = WebPanel::GetRequestData(message.get_data, "number");
	if (message.get_data.count("del") > 0 && !num.empty())
	{
		std::vector<Anope::string> params;
		if (!chname.empty())
			params.push_back(chname);
		params.push_back(num);

		WebPanel::RunCommand(client, na->nc->display, na->nc, "MemoServ", "memoserv/del", params, replacements, "CMDR");
	}

	if (message.get_data.count("read") > 0 && !num.empty())
	{
		std::vector<Anope::string> params;
		int number = -1;

		if (auto n = Anope::TryConvert<int>(num))
			number = n.value();
		else
			replacements["MESSAGES"] = "ERROR - invalid parameter for NUMBER";

		if (number > 0)
		{
			m = mi->GetMemo(number-1);

			if (!m)
				replacements["MESSAGES"] = "ERROR - invalid memo number.";
			else if (message.get_data["read"] == "1")
				m->unread = false;
			else if (message.get_data["read"] == "2")
				m->unread = true;
		}
	}

	for (unsigned i = 0; i < mi->memos->size(); ++i)
	{
		m = mi->GetMemo(i);
		replacements["NUMBER"] = Anope::ToString(i+1);
		replacements["SENDER"] = m->sender;
		replacements["TIME"] = Anope::strftime(m->time);
		replacements["TEXT"] = m->text;
		if (m->unread)
			replacements["UNREAD"] = "YES";
		else
			replacements["UNREAD"] = "NO";
	}

	TemplateFileServer page("memoserv/memos.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

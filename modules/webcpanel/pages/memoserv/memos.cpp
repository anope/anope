/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2016 Anope Team <team@anope.org>
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

#include "../../webcpanel.h"

WebCPanel::MemoServ::Memos::Memos(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::MemoServ::Memos::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{
	const Anope::string &chname = message.get_data["channel"];
	::ChanServ::Channel *ci;
	::MemoServ::MemoInfo *mi;
	::MemoServ::Memo *m;

	if (::ChanServ::service)
		for (auto& it : ::ChanServ::service->GetChannels())
		{
			ci = it.second;

			if (ci->AccessFor(na->GetAccount()).HasPriv("MEMO"))
			{
				replacements["CHANNEL_NAMES"] = ci->GetName();
				replacements["ESCAPED_CHANNEL_NAMES"] = HTTPUtils::URLEncode(ci->GetName());
			}
		}

	if (chname.empty())
	{
		replacements["MESSAGES"] = "No Channel specified, displaying the memos for your Nick";
		mi = na->GetAccount()->GetMemos();
	}
	else
	{
		ci = ::ChanServ::Find(chname);
		if (ci)
		{
			replacements["MESSAGES"] = "Displaying the memos for " + chname + ".";
			mi = ci->GetMemos();
		}
		else
		{
			replacements["MESSAGES"] = "Channel " + chname + " not found, displaying the memos for your nick";
			mi = na->GetAccount()->GetMemos();
		}

		replacements["CHANNEL_NAME"] = ci->GetName();
		replacements["ESCAPED_CHANNEL_NAME"] = HTTPUtils::URLEncode(ci->GetName());
	}
	if (message.post_data.count("receiver") > 0 && message.post_data.count("message") > 0)
	{
		std::vector<Anope::string> params;
		params.push_back(HTTPUtils::URLDecode(message.post_data["receiver"]));
		params.push_back(HTTPUtils::URLDecode(message.post_data["message"]));

		WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "MemoServ", "memoserv/send", params, replacements, "CMDR");
	}
	if (message.get_data.count("del") > 0 && message.get_data.count("number") > 0)
	{
		std::vector<Anope::string> params;
		if (!chname.empty())
			params.push_back(chname);
		params.push_back(message.get_data["number"]);

		WebPanel::RunCommand(na->GetAccount()->GetDisplay(), na->GetAccount(), "MemoServ", "memoserv/del", params, replacements, "CMDR");
	}
	if (message.get_data.count("read") > 0 && message.get_data.count("number") > 0)
	{
		std::vector<Anope::string> params;
		int number = -1;

		try
		{
			number = convertTo<int>(message.get_data["number"]);
		}
		catch (const ConvertException &ex)
		{
			replacements["MESSAGES"] = "ERROR - invalid parameter for NUMBER";
		}

		if (number > 0)
		{
			m = mi ? mi->GetMemo(number-1) : nullptr;

			if (!m)
				replacements["MESSAGES"] = "ERROR - invalid memo number.";
			else if (message.get_data["read"] == "1")
				m->SetUnread(false);
			else if (message.get_data["read"] == "2")
				m->SetUnread(true);
		}
	}

	if (mi)
		for (unsigned i = 0; i < mi->GetMemos().size(); ++i)
		{
			m = mi->GetMemo(i);
			replacements["NUMBER"] = stringify(i+1);
			replacements["SENDER"] = m->GetSender();
			replacements["TIME"] = Anope::strftime(m->GetTime());
			replacements["TEXT"] = HTTPUtils::Escape(m->GetText());
			if (m->GetUnread())
				replacements["UNREAD"] = "YES";
			else
				replacements["UNREAD"] = "NO";
		}

	TemplateFileServer page("memoserv/memos.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}


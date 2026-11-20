/*
 * (C) 2003-2026 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

WebCPanel::MemoServ::Memos::Memos(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::MemoServ::Memos::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	ChannelInfo *ci;
	const MemoInfo *mi;
	Memo *m;

	for (registered_channel_map::const_iterator it = RegisteredChannelList->begin(), it_end = RegisteredChannelList->end(); it != it_end; ++it)
	{
		ci = it->second;

		if (ci->AccessFor(na->nc).HasPriv("MEMO"))
		{
			replacements["CHANNEL_NAMES"] = ci->name;
			replacements["ESCAPED_CHANNEL_NAMES"] = HTTPUtils::URLEncode(ci->name);
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
			replacements["ESCAPED_CHANNEL_NAME"] = HTTPUtils::URLEncode(ci->name);
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

		try
		{
			number = convertTo<int>(num);
		}
		catch (const ConvertException &ex)
		{
			replacements["MESSAGES"] = "ERROR - invalid parameter for NUMBER";
		}

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
		replacements["NUMBER"] = stringify(i+1);
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

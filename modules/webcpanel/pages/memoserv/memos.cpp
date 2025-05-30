/*
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

WebCPanel::MemoServ::Memos::Memos(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::MemoServ::Memos::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	const Anope::string &chname = message.get_data["channel"];
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
	if (message.post_data.count("receiver") > 0 && message.post_data.count("message") > 0)
	{
		std::vector<Anope::string> params;
		params.push_back(HTTP::URLDecode(message.post_data["receiver"]));
		params.push_back(HTTP::URLDecode(message.post_data["message"]));

		WebPanel::RunCommand(client, na->nc->display, na->nc, "MemoServ", "memoserv/send", params, replacements, "CMDR");
	}
	if (message.get_data.count("del") > 0 && message.get_data.count("number") > 0)
	{
		std::vector<Anope::string> params;
		if (!chname.empty())
			params.push_back(chname);
		params.push_back(message.get_data["number"]);

		WebPanel::RunCommand(client, na->nc->display, na->nc, "MemoServ", "memoserv/del", params, replacements, "CMDR");
	}
	if (message.get_data.count("read") > 0 && message.get_data.count("number") > 0)
	{
		std::vector<Anope::string> params;
		int number = -1;

		if (auto num = Anope::TryConvert<int>(message.get_data["number"]))
			number = num.value();
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

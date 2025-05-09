/*
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

WebCPanel::ChanServ::Set::Set(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::ChanServ::Set::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	const Anope::string &chname = message.get_data["channel"];
	bool can_set = false;
	TemplateFileServer page("chanserv/set.html");

	BuildChanList(na, replacements);

	if (chname.empty())
	{
		page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	ChannelInfo *ci = ChannelInfo::Find(chname);

	if (!ci)
	{
		page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	replacements["OKAY"];

	if (ci->AccessFor(na->nc).HasPriv("SET"))
	{
		replacements["CAN_SET"];
		can_set = true;
	}

	if (can_set && !message.post_data.empty())
	{
		if (ci->HasExt("KEEPTOPIC") != message.post_data.count("keeptopic"))
		{
			if (!ci->HasExt("KEEPTOPIC"))
				ci->Extend<bool>("KEEPTOPIC");
			else
				ci->Shrink<bool>("KEEPTOPIC");
			replacements["MESSAGES"] = "Keeptopic updated";
		}
		if (ci->HasExt("PEACE") != message.post_data.count("peace"))
		{
			if (!ci->HasExt("PEACE"))
				ci->Extend<bool>("PEACE");
			else
				ci->Shrink<bool>("PEACE");
			replacements["MESSAGES"] = "Peace updated";
		}
		if (ci->HasExt("CS_PRIVATE") != message.post_data.count("private"))
		{
			if (!ci->HasExt("CS_PRIVATE"))
				ci->Extend<bool>("CS_PRIVATE");
			else
				ci->Shrink<bool>("CS_PRIVATE");
			replacements["MESSAGES"] = "Private updated";
		}
		if (ci->HasExt("RESTRICTED") != message.post_data.count("restricted"))
		{
			if (!ci->HasExt("RESTRICTED"))
				ci->Extend<bool>("RESTRICTED");
			else
				ci->Shrink<bool>("RESTRICTED");
			replacements["MESSAGES"] = "Restricted updated";
		}
		if (ci->HasExt("SECUREOPS") != message.post_data.count("secureops"))
		{
			if (!ci->HasExt("SECUREOPS"))
				ci->Extend<bool>("SECUREOPS");
			else
				ci->Shrink<bool>("SECUREOPS");
			replacements["MESSAGES"] = "Secureops updated";
		}
		if (ci->HasExt("TOPICLOCK") != message.post_data.count("topiclock"))
		{
			if (!ci->HasExt("TOPICLOCK"))
				ci->Extend<bool>("TOPICLOCK");
			else
				ci->Shrink<bool>("TOPICLOCK");
			replacements["MESSAGES"] = "Topiclock updated";
		}
	}

	replacements["CHANNEL"] = ci->name;
	replacements["CHANNEL_ESCAPED"] = HTTP::URLEncode(ci->name);
	if (ci->GetFounder())
		replacements["FOUNDER"] = ci->GetFounder()->display;
	if (ci->GetSuccessor())
		replacements["SUCCESSOR"] = ci->GetSuccessor()->display;
	replacements["REGISTERED"] = Anope::strftime(ci->registered, na->nc);
	replacements["LAST_USED"] = Anope::strftime(ci->last_used, na->nc);
	replacements["ESCAPED_CHANNEL"] = HTTP::URLEncode(chname);

	if (!ci->last_topic.empty())
	{
		replacements["LAST_TOPIC"] = ci->last_topic;
		replacements["LAST_TOPIC_SETTER"] = ci->last_topic_setter;
	}

	if (can_set)
	{
		if (ci->HasExt("KEEPTOPIC"))
			replacements["KEEPTOPIC"];

		if (ci->HasExt("PEACE"))
			replacements["PEACE"];

		if (ci->HasExt("CS_PRIVATE"))
			replacements["PRIVATE"];

		if (ci->HasExt("RESTRICTED"))
			replacements["RESTRICTED"];

		if (ci->HasExt("SECUREOPS"))
			replacements["SECUREOPS"];

		if (ci->HasExt("TOPICLOCK"))
			replacements["TOPICLOCK"];
	}

	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

std::set<Anope::string> WebCPanel::ChanServ::Set::GetData()
{
	std::set<Anope::string> v;
	v.insert("channel");
	return v;
}

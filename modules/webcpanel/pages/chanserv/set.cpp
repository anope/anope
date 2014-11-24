/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"
#include "utils.h"

WebCPanel::ChanServ::Set::Set(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::ChanServ::Set::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
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

	::ChanServ::Channel *ci = ::ChanServ::Find(chname);

	if (!ci)
	{
		page.Serve(server, page_name, client, message, reply, replacements);
		return true;
	}

	replacements["OKAY"];

	if (ci->AccessFor(na->GetAccount()).HasPriv("SET"))
	{
		replacements["CAN_SET"];
		can_set = true;
	}

	if (can_set && message.post_data.empty() == false)
	{
		if (ci->HasFieldS("KEEPTOPIC") != message.post_data.count("keeptopic"))
		{
			if (!ci->HasFieldS("KEEPTOPIC"))
				ci->SetS<bool>("KEEPTOPIC", true);
			else
				ci->UnsetS<bool>("KEEPTOPIC");
			replacements["MESSAGES"] = "Secure updated";
		}
		if (ci->HasFieldS("PEACE") != message.post_data.count("peace"))
		{
			if (!ci->HasFieldS("PEACE"))
				ci->SetS<bool>("PEACE", true);
			else
				ci->UnsetS<bool>("PEACE");
			replacements["MESSAGES"] = "Peace updated";
		}
		if (ci->HasFieldS("CS_PRIVATE") != message.post_data.count("private"))
		{
			if (!ci->HasFieldS("CS_PRIVATE"))
				ci->SetS<bool>("CS_PRIVATE", true);
			else
				ci->UnsetS<bool>("CS_PRIVATE");
			replacements["MESSAGES"] = "Private updated";
		}
		if (ci->HasFieldS("RESTRICTED") != message.post_data.count("restricted"))
		{
			if (!ci->HasFieldS("RESTRICTED"))
				ci->SetS<bool>("RESTRICTED", true);
			else
				ci->UnsetS<bool>("RESTRICTED");
			replacements["MESSAGES"] = "Restricted updated";
		}
		if (ci->HasFieldS("CS_SECURE") != message.post_data.count("secure"))
		{
			if (!ci->HasFieldS("CS_SECURE"))
				ci->SetS<bool>("CS_SECURE", true);
			else
				ci->UnsetS<bool>("CS_SECURE");
			replacements["MESSAGES"] = "Secure updated";
		}
		if (ci->HasFieldS("SECUREOPS") != message.post_data.count("secureops"))
		{
			if (!ci->HasFieldS("SECUREOPS"))
				ci->SetS<bool>("SECUREOPS", true);
			else
				ci->UnsetS<bool>("SECUREOPS");
			replacements["MESSAGES"] = "Secureops updated";
		}
		if (ci->HasFieldS("TOPICLOCK") != message.post_data.count("topiclock"))
		{
			if (!ci->HasFieldS("TOPICLOCK"))
				ci->SetS<bool>("TOPICLOCK", true);
			else
				ci->UnsetS<bool>("TOPICLOCK");
			replacements["MESSAGES"] = "Topiclock updated";
		}
	}

	replacements["CHANNEL"] = HTTPUtils::Escape(ci->GetName());
	replacements["CHANNEL_ESCAPED"] = HTTPUtils::URLEncode(ci->GetName());
	if (ci->GetFounder())
		replacements["FOUNDER"] = ci->GetFounder()->GetDisplay();
	if (ci->GetSuccessor())
		replacements["SUCCESSOR"] = ci->GetSuccessor()->GetDisplay();
	replacements["TIME_REGISTERED"] = Anope::strftime(ci->GetTimeRegistered(), na->GetAccount());
	replacements["LAST_USED"] = Anope::strftime(ci->GetLastUsed(), na->GetAccount());
	replacements["ESCAPED_CHANNEL"] = HTTPUtils::URLEncode(chname);

	if (!ci->GetLastTopic().empty())
	{
		replacements["LAST_TOPIC"] = HTTPUtils::Escape(ci->GetLastTopic());
		replacements["LAST_TOPIC_SETTER"] = HTTPUtils::Escape(ci->GetLastTopicSetter());
	}

	if (can_set)
	{
		if (ci->HasFieldS("KEEPTOPIC"))
			replacements["KEEPTOPIC"];

		if (ci->HasFieldS("PEACE"))
			replacements["PEACE"];

		if (ci->HasFieldS("CS_PRIVATE"))
			replacements["PRIVATE"];

		if (ci->HasFieldS("RESTRICTED"))
			replacements["RESTRICTED"];

		if (ci->HasFieldS("CS_SECURE"))
			replacements["SECURE"];

		if (ci->HasFieldS("SECUREOPS"))
			replacements["SECUREOPS"];

		if (ci->HasFieldS("TOPICLOCK"))
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


/*
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

WebCPanel::ChanServ::Set::Set(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::ChanServ::Set::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	const Anope::string &chname = message.get_data["channel"];

	if (chname.empty())
	{
		reply.error = HTTP_FOUND;
		reply.headers["Location"] = Anope::string("http") + (use_ssl ? "s" : "") + "://" + message.headers["Host"] + "/chanserv/info";
		return true;
	}

	ChannelInfo *ci = ChannelInfo::Find(chname);

	if (!ci || !ci->AccessFor(na->nc).HasPriv("SET"))
		return true;

	if (message.post_data.empty() == false)
	{
		if (ci->HasExt("KEEPTOPIC") != message.post_data.count("keeptopic"))
		{
			if (!ci->HasExt("KEEPTOPIC"))
				ci->ExtendMetadata("KEEPTOPIC");
			else
				ci->Shrink("KEEPTOPIC");
			replacements["MESSAGES"] = "Secure updated";
		}
		if (ci->HasExt("PEACE") != message.post_data.count("peace"))
		{
			if (!ci->HasExt("PEACE"))
				ci->ExtendMetadata("PEACE");
			else
				ci->Shrink("PEACE");
			replacements["MESSAGES"] = "Peace updated";
		}
		if (ci->HasExt("PRIVATE") != message.post_data.count("private"))
		{
			if (!ci->HasExt("PRIVATE"))
				ci->ExtendMetadata("PRIVATE");
			else
				ci->Shrink("PRIVATE");
			replacements["MESSAGES"] = "Private updated";
		}
		if (ci->HasExt("RESTRICTED") != message.post_data.count("restricted"))
		{
			if (!ci->HasExt("RESTRICTED"))
				ci->ExtendMetadata("RESTRICTED");
			else
				ci->Shrink("RESTRICTED");
			replacements["MESSAGES"] = "Restricted updated";
		}
		if (ci->HasExt("SECURE") != message.post_data.count("secure"))
		{
			if (!ci->HasExt("SECURE"))
				ci->ExtendMetadata("SECURE");
			else
				ci->Shrink("SECURE");
			replacements["MESSAGES"] = "Secure updated";
		}
		if (ci->HasExt("SECUREOPS") != message.post_data.count("secureops"))
		{
			if (!ci->HasExt("SECUREOPS"))
				ci->ExtendMetadata("SECUREOPS");
			else
				ci->Shrink("SECUREOPS");
			replacements["MESSAGES"] = "Secureops updated";
		}
		if (ci->HasExt("TOPICLOCK") != message.post_data.count("topiclock"))
		{
			if (!ci->HasExt("TOPICLOCK"))
				ci->ExtendMetadata("TOPICLOCK");
			else
				ci->Shrink("TOPICLOCK");
			replacements["MESSAGES"] = "Topiclock updated";
		}
	}

	replacements["CHANNEL"] = HTTPUtils::Escape(ci->name);
	replacements["CHANNEL_ESCAPED"] = HTTPUtils::URLEncode(ci->name);
	if (ci->GetFounder())
		replacements["FOUNDER"] = ci->GetFounder()->display;
	if (ci->GetSuccessor())
		replacements["SUCCESSOR"] = ci->GetSuccessor()->display;
	replacements["TIME_REGISTERED"] = Anope::strftime(ci->time_registered, na->nc);
	replacements["LAST_USED"] = Anope::strftime(ci->last_used, na->nc);

	if (!ci->last_topic.empty())
	{
		replacements["LAST_TOPIC"] = HTTPUtils::Escape(ci->last_topic);
		replacements["LAST_TOPIC_SETTER"] = HTTPUtils::Escape(ci->last_topic_setter);
	}

	if (ci->HasExt("KEEPTOPIC"))
		replacements["KEEPTOPIC"];
	
	if (ci->HasExt("PEACE"))
		replacements["PEACE"];
	
	if (ci->HasExt("PRIVATE"))
		replacements["PRIVATE"];

	if (ci->HasExt("RESTRICTED"))
		replacements["RESTRICTED"];
	
	if (ci->HasExt("SECURE"))
		replacements["SECURE"];
	
	if (ci->HasExt("SECUREOPS"))
		replacements["SECUREOPS"];
	
	if (ci->HasExt("TOPICLOCK"))
		replacements["TOPICLOCK"];

	TemplateFileServer page("chanserv/set.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

std::set<Anope::string> WebCPanel::ChanServ::Set::GetData()
{
	std::set<Anope::string> v;
	v.insert("channel");
	return v;
}


/*
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

WebCPanel::ChanServ::Set::Set(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

void WebCPanel::ChanServ::Set::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	const Anope::string &chname = message.get_data["channel"];

	if (chname.empty())
	{
		reply.error = HTTP_FOUND;
		reply.headers["Location"] = "http://" + message.headers["Host"] + "/chanserv/info";
		return;
	}

	ChannelInfo *ci = cs_findchan(chname);

	if (!ci || !ci->AccessFor(na->nc).HasPriv("SET"))
		return;

	if (message.post_data.empty() == false)
	{
		if (ci->HasFlag(CI_KEEPTOPIC) != message.post_data.count("keeptopic"))
		{
			if (!ci->HasFlag(CI_KEEPTOPIC))
				ci->SetFlag(CI_KEEPTOPIC);
			else
				ci->UnsetFlag(CI_KEEPTOPIC);
			replacements["MESSAGES"] = "Secure updated";
		}
		if (ci->HasFlag(CI_PEACE) != message.post_data.count("peace"))
		{
			if (!ci->HasFlag(CI_PEACE))
				ci->SetFlag(CI_PEACE);
			else
				ci->UnsetFlag(CI_PEACE);
			replacements["MESSAGES"] = "Peace updated";
		}
		if (ci->HasFlag(CI_PRIVATE) != message.post_data.count("private"))
		{
			if (!ci->HasFlag(CI_PRIVATE))
				ci->SetFlag(CI_PRIVATE);
			else
				ci->UnsetFlag(CI_PRIVATE);
			replacements["MESSAGES"] = "Private updated";
		}
		if (ci->HasFlag(CI_RESTRICTED) != message.post_data.count("restricted"))
		{
			if (!ci->HasFlag(CI_RESTRICTED))
				ci->SetFlag(CI_RESTRICTED);
			else
				ci->UnsetFlag(CI_RESTRICTED);
			replacements["MESSAGES"] = "Restricted updated";
		}
		if (ci->HasFlag(CI_SECURE) != message.post_data.count("secure"))
		{
			if (!ci->HasFlag(CI_SECURE))
				ci->SetFlag(CI_SECURE);
			else
				ci->UnsetFlag(CI_SECURE);
			replacements["MESSAGES"] = "Secure updated";
		}
		if (ci->HasFlag(CI_SECUREOPS) != message.post_data.count("secureops"))
		{
			if (!ci->HasFlag(CI_SECUREOPS))
				ci->SetFlag(CI_SECUREOPS);
			else
				ci->UnsetFlag(CI_SECUREOPS);
			replacements["MESSAGES"] = "Secureops updated";
		}
		if (ci->HasFlag(CI_TOPICLOCK) != message.post_data.count("topiclock"))
		{
			if (!ci->HasFlag(CI_TOPICLOCK))
				ci->SetFlag(CI_TOPICLOCK);
			else
				ci->UnsetFlag(CI_TOPICLOCK);
			replacements["MESSAGES"] = "Topiclock updated";
		}
	}

	replacements["CHANNEL"] = HTTPUtils::Escape(ci->name);
	replacements["CHANNEL_ESCAPED"] = HTTPUtils::URLEncode(ci->name);
	if (ci->GetFounder())
		replacements["FOUNDER"] = ci->GetFounder()->display;
	if (ci->successor)
		replacements["SUCCESSOR"] = ci->successor->display;
	replacements["TIME_REGISTERED"] = do_strftime(ci->time_registered, na->nc);
	replacements["LAST_USED"] = do_strftime(ci->last_used, na->nc);

	if (!ci->last_topic.empty())
	{
		replacements["LAST_TOPIC"] = HTTPUtils::Escape(ci->last_topic);
		replacements["LAST_TOPIC_SETTER"] = HTTPUtils::Escape(ci->last_topic_setter);
	}

	if (ci->HasFlag(CI_KEEPTOPIC))
		replacements["KEEPTOPIC"];
	
	if (ci->HasFlag(CI_PEACE))
		replacements["PEACE"];
	
	if (ci->HasFlag(CI_PRIVATE))
		replacements["PRIVATE"];

	if (ci->HasFlag(CI_RESTRICTED))
		replacements["RESTRICTED"];
	
	if (ci->HasFlag(CI_SECURE))
		replacements["SECURE"];
	
	if (ci->HasFlag(CI_SECUREOPS))
		replacements["SECUREOPS"];
	
	if (ci->HasFlag(CI_TOPICLOCK))
		replacements["TOPICLOCK"];

	TemplateFileServer page("chanserv/set.html");
	page.Serve(server, page_name, client, message, reply, replacements);
}

std::set<Anope::string> WebCPanel::ChanServ::Set::GetData() anope_override
{
	std::set<Anope::string> v;
	v.insert("channel");
	return v;
}


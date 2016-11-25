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
		if (ci->IsKeepTopic() != message.post_data.count("keeptopic"))
		{
			ci->SetKeepTopic(!ci->IsKeepTopic());
			replacements["MESSAGES"] = "Secure updated";
		}
		if (ci->IsPeace() != message.post_data.count("peace"))
		{
			ci->SetPeace(!ci->IsPeace());
			replacements["MESSAGES"] = "Peace updated";
		}
		if (ci->IsPrivate() != message.post_data.count("private"))
		{
			ci->SetPrivate(!ci->IsPrivate());
			replacements["MESSAGES"] = "Private updated";
		}
		if (ci->IsRestricted() != message.post_data.count("restricted"))
		{
			ci->SetRestricted(!ci->IsRestricted());
			replacements["MESSAGES"] = "Restricted updated";
		}
		if (ci->IsSecure() != message.post_data.count("secure"))
		{
			ci->SetSecure(!ci->IsSecure());
			replacements["MESSAGES"] = "Secure updated";
		}
		if (ci->IsSecureOps() != message.post_data.count("secureops"))
		{
			ci->SetSecureOps(!ci->IsSecureOps());
			replacements["MESSAGES"] = "Secureops updated";
		}
		if (ci->IsTopicLock() != message.post_data.count("topiclock"))
		{
			ci->SetTopicLock(!ci->IsTopicLock());
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
		if (ci->IsKeepTopic())
			replacements["KEEPTOPIC"];

		if (ci->IsPeace())
			replacements["PEACE"];

		if (ci->IsPrivate())
			replacements["PRIVATE"];

		if (ci->IsRestricted())
			replacements["RESTRICTED"];

		if (ci->IsSecure())
			replacements["SECURE"];

		if (ci->IsSecureOps())
			replacements["SECUREOPS"];

		if (ci->IsTopicLock())
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


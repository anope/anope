/*
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../../webcpanel.h"

WebCPanel::NickServ::Info::Info(const Anope::string &cat, const Anope::string &u) : WebPanelProtectedPage(cat, u)
{
}

bool WebCPanel::NickServ::Info::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, NickAlias *na, TemplateFileServer::Replacements &replacements)
{
	if (message.post_data.empty() == false)
	{
		if (message.post_data.count("email") > 0)
		{
			if (message.post_data["email"] != na->nc->email)
			{
				if (!message.post_data["email"].empty() && !Mail::Validate(message.post_data["email"]))
					replacements["ERRORS"] = "Invalid email";
				else
				{
					na->nc->email = message.post_data["email"];
					replacements["MESSAGES"] = "Email updated";
				}
			}
		}
		if (message.post_data.count("greet") > 0)
		{
			if (message.post_data["greet"].replace_all_cs("+", " ") != na->nc->greet)
			{
				na->nc->greet = HTTPUtils::URLDecode(message.post_data["greet"]);
				replacements["MESSAGES"] = "Greet updated";
			}
		}
		if (na->nc->HasFlag(NI_AUTOOP) != message.post_data.count("autoop"))
		{
			if (!na->nc->HasFlag(NI_AUTOOP))
				na->nc->SetFlag(NI_AUTOOP);
			else
				na->nc->UnsetFlag(NI_AUTOOP);
			replacements["MESSAGES"] = "Autoop updated";
		}
		if (na->nc->HasFlag(NI_PRIVATE) != message.post_data.count("private"))
		{
			if (!na->nc->HasFlag(NI_PRIVATE))
				na->nc->SetFlag(NI_PRIVATE);
			else
				na->nc->UnsetFlag(NI_PRIVATE);
			replacements["MESSAGES"] = "Private updated";
		}
		if (na->nc->HasFlag(NI_SECURE) != message.post_data.count("secure"))
		{
			if (!na->nc->HasFlag(NI_SECURE))
				na->nc->SetFlag(NI_SECURE);
			else
				na->nc->UnsetFlag(NI_SECURE);
			replacements["MESSAGES"] = "Secure updated";
		}
		if (message.post_data["kill"] == "on" && !na->nc->HasFlag(NI_KILLPROTECT))
		{
			na->nc->SetFlag(NI_KILLPROTECT);
			na->nc->UnsetFlag(NI_KILL_QUICK);
			replacements["MESSAGES"] = "Kill updated";
		}
		else if (message.post_data["kill"] == "quick" && !na->nc->HasFlag(NI_KILL_QUICK))
		{
			na->nc->UnsetFlag(NI_KILLPROTECT);
			na->nc->SetFlag(NI_KILL_QUICK);
			replacements["MESSAGES"] = "Kill updated";
		}
		else if (message.post_data["kill"] == "off" && (na->nc->HasFlag(NI_KILLPROTECT) || na->nc->HasFlag(NI_KILL_QUICK)))
		{
			na->nc->UnsetFlag(NI_KILLPROTECT);
			na->nc->UnsetFlag(NI_KILL_QUICK);
			replacements["MESSAGES"] = "Kill updated";
		}
	}

	replacements["DISPLAY"] = HTTPUtils::Escape(na->nc->display);
	if (na->nc->email.empty() == false)
		replacements["EMAIL"] = HTTPUtils::Escape(na->nc->email);
	replacements["TIME_REGISTERED"] = Anope::strftime(na->time_registered, na->nc);
	if (na->HasVhost())
	{
		if (na->GetVhostIdent().empty() == false)
			replacements["VHOST"] = na->GetVhostIdent() + "@" + na->GetVhostHost();
		else
			replacements["VHOST"] = na->GetVhostHost();
	}
	replacements["GREET"] = HTTPUtils::Escape(na->nc->greet);
	if (na->nc->HasFlag(NI_AUTOOP))
		replacements["AUTOOP"];
	if (na->nc->HasFlag(NI_PRIVATE))
		replacements["PRIVATE"];
	if (na->nc->HasFlag(NI_SECURE))
		replacements["SECURE"];
	if (na->nc->HasFlag(NI_KILLPROTECT))
		replacements["KILL_ON"];
	if (na->nc->HasFlag(NI_KILL_QUICK))
		replacements["KILL_QUICK"];
	if (!na->nc->HasFlag(NI_KILLPROTECT) && !na->nc->HasFlag(NI_KILL_QUICK))
		replacements["KILL_OFF"];
	
	TemplateFileServer page("nickserv/info.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}


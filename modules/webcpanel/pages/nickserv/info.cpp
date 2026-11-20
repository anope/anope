/*
 * (C) 2003-2026 Anope Team
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
		const Anope::string email = WebPanel::GetRequestData(message.post_data, "email");
		if (!email.empty())
		{
			if (email != na->nc->email)
			{
				if (!Mail::Validate(email))
					replacements["ERRORS"] = "Invalid email";
				else
				{
					na->nc->email = email;
					replacements["MESSAGES"] = "Email updated";
				}
			}
		}

		const Anope::string post_greet = WebPanel::GetRequestData(message.post_data, "greet");
		if (!post_greet.empty())
		{
			Anope::string *greet = na->nc->GetExt<Anope::string>("greet");
			if (post_greet.empty())
				na->nc->Shrink<Anope::string>("greet");
			else if (!greet || post_greet != *greet)
				na->nc->Extend<Anope::string>("greet", post_greet);

			replacements["MESSAGES"] = "Greet updated";
		}
		if (na->nc->HasExt("AUTOOP") != !!message.post_data.count("autoop"))
		{
			if (!na->nc->HasExt("AUTOOP"))
				na->nc->Extend<bool>("AUTOOP");
			else
				na->nc->Shrink<bool>("AUTOOP");
			replacements["MESSAGES"] = "Autoop updated";
		}
		if (na->nc->HasExt("NS_PRIVATE") != !!message.post_data.count("private"))
		{
			if (!na->nc->HasExt("NS_PRIVATE"))
				na->nc->Extend<bool>("NS_PRIVATE");
			else
				na->nc->Shrink<bool>("NS_PRIVATE");
			replacements["MESSAGES"] = "Private updated";
		}
		if (na->nc->HasExt("NS_SECURE") != !!message.post_data.count("secure"))
		{
			if (!na->nc->HasExt("NS_SECURE"))
				na->nc->Extend<bool>("NS_SECURE");
			else
				na->nc->Shrink<bool>("NS_SECURE");
			replacements["MESSAGES"] = "Secure updated";
		}
		if (message.post_data["kill"] == "on" && !na->nc->HasExt("KILLPROTECT"))
		{
			na->nc->Extend<bool>("KILLPROTECT");
			na->nc->Shrink<bool>("KILL_QUICK");
			replacements["MESSAGES"] = "Kill updated";
		}
		else if (message.post_data["kill"] == "quick" && !na->nc->HasExt("KILL_QUICK"))
		{
			na->nc->Extend<bool>("KILLPROTECT");
			na->nc->Extend<bool>("KILL_QUICK");
			replacements["MESSAGES"] = "Kill updated";
		}
		else if (message.post_data["kill"] == "off" && (na->nc->HasExt("KILLPROTECT") || na->nc->HasExt("KILL_QUICK")))
		{
			na->nc->Shrink<bool>("KILLPROTECT");
			na->nc->Shrink<bool>("KILL_QUICK");
			replacements["MESSAGES"] = "Kill updated";
		}
	}

	replacements["DISPLAY"] = na->nc->display;
	if (na->nc->email.empty() == false)
		replacements["EMAIL"] = na->nc->email;
	replacements["TIME_REGISTERED"] = Anope::strftime(na->time_registered, na->nc);
	if (na->HasVhost())
	{
		if (na->GetVhostIdent().empty() == false)
			replacements["VHOST"] = na->GetVhostIdent() + "@" + na->GetVhostHost();
		else
			replacements["VHOST"] = na->GetVhostHost();
	}
	Anope::string *greet = na->nc->GetExt<Anope::string>("greet");
	if (greet)
		replacements["GREET"] = *greet;
	if (na->nc->HasExt("AUTOOP"))
		replacements["AUTOOP"];
	if (na->nc->HasExt("NS_PRIVATE"))
		replacements["PRIVATE"];
	if (na->nc->HasExt("NS_SECURE"))
		replacements["SECURE"];
	if (na->nc->HasExt("KILLPROTECT"))
		replacements["KILL_ON"];
	if (na->nc->HasExt("KILL_QUICK"))
		replacements["KILL_QUICK"];
	if (!na->nc->HasExt("KILLPROTECT") && !na->nc->HasExt("KILL_QUICK"))
		replacements["KILL_OFF"];

	TemplateFileServer page("nickserv/info.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

/*
 * (C) 2003-2025 Anope Team
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
	if (!message.post_data.empty())
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
			Anope::string *greet = na->nc->GetExt<Anope::string>("greet");
			const Anope::string &post_greet = HTTPUtils::URLDecode(message.post_data["greet"].replace_all_cs("+", " "));

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
		if (na->nc->HasExt("PROTECT") != !!message.post_data.count("protect"))
		{
			if (na->nc->HasExt("PROTECT"))
			{
				na->nc->Shrink<bool>("PROTECT");
				na->nc->Shrink<time_t>("PROTECT_AFTER");
			}
			else
			{
				na->nc->Extend<bool>("PROTECT");
			}
			replacements["MESSAGES"] = "Protect updated";
		}
		if (na->nc->HasExt("PROTECT") && message.post_data.count("protect_after") > 0)
		{
			auto &block = Config->GetModule("nickserv");
			auto minprotect = block.Get<time_t>("minprotect", "10s");
			auto maxprotect = block.Get<time_t>("maxprotect", "10m");

			auto secs = Anope::TryConvert<time_t>(message.post_data["protect_after"]);
			if (!secs)
				replacements["ERRORS"] = "Protection delay must be a number of seconds";
			else if (*secs < minprotect || *secs > maxprotect)
			{
				replacements["ERRORS"] = Anope::printf("Protection delay must be between %ld and %ld seconds.",
					minprotect, maxprotect);
			}
			else if (!na->nc->HasExt("PROTECT_AFTER") || *secs != *na->nc->GetExt<time_t>("PROTECT_AFTER"))
			{
				na->nc->Extend<time_t>("PROTECT_AFTER", *secs);
				replacements["MESSAGES"] = "Protect after updated";
			}
		}
		else if (na->nc->HasExt("PROTECT") && !message.post_data.count("protect_after"))
			na->nc->Shrink<time_t>("PROTECT_AFTER");
		if (na->nc->HasExt("NS_KEEP_MODES") != !!message.post_data.count("keepmodes"))
		{
			if (!na->nc->HasExt("NS_KEEP_MODES"))
				na->nc->Extend<bool>("NS_KEEP_MODES");
			else
				na->nc->Shrink<bool>("NS_KEEP_MODES");
			replacements["MESSAGES"] = "Keepmodes updated";
		}
		if (na->nc->HasExt("MSG") != !!message.post_data.count("msg"))
		{
			if (!na->nc->HasExt("MSG"))
				na->nc->Extend<bool>("MSG");
			else
				na->nc->Shrink<bool>("MSG");
			replacements["MESSAGES"] = "Message updated";
		}
		if (na->nc->HasExt("NEVEROP") != !!message.post_data.count("neverop"))
		{
			if (!na->nc->HasExt("NEVEROP"))
				na->nc->Extend<bool>("NEVEROP");
			else
				na->nc->Shrink<bool>("NEVEROP");
			replacements["MESSAGES"] = "Neverop updated";
		}
	}

	replacements["DISPLAY"] = na->nc->display;
	if (!na->nc->email.empty())
		replacements["EMAIL"] = na->nc->email;
	replacements["TIME_REGISTERED"] = Anope::strftime(na->nc->time_registered, na->nc);
	if (na->HasVHost())
		replacements["VHOST"] = na->GetVHostMask();
	Anope::string *greet = na->nc->GetExt<Anope::string>("greet");
	if (greet)
		replacements["GREET"] = *greet;
	if (na->nc->HasExt("AUTOOP"))
		replacements["AUTOOP"];
	if (na->nc->HasExt("NS_PRIVATE"))
		replacements["PRIVATE"];
	if (na->nc->HasExt("PROTECT"))
	{
		replacements["PROTECT"];
		auto *protectafter = na->nc->GetExt<time_t>("PROTECT_AFTER");
		if (protectafter)
			replacements["PROTECT_AFTER"] = Anope::ToString(*protectafter);
	}
	if (na->nc->HasExt("NS_KEEP_MODES"))
		replacements["KEEPMODES"];
	if (na->nc->HasExt("MSG"))
		replacements["MSG"];
	if (na->nc->HasExt("NEVEROP"))
		replacements["NEVEROP"];

	TemplateFileServer page("nickserv/info.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

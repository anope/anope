/*
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../webcpanel.h"

void WebCPanel::Register::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply)
{
	TemplateFileServer::Replacements replacements;

	replacements["TITLE"] = page_title;

	if (!Config->NSForceEmail)
		replacements["EMAIL_TYPE"] = "hidden";

	TemplateFileServer page("register.html");

	page.Serve(server, page_name, client, message, reply, replacements);
}


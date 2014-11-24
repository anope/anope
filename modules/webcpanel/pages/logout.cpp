/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../webcpanel.h"

WebCPanel::Logout::Logout(const Anope::string &u) : WebPanelProtectedPage("", u)
{
}

bool WebCPanel::Logout::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, ::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{
	na->ShrinkOK<Anope::string>("webcpanel_id");
	na->ShrinkOK<Anope::string>("webcpanel_ip");

	reply.error = HTTP_FOUND;
	reply.headers["Location"] = Anope::string("http") + (server->IsSSL() ? "s" : "") + "://" + message.headers["Host"] + "/";
	return true;
}


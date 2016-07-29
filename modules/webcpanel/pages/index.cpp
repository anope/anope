/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../webcpanel.h"
#include "modules/nickserv.h"

class WebpanelRequest : public NickServ::IdentifyRequestListener
{
	HTTPReply reply;
	HTTPMessage message;
	Reference<HTTPProvider> server;
	Anope::string page_name;
	Reference<HTTPClient> client;
	TemplateFileServer::Replacements replacements;

 public:
	WebpanelRequest(HTTPReply &r, HTTPMessage &m, HTTPProvider *s, const Anope::string &p_n, HTTPClient *c, TemplateFileServer::Replacements &re) : reply(r), message(m), server(s), page_name(p_n), client(c), replacements(re) { }

	void OnSuccess(NickServ::IdentifyRequest *req) override
	{
		if (!client || !server)
			return;
		::NickServ::Nick *na = ::NickServ::FindNick(req->GetAccount());
		if (!na)
		{
			this->OnFail(req);
			return;
		}

		if (na->GetAccount()->HasFieldS("NS_SUSPENDED"))
		{
			this->OnFail(req);
			return;
		}

		Anope::string id;
		for (int i = 0; i < 64; ++i)
		{
			char c;
			do
				c = 48 + (rand() % 75);
			while (!isalnum(c));
			id += c;
		}

		na->Extend<Anope::string>("webcpanel_id", id);
		na->Extend<Anope::string>("webcpanel_ip", client->GetIP());

		{
			HTTPReply::cookie c;
			c.push_back(std::make_pair("account", na->GetNick()));
			c.push_back(std::make_pair("Path", "/"));
			reply.cookies.push_back(c);
		}

		{
			HTTPReply::cookie c;
			c.push_back(std::make_pair("id", id));
			c.push_back(std::make_pair("Path", "/"));
			reply.cookies.push_back(c);
		}

		reply.error = HTTP_FOUND;
		reply.headers["Location"] = Anope::string("http") + (server->IsSSL() ? "s" : "") + "://" + message.headers["Host"] + "/nickserv/info";

		client->SendReply(&reply);
	}

	void OnFail(NickServ::IdentifyRequest *req) override
	{
		if (!client || !server)
			return;
		replacements["INVALID_LOGIN"] = "Invalid username or password";
		TemplateFileServer page("login.html");
		page.Serve(server, page_name, client, message, reply, replacements);

		client->SendReply(&reply);
	}
};

bool WebCPanel::Index::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply)
{
	TemplateFileServer::Replacements replacements;
	const Anope::string &user = message.post_data["username"], &pass = message.post_data["password"];

	replacements["TITLE"] = page_title;

	if (!user.empty() && !pass.empty() && ::NickServ::service)
	{
		// XXX Rate limit check.

		::NickServ::IdentifyRequest *req = ::NickServ::service->CreateIdentifyRequest(new WebpanelRequest(reply, message, server, page_name, client, replacements), me, user, pass);
		EventManager::Get()->Dispatch(&Event::CheckAuthentication::OnCheckAuthentication, nullptr, req);
		req->Dispatch();
		return false;
	}

	TemplateFileServer page("login.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}


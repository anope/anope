/*
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../webcpanel.h"

class WebpanelRequest : public IdentifyRequest
{
	HTTPReply reply;
	HTTPMessage message;
	Reference<HTTPProvider> server;
	Anope::string page_name;
	Reference<HTTPClient> client;
	TemplateFileServer::Replacements replacements;

 public:
	WebpanelRequest(Module *o, HTTPReply &r, HTTPMessage &m, HTTPProvider *s, const Anope::string &p_n, HTTPClient *c, TemplateFileServer::Replacements &re, const Anope::string &user, const Anope::string &pass) : IdentifyRequest(o, user, pass), reply(r), message(m), server(s), page_name(p_n), client(c), replacements(re) { }

	void OnSuccess() anope_override
	{
		if (!client)
			return;
		NickAlias *na = NickAlias::Find(this->GetAccount());
		if (!na)
		{
			this->OnFail();
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

		na->Extend("webcpanel_id", new ExtensibleItemClass<Anope::string>(id));
		na->Extend("webcpanel_ip", new ExtensibleItemClass<Anope::string>(client->GetIP()));

		{
			HTTPReply::cookie c;
			c.push_back(std::make_pair("account", na->nick));
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
		reply.headers["Location"] = Anope::string("http") + (use_ssl ? "s" : "") + "://" + message.headers["Host"] + "/nickserv/info";

		client->SendReply(&reply);
	}

	void OnFail() anope_override
	{
		if (!client)
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

	if (!user.empty() && !pass.empty())
	{
		// Rate limit check.

		WebpanelRequest *req = new WebpanelRequest(me, reply, message, server, page_name, client, replacements, user, pass);
		FOREACH_MOD(OnCheckAuthentication, (NULL, req));
		req->Dispatch();
		return false;
	}

	TemplateFileServer page("login.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}


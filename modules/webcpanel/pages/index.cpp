/*
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../webcpanel.h"

class WebpanelRequest final
	: public IdentifyRequest
{
	HTTP::Reply reply;
	HTTP::Message message;
	Reference<HTTP::Provider> server;
	Anope::string page_name;
	Reference<HTTP::Client> client;
	TemplateFileServer::Replacements replacements;

public:
	WebpanelRequest(Module *o, HTTP::Reply &r, HTTP::Message &m, HTTP::Provider *s, const Anope::string &p_n, HTTP::Client *c, TemplateFileServer::Replacements &re, const Anope::string &user, const Anope::string &pass)
		: IdentifyRequest(o, user, pass, c->GetIP())
		, reply(r)
		, message(m)
		, server(s)
		, page_name(p_n)
		, client(c)
		, replacements(re)
	{
	}

	void OnSuccess() override
	{
		if (!client || !server)
			return;
		NickAlias *na = NickAlias::Find(this->GetAccount());
		if (!na)
		{
			this->OnFail();
			return;
		}

		if (na->nc->HasExt("NS_SUSPENDED"))
		{
			this->OnFail();
			return;
		}

		// Rate limit logins to 1/sec
		time_t *last_login = na->nc->GetExt<time_t>("webcpanel_last_login");
		if (last_login != NULL && Anope::CurTime == *last_login)
		{
			this->OnFail();
			return;
		}

		Anope::string id;
		for (int i = 0; i < 64; ++i)
		{
			char c;
			do
				c = 48 + (Anope::RandomNumber() % 75);
			while (!isalnum(c));
			id += c;
		}

		na->Extend<Anope::string>("webcpanel_id", id);
		na->Extend<Anope::string>("webcpanel_ip", client->GetIP());
		na->nc->Extend<time_t>("webcpanel_last_login", Anope::CurTime);

		{
			HTTP::Reply::cookie c;
			c.emplace_back("account", na->nick);
			c.emplace_back("Path", "/");
			reply.cookies.push_back(c);
		}

		{
			HTTP::Reply::cookie c;
			c.emplace_back("id", id);
			c.emplace_back("Path", "/");
			reply.cookies.push_back(c);
		}

		reply.error = HTTP::FOUND;
		reply.headers["Location"] = Anope::string("http") + (server->IsSSL() ? "s" : "") + "://" + message.headers["Host"] + "/nickserv/info";

		client->SendReply(&reply);
	}

	void OnFail() override
	{
		if (!client || !server)
			return;
		replacements["INVALID_LOGIN"] = "Invalid username or password";
		TemplateFileServer page("login.html");
		page.Serve(server, page_name, client, message, reply, replacements);

		client->SendReply(&reply);
	}
};

bool WebCPanel::Index::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply)
{
	TemplateFileServer::Replacements replacements;
	const Anope::string &user = message.post_data["username"], &pass = message.post_data["password"];

	replacements["TITLE"] = page_title;

	if (!user.empty() && !pass.empty())
	{
		// Rate limit check.
		Anope::string ip = client->clientaddr.addr();

		Anope::unordered_map<time_t>::iterator it = last_login_attempt.find(ip);
		if (it != last_login_attempt.end())
		{
			time_t last_time = it->second;

			if (last_time == Anope::CurTime)
			{
				replacements["INVALID_LOGIN"] = "Rate limited";
				TemplateFileServer page("login.html");
				page.Serve(server, page_name, client, message, reply, replacements);
				return true;
			}
		}

		// don't let ip hash grow too long
		if (Anope::CurTime > last_clear + FLUSH_TIME)
		{
			last_login_attempt.clear();
			last_clear = Anope::CurTime;
		}

		last_login_attempt[ip] = Anope::CurTime;

		auto *req = new WebpanelRequest(me, reply, message, server, page_name, client, replacements, user, pass);
		FOREACH_MOD(OnCheckAuthentication, (NULL, req));
		req->Dispatch();
		return false;
	}

	TemplateFileServer page("login.html");
	page.Serve(server, page_name, client, message, reply, replacements);
	return true;
}

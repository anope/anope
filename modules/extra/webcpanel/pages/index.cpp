/*
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "../webcpanel.h"

void WebCPanel::Index::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply)
{
	TemplateFileServer::Replacements replacements;
	const Anope::string &user = message.post_data["username"], &pass = message.post_data["password"];

	replacements["TITLE"] = page_title;

	if (!user.empty() && !pass.empty())
	{
		// Rate limit check.

		NickAlias *na = findnick(user);

		EventReturn MOD_RESULT = EVENT_CONTINUE;

		if (na)
		{
			FOREACH_RESULT(I_OnCheckAuthentication, OnCheckAuthentication(NULL, NULL, std::vector<Anope::string>(), na->nc->display, pass));
		}

		if (MOD_RESULT == EVENT_ALLOW)
		{
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
			reply.headers["Location"] = "http://" + message.headers["Host"] + "/nickserv/info";
			return;
		}
		else
		{
			replacements["INVALID_LOGIN"] = "Invalid username or password";
		}
	}

	TemplateFileServer page("login.html");

	page.Serve(server, page_name, client, message, reply, replacements);
}


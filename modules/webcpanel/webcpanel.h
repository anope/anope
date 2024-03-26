/*
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

#include "module.h"
#include "modules/httpd.h"

#include "static_fileserver.h"
#include "template_fileserver.h"

extern Module *me;

extern Anope::string provider_name, template_base, page_title;

struct SubSection final
{
	Anope::string name;
	Anope::string url;
};

struct Section
{
	Anope::string name;
	std::vector<SubSection> subsections;
};

/* An interface for this webpanel used by other modules */
class Panel final
	: public Section
	, public Service
{
public:
	Panel(Module *c, const Anope::string &n) : Service(c, "Panel", n) { }

	std::vector<Section> sections;

	NickAlias *GetNickFromSession(HTTPClient *client, HTTPMessage &msg)
	{
		if (!client)
			return NULL;

		const Anope::string &acc = msg.cookies["account"], &id = msg.cookies["id"];

		if (acc.empty() || id.empty())
			return NULL;

		NickAlias *na = NickAlias::Find(acc);
		if (na == NULL)
			return NULL;

		Anope::string *n_id = na->GetExt<Anope::string>("webcpanel_id"), *n_ip = na->GetExt<Anope::string>("webcpanel_ip");
		if (n_id == NULL || n_ip == NULL)
			return NULL;
		else if (id != *n_id)
			return NULL;
		else if (client->GetIP() != *n_ip)
			return NULL;
		else
			return na;
	}
};

class WebPanelPage
	: public HTTPPage
{
public:
	WebPanelPage(const Anope::string &u, const Anope::string &ct = "text/html") : HTTPPage(u, ct)
	{
	}

	virtual bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &) = 0;
};

class WebPanelProtectedPage
	: public WebPanelPage
{
	Anope::string category;

public:
	WebPanelProtectedPage(const Anope::string &cat, const Anope::string &u, const Anope::string &ct = "text/html") : WebPanelPage(u, ct), category(cat)
	{
	}

	bool OnRequest(HTTPProvider *provider, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply) override final
	{
		ServiceReference<Panel> panel("Panel", "webcpanel");
		NickAlias *na;

		if (!panel || !(na = panel->GetNickFromSession(client, message)))
		{
			reply.error = HTTP_FOUND;
			reply.headers["Location"] = Anope::string("http") + (provider->IsSSL() ? "s" : "") + "://" + message.headers["Host"] + "/";
			return true; // Access denied
		}

		TemplateFileServer::Replacements replacements;

		replacements["TITLE"] = page_title;
		replacements["ACCOUNT"] = na->nc->display;
		replacements["PAGE_NAME"] = page_name;
		replacements["CATEGORY"] = category;
		if (na->nc->IsServicesOper())
			replacements["IS_OPER"];

		Anope::string sections, get;

		for (const auto &[key, value] : message.get_data)
		{
			if (this->GetData().count(key) > 0)
				get += "&" + key + "=" + HTTPUtils::URLEncode(value);
		}
		if (get.empty() == false)
			get = "?" + get.substr(1);

		Section *ns = NULL;
		for (auto &s : panel->sections)
		{
			if (s.name == this->category)
				ns = &s;
			replacements["CATEGORY_URLS"] = s.subsections[0].url;
			replacements["CATEGORY_NAMES"] = s.name;
		}

		if (ns)
		{
			sections = "";
			for (const auto &ss : ns->subsections)
			{
				replacements["SUBCATEGORY_URLS"] = ss.url;
				replacements["SUBCATEGORY_GETS"] = get;
				replacements["SUBCATEGORY_NAMES"] = ss.name;
			}
		}

		return this->OnRequest(provider, page_name, client, message, reply, na, replacements);
	}

	virtual bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, NickAlias *, TemplateFileServer::Replacements &) = 0;

	/* What get data should be appended to links in the navbar */
	virtual std::set<Anope::string> GetData() { return std::set<Anope::string>(); }
};

namespace WebPanel
{
	/** Run a command
	 * @param client HTTP client command is being issued for
	 * @param User name to run command as, probably nc->display unless nc == NULL
	 * @param nc Nick core to run command from
	 * @param service Service for source.owner and source.service
	 * @param c Command to run (as a service name)
	 * @param params Command parameters
	 * @param r Replacements, reply from command goes back here into key
	 * @param key The key to put the replies into r
	 */
	extern void RunCommand(HTTPClient *client, const Anope::string &user, NickCore *nc, const Anope::string &service, const Anope::string &c, std::vector<Anope::string> &params, TemplateFileServer::Replacements &r, const Anope::string &key = "MESSAGES");

	extern void RunCommandWithName(HTTPClient *client, NickCore *nc, const Anope::string &service, const Anope::string &c, const Anope::string &cmdname, std::vector<Anope::string> &params, TemplateFileServer::Replacements &r, const Anope::string &key = "MESSAGES");
}

#include "pages/index.h"
#include "pages/logout.h"
#include "pages/register.h"
#include "pages/confirm.h"

#include "pages/chanserv/chanserv.h"
#include "pages/hostserv/hostserv.h"
#include "pages/memoserv/memoserv.h"
#include "pages/nickserv/nickserv.h"
#include "pages/operserv/operserv.h"

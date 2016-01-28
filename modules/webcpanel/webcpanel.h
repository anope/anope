/*
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/httpd.h"

#include "static_fileserver.h"
#include "template_fileserver.h"

extern Module *me;

extern Anope::string provider_name, template_name, template_base, page_title;

struct SubSection
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
class Panel : public Section, public Service
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

class WebPanelPage : public HTTPPage
{
 public:
	WebPanelPage(const Anope::string &u, const Anope::string &ct = "text/html") : HTTPPage(u, ct)
	{
	}

	virtual bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &) = 0;
};

class WebPanelProtectedPage : public WebPanelPage
{
	Anope::string category;

 public:
	WebPanelProtectedPage(const Anope::string &cat, const Anope::string &u, const Anope::string &ct = "text/html") : WebPanelPage(u, ct), category(cat)
	{
	}

	bool OnRequest(HTTPProvider *provider, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply) anope_override anope_final
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

		for (std::map<Anope::string, Anope::string>::iterator it = message.get_data.begin(), it_end = message.get_data.end(); it != it_end; ++it)
			if (this->GetData().count(it->first) > 0)
				get += "&" + it->first + "=" + HTTPUtils::URLEncode(it->second);
		if (get.empty() == false)
			get = "?" + get.substr(1);

		Section *ns = NULL;
		for (unsigned i = 0; i < panel->sections.size(); ++i)
		{
			Section& s = panel->sections[i];
			if (s.name == this->category)
				ns = &s;
			replacements["CATEGORY_URLS"] = s.subsections[0].url;
			replacements["CATEGORY_NAMES"] = s.name;
		}

		if (ns)
		{
			sections = "";
			for (unsigned i = 0; i < ns->subsections.size(); ++i)
			{
				SubSection& ss = ns->subsections[i];
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
	 * @param User name to run command as, probably nc->display unless nc == NULL
	 * @param nc Nick core to run command from
	 * @param service Service for source.owner and source.service
	 * @param c Command to run (as a service name)
	 * @param params Command parameters
	 * @param r Replacements, reply from command goes back here into key
	 * @param key The key to put the replies into r
	 */
	extern void RunCommand(const Anope::string &user, NickCore *nc, const Anope::string &service, const Anope::string &c, std::vector<Anope::string> &params, TemplateFileServer::Replacements &r, const Anope::string &key = "MESSAGES");

	extern void RunCommandWithName(NickCore *nc, const Anope::string &service, const Anope::string &c, const Anope::string &cmdname, std::vector<Anope::string> &params, TemplateFileServer::Replacements &r, const Anope::string &key = "MESSAGES");
}

#include "pages/index.h"
#include "pages/logout.h"
#include "pages/register.h"
#include "pages/confirm.h"

#include "pages/nickserv/info.h"
#include "pages/nickserv/cert.h"
#include "pages/nickserv/access.h"
#include "pages/nickserv/alist.h"

#include "pages/chanserv/info.h"
#include "pages/chanserv/set.h"
#include "pages/chanserv/access.h"
#include "pages/chanserv/akick.h"
#include "pages/chanserv/modes.h"
#include "pages/chanserv/drop.h"

#include "pages/memoserv/memos.h"

#include "pages/hostserv/request.h"

#include "pages/operserv/akill.h"

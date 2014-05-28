/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

namespace WebCPanel
{

namespace ChanServ
{

class Access : public WebPanelProtectedPage
{
 public:
 	Access(const Anope::string &cat, const Anope::string &u);

	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, ::NickServ::Nick *, TemplateFileServer::Replacements &) override;

	std::set<Anope::string> GetData() override;
};

}

}


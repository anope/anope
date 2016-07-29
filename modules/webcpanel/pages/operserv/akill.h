/*
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

namespace WebCPanel
{

namespace OperServ
{

class Akill : public WebPanelProtectedPage
{
	ServiceReference<XLineManager> akills;
	
 public:
	Akill(const Anope::string &cat, const Anope::string &u);

	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, ::NickServ::Nick *, TemplateFileServer::Replacements &) override;
};

}

}


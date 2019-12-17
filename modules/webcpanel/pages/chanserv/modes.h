/*
 * (C) 2003-2019 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

namespace WebCPanel
{

namespace ChanServ
{

class Modes : public WebPanelProtectedPage
{
 public:
	Modes(const Anope::string &cat, const Anope::string &u);

	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, NickAlias *, TemplateFileServer::Replacements &) anope_override;

	std::set<Anope::string> GetData() anope_override;
};

}

}

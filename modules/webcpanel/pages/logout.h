/*
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

namespace WebCPanel
{

class Logout final
	: public WebPanelProtectedPage
{
public:
	Logout(const Anope::string &u);

	bool OnRequest(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &, NickAlias *, TemplateFileServer::Replacements &) override;
};

}

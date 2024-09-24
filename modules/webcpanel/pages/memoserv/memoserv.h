/*
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

namespace WebCPanel::MemoServ
{
	class Memos;
}

class WebCPanel::MemoServ::Memos final
	: public WebPanelProtectedPage
{
public:
	Memos(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, NickAlias *, TemplateFileServer::Replacements &) override;
};

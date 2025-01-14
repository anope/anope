/*
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

namespace WebCPanel::NickServ
{
	class Alist;
	class Cert;
	class Confirm;
	class Info;
}

class WebCPanel::NickServ::Alist final
	: public WebPanelProtectedPage
{
public:
	Alist(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, NickAlias *, TemplateFileServer::Replacements &) override;
};

class WebCPanel::NickServ::Cert final
	: public WebPanelProtectedPage
{
public:
	Cert(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, NickAlias *, TemplateFileServer::Replacements &) override;
};

class WebCPanel::NickServ::Confirm final
	: public WebPanelProtectedPage
{
public:
	Confirm(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, NickAlias *, TemplateFileServer::Replacements &) override;
};

class WebCPanel::NickServ::Info final
	: public WebPanelProtectedPage
{
public:
	Info(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, NickAlias *, TemplateFileServer::Replacements &) override;
};

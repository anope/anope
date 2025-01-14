/*
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

namespace WebCPanel::ChanServ
{
	class Access;
	class Akick;
	class Drop;
	class Info;
	class Modes;
	class Set;

	extern void BuildChanList(NickAlias *, TemplateFileServer::Replacements &);
}

class WebCPanel::ChanServ::Access final
	: public WebPanelProtectedPage
{
public:
	Access(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, NickAlias *, TemplateFileServer::Replacements &) override;
	std::set<Anope::string> GetData() override;
};

class WebCPanel::ChanServ::Akick final
	: public WebPanelProtectedPage
{
public:
	Akick(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, NickAlias *, TemplateFileServer::Replacements &) override;
	std::set<Anope::string> GetData() override;
};

class WebCPanel::ChanServ::Drop final
	: public WebPanelProtectedPage
{
public:
	Drop(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, NickAlias *, TemplateFileServer::Replacements &) override;
};

class WebCPanel::ChanServ::Info final
	: public WebPanelProtectedPage
{
public:
	Info(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, NickAlias *, TemplateFileServer::Replacements &) override;
};

class WebCPanel::ChanServ::Modes final
	: public WebPanelProtectedPage
{
public:
	Modes(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, NickAlias *, TemplateFileServer::Replacements &) override;
	std::set<Anope::string> GetData() override;
};

class WebCPanel::ChanServ::Set final
	: public WebPanelProtectedPage
{
public:
	Set(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, NickAlias *, TemplateFileServer::Replacements &) override;
	std::set<Anope::string> GetData() override;
};

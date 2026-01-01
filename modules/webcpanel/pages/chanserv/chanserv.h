// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

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
	bool OnRequest(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &, NickAlias *, TemplateFileServer::Replacements &) override;
	std::set<Anope::string> GetData() override;
};

class WebCPanel::ChanServ::Akick final
	: public WebPanelProtectedPage
{
public:
	Akick(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &, NickAlias *, TemplateFileServer::Replacements &) override;
	std::set<Anope::string> GetData() override;
};

class WebCPanel::ChanServ::Drop final
	: public WebPanelProtectedPage
{
public:
	Drop(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &, NickAlias *, TemplateFileServer::Replacements &) override;
};

class WebCPanel::ChanServ::Info final
	: public WebPanelProtectedPage
{
public:
	Info(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &, NickAlias *, TemplateFileServer::Replacements &) override;
};

class WebCPanel::ChanServ::Modes final
	: public WebPanelProtectedPage
{
public:
	Modes(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &, NickAlias *, TemplateFileServer::Replacements &) override;
	std::set<Anope::string> GetData() override;
};

class WebCPanel::ChanServ::Set final
	: public WebPanelProtectedPage
{
public:
	Set(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &, NickAlias *, TemplateFileServer::Replacements &) override;
	std::set<Anope::string> GetData() override;
};

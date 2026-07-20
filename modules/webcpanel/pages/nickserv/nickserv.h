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

namespace WebCPanel::NickServ
{
	class Alist;
	class Cert;
	class Info;
}

class WebCPanel::NickServ::Alist final
	: public WebPanelProtectedPage
{
public:
	Alist(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &, NickAlias *, TemplateFileServer::Replacements &) override;
};

class WebCPanel::NickServ::Cert final
	: public WebPanelProtectedPage
{
public:
	Cert(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &, NickAlias *, TemplateFileServer::Replacements &) override;
};

class WebCPanel::NickServ::Info final
	: public WebPanelProtectedPage
{
public:
	Info(const Anope::string &cat, const Anope::string &u);
	bool OnRequest(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &, NickAlias *, TemplateFileServer::Replacements &) override;
};

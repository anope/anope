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

#include "modules/httpd.h"

namespace WebCPanel
{

class Index final
	: public WebPanelPage
{
	static const int FLUSH_TIME = 60;

	Anope::unordered_map<time_t> last_login_attempt;
	time_t last_clear = 0;

public:
	Index(const Anope::string &u) : WebPanelPage(u) { }

	bool OnRequest(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &) override;
};

}

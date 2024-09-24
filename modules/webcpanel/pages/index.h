/*
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

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

	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &) override;
};

}

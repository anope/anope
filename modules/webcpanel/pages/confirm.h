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

class Confirm final
	: public WebPanelPage
{
public:
	Confirm(const Anope::string &u) : WebPanelPage(u) { }

	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &) override;
};

}

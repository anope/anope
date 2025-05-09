/*
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

#include "modules/httpd.h"

namespace WebCPanel
{

class Register final
	: public WebPanelPage
{
public:
	Register(const Anope::string &u) : WebPanelPage(u) { }

	bool OnRequest(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &) override;
};

}

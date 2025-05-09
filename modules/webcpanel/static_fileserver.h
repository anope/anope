/*
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

#include "modules/httpd.h"

/* A basic file server. Used for serving static content on disk. */
class StaticFileServer final
	: public HTTP::Page
{
	Anope::string file_name;
public:
	StaticFileServer(const Anope::string &f_n, const Anope::string &u, const Anope::string &c_t);

	bool OnRequest(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &) override;
};

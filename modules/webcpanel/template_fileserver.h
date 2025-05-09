/*
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

#include "modules/httpd.h"

/* A basic file server. Used for serving non-static non-binary content on disk. */
class TemplateFileServer final
{
	Anope::string file_name;
public:
	struct Replacements final
		: std::multimap<Anope::string, Anope::string>
	{
		Anope::string &operator[](const Anope::string &key)
		{
			return emplace(key, "")->second;
		}
	};

	TemplateFileServer(const Anope::string &f_n);

	void Serve(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &, Replacements &);
};

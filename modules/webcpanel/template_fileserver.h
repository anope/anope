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

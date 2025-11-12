// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2025 Anope Contributors
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

/* A basic file server. Used for serving static content on disk. */
class StaticFileServer final
	: public HTTP::Page
{
	Anope::string file_name;
public:
	StaticFileServer(const Anope::string &f_n, const Anope::string &u, const Anope::string &c_t);

	bool OnRequest(HTTP::Provider *, const Anope::string &, HTTP::Client *, HTTP::Message &, HTTP::Reply &) override;
};

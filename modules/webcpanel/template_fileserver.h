/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "modules/httpd.h"

/* A basic file server. Used for serving non-static non-binary content on disk. */
class TemplateFileServer
{
	Anope::string file_name;
 public:
	struct Replacements : std::multimap<Anope::string, Anope::string>
	{
		Anope::string& operator[](const Anope::string &key)
		{
			return insert(std::make_pair(key, ""))->second;
		}
	};

	TemplateFileServer(const Anope::string &f_n);

	void Serve(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &, Replacements &);
};


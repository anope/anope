/*
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
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


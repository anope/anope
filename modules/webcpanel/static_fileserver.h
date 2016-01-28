/*
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "modules/httpd.h"

/* A basic file server. Used for serving static content on disk. */
class StaticFileServer : public HTTPPage
{
	Anope::string file_name;
 public:
	StaticFileServer(const Anope::string &f_n, const Anope::string &u, const Anope::string &c_t);

	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &) anope_override;
};


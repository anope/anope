/*
 * (C) 2003-2019 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "modules/httpd.h"

namespace WebCPanel
{

class Index : public WebPanelPage
{
	static const int FLUSH_TIME = 60;

	Anope::hash_map<time_t> last_login_attempt;
	time_t last_clear;

 public:
	Index(const Anope::string &u) : WebPanelPage(u), last_clear(0) { }

	bool OnRequest(HTTPProvider *, const Anope::string &, HTTPClient *, HTTPMessage &, HTTPReply &) anope_override;
};

}

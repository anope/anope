/*
 * (C) 2003-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "webcpanel.h"
#include <fstream>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

StaticFileServer::StaticFileServer(const Anope::string &f_n, const Anope::string &u, const Anope::string &c_t) : HTTPPage(u, c_t), file_name(f_n)
{
}

bool StaticFileServer::OnRequest(HTTPProvider *server, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply)
{
	int fd = open((template_base + "/" + this->file_name).c_str(), O_RDONLY);
	if (fd < 0)
	{
		Log(LOG_NORMAL, "httpd") << "Error serving file " << page_name << " (" << (template_base + "/" + this->file_name) << "): " << strerror(errno);

		client->SendError(HTTP_PAGE_NOT_FOUND, "Page not found");
		return true;
	}

	reply.content_type = this->GetContentType();
	reply.headers["Cache-Control"] = "public";

	int i;
	char buffer[BUFSIZE];
	while ((i = read(fd, buffer, sizeof(buffer))) > 0)
		reply.Write(buffer, i);
	
	close(fd);
	return true;
}


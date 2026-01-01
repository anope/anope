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

#include "webcpanel.h"
#include <cerrno>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

StaticFileServer::StaticFileServer(const Anope::string &f_n, const Anope::string &u, const Anope::string &c_t) : HTTP::Page(u, c_t), file_name(f_n)
{
}

bool StaticFileServer::OnRequest(HTTP::Provider *server, const Anope::string &page_name, HTTP::Client *client, HTTP::Message &message, HTTP::Reply &reply)
{
	int fd = open((template_base + "/" + this->file_name).c_str(), O_RDONLY);
	if (fd < 0)
	{
		Log(LOG_NORMAL, "httpd") << "Error serving file " << page_name << " (" << (template_base + "/" + this->file_name) << "): " << strerror(errno);

		client->SendError(HTTP::PAGE_NOT_FOUND, "Page not found");
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

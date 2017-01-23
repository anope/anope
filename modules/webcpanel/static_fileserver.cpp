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
		Anope::Logger.Category("webcpanel").Log("Error serving file {0} ({1}/{2}): {3}", page_name, template_base, this->file_name, strerror(errno));

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


/*
 * Anope IRC Services
 *
 * Copyright (C) 2010-2016 Anope Team <team@anope.org>
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

#pragma once

#include "anope.h"
#include "threadengine.h"
#include "serialize.h"

namespace Mail
{
	extern CoreExport bool Send(User *from, NickServ::Account *to, ServiceBot *service, const Anope::string &subject, const Anope::string &message);
	extern CoreExport bool Send(NickServ::Account *to, const Anope::string &subject, const Anope::string &message);
	extern CoreExport bool Validate(const Anope::string &email);

	/* A email message being sent */
	class Message : public Thread
	{
	 private:
	 	Anope::string sendmail_path;
		Anope::string send_from;
		Anope::string mail_to;
		Anope::string addr;
		Anope::string subject;
		Anope::string message;
		bool dont_quote_addresses;

		bool success;
	 public:
	 	/** Construct this message. Once constructed call Thread::Start to launch the mail sending.
		 * @param sf Config->SendFrom
		 * @param mailto Name of person being mailed (u->nick, nc->display, etc)
		 * @param addr Destination address to mail
		 * @param subject Message subject
		 * @param message The actual message
		 */
		Message(const Anope::string &sf, const Anope::string &mailto, const Anope::string &addr, const Anope::string &subject, const Anope::string &message);

		~Message();

		/* Called from within the thread to actually send the mail */
		void Run() override;
	};

} // namespace Mail


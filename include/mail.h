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

#include "anope.h"
#include "threadengine.h"
#include "serialize.h"

namespace Mail
{
	extern CoreExport bool Send(User *from, NickCore *to, BotInfo *service, const Anope::string &subject, const Anope::string &message);
	extern CoreExport bool Send(NickCore *to, const Anope::string &subject, const Anope::string &message);
	extern CoreExport bool Validate(const Anope::string &email);

	/* A email message being sent */
	class Message final
		: public Thread
	{
	private:
		Anope::string error;
		Anope::string sendmail_path;
		Anope::string send_from;
		Anope::string mail_to;
		Anope::string addr;
		Anope::string subject;
		Anope::string message;
		Anope::string content_type;
		bool dont_quote_addresses;

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

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

#include <cerrno>

#include "services.h"
#include "mail.h"
#include "config.h"

Mail::Message::Message(const Anope::string &sf, const Anope::string &mailto, const Anope::string &a, const Anope::string &s, const Anope::string &m, const Mail::HeaderMap& hm)
	: Thread()
	, sendmail_path(Config->GetBlock("mail").Get<const Anope::string>("sendmailpath", "/usr/sbin/sendmail -it"))
	, send_from(sf)
	, mail_to(mailto)
	, addr(a)
	, subject(s)
	, message(m)
	, headers(hm)
	, dont_quote_addresses(Config->GetBlock("mail").Get<bool>("dontquoteaddresses"))
	, eol(Config->GetBlock("mail").Get<bool>("dontincludecr") ? "\n" : "\r\n")
{
	this->headers.emplace("Content-Transfer-Encoding", "8bit");
	this->headers.emplace("Content-Type", Config->GetBlock("mail").Get<const Anope::string>("content_type", "text/plain; charset=UTF-8"));

}

Mail::Message::~Message()
{
	if (error.empty())
		Log(LOG_NORMAL, "mail") << "Successfully delivered mail for " << mail_to << " (" << addr << ")";
	else
		Log(LOG_NORMAL, "mail") << "Error delivering mail for " << mail_to << " (" << addr << "): " << error;
}

void Mail::Message::Run()
{
	errno = 0;
	auto *pipe = popen(sendmail_path.c_str(), "w");
	if (!pipe)
	{
		error = strerror(errno);
		SetExitState();
		return;
	}

	fprintf(pipe, "From: %s%s", send_from.c_str(), eol.c_str());
	if (this->dont_quote_addresses)
		fprintf(pipe, "To: %s <%s>%s", mail_to.c_str(), addr.c_str(), eol.c_str());
	else
		fprintf(pipe, "To: \"%s\" <%s>%s", mail_to.replace_all_cs("\\", "\\\\").c_str(), addr.c_str(), eol.c_str());
	fprintf(pipe, "Subject: %s%s", subject.c_str(), eol.c_str());
	for (const auto &[hname, hvalue] : this->headers)
		fprintf(pipe, "%s: %s%s", hname.c_str(), hvalue.c_str(), eol.c_str());
	fprintf(pipe, "%s", eol.c_str());

	std::stringstream stream(message.str());
	for (Anope::string line; std::getline(stream, line.str()); )
		fprintf(pipe, "%s%s", line.c_str(), eol.c_str());
	fprintf(pipe, "%s", eol.c_str());

	auto result = pclose(pipe);
	if (result > 0)
		error = "Sendmail exited with code " + Anope::ToString(result);

	SetExitState();
}

bool Mail::Send(User *u, NickCore *nc, BotInfo *service, const Anope::string &subject, const Anope::string &message, const Mail::HeaderMap& hm)
{
	if (!nc || !service || subject.empty() || message.empty())
		return false;

	const auto &b = Config->GetBlock("mail");

	if (!u)
	{
		if (!b.Get<bool>("usemail") || b.Get<const Anope::string>("sendfrom").empty())
			return false;
		else if (nc->email.empty())
			return false;

		nc->lastmail = Anope::CurTime;
		auto *t = new Mail::Message(b.Get<const Anope::string>("sendfrom"), nc->display, nc->email, subject, message, hm);
		t->Start();
		return true;
	}
	else
	{
		if (!b.Get<bool>("usemail") || b.Get<const Anope::string>("sendfrom").empty())
			u->SendMessage(service, _("Services have been configured to not send mail."));
		else if (Anope::CurTime - u->lastmail < b.Get<time_t>("delay", "5m"))
		{
			const auto delay = b.Get<time_t>("delay") - (Anope::CurTime - u->lastmail);
			u->SendMessage(service, _("Please wait \002%s\002 and retry."), Anope::Duration(delay, u->Account()).c_str());
		}
		else if (nc->email.empty())
			u->SendMessage(service, _("Email for \002%s\002 is invalid."), nc->display.c_str());
		else
		{
			u->lastmail = nc->lastmail = Anope::CurTime;
			auto *t = new Mail::Message(b.Get<const Anope::string>("sendfrom"), nc->display, nc->email, subject, message, hm);
			t->Start();
			return true;
		}

		return false;
	}
}

bool Mail::Send(NickCore *nc, const Anope::string &subject, const Anope::string &message, const Mail::HeaderMap& hm)
{
	const auto &b = Config->GetBlock("mail");
	if (!b.Get<bool>("usemail") || b.Get<const Anope::string>("sendfrom").empty() || !nc || nc->email.empty() || subject.empty() || message.empty())
		return false;

	nc->lastmail = Anope::CurTime;
	auto *t = new Mail::Message(b.Get<const Anope::string>("sendfrom"), nc->display, nc->email, subject, message, hm);
	t->Start();

	return true;
}

/**
 * Checks whether we have a valid, common email address.
 * This is NOT entirely RFC compliant, and won't be so, because I said
 * *common* cases. ;) It is very unlikely that email addresses that
 * are really being used will fail the check.
 *
 * @param email Email to Validate
 * @return bool
 */
bool Mail::Validate(const Anope::string &email)
{
	bool has_period = false;

	static char specials[] = {'(', ')', '<', '>', '@', ',', ';', ':', '\\', '\"', '[', ']', ' '};

	if (email.empty())
		return false;
	Anope::string copy = email;

	size_t at = copy.find('@');
	if (at == Anope::string::npos)
		return false;
	Anope::string domain = copy.substr(at + 1);
	copy = copy.substr(0, at);

	/* Don't accept empty copy or domain. */
	if (copy.empty() || domain.empty())
		return false;

	/* Check for forbidden characters in the name */
	for (auto chr : copy)
	{
		if (chr <= 31 || chr >= 127)
			return false;
		for (auto special : specials)
		{
			if (chr == special)
				return false;
		}
	}

	/* Check for forbidden characters in the domain */
	for (unsigned i = 0, end = domain.length(); i < end; ++i)
	{
		if (domain[i] <= 31 || domain[i] >= 127)
			return false;
		for (auto special : specials)
		{
			if (domain[i] == special)
				return false;
		}
		if (domain[i] == '.')
		{
			if (!i || i == end - 1)
				return false;
			has_period = true;
		}
	}

	return has_period;
}

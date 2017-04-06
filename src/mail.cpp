/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
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

#include "services.h"
#include "mail.h"
#include "config.h"
#include "bots.h"
#include "protocol.h"
#include "modules/nickserv.h"

Mail::Message::Message(const Anope::string &sf, const Anope::string &mailto, const Anope::string &a, const Anope::string &s, const Anope::string &m) : Thread(), sendmail_path(Config->GetBlock("mail")->Get<Anope::string>("sendmailpath")), send_from(sf), mail_to(mailto), addr(a), subject(s), message(m), dont_quote_addresses(Config->GetBlock("mail")->Get<bool>("dontquoteaddresses")), success(false)
{
}

Mail::Message::~Message()
{
	if (success)
		Anope::Logger.Category("mail").Log(_("Successfully delivered mail for {0} ({1})"), mail_to, addr);
	else
		Anope::Logger.Category("mail").Log(_("Error delivering mail for {0} ({1})"), mail_to, addr);
}

void Mail::Message::Run()
{
	FILE *pipe = popen(sendmail_path.c_str(), "w");

	if (!pipe)
	{
		SetExitState();
		return;
	}

	fprintf(pipe, "From: %s\n", send_from.c_str());
	if (this->dont_quote_addresses)
		fprintf(pipe, "To: %s <%s>\n", mail_to.c_str(), addr.c_str());
	else
		fprintf(pipe, "To: \"%s\" <%s>\n", mail_to.c_str(), addr.c_str());
	fprintf(pipe, "Subject: %s\n", subject.c_str());
	fprintf(pipe, "%s", message.c_str());
	fprintf(pipe, "\n.\n");

	pclose(pipe);

	success = true;
	SetExitState();
}

bool Mail::Send(User *u, NickServ::Account *nc, ServiceBot *service, const Anope::string &subject, const Anope::string &message)
{
	if (!nc || !service || subject.empty() || message.empty())
		return false;

	Configuration::Block *b = Config->GetBlock("mail");

	if (!u)
	{
		if (!b->Get<bool>("usemail") || b->Get<Anope::string>("sendfrom").empty())
			return false;
		else if (nc->GetEmail().empty())
			return false;

		nc->SetLastMail(Anope::CurTime);
		Thread *t = new Mail::Message(b->Get<Anope::string>("sendfrom"), nc->GetDisplay(), nc->GetEmail(), subject, message);
		t->Start();
		return true;
	}
	else
	{
		if (!b->Get<bool>("usemail") || b->Get<Anope::string>("sendfrom").empty())
			u->SendMessage(service, _("Services have been configured to not send mail."));
		else if (Anope::CurTime - u->lastmail < b->Get<time_t>("delay"))
			u->SendMessage(service, _("Please wait \002%d\002 seconds and retry."), b->Get<time_t>("delay") - (Anope::CurTime - u->lastmail));
		else if (nc->GetEmail().empty())
			u->SendMessage(service, _("E-mail for \002%s\002 is invalid."), nc->GetDisplay().c_str());
		else
		{
			u->lastmail = Anope::CurTime;
			nc->SetLastMail(Anope::CurTime);
			Thread *t = new Mail::Message(b->Get<Anope::string>("sendfrom"), nc->GetDisplay(), nc->GetEmail(), subject, message);
			t->Start();
			return true;
		}

		return false;
	}
}

bool Mail::Send(NickServ::Account *nc, const Anope::string &subject, const Anope::string &message)
{
	Configuration::Block *b = Config->GetBlock("mail");
	if (!b->Get<bool>("usemail") || b->Get<Anope::string>("sendfrom").empty() || !nc || nc->GetEmail().empty() || subject.empty() || message.empty())
		return false;

	nc->SetLastMail(Anope::CurTime);
	Thread *t = new Mail::Message(b->Get<Anope::string>("sendfrom"), nc->GetDisplay(), nc->GetEmail(), subject, message);
	t->Start();

	return true;
}

/**
 * Checks whether we have a valid, common e-mail address.
 * This is NOT entirely RFC compliant, and won't be so, because I said
 * *common* cases. ;) It is very unlikely that e-mail addresses that
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
	for (unsigned i = 0, end = copy.length(); i < end; ++i)
	{
		if (copy[i] <= 31 || copy[i] >= 127)
			return false;
		for (unsigned int j = 0; j < 13; ++j)
			if (copy[i] == specials[j])
				return false;
	}

	/* Check for forbidden characters in the domain */
	for (unsigned i = 0, end = domain.length(); i < end; ++i)
	{
		if (domain[i] <= 31 || domain[i] >= 127)
			return false;
		for (unsigned int j = 0; j < 13; ++j)
			if (domain[i] == specials[j])
				return false;
		if (domain[i] == '.')
		{
			if (!i || i == end - 1)
				return false;
			has_period = true;
		}
	}

	return has_period;
}

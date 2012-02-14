/*
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */


#include "services.h"
#include "mail.h"
#include "config.h"

MailThread::MailThread(const Anope::string &mailto, const Anope::string &addr, const Anope::string &subject, const Anope::string &message) : Thread(), MailTo(mailto), Addr(addr), Subject(subject), Message(message), DontQuoteAddresses(Config->DontQuoteAddresses), Success(false)
{
}

MailThread::~MailThread()
{
	if (Success)
		Log(LOG_NORMAL, "mail") << "Successfully delivered mail for " << MailTo << " (" << Addr << ")";
	else
		Log(LOG_NORMAL, "mail") << "Error delivering mail for " << MailTo << " (" << Addr << ")";
}

void MailThread::Run()
{
	FILE *pipe = popen(Config->SendMailPath.c_str(), "w");

	if (!pipe)
	{
		SetExitState();
		return;
	}

	fprintf(pipe, "From: %s\n", Config->SendFrom.c_str());
	if (this->DontQuoteAddresses)
		fprintf(pipe, "To: %s <%s>\n", MailTo.c_str(), Addr.c_str());
	else
		fprintf(pipe, "To: \"%s\" <%s>\n", MailTo.c_str(), Addr.c_str());
	fprintf(pipe, "Subject: %s\n", Subject.c_str());
	fprintf(pipe, "%s", Message.c_str());
	fprintf(pipe, "\n.\n");

	pclose(pipe);

	Success = true;
	SetExitState();
}

bool Mail(User *u, NickCore *nc, BotInfo *service, const Anope::string &subject, const Anope::string &message)
{
	if (!u || !nc || !service || subject.empty() || message.empty())
		return false;

	if (!Config->UseMail)
		u->SendMessage(service, _("Services have been configured to not send mail."));
	else if (Anope::CurTime - u->lastmail < Config->MailDelay)
		u->SendMessage(service, _("Please wait \002%d\002 seconds and retry."), Config->MailDelay - (Anope::CurTime - u->lastmail));
	else if (nc->email.empty())
		u->SendMessage(service, _("E-mail for \002%s\002 is invalid."), nc->display.c_str());
	else
	{
		u->lastmail = nc->lastmail = Anope::CurTime;
		Thread *t = new MailThread(nc->display, nc->email, subject, message);
		t->Start();
		return true;
	}

	return false;
}

bool Mail(NickCore *nc, const Anope::string &subject, const Anope::string &message)
{
	if (!Config->UseMail || !nc || nc->email.empty() || subject.empty() || message.empty())
		return false;

	nc->lastmail = Anope::CurTime;
	Thread *t = new MailThread(nc->display, nc->email, subject, message);
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
bool MailValidate(const Anope::string &email)
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

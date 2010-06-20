#include "services.h"
#include "language.h"

MailThread::~MailThread()
{
	if (Success)
		Alog() << "Successfully delivered mail for " << MailTo << " (" << Addr << ")";
	else
		Alog() << "Error delivering mail for " << MailTo << " (" << Addr << ")";
}

void MailThread::Run()
{
	FILE *pipe = popen(Config.SendMailPath, "w");

	if (!pipe)
		return;

	fprintf(pipe, "From: %s\n", Config.SendFrom);
	if (Config.DontQuoteAddresses)
		fprintf(pipe, "To: %s <%s>\n", MailTo.c_str(), Addr.c_str());
	else
		fprintf(pipe, "To: \"%s\" <%s>\n", MailTo.c_str(), Addr.c_str());
	fprintf(pipe, "Subject: %s\n", Subject.c_str());
	fprintf(pipe, "%s", Message.c_str());
	fprintf(pipe, "\n.\n");

	pclose(pipe);

	Success = true;
}

bool Mail(User *u, NickRequest *nr, const std::string &service, const std::string &subject, const std::string &message)
{
	if (!u || !nr || subject.empty() || service.empty() || message.empty())
		return false;

	time_t t = time(NULL);

	if (!Config.UseMail)
		notice_lang(service.c_str(), u, MAIL_DISABLED);
	else if (t - u->lastmail < Config.MailDelay)
		notice_lang(service.c_str(), u, MAIL_DELAYED, t - u->lastmail);
	else if (!nr->email)
		notice_lang(service.c_str(), u, MAIL_INVALID, nr->nick);
	else
	{
		u->lastmail = nr->lastmail = t;
		threadEngine.Start(new MailThread(nr->nick, nr->email, subject, message));
		return true;
	}

	return false;
}

bool Mail(User *u, NickCore *nc, const std::string &service, const std::string &subject, const std::string &message)
{
	if (!u || !nc || subject.empty() || service.empty() || message.empty())
		return false;

	time_t t = time(NULL);

	if (!Config.UseMail)
		notice_lang(service.c_str(), u, MAIL_DISABLED);
	else if (t - u->lastmail < Config.MailDelay)
		notice_lang(service.c_str(), u, MAIL_DELAYED, t - u->lastmail);
	else if (!nc->email)
		notice_lang(service.c_str(), u, MAIL_INVALID, nc->display);
	else
	{
		u->lastmail = nc->lastmail = t;
		threadEngine.Start(new MailThread(nc->display, nc->email, subject, message));
		return true;
	}

	return false;
}

bool Mail(NickCore *nc, const std::string &subject, const std::string &message)
{
	if (!Config.UseMail || !nc || !nc->email || subject.empty() || message.empty())
		return false;

	nc->lastmail = time(NULL);
	threadEngine.Start(new MailThread(nc->display, nc->email, subject, message));

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
bool MailValidate(const std::string &email)
{
	bool has_period = false;
	char copy[BUFSIZE];

	static char specials[] = {'(', ')', '<', '>', '@', ',', ';', ':', '\\', '\"', '[', ']', ' '};

	if (email.empty())
		return false;
	strlcpy(copy, email.c_str(), sizeof(copy));

	char *domain = strchr(copy, '@');
	if (!domain)
		return false;
	*domain++ = '\0';

	/* Don't accept NULL copy or domain. */
	if (!*copy || !*domain)
		return false;

	/* Check for forbidden characters in the name */
	for (unsigned int i = 0; i < strlen(copy); ++i)
	{
		if (copy[i] <= 31 || copy[i] >= 127)
			return false;
		for (unsigned int j = 0; j < 13; ++j)
			if (copy[i] == specials[j])
				return false;
	}

	/* Check for forbidden characters in the domain */
	for (unsigned int i = 0; i < strlen(domain); ++i)
	{
		if (domain[i] <= 31 || domain[i] >= 127)
			return false;
		for (unsigned int j = 0; j < 13; ++j)
			if (domain[i] == specials[j])
				return false;
		if (domain[i] == '.')
		{
			if (!i || i == strlen(domain) - 1)
				return false;
			has_period = true;
		}
	}

	return has_period;
}

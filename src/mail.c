/* Mail utility routines.
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church. 
 * 
 * $Id$ 
 *
 */

#include "services.h"
#include "language.h"

/* Begins to send a mail. Must be followed by a MailEnd call.
 * Returns NULL if the call failed. Error messages are
 * automatically sent to the user.
 */

MailInfo *MailRegBegin(User * u, NickRequest * nr, char *subject,
                       char *service)
{
    if (!u || !nr || !subject)
        return NULL;

    if (!UseMail) {
        notice_lang(service, u, MAIL_DISABLED);
    } else if ((time(NULL) - u->lastmail < MailDelay)
               || (time(NULL) - nr->lastmail < MailDelay)) {
        notice_lang(service, u, MAIL_DELAYED, MailDelay);
    } else if (!nr->email) {
        notice_lang(service, u, MAIL_INVALID, nr->nick);
    } else {
        MailInfo *mail;

        mail = scalloc(sizeof(MailInfo), 1);
        mail->sender = u;
        mail->recipient = NULL;
        mail->recip = nr;

        if (!(mail->pipe = popen(SendMailPath, "w"))) {
            free(mail);
            notice_lang(service, u, MAIL_LATER);
            return NULL;
        }

        fprintf(mail->pipe, "From: %s\n", SendFrom);
        if (DontQuoteAddresses) {
            fprintf(mail->pipe, "To: %s <%s>\n", nr->nick, nr->email);
        } else {
            fprintf(mail->pipe, "To: \"%s\" <%s>\n", nr->nick, nr->email);
        }
        fprintf(mail->pipe, "Subject: %s\n", subject);
        return mail;
    }

    return NULL;
}

/* Begins to send a mail. Must be followed by a MailEnd call.
 * Returns NULL if the call failed. Error messages are
 * automatically sent to the user.
 */

MailInfo *MailBegin(User * u, NickCore * nc, char *subject, char *service)
{
    if (!u || !nc || !subject)
        return NULL;

    if (!UseMail) {
        notice_lang(service, u, MAIL_DISABLED);
    } else if (((time(NULL) - u->lastmail < MailDelay)
                || (time(NULL) - nc->lastmail < MailDelay))
               && !is_services_root(u)) {
        notice_lang(service, u, MAIL_DELAYED, MailDelay);
    } else if (!nc->email) {
        notice_lang(service, u, MAIL_INVALID, nc->display);
    } else {
        MailInfo *mail;

        mail = scalloc(sizeof(MailInfo), 1);
        mail->sender = u;
        mail->recipient = nc;
        mail->recip = NULL;

        if (!(mail->pipe = popen(SendMailPath, "w"))) {
            free(mail);
            notice_lang(service, u, MAIL_LATER);
            return NULL;
        }

        fprintf(mail->pipe, "From: %s\n", SendFrom);
        if (DontQuoteAddresses) {
            fprintf(mail->pipe, "To: %s <%s>\n", nc->display, nc->email);
        } else {
            fprintf(mail->pipe, "To: \"%s\" <%s>\n", nc->display,
                    nc->email);
        }
        fprintf(mail->pipe, "Subject: %s\n", subject);

        return mail;
    }

    return NULL;
}

/* new function to send memo mails */

MailInfo *MailMemoBegin(NickCore * nc)
{

    if (!nc)
        return NULL;

    if (!UseMail || !nc->email) {
        return NULL;

    } else {
        MailInfo *mail;

        mail = scalloc(sizeof(MailInfo), 1);
        mail->sender = NULL;
        mail->recipient = nc;
        mail->recip = NULL;

        if (!(mail->pipe = popen(SendMailPath, "w"))) {
            free(mail);
            return NULL;
        }

        fprintf(mail->pipe, "From: %s\n", SendFrom);
        if (DontQuoteAddresses) {
            fprintf(mail->pipe, "To: %s <%s>\n", nc->display, nc->email);
        } else {
            fprintf(mail->pipe, "To: \"%s\" <%s>\n", nc->display,
                    nc->email);
        }
        fprintf(mail->pipe, "Subject: %s\n",
                getstring2(NULL, MEMO_MAIL_SUBJECT));
        return mail;
    }
    return NULL;
}

/* Finish to send the mail. Cleanup everything. */

/*  - param checking modified because we don't
      have an user sending this mail.
      Certus, 02.04.2004 */

void MailEnd(MailInfo * mail)
{
    if (!mail || !mail->pipe)   /* removed sender check */
        return;

    if (!mail->recipient && !mail->recip)
        return;

    pclose(mail->pipe);

    if (mail->sender)           /* added sender check */
        mail->sender->lastmail = time(NULL);

    if (mail->recipient)
        mail->recipient->lastmail = time(NULL);
    else
        mail->recip->lastmail = time(NULL);


    free(mail);
}

/* Resets the MailDelay protection */

void MailReset(User * u, NickCore * nc)
{
    if (u)
        u->lastmail = 0;
    if (nc)
        nc->lastmail = 0;
}

/* Checks whether we have a valid, common e-mail address.
 * This is NOT entirely RFC compliant, and won't be so, because I said 
 * *common* cases. ;) It is very unlikely that e-mail addresses that
 * are really being used will fail the check.
 *
 * FIXME: rewrite this a bit cleaner.
 */

int MailValidate(const char *email)
{
    int i, j, has_period = 0, len;
    char copy[BUFSIZE], *domain;

    static char specials[] =
        { '(', ')', '<', '>', '@', ',', ';', ':', '\\', '\"', '[', ']',
        ' '
    };

    if (!email)
        return 0;
    strcpy(copy, email);

    domain = strchr(copy, '@');
    if (!domain)
        return 0;
    *domain = '\0';
    domain++;

    /* Don't accept NULL copy or domain. */
    if (*copy == 0 || *domain == 0)
        return 0;

    /* Check for forbidden characters in the name */
    for (i = 0; i < strlen(copy); i++) {

        if (copy[i] <= 31 || copy[i] >= 127)
            return 0;
        for (j = 0; j < 13; j++)
            if (copy[i] == specials[j])
                return 0;
    }

    /* Check for forbidden characters in the domain, and if it seems to be valid. */
    for (i = 0; i < (len = strlen(domain)); i++) {
        if (domain[i] <= 31 || domain[i] >= 127)
            return 0;
        for (j = 0; j < 13; j++)
            if (domain[i] == specials[j])
                return 0;
        if (domain[i] == '.') {
            if (i == 0 || i == len - 1)
                return 0;
            has_period = 1;
        }
    }

    if (!has_period)
        return 0;

    return 1;
}

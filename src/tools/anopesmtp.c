/* smtp stuff handler for win32.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * Written by Dominick Meglio <codemastr@unrealircd.com>
 * *nix port by Trystan Scott Lee <trystan@nomadirc.net>
 *
 */

#include "smtp.h"

static FILE *logfile;
static int curday = 0;

/*************************************************************************/

#ifdef _WIN32
int strcasecmp(const char *s1, const char *s2)
{
    register int c;

    while ((c = tolower(*s1)) == tolower(*s2)) {
        if (c == 0)
            return 0;
        s1++;
        s2++;
    }
    if (c < tolower(*s2))
        return -1;
    return 1;
}
#endif

static int get_logname(char *name, int count, struct tm *tm)
{

    char timestamp[32];

    if (!tm) {
        time_t t;

        time(&t);
        tm = localtime(&t);
    }

    strftime(timestamp, count, "%Y%m%d", tm);
    snprintf(name, count, "logs/%s.%s", "anopesmtp", timestamp);
    curday = tm->tm_yday;

    return 1;
}

/*************************************************************************/

/* Close the log file. */

void close_log(void)
{
    if (!logfile)
        return;
    fclose(logfile);
    logfile = NULL;
}

/*************************************************************************/

static void remove_log(void)
{
    time_t t;
    struct tm tm;

    char name[PATH_MAX];

    time(&t);
    t -= (60 * 60 * 24 * 30);
    tm = *localtime(&t);

    if (!get_logname(name, sizeof(name), &tm))
        return;
    unlink(name);
}

/*************************************************************************/

/* Open the log file.  Return -1 if the log file could not be opened, else
 * return 0. */

int open_log(void)
{
    char name[PATH_MAX];

    if (logfile)
        return 0;

    if (!get_logname(name, sizeof(name), NULL))
        return 0;
    logfile = fopen(name, "a");

    if (logfile)
        setbuf(logfile, NULL);
    return logfile != NULL ? 0 : -1;
}

/*************************************************************************/

static void checkday(void)
{
    time_t t;
    struct tm tm;

    time(&t);
    tm = *localtime(&t);

    if (curday != tm.tm_yday) {
        close_log();
        remove_log();
        open_log();
    }
}

/*************************************************************************/

/* Log stuff to the log file with a datestamp.  Note that errno is
 * preserved by this routine and log_perror().
 */

void alog(const char *fmt, ...)
{
    va_list args;
    time_t t;
    struct tm tm;
    char buf[256];
    int errno_save = errno;

    if (!smtp_debug) {
        return;
    }

    checkday();

    if (!fmt) {
        return;
    }

    va_start(args, fmt);
    time(&t);
    tm = *localtime(&t);
    strftime(buf, sizeof(buf) - 1, "[%b %d %H:%M:%S %Y] ", &tm);
    if (logfile) {
        fputs(buf, logfile);
        vfprintf(logfile, fmt, args);
        fputc('\n', logfile);
    }
    va_end(args);
    errno = errno_save;
}

/*************************************************************************/

/* Remove a trailing \r\n */
char *strip(char *buf)
{
    char *c;
    if ((c = strchr(buf, '\n')))
        *c = 0;
    if ((c = strchr(buf, '\r')))
        *c = 0;
    return buf;
}

/*************************************************************************/

/* Convert a trailing \n to \r\n 
 * The caller must free the allocated memory
 */
char *lftocrlf(char *buf)
{
    char *result = malloc(strlen(buf) + 2);
    strip(buf);
    strcpy(result, buf);
    strcat(result, "\r\n");
    return result;
}

/*************************************************************************/

/* Add a header to the list */
void smtp_add_header(char *header)
{
    struct smtp_header *head = malloc(sizeof(struct smtp_header));

    head->header = lftocrlf(header);
    head->next = NULL;

    if (!mail.smtp_headers) {
        mail.smtp_headers = head;
    }
    if (mail.smtp_headers_tail) {
        mail.smtp_headers_tail->next = head;
    }
    mail.smtp_headers_tail = head;
}

/*************************************************************************/

/* Is the buffer a header? */
int smtp_is_header(char *buf)
{
    char *tmp = strchr(buf, ' ');

    if (!tmp)
        return 0;

    if (*(tmp - 1) == ':')
        return 1;
    return 0;
}

/*************************************************************************/

/* Parse a header into a name and value */
void smtp_parse_header(char *buf, char **header, char **value)
{
    strip(buf);

    *header = strtok(buf, " ");
    *value = strtok(NULL, "");
    if (*header)
        (*header)[strlen(*header) - 1] = 0;
}

/*************************************************************************/

/* Have we reached the end of input? */
int smtp_is_end(char *buf)
{
    if (*buf == '.')
        if (*(buf + 1) == '\r' || *(buf + 1) == '\n')
            return 1;

    return 0;
}

/*************************************************************************/

/* Set who the email is from */
void smtp_set_from(char *from)
{
    mail.from = strdup(from);
}

/*************************************************************************/

/* Set who the email is to */
void smtp_set_to(char *to)
{
    char *c;

    if ((c = strrchr(to, '<')) && *(c + 1)) {
        to = c + 1;
        to[strlen(to) - 1] = 0;
    }
    mail.to = strdup(to);
}

/*************************************************************************/

/* Add a line of body text */
void smtp_add_body_line(char *line)
{
    struct smtp_body_line *body;

    body = malloc(sizeof(struct smtp_body_line));

    body->line = lftocrlf(line);
    body->next = NULL;

    if (!mail.smtp_body)
        mail.smtp_body = body;
    if (mail.smtp_body_tail)
        mail.smtp_body_tail->next = body;
    mail.smtp_body_tail = body;

}

/*************************************************************************/

/* Establish a connection to the SMTP server */
int smtp_connect(char *host, unsigned short port)
{
    struct sockaddr_in addr;

    if ((mail.sock = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
        return 0;

    if ((addr.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) {
        struct hostent *hent;
        if (!(hent = gethostbyname(host)))
            return 0;
        memcpy(&addr.sin_addr, hent->h_addr, hent->h_length);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port ? port : 25);
    if (connect(mail.sock, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
        ano_sockclose(mail.sock);
        return 0;
    }

    return 1;
}

/*************************************************************************/

/* Send a line of text */
int smtp_send(char *text)
{
    int result = ano_sockwrite(mail.sock, text, strlen(text));

    alog("SMTP: sent %s",text);

    if (result == SOCKET_ERROR)
        ano_sockclose(mail.sock);

    return result;
}

/*************************************************************************/

/* Read a line of text */
int smtp_read(char *buf, int len)
{
    int result;

    memset(buf, 0, len);
    result = ano_sockread(mail.sock, buf, len);

    if (result == SOCKET_ERROR)
        ano_sockclose(mail.sock);

    return result;
}

/*************************************************************************/

/* Retrieve a response code */
int smtp_get_code(char *text)
{
    char *tmp = strtok(text, " ");

    if (!tmp)
        return 0;

    return atol(tmp);
}

/*************************************************************************/

/* Send the email */
int smtp_send_email()
{
    char buf[1024];
    struct smtp_header *head;
    struct smtp_body_line *body;
    int code;
    int skip_done = 0;

    if (!smtp_read(buf, 1024)) {
	    alog("SMTP: error reading buffer");
        return 0;
    }

    code = smtp_get_code(buf);
    if (code != 220) {
	    alog("SMTP: error expected code 220 got %d",code);
        return 0;
    }

    if (!smtp_send("HELO anope\r\n")) {
	    alog("SMTP: error writting to socket");
        return 0;
    } 

    if (!smtp_read(buf, 1024)) {
	alog("SMTP: error reading buffer");
        return 0;
    }

    code = smtp_get_code(buf);
    if (code != 250) {
	    alog("SMTP: error expected code 250 got %d",code);
        return 0;
    }

    strcpy(buf, "MAIL FROM: <");
    strcat(buf, mail.from);
    strcat(buf, ">\r\n");

    if (!smtp_send(buf)) {
	    alog("SMTP: error writting to socket");
        return 0;
    }

    if (!smtp_read(buf, 1024)) {
	    alog("SMTP: error reading buffer");
        return 0;
    }

    code = smtp_get_code(buf);
    if (code != 250)
        return 0;

    strcpy(buf, "RCPT TO: <");
    strcat(buf, mail.to);
    strcat(buf, ">\r\n");

    if (!smtp_send(buf)) {
	    alog("SMTP: error writting to socket");
        return 0;
    }

    if (!smtp_read(buf, 1024)) {
	    alog("SMTP: error reading buffer");
        return 0;
    }

    code = smtp_get_code(buf);
    if (smtp_get_code(buf) != 250) {
	    alog("SMTP: error expected code 250 got %d",code);
        return 0;
    }

    if (!smtp_send("DATA\r\n")) {
	    alog("SMTP: error writting to socket");
        return 0;
    }

    if (!smtp_read(buf, 1024)) {
	    alog("SMTP: error reading buffer");
        return 0;
    }

    code = smtp_get_code(buf);
    if (code != 354) {
	    alog("SMTP: error expected code 354 got %d",code);
        return 0;
    }

    for (head = mail.smtp_headers; head; head = head->next) {
        if (!smtp_send(head->header)) {
	        alog("SMTP: error writting to socket");
            return 0;
 	    }
    }

    if (!smtp_send("\r\n")) {
	    alog("SMTP: error writting to socket");
        return 0;
    }

    for (body = mail.smtp_body; body; body = body->next) {
        if (skip_done) {
            if (!smtp_send(body->line)) {
	            alog("SMTP: error writting to socket");
                return 0;
            }
        } else {
            skip_done = 1;
        }
    }

    if (!smtp_send("\r\n.\r\n")) {
	    alog("SMTP: error writting to socket");
        return 0;
    }

    return 1;
}

/*************************************************************************/

void smtp_disconnect()
{
    smtp_send("QUIT\r\n");
    ano_sockclose(mail.sock);
}

/*************************************************************************/

void mail_cleanup()
{
	struct smtp_header *headers, *nexth;
	struct smtp_body_line *body, *nextb;
	
	if (mail.from)
		free(mail.from);
	if (mail.to)
		free(mail.to);
	
	headers = mail.smtp_headers;
	while (headers) {
		nexth = headers->next;
		free(headers->header);
		free(headers);
		headers = nexth;
	}
	
	body = mail.smtp_body;
	while (body) {
		nextb = body->next;
		free(body->line);
		free(body);
		body = nextb;
	}
}

/*************************************************************************/

int main(int argc, char *argv[])
{
    char buf[8192];
/*	These are somehow unused - why are they here? -GD

    struct smtp_body_line *b;
    struct smtp_header *h;
*/
    int headers_done = 0;
/* Win32 stuff */
#ifdef _WIN32
    WSADATA wsa;
#endif
    char *server, *aport;
    short port;

    if (argc == 1)
        return 0;

    server = strtok(argv[1], ":");
    if ((aport = strtok(NULL, ""))) {
        port = atoi(aport);
    } else {
        port = 25;
    }

   if (!server) {
        alog("No Server");
	/* Bad, bad, bad. This was a eturn from main with no value! -GD */
        return 0;
    } else {
        alog("SMTP: server %s port %d",server,port);
    }

    memset(&mail, 0, sizeof(mail));

/* The WSAStartup function initiates use of WS2_32.DLL by a process. */
/* guessing we can skip it under *nix */
#ifdef _WIN32
    if (WSAStartup(MAKEWORD(1, 1), &wsa) != 0)
        return 0;
#endif

    /* Read the message and parse it */
    while (fgets(buf, 8192, stdin)) {
        if (smtp_is_header(buf) && !headers_done) {
            char *header, *value;
            smtp_add_header(buf);
            smtp_parse_header(buf, &header, &value);
            if (!strcasecmp(header, "from")) {
		        alog("SMTP: from: %s",value);
                smtp_set_from(value);
            } else if (!strcasecmp(header, "to")) {
        		alog("SMTP: to: %s",value);
                smtp_set_to(value);
            } else if (smtp_is_end(buf)) {
                break;
            } else {
                headers_done = 1;
                smtp_add_body_line(buf);
            }
        } else {
                smtp_add_body_line(buf);
        }
    }

    if (!smtp_connect(server, port)) {
	    alog("SMTP: failed to connect to %s:%d",server, port);
		mail_cleanup();
        return 0;
    }
    if (!smtp_send_email()) {
	    alog("SMTP: error during sending of mail");
		mail_cleanup();
        return 0;
    }
    smtp_disconnect();
	mail_cleanup();
	
    return 1;
}

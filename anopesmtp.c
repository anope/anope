/* smtp stuff handler for win32.
 *
 * (C) 2003 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * Written by Dominick Meglio <codemastr@unrealircd.com>
 *
 */

#include <winsock.h>
#include <stdio.h>

/* Data structures */
struct smtp_header {
    char *header;
    struct smtp_header *next;
};

struct smtp_body_line {
    char *line;
    struct smtp_body_line *next;
};

struct smtp_message {
    struct smtp_header *smtp_headers, *smtp_headers_tail;
    struct smtp_body_line *smtp_body, *smtp_body_tail;
    char *from;
    char *to;
    SOCKET sock;
};

struct smtp_message mail;

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

/* Add a header to the list */
void smtp_add_header(char *header)
{
    struct smtp_header *head = malloc(sizeof(struct smtp_header));

    head->header = lftocrlf(header);
    head->next = NULL;

    if (!mail.smtp_headers)
        mail.smtp_headers = head;
    if (mail.smtp_headers_tail)
        mail.smtp_headers_tail->next = head;
    mail.smtp_headers_tail = head;
}

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

/* Parse a header into a name and value */
void smtp_parse_header(char *buf, char **header, char **value)
{
    strip(buf);

    *header = strtok(buf, " ");
    *value = strtok(NULL, "");
    if (*header)
        (*header)[strlen(*header) - 1] = 0;
}

/* Have we reached the end of input? */
int smtp_is_end(char *buf)
{
    if (*buf == '.')
        if (*(buf + 1) == '\r' || *(buf + 1) == '\n')
            return 1;

    return 0;
}

/* Set who the email is from */
void smtp_set_from(char *from)
{
    mail.from = strdup(from);
}

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

/* Add a line of body text */
void smtp_add_body_line(char *line)
{
    struct smtp_body_line *body = malloc(sizeof(struct smtp_body_line));

    body->line = lftocrlf(line);
    body->next = NULL;

    if (!mail.smtp_body)
        mail.smtp_body = body;
    if (mail.smtp_body_tail)
        mail.smtp_body_tail->next = body;
    mail.smtp_body_tail = body;
}

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
    if (connect
        (mail.sock, (struct sockaddr *) &addr,
         sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
        closesocket(mail.sock);
        return 0;
    }

    return 1;
}

/* Send a line of text */
int smtp_send(char *text)
{
    int result = send(mail.sock, text, strlen(text), 0);

    if (result == SOCKET_ERROR)
        closesocket(mail.sock);

    return result;
}

/* Read a line of text */
int smtp_read(char *buf, int len)
{
    int result;

    memset(buf, 0, len);
    result = recv(mail.sock, buf, len, 0);

    if (result == SOCKET_ERROR)
        closesocket(mail.sock);

    return result;
}

/* Retrieve a response code */
int smtp_get_code(char *text)
{
    char *tmp = strtok(text, " ");

    if (!tmp)
        return 0;

    return atol(tmp);
}

/* Send the email */
int smtp_send_email()
{
    char buf[1024];
    struct smtp_header *head;
    struct smtp_body_line *body;

    if (!smtp_read(buf, 1024))
        return 0;

    if (smtp_get_code(buf) != 220)
        return 0;

    if (!smtp_send("HELO anope\r\n"))
        return 0;

    if (!smtp_read(buf, 1024))
        return 0;

    if (smtp_get_code(buf) != 250)
        return 0;

    strcpy(buf, "MAIL FROM: <");
    strcat(buf, mail.from);
    strcat(buf, ">\r\n");

    if (!smtp_send(buf))
        return 0;

    if (!smtp_read(buf, 1024))
        return 0;

    if (smtp_get_code(buf) != 250)
        return 0;

    strcpy(buf, "RCPT TO: <");
    strcat(buf, mail.to);
    strcat(buf, ">\r\n");

    if (!smtp_send(buf))
        return 0;

    if (!smtp_read(buf, 1024))
        return 0;

    if (smtp_get_code(buf) != 250)
        return 0;


    if (!smtp_send("DATA\r\n"))
        return 0;

    if (!smtp_read(buf, 1024))
        return 0;

    if (smtp_get_code(buf) != 354)
        return 0;

    for (head = mail.smtp_headers; head; head = head->next) {
        if (!smtp_send(head->header))
            return 0;
    }

    if (!smtp_send("\r\n"))
        return 0;

    for (body = mail.smtp_body; body; body = body->next) {
        if (!smtp_send(body->line))
            return 0;
    }

    if (!smtp_send("\r\n.\r\n"))
        return 0;

    return 1;
}

void smtp_disconnect()
{
    smtp_send("QUIT\r\n");
    closesocket(mail.sock);
}

int main(int argc, char *argv[])
{
    char buf[8192];
    struct smtp_body_line *b;
    struct smtp_header *h;
    int headers_done = 0;
    WSADATA wsa;
    char *server, *aport;
    short port;


    if (argc == 1)
        return 0;

    server = strtok(argv[1], ":");
    if ((aport = strtok(NULL, "")))
        port = atoi(aport);
    else
        port = 25;


    memset(&mail, 0, sizeof(mail));

    if (WSAStartup(MAKEWORD(1, 1), &wsa) != 0)
        return 0;

    /* Read the message and parse it */
    while (fgets(buf, 8192, stdin)) {
        if (smtp_is_header(buf) && !headers_done) {
            char *header, *value;
            smtp_add_header(buf);
            smtp_parse_header(buf, &header, &value);
            if (!stricmp(header, "from"))
                smtp_set_from(value);
            else if (!stricmp(header, "to"))
                smtp_set_to(value);
        } else if (smtp_is_end(buf))
            break;
        else {
            headers_done = 1;
            smtp_add_body_line(buf);
        }
    }

    if (!smtp_connect(server, port))
        return 0;
    if (!smtp_send_email())
        return 0;
    smtp_disconnect();
}

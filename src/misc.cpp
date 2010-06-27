
/* Miscellaneous routines.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "language.h"
#include "hashcomp.h" // If this gets added to services.h or someplace else later, remove it from here -- CyberBotX

/* Cheaper than isspace() or isblank() */
#define issp(c) ((c) == 32)

struct arc4_stream
{
	uint8 i;
	uint8 j;
	uint8 s[256];
} rs;

/*************************************************************************/

/** Check if a file exists
 * @param filename The file
 * @return true if the file exists, false if it doens't
 */
bool IsFile(const std::string &filename)
{
	struct stat fileinfo;
	if (!stat(filename.c_str(), &fileinfo))
		return true;

	return false;
}

/**
 * toupper:  Like the ANSI functions, but make sure we return an
 *           int instead of a (signed) char.
 * @param c Char
 * @return int
 */
int toupper(char c)
{
	if (islower(c))
		return static_cast<int>(c) - ('a' - 'A');
	else
		return static_cast<int>(c);
}

/*************************************************************************/

/**
 * tolower:  Like the ANSI functions, but make sure we return an
 *           int instead of a (signed) char.
 * @param c Char
 * @return int
 */
int tolower(char c)
{
	if (isupper(c))
		return static_cast<int>(c) + ('a' - 'A');
	else
		return static_cast<int>(c);
}

/*************************************************************************/

/**
 * Simple function to convert binary data to hex.
 * Taken from hybrid-ircd ( http://ircd-hybrid.com/ )
 */
void binary_to_hex(unsigned char *bin, char *hex, int length)
{
	static const char trans[] = "0123456789ABCDEF";
	int i;

	for (i = 0; i < length; ++i)
	{
		hex[i << 1] = trans[bin[i] >> 4];
		hex[(i << 1) + 1] = trans[bin[i] & 0xf];
	}

	hex[i << 1] = '\0';
}


/*************************************************************************/

/**
 * strscpy:  Copy at most len-1 characters from a string to a buffer, and
 *		   add a null terminator after the last character copied.
 * @param d Buffer to copy into
 * @param s Data to copy int
 * @param len Length of data
 * @return updated buffer
 */
char *strscpy(char *d, const char *s, size_t len)
{
	char *d_orig = d;

	if (!len)
		return d;
	while (--len && (*d++ = *s++));
	*d = '\0';
	return d_orig;
}

/*************************************************************************/

/**
 * stristr:  Search case-insensitively for string s2 within string s1,
 *		   returning the first occurrence of s2 or NULL if s2 was not
 *		   found.
 * @param s1 String 1
 * @param s2 String 2
 * @return first occurrence of s2
 */
const char *stristr(const char *s1, const char *s2)
{
	register const char *s = s1, *d = s2;

	while (*s1)
	{
		if (tolower(*s1) == tolower(*d))
		{
			++s1;
			++d;
			if (!*d)
				return s;
		}
		else
		{
			s = ++s1;
			d = s2;
		}
	}
	return NULL;
}

/*************************************************************************/

/**
 * strnrepl:  Replace occurrences of `old' with `new' in string `s'.  Stop
 *            replacing if a replacement would cause the string to exceed
 *            `size' bytes (including the null terminator).  Return the
 *            string.
 * @param s String
 * @param size size of s
 * @param old character to replace
 * @param newstr character to replace with
 * @return updated s
 */
char *strnrepl(char *s, int32 size, const char *old, const char *newstr)
{
	char *ptr = s;
	int32 left = strlen(s);
	int32 avail = size - (left + 1);
	int32 oldlen = strlen(old);
	int32 newlen = strlen(newstr);
	int32 diff = newlen - oldlen;

	while (left >= oldlen)
	{
		if (strncmp(ptr, old, oldlen))
		{
			--left;
			++ptr;
			continue;
		}
		if (diff > avail)
			break;
		if (diff)
			memmove(ptr + oldlen + diff, ptr + oldlen, left + 1 - oldlen);
		strncpy(ptr, newstr, newlen);
		ptr += newlen;
		left -= oldlen;
	}
	return s;
}

/*************************************************************************/

/**
 * merge_args:  Take an argument count and argument vector and merge them
 *              into a single string in which each argument is separated by
 *              a space.
 * @param int Number of Args
 * @param argv Array
 * @return string of the merged array
 */
const char *merge_args(int argc, const char **argv)
{
	int i;
	static char s[4096];
	char *t;

	t = s;
	for (i = 0; i < argc; ++i)
		t += snprintf(t, sizeof(s) - (t - s), "%s%s", *argv++, i < argc - 1 ? " " : "");
	return s;
}

/*
 * XXX: temporary "safe" version to avoid casting, it's still ugly.
 */
const char *merge_args(int argc, char **argv)
{
	int i;
	static char s[4096];
	char *t;

	t = s;
	for (i = 0; i < argc; ++i)
		t += snprintf(t, sizeof(s) - (t - s), "%s%s", *argv++, i < argc - 1 ? " " : "");
	return s;
}

/*************************************************************************/

NumberList::NumberList(const std::string &list, bool descending) : desc(descending)
{
	char *error;
	commasepstream sep(list);
	std::string token;

	sep.GetToken(token);
	if (token.empty())
		token = list;
	do
	{
		size_t t = token.find('-');

		if (t == std::string::npos)
		{
			unsigned num = strtol(token.c_str(), &error, 10);
			if (!*error)
				numbers.insert(num);
			else
			{
				if (!this->InvalidRange(list))
				{
					delete this;
					return;
				}
			}
		}
		else
		{
			char *error2;
			unsigned num1 = strtol(token.substr(0, t).c_str(), &error, 10);
			unsigned num2 = strtol(token.substr(t + 1).c_str(), &error2, 10);
			if (!*error && !*error2)
			{
				for (unsigned i = num1; i <= num2; ++i)
					numbers.insert(i);
			}
			else
			{
				if (!this->InvalidRange(list))
				{
					delete this;
					return;
				}
			}
		}
	} while (sep.GetToken(token));
}

NumberList::~NumberList()
{
}

void NumberList::Process()
{
	if (this->desc)
	{
		for (std::set<unsigned>::reverse_iterator it = numbers.rbegin(), it_end = numbers.rend(); it != it_end; ++it)
			this->HandleNumber(*it);
	}
	else
	{
		for (std::set<unsigned>::iterator it = numbers.begin(), it_end = numbers.end(); it != it_end; ++it)
			this->HandleNumber(*it);
	}

	delete this;
}

void NumberList::HandleNumber(unsigned)
{
}

bool NumberList::InvalidRange(const std::string &)
{
	return true;
}

/**
 * dotime:  Return the number of seconds corresponding to the given time
 *          string.  If the given string does not represent a valid time,
 *          return -1.
 *
 *          A time string is either a plain integer (representing a number
 *          of seconds), or an integer followed by one of these characters:
 *          "s" (seconds), "m" (minutes), "h" (hours), or "d" (days).
 * @param s String to convert
 * @return time_t
 */
time_t dotime(const char *s)
{
	int amount;

	if (!s || !*s)
		return -1;

	amount = strtol(s, const_cast<char **>(&s), 10);
	if (*s)
	{
		switch (*s)
		{
			case 's':
				return amount;
			case 'm':
				return amount * 60;
			case 'h':
				return amount * 3600;
			case 'd':
				return amount * 86400;
			case 'w':
				return amount * 86400 * 7;
			case 'y':
				return amount * 86400 * 365;
			default:
				return -1;
		}
	}
	else
		return amount;
}

/*************************************************************************/

/**
 * Expresses in a string the period of time represented by a given amount
 * of seconds (with days/hours/minutes).
 * @param na Nick Alias
 * @param buf buffer to store result into
 * @param bufsize Size of the buffer
 * @param seconds time in seconds
 * @return buffer
 */
const char *duration(NickCore *nc, char *buf, int bufsize, time_t seconds)
{
	int days = 0, hours = 0, minutes = 0;
	int need_comma = 0;

	char buf2[64], *end;
	const char *comma = getstring(nc, COMMA_SPACE);

	/* We first calculate everything */
	days = seconds / 86400;
	seconds -= (days * 86400);
	hours = seconds / 3600;
	seconds -= (hours * 3600);
	minutes = seconds / 60;

	if (!days && !hours && !minutes)
		snprintf(buf, bufsize, getstring(nc, seconds <= 1 ? DURATION_SECOND : DURATION_SECONDS), seconds);
	else
	{
		end = buf;
		if (days)
		{
			snprintf(buf2, sizeof(buf2), getstring(nc, days == 1 ? DURATION_DAY : DURATION_DAYS), days);
			end += snprintf(end, bufsize - (end - buf), "%s", buf2);
			need_comma = 1;
		}
		if (hours)
		{
			snprintf(buf2, sizeof(buf2), getstring(nc, hours == 1 ? DURATION_HOUR : DURATION_HOURS), hours);
			end += snprintf(end, bufsize - (end - buf), "%s%s", need_comma ? comma : "", buf2);
			need_comma = 1;
		}
		if (minutes)
		{
			snprintf(buf2, sizeof(buf2), getstring(nc, minutes == 1 ? DURATION_MINUTE : DURATION_MINUTES), minutes);
			end += snprintf(end, bufsize - (end - buf), "%s%s", need_comma ? comma : "", buf2);
			need_comma = 1;
		}
	}

	return buf;
}

/*************************************************************************/

/**
 * Generates a human readable string of type "expires in ..."
 * @param na Nick Alias
 * @param buf buffer to store result into
 * @param bufsize Size of the buffer
 * @param seconds time in seconds
 * @return buffer
 */
const char *expire_left(NickCore *nc, char *buf, int len, time_t expires)
{
	time_t now = time(NULL);

	if (!expires)
		strlcpy(buf, getstring(nc, NO_EXPIRE), len);
	else if (expires <= now)
		strlcpy(buf, getstring(nc, EXPIRES_SOON), len);
	else
	{
		time_t diff = expires - now + 59;

		if (diff >= 86400)
		{
			int days = diff / 86400;
			snprintf(buf, len, getstring(nc, days == 1 ? EXPIRES_1D : EXPIRES_D), days);
		}
		else
		{
			if (diff <= 3600)
			{
				int minutes = diff / 60;
				snprintf(buf, len, getstring(nc, minutes == 1 ? EXPIRES_1M : EXPIRES_M), minutes);
			}
			else
			{
				int hours = diff / 3600, minutes;
				diff -= hours * 3600;
				minutes = diff / 60;
				snprintf(buf, len, getstring(nc, hours == 1 && minutes == 1 ? EXPIRES_1H1M : (hours == 1 && minutes != 1 ? EXPIRES_1HM : (hours != 1 && minutes == 1 ? EXPIRES_H1M : EXPIRES_HM))), hours, minutes);
			}
		}
	}

	return buf;
}


/*************************************************************************/

/**
 * Validate the host
 * shortname  =  ( letter / digit ) *( letter / digit / "-" ) *( letter / digit )
 * hostname   =  shortname *( "." shortname )
 * ip4addr	=  1*3digit "." 1*3digit "." 1*3digit "." 1*3digit
 * @param host = string to check
 * @param type = format, 1 = ip4addr, 2 = hostname
 * @return 1 if a host is valid, 0 if it isnt.
 */
int doValidHost(const char *host, int type)
{
	int idx = 0;
	int len = 0;
	int sec_len = 0;
	int dots = 1;
	if (type != 1 && type != 2)
		return 0;
	if (!host)
		return 0;

	len = strlen(host);

	if (len > Config.HostLen)
		return 0;

	switch (type)
	{
		case 1:
			for (idx = 0; idx < len; ++idx)
			{
				if (isdigit(host[idx]))
				{
					if (sec_len < 3)
						++sec_len;
					else
						return 0;
				}
				else
				{
					if (!idx)
						return 0; /* cant start with a non-digit */
					if (host[idx] != '.')
						return 0; /* only . is a valid non-digit */
					if (sec_len > 3)
						return 0; /* sections cant be more than 3 digits */
					sec_len = 0;
					++dots;
				}
			}
			if (dots != 4)
				return 0;
			break;
		case 2:
			dots = 0;
			for (idx = 0; idx < len; ++idx)
			{
				if (!isalnum(host[idx]))
				{
					if (!idx)
						return 0;
					if (host[idx] != '.' && host[idx] != '-')
						return 0;
					if (host[idx] == '.')
						++dots;
				}
			}
			if (host[len - 1] == '.')
				return 0;
			/**
			 * Ultimate3 dosnt like a non-dotted hosts at all, nor does unreal,
			 * so just dont allow them.
			 */
			if (!dots)
				return 0;

			break;
	}
	return 1;
}

/*************************************************************************/

/**
 * Front end to doValidHost
 * @param host = string to check
 * @param type = format, 1 = ip4addr, 2 = hostname
 * @return 1 if a host is valid, 0 if it isnt.
 */
int isValidHost(const char *host, int type)
{
	int status = 0;
	if (type == 3)
	{
		if (!(status = doValidHost(host, 1)))
			status = doValidHost(host, 2);
	}
	else
		status = doValidHost(host, type);
	return status;
}

/*************************************************************************/

/**
 * Valid character check
 * @param c Character to check
 * @return 1 if a host is valid, 0 if it isnt.
 */
int isvalidchar(const char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '-')
		return 1;
	else
		return 0;
}

/*************************************************************************/

/**
 * Get the token
 * @param str String to search in
 * @param dilim Character to search for
 * @param token_number the token number
 * @return token
 */
char *myStrGetToken(const char *str, const char dilim, int token_number)
{
	int len, idx, counter = 0, start_pos = 0;
	char *substring = NULL;
	if (!str)
		return NULL;
	len = strlen(str);
	for (idx = 0; idx <= len; ++idx)
	{
		if (str[idx] == dilim || idx == len)
		{
			if (counter == token_number)
				substring = myStrSubString(str, start_pos, idx);
			else
				start_pos = idx + 1;
			++counter;
		}
	}
	return substring;
}

/*************************************************************************/

/**
 * Get the token only
 * @param str String to search in
 * @param dilim Character to search for
 * @param token_number the token number
 * @return token
 */
char *myStrGetOnlyToken(const char *str, const char dilim, int token_number)
{
	int len, idx, counter = 0, start_pos = 0;
	char *substring = NULL;
	if (!str)
		return NULL;
	len = strlen(str);
	for (idx = 0; idx <= len; ++idx)
	{
		if (str[idx] == dilim)
		{
			if (counter == token_number)
			{
				if (str[idx] == '\r')
					substring = myStrSubString(str, start_pos, idx - 1);
				else
					substring = myStrSubString(str, start_pos, idx);
			}
			else
				start_pos = idx + 1;
			++counter;
		}
	}
	return substring;
}

/*************************************************************************/

/**
 * Get the Remaining tokens
 * @param str String to search in
 * @param dilim Character to search for
 * @param token_number the token number
 * @return token
 */
char *myStrGetTokenRemainder(const char *str, const char dilim, int token_number)
{
	int len, idx, counter = 0, start_pos = 0;
	char *substring = NULL;
	if (!str)
		return NULL;
	len = strlen(str);

	for (idx = 0; idx <= len; ++idx)
	{
		if (str[idx] == dilim || idx == len)
		{
			if (counter == token_number)
				substring = myStrSubString(str, start_pos, len);
			else
				start_pos = idx + 1;
			++counter;
		}
	}
	return substring;
}

/*************************************************************************/

/**
 * Get the string between point A and point B
 * @param str String to search in
 * @param start Point A
 * @param end Point B
 * @return the string in between
 */
char *myStrSubString(const char *src, int start, int end)
{
	char *substring = NULL;
	int len, idx;
	if (!src)
		return NULL;
	len = strlen(src);
	if (start >= 0 && end <= len && end > start)
	{
		substring = new char[(end - start) + 1];
		for (idx = 0; idx <= end - start; ++idx)
			substring[idx] = src[start + idx];
		substring[end - start] = '\0';
	}
	return substring;
}

/*************************************************************************/

/**
 * Clean up the buffer for extra spaces
 * @param str to clean up
 * @return void
 */
void doCleanBuffer(char *str)
{
	char *in, *out;
	char ch;

	if (!str)
		return;

	in = str;
	out = str;

	while (issp(ch = *in++));
	if (ch)
		for (;;)
		{
			*out++ = ch;
			ch = *in++;
			if (!ch)
				break;
			if (!issp(ch))
				continue;
			while (issp(ch = *in++));
			if (!ch)
				break;
			*out++ = ' ';
		}
	*out = ch; /* == '\0' */
}

/*************************************************************************/

/**
 * Kill the user to enforce the sqline
 * @param nick to kill
 * @param killer whom is doing the killing
 * @return void
 */
void EnforceQlinedNick(const std::string &nick, const char *killer)
{
	if (findbot(nick))
		return;
	
	User *u2 = finduser(nick);

	if (u2)
	{
		Alog() << "Killed Q-lined nick: " << u2->GetMask();
		kill_user(killer, u2->nick.c_str(), "This nick is reserved for Services. Please use a non Q-Lined nick.");
	}
}

/*************************************************************************/

/**
 * Is the given nick a network service
 * @param nick to check
 * @param int Check if botserv bots
 * @return int
 */
int nickIsServices(const char *tempnick, int bot)
{
	int found = 0;
	char *s, *nick;

	if (!tempnick)
		return found;

	nick = sstrdup(tempnick);

	s = strchr(nick, '@');
	if (s)
	{
		*s++ = 0;
		if (stricmp(s, Config.ServerName))
		{
			delete [] nick;
			return found;
		}
	}

	if (Config.s_NickServ && !stricmp(nick, Config.s_NickServ))
		++found;
	else if (Config.s_ChanServ && !stricmp(nick, Config.s_ChanServ))
		++found;
	else if (Config.s_HostServ && !stricmp(nick, Config.s_HostServ))
		++found;
	else if (Config.s_MemoServ && !stricmp(nick, Config.s_MemoServ))
		++found;
	else if (Config.s_BotServ && !stricmp(nick, Config.s_BotServ))
		++found;
	else if (Config.s_OperServ && !stricmp(nick, Config.s_OperServ))
		++found;
	else if (Config.s_GlobalNoticer && !stricmp(nick, Config.s_GlobalNoticer))
		++found;
	else if (Config.s_BotServ && bot)
	{
		for (botinfo_map::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
		{
			BotInfo *bi = it->second;

			ci::string ci_bi_nick(bi->nick.c_str());
			if (ci_bi_nick == nick)
			{
				++found;
				break;
			}
		}
	}

	/* Somehow, something tells me we should free this :) -GD */
	delete [] nick;

	return found;
}

/*************************************************************************/

/**
 * arc4 init
 * @return void
 */
static void arc4_init()
{
	int n;
	for (n = 0; n < 256; ++n)
		rs.s[n] = n;
	rs.i = 0;
	rs.j = 0;
}

/*************************************************************************/

/**
 * arc4 addrandom
 * @param data
 * @param dalen Data Length
 * @return void
 */
static void arc4_addrandom(void *dat, int datlen)
{
	int n;
	uint8 si;

	--rs.i;
	for (n = 0; n < 256; ++n)
	{
		rs.i = rs.i + 1;
		si = rs.s[rs.i];
		rs.j = rs.j + si + (static_cast<unsigned char *>(dat))[n % datlen];
		rs.s[rs.i] = rs.s[rs.j];
		rs.s[rs.j] = si;
	}
}

/*************************************************************************/

/**
 * random init
 * @return void
 */
void rand_init()
{
#ifndef _WIN32
	int n, fd;
#endif
	struct {
#ifndef _WIN32
		struct timeval nowt; /* time */
		char rnd[32]; /* /dev/urandom */
#else
		MEMORYSTATUS mstat; /* memory status */
		struct _timeb nowt; /* time */
#endif
	} rdat;

	arc4_init();

	/* Grab OS specific "random" data */
#ifndef _WIN32
	/* unix/bsd: time */
	gettimeofday(&rdat.nowt, NULL);
	/* unix/bsd: /dev/urandom */
	fd = open("/dev/urandom", O_RDONLY);
	if (fd)
	{
		n = read(fd, &rdat.rnd, sizeof(rdat.rnd));
		close(fd);
	}
#else
	/* win32: time */
	_ftime(&rdat.nowt);
	/* win32: memory status */
	GlobalMemoryStatus(&rdat.mstat);
#endif

	arc4_addrandom(&rdat, sizeof(rdat));
}

/*************************************************************************/

/**
 * Setup the random numbers
 * @return void
 */
void add_entropy_userkeys()
{
	arc4_addrandom(&Config.UserKey1, sizeof(Config.UserKey1));
	arc4_addrandom(&Config.UserKey2, sizeof(Config.UserKey2));
	arc4_addrandom(&Config.UserKey3, sizeof(Config.UserKey3));
	/* UserKey3 is also used in mysql_rand() */
}

/*************************************************************************/

/**
 * Get the random numbers 8 byte deep
 * @return char
 */
unsigned char getrandom8()
{
	unsigned char si, sj;

	rs.i = rs.i + 1;
	si = rs.s[rs.i];
	rs.j = rs.j + si;
	sj = rs.s[rs.j];
	rs.s[rs.i] = sj;
	rs.s[rs.j] = si;
	return rs.s[(si + sj) & 0xff];
}

/*************************************************************************/

/**
 * Get the random numbers 16 byte deep
 * @return char
 */
uint16 getrandom16()
{
	uint16 val;

	val = getrandom8() << 8;
	val |= getrandom8();
	return val;
}

/*************************************************************************/

/**
 * Get the random numbers 32 byte deep
 * @return char
 */
uint32 getrandom32()
{
	uint32 val;

	val = getrandom8() << 24;
	val |= getrandom8() << 16;
	val |= getrandom8() << 8;
	val |= getrandom8();
	return val;
}

/*************************************************************************/

/**
 * Number of tokens in a string
 * @param str String
 * @param dilim Dilimiter
 * @return number of tokens
 */
int myNumToken(const char *str, const char dilim)
{
	int len, idx, counter = 0, start_pos = 0;
	if (!str)
		return 0;
	len = strlen(str);
	for (idx = 0; idx <= len; ++idx)
	{
		if (str[idx] == dilim || idx == len)
		{
			start_pos = idx + 1;
			++counter;
		}
	}
	return counter;
}

/*************************************************************************/

/**
 * Resolve a host to an IP
 * @param host to convert
 * @return ip address
 */
char *host_resolve(char *host)
{
	struct hostent *hentp = NULL;
	uint32 ip = INADDR_NONE;
	char ipbuf[16];
	char *ipreturn;
	struct in_addr addr;

	ipreturn = NULL;

	hentp = gethostbyname(host);

	if (hentp)
	{
		memcpy(&ip, hentp->h_addr, sizeof(hentp->h_length));
		addr.s_addr = ip;
		ntoa(addr, ipbuf, sizeof(ipbuf));
		ipreturn = sstrdup(ipbuf);
		Alog(LOG_DEBUG) << "resolved " << host << " to " << ipbuf;
	}
	return ipreturn;
}

/*************************************************************************/

/** Build a string list from a source string
 * @param src The source string
 * @return a list of strings
 */
std::list<std::string> BuildStringList(const std::string &src)
{
	spacesepstream tokens(src);
	std::string token;
	std::list<std::string> Ret;

	while (tokens.GetToken(token))
		Ret.push_back(token);

	return Ret;
}

std::list<ci::string> BuildStringList(const ci::string &src)
{
	spacesepstream tokens(src);
	ci::string token;
	std::list<ci::string> Ret;

	while (tokens.GetToken(token))
		Ret.push_back(token);

	return Ret;
}

std::vector<std::string> BuildStringVector(const std::string &src)
{
	spacesepstream tokens(src);
	std::string token;
	std::vector<std::string> Ret;

	while (tokens.GetToken(token))
		Ret.push_back(token);

	return Ret;
}

/*************************************************************************/

/**
 * Change an unsigned string to a signed string, overwriting the original
 * string.
 * @param input string
 * @return output string, same as input string.
 */

char *str_signed(unsigned char *str)
{
	char *nstr;

	nstr = reinterpret_cast<char *>(str);
	while (*str)
	{
		*nstr = static_cast<char>(*str);
		str++;
		nstr++;
	}

	return nstr;
}

/**
 *  Strip the mode prefix from the given string.
 *  Useful for using the modes stored in things like ircd->ownerset etc..
 **/

char *stripModePrefix(const char *str)
{
	if (str && (*str == '+' || *str == '-'))
		return sstrdup(str + 1);
	return NULL;
}

/* Equivalent to inet_ntoa */

void ntoa(struct in_addr addr, char *ipaddr, int len)
{
	unsigned char *bytes = reinterpret_cast<unsigned char *>(&addr.s_addr);
	snprintf(ipaddr, len, "%u.%u.%u.%u", bytes[0], bytes[1], bytes[2], bytes[3]);
}

/*
* strlcat and strlcpy were ripped from openssh 2.5.1p2
* They had the following Copyright info:
*
*
* Copyright (c) 1998 Todd C. Miller <Todd.Miller@courtesan.com>
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED `AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
* THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef HAVE_STRLCAT
size_t strlcat(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz, dlen;

	while (n-- && *d)
		++d;

	dlen = d - dst;
	n = siz - dlen;

	if (!n)
		return dlen + strlen(s);

	while (*s)
	{
		if (n != 1)
		{
			*d++ = *s;
			--n;
		}

		++s;
	}

	*d = '\0';
	return dlen + (s - src); /* count does not include NUL */
}
#endif

#ifndef HAVE_STRLCPY
size_t strlcpy(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;

	/* Copy as many bytes as will fit */
	if (n && --n)
	{
		do
		{
			if (!(*d++ = *s++))
				break;
		}
		while (--n);
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (!n)
	{
		if (siz)
			*d = '\0'; /* NUL-terminate dst */
		while (*s++);
	}

	return s - src - 1; /* count does not include NUL */
}
#endif

#ifdef _WIN32
char *GetWindowsVersion()
{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	char buf[BUFSIZE];
	char *extra;
	char *cputype;
	SYSTEM_INFO si;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO *)&osvi))
			return sstrdup("");
	}
	GetSystemInfo(&si);

	/* Determine CPU type 32 or 64 */
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		cputype = sstrdup(" 64-bit");
	else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
		cputype = sstrdup(" 32-bit");
	else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		cputype = sstrdup(" Itanium 64-bit");
	else
		cputype = sstrdup(" ");

	switch (osvi.dwPlatformId)
	{
		/* test for the Windows NT product family. */
		case VER_PLATFORM_WIN32_NT:
			/* Windows Vista or Windows Server 2008 */
			if (osvi.dwMajorVersion == 6 && !osvi.dwMinorVersion)
			{
				if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					extra = sstrdup("Enterprise Edition");
				else if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
					extra = sstrdup("Datacenter Edition");
				else if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
					extra = sstrdup("Home Premium/Basic");
				else
					extra = sstrdup(" ");
				if (osvi.wProductType & VER_NT_WORKSTATION)
					snprintf(buf, sizeof(buf), "Microsoft Windows Vista %s%s", cputype, extra);
				else
					snprintf(buf, sizeof(buf), "Microsoft Windows Server 2008 %s%s", cputype, extra);
				delete [] extra;
			}
			/* Windows 2003 or Windows XP Pro 64 */
			if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
			{
				if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
					extra = sstrdup("Datacenter Edition");
				else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					extra = sstrdup("Enterprise Edition");
#ifdef VER_SUITE_COMPUTE_SERVER
				else if (osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
					extra = sstrdup("Compute Cluster Edition");
#endif
				else if (osvi.wSuiteMask == VER_SUITE_BLADE)
					extra = sstrdup("Web Edition");
				else
					extra = sstrdup("Standard Edition");
				if (osvi.wProductType & VER_NT_WORKSTATION && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					snprintf(buf, sizeof(buf), "Windows XP Professional x64 Edition %s", extra);
				else
					snprintf(buf, sizeof(buf), "Microsoft Windows Server 2003 Family %s%s", cputype, extra);
				delete [] extra;
			}
			if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
			{
				if (osvi.wSuiteMask & VER_SUITE_EMBEDDEDNT)
					extra = sstrdup("Embedded");
				else if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
					extra = sstrdup("Home Edition");
				else
					extra = sstrdup(" ");
				snprintf(buf, sizeof(buf), "Microsoft Windows XP %s", extra);
				delete [] extra;
			}
			if (osvi.dwMajorVersion == 5 && !osvi.dwMinorVersion)
			{
				if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
					extra = sstrdup("Datacenter Server");
				else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					extra = sstrdup("Advanced Server");
				else
					extra = sstrdup("Server");
				snprintf(buf, sizeof(buf), "Microsoft Windows 2000 %s", extra);
				delete [] extra;
			}
			if (osvi.dwMajorVersion <= 4)
			{
				if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					extra = sstrdup("Server 4.0, Enterprise Edition");
				else
					extra = sstrdup("Server 4.0");
				snprintf(buf, sizeof(buf), "Microsoft Windows NT %s", extra);
				delete [] extra;
			}
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			if (osvi.dwMajorVersion == 4 && !osvi.dwMinorVersion)
			{
				if (osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B')
					extra = sstrdup("OSR2");
				else
					extra = sstrdup(" ");
				snprintf(buf, sizeof(buf), "Microsoft Windows 95 %s", extra);
				delete [] extra;
			}
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
			{
				if (osvi.szCSDVersion[1] == 'A')
					extra = sstrdup("SE");
				else
					extra = sstrdup(" ");
				snprintf(buf, sizeof(buf), "Microsoft Windows 98 %s", extra);
				delete [] extra;
			}
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
				snprintf(buf, sizeof(buf), "Microsoft Windows Millennium Edition");
	}
	delete [] cputype;
	return sstrdup(buf);
}

int SupportedWindowsVersion()
{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO *)&osvi))
			return 0;
	}

	switch (osvi.dwPlatformId)
	{
		/* test for the Windows NT product family. */
		case VER_PLATFORM_WIN32_NT:
			/* win nt4 */
			if (osvi.dwMajorVersion <= 4)
				return 0;
			/* the rest */
			return 1;
		/* win95 win98 winME */
		case VER_PLATFORM_WIN32_WINDOWS:
			return 0;
	}
	return 0;
}

#endif

/*************************************************************************/
/* This 2 functions were originally found in Bahamut */

/**
 * Turn a cidr value into a netmask
 * @param cidr CIDR value
 * @return Netmask value
 */
uint32 cidr_to_netmask(uint16 cidr)
{
	if (!cidr)
		return 0;

	return 0xFFFFFFFF - (1 << (32 - cidr)) + 1;
}

/**
 * Turn a netmask into a cidr value
 * @param mask Netmask
 * @return CIDR value
 */
uint16 netmask_to_cidr(uint32 mask)
{
	int tmp = 0;

	while (!(mask & (1 << tmp)) && tmp < 32)
		++tmp;

	return 32 - tmp;
}

/*************************************************************************/

/**
 * Check if the given string is some sort of wildcard
 * @param str String to check
 * @return 1 for wildcard, 0 for anything else
 */
int str_is_wildcard(const char *str)
{
	while (*str)
	{
		if (*str == '*' || *str == '?')
			return 1;
		++str;
	}

	return 0;
}

/**
 * Check if the given string is a pure wildcard
 * @param str String to check
 * @return 1 for pure wildcard, 0 for anything else
 */
int str_is_pure_wildcard(const char *str)
{
	while (*str)
	{
		if (*str != '*')
			return 0;
		++str;
	}

	return 1;
}

/*************************************************************************/

/**
 * Check if the given string is an IP, and return the IP.
 * @param str String to check
 * @return The IP, if one found. 0 if none.
 */
uint32 str_is_ip(char *str)
{
	int i;
	int octets[4] = { -1, -1, -1, -1 };
	char *s = str;
	uint32 ip;

	for (i = 0; i < 4; ++i)
	{
		octets[i] = strtol(s, &s, 10);
		/* Bail out if the octet is invalid or wrongly terminated */
		if (octets[i] < 0 || octets[i] > 255 || (i < 3 && *s != '.'))
			return 0;
		if (i < 3)
			++s;
	}

	/* Fill the IP - the dirty way */
	ip = octets[3];
	ip += octets[2] * 256;
	ip += octets[1] * 65536;
	ip += octets[0] * 16777216;

	return ip;
}

/*************************************************************************/

/**
 * Check if the given string is an IP or CIDR mask, and fill the given
 * ip/cidr params if so.
 * @param str String to check
 * @param ip The ipmask to fill when a CIDR is found
 * @param mask The CIDR mask to fill when a CIDR is found
 * @param host Displayed host
 * @return 1 for IP/CIDR, 0 for anything else
 */
int str_is_cidr(char *str, uint32 *ip, uint32 *mask, char **host)
{
	int i;
	int octets[4] = { -1, -1, -1, -1 };
	char *s = str;
	char buf[512];
	uint16 cidr;

	for (i = 0; i < 4; ++i)
	{
		octets[i] = strtol(s, &s, 10);
		/* Bail out if the octet is invalid or wrongly terminated */
		if (octets[i] < 0 || octets[i] > 255 || (i < 3 && *s != '.'))
			return 0;
		if (i < 3)
			++s;
	}

	/* Fill the IP - the dirty way */
	*ip = octets[3];
	*ip += octets[2] * 256;
	*ip += octets[1] * 65536;
	*ip += octets[0] * 16777216;

	if (*s == '/')
	{
		++s;
		/* There's a CIDR mask here! */
		cidr = strtol(s, &s, 10);
		/* Bail out if the CIDR is invalid or the string isn't done yet */
		if (cidr > 32 || *s)
			return 0;
	}
	else
		/* No CIDR mask here - use 32 so the whole ip will be matched */
		cidr = 32;

	*mask = cidr_to_netmask(cidr);
	/* Apply the mask to avoid 255.255.255.255/8 bans */
	*ip &= *mask;

	/* Refill the octets to fill the host */
	octets[0] = (*ip & 0xFF000000) / 16777216;
	octets[1] = (*ip & 0x00FF0000) / 65536;
	octets[2] = (*ip & 0x0000FF00) / 256;
	octets[3] = (*ip & 0x000000FF);

	if (cidr == 32)
		snprintf(buf, 512, "%d.%d.%d.%d", octets[0], octets[1], octets[2], octets[3]);
	else
		snprintf(buf, 512, "%d.%d.%d.%d/%d", octets[0], octets[1], octets[2], octets[3], cidr);

	*host = sstrdup(buf);

	return 1;
}

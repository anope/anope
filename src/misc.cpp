
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
bool IsFile(const Anope::string &filename)
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
 * strscpy:  Copy at most len-1 characters from a string to a buffer, and
 *           add a null terminator after the last character copied.
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

NumberList::NumberList(const Anope::string &list, bool descending) : is_valid(true), desc(descending)
{
	Anope::string error;
	commasepstream sep(list);
	Anope::string token;

	sep.GetToken(token);
	if (token.empty())
		token = list;
	do
	{
		size_t t = token.find('-');

		if (t == Anope::string::npos)
		{
			unsigned num = convertTo<unsigned>(token, error, false);
			if (error.empty())
				numbers.insert(num);
			else
			{
				if (!this->InvalidRange(list))
				{
					is_valid = false;
					return;
				}
			}
		}
		else
		{
			Anope::string error2;
			unsigned num1 = convertTo<unsigned>(token.substr(0, t), error, false);
			unsigned num2 = convertTo<unsigned>(token.substr(t + 1), error2, false);
			if (error.empty() && error2.empty())
			{
				for (unsigned i = num1; i <= num2; ++i)
					numbers.insert(i);
			}
			else
			{
				if (!this->InvalidRange(list))
				{
					is_valid = false;
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
	if (!is_valid)
		return;

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
}

void NumberList::HandleNumber(unsigned)
{
}

bool NumberList::InvalidRange(const Anope::string &)
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
time_t dotime(const Anope::string &s)
{
	if (s.empty())
		return -1;

	int amount = 0;
	Anope::string end;

	try
	{
		amount = convertTo<int>(s, end, false);
		if (!end.empty())
		{
			switch (end[0])
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
	}
	catch (const CoreException &) { }

	return amount;
}

/*************************************************************************/

/**
 * Expresses in a string the period of time represented by a given amount
 * of seconds (with days/hours/minutes).
 * @param na Nick Alias
 * @param seconds time in seconds
 * @return buffer
 */
Anope::string duration(const NickCore *nc, time_t seconds)
{
	/* We first calculate everything */
	int days = seconds / 86400;
	seconds -= (days * 86400);
	int hours = seconds / 3600;
	seconds -= (hours * 3600);
	int minutes = seconds / 60;

	char buf[64];
	Anope::string buffer;

	if (!days && !hours && !minutes)
	{
		snprintf(buf, sizeof(buf), GetString(nc, seconds <= 1 ? DURATION_SECOND : DURATION_SECONDS).c_str(), seconds);
		buffer = buf;
	}
	else
	{
		bool need_comma = false;
		if (days)
		{
			snprintf(buf, sizeof(buf), GetString(nc, days == 1 ? DURATION_DAY : DURATION_DAYS).c_str(), days);
			buffer = buf;
			need_comma = true;
		}
		if (hours)
		{
			snprintf(buf, sizeof(buf), GetString(nc, hours == 1 ? DURATION_HOUR : DURATION_HOURS).c_str(), hours);
			buffer += Anope::string(need_comma ? ", " : "") + buf;
			need_comma = true;
		}
		if (minutes)
		{
			snprintf(buf, sizeof(buf), GetString(nc, minutes == 1 ? DURATION_MINUTE : DURATION_MINUTES).c_str(), minutes);
			buffer += Anope::string(need_comma ? ", " : "") + buf;
		}
	}

	return buffer;
}

Anope::string do_strftime(const time_t &t)
{
	tm tm = *localtime(&t);
	char buf[BUFSIZE];
	strftime(buf, sizeof(buf), "%b %d %H:%M:%S %Y %Z", &tm);
	if (t < Anope::CurTime)
		return Anope::string(buf) + " (" + duration(NULL, Anope::CurTime - t) + " ago)";
	else
		return Anope::string(buf) + " (" + duration(NULL, t - Anope::CurTime) + " from now)";
}

/*************************************************************************/

/**
 * Generates a human readable string of type "expires in ..."
 * @param na Nick Alias
 * @param seconds time in seconds
 * @return buffer
 */
Anope::string expire_left(const NickCore *nc, time_t expires)
{
	if (!expires)
		return GetString(nc, NO_EXPIRE);
	else if (expires <= Anope::CurTime)
		return GetString(nc, EXPIRES_SOON);
	else
	{
		char buf[256];
		time_t diff = expires - Anope::CurTime + 59;

		if (diff >= 86400)
		{
			int days = diff / 86400;
			snprintf(buf, sizeof(buf), GetString(nc, days == 1 ? EXPIRES_1D : EXPIRES_D).c_str(), days);
		}
		else
		{
			if (diff <= 3600)
			{
				int minutes = diff / 60;
				snprintf(buf, sizeof(buf), GetString(nc, minutes == 1 ? EXPIRES_1M : EXPIRES_M).c_str(), minutes);
			}
			else
			{
				int hours = diff / 3600, minutes;
				diff -= hours * 3600;
				minutes = diff / 60;
				snprintf(buf, sizeof(buf), GetString(nc, hours == 1 && minutes == 1 ? EXPIRES_1H1M : (hours == 1 && minutes != 1 ? EXPIRES_1HM : (hours != 1 && minutes == 1 ? EXPIRES_H1M : EXPIRES_HM))).c_str(), hours, minutes);
			}
		}

		return buf;
	}
}

/*************************************************************************/

/**
 * Validate the host
 * shortname  =  ( letter / digit ) *( letter / digit / "-" ) *( letter / digit )
 * hostname   =  shortname *( "." shortname )
 * ip4addr    =  1*3digit "." 1*3digit "." 1*3digit "." 1*3digit
 * @param host = string to check
 * @param type = format, 1 = ip4addr, 2 = hostname
 * @return 1 if a host is valid, 0 if it isnt.
 */
bool doValidHost(const Anope::string &host, int type)
{
	if (type != 1 && type != 2)
		return false;
	if (host.empty())
		return false;

	size_t len = host.length();

	if (len > Config->HostLen)
		return false;

	size_t idx, sec_len = 0, dots = 1;
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
						return false;
				}
				else
				{
					if (!idx)
						return false; /* cant start with a non-digit */
					if (host[idx] != '.')
						return false; /* only . is a valid non-digit */
					if (sec_len > 3)
						return false; /* sections cant be more than 3 digits */
					sec_len = 0;
					++dots;
				}
			}
			if (dots != 4)
				return false;
			break;
		case 2:
			dots = 0;
			for (idx = 0; idx < len; ++idx)
			{
				if (!isalnum(host[idx]))
				{
					if (!idx)
						return false;
					if (host[idx] != '.' && host[idx] != '-')
						return false;
					if (host[idx] == '.')
						++dots;
				}
			}
			if (host[len - 1] == '.')
				return false;
			/**
			 * Ultimate3 dosnt like a non-dotted hosts at all, nor does unreal,
			 * so just dont allow them.
			 */
			if (!dots)
				return false;
	}
	return true;
}

/*************************************************************************/

/**
 * Front end to doValidHost
 * @param host = string to check
 * @param type = format, 1 = ip4addr, 2 = hostname
 * @return 1 if a host is valid, 0 if it isnt.
 */
bool isValidHost(const Anope::string &host, int type)
{
	bool status = false;
	if (type == 3)
	{
		status = doValidHost(host, 1);
		if (!status)
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
bool isvalidchar(char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '-';
}

/*************************************************************************/

/**
 * Get the token
 * @param str String to search in
 * @param dilim Character to search for
 * @param token_number the token number
 * @return token
 */
Anope::string myStrGetToken(const Anope::string &str, char dilim, int token_number)
{
	if (str.empty() || str.find(dilim) == Anope::string::npos)
		return "";

	Anope::string substring;
	sepstream sep(str, dilim);

	for (int i = 0; i < token_number + 1 && !sep.StreamEnd() && sep.GetToken(substring); ++i);

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
Anope::string myStrGetTokenRemainder(const Anope::string &str, const char dilim, int token_number)
{
	if (str.empty() || str.find(dilim) == Anope::string::npos)
		return "";

	Anope::string substring;
	sepstream sep(str, dilim);

	for (int i = 0; i < token_number + 1 && !sep.StreamEnd() && sep.GetToken(substring); ++i);

	if (!sep.StreamEnd())
		substring += dilim + sep.GetRemaining();
	return substring;
}

/*************************************************************************/

/**
 * Kill the user to enforce the sqline
 * @param nick to kill
 * @param killer whom is doing the killing
 * @return void
 */
void EnforceQlinedNick(const Anope::string &nick, const Anope::string &killer)
{
	if (findbot(nick))
		return;

	User *u2 = finduser(nick);

	if (u2)
	{
		Log() << "Killed Q-lined nick: " << u2->GetMask();
		kill_user(killer, u2->nick, "This nick is reserved for Services. Please use a non Q-Lined nick.");
	}
}

/*************************************************************************/

/**
 * Is the given nick a network service
 * @param nick to check
 * @param int Check if botserv bots
 * @return int
 */
bool nickIsServices(const Anope::string &tempnick, bool bot)
{
	if (tempnick.empty())
		return false;

	Anope::string nick = tempnick;

	size_t at = nick.find('@');
	if (at != Anope::string::npos)
	{
		Anope::string servername = nick.substr(at + 1);
		if (!servername.equals_ci(Config->ServerName))
			return false;
		nick = nick.substr(0, at);
	}

	if (!Config->s_NickServ.empty() && nick.equals_ci(Config->s_NickServ))
		return true;
	else if (!Config->s_ChanServ.empty() && nick.equals_ci(Config->s_ChanServ))
		return true;
	else if (!Config->s_HostServ.empty() && nick.equals_ci(Config->s_HostServ))
		return true;
	else if (!Config->s_MemoServ.empty() && nick.equals_ci(Config->s_MemoServ))
		return true;
	else if (!Config->s_BotServ.empty() && nick.equals_ci(Config->s_BotServ))
		return true;
	else if (!Config->s_OperServ.empty() && nick.equals_ci(Config->s_OperServ))
		return true;
	else if (!Config->s_GlobalNoticer.empty() && nick.equals_ci(Config->s_GlobalNoticer))
		return true;
	else if (!Config->s_BotServ.empty() && bot)
	{
		for (botinfo_map::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
		{
			BotInfo *bi = it->second;

			if (nick.equals_ci(bi->nick))
				return true;
		}
	}

	return false;
}

/*************************************************************************/

/**
 * arc4 init
 * @return void
 */
static void arc4_init()
{
	for (int n = 0; n < 256; ++n)
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
	--rs.i;
	for (int n = 0; n < 256; ++n)
	{
		++rs.i;
		uint8 si = rs.s[rs.i];
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
	struct
	{
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
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd)
	{
		read(fd, &rdat.rnd, sizeof(rdat.rnd));
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
	arc4_addrandom(&Config->UserKey1, sizeof(Config->UserKey1));
	arc4_addrandom(&Config->UserKey2, sizeof(Config->UserKey2));
	arc4_addrandom(&Config->UserKey3, sizeof(Config->UserKey3));
	/* UserKey3 is also used in mysql_rand() */
}

/*************************************************************************/

/**
 * Get the random numbers 8 byte deep
 * @return char
 */
unsigned char getrandom8()
{
	++rs.i;
	unsigned char si = rs.s[rs.i];
	rs.j += si;
	unsigned char sj = rs.s[rs.j];
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
	uint16 val = getrandom8() << 8;
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
	uint32 val = getrandom8() << 24;
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
int myNumToken(const Anope::string &str, char dilim)
{
	if (str.empty())
		return 0;
	int counter = 0;
	for (size_t idx = 0, len = str.length(); idx <= len; ++idx)
		if (str[idx] == dilim || idx == len)
			++counter;
	return counter;
}

/*************************************************************************/

/** Build a string list from a source string
 * @param src The source string
 * @return a list of strings
 */
std::list<Anope::string> BuildStringList(const Anope::string &src, char delim)
{
	sepstream tokens(src, delim);
	Anope::string token;
	std::list<Anope::string> Ret;

	while (tokens.GetToken(token))
		Ret.push_back(token);

	return Ret;
}

std::vector<Anope::string> BuildStringVector(const Anope::string &src, char delim)
{
	sepstream tokens(src, delim);
	Anope::string token;
	std::vector<Anope::string> Ret;

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
	char *nstr = reinterpret_cast<char *>(str);
	while (*str)
	{
		*nstr = static_cast<char>(*str);
		++str;
		++nstr;
	}

	return nstr;
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
Anope::string GetWindowsVersion()
{
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	BOOL bOsVersionInfoEx = GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&osvi));
	if (!bOsVersionInfoEx)
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&osvi)))
			return "";
	}
	GetSystemInfo(&si);

	Anope::string buf, extra, cputype;
	/* Determine CPU type 32 or 64 */
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
		cputype = " 64-bit";
	else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
		cputype = " 32-bit";
	else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		cputype = " Itanium 64-bit";

	switch (osvi.dwPlatformId)
	{
		/* test for the Windows NT product family. */
		case VER_PLATFORM_WIN32_NT:
			/* Windows Vista or Windows Server 2008 */
			if (osvi.dwMajorVersion == 6 && !osvi.dwMinorVersion)
			{
				if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					extra = " Enterprise Edition";
				else if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
					extra = " Datacenter Edition";
				else if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
					extra = " Home Premium/Basic";
				if (osvi.wProductType & VER_NT_WORKSTATION)
					buf = "Microsoft Windows Vista" + cputype + extra;
				else
					buf = "Microsoft Windows Server 2008" + cputype + extra;
			}
			/* Windows 2003 or Windows XP Pro 64 */
			if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
			{
				if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
					extra = " Datacenter Edition";
				else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					extra = " Enterprise Edition";
#ifdef VER_SUITE_COMPUTE_SERVER
				else if (osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER)
					extra = " Compute Cluster Edition";
#endif
				else if (osvi.wSuiteMask == VER_SUITE_BLADE)
					extra = " Web Edition";
				else
					extra = " Standard Edition";
				if (osvi.wProductType & VER_NT_WORKSTATION && si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					buf = "Microsoft Windows XP Professional x64 Edition" + extra;
				else
					buf = "Microsoft Windows Server 2003 Family" + cputype + extra;
			}
			if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
			{
				if (osvi.wSuiteMask & VER_SUITE_EMBEDDEDNT)
					extra = " Embedded";
				else if (osvi.wSuiteMask & VER_SUITE_PERSONAL)
					extra = " Home Edition";
				buf = "Microsoft Windows XP" + extra;
			}
			if (osvi.dwMajorVersion == 5 && !osvi.dwMinorVersion)
			{
				if (osvi.wSuiteMask & VER_SUITE_DATACENTER)
					extra = " Datacenter Server";
				else if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					extra = " Advanced Server";
				else
					extra = " Server";
				buf = "Microsoft Windows 2000" + extra;
			}
			if (osvi.dwMajorVersion <= 4)
			{
				if (osvi.wSuiteMask & VER_SUITE_ENTERPRISE)
					extra = " Enterprise Edition";
				buf = "Microsoft Windows NT Server 4.0" + extra;
			}
			break;
		case VER_PLATFORM_WIN32_WINDOWS:
			if (osvi.dwMajorVersion == 4 && !osvi.dwMinorVersion)
			{
				if (osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B')
					extra = " OSR2";
				buf = "Microsoft Windows 95" + extra;
			}
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
			{
				if (osvi.szCSDVersion[1] == 'A')
					extra = "SE";
				buf = "Microsoft Windows 98" + extra;
			}
			if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
				buf = "Microsoft Windows Millenium Edition";
	}
	return buf;
}

bool SupportedWindowsVersion()
{
	OSVERSIONINFOEX osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	BOOL bOsVersionInfoEx = GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&osvi));
	if (!bOsVersionInfoEx)
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&osvi)))
			return false;
	}

	switch (osvi.dwPlatformId)
	{
		/* test for the Windows NT product family. */
		case VER_PLATFORM_WIN32_NT:
			/* win nt4 */
			if (osvi.dwMajorVersion <= 4)
				return false;
			/* the rest */
			return true;
		/* win95 win98 winME */
		case VER_PLATFORM_WIN32_WINDOWS:
			return false;
	}
	return false;
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
bool str_is_wildcard(const Anope::string &str)
{
	for (Anope::string::const_iterator c = str.begin(), c_end = str.end(); c != c_end; ++c)
		if (*c == '*' || *c == '?')
			return true;

	return false;
}

/**
 * Check if the given string is a pure wildcard
 * @param str String to check
 * @return 1 for pure wildcard, 0 for anything else
 */
bool str_is_pure_wildcard(const Anope::string &str)
{
	for (Anope::string::const_iterator c = str.begin(), c_end = str.end(); c != c_end; ++c)
		if (*c != '*')
			return false;

	return true;
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
bool str_is_cidr(const Anope::string &str, uint32 &ip, uint32 &mask, Anope::string &host)
{
	int octets[4] = { -1, -1, -1, -1 };
	std::vector<Anope::string> octets_str = BuildStringVector(str, '.');

	if (octets_str.size() != 4)
		return false;

	Anope::string cidr_str;
	for (unsigned i = 0; i < 4; ++i)
	{
		Anope::string octet = octets_str[i];

		if (i == 3)
		{
			size_t slash = octet.find('/');
			if (slash != Anope::string::npos)
			{
				cidr_str = octet.substr(slash + 1);
				octet = octet.substr(0, slash);
			}
		}

		if (!octet.is_number_only())
			return false;

		octets[i] = convertTo<int>(octet);
		/* Bail out if the octet is invalid or wrongly terminated */
		if (octets[i] < 0 || octets[i] > 255)
			return false;
	}

	/* Fill the IP - the dirty way */
	ip = octets[3];
	ip += octets[2] * 256;
	ip += octets[1] * 65536;
	ip += octets[0] * 16777216;

	uint16 cidr;
	if (!cidr_str.empty())
	{
		Anope::string s;
		/* There's a CIDR mask here! */
		cidr = convertTo<uint16>(s.substr(1), s, false);
		/* Bail out if the CIDR is invalid or the string isn't done yet */
		if (cidr > 32 || !s.empty())
			return false;
	}
	else
		/* No CIDR mask here - use 32 so the whole ip will be matched */
		cidr = 32;

	mask = cidr_to_netmask(cidr);
	/* Apply the mask to avoid 255.255.255.255/8 bans */
	ip &= mask;

	/* Refill the octets to fill the host */
	octets[0] = (ip & 0xFF000000) / 16777216;
	octets[1] = (ip & 0x00FF0000) / 65536;
	octets[2] = (ip & 0x0000FF00) / 256;
	octets[3] = (ip & 0x000000FF);

	host = stringify(octets[0]) + "." + stringify(octets[1]) + "." + stringify(octets[2]) + "." + stringify(octets[3]);
	if (cidr != 32)
		host += "/" + stringify(cidr);

	return true;
}

/********************************************************************************/

/** Converts a string to hex
 * @param the data to be converted
 * @return a anope::string containing the hex value
 */
Anope::string Anope::Hex(const Anope::string &data)
{
	const char hextable[] = "0123456789abcdef";

	size_t l = data.length();
	Anope::string rv;
	for (size_t i = 0; i < l; ++i)
	{
		unsigned char c = data[i];
		rv += hextable[c >> 4];
		rv += hextable[c & 0xF];
	}
	return rv;
}

Anope::string Anope::Hex(const char *data, unsigned len)
{
	const char hextable[] = "0123456789abcdef";

	Anope::string rv;
	for (size_t i = 0; i < len; ++i)
	{
		unsigned char c = data[i];
		rv += hextable[c >> 4];
		rv += hextable[c & 0xF];
	}
	return rv;
}

/** Converts a string from hex
 * @param src The data to be converted
 * @param dest The destination string
 */
void Anope::Unhex(const Anope::string &src, Anope::string &dest)
{
	size_t len = src.length();
	Anope::string rv;
	for (size_t i = 0; i < len; i += 2)
	{
		char h = src[i], l = src[i + 1];
		unsigned char byte = (h >= 'a' ? h - 'a' + 10 : h - '0') << 4;
		byte += (l >= 'a' ? l - 'a' + 10 : l - '0');
		rv += byte;
	}
	dest = rv;
}

void Anope::Unhex(const Anope::string &src, char *dest)
{
	size_t len = src.length(), destpos = 0;
	for (size_t i = 0; i < len; i += 2)
	{
		char h = src[i], l = src[i + 1];
		unsigned char byte = (h >= 'a' ? h - 'a' + 10 : h - '0') << 4;
		byte += (l >= 'a' ? l - 'a' + 10 : l - '0');
		dest[destpos++] = byte;
	}
	dest[destpos] = 0;
}

const Anope::string Anope::LastError()
{
#ifndef _WIN32
	return strerror(errno);
#else
	char errbuf[513];
	DWORD err = GetLastError();
	if (!err)
		return "";
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, 0, errbuf, 512, NULL);
	return errbuf;
#endif
}


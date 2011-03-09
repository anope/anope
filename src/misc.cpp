
/* Miscellaneous routines.
 *
 * (C) 2003-2011 Anope Team
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

	int amount;
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
	catch (const ConvertException &) { }

	return 0;
}

/*************************************************************************/

/**
 * Expresses in a string the period of time represented by a given amount
 * of seconds (with days/hours/minutes).
 * @param na Nick Alias
 * @param seconds time in seconds
 * @return buffer
 */
Anope::string duration(NickCore *nc, time_t seconds)
{
	/* We first calculate everything */
	time_t days = seconds / 86400;
	seconds -= (days * 86400);
	time_t hours = seconds / 3600;
	seconds -= (hours * 3600);
	time_t minutes = seconds / 60;

	Anope::string buffer;

	if (!days && !hours && !minutes)
		buffer = stringify(seconds) + " second" + (seconds != 1 ? "s" : "");
	else
	{
		bool need_comma = false;
		if (days)
		{
			buffer = stringify(days) + " day" + (days != 1 ? "s" : "");
			need_comma = true;
		}
		if (hours)
		{
			buffer += need_comma ? ", " : "";
			buffer += stringify(hours) + " hour" + (hours != 1 ? "s" : "");
			need_comma = true;
		}
		if (minutes)
		{
			buffer += need_comma ? ", " : "";
			buffer += stringify(minutes) + " minute" + (minutes != 1 ? "s" : "");
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
Anope::string expire_left(NickCore *nc, time_t expires)
{
	if (!expires)
		return GetString(nc, _(NO_EXPIRE));
	else if (expires <= Anope::CurTime)
		return GetString(nc, _("expires at next database update"));
	else
	{
		char buf[256];
		time_t diff = expires - Anope::CurTime + 59;

		if (diff >= 86400)
		{
			int days = diff / 86400;
			snprintf(buf, sizeof(buf), GetString(nc, days == 1 ? _("expires in %d day") : _("expires in %d days")).c_str(), days);
		}
		else
		{
			if (diff <= 3600)
			{
				int minutes = diff / 60;
				snprintf(buf, sizeof(buf), GetString(nc, minutes == 1 ? _("expires in %d minute") : _("expires in %d minutes")).c_str(), minutes);
			}
			else
			{
				int hours = diff / 3600, minutes;
				diff -= hours * 3600;
				minutes = diff / 60;
				snprintf(buf, sizeof(buf), GetString(nc, hours == 1 && minutes == 1 ? _("expires in %d hour, %d minute") : (hours == 1 && minutes != 1 ? _("expires in %d hour, %d minutes") : (hours != 1 && minutes == 1 ? _("expires in %d hours, %d minute") : _("expires in %d hours, %d minutes")))).c_str(), hours, minutes);
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
		Log(LOG_NORMAL, "xline") << "Killed Q-lined nick: " << u2->GetMask();
		kill_user(killer, u2, "This nick is reserved for Services. Please use a non Q-Lined nick.");
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
	else if (!Config->s_BotServ.empty() && bot && BotListByNick.find(nick))
		return true;

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

bool Anope::Match(const Anope::string &str, const Anope::string &mask, bool case_sensitive)
{
	size_t s = 0, m = 0, str_len = str.length(), mask_len = mask.length();

	while (s < str_len && m < mask_len && mask[m] != '*')
	{
		char string = str[s], wild = mask[m];
		if (case_sensitive)
		{
			if (wild != string && wild != '?')
				return false;
		}
		else
		{
			if (tolower(wild) != tolower(string) && wild != '?')
				return false;
		}

		++m;
		++s;
	}

	size_t sp = Anope::string::npos, mp = Anope::string::npos;
	while (s < str_len)
	{
		char string = str[s], wild = mask[m];
		if (wild == '*')
		{
			if (++m == mask_len)
				return 1;

			mp = m;
			sp = s + 1;
		}
		else if (case_sensitive)
		{
			if (wild == string || wild == '?')
			{
				++m;
				++s;
			}
			else
			{
				m = mp;
				s = sp++;
			}
		}
		else
		{
			if (tolower(wild) == tolower(string) || wild == '?')
			{
				++m;
				++s;
			}
			else
			{
				m = mp;
				s = sp++;
			}
		}
	}

	if (mask[m] == '*')
		++m;

	return m == mask_len;
}

/** Returns a sequence of data formatted as the format argument specifies.
 ** After the format parameter, the function expects at least as many
 ** additional arguments as specified in format.
 * @param fmt Format of the Message
 * @param ... any number of parameters
 * @return a Anope::string
 */
Anope::string Anope::printf(const Anope::string &fmt, ...)
{
	va_list args;
	char buf[1024];
	va_start(args, fmt.c_str());
	vsnprintf(buf, sizeof(buf), fmt.c_str(), args);
	va_end(args);
	return buf;
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

/**
 * Check if the given string is some sort of wildcard
 * @param str String to check
 * @return 1 for wildcard, 0 for anything else
 */
bool str_is_wildcard(const Anope::string &str)
{
	return str.find_first_of("*?") != Anope::string::npos;
}

/**
 * Check if the given string is a pure wildcard
 * @param str String to check
 * @return 1 for pure wildcard, 0 for anything else
 */
bool str_is_pure_wildcard(const Anope::string &str)
{
	return str.find_first_not_of('*') == Anope::string::npos;
}

/*************************************************************************/

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


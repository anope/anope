
/* Miscellaneous routines.
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
#include "version.h"
#include "modules.h"
#include "extern.h"
#include "lists.h"
#include "config.h"
#include "bots.h"
#include "language.h"
#include "regexpr.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

ExtensibleItem::ExtensibleItem()
{
}

ExtensibleItem::~ExtensibleItem()
{
}

void ExtensibleItem::OnDelete()
{
	delete this;
}

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

ListFormatter &ListFormatter::addColumn(const Anope::string &name)
{
	this->columns.push_back(name);
	return *this;
}

void ListFormatter::addEntry(const ListEntry &entry)
{
	this->entries.push_back(entry);
}

bool ListFormatter::isEmpty() const
{
	return this->entries.empty();
}

void ListFormatter::Process(std::vector<Anope::string> &buffer)
{
	buffer.clear();

	std::map<Anope::string, size_t> lenghts;
	std::set<Anope::string> breaks;
	for (unsigned i = 0; i < this->columns.size(); ++i)
		lenghts[this->columns[i]] = this->columns[i].length();
	for (unsigned i = 0; i < this->entries.size(); ++i)
	{
		ListEntry &e = this->entries[i];
		for (unsigned j = 0; j < this->columns.size(); ++j)
			if (e[this->columns[j]].length() > lenghts[this->columns[j]])
				lenghts[this->columns[j]] = e[this->columns[j]].length();
	}
	unsigned length = 0;
	for (std::map<Anope::string, size_t>::iterator it = lenghts.begin(), it_end = lenghts.end(); it != it_end; ++it)
	{
		/* Break lines at 80 chars */
		if (length > 80)
		{
			breaks.insert(it->first);
			length = 0;
		}
		else
			length += it->second;
	}

	/* Only put a list header if more than 1 column */
	if (this->columns.size() > 1)
	{
		Anope::string s;
		for (unsigned i = 0; i < this->columns.size(); ++i)
		{
			if (breaks.count(this->columns[i]))
			{
				buffer.push_back(s);
				s = "  ";
			}
			else if (!s.empty())
				s += "  ";
			s += this->columns[i];
			if (i + 1  != this->columns.size())
				for (unsigned j = this->columns[i].length(); j < lenghts[this->columns[i]]; ++j)
					s += " ";
		}
		buffer.push_back(s);
	}

	for (unsigned i = 0; i < this->entries.size(); ++i)
	{
		ListEntry &e = this->entries[i];

		Anope::string s;
		for (unsigned j = 0; j < this->columns.size(); ++j)
		{
			if (breaks.count(this->columns[j]))
			{
				buffer.push_back(s);
				s = "  ";
			}
			else if (!s.empty())
				s += "  ";
			s += e[this->columns[j]];
			if (j + 1 != this->columns.size())
				for (unsigned k = e[this->columns[j]].length(); k < lenghts[this->columns[j]]; ++k)
					s += " ";
		}
		buffer.push_back(s);
	}
}

InfoFormatter::InfoFormatter(User *u) : user(u), longest(0)
{
}

void InfoFormatter::Process(std::vector<Anope::string> &buffer)
{
	buffer.clear();

	for (std::vector<std::pair<Anope::string, Anope::string> >::iterator it = this->replies.begin(), it_end = this->replies.end(); it != it_end; ++it)
	{
		Anope::string s;
		for (unsigned i = it->first.length(); i < this->longest; ++i)
			s += " ";
		s += Anope::string(translate(this->user, it->first.c_str())) + ": " + it->second;

		buffer.push_back(s);
	}
}

Anope::string& InfoFormatter::operator[](const Anope::string &key)
{
	if (key.length() > this->longest)
		this->longest = key.length();
	this->replies.push_back(std::make_pair(key, ""));
	return this->replies.back().second;
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
 * @param seconds time in seconds
 * @return buffer
 */
Anope::string duration(const time_t &t, const NickCore *nc)
{
	/* We first calculate everything */
	time_t days = (t / 86400);
	time_t hours = (t / 3600) % 24;
	time_t minutes = (t / 60) % 60;
	time_t seconds = (t) % 60;

	if (!days && !hours && !minutes)
		return stringify(seconds) + " " + (seconds != 1 ? translate(nc, _("seconds")) : translate(nc, _("second")));
	else
	{
		bool need_comma = false;
		Anope::string buffer;
		if (days)
		{
			buffer = stringify(days) + " " + (days != 1 ? translate(nc, _("days")) : translate(nc, _("day")));
			need_comma = true;
		}
		if (hours)
		{
			buffer += need_comma ? ", " : "";
			buffer += stringify(hours) + " " + (hours != 1 ? translate(nc, _("hours")) : translate(nc, _("hour")));
			need_comma = true;
		}
		if (minutes)
		{
			buffer += need_comma ? ", " : "";
			buffer += stringify(minutes) + " " + (minutes != 1 ? translate(nc, _("minutes")) : translate(nc, _("minute")));
		}
		return buffer;
	}
}

Anope::string do_strftime(const time_t &t, const NickCore *nc, bool short_output)
{
	tm tm = *localtime(&t);
	char buf[BUFSIZE];
	strftime(buf, sizeof(buf), translate(nc, _("%b %d %H:%M:%S %Y %Z")), &tm);
	if (short_output)
		return buf;
	if (t < Anope::CurTime)
		return Anope::string(buf) + " " + Anope::printf(translate(nc, _("(%s ago)")), duration(Anope::CurTime - t).c_str(), nc);
	else
		return Anope::string(buf) + " " + Anope::printf(translate(nc, _("(%s from now)")), duration(t - Anope::CurTime).c_str(), nc);
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
		return translate(nc, NO_EXPIRE);
	else if (expires <= Anope::CurTime)
		return translate(nc, _("expires momentarily"));
	else
	{
		char buf[256];
		time_t diff = expires - Anope::CurTime + 59;

		if (diff >= 86400)
		{
			int days = diff / 86400;
			snprintf(buf, sizeof(buf), translate(nc, days == 1 ? _("expires in %d day") : _("expires in %d days")), days);
		}
		else
		{
			if (diff <= 3600)
			{
				int minutes = diff / 60;
				snprintf(buf, sizeof(buf), translate(nc, minutes == 1 ? _("expires in %d minute") : _("expires in %d minutes")), minutes);
			}
			else
			{
				int hours = diff / 3600, minutes;
				diff -= hours * 3600;
				minutes = diff / 60;
				snprintf(buf, sizeof(buf), translate(nc, hours == 1 && minutes == 1 ? _("expires in %d hour, %d minute") : (hours == 1 && minutes != 1 ? _("expires in %d hour, %d minutes") : (hours != 1 && minutes == 1 ? _("expires in %d hours, %d minute") : _("expires in %d hours, %d minutes")))), hours, minutes);
			}
		}

		return buf;
	}
}

/*************************************************************************/

/** Checks if a username is valid
 * @param ident The username
 * @return true if the ident is valid
 */
bool IsValidIdent(const Anope::string &ident)
{
	if (ident.empty() || ident.length() > Config->UserLen)
		return false;
	for (unsigned i = 0; i < ident.length(); ++i)
	{
		const char &c = ident[i];
		if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '-')
			;
		else
			return false;
	}

	return true;
}

/** Checks if a host is valid
 * @param host The host
 * @param true if the host is valid
 */
bool IsValidHost(const Anope::string &host)
{
	if (host.empty() || host.length() > Config->HostLen)
		return false;

	if (Config->VhostDisallowBE.find_first_of(host[0]) != Anope::string::npos)
		return false;
	else if (Config->VhostDisallowBE.find_first_of(host[host.length() - 1]) != Anope::string::npos)
		return false;

	int dots = 0;
	for (unsigned i = 0; i < host.length(); ++i)
	{
		if (host[i] == '.')
			++dots;
		if (Config->VhostChars.find_first_of(host[i]) == Anope::string::npos)
			return false;
	}

	return Config->VhostUndotted || dots > 0;
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
		return token_number ? "" : str;

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
		return token_number ? "" : str;

	Anope::string substring;
	sepstream sep(str, dilim);

	for (int i = 0; i < token_number + 1 && !sep.StreamEnd() && sep.GetToken(substring); ++i);

	if (!sep.StreamEnd())
		substring += dilim + sep.GetRemaining();
	return substring;
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

	const BotInfo *bi = findbot(nick);
	if (bi)
		return bot ? true : bi->HasFlag(BI_CORE);
	return false;
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

bool Anope::Match(const Anope::string &str, const Anope::string &mask, bool case_sensitive, bool use_regex)
{
	size_t s = 0, m = 0, str_len = str.length(), mask_len = mask.length();

	if (use_regex && mask_len >= 2 && mask[0] == '/' && mask[mask.length() - 1] == '/')
	{
		Anope::string stripped_mask = mask.substr(1, mask_len - 2);
		// This is often called with the same mask multiple times in a row, so cache it
		static Regex *r = NULL;

		if (r == NULL || r->GetExpression() != stripped_mask)
		{
			service_reference<RegexProvider> provider("Regex", Config->RegexEngine);
			if (provider)
			{
				try
				{
					delete r;
					r = NULL;
					// This may throw
					r = provider->Compile(stripped_mask);
				}
				catch (const RegexException &ex)
				{
					Log(LOG_DEBUG) << ex.GetReason();
				}
			}
			else
			{
				delete r;
				r = NULL;
			}
		}

		if (r != NULL && r->Matches(str))
			return true;

		// Fall through to non regex match
	}

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

	if (m < mask_len && mask[m] == '*')
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
Anope::string Anope::printf(const char *fmt, ...)
{
	va_list args;
	char buf[1024];
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	return buf;
}


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

int Anope::LastErrorCode()
{
#ifndef _WIN32
	return errno;
#else
	return GetLastError();
#endif
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

ModuleVersion Module::GetVersion() const
{
	return ModuleVersion(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
}

Anope::string Anope::Version()
{
#ifdef VERSION_GIT
	return stringify(VERSION_MAJOR) + "." + stringify(VERSION_MINOR) + "." + stringify(VERSION_PATCH) + VERSION_EXTRA + " (" + VERSION_GIT + ")";
#else
	return stringify(VERSION_MAJOR) + "." + stringify(VERSION_MINOR) + "." + stringify(VERSION_PATCH) + VERSION_EXTRA;
#endif
}

Anope::string Anope::VersionShort()
{
	return stringify(VERSION_MAJOR) + "." + stringify(VERSION_MINOR) + "." + stringify(VERSION_PATCH);
}

Anope::string Anope::VersionBuildString()
{
	return "build #" + stringify(BUILD) + ", compiled " + Anope::compiled;
}

int Anope::VersionMajor() { return VERSION_MAJOR; }
int Anope::VersionMinor() { return VERSION_MINOR; }
int Anope::VersionPatch() { return VERSION_PATCH; }

/**
 * Normalize buffer stripping control characters and colors
 * @param A string to be parsed for control and color codes
 * @return A string stripped of control and color codes
 */
Anope::string normalizeBuffer(const Anope::string &buf)
{
	Anope::string newbuf;

	for (unsigned i = 0, end = buf.length(); i < end; ++i)
	{
		switch (buf[i])
		{
			/* ctrl char */
			case 1:
			/* Bold ctrl char */
			case 2:
				break;
			/* Color ctrl char */
			case 3:
				/* If the next character is a digit, its also removed */
				if (isdigit(buf[i + 1]))
				{
					++i;

					/* not the best way to remove colors
					 * which are two digit but no worse then
					 * how the Unreal does with +S - TSL
					 */
					if (isdigit(buf[i + 1]))
						++i;

					/* Check for background color code
					 * and remove it as well
					 */
					if (buf[i + 1] == ',')
					{
						++i;

						if (isdigit(buf[i + 1]))
							++i;
						/* not the best way to remove colors
						 * which are two digit but no worse then
						 * how the Unreal does with +S - TSL
						 */
						if (isdigit(buf[i + 1]))
							++i;
					}
				}

				break;
			/* line feed char */
			case 10:
			/* carriage returns char */
			case 13:
			/* Reverse ctrl char */
			case 22:
			/* Italic ctrl char */
			case 29:
			/* Underline ctrl char */
			case 31:
				break;
			/* A valid char gets copied into the new buffer */
			default:
				newbuf += buf[i];
		}
	}

	return newbuf;
}


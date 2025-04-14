/* Miscellaneous routines.
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "build.h"
#include "modules.h"
#include "lists.h"
#include "config.h"
#include "bots.h"
#include "language.h"
#include "regexpr.h"
#include "sockets.h"

#include <cerrno>
#include <climits>
#include <numeric>
#include <random>
#include <filesystem>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef _WIN32
#include <sys/socket.h>
#include <netdb.h>
#endif

NumberList::NumberList(const Anope::string &list, bool descending) : desc(descending)
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
			if (auto num = Anope::TryConvert<unsigned>(token, &error))
			{
				if (error.empty())
					numbers.insert(num.value());
			}
			else
			{
				error = "1";
			}

			if (!error.empty())
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
			auto n1 = Anope::TryConvert<unsigned>(token.substr(0, t), &error);
			auto n2 = Anope::TryConvert<unsigned>(token.substr(t + 1), &error);
			if (n1.has_value() && n2.has_value())
			{
				auto num1 = n1.value();
				auto num2 = n2.value();
				if (error.empty() && error2.empty())
					for (unsigned i = num1; i <= num2; ++i)
						numbers.insert(i);
			}
			else
			{
				error = "1";
			}

			if (!error.empty() || !error2.empty())
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
		for (unsigned int number : numbers)
			this->HandleNumber(number);
	}
}

void NumberList::HandleNumber(unsigned)
{
}

bool NumberList::InvalidRange(const Anope::string &)
{
	return true;
}

ListFormatter::ListFormatter(NickCore *acc) : nc(acc)
{
}

ListFormatter &ListFormatter::AddColumn(const Anope::string &name)
{
	this->columns.push_back(name);
	return *this;
}

void ListFormatter::AddEntry(const ListEntry &entry)
{
	this->entries.push_back(entry);
}

bool ListFormatter::IsEmpty() const
{
	return this->entries.empty();
}

void ListFormatter::Process(std::vector<Anope::string> &buffer)
{
	std::vector<Anope::string> tcolumns;
	std::map<Anope::string, size_t> lengths;
	std::set<Anope::string> breaks;
	for (const auto &column : this->columns)
	{
		tcolumns.emplace_back(Language::Translate(this->nc, column.c_str()));
		lengths[column] = column.length();
	}
	for (auto &entry : this->entries)
	{
		for (const auto &column : this->columns)
		{
			if (entry[column].length() > lengths[column])
				lengths[column] = entry[column].length();
		}
	}
	const auto max_length = Config->GetBlock("options").Get<size_t>("linelength", "120");
	unsigned total_length = 0;
	for (const auto &[column, length] : lengths)
	{
		// Break lines that are getting too long.
		if (total_length > max_length)
		{
			breaks.insert(column);
			total_length = 0;
		}
		else
			total_length += length;
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
			s += tcolumns[i];
			if (i + 1 != this->columns.size())
				for (unsigned j = tcolumns[i].length(); j < lengths[this->columns[i]]; ++j)
					s += " ";
		}
		buffer.push_back(s);
	}

	for (auto &entry : this->entries)
	{
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
			s += entry[this->columns[j]];
			if (j + 1 != this->columns.size())
				for (unsigned k = entry[this->columns[j]].length(); k < lengths[this->columns[j]]; ++k)
					s += " ";
		}
		buffer.push_back(s);
	}
}

InfoFormatter::InfoFormatter(NickCore *acc) : nc(acc)
{
}

void InfoFormatter::Process(std::vector<Anope::string> &buffer)
{
	buffer.clear();

	for (const auto &[key, value] : this->replies)
	{
		Anope::string s;
		for (unsigned i = key.length(); i < this->longest; ++i)
			s += " ";
		s += key + ": " + Language::Translate(this->nc, value.c_str());

		buffer.push_back(s);
	}
}

Anope::string &InfoFormatter::operator[](const Anope::string &key)
{
	Anope::string tkey = Language::Translate(this->nc, key.c_str());
	if (tkey.length() > this->longest)
		this->longest = tkey.length();
	this->replies.emplace_back(tkey, "");
	return this->replies.back().second;
}

void InfoFormatter::AddOption(const Anope::string &opt)
{
	Anope::string options = Language::Translate(this->nc, "Options");
	Anope::string *optstr = NULL;
	for (auto &[option, value] : this->replies)
	{
		if (option == options)
		{
			optstr = &value;
			break;
		}
	}
	if (!optstr)
		optstr = &(*this)[_("Options")];

	if (!optstr->empty())
		*optstr += ", ";
	*optstr += Language::Translate(nc, opt.c_str());
}

bool Anope::IsFile(const Anope::string &filename)
{
	struct stat fileinfo;
	return stat(filename.c_str(), &fileinfo) == 0;
}

time_t Anope::DoTime(const Anope::string &s)
{
	if (s.empty())
		return 0;

	Anope::string end;
	auto amount = Anope::Convert<int>(s, -1, &end);
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
				break;
		}
	}

	return amount;
}

Anope::string Anope::Duration(time_t t, const NickCore *nc)
{
	/* We first calculate everything */
	time_t years = t / 31536000;
	time_t days = (t / 86400) % 365;
	time_t hours = (t / 3600) % 24;
	time_t minutes = (t / 60) % 60;
	time_t seconds = (t) % 60;

	Anope::string buffer;
	if (years)
	{
		buffer = Anope::printf(Language::Translate(nc, years, N_("%lld year", "%lld years")), (long long)years);
	}
	if (days)
	{
		buffer += buffer.empty() ? "" : ", ";
		buffer += Anope::printf(Language::Translate(nc, days, N_("%lld day", "%lld days")), (long long)days);
	}
	if (hours)
	{
		buffer += buffer.empty() ? "" : ", ";
		buffer += Anope::printf(Language::Translate(nc, hours, N_("%lld hour", "%lld hours")), (long long)hours);
	}
	if (minutes)
	{
		buffer += buffer.empty() ? "" : ", ";
		buffer += Anope::printf(Language::Translate(nc, minutes, N_("%lld minute", "%lld minutes")), (long long)minutes);
	}
	if (seconds || buffer.empty())
	{
		buffer += buffer.empty() ? "" : ", ";
		buffer += Anope::printf(Language::Translate(nc, seconds, N_("%lld second", "%lld seconds")), (long long)seconds);
	}
	return buffer;
}

Anope::string Anope::strftime(time_t t, const NickCore *nc, bool short_output)
{
	tm tm = *localtime(&t);
	char buf[BUFSIZE];
	strftime(buf, sizeof(buf), Language::Translate(nc, _("%b %d %Y %H:%M:%S %Z")), &tm);
	if (short_output)
		return buf;
	if (t < Anope::CurTime)
		return Anope::string(buf) + " " + Anope::printf(Language::Translate(nc, _("(%s ago)")), Duration(Anope::CurTime - t, nc).c_str(), nc);
	else if (t > Anope::CurTime)
		return Anope::string(buf) + " " + Anope::printf(Language::Translate(nc, _("(%s from now)")), Duration(t - Anope::CurTime, nc).c_str(), nc);
	else
		return Anope::string(buf) + " " + Language::Translate(nc, _("(now)"));
}

Anope::string Anope::Expires(time_t expires, const NickCore *nc)
{
	if (!expires)
		return Language::Translate(nc, NO_EXPIRE);

	if (expires <= Anope::CurTime)
		return Language::Translate(nc, _("expires momentarily"));

	// This will get inlined when compiled with optimisations.
	auto nearest = [](auto timeleft, auto roundto) {
		if ((timeleft % roundto) <= (roundto / 2))
			return timeleft - (timeleft % roundto);
		return timeleft - (timeleft % roundto) + roundto;
	};

	// In order to get a shorter result we round to the nearest period.
	auto timeleft = expires - Anope::CurTime;
	if (timeleft >= 31536000)
		timeleft = nearest(timeleft, 86400); // Nearest day if its more than a year
	else if (timeleft >= 86400)
		timeleft = nearest(timeleft, 3600); // Nearest hour if its more than a day
	else if (timeleft >= 3600)
		timeleft = nearest(timeleft, 60); // Nearest minute if its more than an hour

	auto duration = Anope::Duration(timeleft, nc);
	return Anope::printf(Language::Translate(nc, _("expires in %s")), duration.c_str());
}

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
			ServiceReference<RegexProvider> provider("Regex", Config->GetBlock("options").Get<const Anope::string>("regexengine"));
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
			if (Anope::tolower(wild) != Anope::tolower(string) && wild != '?')
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
			if (Anope::tolower(wild) == Anope::tolower(string) || wild == '?')
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

bool Anope::Encrypt(const Anope::string &src, Anope::string &dest)
{
	EventReturn MOD_RESULT;
	FOREACH_RESULT(OnEncrypt, MOD_RESULT, (src, dest));
	return MOD_RESULT == EVENT_ALLOW &&!dest.empty();
}

Anope::string Anope::printf(const char *fmt, ...)
{
	va_list args;
	char buf[1024];
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	return buf;
}

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

void Anope::Unhex(const Anope::string &src, Anope::string &dest)
{
	size_t len = src.length();
	Anope::string rv;
	for (size_t i = 0; i + 1 < len; i += 2)
	{
		char h = Anope::tolower(src[i]), l = Anope::tolower(src[i + 1]);
		unsigned char byte = (h >= 'a' ? h - 'a' + 10 : h - '0') << 4;
		byte += (l >= 'a' ? l - 'a' + 10 : l - '0');
		rv += byte;
	}
	dest = rv;
}

void Anope::Unhex(const Anope::string &src, char *dest, size_t sz)
{
	Anope::string d;
	Anope::Unhex(src, d);

	memcpy(dest, d.c_str(), std::min(d.length() + 1, sz));
}

int Anope::LastErrorCode()
{
#ifndef _WIN32
	return errno;
#else
	return GetLastError();
#endif
}

Anope::string Anope::LastError()
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

Anope::string Anope::Version()
{
#ifdef VERSION_GIT
	return Anope::ToString(VERSION_MAJOR) + "." + Anope::ToString(VERSION_MINOR) + "." + Anope::ToString(VERSION_PATCH) + VERSION_EXTRA + " (" + VERSION_GIT + ")";
#else
	return Anope::ToString(VERSION_MAJOR) + "." + Anope::ToString(VERSION_MINOR) + "." + Anope::ToString(VERSION_PATCH) + VERSION_EXTRA;
#endif
}

Anope::string Anope::VersionShort()
{
	return Anope::ToString(VERSION_MAJOR) + "." + Anope::ToString(VERSION_MINOR) + "." + Anope::ToString(VERSION_PATCH);
}

Anope::string Anope::VersionBuildString()
{
#if REPRODUCIBLE_BUILD
	Anope::string s = "build #" + Anope::ToString(BUILD);
#else
	Anope::string s = "build #" + Anope::ToString(BUILD) + ", compiled " + Anope::compiled;
#endif
	Anope::string flags;

#if DEBUG_BUILD
	flags += "D";
#endif
#ifdef VERSION_GIT
	flags += "G";
#endif
#if REPRODUCIBLE_BUILD
	flags += "R"
#endif
#ifdef _WIN32
	flags += "W";
#endif

	if (!flags.empty())
		s += ", flags " + flags;

	return s;
}

int Anope::VersionMajor() { return VERSION_MAJOR; }
int Anope::VersionMinor() { return VERSION_MINOR; }
int Anope::VersionPatch() { return VERSION_PATCH; }

Anope::string Anope::NormalizeBuffer(const Anope::string &buf)
{
	Anope::string newbuf;
	for (size_t idx = 0; idx < buf.length(); )
	{
		switch (buf[idx])
		{
			case '\x02': // Bold
			case '\x1D': // Italic
			case '\x11': // Monospace
			case '\x16': // Reverse
			case '\x1E': // Strikethrough
			case '\x1F': // Underline
			case '\x0F': // Reset
				idx++;
				break;

			case '\x03': // Color
			{
				const auto start = idx;
				while (++idx < buf.length() && idx - start < 6)
				{
					const auto chr = buf[idx];
					if (chr != ',' && (chr < '0' || chr > '9'))
						break;
				}
				break;
			}
			case '\x04': // Hex Color
			{
				const auto start = idx;
				while (++idx < buf.length() && idx - start < 14)
				{
					const auto chr = buf[idx];
					if (chr != ',' && (chr < '0' || chr > '9') && (chr < 'A' || chr > 'F') && (chr < 'a' || chr > 'f'))
						break;
				}
				break;
			}

			default: // Non-formatting character.
				newbuf.push_back(buf[idx++]);
				break;
		}
	}
	return newbuf;
}

Anope::string Anope::Resolve(const Anope::string &host, int type)
{
	std::vector<Anope::string> results = Anope::ResolveMultiple(host, type);
	return results.empty() ? host : results[0];
}

std::vector<Anope::string> Anope::ResolveMultiple(const Anope::string &host, int type)
{
	std::vector<Anope::string> results;

	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = type;

	Log(LOG_DEBUG_2) << "Resolver: BlockingQuery: Looking up " << host;

	addrinfo *addrresult = NULL;
	if (getaddrinfo(host.c_str(), NULL, &hints, &addrresult) == 0)
	{
		for (addrinfo *thisresult = addrresult; thisresult; thisresult = thisresult->ai_next)
		{
			sockaddrs addr;
			memcpy(static_cast<void*>(&addr), thisresult->ai_addr, thisresult->ai_addrlen);

			results.push_back(addr.addr());
			Log(LOG_DEBUG_2) << "Resolver: " << host << " -> " << addr.addr();
		}

		freeaddrinfo(addrresult);
	}

	return results;
}

Anope::string Anope::Random(size_t len)
{
	char chars[] = {
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
		'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
		'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
		'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
		'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
	};
	Anope::string buf;
	for (size_t i = 0; i < len; ++i)
		buf.append(chars[Anope::RandomNumber() % sizeof(chars)]);
	return buf;
}

int Anope::RandomNumber()
{
	static std::random_device device;
	static std::mt19937 engine(device());
	static std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
	return dist(engine);
}

// Implementation of https://en.wikipedia.org/wiki/Levenshtein_distance
size_t Anope::Distance(const Anope::string &s1, const Anope::string &s2)
{
	if (s1.empty())
		return s2.length();
	if (s2.empty())
		return s1.length();

	std::vector<size_t> costs(s2.length() + 1);
	std::iota(costs.begin(), costs.end(), 0);

	size_t i = 0;
	for (const auto c1 : s1)
	{
		costs[0] = i + 1;
		size_t corner = i;
		size_t j = 0;
		for (const auto &c2 : s2)
		{
			size_t upper = costs[j + 1];
			if (c1 == c2)
				costs[j + 1] = corner;
			else
			{
				size_t t = upper < corner ? upper : corner;
				costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
			}
			corner = upper;
			j++;
		}
		i++;
	}
	return costs[s2.length()];
}

void Anope::UpdateTime()
{
#ifdef _WIN32
	SYSTEMTIME st;
	GetSystemTime(&st);

	CurTime = time(nullptr);
	CurTimeNs = st.wMilliseconds;
#elif HAVE_CLOCK_GETTIME
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);

	CurTime = ts.tv_sec;
	CurTimeNs = ts.tv_nsec;
#else
	struct timeval tv;
	gettimeofday(&tv, nullptr);

	CurTime = tv.tv_sec;
	CurTimeNs = tv.tv_usec * 1000;
#endif
}

Anope::string Anope::Expand(const Anope::string &base, const Anope::string &fragment)
{
	if (fragment.empty())
		return ""; // We can't expand an empty fragment.

	// The fragment is an absolute path, don't modify it.
	if (std::filesystem::path(fragment.str()).is_absolute())
		return fragment;

#ifdef _WIN32
	static constexpr const char separator = '\\';
#else
	static constexpr const char separator = '/';
#endif

	// The fragment is relative to a home directory, expand that.
	if (!fragment.compare(0, 2, "~/", 2))
	{
		const auto *homedir = getenv("HOME");
		if (homedir && *homedir)
			return Anope::printf("%s%c%s", homedir, separator, fragment.c_str() + 2);
	}

	return Anope::printf("%s%c%s", base.c_str(), separator, fragment.c_str());
}

Anope::string Anope::FormatCTCP(const Anope::string &name, const Anope::string &value)
{
	if (value.empty())
		return Anope::printf("\1%s\1", name.c_str());

	return Anope::printf("\1%s %s\1", name.c_str(), value.c_str());
}

bool Anope::ParseCTCP(const Anope::string &text, Anope::string &name, Anope::string &body)
{
	// According to draft-oakley-irc-ctcp-02 a valid CTCP must begin with SOH and
	// contain at least one octet which is not NUL, SOH, CR, LF, or SPACE. As most
	// of these are restricted at the protocol level we only need to check for SOH
	// and SPACE.
	if (text.length() < 2 || text[0] != '\x1' || text[1] == '\x1' || text[1] == ' ')
	{
		name.clear();
		body.clear();
		return false;
	}

	auto end_of_name = text.find(' ', 2);
	auto end_of_ctcp = *text.rbegin() == '\x1' ? 1 : 0;
	if (end_of_name == std::string::npos)
	{
		// The CTCP only contains a name.
		name = text.substr(1, text.length() - 1 - end_of_ctcp);
		body.clear();
		return true;
	}

	// The CTCP contains a name and a body.
	name = text.substr(1, end_of_name - 1);

	auto start_of_body = text.find_first_not_of(' ', end_of_name + 1);
	if (start_of_body == std::string::npos)
	{
		// The CTCP body is provided but empty.
		body.clear();
		return true;
	}

	// The CTCP body provided was non-empty.
	body = text.substr(start_of_body, text.length() - start_of_body - end_of_ctcp);
	return true;
}

Anope::string Anope::Template(const Anope::string &str, const Anope::map<Anope::string> &vars)
{
	Anope::string out;
	for (size_t idx = 0; idx < str.length(); ++idx)
	{
		if (str[idx] != '{')
		{
			out.push_back(str[idx]);
			continue;
		}

		for (size_t endidx = idx + 1; endidx < str.length(); ++endidx)
		{
			if (str[endidx] == '}')
			{
				if (endidx - idx == 1)
				{
					// foo{{bar is an escape of foo{bar
					out.push_back('{');
					idx = endidx;
					break;
				}

				auto var = vars.find(str.substr(idx + 1, endidx - idx - 1));
				if (var != vars.end())
				{
					// We have a variable, replace it in the string.
					out.append(var->second);
				}

				idx = endidx;
				break;
			}
		}
	}
	return out;
}

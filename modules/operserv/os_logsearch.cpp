/* OperServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

static unsigned int HARDMAX = 65536;

class CommandOSLogSearch final
	: public Command
{
	static inline Anope::string CreateLogName(const Anope::string &file, time_t t = Anope::CurTime)
	{
		char timestamp[32];

		tm *tm = localtime(&t);

		strftime(timestamp, sizeof(timestamp), "%Y%m%d", tm);

		return Anope::ExpandLog(file + "." + timestamp);
	}

public:
	CommandOSLogSearch(Module *creator) : Command(creator, "operserv/logsearch", 1, 3)
	{
		this->SetDesc(_("Searches logs for a matching pattern"));
		this->SetSyntax(_("[+\037days\037d] [+\037limit\037l] \037pattern\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		int days = 7, replies = 50;

		unsigned i;
		for (i = 0; i < params.size() && params[i][0] == '+'; ++i)
		{
			switch (params[i][params[i].length() - 1])
			{
				case 'd':
					if (params[i].length() > 2)
					{
						Anope::string dur = params[i].substr(1, params[i].length() - 2);
						auto d = Anope::Convert<int>(dur, 0);
						if (d > 0)
							days = d;
						else
							source.Reply(_("Invalid duration %s, using %d days."), dur.c_str(), days);
					}
					break;
				case 'l':
					if (params[i].length() > 2)
					{
						Anope::string dur = params[i].substr(1, params[i].length() - 2);
						auto r = Anope::Convert<int>(dur, 0);
						if (r > 0)
							replies = r;
						else
							source.Reply(_("Invalid limit %s, using %d."), dur.c_str(), replies);
					}
					break;
				default:
					source.Reply(_("Unknown parameter: %s"), params[i].c_str());
			}
		}

		if (i >= params.size())
		{
			this->OnSyntaxError(source, "");
			return;
		}

		Anope::string search_string = params[i++];
		for (; i < params.size(); ++i)
			search_string += " " + params[i];

		Log(LOG_ADMIN, source, this) << "for " << search_string;

		bool wildcard = search_string.find_first_of("?*") != Anope::string::npos;
		bool regex = !search_string.empty() && search_string[0] == '/' && search_string[search_string.length() - 1] == '/';

		const Anope::string &logfile_name = Config->GetModule(this->owner).Get<const Anope::string>("logname");
		std::vector<Anope::string> matches;
		for (int d = days - 1; d >= 0; --d)
		{
			Anope::string lf_name = CreateLogName(logfile_name, Anope::CurTime - (d * 86400));
			Log(LOG_DEBUG) << "Searching " << lf_name;
			std::fstream fd(lf_name.c_str(), std::ios_base::in);
			if (!fd.is_open())
				continue;

			for (Anope::string buf, token; std::getline(fd, buf.str());)
			{
				bool match = false;

				if (regex)
					match = Anope::Match(buf, search_string, false, true);
				else if (wildcard)
					match = Anope::Match(buf, "*" + search_string + "*");
				else
					match = buf.find_ci(search_string) != Anope::string::npos;

				if (match)
				{
					matches.push_back(buf);

					if (matches.size() >= HARDMAX)
						break;
				}
			}

			fd.close();
		}

		size_t found = matches.size();
		if (!found)
		{
			source.Reply(_("No matches for \002%s\002 found."), search_string.c_str());
			return;
		}

		if (matches.size() >= HARDMAX)
		{
			source.Reply(_("Too many results for \002%s\002."), search_string.c_str());
			return;
		}

		if (matches.size() > static_cast<unsigned int>(replies))
		{
			matches.erase(matches.begin(), matches.begin() + (matches.size() - static_cast<unsigned int>(replies)));
		}

		source.Reply(_("Matches for \002%s\002:"), search_string.c_str());
		size_t count = 0;
		for (const auto &match : matches)
			source.Reply("#%zu: %s", ++count, match.c_str());
		source.Reply(_("Showed %zu/%zu matches for \002%s\002."), matches.size(), found, search_string.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"This command searches the services logfiles for messages "
			"that match the given pattern. The day and limit argument "
			"may be used to specify how many days of logs to search "
			"and the number of replies to limit to. By default this "
			"command searches one week of logs, and limits replies "
			"to 50."
			"\n\n"
			"For example:\n"
			"    \002LOGSEARCH\032+21d\032+500l\032Anope\002\n"
			"      Searches the last 21 days worth of logs for messages\n"
			"      containing Anope and lists the most recent 500 of them."
		));
		return true;
	}
};

class OSLogSearch final
	: public Module
{
	CommandOSLogSearch commandoslogsearch;

public:
	OSLogSearch(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandoslogsearch(this)
	{
	}
};

MODULE_INIT(OSLogSearch)

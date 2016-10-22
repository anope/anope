/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"

class CommandOSLogSearch : public Command
{
	static inline Anope::string CreateLogName(const Anope::string &file, time_t t = Anope::CurTime)
	{
		char timestamp[32];

		tm *tm = localtime(&t);

		strftime(timestamp, sizeof(timestamp), "%Y%m%d", tm);

		return Anope::LogDir + "/" + file + "." + timestamp;
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
						try
						{
							days = convertTo<int>(dur);
							if (days <= 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							source.Reply(_("Invalid duration \002{0}\002, using \002{1}\002 days."), dur, days);
						}
					}
					break;
				case 'l':
					if (params[i].length() > 2)
					{
						Anope::string dur = params[i].substr(1, params[i].length() - 2);
						try
						{
							replies = convertTo<int>(dur);
							if (replies <= 0)
								throw ConvertException();
						}
						catch (const ConvertException &)
						{
							source.Reply(_("Invalid limit \002{0}\002, using \002{1}\002."), dur, replies);
						}
					}
					break;
				default:
					source.Reply(_("Unknown parameter: {0}"), params[i]);
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

		const Anope::string &logfile_name = Config->GetModule(this->GetOwner())->Get<Anope::string>("logname");
		std::list<Anope::string> matches;
		for (int d = days - 1; d >= 0; --d)
		{
			Anope::string lf_name = CreateLogName(logfile_name, Anope::CurTime - (d * 86400));
			Log(LOG_DEBUG) << "Searching " << lf_name;
			std::fstream fd(lf_name.c_str(), std::ios_base::in);
			if (!fd.is_open())
				continue;

			for (Anope::string buf; std::getline(fd, buf.str());)
				if (Anope::Match(buf, "*" + search_string + "*"))
					matches.push_back(buf);

			fd.close();
		}

		unsigned found = matches.size();
		if (!found)
		{
			source.Reply(_("No matches for \002{0}\002 found."), search_string);
			return;
		}

		while (matches.size() > static_cast<unsigned>(replies))
			matches.pop_front();

		source.Reply(_("Matches for \002{0}\002:"), search_string);
		unsigned count = 0;
		for (const Anope::string &str : matches)
			source.Reply("#{0}: {1}", ++count, str);
		source.Reply(_("Showed \002{0}/{1}\002 matches for \002{2}\002."), matches.size(), found, search_string);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("This command searches the Services logfiles for messages that match the given pattern."
		               " The day and limit argument may be used to specify how many days of logs to search and the number of replies to limit to."
		               " By default this command searches one week of logs, and limits replies to 50.\n"
		               "\n"
		               "Example:\n"
		               "         {0} +21d +500l Anope\n"
		               "         Searches the last 21 days worth of logs for messages containing \"Anope\" and lists the most recent 500 of them."),
		               source.command);
		return true;
	}
};

class OSLogSearch : public Module
{
	CommandOSLogSearch commandoslogsearch;

 public:
	OSLogSearch(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandoslogsearch(this)
	{
	}
};

MODULE_INIT(OSLogSearch)

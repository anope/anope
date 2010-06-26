/* HostServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandHSList : public Command
{
 public:
	CommandHSList() : Command("LIST", 0, 1, "hostserv/list")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		ci::string key = params.size() ? params[0] : "";
		int from = 0, to = 0, counter = 1;
		unsigned display_counter = 0;
		tm *tm;
		char buf[BUFSIZE];

		/**
		 * Do a check for a range here, then in the next loop
		 * we'll only display what has been requested..
		 **/
		if (!key.empty() && key[0] == '#')
		{
			size_t tmp = key.find('-');
			if (tmp == ci::string::npos || tmp == key.size() || tmp == 1)
			{
				notice_lang(Config.s_HostServ, u, LIST_INCORRECT_RANGE);
				return MOD_CONT;
			}
			for (unsigned i = 1, end = key.size(); i < end; ++i)
			{
				if (!isdigit(key[i]) && i != tmp)
				{
					notice_lang(Config.s_HostServ, u, LIST_INCORRECT_RANGE);
					return MOD_CONT;
				}
				from = atoi(key.substr(1, tmp - 1).c_str());
				to = atoi(key.substr(tmp + 1, key.size()).c_str());
			}
		}

		for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; ++it)
		{
			NickAlias *na = it->second;

			if (!na->hostinfo.HasVhost())
				continue;

			if (!key.empty() && key[0] != '#')
			{
				if ((Anope::Match(na->nick, key) || Anope::Match(na->hostinfo.GetHost(), key.c_str())) && display_counter < Config.NSListMax)
				{
					++display_counter;
					time_t time = na->hostinfo.GetTime();
					tm = localtime(&time);
					strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
					if (!na->hostinfo.GetIdent().empty())
						notice_lang(Config.s_HostServ, u, HOST_IDENT_ENTRY, counter, na->nick, na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str(), na->hostinfo.GetCreator().c_str(), buf);
					else
						notice_lang(Config.s_HostServ, u, HOST_ENTRY, counter, na->nick, na->hostinfo.GetHost().c_str(), na->hostinfo.GetCreator().c_str(), buf);
				}
			}
			else
			{
				/**
				 * List the host if its in the display range, and not more
				 * than NSListMax records have been displayed...
				 **/
				if (((counter >= from && counter <= to) || (!from && !to)) && display_counter < Config.NSListMax)
				{
					++display_counter;
					time_t time = na->hostinfo.GetTime();
					tm = localtime(&time);
					strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
					if (!na->hostinfo.GetIdent().empty())
						notice_lang(Config.s_HostServ, u, HOST_IDENT_ENTRY, counter, na->nick, na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str(), na->hostinfo.GetCreator().c_str(), buf);
					else
						notice_lang(Config.s_HostServ, u, HOST_ENTRY, counter, na->nick, na->hostinfo.GetHost().c_str(), na->hostinfo.GetCreator().c_str(), buf);
				}
			}
			++counter;
		}
		if (!key.empty())
			notice_lang(Config.s_HostServ, u, HOST_LIST_KEY_FOOTER, key.c_str(), display_counter);
		else
		{
			if (from)
				notice_lang(Config.s_HostServ, u, HOST_LIST_RANGE_FOOTER, from, to);
			else
				notice_lang(Config.s_HostServ, u, HOST_LIST_FOOTER, display_counter);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_HostServ, u, HOST_HELP_LIST);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_HostServ, u, HOST_HELP_CMD_LIST);
	}
};

class HSList : public Module
{
 public:
	HSList(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion(VERSION_STRING);
		this->SetType(CORE);

		this->AddCommand(HostServ, new CommandHSList());
	}
};

MODULE_INIT(HSList)

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

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &key = !params.empty() ? params[0] : "";
		int from = 0, to = 0, counter = 1;
		unsigned display_counter = 0;

		/**
		 * Do a check for a range here, then in the next loop
		 * we'll only display what has been requested..
		 **/
		if (!key.empty() && key[0] == '#')
		{
			size_t tmp = key.find('-');
			if (tmp == Anope::string::npos || tmp == key.length() || tmp == 1)
			{
				source.Reply(LIST_INCORRECT_RANGE);
				return MOD_CONT;
			}
			for (unsigned i = 1, end = key.length(); i < end; ++i)
			{
				if (!isdigit(key[i]) && i != tmp)
				{
					source.Reply(LIST_INCORRECT_RANGE);
					return MOD_CONT;
				}
				from = convertTo<int>(key.substr(1, tmp - 1));
				to = convertTo<int>(key.substr(tmp + 1));
			}
		}

		for (nickalias_map::const_iterator it = NickAliasList.begin(), it_end = NickAliasList.end(); it != it_end; ++it)
		{
			NickAlias *na = it->second;

			if (!na->hostinfo.HasVhost())
				continue;

			if (!key.empty() && key[0] != '#')
			{
				if ((Anope::Match(na->nick, key) || Anope::Match(na->hostinfo.GetHost(), key)) && display_counter < Config->NSListMax)
				{
					++display_counter;
					if (!na->hostinfo.GetIdent().empty())
						source.Reply(HOST_IDENT_ENTRY, counter, na->nick.c_str(), na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str(), na->hostinfo.GetCreator().c_str(), do_strftime(na->hostinfo.GetTime()).c_str());
					else
						source.Reply(HOST_ENTRY, counter, na->nick.c_str(), na->hostinfo.GetHost().c_str(), na->hostinfo.GetCreator().c_str(), do_strftime(na->hostinfo.GetTime()).c_str());
				}
			}
			else
			{
				/**
				 * List the host if its in the display range, and not more
				 * than NSListMax records have been displayed...
				 **/
				if (((counter >= from && counter <= to) || (!from && !to)) && display_counter < Config->NSListMax)
				{
					++display_counter;
					if (!na->hostinfo.GetIdent().empty())
						source.Reply(HOST_IDENT_ENTRY, counter, na->nick.c_str(), na->hostinfo.GetIdent().c_str(), na->hostinfo.GetHost().c_str(), na->hostinfo.GetCreator().c_str(), do_strftime(na->hostinfo.GetTime()).c_str());
					else
						source.Reply(HOST_ENTRY, counter, na->nick.c_str(), na->hostinfo.GetHost().c_str(), na->hostinfo.GetCreator().c_str(), do_strftime(na->hostinfo.GetTime()).c_str());
				}
			}
			++counter;
		}
		if (!key.empty())
			source.Reply(HOST_LIST_KEY_FOOTER, key.c_str(), display_counter);
		else
		{
			if (from)
				source.Reply(HOST_LIST_RANGE_FOOTER, from, to);
			else
				source.Reply(HOST_LIST_FOOTER, display_counter);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(HostServ, HOST_HELP_LIST);
		return true;
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(HostServ, HOST_HELP_CMD_LIST);
	}
};

class HSList : public Module
{
	CommandHSList commandhslist;

 public:
	HSList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(HostServ, &commandhslist);
	}
};

MODULE_INIT(HSList)

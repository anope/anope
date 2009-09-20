/* HostServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

class CommandHSList : public Command
{
 public:
	CommandHSList() : Command("LIST", 0, 1, "hostserv/list")
	{
	}

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		const char *key = params.size() ? params[0].c_str() : NULL;
		struct tm *tm;
		char buf[BUFSIZE];
		int counter = 1;
		int from = 0, to = 0;
		char *tmp = NULL;
		char *s = NULL;
		unsigned display_counter = 0;
		HostCore *head = NULL;
		HostCore *current;

		head = hostCoreListHead();

		current = head;
		if (!current)
			notice_lang(s_HostServ, u, HOST_EMPTY);
		else
		{
			/**
			 * Do a check for a range here, then in the next loop
			 * we'll only display what has been requested..
			 **/
			if (key)
			{
				if (key[0] == '#')
				{
					tmp = myStrGetOnlyToken((key + 1), '-', 0); /* Read FROM out */
					if (!tmp)
					{
						notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
						return MOD_CONT;
					}
					for (s = tmp; *s; ++s)
					{
						if (!isdigit(*s))
						{
							delete [] tmp;
							notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
							return MOD_CONT;
						}
					}
					from = atoi(tmp);
					delete [] tmp;
					tmp = myStrGetTokenRemainder(key, '-', 1); /* Read TO out */
					if (!tmp)
					{
						notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
						return MOD_CONT;
					}
					for (s = tmp; *s; ++s)
					{
						if (!isdigit(*s))
						{
							delete [] tmp;
							notice_lang(s_ChanServ, u, LIST_INCORRECT_RANGE);
							return MOD_CONT;
						}
					}
					to = atoi(tmp);
					delete [] tmp;
					key = NULL;
				}
			}

			while (current)
			{
				if (key)
				{
					if ((Anope::Match(current->nick, key, false) || Anope::Match(current->vHost, key, false)) && display_counter < NSListMax)
					{
						++display_counter;
						tm = localtime(&current->time);
						strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
						if (current->vIdent)
							notice_lang(s_HostServ, u, HOST_IDENT_ENTRY, counter, current->nick, current->vIdent, current->vHost, current->creator, buf);
						else
							notice_lang(s_HostServ, u, HOST_ENTRY, counter, current->nick, current->vHost, current->creator, buf);
					}
				}
				else
				{
					/**
					 * List the host if its in the display range, and not more
					 * than NSListMax records have been displayed...
					 **/
					if (((counter >= from && counter <= to) || (!from && !to)) && display_counter < NSListMax)
					{
						++display_counter;
						tm = localtime(&current->time);
						strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
						if (current->vIdent)
							notice_lang(s_HostServ, u, HOST_IDENT_ENTRY, counter, current->nick, current->vIdent, current->vHost, current->creator, buf);
						else
							notice_lang(s_HostServ, u, HOST_ENTRY, counter, current->nick, current->vHost, current->creator, buf);
					}
				}
				++counter;
				current = current->next;
			}
			if (key)
				notice_lang(s_HostServ, u, HOST_LIST_KEY_FOOTER, key, display_counter);
			else {
				if (from)
					notice_lang(s_HostServ, u, HOST_LIST_RANGE_FOOTER, from, to);
				else
					notice_lang(s_HostServ, u, HOST_LIST_FOOTER, display_counter);
			}
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_HostServ, u, HOST_HELP_LIST);
		return true;
	}
};

class HSList : public Module
{
 public:
	HSList(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(HOSTSERV, new CommandHSList());
	}
	void HostServHelp(User *u)
	{
		notice_lang(s_HostServ, u, HOST_HELP_CMD_LIST);
	}
};

MODULE_INIT(HSList)

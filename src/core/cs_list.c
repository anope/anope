/* ChanServ core functions
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
#include "hashcomp.h"

class CommandCSList : public Command
{
public:
	CommandCSList() : Command("LIST", 1, 2)
	{
		this->SetFlag(CFLAG_STRIP_CHANNEL);
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *pattern = params[0].c_str();
		int spattern_size;
		char *spattern;
		ChannelInfo *ci;
		unsigned nchans, i;
		char buf[BUFSIZE];
		bool is_servadmin = u->nc->HasCommand("chanserv/list");
		int count = 0, from = 0, to = 0, tofree = 0;
		char *tmp = NULL;
		char *s = NULL;
		bool forbidden = false, suspended = false, channoexpire = false;

		if (Config.CSListOpersOnly && !is_oper(u))
		{
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
			return MOD_STOP;
		}

		if (pattern[0] == '#')
		{
			tmp = myStrGetOnlyToken((pattern + 1), '-', 0); /* Read FROM out */
			if (!tmp)
			{
				notice_lang(Config.s_ChanServ, u, LIST_INCORRECT_RANGE);
				notice_lang(Config.s_ChanServ, u, CS_LIST_INCORRECT_RANGE);
				return MOD_CONT;
			}
			for (s = tmp; *s; s++)
			{
				if (!isdigit(*s))
				{
					delete [] tmp;
					notice_lang(Config.s_ChanServ, u, LIST_INCORRECT_RANGE);
					notice_lang(Config.s_ChanServ, u, CS_LIST_INCORRECT_RANGE);
					return MOD_CONT;
				}
			}
			from = atoi(tmp);
			delete [] tmp;
			tmp = myStrGetTokenRemainder(pattern, '-', 1);  /* Read TO out */
			if (!tmp)
			{
				notice_lang(Config.s_ChanServ, u, LIST_INCORRECT_RANGE);
				notice_lang(Config.s_ChanServ, u, CS_LIST_INCORRECT_RANGE);
				return MOD_CONT;
			}
			for (s = tmp; *s; s++)
			{
				if (!isdigit(*s))
				{
					delete [] tmp;
					notice_lang(Config.s_ChanServ, u, LIST_INCORRECT_RANGE);
					notice_lang(Config.s_ChanServ, u, CS_LIST_INCORRECT_RANGE);
					return MOD_CONT;
				}
			}
			to = atoi(tmp);
			delete [] tmp;
			pattern = sstrdup("*");
			tofree = 1;
		}

		nchans = 0;

		if (is_servadmin && params.size() > 1)
		{
			std::string keyword;
			spacesepstream keywords(params[1].c_str());
			while (keywords.GetToken(keyword))
			{
				ci::string keyword_ci = keyword.c_str();
				if (keyword_ci == "FORBIDDEN")
					forbidden = true;
				if (keyword_ci == "SUSPENDED")
					suspended = true;
				if (keyword_ci == "NOEXPIRE")
					channoexpire = true;

			}
		}

		spattern_size = (strlen(pattern) + 2) * sizeof(char);
		spattern = new char[spattern_size];
		snprintf(spattern, spattern_size, "#%s", pattern);

		notice_lang(Config.s_ChanServ, u, CHAN_LIST_HEADER, pattern);
		for (i = 0; i < 256; i++)
		{
			for (ci = chanlists[i]; ci; ci = ci->next)
			{
				if (!is_servadmin && ((ci->HasFlag(CI_PRIVATE))
					|| (ci->HasFlag(CI_FORBIDDEN)) || (ci->HasFlag(CI_SUSPENDED))))
					continue;
				if (forbidden && !ci->HasFlag(CI_FORBIDDEN))
					continue;
				else if (suspended && !ci->HasFlag(CI_SUSPENDED))
					continue;
				else if (channoexpire && !ci->HasFlag(CI_NO_EXPIRE))
					continue;

				if ((stricmp(pattern, ci->name) == 0)
					  || (stricmp(spattern, ci->name) == 0)
					  || Anope::Match(ci->name, pattern, false)
					  || Anope::Match(ci->name, spattern, false))
				{
					if ((((count + 1 >= from) && (count + 1 <= to))
						  || ((from == 0) && (to == 0)))
						  && (++nchans <= Config.CSListMax))
					{
						char noexpire_char = ' ';
						if (is_servadmin && (ci->HasFlag(CI_NO_EXPIRE)))
							noexpire_char = '!';

						if (ci->HasFlag(CI_FORBIDDEN))
						{
							snprintf(buf, sizeof(buf),
								   "%-20s  [Forbidden]", ci->name);
						}
						else if (ci->HasFlag(CI_SUSPENDED))
						{
							snprintf(buf, sizeof(buf),
								   "%-20s  [Suspended]", ci->name);
						}
						else
						{
							snprintf(buf, sizeof(buf), "%-20s  %s",
								   ci->name, ci->desc ? ci->desc : "");
						}

						u->SendMessage(Config.s_ChanServ, "  %c%s", noexpire_char, buf);
					}
					count++;
				}
			}
		}
		notice_lang(Config.s_ChanServ, u, CHAN_LIST_END,
				nchans > Config.CSListMax ? Config.CSListMax : nchans, nchans);
		delete [] spattern;
		if (tofree)
			delete [] pattern;
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_LIST);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "LIST", CHAN_LIST_SYNTAX);
	}
};

class CSList : public Module
{
public:
	CSList(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSList());

		ModuleManager::Attach(I_OnChanServHelp, this);
	}
	void OnChanServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_LIST);
	}
};

MODULE_INIT(CSList)

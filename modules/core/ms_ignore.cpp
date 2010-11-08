/* MemoServ core functions
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

class CommandMSIgnore : public Command
{
 public:
	CommandMSIgnore() : Command("IGNORE", 1, 3)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string channel = params[0];
		Anope::string command = (params.size() > 1 ? params[1] : "");
		Anope::string param = (params.size() > 2 ? params[2] : "");

		if (channel[0] != '#')
		{
			param = command;
			command = channel;
			channel = u->nick;
		}

		bool ischan, isforbid;
		MemoInfo *mi = getmemoinfo(channel, ischan, isforbid);
		if (!mi)
		{
			if (isforbid)
				u->SendMessage(MemoServ, ischan ? CHAN_X_FORBIDDEN : NICK_X_FORBIDDEN, channel.c_str());
			else
				u->SendMessage(MemoServ, ischan ? CHAN_X_NOT_REGISTERED : NICK_X_NOT_REGISTERED, channel.c_str());
		}
		else if (ischan && !check_access(u, cs_findchan(channel), CA_MEMO))
			u->SendMessage(MemoServ, ACCESS_DENIED);
		else if (command.equals_ci("ADD") && !param.empty())
		{
			if (std::find(mi->ignores.begin(), mi->ignores.end(), param.ci_str()) == mi->ignores.end())
			{
				mi->ignores.push_back(param.ci_str());
				u->SendMessage(MemoServ, MEMO_IGNORE_ADD, param.c_str());
			}
			else
				u->SendMessage(MemoServ, MEMO_IGNORE_ALREADY_IGNORED, param.c_str());
		}
		else if (command.equals_ci("DEL") && !param.empty())
		{
			std::vector<ci::string>::iterator it = std::find(mi->ignores.begin(), mi->ignores.end(), param.ci_str());

			if (it != mi->ignores.end())
			{
				mi->ignores.erase(it);
				u->SendMessage(MemoServ, MEMO_IGNORE_DEL, param.c_str());
			}
			else
				u->SendMessage(MemoServ, MEMO_IGNORE_NOT_IGNORED, param.c_str());
		}
		else if (command.equals_ci("LIST"))
		{
			if (mi->ignores.empty())
				u->SendMessage(MemoServ, MEMO_IGNORE_LIST_EMPTY);
			else
			{
				u->SendMessage(MemoServ, MEMO_IGNORE_LIST_HEADER);
				for (unsigned i = 0; i < mi->ignores.size(); ++i)
					u->SendMessage(Config->s_MemoServ, "    %s", mi->ignores[i].c_str());
			}
		}
		else
			this->OnSyntaxError(u, "");

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(MemoServ, MEMO_HELP_IGNORE);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(MemoServ, u, "IGNORE", MEMO_IGNORE_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(MemoServ, MEMO_HELP_CMD_IGNORE);
	}
};

class MSIgnore : public Module
{
	CommandMSIgnore commandmsignore;

 public:
	MSIgnore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(MemoServ, &commandmsignore);
	}
};

MODULE_INIT(MSIgnore)

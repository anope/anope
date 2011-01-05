/* MemoServ core functions
 *
 * (C) 2003-2011 Anope Team
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

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

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
				source.Reply(ischan ? CHAN_X_FORBIDDEN : NICK_X_FORBIDDEN, channel.c_str());
			else
				source.Reply(ischan ? CHAN_X_NOT_REGISTERED : NICK_X_NOT_REGISTERED, channel.c_str());
		}
		else if (ischan && !check_access(u, cs_findchan(channel), CA_MEMO))
			source.Reply(ACCESS_DENIED);
		else if (command.equals_ci("ADD") && !param.empty())
		{
			if (std::find(mi->ignores.begin(), mi->ignores.end(), param.ci_str()) == mi->ignores.end())
			{
				mi->ignores.push_back(param.ci_str());
				source.Reply(MEMO_IGNORE_ADD, param.c_str());
			}
			else
				source.Reply(MEMO_IGNORE_ALREADY_IGNORED, param.c_str());
		}
		else if (command.equals_ci("DEL") && !param.empty())
		{
			std::vector<ci::string>::iterator it = std::find(mi->ignores.begin(), mi->ignores.end(), param.ci_str());

			if (it != mi->ignores.end())
			{
				mi->ignores.erase(it);
				source.Reply(MEMO_IGNORE_DEL, param.c_str());
			}
			else
				source.Reply(MEMO_IGNORE_NOT_IGNORED, param.c_str());
		}
		else if (command.equals_ci("LIST"))
		{
			if (mi->ignores.empty())
				source.Reply(MEMO_IGNORE_LIST_EMPTY);
			else
			{
				source.Reply(MEMO_IGNORE_LIST_HEADER);
				for (unsigned i = 0; i < mi->ignores.size(); ++i)
					source.Reply("    %s", mi->ignores[i].c_str());
			}
		}
		else
			this->OnSyntaxError(source, "");

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(MEMO_HELP_IGNORE);
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "IGNORE", MEMO_IGNORE_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(MEMO_HELP_CMD_IGNORE);
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

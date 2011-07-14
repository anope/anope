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
#include "memoserv.h"

class CommandMSIgnore : public Command
{
 public:
	CommandMSIgnore(Module *creator) : Command(creator, "memoserv/ignore", 1, 3)
	{
		this->SetDesc(_("Manage your memo ignore list"));
		this->SetSyntax(_("[\037channel\037] {\002ADD|DEL|LIST\002} [\037entry\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
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

		bool ischan;
		MemoInfo *mi = memoserv->GetMemoInfo(channel, ischan);
		if (!mi)
			source.Reply(ischan ? CHAN_X_NOT_REGISTERED : _(NICK_X_NOT_REGISTERED), channel.c_str());
		else if (ischan && !check_access(u, cs_findchan(channel), CA_MEMO))
			source.Reply(ACCESS_DENIED);
		else if (command.equals_ci("ADD") && !param.empty())
		{
			if (std::find(mi->ignores.begin(), mi->ignores.end(), param.ci_str()) == mi->ignores.end())
			{
				mi->ignores.push_back(param.ci_str());
				source.Reply(_("\002%s\002 added to your ignore list."), param.c_str());
			}
			else
				source.Reply(_("\002%s\002 is already on your ignore list."), param.c_str());
		}
		else if (command.equals_ci("DEL") && !param.empty())
		{
			std::vector<Anope::string>::iterator it = std::find(mi->ignores.begin(), mi->ignores.end(), param.ci_str());

			if (it != mi->ignores.end())
			{
				mi->ignores.erase(it);
				source.Reply(_("\002%s\002 removed from your ignore list."), param.c_str());
			}
			else
				source.Reply(_("\002%s\002 is not on your ignore list."), param.c_str());
		}
		else if (command.equals_ci("LIST"))
		{
			if (mi->ignores.empty())
				source.Reply(_("Your memo ignore list is empty."));
			else
			{
				source.Reply(_("Ignore list:"));
				for (unsigned i = 0; i < mi->ignores.size(); ++i)
					source.Reply("    %s", mi->ignores[i].c_str());
			}
		}
		else
			this->OnSyntaxError(source, "");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to ignore users by nick or host from memoing you. If someone on your\n"
				"memo ignore list tries to memo you, they will not be told that you have them ignored."));
		return true;
	}
};

class MSIgnore : public Module
{
	CommandMSIgnore commandmsignore;

 public:
	MSIgnore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandmsignore(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandmsignore);
	}
};

MODULE_INIT(MSIgnore)

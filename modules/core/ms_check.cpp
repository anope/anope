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

class CommandMSCheck : public Command
{
 public:
	CommandMSCheck(Module *creator) : Command(creator, "memoserv/check", 1, 1)
	{
		this->SetDesc(_("Checks if last memo to a nick was read"));
		this->SetSyntax(_("\037nick\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;

		const Anope::string &recipient = params[0];

		bool found = false;

		NickAlias *na = findnick(recipient);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, recipient.c_str());
			return;
		}

		MemoInfo *mi = &na->nc->memos;

		/* Okay, I know this looks strange but we wanna get the LAST memo, so we
			have to loop backwards */

		for (int i = mi->memos.size() - 1; i >= 0; --i)
		{
			if (u->Account()->display.equals_ci(mi->memos[i]->sender))
			{
				found = true; /* Yes, we've found the memo */

				if (mi->memos[i]->HasFlag(MF_UNREAD))
					source.Reply(_("The last memo you sent to %s (sent on %s) has not yet been read."), na->nick.c_str(), do_strftime(mi->memos[i]->time).c_str());
				else
					source.Reply(_("The last memo you sent to %s (sent on %s) has been read."), na->nick.c_str(), do_strftime(mi->memos[i]->time).c_str());
				break;
			}
		}

		if (!found)
			source.Reply(_("Nick %s doesn't have a memo from you."), na->nick.c_str());

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Checks whether the _last_ memo you sent to \037nick\037 has been read\n"
				"or not. Note that this does only work with nicks, not with chans."));
		return true;
	}
};

class MSCheck : public Module
{
	CommandMSCheck commandmscheck;

 public:
	MSCheck(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandmscheck(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&commandmscheck);
	}
};

MODULE_INIT(MSCheck)

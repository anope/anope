/* MemoServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandMSCheck : public Command
{
 public:
	CommandMSCheck(Module *creator) : Command(creator, "memoserv/check", 1, 1)
	{
		this->SetDesc(_("Checks if last memo to a user was read"));
		this->SetSyntax(_("\037user\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &recipient = params[0];

		bool found = false;

		const NickServ::Nick *na = NickServ::FindNick(recipient);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), recipient);
			return;
		}

		MemoServ::MemoInfo *mi = na->nc->memos;

		if (mi)
			/* find the last memo */
			for (unsigned i = mi->memos->size(); i > 0; --i)
			{
				MemoServ::Memo *m = mi->GetMemo(i - 1);
				NickServ::Nick *na2 = NickServ::FindNick(m->sender);

				if (na2 != NULL && na2->nc == source.GetAccount())
				{
					found = true; /* Yes, we've found the memo */

					if (m->unread)
						source.Reply(_("The last memo you sent to \002{0}\002 (sent on \002{1}\002) has not yet been read."), na->nick, Anope::strftime(m->time, source.GetAccount()));
					else
						source.Reply(_("The last memo you sent to \002{0}\002 (sent on \002{1}\002) has been read."), na->nick, Anope::strftime(m->time, source.GetAccount()));
					break;
				}
			}

		if (!found)
			source.Reply(_("\002{0}\002 doesn't have a memo from you."), na->nick);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Checks whether the last memo you sent to \037user\037 has been read or not."));
		return true;
	}
};

class MSCheck : public Module
{
	CommandMSCheck commandmscheck;

 public:
	MSCheck(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandmscheck(this)
	{

	}
};

MODULE_INIT(MSCheck)

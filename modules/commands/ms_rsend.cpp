/* MemoServ core functions
 *
 * (C) 2003-2013 Anope Team
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

static ServiceReference<MemoServService> MemoServService("MemoServService", "MemoServ");

class CommandMSRSend : public Command
{
 public:
	CommandMSRSend(Module *creator) : Command(creator, "memoserv/rsend", 2, 2)
	{
		this->SetDesc(_("Sends a memo and requests a read receipt"));
		this->SetSyntax(_("{\037nick\037 | \037channel\037} \037memo-text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (!MemoServService)
			return;


		const Anope::string &nick = params[0];
		const Anope::string &text = params[1];
		const NickAlias *na = NULL;

		/* prevent user from rsend to themselves */
		if ((na = NickAlias::Find(nick)) && na->nc == source.GetAccount())
		{
			source.Reply(_("You can not request a receipt when sending a memo to yourself."));
			return;
		}

		if (Config->MSMemoReceipt == 1 && !source.IsServicesOper())
			source.Reply(ACCESS_DENIED);
		else if (Config->MSMemoReceipt > 2 || Config->MSMemoReceipt == 0)
		{
			Log(this->owner) << "MSMemoReceipt is misconfigured to " << Config->MSMemoReceipt;
			source.Reply(_("Sorry, RSEND has been disabled on this network."));
		}
		else
		{
			MemoServService::MemoResult result = MemoServService->Send(source.GetNick(), nick, text);
			if (result == MemoServService::MEMO_INVALID_TARGET)
				source.Reply(_("\002%s\002 is not a registered unforbidden nick or channel."), nick.c_str());
			else if (result == MemoServService::MEMO_TOO_FAST)
				source.Reply(_("Please wait %d seconds before using the SEND command again."), Config->MSSendDelay);
			else if (result == MemoServService::MEMO_TARGET_FULL)
				source.Reply(_("Sorry, %s currently has too many memos and cannot receive more."), nick.c_str());
			else	
			{
				source.Reply(_("Memo sent to \002%s\002."), name.c_str());

				bool ischan;
				MemoInfo *mi = MemoInfo::GetMemoInfo(nick, ischan);
				if (mi == NULL)
					throw CoreException("NULL mi in ms_rsend");
				Memo *m = (mi->memos->size() ? mi->GetMemo(mi->memos->size() - 1) : NULL);
				if (m != NULL)
					m->receipt = true;
			}
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Sends the named \037nick\037 or \037channel\037 a memo containing\n"
				"\037memo-text\037. When sending to a nickname, the recipient will\n"
				"receive a notice that he/she has a new memo. The target\n"
				"nickname/channel must be registered.\n"
				"Once the memo is read by its recipient, an automatic notification\n"
				"memo will be sent to the sender informing him/her that the memo\n"
				"has been read."));
		return true;
	}
};

class MSRSend : public Module
{
	CommandMSRSend commandmsrsend;

 public:
	MSRSend(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandmsrsend(this)
	{

		if (!MemoServService)
			throw ModuleException("No MemoServ!");
		else if (!Config->MSMemoReceipt)
			throw ModuleException("Invalid value for memoreceipt");
	}
};

MODULE_INIT(MSRSend)

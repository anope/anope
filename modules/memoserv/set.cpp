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

class CommandMSSet : public Command
{
 private:
	void DoNotify(CommandSource &source, const std::vector<Anope::string> &params, MemoServ::MemoInfo *mi)
	{
		const Anope::string &param = params[1];
		NickServ::Account *nc = source.nc;
		ServiceBot *MemoServ = Config->GetClient("MemoServ");

		if (!MemoServ)
			return;

		if (param.equals_ci("ON"))
		{
			nc->SetS<bool>("MEMO_SIGNON", true);
			nc->SetS<bool>("MEMO_RECEIVE", true);
			source.Reply(_("\002{0}\002 will now notify you of memos when you log on and when they are sent to you."), MemoServ->nick);
		}
		else if (param.equals_ci("LOGON"))
		{
			nc->SetS<bool>("MEMO_SIGNON", true);
			nc->UnsetS<bool>("MEMO_RECEIVE");
			source.Reply(_("\002{0}\002 will now notify you of memos when you log on or unset /AWAY."), MemoServ->nick);
		}
		else if (param.equals_ci("NEW"))
		{
			nc->UnsetS<bool>("MEMO_SIGNON");
			nc->SetS<bool>("MEMO_RECEIVE", true);
			source.Reply(_("\002{0}\002 will now notify you of memos when they are sent to you."), MemoServ->nick);
		}
		else if (param.equals_ci("MAIL"))
		{
			if (!nc->GetEmail().empty())
			{
				nc->SetS<bool>("MEMO_MAIL", true);
				source.Reply(_("You will now be informed about new memos via email."));
			}
			else
				source.Reply(_("There's no email address set for your nick."));
		}
		else if (param.equals_ci("NOMAIL"))
		{
			nc->UnsetS<bool>("MEMO_MAIL");
			source.Reply(_("You will no longer be informed via email."));
		}
		else if (param.equals_ci("OFF"))
		{
			nc->UnsetS<bool>("MEMO_SIGNON");
			nc->UnsetS<bool>("MEMO_RECEIVE");
			nc->UnsetS<bool>("MEMO_MAIL");
			source.Reply(_("\002{0}\002 will not send you any notification of memos."), MemoServ->nick);
		}
		else
			this->OnSyntaxError(source, "");
	}

	void DoLimit(CommandSource &source, const std::vector<Anope::string> &params, MemoServ::MemoInfo *mi)
	{

		Anope::string p1 = params[1];
		Anope::string p2 = params.size() > 2 ? params[2] : "";
		Anope::string p3 = params.size() > 3 ? params[3] : "";
		Anope::string user, chan;
		int16_t limit;
		NickServ::Account *nc = source.nc;
		ChanServ::Channel *ci = NULL;
		bool is_servadmin = source.HasPriv("memoserv/set-limit");

		if (p1[0] == '#')
		{
			chan = p1;
			p1 = p2;
			p2 = p3;
			p3 = params.size() > 4 ? params[4] : "";

			ci = ChanServ::Find(chan);
			if (!ci)
			{
				source.Reply(_("Channel \002{0}\002 isn't registered."), chan);
				return;
			}

			if (!is_servadmin && !source.AccessFor(ci).HasPriv("MEMO"))
			{
				source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "MEMO", ci->GetName());
				return;
			}
			mi = ci->GetMemos();
		}
		if (is_servadmin)
		{
			if (!p2.empty() && !p2.equals_ci("HARD") && chan.empty())
			{
				NickServ::Nick *na;
				if (!(na = NickServ::FindNick(p1)))
				{
					source.Reply(_("\002{0}\002 isn't registered."), p1);
					return;
				}
				user = p1;
				mi = na->GetAccount()->GetMemos();
				nc = na->GetAccount();
				p1 = p2;
				p2 = p3;
			}
			else if (p1.empty() || (!p1.is_pos_number_only() && !p1.equals_ci("NONE")) || (!p2.empty() && !p2.equals_ci("HARD")))
			{
				this->OnSyntaxError(source, "");
				return;
			}
			if (!chan.empty())
			{
				if (!p2.empty())
					ci->SetS<bool>("MEMO_HARDMAX", true);
				else
					ci->UnsetS<bool>("MEMO_HARDMAX");
			}
			else
			{
				if (!p2.empty())
					nc->SetS<bool>("MEMO_HARDMAX", true);
				else
					nc->UnsetS<bool>("MEMO_HARDMAX");
			}
			limit = -1;
			try
			{
				limit = convertTo<int16_t>(p1);
			}
			catch (const ConvertException &) { }
		}
		else
		{
			if (p1.empty() || !p2.empty() || !isdigit(p1[0]))
			{
				this->OnSyntaxError(source, "");
				return;
			}
			if (!chan.empty() && ci->HasFieldS("MEMO_HARDMAX"))
			{
				source.Reply(_("The memo limit for \002{0}\002 may not be changed."), chan);
				return;
			}
			if (chan.empty() && nc->HasFieldS("MEMO_HARDMAX"))
			{
				source.Reply(_("You are not permitted to change your memo limit."));
				return;
			}
			int max_memos = Config->GetModule("memoserv")->Get<int>("maxmemos");
			limit = -1;
			try
			{
				limit = convertTo<int16_t>(p1);
			}
			catch (const ConvertException &) { }
			/* The first character is a digit, but we could still go negative
			 * from overflow... watch out! */
			if (limit < 0 || (max_memos > 0 && limit > max_memos))
			{
				if (!chan.empty())
					source.Reply(_("You cannot set the memo limit for \002{0}\002 higher than \002{1}\002."), chan, max_memos);
				else
					source.Reply(_("You cannot set your memo limit higher than \002{0}\002."), max_memos);
				return;
			}
		}
		mi->SetMemoMax(limit);
		if (limit > 0)
		{
			if (chan.empty() && nc == source.nc)
				source.Reply(_("Your memo limit has been set to \002{0}\002."), limit);
			else
				source.Reply(_("Memo limit for \002{0}\002 set to \002{1}\002."), !chan.empty() ? chan : user, limit);
		}
		else if (!limit)
		{
			if (chan.empty() && nc == source.nc)
				source.Reply(_("You will no longer be able to receive memos."));
			else
				source.Reply(_("Memo limit for \002{0}\002 set to \0020\002."), !chan.empty() ? chan : user);
		}
		else
		{
			if (chan.empty() && nc == source.nc)
				source.Reply(_("Your memo limit has been disabled."));
			else
				source.Reply(_("Memo limit \002disabled\002 for \002{0}\002."), !chan.empty() ? chan : user);
		}
	}
 public:
	CommandMSSet(Module *creator) : Command(creator, "memoserv/set", 2, 5)
	{
		this->SetDesc(_("Set options related to memos"));
		this->SetSyntax(_("\037option\037 \037parameters\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &cmd = params[0];
		MemoServ::MemoInfo *mi = source.nc->GetMemos();

		if (Anope::ReadOnly)
			source.Reply(_("Sorry, memo option setting is temporarily disabled."));
		else if (cmd.equals_ci("NOTIFY"))
			return this->DoNotify(source, params, mi);
		else if (cmd.equals_ci("LIMIT"))
			return this->DoLimit(source, params, mi);
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		if (subcommand.empty())
		{
			CommandInfo *help = source.service->FindCommand("generic/help");
			if (!help)
				return false;
			source.Reply(_("Sets various memo options. \037option\037 can be one of:\n"
			               "\n"
			               "    NOTIFY      Changes when you will be notified about\n"
			               "                new memos (only for nicknames)\n"
			               "    LIMIT       Sets the maximum number of memos you can\n"
			               "                receive\n"
			               "\n"
			               "Type \002{0}{1} {2} {3} \037option\037\002 for more information on a specific option."),
			               Config->StrictPrivmsg, source.service->nick, help->cname, source.command);
		}
		else if (subcommand.equals_ci("NOTIFY"))
			source.Reply(_("Syntax: \002NOTIFY {ON | LOGON | NEW | MAIL | NOMAIL | OFF}\002\n"
			               "\n"
			               "Changes when you will be notified about new memos:\n"
			               "\n"
			               "    ON      You will be notified of memos when you log on,\n"
			               "            when you unset /AWAY, and when they are sent\n"
			               "            to you.\n"
			               "\n"
			               "    LOGON   You will only be notified of memos when you log\n"
			               "            on or when you unset /AWAY.\n"
			               "\n"
			               "    NEW     You will only be notified of memos when they\n"
			               "            are sent to you.\n"
			               "\n"
			               "    MAIL    You will be notified of memos by email as well as\n"
			               "            any other settings you have.\n"
			               "\n"
			               "    NOMAIL  You will not be notified of memos by email.\n"
			               "\n"
			               "    OFF     You will not receive any notification of memos.\n"
			               "\n"
			               "\002ON\002 is essentially \002LOGON\002 and \002NEW\002 combined."));
		else if (subcommand.equals_ci("LIMIT"))
		{
			int max_memos = Config->GetModule("memoserv")->Get<int>("maxmemos");
			if (source.IsServicesOper())
				source.Reply(_("Syntax: \002LIMIT [\037user\037 | \037channel\037] {\037limit\037 | NONE} [HARD]\002\n"
				               "\n"
				               "Sets the maximum number of memos a user or channel is allowed to have."
				               "  Setting the limit to 0 prevents the user from receiving any memos; setting it to \002NONE\002 allows the user to receive and keep as many memos as they want."
				               "  If you do not give a nickname or channel, your own limit is set.\n"
				               "\n"
				               "Adding \002HARD\002 prevents the user from changing the limit."
				               " Not adding \002HARD\002 has the opposite effect, allowing the user to change the limit, even if a previous limit was set.\n"
				               " \n"
				               "This use of the \002{0} LIMIT\002 command is limited to \002Services Operators\002."
				               " Other users may only enter a limit for themselves or a channel on which they have the \002MEMO\002 privilege on, may not remove their limit, may not set a limit above {1}, and may not set a hard limit."),
				               source.command, max_memos);
			else
				source.Reply(_("Syntax: \002LIMIT [\037channel\037] \037limit\037\002\n"
				               "\n"
				              "Sets the maximum number of memos you, or the given channel, are allowed to have."
				              " If you set this to 0, no one will be able to send any memos to you."
				              "However, you cannot set this any higher than {0}."), max_memos);
		}
		else
			return false;

		return true;
	}
};

class MSSet : public Module
{
	CommandMSSet commandmsset;
	Serialize::Field<NickServ::Account, bool> memo_signon, memo_receive, memo_mail, memo_hardmax_nick;
	Serialize::Field<ChanServ::Channel, bool> memo_hardmax_channel;

 public:
	MSSet(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandmsset(this)
		, memo_signon(this, "MEMO_SIGNON")
		, memo_receive(this, "MEMO_RECEIVE")
		, memo_mail(this, "MEMO_MAIL")
		, memo_hardmax_nick(this, "MEMO_HARDMAX")
		, memo_hardmax_channel(this, "MEMO_HARDMAX")
	{

	}
};

MODULE_INIT(MSSet)

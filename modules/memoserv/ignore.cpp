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

class CommandMSIgnore : public Command
{
 public:
	CommandMSIgnore(Module *creator) : Command(creator, "memoserv/ignore", 1, 3)
	{
		this->SetDesc(_("Manage the memo ignore list"));
		this->SetSyntax(_("[\037channel\037] ADD \037entry\037"));
		this->SetSyntax(_("[\037channel\037] DEL \037entry\037"));
		this->SetSyntax(_("[\037channel\037] LIST"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		if (!MemoServ::service)
			return;

		Anope::string channel = params[0];
		Anope::string command = (params.size() > 1 ? params[1] : "");
		Anope::string param = (params.size() > 2 ? params[2] : "");

		if (channel[0] != '#')
		{
			param = command;
			command = channel;
			channel = source.GetNick();
		}

		bool ischan, isregistered;
		MemoServ::MemoInfo *mi = MemoServ::service->GetMemoInfo(channel, ischan, isregistered, true);
		ChanServ::Channel *ci = ChanServ::Find(channel);
		if (!isregistered)
		{
			if (ischan)
				source.Reply(_("Channel \002{0}\002 isn't registered."), channel);
			else
				source.Reply(_("\002{0}\002 isn't registered."), channel);
			return;
		}

		if (ischan && !source.AccessFor(ci).HasPriv("MEMO"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "MEMO", ci->GetName());
			return;
		}

		auto ignores = mi->GetIgnores();

		if (command.equals_ci("ADD") && !param.empty())
		{
			if (ignores.size() >= Config->GetModule(this->owner)->Get<unsigned>("max", "32"))
			{
				source.Reply(_("Sorry, the memo ignore list for \002{0}\002 is full."), channel);
				return;
			}

			for (MemoServ::Ignore *ign : ignores)
				if (ign->GetMask().equals_ci(param))
				{
					source.Reply(_("\002{0}\002 is already on your memo ignore list."), param);
					return;
				}

			MemoServ::Ignore *ign = MemoServ::service->CreateIgnore();
			ign->SetMemoInfo(mi);
			ign->SetMask(param);

			source.Reply(_("\002{0}\002 has been added to your memo ignore list."), param);
		}
		else if (command.equals_ci("DEL") && !param.empty())
		{
			for (MemoServ::Ignore *ign : ignores)
				if (ign->GetMask().equals_ci(param))
				{
					delete ign;
					source.Reply(_("\002{0}\002 has been removed from your memo ignore list."), param);
					return;
				}

			source.Reply(_("\002{0}\002 is not on your memo ignore list."), param);
		}
		else if (command.equals_ci("LIST"))
		{
			if (ignores.empty())
			{
				source.Reply(_("Your memo ignore list is empty."));
				return;
			}
			ListFormatter list(source.GetAccount());
			list.AddColumn(_("Mask"));
			for (MemoServ::Ignore *ign : ignores)
			{
				ListFormatter::ListEntry entry;
				entry["Mask"] = ign->GetMask();
				list.AddEntry(entry);
			}

			source.Reply(_("Memo ignore list:"));

			std::vector<Anope::string> replies;
			list.Process(replies);

			for (unsigned i = 0; i < replies.size(); ++i)
				source.Reply(replies[i]);
		}
		else
			this->OnSyntaxError(source, "");
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Allows you to ignore users from memoing you or a channel."
		               " If someone on the memo ignore list tries to memo you or a channel, they will not be told that you have them ignored."));
		return true;
	}
};

class MSIgnore : public Module
{
	CommandMSIgnore commandmsignore;

 public:
	MSIgnore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandmsignore(this)
	{
	}
};

MODULE_INIT(MSIgnore)

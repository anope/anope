/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2017 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"
#include "modules/nickserv/info.h"
#include "modules/nickserv/set.h"

class CommandNSInfo : public Command
{
 public:
	CommandNSInfo(Module *creator) : Command(creator, "nickserv/info", 0, 2)
	{
		this->SetDesc(_("Displays information about a given nickname"));
		this->SetSyntax(_("[\037nickname\037]"));
		this->AllowUnregistered(true);
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{

		const Anope::string &nick = params.size() ? params[0] : (source.nc ? source.nc->GetDisplay() : source.GetNick());
		NickServ::Nick *na = NickServ::FindNick(nick);
		bool has_auspex = source.HasPriv("nickserv/auspex");

		if (!na)
		{
			if (ServiceBot::Find(nick, true))
				source.Reply(_("\002{0}\002 is part of this Network's Services."), nick);
			else
				source.Reply(_("\002{0}\002 isn't registered."), nick);
			return;
		}

		bool nick_online = false, show_hidden = false;

		/* Is the real owner of the nick we're looking up online? -TheShadow */
		User *u2 = User::Find(na->GetNick(), true);
		if (u2 && u2->Account() == na->GetAccount())
		{
			nick_online = true;
			na->SetLastSeen(Anope::CurTime);
		}

		if (has_auspex || na->GetAccount() == source.GetAccount())
			show_hidden = true;

		source.Reply(_("\002{0}\002 is \002{1}\002"), na->GetNick(), na->GetLastRealname());

		if (na->GetAccount()->IsUnconfirmed())
			source.Reply(_("\002{0}\002 has not confirmed their account."), na->GetNick());

		if (na->GetAccount()->GetOper() && (show_hidden || !na->GetAccount()->IsHideStatus()))
			source.Reply(_("\002{0}\002 is a Services Operator of type \002{1}\002."), na->GetNick(), na->GetAccount()->GetOper()->GetType()->GetName());

		InfoFormatter info(source.nc);

		if (nick_online)
		{
			bool shown = false;
			if (show_hidden && !na->GetLastRealhost().empty())
			{
				info[_("Online from")] = na->GetLastRealhost();
				shown = true;
			}
			if ((show_hidden || !na->GetAccount()->IsHideMask()) && (!shown || na->GetLastUsermask() != na->GetLastRealhost()))
				info[_("Online from")] = na->GetLastUsermask();
			else
				source.Reply(_("\002{0}\002 is currently online."), na->GetNick());
		}
		else
		{
			Anope::string shown;
			if (show_hidden || !na->GetAccount()->IsHideMask())
			{
				info[_("Last seen address")] = na->GetLastUsermask();
				shown = na->GetLastUsermask();
			}

			if (show_hidden && !na->GetLastRealhost().empty() && na->GetLastRealhost() != shown)
				info[_("Last seen address")] = na->GetLastRealhost();
		}

		info[_("Registered")] = Anope::strftime(na->GetTimeRegistered(), source.GetAccount());

		if (!nick_online)
			info[_("Last seen")] = Anope::strftime(na->GetLastSeen(), source.GetAccount());

		if (!na->GetLastQuit().empty() && (show_hidden || !na->GetAccount()->IsHideQuit()))
			info[_("Last quit message")] = na->GetLastQuit();

		if (!na->GetAccount()->GetEmail().empty() && (show_hidden || !na->GetAccount()->IsHideEmail()))
			info[_("Email address")] = na->GetAccount()->GetEmail();

		EventManager::Get()->Dispatch(&Event::NickInfo::OnNickInfo, source, na, info, show_hidden);

		std::vector<Anope::string> replies;
		info.Process(replies);

		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Displays information about the given nickname, such as "
				"the nick's owner, last seen address and time, and nick "
				"options. If no nick is given, and you are identified, "
				"your account name is used, else your current nickname is "
				"used."));

		return true;
	}
};


class CommandNSSetHide : public Command
{
 public:
	CommandNSSetHide(Module *creator, const Anope::string &sname = "nickserv/set/hide", size_t min = 2) : Command(creator, sname, min, min + 1)
	{
		this->SetDesc(_("Hide certain pieces of nickname information"));
		this->SetSyntax("{EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}");
	}

	void Run(CommandSource &source, const Anope::string &user, const Anope::string &param, const Anope::string &arg)
	{
		if (Anope::ReadOnly)
		{
			source.Reply(_("Services are in read-only mode."));
			return;
		}

		NickServ::Nick *na = NickServ::FindNick(user);
		if (!na)
		{
			source.Reply(_("\002{0}\002 isn't registered."), user);
			return;
		}
		NickServ::Account *nc = na->GetAccount();

		EventReturn MOD_RESULT = EventManager::Get()->Dispatch(&Event::SetNickOption::OnSetNickOption, source, this, nc, param);
		if (MOD_RESULT == EVENT_STOP)
			return;

		const char *onmsg, *offmsg, *flag;

		if (param.equals_ci("EMAIL"))
		{
			flag = "hide_email";
			onmsg = _("The \002e-mail address\002 of \002{0}\002 will now be \002hidden\002.");
			offmsg = _("The \002e-mail address\002 of \002{0}\002 will now be \002shown\002.");
		}
		else if (param.equals_ci("USERMASK"))
		{
			flag = "hide_mask";
			onmsg = _("The \002last seen host mask\002 of \002{0}\002 will now be \002hidden\002.");
			offmsg = _("The \002last seen host mask\002 of \002{0}\002 will now be \002shown\002.");
		}
		else if (param.equals_ci("STATUS"))
		{
			flag = "hide_status";
			onmsg = _("The \002services operator status\002 of \002{0}\002 will now be \002hidden\002.");
			offmsg = _("The \002services operator status\002 of \002{0}\002 will now be \002shown\002.");
		}
		else if (param.equals_ci("QUIT"))
		{
			flag = "hide_quit";
			onmsg = _("The \002last quit message\002 of \002{0}\002 will now be \002hidden\002.");
			offmsg = _("The \002last quit message\002 of \002{0}\002 will now be \002shown\002.");
		}
		else
		{
			this->OnSyntaxError(source, "HIDE");
			return;
		}

		if (arg.equals_ci("ON"))
		{
			logger.Command(nc == source.GetAccount() ? LogType::COMMAND : LogType::ADMIN, source,
					_("{source} used {command} to change hide {0} to {1} for {2}"),
					param.upper(), arg.upper(), nc->GetDisplay());

			nc->SetS<bool>(flag, true);
			source.Reply(onmsg, nc->GetDisplay(), source.service->nick);
		}
		else if (arg.equals_ci("OFF"))
		{
			logger.Command(nc == source.GetAccount() ? LogType::COMMAND : LogType::ADMIN, source,
					_("{source} used {command} to change hide {0} to {1} for {2}"),
					param.upper(), arg.upper(), nc->GetDisplay());

			nc->UnsetS<bool>(flag);
			source.Reply(offmsg, nc->GetDisplay(), source.service->nick);
		}
		else
			this->OnSyntaxError(source, "HIDE");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->GetDisplay(), params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Allows you to prevent certain pieces of information from being displayed when someone does a %s \002INFO\002 on you." //XXX
		               " You can hide the e-mail address (\002EMAIL\002), last seen hostmask (\002USERMASK\002), the services access status (\002STATUS\002) and last quit message (\002QUIT\002)."
				"The second parameter specifies whether the information should\n"
				"be displayed (\002OFF\002) or hidden (\002ON\002)."), source.service->nick);
		return true;
	}
};

class CommandNSSASetHide : public CommandNSSetHide
{
 public:
	CommandNSSASetHide(Module *creator) : CommandNSSetHide(creator, "nickserv/saset/hide", 3)
	{
		this->SetSyntax(_("\037nickname\037 {EMAIL | STATUS | USERMASK | QUIT} {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->ClearSyntax();
		this->Run(source, params[0], params[1], params[2]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		source.Reply(_("Allows you to prevent certain pieces of information from being displayed when someone does a %s \002INFO\002 on the \037nickname\037. "
		               " You can hide the e-mail address (\002EMAIL\002), last seen hostmask (\002USERMASK\002), the services access status (\002STATUS\002) and last quit message (\002QUIT\002)."
		               " The second parameter specifies whether the information should be displayed (\002OFF\002) or hidden (\002ON\002)."),
		               source.service->nick);
		return true;
	}
};

class NSInfo : public Module
{
	CommandNSInfo commandnsinfo;

	CommandNSSetHide commandnssethide;
	CommandNSSASetHide commandnssasethide;

 public:
	NSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandnsinfo(this)
		, commandnssethide(this)
		, commandnssasethide(this)
	{

	}
};

MODULE_INIT(NSInfo)

/* NickServ core functions
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandNSInfo final
	: public Command
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
		Anope::string nick;
		if (params.empty())
		{
			// Use the display of the source if logged in; otherwise, use their nick.
			nick = source.nc ? source.nc->na->nick : source.GetNick();
		}
		else if (params[0][0] == '=' && params[0].length() > 1)
		{
			// Look up the account of the specified user.
			nick = params[0].substr(1);
			auto *u = User::Find(nick);
			if (!u)
			{
				source.Reply(NICK_X_NOT_IN_USE, nick.c_str());
				return;
			}
			if (!u->AccountNick())
			{
				source.Reply(_("User \002%s\002 isn't currently logged in to an account."), nick.c_str());
				return;
			}
			nick = u->AccountNick()->nick;
		}
		else
		{
			// Just use the nick the user specified.
			nick = params[0];
		}

		NickAlias *na = NickAlias::Find(nick);
		bool has_auspex = source.HasPriv("nickserv/auspex");

		if (!na)
		{
			if (BotInfo::Find(nick, true))
				source.Reply(_("Nick \002%s\002 is part of this Network's Services."), nick.c_str());
			else
				source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
		}
		else
		{
			bool nick_online = false, show_hidden = false;

			/* Is the real owner of the nick we're looking up online? -TheShadow */
			User *u2 = User::Find(na->nick, true);
			if (u2 && u2->Account() == na->nc)
			{
				nick_online = true;
				na->last_seen = Anope::CurTime;
			}

			if (has_auspex || na->nc == source.GetAccount())
				show_hidden = true;

			source.Reply(_("%s is %s"), na->nick.c_str(), na->last_realname.c_str());

			if (na->nc->HasExt("UNCONFIRMED"))
				source.Reply(_("%s is an unconfirmed nickname."), na->nick.c_str());

			if (na->nc->IsServicesOper() && (show_hidden || !na->nc->HasExt("HIDE_STATUS")))
				source.Reply(_("%s is a Services Operator of type %s."), na->nick.c_str(), na->nc->o->ot->GetName().c_str());

			InfoFormatter info(source.nc);

			info[_("Account")] = na->nc->display;
			info[_("Account id")] = Anope::ToString(na->nc->GetId());
			if (nick_online)
			{
				bool shown = false;
				if (show_hidden && !na->last_realhost.empty())
				{
					info[_("Online from")] = na->last_realhost;
					shown = true;
				}
				if ((show_hidden || !na->nc->HasExt("HIDE_MASK")) && (!shown || na->last_usermask != na->last_realhost))
					info[_("Online from")] = na->last_usermask;
				else
					source.Reply(_("%s is currently online."), na->nick.c_str());
			}
			else
			{
				Anope::string shown;
				if (show_hidden || !na->nc->HasExt("HIDE_MASK"))
				{
					info[_("Last seen address")] = na->last_usermask;
					shown = na->last_usermask;
				}

				if (show_hidden && !na->last_realhost.empty() && na->last_realhost != shown)
					info[_("Last seen address")] = na->last_realhost;
			}

			info[_("Account registered")] = Anope::strftime(na->nc->time_registered, source.GetAccount());
			info[_("Nick registered")] = Anope::strftime(na->time_registered, source.GetAccount());

			if (!nick_online)
				info[_("Last seen")] = Anope::strftime(na->last_seen, source.GetAccount());

			if (!na->last_quit.empty() && (show_hidden || !na->nc->HasExt("HIDE_QUIT")))
				info[_("Last quit message")] = na->last_quit;

			if (!na->nc->email.empty() && (show_hidden || !na->nc->HasExt("HIDE_EMAIL")))
				info[_("Email address")] = na->nc->email;

			if (show_hidden)
			{
				if (na->HasVHost())
					info[_("VHost")] = na->GetVHostMask();
			}

			FOREACH_MOD(OnNickInfo, (source, na, info, show_hidden));

			std::vector<Anope::string> replies;
			info.Process(replies);

			for (const auto &reply : replies)
				source.Reply(reply);
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Displays information about the given nickname, such as\n"
				"the nick's owner, last seen address and time, and nick\n"
				"options. If no nick is given, and you are identified,\n"
				"your account name is used, else your current nickname is\n"
				"used."));

		return true;
	}
};


class CommandNSSetHide
	: public Command
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
			source.Reply(READ_ONLY_MODE);
			return;
		}

		const NickAlias *na = NickAlias::Find(user);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, user.c_str());
			return;
		}
		NickCore *nc = na->nc;

		EventReturn MOD_RESULT;
		FOREACH_RESULT(OnSetNickOption, MOD_RESULT, (source, this, nc, param));
		if (MOD_RESULT == EVENT_STOP)
			return;

		Anope::string onmsg, offmsg, flag;

		if (param.equals_ci("EMAIL"))
		{
			flag = "HIDE_EMAIL";
			onmsg = _("The email address of \002%s\002 will now be hidden from %s INFO displays.");
			offmsg = _("The email address of \002%s\002 will now be shown in %s INFO displays.");
		}
		else if (param.equals_ci("USERMASK"))
		{
			flag = "HIDE_MASK";
			onmsg = _("The last seen user@host mask of \002%s\002 will now be hidden from %s INFO displays.");
			offmsg = _("The last seen user@host mask of \002%s\002 will now be shown in %s INFO displays.");
		}
		else if (param.equals_ci("STATUS"))
		{
			flag = "HIDE_STATUS";
			onmsg = _("The services access status of \002%s\002 will now be hidden from %s INFO displays.");
			offmsg = _("The services access status of \002%s\002 will now be shown in %s INFO displays.");
		}
		else if (param.equals_ci("QUIT"))
		{
			flag = "HIDE_QUIT";
			onmsg = _("The last quit message of \002%s\002 will now be hidden from %s INFO displays.");
			offmsg = _("The last quit message of \002%s\002 will now be shown in %s INFO displays.");
		}
		else
		{
			this->OnSyntaxError(source, "HIDE");
			return;
		}

		if (arg.equals_ci("ON"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change hide " << param.upper() << " to " << arg.upper() << " for " << nc->display;
			nc->Extend<bool>(flag);
			source.Reply(onmsg.c_str(), nc->display.c_str(), source.service->nick.c_str());
		}
		else if (arg.equals_ci("OFF"))
		{
			Log(nc == source.GetAccount() ? LOG_COMMAND : LOG_ADMIN, source, this) << "to change hide " << param.upper() << " to " << arg.upper() << " for " << nc->display;
			nc->Shrink<bool>(flag);
			source.Reply(offmsg.c_str(), nc->display.c_str(), source.service->nick.c_str());
		}
		else
			this->OnSyntaxError(source, "HIDE");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		this->Run(source, source.nc->display, params[0], params[1]);
	}

	bool OnHelp(CommandSource &source, const Anope::string &) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to prevent certain pieces of information from\n"
				"being displayed when someone does a %s \002INFO\002 on your\n"
				"nick.  You can hide your email address (\002EMAIL\002), last seen\n"
				"user@host mask (\002USERMASK\002), your services access status\n"
				"(\002STATUS\002) and last quit message (\002QUIT\002).\n"
				"The second parameter specifies whether the information should\n"
				"be displayed (\002OFF\002) or hidden (\002ON\002)."), source.service->nick.c_str());
		return true;
	}
};

class CommandNSSASetHide final
	: public CommandNSSetHide
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
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Allows you to prevent certain pieces of information from\n"
				"being displayed when someone does a %s \002INFO\002 on the\n"
				"nick.  You can hide the email address (\002EMAIL\002), last seen\n"
				"user@host mask (\002USERMASK\002), the services access status\n"
				"(\002STATUS\002) and last quit message (\002QUIT\002).\n"
				"The second parameter specifies whether the information should\n"
				"be displayed (\002OFF\002) or hidden (\002ON\002)."), source.service->nick.c_str());
		return true;
	}
};

class NSInfo final
	: public Module
{
	CommandNSInfo commandnsinfo;

	CommandNSSetHide commandnssethide;
	CommandNSSASetHide commandnssasethide;

	SerializableExtensibleItem<bool> hide_email, hide_usermask, hide_status, hide_quit;

public:
	NSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnsinfo(this), commandnssethide(this), commandnssasethide(this),
		 hide_email(this, "HIDE_EMAIL"), hide_usermask(this, "HIDE_MASK"), hide_status(this, "HIDE_STATUS"),
		 hide_quit(this, "HIDE_QUIT")
	{

	}
};

MODULE_INIT(NSInfo)

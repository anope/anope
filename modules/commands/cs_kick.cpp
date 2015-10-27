/* ChanServ core functions
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

class CommandCSKick : public Command
{
 public:
	CommandCSKick(Module *creator) : Command(creator, "chanserv/kick", 2, 3)
	{
		this->SetDesc(_("Kicks a specified nick from a channel"));
		this->SetSyntax(_("\037channel\037 \037user\037 [\037reason\037]"));
		this->SetSyntax(_("\037channel\037 \037mask\037 [\037reason\037]"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &chan = params[0];
		const Anope::string &target = params[1];
		Anope::string reason = params.size() > 2 ? params[2] : "Requested";

		User *u = source.GetUser();
		ChanServ::Channel *ci = ChanServ::Find(params[0]);
		Channel *c = Channel::Find(params[0]);
		User *u2 = User::Find(target, true);

		if (!c)
		{
			source.Reply(_("Channel \002{0}\002 doesn't exist."), chan);
			return;
		}

		if (!ci)
		{
			source.Reply(_("Channel \002{0}\002 isn't registered."), c->name);
			return;
		}

		unsigned reasonmax = Config->GetModule("chanserv")->Get<unsigned>("reasonmax", "200");
		if (reason.length() > reasonmax)
			reason = reason.substr(0, reasonmax);

		ChanServ::AccessGroup u_access = source.AccessFor(ci);

		Anope::string signkickformat = Config->GetModule("chanserv")->Get<Anope::string>("signkickformat", "%m (%n)");
		signkickformat = signkickformat.replace_all_cs("%n", source.GetNick());

		if (!u_access.HasPriv("KICK") && !source.HasPriv("chanserv/kick"))
		{
			source.Reply(_("Access denied. You do not have privilege \002{0}\002 on \002{1}\002."), "KICK", ci->GetName());
			return;
		}

		if (u2)
		{
			ChanServ::AccessGroup u2_access = ci->AccessFor(u2);
			if (u != u2 && ci->HasFieldS("PEACE") && u2_access >= u_access && !source.HasPriv("chanserv/kick"))
				source.Reply(_("Access denied. \002{0}\002 has the same or more privileges than you on \002{1}\002."), u2->nick, ci->GetName());
			else if (u2->IsProtected())
				source.Reply(_("Access denied. \002{0}\002 is protected and can not be kicked."), u2->nick);
			else if (!c->FindUser(u2))
				source.Reply(_("User \002{0}\002 is not on channel \002{1}\002."), u2->nick, c->name);
			else
			{
				bool override = !u_access.HasPriv("KICK") || (u != u2 && ci->HasFieldS("PEACE") && u2_access >= u_access);
				Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "for " << u2->nick;

				if (ci->HasFieldS("SIGNKICK") || (ci->HasFieldS("SIGNKICK_LEVEL") && !u_access.HasPriv("SIGNKICK")))
				{
					signkickformat = signkickformat.replace_all_cs("%m", reason);
					c->Kick(ci->WhoSends(), u2, "%s", signkickformat.c_str());
				}
				else
					c->Kick(ci->WhoSends(), u2, "%s", reason.c_str());
			}
		}
		else if (u_access.HasPriv("FOUNDER"))
		{
			Anope::string mask = IRCD->NormalizeMask(target);

			Log(LOG_COMMAND, source, this, ci) << "for " << mask;

			int matched = 0, kicked = 0;
			for (Channel::ChanUserList::iterator it = c->users.begin(), it_end = c->users.end(); it != it_end;)
			{
				ChanUserContainer *uc = it->second;
				++it;

				Entry e("",  mask);
				if (e.Matches(uc->user))
				{
					++matched;

					ChanServ::AccessGroup u2_access = ci->AccessFor(uc->user);
					if (u != uc->user && ci->HasFieldS("PEACE") && u2_access >= u_access)
						continue;
					else if (uc->user->IsProtected())
						continue;

					++kicked;

					if (ci->HasFieldS("SIGNKICK") || (ci->HasFieldS("SIGNKICK_LEVEL") && !u_access.HasPriv("SIGNKICK")))
					{
						reason += " (Matches " + mask + ")";
						signkickformat = signkickformat.replace_all_cs("%m", reason);
						c->Kick(ci->WhoSends(), uc->user, "%s", signkickformat.c_str());
					}
					else
						c->Kick(ci->WhoSends(), uc->user, "%s (Matches %s)", reason.c_str(), mask.c_str());
				}
			}

			if (matched)
				source.Reply(_("Kicked \002{0}/{1}\002 users matching \002{2}\002 from \002{3}\002."), kicked, matched, mask, c->name);
			else
				source.Reply(_("No users on\002{0}\002 match \002{1}\002."), c->name, mask);
		}
		else
			source.Reply(_("\002{0}\002 isn't currently in use."), target);
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		source.Reply(_("Kicks \037user\037 from a \037channel\037. Users with the \002{0}\002 privilege on \037channel\037 may give a \037mask\037, which kicks all users who match the wildcard mask.\n"
		                " \n"
		                "Use of this command requires the \002{1}\002 privilege on \037channel\037."),
		                "FOUNDER", "KICK");
		return true;
	}
};

class CSKick : public Module
{
	CommandCSKick commandcskick;

 public:
	CSKick(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR)
		, commandcskick(this)
	{

	}
};

MODULE_INIT(CSKick)

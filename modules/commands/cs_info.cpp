/* ChanServ core functions
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

class CommandCSInfo : public Command
{
	void CheckOptStr(Anope::string &buf, ChannelInfoFlag opt, const Anope::string &str, ChannelInfo *ci, const NickCore *nc)
	{
		if (ci->HasFlag(opt))
		{
			if (!buf.empty())
				buf += ", ";

			buf += str;
		}
	}

 public:
	CommandCSInfo(Module *creator) : Command(creator, "chanserv/info", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Lists information about the named registered channel"));
		this->SetSyntax(_("\037channel\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];

		User *u = source.u;
		ChannelInfo *ci = cs_findchan(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		bool has_auspex = u->IsIdentified() && u->HasPriv("chanserv/auspex");
		bool show_all = false;

		/* Should we show all fields? Only for sadmins and identified users */
		if (has_auspex || ci->AccessFor(u).HasPriv("INFO"))
			show_all = true;

		source.Reply(CHAN_INFO_HEADER, chan.c_str());
		if (ci->GetFounder())
			source.Reply(_("        Founder: %s"), ci->GetFounder()->display.c_str());

		if (show_all && ci->successor)
			source.Reply(_("      Successor: %s"), ci->successor->display.c_str());

		if (!ci->desc.empty())
			source.Reply(_("    Description: %s"), ci->desc.c_str());
		source.Reply(_("     Registered: %s"), do_strftime(ci->time_registered).c_str());
		source.Reply(_("      Last used: %s"), do_strftime(ci->last_used).c_str());

		ModeLock *secret = ci->GetMLock(CMODE_SECRET);
		if (!ci->last_topic.empty() && (show_all || ((!secret || secret->set == false) && (!ci->c || !ci->c->HasMode(CMODE_SECRET)))))
		{
			source.Reply(_("     Last topic: %s"), ci->last_topic.c_str());
			source.Reply(_("   Topic set by: %s"), ci->last_topic_setter.c_str());
		}

		if (show_all)
		{
			source.Reply(_("       Ban type: %d"), ci->bantype);
			Anope::string optbuf;

			CheckOptStr(optbuf, CI_KEEPTOPIC, _("Topic Retention"), ci, u->Account());
			CheckOptStr(optbuf, CI_PEACE, _("Peace"), ci, u->Account());
			CheckOptStr(optbuf, CI_PRIVATE, _("Private"), ci, u->Account());
			CheckOptStr(optbuf, CI_RESTRICTED, _("Restricted Access"), ci, u->Account());
			CheckOptStr(optbuf, CI_SECURE, _("Secure"), ci, u->Account());
			CheckOptStr(optbuf, CI_SECUREFOUNDER, _("Secure Founder"), ci, u->Account());
			CheckOptStr(optbuf, CI_SECUREOPS, _("Secure Ops"), ci, u->Account());
			if (ci->HasFlag(CI_SIGNKICK))
				CheckOptStr(optbuf, CI_SIGNKICK, _("Signed kicks"), ci, u->Account());
			else
				CheckOptStr(optbuf, CI_SIGNKICK_LEVEL, _("Signed kicks"), ci, u->Account());
			CheckOptStr(optbuf, CI_TOPICLOCK, _("Topic Lock"), ci, u->Account());
			CheckOptStr(optbuf, CI_PERSIST, _("Persistant"), ci, u->Account());
			CheckOptStr(optbuf, CI_NO_EXPIRE, _("No expire"), ci, u->Account());

			source.Reply(_("        Options: %s"), optbuf.empty() ? _("None") : optbuf.c_str());
			source.Reply(_("      Mode lock: %s"), ci->GetMLockAsString(true).c_str());

			if (!ci->HasFlag(CI_NO_EXPIRE))
				source.Reply(_("      Expires on: %s"), do_strftime(ci->last_used + Config->CSExpire).c_str());
		}
		if (ci->HasFlag(CI_SUSPENDED))
		{
			Anope::string by, reason;
			ci->GetExtRegular("suspend_by", by);
			ci->GetExtRegular("suspend_reason", reason);
			source.Reply(_("      Suspended: [%s] %s"), by.c_str(), !reason.empty() ? reason.c_str() : NO_REASON);
		}

		FOREACH_MOD(I_OnChanInfo, OnChanInfo(source, ci, show_all));

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Lists information about the named registered channel,\n"
				"including its founder, time of registration, last time\n"
				"used, description, and mode lock, if any. If \002ALL\002 is \n"
				"specified, the entry message and successor will also \n"
				"be displayed."));
		return true;
	}
};

class CSInfo : public Module
{
	CommandCSInfo commandcsinfo;

 public:
	CSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcsinfo(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSInfo)

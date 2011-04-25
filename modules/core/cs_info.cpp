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
#include "chanserv.h"

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
	CommandCSInfo() : Command("INFO", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetFlag(CFLAG_ALLOW_SUSPENDED);
		this->SetFlag(CFLAG_ALLOW_FORBIDDEN);
		this->SetDesc(_("Lists information about the named registered channel"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];

		User *u = source.u;
		ChannelInfo *ci = source.ci;

		bool has_auspex = u->IsIdentified() && u->HasPriv("chanserv/auspex");
		bool show_all = false;

		if (ci->HasFlag(CI_FORBIDDEN))
		{
			if (u->HasMode(UMODE_OPER) && !ci->forbidby.empty())
				source.Reply(_(CHAN_X_FORBIDDEN_OPER), chan.c_str(), ci->forbidby.c_str(), !ci->forbidreason.empty() ? ci->forbidreason.c_str() : _(NO_REASON));
			else
				source.Reply(_(CHAN_X_FORBIDDEN), chan.c_str());

			return MOD_CONT;
		}

		/* Should we show all fields? Only for sadmins and identified users */
		if (has_auspex || check_access(u, ci, CA_INFO))
			show_all = true;

		source.Reply(_(CHAN_INFO_HEADER), chan.c_str());
		source.Reply(_("        Founder: %s"), ci->founder->display.c_str());

		if (show_all && ci->successor)
			source.Reply(_("      Successor: %s"), ci->successor->display.c_str());

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
			CheckOptStr(optbuf, CI_OPNOTICE, _("OP Notice"), ci, u->Account());
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
			CheckOptStr(optbuf, CI_XOP, _("xOP lists system"), ci, u->Account());
			CheckOptStr(optbuf, CI_PERSIST, _("Persistant"), ci, u->Account());
			CheckOptStr(optbuf, CI_NO_EXPIRE, _("No expire"), ci, u->Account());

			source.Reply(_(NICK_INFO_OPTIONS), optbuf.empty() ? _("None") : optbuf.c_str());
			source.Reply(_("      Mode lock: %s"), ci->GetMLockAsString(true).c_str());

			if (!ci->HasFlag(CI_NO_EXPIRE))
				source.Reply(_("      Expires on: %s"), do_strftime(ci->last_used + Config->CSExpire).c_str());
		}
		if (ci->HasFlag(CI_SUSPENDED))
			source.Reply(_("      Suspended: [%s] %s"), ci->forbidby.c_str(), !ci->forbidreason.empty() ? ci->forbidreason.c_str() : _(NO_REASON));

		FOREACH_MOD(I_OnChanInfo, OnChanInfo(source, ci, show_all));

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002INFO \037channel\037\002\n"
				" \n"
				"Lists information about the named registered channel,\n"
				"including its founder, time of registration, last time\n"
				"used, description, and mode lock, if any. If \002ALL\002 is \n"
				"specified, the entry message and successor will also \n"
				"be displayed."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "INFO", _("INFO \037channel\037"));
	}
};

class CSInfo : public Module
{
	CommandCSInfo commandcsinfo;

 public:
	CSInfo(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		if (!chanserv)
			throw ModuleException("ChanServ is not loaded!");

		this->AddCommand(chanserv->Bot(), &commandcsinfo);
	}
};

MODULE_INIT(CSInfo)

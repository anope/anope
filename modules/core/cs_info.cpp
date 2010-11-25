/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
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
	CommandCSInfo() : Command("INFO", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetFlag(CFLAG_ALLOW_SUSPENDED);
		this->SetFlag(CFLAG_ALLOW_FORBIDDEN);
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &chan = params[0];

		User *u = source.u;
		ChannelInfo *ci = source.ci;

		bool has_auspex = u->IsIdentified() && u->Account()->HasPriv("chanserv/auspex");
		bool show_all = false;

		if (ci->HasFlag(CI_FORBIDDEN))
		{
			if (is_oper(u) && !ci->forbidby.empty())
				source.Reply(CHAN_X_FORBIDDEN_OPER, chan.c_str(), ci->forbidby.c_str(), !ci->forbidreason.empty() ? ci->forbidreason.c_str() : GetString(u, NO_REASON).c_str());
			else
				source.Reply(CHAN_X_FORBIDDEN, chan.c_str());

			return MOD_CONT;
		}

		/* Should we show all fields? Only for sadmins and identified users */
		if (has_auspex || check_access(u, ci, CA_INFO))
			show_all = true;

		u->SendMessage(ChanServ, CHAN_INFO_HEADER, chan.c_str());
		u->SendMessage(ChanServ, CHAN_INFO_NO_FOUNDER, ci->founder->display.c_str());

		if (show_all && ci->successor)
			source.Reply(CHAN_INFO_NO_SUCCESSOR, ci->successor->display.c_str());

		u->SendMessage(ChanServ, CHAN_INFO_DESCRIPTION, ci->desc.c_str());
		u->SendMessage(ChanServ, CHAN_INFO_TIME_REGGED, do_strftime(ci->time_registered).c_str());
		u->SendMessage(ChanServ, CHAN_INFO_LAST_USED, do_strftime(ci->last_used).c_str());

		ModeLock *secret = ci->GetMLock(CMODE_SECRET);
		if (!ci->last_topic.empty() && (show_all || ((!secret || secret->set == false) && (!ci->c || !ci->c->HasMode(CMODE_SECRET)))))
		{
			source.Reply(CHAN_INFO_LAST_TOPIC, ci->last_topic.c_str());
			source.Reply(CHAN_INFO_TOPIC_SET_BY, ci->last_topic_setter.c_str());
		}

		if (show_all)
		{
			source.Reply(CHAN_INFO_BANTYPE, ci->bantype);
			Anope::string optbuf;

			CheckOptStr(optbuf, CI_KEEPTOPIC, GetString(u, CHAN_INFO_OPT_KEEPTOPIC), ci, u->Account());
			CheckOptStr(optbuf, CI_OPNOTICE, GetString(u, CHAN_INFO_OPT_OPNOTICE), ci, u->Account());
			CheckOptStr(optbuf, CI_PEACE, GetString(u, CHAN_INFO_OPT_PEACE), ci, u->Account());
			CheckOptStr(optbuf, CI_PRIVATE, GetString(u, NICK_INFO_OPT_PRIVATE), ci, u->Account());
			CheckOptStr(optbuf, CI_RESTRICTED, GetString(u, CHAN_INFO_OPT_RESTRICTED), ci, u->Account());
			CheckOptStr(optbuf, CI_SECURE, GetString(u, CHAN_INFO_OPT_SECURE), ci, u->Account());
			CheckOptStr(optbuf, CI_SECUREFOUNDER, GetString(u, CHAN_INFO_OPT_SECUREFOUNDER), ci, u->Account());
			CheckOptStr(optbuf, CI_SECUREOPS, GetString(u, CHAN_INFO_OPT_SECUREOPS), ci, u->Account());
			if (ci->HasFlag(CI_SIGNKICK))
				CheckOptStr(optbuf, CI_SIGNKICK, GetString(u, CHAN_INFO_OPT_SIGNKICK), ci, u->Account());
			else
				CheckOptStr(optbuf, CI_SIGNKICK_LEVEL, GetString(u, CHAN_INFO_OPT_SIGNKICK), ci, u->Account());
			CheckOptStr(optbuf, CI_TOPICLOCK, GetString(u, CHAN_INFO_OPT_TOPICLOCK), ci, u->Account());
			CheckOptStr(optbuf, CI_XOP, GetString(u, CHAN_INFO_OPT_XOP), ci, u->Account());
			CheckOptStr(optbuf, CI_PERSIST, GetString(u, CHAN_INFO_OPT_PERSIST), ci, u->Account());

			source.Reply(NICK_INFO_OPTIONS, optbuf.empty() ? GetString(u, NICK_INFO_OPT_NONE).c_str() : optbuf.c_str());
			source.Reply(CHAN_INFO_MODE_LOCK, get_mlock_modes(ci, 1).c_str());

			// XXX: we could just as easily (and tidily) merge this in with the flags display above.
			if (ci->HasFlag(CI_NO_EXPIRE))
				source.Reply(CHAN_INFO_NO_EXPIRE);
			else
				source.Reply(CHAN_INFO_EXPIRE, do_strftime(ci->last_used + Config->CSExpire).c_str());
		}
		if (ci->HasFlag(CI_SUSPENDED))
			source.Reply(CHAN_X_SUSPENDED, ci->forbidby.c_str(), !ci->forbidreason.empty() ? ci->forbidreason.c_str() : GetString(u, NO_REASON).c_str());

		FOREACH_MOD(I_OnChanInfo, OnChanInfo(u, ci, show_all));

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		u->SendMessage(ChanServ, CHAN_HELP_INFO);

		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		SyntaxError(ChanServ, u, "INFO", CHAN_INFO_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		u->SendMessage(ChanServ, CHAN_HELP_CMD_INFO);
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

		this->AddCommand(ChanServ, &commandcsinfo);
	}
};

MODULE_INIT(CSInfo)

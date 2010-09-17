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
			const char *commastr = getstring(nc, COMMA_SPACE);
			if (!buf.empty())
				buf += commastr;

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

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		char buf[BUFSIZE];
		struct tm *tm;
		bool has_auspex = u->IsIdentified() && u->Account()->HasPriv("chanserv/auspex");
		bool show_all = false;
		time_t expt;

		ChannelInfo *ci = cs_findchan(chan);

		if (ci->HasFlag(CI_FORBIDDEN))
		{
			if (is_oper(u) && !ci->forbidby.empty())
				notice_lang(Config->s_ChanServ, u, CHAN_X_FORBIDDEN_OPER, chan.c_str(), ci->forbidby.c_str(), !ci->forbidreason.empty() ? ci->forbidreason.c_str() : getstring(u, NO_REASON));
			else
				notice_lang(Config->s_ChanServ, u, CHAN_X_FORBIDDEN, chan.c_str());

			return MOD_CONT;
		}

		/* Should we show all fields? Only for sadmins and identified users */
		if (has_auspex || check_access(u, ci, CA_INFO))
			show_all = true;

		notice_lang(Config->s_ChanServ, u, CHAN_INFO_HEADER, chan.c_str());
		notice_lang(Config->s_ChanServ, u, CHAN_INFO_NO_FOUNDER, ci->founder->display.c_str());

		if (show_all && ci->successor)
			notice_lang(Config->s_ChanServ, u, CHAN_INFO_NO_SUCCESSOR, ci->successor->display.c_str());

		notice_lang(Config->s_ChanServ, u, CHAN_INFO_DESCRIPTION, ci->desc.c_str());
		tm = localtime(&ci->time_registered);
		strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
		notice_lang(Config->s_ChanServ, u, CHAN_INFO_TIME_REGGED, buf);
		tm = localtime(&ci->last_used);
		strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
		notice_lang(Config->s_ChanServ, u, CHAN_INFO_LAST_USED, buf);

		if (!ci->last_topic.empty() && (show_all || (!ci->HasMLock(CMODE_SECRET, true) && (!ci->c || !ci->c->HasMode(CMODE_SECRET)))))
		{
			notice_lang(Config->s_ChanServ, u, CHAN_INFO_LAST_TOPIC, ci->last_topic.c_str());
			notice_lang(Config->s_ChanServ, u, CHAN_INFO_TOPIC_SET_BY, ci->last_topic_setter.c_str());
		}

		if (!ci->entry_message.empty() && show_all)
			notice_lang(Config->s_ChanServ, u, CHAN_INFO_ENTRYMSG, ci->entry_message.c_str());

		if (show_all)
		{
			notice_lang(Config->s_ChanServ, u, CHAN_INFO_BANTYPE, ci->bantype);
			Anope::string optbuf;

			CheckOptStr(optbuf, CI_KEEPTOPIC, getstring(u, CHAN_INFO_OPT_KEEPTOPIC), ci, u->Account());
			CheckOptStr(optbuf, CI_OPNOTICE, getstring(u, CHAN_INFO_OPT_OPNOTICE), ci, u->Account());
			CheckOptStr(optbuf, CI_PEACE, getstring(u, CHAN_INFO_OPT_PEACE), ci, u->Account());
			CheckOptStr(optbuf, CI_PRIVATE, getstring(u, CHAN_INFO_OPT_PRIVATE), ci, u->Account());
			CheckOptStr(optbuf, CI_RESTRICTED, getstring(u, CHAN_INFO_OPT_RESTRICTED), ci, u->Account());
			CheckOptStr(optbuf, CI_SECURE, getstring(u, CHAN_INFO_OPT_SECURE), ci, u->Account());
			CheckOptStr(optbuf, CI_SECUREFOUNDER, getstring(u, CHAN_INFO_OPT_SECUREFOUNDER), ci, u->Account());
			CheckOptStr(optbuf, CI_SECUREOPS, getstring(u, CHAN_INFO_OPT_SECUREOPS), ci, u->Account());
			if (ci->HasFlag(CI_SIGNKICK))
				CheckOptStr(optbuf, CI_SIGNKICK, getstring(u, CHAN_INFO_OPT_SIGNKICK), ci, u->Account());
			else
				CheckOptStr(optbuf, CI_SIGNKICK_LEVEL, getstring(u, CHAN_INFO_OPT_SIGNKICK), ci, u->Account());
			CheckOptStr(optbuf, CI_TOPICLOCK, getstring(u, CHAN_INFO_OPT_TOPICLOCK), ci, u->Account());
			CheckOptStr(optbuf, CI_XOP, getstring(u, CHAN_INFO_OPT_XOP), ci, u->Account());
			CheckOptStr(optbuf, CI_PERSIST, getstring(u, CHAN_INFO_OPT_PERSIST), ci, u->Account());

			notice_lang(Config->s_ChanServ, u, CHAN_INFO_OPTIONS, optbuf.empty() ? getstring(u, CHAN_INFO_OPT_NONE) : optbuf.c_str());
			notice_lang(Config->s_ChanServ, u, CHAN_INFO_MODE_LOCK, get_mlock_modes(ci, 1).c_str());

			// XXX: we could just as easily (and tidily) merge this in with the flags display above.
			if (ci->HasFlag(CI_NO_EXPIRE))
				notice_lang(Config->s_ChanServ, u, CHAN_INFO_NO_EXPIRE);
			else
			{
				expt = ci->last_used + Config->CSExpire;
				tm = localtime(&expt);
				strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
				notice_lang(Config->s_ChanServ, u, CHAN_INFO_EXPIRE, buf);
			}
		}
		if (ci->HasFlag(CI_SUSPENDED))
			notice_lang(Config->s_ChanServ, u, CHAN_X_SUSPENDED, ci->forbidby.c_str(), !ci->forbidreason.empty() ? ci->forbidreason.c_str() : getstring(u, NO_REASON));

		FOREACH_MOD(I_OnChanInfo, OnChanInfo(u, ci, show_all));

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_lang(Config->s_ChanServ, u, CHAN_HELP_INFO);

		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_ChanServ, u, "INFO", CHAN_INFO_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_ChanServ, u, CHAN_HELP_CMD_INFO);
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

/* ChanServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

class CommandCSInfo : public Command
{       
	void CheckOptStr(std::string &buf, ChannelInfoFlag opt, const std::string &str, ChannelInfo *ci, NickCore *nc)
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
	CommandCSInfo() : Command("INFO", 1, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetFlag(CFLAG_ALLOW_SUSPENDED);
		this->SetFlag(CFLAG_ALLOW_FORBIDDEN);
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		ci::string param = params.size() > 1 ? params[1] : "";
		ChannelInfo *ci;
		char buf[BUFSIZE];
		struct tm *tm;
		bool has_auspex = u->nc && u->nc->HasPriv("chanserv/auspex");
		int show_all = 0;
		time_t expt;
		ChannelMode *cm;

		ci = cs_findchan(chan);

		if (ci->HasFlag(CI_FORBIDDEN))
		{
			if (is_oper(u) && ci->forbidby)
				notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN_OPER, chan,
							ci->forbidby,
							(ci->forbidreason ? ci->
							 forbidreason : getstring(u, NO_REASON)));
			else
				notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);

			return MOD_CONT;
		}

		/* Should we show all fields? Only for sadmins and identified users */
		if (!param.empty() && param == "ALL" && (check_access(u, ci, CA_INFO) || has_auspex))
			show_all = 1;

		notice_lang(s_ChanServ, u, CHAN_INFO_HEADER, chan);
		notice_lang(s_ChanServ, u, CHAN_INFO_NO_FOUNDER, ci->founder->display);

		if (show_all && ci->successor)
			notice_lang(s_ChanServ, u, CHAN_INFO_NO_SUCCESSOR, ci->successor->display);

		notice_lang(s_ChanServ, u, CHAN_INFO_DESCRIPTION, ci->desc);
		tm = localtime(&ci->time_registered);
		strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
		notice_lang(s_ChanServ, u, CHAN_INFO_TIME_REGGED, buf);
		tm = localtime(&ci->last_used);
		strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
		notice_lang(s_ChanServ, u, CHAN_INFO_LAST_USED, buf);

		cm = ModeManager::FindChannelModeByName(CMODE_SECRET);
		// XXX: yuck.
		if (ci->last_topic &&
				(show_all || (!ci->HasMLock(cm->Name, true))
											&& (!ci->c || !(ci->c->HasMode(CMODE_SECRET)))))
		{
			notice_lang(s_ChanServ, u, CHAN_INFO_LAST_TOPIC, ci->last_topic);
			notice_lang(s_ChanServ, u, CHAN_INFO_TOPIC_SET_BY, ci->last_topic_setter);
		}

		if (ci->entry_message && show_all)
			notice_lang(s_ChanServ, u, CHAN_INFO_ENTRYMSG, ci->entry_message);
		if (ci->url)
			notice_lang(s_ChanServ, u, CHAN_INFO_URL, ci->url);
		if (ci->email)
			notice_lang(s_ChanServ, u, CHAN_INFO_EMAIL, ci->email);

		if (show_all)
		{
			notice_lang(s_ChanServ, u, CHAN_INFO_BANTYPE, ci->bantype);
			std::string optbuf;

			CheckOptStr(optbuf, CI_KEEPTOPIC, getstring(u,  CHAN_INFO_OPT_KEEPTOPIC), ci, u->nc);
			CheckOptStr(optbuf, CI_OPNOTICE, getstring(u,  CHAN_INFO_OPT_OPNOTICE),	ci, u->nc);
			CheckOptStr(optbuf, CI_PEACE, getstring(u,  CHAN_INFO_OPT_PEACE), ci, u->nc);
			CheckOptStr(optbuf, CI_PRIVATE,	getstring(u,  CHAN_INFO_OPT_PRIVATE), ci, u->nc);
			CheckOptStr(optbuf, CI_RESTRICTED, getstring(u,  CHAN_INFO_OPT_RESTRICTED), ci, u->nc);
			CheckOptStr(optbuf, CI_SECURE, getstring(u,  CHAN_INFO_OPT_SECURE), ci, u->nc);
			CheckOptStr(optbuf, CI_SECUREFOUNDER, getstring(u,  CHAN_INFO_OPT_SECUREFOUNDER), ci, u->nc);
			CheckOptStr(optbuf, CI_SECUREOPS, getstring(u,  CHAN_INFO_OPT_SECUREOPS), ci, u->nc);
			if (ci->HasFlag(CI_SIGNKICK))
				CheckOptStr(optbuf, CI_SIGNKICK, getstring(u,  CHAN_INFO_OPT_SIGNKICK), ci, u->nc);
			else
				CheckOptStr(optbuf, CI_SIGNKICK_LEVEL, getstring(u,  CHAN_INFO_OPT_SIGNKICK), ci, u->nc);
			CheckOptStr(optbuf, CI_TOPICLOCK, getstring(u,	CHAN_INFO_OPT_TOPICLOCK), ci, u->nc);
			CheckOptStr(optbuf, CI_XOP, getstring(u,  CHAN_INFO_OPT_XOP), ci, u->nc);
			CheckOptStr(optbuf, CI_PERSIST, getstring(u, CHAN_INFO_OPT_PERSIST), ci, u->nc);

			notice_lang(s_ChanServ, u, CHAN_INFO_OPTIONS,	optbuf.empty() ? getstring(u, CHAN_INFO_OPT_NONE) : optbuf.c_str());
			notice_lang(s_ChanServ, u, CHAN_INFO_MODE_LOCK,	get_mlock_modes(ci, 1));

			// XXX: we could just as easily (and tidily) merge this in with the flags display above.
			if (ci->HasFlag(CI_NO_EXPIRE))
			{
				notice_lang(s_ChanServ, u, CHAN_INFO_NO_EXPIRE);
			}
			else
			{
				if (has_auspex)
				{
					expt = ci->last_used + CSExpire;
					tm = localtime(&expt);
					strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
					notice_lang(s_ChanServ, u, CHAN_INFO_EXPIRE, buf);
				}
			}
		}
		if (ci->HasFlag(CI_SUSPENDED))
		{
			notice_lang(s_ChanServ, u, CHAN_X_SUSPENDED, ci->forbidby, (ci->forbidreason ? ci->forbidreason : getstring(u, NO_REASON)));
		}

		if (!show_all && (check_access(u, ci, CA_INFO) || has_auspex))
			notice_lang(s_ChanServ, u, NICK_INFO_FOR_MORE, s_ChanServ, ci->name);
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_INFO);
		if (u->nc && u->nc->IsServicesOper())
			notice_lang(s_ChanServ, u, CHAN_SERVADMIN_HELP_INFO);

		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(s_ChanServ, u, "INFO", CHAN_INFO_SYNTAX);
	}
};

class CSInfo : public Module
{
 public:
	CSInfo(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSInfo());

		ModuleManager::Attach(I_OnChanServHelp, this);
	}
	void OnChanServHelp(User *u)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_CMD_INFO);
	}
};

MODULE_INIT(CSInfo)

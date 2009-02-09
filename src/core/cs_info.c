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


/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User * u)
{
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_INFO);
}


class CommandCSInfo : public Command
{                  // cannot be const, as it is modified.
	void CheckOptStr(std::string &buf, int opt, const std::string &str, ChannelInfo *ci, NickAlias *na)
	{
		if (ci->flags & opt)
		{
			const char *commastr = getstring(na, COMMA_SPACE);
			if (!buf.empty())
				buf += commastr;

			buf += str;
		}
	}

 public:
	CommandCSInfo() : Command("INFO", 1, 2)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		char *chan = strtok(NULL, " ");
		char *param = strtok(NULL, " ");
		ChannelInfo *ci;
		char buf[BUFSIZE];
		struct tm *tm;
		int is_servadmin = is_services_admin(u);
		int show_all = 0;
		time_t expt;

		if (!(ci = cs_findchan(chan)))
		{
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
			return MOD_CONT;
		}

		if (ci->flags & CI_VERBOTEN)
		{
			if (is_oper(u) && ci->forbidby)
				notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN_OPER, chan,
							ci->forbidby,
							(ci->forbidreason ? ci->
							 forbidreason : getstring(u->na, NO_REASON)));
			else
				notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);

			return MOD_CONT;
		}


		/* Should we show all fields? Only for sadmins and identified users */
		if (param && stricmp(param, "ALL") == 0 && (check_access(u, ci, CA_INFO) || is_servadmin))
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
		// XXX: yuck.
		if (ci->last_topic &&
				(show_all || (!(ci->mlock_on & anope_get_secret_mode())
											&& (!ci->c || !(ci->c->mode & anope_get_secret_mode())))))
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

			CheckOptStr(optbuf, CI_KEEPTOPIC,											getstring(u->na,  CHAN_INFO_OPT_KEEPTOPIC),			ci, u->na);
			CheckOptStr(optbuf, CI_OPNOTICE,											getstring(u->na,  CHAN_INFO_OPT_OPNOTICE),			ci, u->na);
			CheckOptStr(optbuf, CI_PEACE,													getstring(u->na,  CHAN_INFO_OPT_PEACE),					ci, u->na);
			CheckOptStr(optbuf, CI_PRIVATE,												getstring(u->na,  CHAN_INFO_OPT_PRIVATE),				ci, u->na);
			CheckOptStr(optbuf, CI_RESTRICTED,										getstring(u->na,  CHAN_INFO_OPT_RESTRICTED),		ci, u->na);
			CheckOptStr(optbuf, CI_SECURE,												getstring(u->na,  CHAN_INFO_OPT_SECURE),				ci, u->na);
			CheckOptStr(optbuf, CI_SECUREFOUNDER,									getstring(u->na,  CHAN_INFO_OPT_SECUREFOUNDER),	ci, u->na);
			CheckOptStr(optbuf, CI_SECUREOPS,											getstring(u->na,  CHAN_INFO_OPT_SECUREOPS),			ci, u->na);
			CheckOptStr(optbuf, CI_SIGNKICK | CI_SIGNKICK_LEVEL,	getstring(u->na,  CHAN_INFO_OPT_SIGNKICK),			ci, u->na);
			CheckOptStr(optbuf, CI_TOPICLOCK,											getstring(u->na,	CHAN_INFO_OPT_TOPICLOCK),			ci, u->na);
			CheckOptStr(optbuf, CI_XOP,														getstring(u->na,  CHAN_INFO_OPT_XOP),						ci, u->na);

			notice_lang(s_ChanServ, u, CHAN_INFO_OPTIONS,	optbuf.empty() ? getstring(u->na, CHAN_INFO_OPT_NONE) : optbuf.c_str());
			notice_lang(s_ChanServ, u, CHAN_INFO_MODE_LOCK,	get_mlock_modes(ci, 1));

			// XXX: we could just as easily (and tidily) merge this in with the flags display above.
			if (ci->flags & CI_NO_EXPIRE)
			{
				notice_lang(s_ChanServ, u, CHAN_INFO_NO_EXPIRE);
			}
			else
			{
				if (is_servadmin)
				{
					expt = ci->last_used + CSExpire;
					tm = localtime(&expt);
					strftime_lang(buf, sizeof(buf), u, STRFTIME_DATE_TIME_FORMAT, tm);
					notice_lang(s_ChanServ, u, CHAN_INFO_EXPIRE, buf);
				}
			}
		}
		if (ci->flags & CI_SUSPENDED)
		{
			notice_lang(s_ChanServ, u, CHAN_X_SUSPENDED, ci->forbidby, (ci->forbidreason ? ci->forbidreason : getstring(u->na, NO_REASON)));
		}

		if (!show_all && (check_access(u, ci, CA_INFO) || is_servadmin))
			notice_lang(s_ChanServ, u, NICK_INFO_FOR_MORE, s_ChanServ, ci->name);
		return MOD_CONT;
	}

	bool OnUserHelp(User *u)
	{
		if (is_services_admin(u) || is_services_root(u))
			notice_lang(s_ChanServ, u, CHAN_SERVADMIN_HELP_INFO);
		else
			notice_lang(s_NickServ, u, CHAN_HELP_INFO);

		return true;
	}

	void OnSyntaxError(User *u)
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
		this->AddCommand(CHANSERV, new CommandCSInfo(), MOD_UNIQUE);

		this->SetChanHelp(myChanServHelp);
	}
};




MODULE_INIT("cs_info", CSInfo)

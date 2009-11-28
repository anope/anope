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

class CommandCSRegister : public Command
{
 public:
	CommandCSRegister() : Command("REGISTER", 2, 2)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTEREDCHANNEL);
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *desc = params[1].c_str();
		Channel *c;
		ChannelInfo *ci;
		ChannelMode *cm;

		if (readonly)
		{
			notice_lang(Config.s_ChanServ, u, CHAN_REGISTER_DISABLED);
			return MOD_CONT;
		}

		if (*chan == '&')
			notice_lang(Config.s_ChanServ, u, CHAN_REGISTER_NOT_LOCAL);
		else if (*chan != '#')
			notice_lang(Config.s_ChanServ, u, CHAN_SYMBOL_REQUIRED);
		else if (!ircdproto->IsChannelValid(chan))
			notice_lang(Config.s_ChanServ, u, CHAN_X_INVALID, chan);
		else if (!(c = findchan(chan)))
			notice_lang(Config.s_ChanServ, u, CHAN_REGISTER_NONE_CHANNEL, chan);
		else if ((ci = cs_findchan(chan)))
			notice_lang(Config.s_ChanServ, u, CHAN_ALREADY_REGISTERED, chan);
		else if (!stricmp(chan, "#"))
			notice_lang(Config.s_ChanServ, u, CHAN_MAY_NOT_BE_REGISTERED, chan);
		else if (!chan_has_user_status(c, u, CUS_OP))
			notice_lang(Config.s_ChanServ, u, CHAN_MUST_BE_CHANOP);
		else if (Config.CSMaxReg && u->nc->channelcount >= Config.CSMaxReg && !u->nc->HasPriv("chanserv/no-register-limit"))
			notice_lang(Config.s_ChanServ, u, u->nc->channelcount > Config.CSMaxReg ? CHAN_EXCEEDED_CHANNEL_LIMIT : CHAN_REACHED_CHANNEL_LIMIT, Config.CSMaxReg);
		else if (!(ci = new ChannelInfo(chan)))
		{
			alog("%s: makechan() failed for REGISTER %s", Config.s_ChanServ, chan);
			notice_lang(Config.s_ChanServ, u, CHAN_REGISTRATION_FAILED);
		}
		else
		{
			c->ci = ci;
			ci->c = c;
			ci->founder = u->nc;
			ci->desc = sstrdup(desc);

			if (c->topic)
			{
				ci->last_topic = sstrdup(c->topic);
				strscpy(ci->last_topic_setter, c->topic_setter, NICKMAX);
				ci->last_topic_time = c->topic_time;
			}
			else strscpy(ci->last_topic_setter, Config.s_ChanServ, NICKMAX); /* Set this to something, otherwise it will maliform the topic */

			ci->bi = NULL;
			++ci->founder->channelcount;
			alog("%s: Channel '%s' registered by %s!%s@%s", Config.s_ChanServ, chan, u->nick, u->GetIdent().c_str(), u->host);
			notice_lang(Config.s_ChanServ, u, CHAN_REGISTERED, chan, u->nick);

			/* Implement new mode lock */
			check_modes(c);
			/* On most ircds you do not receive the admin/owner mode till its registered */
			if ((cm = ModeManager::FindChannelModeByName(CMODE_OWNER)))
				ircdproto->SendMode(findbot(Config.s_ChanServ), chan, "+%c %s", cm->ModeChar, u->nick);
			else if ((cm = ModeManager::FindChannelModeByName(CMODE_PROTECT)))
				ircdproto->SendMode(findbot(Config.s_ChanServ), chan, "+%c %s", cm->ModeChar, u->nick);

			/* Mark the channel as persistant */
			if (c->HasMode(CMODE_PERM))
				ci->SetFlag(CI_PERSIST);
			/* Persist may be in def cflags, set it here */
			else if (ci->HasFlag(CI_PERSIST) && (cm = ModeManager::FindChannelModeByName(CMODE_PERM)))
			{
				ircdproto->SendMode(whosends(ci), chan, "+%c", cm->ModeChar);
				ci->SetFlag(CI_PERSIST);
			}

			FOREACH_MOD(I_OnChanRegistered, OnChanRegistered(ci));
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_REGISTER, Config.s_ChanServ);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "REGISTER", CHAN_REGISTER_SYNTAX);
	}
};

class CSRegister : public Module
{
 public:
	CSRegister(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(CHANSERV, new CommandCSRegister());

		ModuleManager::Attach(I_OnChanServHelp, this);
	}
	void OnChanServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_REGISTER);
	}
};

MODULE_INIT(CSRegister)

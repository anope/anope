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

void myChanServHelp(User * u);

class CommandCSRegister : public Command
{
 public:
	CommandCSRegister() : Command("REGISTER", 3, 3)
	{

	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *pass = params[1].c_str();
		const char *desc = params[2].c_str();
		Channel *c;
		ChannelInfo *ci;
		struct u_chaninfolist *uc;
		char founderpass[PASSMAX];
		char tmp_pass[PASSMAX];

		if (readonly)
		{
			notice_lang(s_ChanServ, u, CHAN_REGISTER_DISABLED);
			return MOD_CONT;
		}

		if (checkDefCon(DEFCON_NO_NEW_CHANNELS))
		{
			notice_lang(s_ChanServ, u, OPER_DEFCON_DENIED);
			return MOD_CONT;
		}

		if (*chan == '&')
			notice_lang(s_ChanServ, u, CHAN_REGISTER_NOT_LOCAL);
		else if (*chan != '#')
			notice_lang(s_ChanServ, u, CHAN_SYMBOL_REQUIRED);
		else if (!ircdproto->IsChannelValid(chan))
			notice_lang(s_ChanServ, u, CHAN_X_INVALID, chan);
		else if (!(c = findchan(chan)))
			notice_lang(s_ChanServ, u, CHAN_REGISTER_NONE_CHANNEL, chan);
		else if ((ci = cs_findchan(chan)))
		{
			if (ci->flags & CI_FORBIDDEN)
			{
				alog("%s: Attempt to register FORBIDden channel %s by %s!%s@%s", s_ChanServ, ci->name, u->nick, u->GetIdent().c_str(), u->host);
				notice_lang(s_ChanServ, u, CHAN_MAY_NOT_BE_REGISTERED, chan);
			}
			else
				notice_lang(s_ChanServ, u, CHAN_ALREADY_REGISTERED, chan);
		}
		else if (!stricmp(chan, "#"))
			notice_lang(s_ChanServ, u, CHAN_MAY_NOT_BE_REGISTERED, chan);
		else if (!chan_has_user_status(c, u, CUS_OP))
			notice_lang(s_ChanServ, u, CHAN_MUST_BE_CHANOP);
		else if (CSMaxReg && u->nc->channelcount >= CSMaxReg && !u->nc->HasPriv("chanserv/no-register-limit"))
			notice_lang(s_ChanServ, u, u->nc->channelcount > CSMaxReg ? CHAN_EXCEEDED_CHANNEL_LIMIT : CHAN_REACHED_CHANNEL_LIMIT, CSMaxReg);
		else if (!stricmp(u->nick, pass) || (StrictPasswords && strlen(pass) < 5))
			notice_lang(s_ChanServ, u, MORE_OBSCURE_PASSWORD);
		else if (enc_encrypt_check_len(strlen(pass), PASSMAX - 1))
			notice_lang(s_ChanServ, u, PASSWORD_TOO_LONG);
		else if (!(ci = makechan(chan)))
		{
			alog("%s: makechan() failed for REGISTER %s", s_ChanServ, chan);
			notice_lang(s_ChanServ, u, CHAN_REGISTRATION_FAILED);

		}
		else if (strscpy(founderpass, pass, PASSMAX), enc_encrypt_in_place(founderpass, PASSMAX) < 0)
		{
			alog("%s: Couldn't encrypt password for %s (REGISTER)", s_ChanServ, chan);
			notice_lang(s_ChanServ, u, CHAN_REGISTRATION_FAILED);
			delchan(ci);
		}
		else
		{
			c->ci = ci;
			ci->c = c;
			ci->bantype = CSDefBantype;
			ci->flags = CSDefFlags;
			ci->mlock_on = ircd->defmlock;
			ci->memos.memomax = MSMaxMemos;
			ci->last_used = ci->time_registered;
			ci->founder = u->nc;

			memcpy(ci->founderpass, founderpass, PASSMAX);
			ci->desc = sstrdup(desc);
			if (c->topic)
			{
				ci->last_topic = sstrdup(c->topic);
				strscpy(ci->last_topic_setter, c->topic_setter, NICKMAX);
				ci->last_topic_time = c->topic_time;
			}
			else strscpy(ci->last_topic_setter, s_ChanServ, NICKMAX); /* Set this to something, otherwise it will maliform the topic */
			ci->bi = NULL;
			ci->botflags = BSDefFlags;
			++ci->founder->channelcount;
			alog("%s: Channel '%s' registered by %s!%s@%s", s_ChanServ, chan, u->nick, u->GetIdent().c_str(), u->host);
			notice_lang(s_ChanServ, u, CHAN_REGISTERED, chan, u->nick);

			if (enc_decrypt(ci->founderpass, tmp_pass, PASSMAX - 1) == 1)
				notice_lang(s_ChanServ, u, CHAN_PASSWORD_IS, tmp_pass);

			uc = new u_chaninfolist;
			uc->next = u->founder_chans;
			uc->prev = NULL;
			if (u->founder_chans)
				u->founder_chans->prev = uc;
			u->founder_chans = uc;
			uc->chan = ci;
			/* Implement new mode lock */
			check_modes(c);
			/* On most ircds you do not receive the admin/owner mode till its registered */
			if (ircd->admin)
				ircdproto->SendMode(findbot(s_ChanServ), chan, "%s %s", ircd->adminset, u->nick);
			if (ircd->owner && ircd->ownerset)
				ircdproto->SendMode(findbot(s_ChanServ), chan, "%s %s", ircd->ownerset, u->nick);
			send_event(EVENT_CHAN_REGISTERED, 1, chan);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_REGISTER);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "REGISTER", CHAN_REGISTER_SYNTAX);
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

		this->AddCommand(CHANSERV, new CommandCSRegister(), MOD_UNIQUE);

		this->SetChanHelp(myChanServHelp);
	}
};

/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User *u)
{
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_REGISTER);
}

MODULE_INIT("cs_register", CSRegister)

/* ChanServ core functions
 *
 * (C) 2003-2008 Anope Team
 * Contact us at info@anope.org
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

int do_register(User * u);
void myChanServHelp(User * u);

class CSRegister : public Module
{
 public:
	CSRegister(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		Command *c;

		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		c = createCommand("REGISTER", do_register, NULL, CHAN_HELP_REGISTER, -1, -1, -1, -1);
		c->help_param1 = s_NickServ;
		this->AddCommand(CHANSERV, c, MOD_UNIQUE);

		this->SetChanHelp(myChanServHelp);
	}
};



/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User * u)
{
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_REGISTER);
}

/**
 * The /cs register command.
 * @param u The user who issued the command
 * @param MOD_CONT to continue processing other modules, MOD_STOP to stop processing.
 **/
int do_register(User * u)
{
	char *chan = strtok(NULL, " ");
	char *pass = strtok(NULL, " ");
	char *desc = strtok(NULL, "");
	NickCore *nc;
	Channel *c;
	ChannelInfo *ci;
	struct u_chaninfolist *uc;
	int is_servadmin = is_services_admin(u);
	char founderpass[PASSMAX];
	char tmp_pass[PASSMAX];

	if (readonly) {
		notice_lang(s_ChanServ, u, CHAN_REGISTER_DISABLED);
		return MOD_CONT;
	}

	if (checkDefCon(DEFCON_NO_NEW_CHANNELS)) {
		notice_lang(s_ChanServ, u, OPER_DEFCON_DENIED);
		return MOD_CONT;
	}

	if (!desc) {
		syntax_error(s_ChanServ, u, "REGISTER", CHAN_REGISTER_SYNTAX);
	} else if (*chan == '&') {
		notice_lang(s_ChanServ, u, CHAN_REGISTER_NOT_LOCAL);
	} else if (*chan != '#') {
		notice_lang(s_ChanServ, u, CHAN_SYMBOL_REQUIRED);
	} else if (!ircdproto->IsChannelValid(chan)) {
		notice_lang(s_ChanServ, u, CHAN_X_INVALID, chan);
	} else if (!u->na || !(nc = u->na->nc)) {
		notice_lang(s_ChanServ, u, CHAN_MUST_REGISTER_NICK, s_NickServ);
	} else if (!nick_recognized(u)) {
		notice_lang(s_ChanServ, u, CHAN_MUST_IDENTIFY_NICK, s_NickServ,
					s_NickServ);
	} else if (!(c = findchan(chan))) {
		notice_lang(s_ChanServ, u, CHAN_REGISTER_NONE_CHANNEL, chan);
	} else if ((ci = cs_findchan(chan)) != NULL) {
		if (ci->flags & CI_VERBOTEN) {
			alog("%s: Attempt to register FORBIDden channel %s by %s!%s@%s", s_ChanServ, ci->name, u->nick, u->username, u->host);
			notice_lang(s_ChanServ, u, CHAN_MAY_NOT_BE_REGISTERED, chan);
		} else {
			notice_lang(s_ChanServ, u, CHAN_ALREADY_REGISTERED, chan);
		}
	} else if (!stricmp(chan, "#")) {
		notice_lang(s_ChanServ, u, CHAN_MAY_NOT_BE_REGISTERED, chan);
	} else if (!chan_has_user_status(c, u, CUS_OP)) {
		notice_lang(s_ChanServ, u, CHAN_MUST_BE_CHANOP);

	} else if (!is_servadmin && CSMaxReg && nc->channelcount >= CSMaxReg) {
		notice_lang(s_ChanServ, u, nc->channelcount >
					CSMaxReg ? CHAN_EXCEEDED_CHANNEL_LIMIT : CHAN_REACHED_CHANNEL_LIMIT, CSMaxReg);
	} else if (stricmp(u->nick, pass) == 0
			   || (StrictPasswords && strlen(pass) < 5)) {
		notice_lang(s_ChanServ, u, MORE_OBSCURE_PASSWORD);
	} else if(enc_encrypt_check_len(strlen(pass), PASSMAX - 1)) {
		notice_lang(s_ChanServ, u, PASSWORD_TOO_LONG);
	} else if (!(ci = makechan(chan))) {
		alog("%s: makechan() failed for REGISTER %s", s_ChanServ, chan);
		notice_lang(s_ChanServ, u, CHAN_REGISTRATION_FAILED);

	} else if (strscpy(founderpass, pass, PASSMAX),
			   enc_encrypt_in_place(founderpass, PASSMAX) < 0) {
		alog("%s: Couldn't encrypt password for %s (REGISTER)",
			 s_ChanServ, chan);
		notice_lang(s_ChanServ, u, CHAN_REGISTRATION_FAILED);
		delchan(ci);
	} else {
		c->ci = ci;
		ci->c = c;
		ci->bantype = CSDefBantype;
		ci->flags = CSDefFlags;
		ci->mlock_on = ircd->defmlock;
		ci->memos.memomax = MSMaxMemos;
		ci->last_used = ci->time_registered;
		ci->founder = nc;

		memset(pass, 0, strlen(pass));
		memcpy(ci->founderpass, founderpass, PASSMAX);
		ci->desc = sstrdup(desc);
		if (c->topic) {
			ci->last_topic = sstrdup(c->topic);
			strscpy(ci->last_topic_setter, c->topic_setter, NICKMAX);
			ci->last_topic_time = c->topic_time;
		} else {
			/* Set this to something, otherwise it will maliform the topic */
			strscpy(ci->last_topic_setter, s_ChanServ, NICKMAX);
		}
		ci->bi = NULL;
		ci->botflags = BSDefFlags;
		ci->founder->channelcount++;
		alog("%s: Channel '%s' registered by %s!%s@%s", s_ChanServ, chan,
			 u->nick, u->username, u->host);
		notice_lang(s_ChanServ, u, CHAN_REGISTERED, chan, u->nick);

		if(enc_decrypt(ci->founderpass,tmp_pass,PASSMAX - 1) == 1) {
			notice_lang(s_ChanServ, u, CHAN_PASSWORD_IS, tmp_pass);
		}

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
		if (ircd->admin) {
			ircdproto->SendMode(findbot(s_ChanServ), chan, "%s %s", ircd->adminset,
						   u->nick);
		}
		if (ircd->owner && ircd->ownerset) {
			ircdproto->SendMode(findbot(s_ChanServ), chan, "%s %s", ircd->ownerset,
						   u->nick);
		}
		send_event(EVENT_CHAN_REGISTERED, 1, chan);
	}
	return MOD_CONT;
}

MODULE_INIT("cs_register", CSRegister)

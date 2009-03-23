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
#include "encrypt.h"
#include "hashcomp.h"

class CommandCSSet : public Command
{
 private:
	CommandReturn DoSetFounder(User * u, ChannelInfo * ci, const char *param)
	{
		NickAlias *na = findnick(param);
		NickCore *nc, *nc0 = ci->founder;

		if (!na) {
			notice_lang(s_ChanServ, u, NICK_X_NOT_REGISTERED, param);
			return MOD_CONT;
		} else if (na->status & NS_FORBIDDEN) {
			notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, param);
			return MOD_CONT;
		}

		nc = na->nc;
		if (CSMaxReg && nc->channelcount >= CSMaxReg && !u->nc->HasPriv("chanserv/no-register-limit")) {
			notice_lang(s_ChanServ, u, CHAN_SET_FOUNDER_TOO_MANY_CHANS, param);
			return MOD_CONT;
		}

		alog("%s: Changing founder of %s from %s to %s by %s!%s@%s",
			 s_ChanServ, ci->name, ci->founder->display, nc->display, u->nick,
			 u->GetIdent().c_str(), u->host);

		/* Founder and successor must not be the same group */
		if (nc == ci->successor)
			ci->successor = NULL;

		nc0->channelcount--;
		ci->founder = nc;
		nc->channelcount++;

		notice_lang(s_ChanServ, u, CHAN_FOUNDER_CHANGED, ci->name, param);
		return MOD_CONT;
	}

	CommandReturn DoSetSuccessor(User * u, ChannelInfo * ci, const char *param)
	{
		NickAlias *na;
		NickCore *nc;

		if (param) {
			na = findnick(param);

			if (!na) {
				notice_lang(s_ChanServ, u, NICK_X_NOT_REGISTERED, param);
				return MOD_CONT;
			}
			if (na->status & NS_FORBIDDEN) {
				notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, param);
				return MOD_CONT;
			}
			if (na->nc == ci->founder) {
				notice_lang(s_ChanServ, u, CHAN_SUCCESSOR_IS_FOUNDER, param,
							ci->name);
				return MOD_CONT;
			}
			nc = na->nc;

		} else {
			nc = NULL;
		}

		alog("%s: Changing successor of %s from %s to %s by %s!%s@%s",
			 s_ChanServ, ci->name,
			 (ci->successor ? ci->successor->display : "none"),
			 (nc ? nc->display : "none"), u->nick, u->GetIdent().c_str(), u->host);

		ci->successor = nc;

		if (nc)
			notice_lang(s_ChanServ, u, CHAN_SUCCESSOR_CHANGED, ci->name,
						param);
		else
			notice_lang(s_ChanServ, u, CHAN_SUCCESSOR_UNSET, ci->name);
		return MOD_CONT;
	}

	CommandReturn DoSetPassword(User * u, ChannelInfo * ci, const char *param)
	{
		int len = strlen(param);

		if (stricmp(u->nick, param) == 0 || (StrictPasswords && len < 5)) {
			notice_lang(s_ChanServ, u, MORE_OBSCURE_PASSWORD);
			return MOD_CONT;
		}

		if (enc_encrypt_check_len(len ,PASSMAX - 1)) {
			notice_lang(s_ChanServ, u, PASSWORD_TOO_LONG);
			return MOD_CONT;
		}

		if (enc_encrypt(param, len, ci->founderpass, PASSMAX -1) < 0) {
			alog("%s: Failed to encrypt password for %s (set)", s_ChanServ,
				 ci->name);
			notice_lang(s_ChanServ, u, CHAN_SET_PASSWORD_FAILED);
			return MOD_CONT;
		}

		notice_lang(s_ChanServ, u, CHAN_PASSWORD_CHANGED, ci->name);

		if (get_access(u, ci) < ACCESS_FOUNDER) {
			alog("%s: %s!%s@%s set password as Services admin for %s",
				 s_ChanServ, u->nick, u->GetIdent().c_str(), u->host, ci->name);
			if (WallSetpass)
				ircdproto->SendGlobops(s_ChanServ,
								 "\2%s\2 set password as Services admin for channel \2%s\2",
								 u->nick, ci->name);
		} else {
			alog("%s: %s!%s@%s changed password of %s (founder: %s)",
				 s_ChanServ, u->nick, u->GetIdent().c_str(), u->host,
				 ci->name, ci->founder->display);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetDesc(User * u, ChannelInfo * ci, const char *param)
	{
		if (ci->desc)
			delete [] ci->desc;
		ci->desc = sstrdup(param);
		notice_lang(s_ChanServ, u, CHAN_DESC_CHANGED, ci->name, param);
		return MOD_CONT;
	}

	CommandReturn DoSetURL(User * u, ChannelInfo * ci, const char *param)
	{
		if (ci->url)
			delete [] ci->url;
		if (param) {
			ci->url = sstrdup(param);
			notice_lang(s_ChanServ, u, CHAN_URL_CHANGED, ci->name, param);
		} else {
			ci->url = NULL;
			notice_lang(s_ChanServ, u, CHAN_URL_UNSET, ci->name);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetEMail(User * u, ChannelInfo * ci, const char *param)
	{
		if (ci->email)
			delete [] ci->email;
		if (param) {
			ci->email = sstrdup(param);
			notice_lang(s_ChanServ, u, CHAN_EMAIL_CHANGED, ci->name, param);
		} else {
			ci->email = NULL;
			notice_lang(s_ChanServ, u, CHAN_EMAIL_UNSET, ci->name);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetEntryMsg(User * u, ChannelInfo * ci, const char *param)
	{
		if (ci->entry_message)
			delete [] ci->entry_message;
		if (param) {
			ci->entry_message = sstrdup(param);
			notice_lang(s_ChanServ, u, CHAN_ENTRY_MSG_CHANGED, ci->name,
						param);
		} else {
			ci->entry_message = NULL;
			notice_lang(s_ChanServ, u, CHAN_ENTRY_MSG_UNSET, ci->name);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetMLock(User * u, ChannelInfo * ci, const char *modes)
	{
		int add = -1;			   /* 1 if adding, 0 if deleting, -1 if neither */
		unsigned char mode;
		CBMode *cbm;

		if (checkDefCon(DEFCON_NO_MLOCK_CHANGE)) {
			notice_lang(s_ChanServ, u, OPER_DEFCON_DENIED);
			return MOD_CONT;
		}

		/* Reinitialize everything */
		if (ircd->chanreg) {
			ci->mlock_on = ircd->regmode;
		} else {
			ci->mlock_on = 0;
		}
		ci->mlock_off = ci->mlock_limit = 0;
		ci->mlock_key = NULL;
		if (ircd->fmode) {
			ci->mlock_flood = NULL;
		}
		if (ircd->Lmode) {
			ci->mlock_redirect = NULL;
		}

		std::string params(modes), param;
		unsigned space = params.find(' ');
		if (space != std::string::npos)
		{
			param = params.substr(space + 1);
			params = params.substr(0, space);
			modes = params.c_str();
		}
		spacesepstream modeparams(param);

		while ((mode = *modes++)) {
			switch (mode) {
			case '+':
				add = 1;
				continue;
			case '-':
				add = 0;
				continue;
			default:
				if (add < 0)
					continue;
			}

			if (static_cast<int>(mode) < 128 && (cbm = &cbmodes[static_cast<int>(mode)])->flag != 0) {
				if ((cbm->flags & CBM_NO_MLOCK)
					|| ((cbm->flags & CBM_NO_USER_MLOCK) && !is_oper(u))) {
					notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_IMPOSSIBLE_CHAR,
								mode);
				} else if (add) {
					ci->mlock_on |= cbm->flag;
					ci->mlock_off &= ~cbm->flag;
					if (cbm->cssetvalue)
					{
						modeparams.GetToken(param);
						cbm->cssetvalue(ci, param.c_str());
					}
				} else {
					ci->mlock_off |= cbm->flag;
					if (ci->mlock_on & cbm->flag) {
						ci->mlock_on &= ~cbm->flag;
						if (cbm->cssetvalue)
							cbm->cssetvalue(ci, NULL);
					}
				}
			} else {
				notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_UNKNOWN_CHAR, mode);
			}
		}						   /* while (*modes) */

		if (ircd->Lmode) {
			/* We can't mlock +L if +l is not mlocked as well. */
			if ((ci->mlock_on & ircd->chan_lmode)
				&& !(ci->mlock_on & anope_get_limit_mode())) {
				ci->mlock_on &= ~ircd->chan_lmode;
				delete [] ci->mlock_redirect;
				notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_L_REQUIRED);
			}
		}

		/* Some ircd we can't set NOKNOCK without INVITE */
		/* So check if we need there is a NOKNOCK MODE and that we need INVITEONLY */
		if (ircd->noknock && ircd->knock_needs_i) {
			if ((ci->mlock_on & ircd->noknock)
				&& !(ci->mlock_on & anope_get_invite_mode())) {
				ci->mlock_on &= ~ircd->noknock;
				notice_lang(s_ChanServ, u, CHAN_SET_MLOCK_K_REQUIRED);
			}
		}

		/* Since we always enforce mode r there is no way to have no
		 * mode lock at all.
		 */
		if (get_mlock_modes(ci, 0)) {
			notice_lang(s_ChanServ, u, CHAN_MLOCK_CHANGED, ci->name,
						get_mlock_modes(ci, 0));
		}

		/* Implement the new lock. */
		if (ci->c)
			check_modes(ci->c);
		return MOD_CONT;
	}

	CommandReturn DoSetBanType(User * u, ChannelInfo * ci, const char *param)
	{
		char *endptr;

		int16 bantype = strtol(param, &endptr, 10);

		if (*endptr != 0 || bantype < 0 || bantype > 3) {
			notice_lang(s_ChanServ, u, CHAN_SET_BANTYPE_INVALID, param);
		} else {
			ci->bantype = bantype;
			notice_lang(s_ChanServ, u, CHAN_SET_BANTYPE_CHANGED, ci->name,
						ci->bantype);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetKeepTopic(User * u, ChannelInfo * ci, const char *param)
	{
		if (stricmp(param, "ON") == 0) {
			ci->flags |= CI_KEEPTOPIC;
			notice_lang(s_ChanServ, u, CHAN_SET_KEEPTOPIC_ON, ci->name);
		} else if (stricmp(param, "OFF") == 0) {
			ci->flags &= ~CI_KEEPTOPIC;
			notice_lang(s_ChanServ, u, CHAN_SET_KEEPTOPIC_OFF, ci->name);
		} else {
			syntax_error(s_ChanServ, u, "SET KEEPTOPIC",
						 CHAN_SET_KEEPTOPIC_SYNTAX);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetTopicLock(User * u, ChannelInfo * ci, const char *param)
	{
		if (stricmp(param, "ON") == 0) {
			ci->flags |= CI_TOPICLOCK;
			notice_lang(s_ChanServ, u, CHAN_SET_TOPICLOCK_ON, ci->name);
		} else if (stricmp(param, "OFF") == 0) {
			ci->flags &= ~CI_TOPICLOCK;
			notice_lang(s_ChanServ, u, CHAN_SET_TOPICLOCK_OFF, ci->name);
		} else {
			syntax_error(s_ChanServ, u, "SET TOPICLOCK",
						 CHAN_SET_TOPICLOCK_SYNTAX);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetPrivate(User * u, ChannelInfo * ci, const char *param)
	{
		if (stricmp(param, "ON") == 0) {
			ci->flags |= CI_PRIVATE;
			notice_lang(s_ChanServ, u, CHAN_SET_PRIVATE_ON, ci->name);
		} else if (stricmp(param, "OFF") == 0) {
			ci->flags &= ~CI_PRIVATE;
			notice_lang(s_ChanServ, u, CHAN_SET_PRIVATE_OFF, ci->name);
		} else {
			syntax_error(s_ChanServ, u, "SET PRIVATE",
						 CHAN_SET_PRIVATE_SYNTAX);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetSecureOps(User * u, ChannelInfo * ci, const char *param)
	{
		if (stricmp(param, "ON") == 0) {
			ci->flags |= CI_SECUREOPS;
			notice_lang(s_ChanServ, u, CHAN_SET_SECUREOPS_ON, ci->name);
		} else if (stricmp(param, "OFF") == 0) {
			ci->flags &= ~CI_SECUREOPS;
			notice_lang(s_ChanServ, u, CHAN_SET_SECUREOPS_OFF, ci->name);
		} else {
			syntax_error(s_ChanServ, u, "SET SECUREOPS",
						 CHAN_SET_SECUREOPS_SYNTAX);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetSecureFounder(User * u, ChannelInfo * ci, const char *param)
	{
		if (stricmp(param, "ON") == 0) {
			ci->flags |= CI_SECUREFOUNDER;
			notice_lang(s_ChanServ, u, CHAN_SET_SECUREFOUNDER_ON, ci->name);
		} else if (stricmp(param, "OFF") == 0) {
			ci->flags &= ~CI_SECUREFOUNDER;
			notice_lang(s_ChanServ, u, CHAN_SET_SECUREFOUNDER_OFF, ci->name);
		} else {
			syntax_error(s_ChanServ, u, "SET SECUREFOUNDER",
						 CHAN_SET_SECUREFOUNDER_SYNTAX);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetRestricted(User * u, ChannelInfo * ci, const char *param)
	{
		if (stricmp(param, "ON") == 0) {
			ci->flags |= CI_RESTRICTED;
			if (ci->levels[CA_NOJOIN] < 0)
				ci->levels[CA_NOJOIN] = 0;
			notice_lang(s_ChanServ, u, CHAN_SET_RESTRICTED_ON, ci->name);
		} else if (stricmp(param, "OFF") == 0) {
			ci->flags &= ~CI_RESTRICTED;
			if (ci->levels[CA_NOJOIN] >= 0)
				ci->levels[CA_NOJOIN] = -2;
			notice_lang(s_ChanServ, u, CHAN_SET_RESTRICTED_OFF, ci->name);
		} else {
			syntax_error(s_ChanServ, u, "SET RESTRICTED",
						 CHAN_SET_RESTRICTED_SYNTAX);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetSecure(User * u, ChannelInfo * ci, const char *param)
	{
		if (stricmp(param, "ON") == 0) {
			ci->flags |= CI_SECURE;
			notice_lang(s_ChanServ, u, CHAN_SET_SECURE_ON, ci->name);
		} else if (stricmp(param, "OFF") == 0) {
			ci->flags &= ~CI_SECURE;
			notice_lang(s_ChanServ, u, CHAN_SET_SECURE_OFF, ci->name);
		} else {
			syntax_error(s_ChanServ, u, "SET SECURE", CHAN_SET_SECURE_SYNTAX);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetSignKick(User * u, ChannelInfo * ci, const char *param)
	{
		if (stricmp(param, "ON") == 0) {
			ci->flags |= CI_SIGNKICK;
			ci->flags &= ~CI_SIGNKICK_LEVEL;
			notice_lang(s_ChanServ, u, CHAN_SET_SIGNKICK_ON, ci->name);
		} else if (stricmp(param, "LEVEL") == 0) {
			ci->flags |= CI_SIGNKICK_LEVEL;
			ci->flags &= ~CI_SIGNKICK;
			notice_lang(s_ChanServ, u, CHAN_SET_SIGNKICK_LEVEL, ci->name);
		} else if (stricmp(param, "OFF") == 0) {
			ci->flags &= ~(CI_SIGNKICK | CI_SIGNKICK_LEVEL);
			notice_lang(s_ChanServ, u, CHAN_SET_SIGNKICK_OFF, ci->name);
		} else {
			syntax_error(s_ChanServ, u, "SET SIGNKICK",
						 CHAN_SET_SIGNKICK_SYNTAX);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetOpNotice(User * u, ChannelInfo * ci, const char *param)
	{
		if (stricmp(param, "ON") == 0) {
			ci->flags |= CI_OPNOTICE;
			notice_lang(s_ChanServ, u, CHAN_SET_OPNOTICE_ON, ci->name);
		} else if (stricmp(param, "OFF") == 0) {
			ci->flags &= ~CI_OPNOTICE;
			notice_lang(s_ChanServ, u, CHAN_SET_OPNOTICE_OFF, ci->name);
		} else {
			syntax_error(s_ChanServ, u, "SET OPNOTICE",
						 CHAN_SET_OPNOTICE_SYNTAX);
		}
		return MOD_CONT;
	}

#define CHECKLEV(lev) ((ci->levels[(lev)] != ACCESS_INVALID) && (access->level >= ci->levels[(lev)]))

	CommandReturn DoSetXOP(User * u, ChannelInfo * ci, const char *param)
	{
		if (stricmp(param, "ON") == 0) {
			if (!(ci->flags & CI_XOP)) {
				int i;
				ChanAccess *access;

				for (i = 0; i < ci->access.size(); i++) {
					access = ci->GetAccess(i);
					if (!access->in_use)
						continue;
					/* This will probably cause wrong levels to be set, but hey,
					 * it's better than losing it altogether.
					 */
					if (CHECKLEV(CA_AKICK) || CHECKLEV(CA_SET)) {
						access->level = ACCESS_SOP;
					} else if (CHECKLEV(CA_AUTOOP) || CHECKLEV(CA_OPDEOP)
							   || CHECKLEV(CA_OPDEOPME)) {
						access->level = ACCESS_AOP;
					} else if (ircd->halfop) {
						if (CHECKLEV(CA_AUTOHALFOP) || CHECKLEV(CA_HALFOP)
							|| CHECKLEV(CA_HALFOPME)) {
							access->level = ACCESS_HOP;
						}
					} else if (CHECKLEV(CA_AUTOVOICE) || CHECKLEV(CA_VOICE)
							   || CHECKLEV(CA_VOICEME)) {
						access->level = ACCESS_VOP;
					} else {
						access->in_use = 0;
						access->nc = NULL;
					}
				}

				/* The above may have set an access entry to not be in use, this will clean that up. */
				ci->CleanAccess();

				reset_levels(ci);
				ci->flags |= CI_XOP;
			}

			alog("%s: %s!%s@%s enabled XOP for %s", s_ChanServ, u->nick,
				 u->GetIdent().c_str(), u->host, ci->name);
			notice_lang(s_ChanServ, u, CHAN_SET_XOP_ON, ci->name);
		} else if (stricmp(param, "OFF") == 0) {
			ci->flags &= ~CI_XOP;

			alog("%s: %s!%s@%s disabled XOP for %s", s_ChanServ, u->nick,
				 u->GetIdent().c_str(), u->host, ci->name);
			notice_lang(s_ChanServ, u, CHAN_SET_XOP_OFF, ci->name);
		} else {
			syntax_error(s_ChanServ, u, "SET XOP", CHAN_SET_XOP_SYNTAX);
		}
		return MOD_CONT;
	}

#undef CHECKLEV

	CommandReturn DoSetPeace(User * u, ChannelInfo * ci, const char *param)
	{
		if (stricmp(param, "ON") == 0) {
			ci->flags |= CI_PEACE;
			notice_lang(s_ChanServ, u, CHAN_SET_PEACE_ON, ci->name);
		} else if (stricmp(param, "OFF") == 0) {
			ci->flags &= ~CI_PEACE;
			notice_lang(s_ChanServ, u, CHAN_SET_PEACE_OFF, ci->name);
		} else {
			syntax_error(s_ChanServ, u, "SET PEACE", CHAN_SET_PEACE_SYNTAX);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetNoExpire(User * u, ChannelInfo * ci, const char *param)
	{
		if (!u->nc->HasCommand("chanserv/set/noexpire"))
		{
			notice_lang(s_ChanServ, u, PERMISSION_DENIED);
			return MOD_CONT;
		}
		if (stricmp(param, "ON") == 0) {
			ci->flags |= CI_NO_EXPIRE;
			notice_lang(s_ChanServ, u, CHAN_SET_NOEXPIRE_ON, ci->name);
		} else if (stricmp(param, "OFF") == 0) {
			ci->flags &= ~CI_NO_EXPIRE;
			notice_lang(s_ChanServ, u, CHAN_SET_NOEXPIRE_OFF, ci->name);
		} else {
			syntax_error(s_ChanServ, u, "SET NOEXPIRE",
						 CHAN_SET_NOEXPIRE_SYNTAX);
		}
		return MOD_CONT;
	}

 public:
	CommandCSSet() : Command("SET", 2, 3)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *cmd = params[1].c_str();
		const char *param = params.size() > 2 ? params[2].c_str() : NULL;
		ChannelInfo *ci;
		bool is_servadmin = u->nc->HasPriv("chanserv/set");

		if (readonly) {
			notice_lang(s_ChanServ, u, CHAN_SET_DISABLED);
			return MOD_CONT;
		}

		if (!param && (!cmd || (stricmp(cmd, "SUCCESSOR") != 0 &&
								stricmp(cmd, "URL") != 0 &&
								stricmp(cmd, "EMAIL") != 0 &&
								stricmp(cmd, "ENTRYMSG") != 0))) {
			syntax_error(s_ChanServ, u, "SET", CHAN_SET_SYNTAX);
		} else if (!(ci = cs_findchan(chan))) {
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
		} else if (ci->flags & CI_FORBIDDEN) {
			notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
		} else if (!is_servadmin && !check_access(u, ci, CA_SET)) {
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
		} else if (stricmp(cmd, "FOUNDER") == 0) {
			if (!is_servadmin
				&& (ci->
					flags & CI_SECUREFOUNDER ? !is_real_founder(u,
																ci) :
					!is_founder(u, ci))) {
				notice_lang(s_ChanServ, u, ACCESS_DENIED);
			} else {
				DoSetFounder(u, ci, param);
			}
		} else if (stricmp(cmd, "SUCCESSOR") == 0) {
			if (!is_servadmin
				&& (ci->
					flags & CI_SECUREFOUNDER ? !is_real_founder(u,
																ci) :
					!is_founder(u, ci))) {
				notice_lang(s_ChanServ, u, ACCESS_DENIED);
			} else {
				DoSetSuccessor(u, ci, param);
			}
		} else if (stricmp(cmd, "PASSWORD") == 0) {
			if (!is_servadmin
				&& (ci->
					flags & CI_SECUREFOUNDER ? !is_real_founder(u,
																ci) :
					!is_founder(u, ci))) {
				notice_lang(s_ChanServ, u, ACCESS_DENIED);
			} else {
				DoSetPassword(u, ci, param);
			}
		} else if (stricmp(cmd, "DESC") == 0) {
			DoSetDesc(u, ci, param);
		} else if (stricmp(cmd, "URL") == 0) {
			DoSetURL(u, ci, param);
		} else if (stricmp(cmd, "EMAIL") == 0) {
			DoSetEMail(u, ci, param);
		} else if (stricmp(cmd, "ENTRYMSG") == 0) {
			DoSetEntryMsg(u, ci, param);
		} else if (stricmp(cmd, "TOPIC") == 0) {
			notice_lang(s_ChanServ, u, OBSOLETE_COMMAND, "TOPIC");
		} else if (stricmp(cmd, "BANTYPE") == 0) {
			DoSetBanType(u, ci, param);
		} else if (stricmp(cmd, "MLOCK") == 0) {
			DoSetMLock(u, ci, param);
		} else if (stricmp(cmd, "KEEPTOPIC") == 0) {
			DoSetKeepTopic(u, ci, param);
		} else if (stricmp(cmd, "TOPICLOCK") == 0) {
			DoSetTopicLock(u, ci, param);
		} else if (stricmp(cmd, "PRIVATE") == 0) {
			DoSetPrivate(u, ci, param);
		} else if (stricmp(cmd, "SECUREOPS") == 0) {
			DoSetSecureOps(u, ci, param);
		} else if (stricmp(cmd, "SECUREFOUNDER") == 0) {
			if (!is_servadmin
				&& (ci->
					flags & CI_SECUREFOUNDER ? !is_real_founder(u,
																ci) :
					!is_founder(u, ci))) {
				notice_lang(s_ChanServ, u, ACCESS_DENIED);
			} else {
				DoSetSecureFounder(u, ci, param);
			}
		} else if (stricmp(cmd, "RESTRICTED") == 0) {
			DoSetRestricted(u, ci, param);
		} else if (stricmp(cmd, "SECURE") == 0) {
			DoSetSecure(u, ci, param);
		} else if (stricmp(cmd, "SIGNKICK") == 0) {
			DoSetSignKick(u, ci, param);
		} else if (stricmp(cmd, "OPNOTICE") == 0) {
			DoSetOpNotice(u, ci, param);
		} else if (stricmp(cmd, "XOP") == 0) {
			if (!(findModule("cs_xop"))) {
				notice_lang(s_ChanServ, u, CHAN_XOP_NOT_AVAILABLE, cmd);
			} else {
				DoSetXOP(u, ci, param);
			}
		} else if (stricmp(cmd, "PEACE") == 0) {
			DoSetPeace(u, ci, param);
		} else if (stricmp(cmd, "NOEXPIRE") == 0) {
			DoSetNoExpire(u, ci, param);
		} else {
			notice_lang(s_ChanServ, u, CHAN_SET_UNKNOWN_OPTION, cmd);
			notice_lang(s_ChanServ, u, MORE_INFO, s_ChanServ, "SET");
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		if (subcommand.empty())
		{
			if (is_services_admin(u) || is_services_root(u))
			{
				notice_help(s_ChanServ, u, CHAN_SERVADMIN_HELP_SET);
			}
			else
			{
				notice_help(s_ChanServ, u, CHAN_HELP_SET);
			}
		}
		else if (subcommand == "FOUNDER")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_FOUNDER);
		else if (subcommand == "SUCCESSOR")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_SUCCESSOR);
		else if (subcommand == "PASSWORD")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_PASSWORD);
		else if (subcommand == "DESC")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_DESC);
		else if (subcommand == "URL")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_URL);
		else if (subcommand == "EMAIL")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_EMAIL);
		else if (subcommand == "ENTRYMSG")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_ENTRYMSG);
		else if (subcommand == "BANTYPE")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_BANTYPE);
		else if (subcommand == "PRIVATE")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_PRIVATE);
		else if (subcommand == "KEEPTOPIC")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_KEEPTOPIC);
		else if (subcommand == "TOPICLOCK")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_TOPICLOCK);
		else if (subcommand == "MLOCK")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_MLOCK);
		else if (subcommand == "RESTRICTED")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_RESTRICTED);
		else if (subcommand == "SECURE")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_SECURE);
		else if (subcommand == "SECUREOPS")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_SECUREOPS);
		else if (subcommand == "SECUREFOUNDER")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_SECUREFOUNDER);
		else if (subcommand == "SIGNKICK")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_SIGNKICK);
		else if (subcommand == "OPNOTICE")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_OPNOTICE);
		else if (subcommand == "XOP")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_XOP);
		else if (subcommand == "PEACE")
			notice_help(s_ChanServ, u, CHAN_HELP_SET_PEACE);
		else if (subcommand == "NOEXPIRE")
			notice_help(s_ChanServ, u, CHAN_SERVADMIN_HELP_SET_NOEXPIRE);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "SET", CHAN_SET_SYNTAX);
	}
};

void myChanServHelp(User * u)
{
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_SET);
}

class CSSet : public Module
{
 public:
	CSSet(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSSet(), MOD_UNIQUE);
		this->SetChanHelp(myChanServHelp);
	}
};


MODULE_INIT("cs_set", CSSet)

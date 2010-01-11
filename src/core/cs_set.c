/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
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
#include "hashcomp.h"

class CommandCSSet : public Command
{
 private:
	CommandReturn DoSetFounder(User * u, ChannelInfo * ci, const ci::string &param)
	{
		NickAlias *na = findnick(param.c_str());
		NickCore *nc, *nc0 = ci->founder;

		if (!na)
		{
			notice_lang(Config.s_ChanServ, u, NICK_X_NOT_REGISTERED, param.c_str());
			return MOD_CONT;
		}
		else if (na->HasFlag(NS_FORBIDDEN))
		{
			notice_lang(Config.s_ChanServ, u, NICK_X_FORBIDDEN, param.c_str());
			return MOD_CONT;
		}

		nc = na->nc;
		if (Config.CSMaxReg && nc->channelcount >= Config.CSMaxReg && !u->nc->HasPriv("chanserv/no-register-limit"))
		{
			notice_lang(Config.s_ChanServ, u, CHAN_SET_FOUNDER_TOO_MANY_CHANS, param.c_str());
			return MOD_CONT;
		}

		alog("%s: Changing founder of %s from %s to %s by %s!%s@%s",
			 Config.s_ChanServ, ci->name.c_str(), ci->founder->display, nc->display, u->nick.c_str(),
			 u->GetIdent().c_str(), u->host);

		/* Founder and successor must not be the same group */
		if (nc == ci->successor)
			ci->successor = NULL;

		nc0->channelcount--;
		ci->founder = nc;
		nc->channelcount++;

		notice_lang(Config.s_ChanServ, u, CHAN_FOUNDER_CHANGED, ci->name.c_str(), param.c_str());
		return MOD_CONT;
	}

	CommandReturn DoSetSuccessor(User * u, ChannelInfo * ci, const ci::string &param)
	{
		NickAlias *na;
		NickCore *nc;

		if (!param.empty())
		{
			na = findnick(param.c_str());

			if (!na)
			{
				notice_lang(Config.s_ChanServ, u, NICK_X_NOT_REGISTERED, param.c_str());
				return MOD_CONT;
			}
			if (na->HasFlag(NS_FORBIDDEN))
			{
				notice_lang(Config.s_ChanServ, u, NICK_X_FORBIDDEN, param.c_str());
				return MOD_CONT;
			}
			if (na->nc == ci->founder)
			{
				notice_lang(Config.s_ChanServ, u, CHAN_SUCCESSOR_IS_FOUNDER, param.c_str(), ci->name.c_str());
				return MOD_CONT;
			}
			nc = na->nc;

		}
		else
			nc = NULL;

		alog("%s: Changing successor of %s from %s to %s by %s!%s@%s",
			 Config.s_ChanServ, ci->name.c_str(),
			 (ci->successor ? ci->successor->display : "none"),
			 (nc ? nc->display : "none"), u->nick.c_str(), u->GetIdent().c_str(), u->host);

		ci->successor = nc;

		if (nc)
			notice_lang(Config.s_ChanServ, u, CHAN_SUCCESSOR_CHANGED, ci->name.c_str(), param.c_str());
		else
			notice_lang(Config.s_ChanServ, u, CHAN_SUCCESSOR_UNSET, ci->name.c_str());

		return MOD_CONT;
	}

	CommandReturn DoSetDesc(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (ci->desc)
			delete [] ci->desc;
		ci->desc = sstrdup(param.c_str());
		notice_lang(Config.s_ChanServ, u, CHAN_DESC_CHANGED, ci->name.c_str(), param.c_str());
		return MOD_CONT;
	}

	CommandReturn DoSetURL(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (ci->url)
			delete [] ci->url;
		if (!param.empty())
		{
			ci->url = sstrdup(param.c_str());
			notice_lang(Config.s_ChanServ, u, CHAN_URL_CHANGED, ci->name.c_str(), param.c_str());
		}
		else
		{
			ci->url = NULL;
			notice_lang(Config.s_ChanServ, u, CHAN_URL_UNSET, ci->name.c_str());
		}
		return MOD_CONT;
	}

	CommandReturn DoSetEMail(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (ci->email)
			delete [] ci->email;
		if (!param.empty())
		{
			ci->email = sstrdup(param.c_str());
			notice_lang(Config.s_ChanServ, u, CHAN_EMAIL_CHANGED, ci->name.c_str(), param.c_str());
		}
		else
		{
			ci->email = NULL;
			notice_lang(Config.s_ChanServ, u, CHAN_EMAIL_UNSET, ci->name.c_str());
		}
		return MOD_CONT;
	}

	CommandReturn DoSetEntryMsg(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (ci->entry_message)
			delete [] ci->entry_message;
		if (!param.empty())
		{
			ci->entry_message = sstrdup(param.c_str());
			notice_lang(Config.s_ChanServ, u, CHAN_ENTRY_MSG_CHANGED, ci->name.c_str(), param.c_str());
		}
		else
		{
			ci->entry_message = NULL;
			notice_lang(Config.s_ChanServ, u, CHAN_ENTRY_MSG_UNSET, ci->name.c_str());
		}
		return MOD_CONT;
	}

	CommandReturn DoSetMLock(User * u, ChannelInfo * ci, const char *modes)
	{
		int add = -1;			   /* 1 if adding, 0 if deleting, -1 if neither */
		unsigned char mode;
		ChannelMode *cm;
		ChannelModeParam *cmp;

		ci->ClearMLock();

		if (ModeManager::FindChannelModeByName(CMODE_REGISTERED))
			ci->SetMLock(CMODE_REGISTERED, true);

		ci->ClearParams();

		std::string params(modes ? modes : ""), param;
		unsigned space = params.find(' ');
		if (space != std::string::npos)
		{
			param = params.substr(space + 1);
			params = params.substr(0, space);
			modes = params.c_str();
		}
		spacesepstream modeparams(param);

		while (modes && (mode = *modes++)) {
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

			if ((cm = ModeManager::FindChannelModeByChar(mode)))
			{
				if (cm->Type == MODE_STATUS || cm->Type == MODE_LIST || !cm->CanSet(u))
				{
					notice_lang(Config.s_ChanServ, u, CHAN_SET_MLOCK_IMPOSSIBLE_CHAR, mode);
				}
				else if (add)
				{
					ci->RemoveMLock(cm->Name);

					if (cm->Type == MODE_PARAM)
					{
						cmp = dynamic_cast<ChannelModeParam *>(cm);

						if (!modeparams.GetToken(param))
							continue;

						if (!cmp->IsValid(param.c_str()))
							continue;

						ci->SetMLock(cmp->Name, true, param);
					}
					else
					{
						ci->SetMLock(cm->Name, true);
					}
				}
				else
				{
					ci->SetMLock(cm->Name, false);
				}
			}
			else
				notice_lang(Config.s_ChanServ, u, CHAN_SET_MLOCK_UNKNOWN_CHAR, mode);
		}						   /* while (*modes) */

		if (ModeManager::FindChannelModeByName(CMODE_REDIRECT)) {
			/* We can't mlock +L if +l is not mlocked as well. */
			if (ci->HasMLock(CMODE_REDIRECT, true) && !ci->HasMLock(CMODE_LIMIT, true))
			{
				ci->RemoveMLock(CMODE_REDIRECT);
				notice_lang(Config.s_ChanServ, u, CHAN_SET_MLOCK_L_REQUIRED);
			}
		}

		/* Some ircd we can't set NOKNOCK without INVITE */
		/* So check if we need there is a NOKNOCK MODE and that we need INVITEONLY */
		if (ModeManager::FindChannelModeByName(CMODE_NOKNOCK) && ircd->knock_needs_i) {
			if (ci->HasMLock(CMODE_NOKNOCK, true) && !ci->HasMLock(CMODE_INVITE, true))
			{
				ci->RemoveMLock(CMODE_NOKNOCK);
				notice_lang(Config.s_ChanServ, u, CHAN_SET_MLOCK_K_REQUIRED);
			}
		}

		/* Since we always enforce mode r there is no way to have no
		 * mode lock at all.
		 */
		if (get_mlock_modes(ci, 0)) {
			notice_lang(Config.s_ChanServ, u, CHAN_MLOCK_CHANGED, ci->name.c_str(),
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
			notice_lang(Config.s_ChanServ, u, CHAN_SET_BANTYPE_INVALID, param);
		} else {
			ci->bantype = bantype;
			notice_lang(Config.s_ChanServ, u, CHAN_SET_BANTYPE_CHANGED, ci->name.c_str(),
						ci->bantype);
		}
		return MOD_CONT;
	}

	CommandReturn DoSetKeepTopic(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (param == "ON")
		{
			ci->SetFlag(CI_KEEPTOPIC);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_KEEPTOPIC_ON, ci->name.c_str());
		}
		else if (param == "OFF")
		{
			ci->UnsetFlag(CI_KEEPTOPIC);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_KEEPTOPIC_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "KEEPTOPIC");

		return MOD_CONT;
	}

	CommandReturn DoSetTopicLock(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (param == "ON")
		{
			ci->SetFlag(CI_TOPICLOCK);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_TOPICLOCK_ON, ci->name.c_str());
		}
		else if (param == "OFF")
		{
			ci->UnsetFlag(CI_TOPICLOCK);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_TOPICLOCK_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "TOPICLOCK");

		return MOD_CONT;
	}

	CommandReturn DoSetPrivate(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (param == "ON")
		{
			ci->SetFlag(CI_PRIVATE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_PRIVATE_ON, ci->name.c_str());
		}
		else if (param == "OFF")
		{
			ci->UnsetFlag(CI_PRIVATE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_PRIVATE_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "PRIVATE");

		return MOD_CONT;
	}

	CommandReturn DoSetSecureOps(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (param == "ON")
		{
			ci->SetFlag(CI_SECUREOPS);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SECUREOPS_ON, ci->name.c_str());
		}
		else if (param == "OFF")
		{
			ci->UnsetFlag(CI_SECUREOPS);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SECUREOPS_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "SECUREOPS");

		return MOD_CONT;
	}

	CommandReturn DoSetSecureFounder(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (param == "ON")
		{
			ci->SetFlag(CI_SECUREFOUNDER);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SECUREFOUNDER_ON, ci->name.c_str());
		}
		else if (param == "OFF")
		{
			ci->UnsetFlag(CI_SECUREFOUNDER);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SECUREFOUNDER_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "SECUREFOUNDER");

		return MOD_CONT;
	}

	CommandReturn DoSetRestricted(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (param == "ON")
		{
			ci->SetFlag(CI_RESTRICTED);
			if (ci->levels[CA_NOJOIN] < 0)
				ci->levels[CA_NOJOIN] = 0;
			notice_lang(Config.s_ChanServ, u, CHAN_SET_RESTRICTED_ON, ci->name.c_str());
		}
		else if (param == "OFF")
		{
			ci->UnsetFlag(CI_RESTRICTED);
			if (ci->levels[CA_NOJOIN] >= 0)
				ci->levels[CA_NOJOIN] = -2;
			notice_lang(Config.s_ChanServ, u, CHAN_SET_RESTRICTED_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "RESTRICTED");

		return MOD_CONT;
	}

	CommandReturn DoSetSecure(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (param == "ON")
		{
			ci->SetFlag(CI_SECURE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SECURE_ON, ci->name.c_str());
		}
		else if (param == "OFF")
		{
			ci->UnsetFlag(CI_SECURE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SECURE_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "SECURE");

		return MOD_CONT;
	}

	CommandReturn DoSetSignKick(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (param == "ON")
		{
			ci->SetFlag(CI_SIGNKICK);
			ci->UnsetFlag(CI_SIGNKICK_LEVEL);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SIGNKICK_ON, ci->name.c_str());
		}
		else if (param == "LEVEL")
		{
			ci->SetFlag(CI_SIGNKICK_LEVEL);
			ci->UnsetFlag(CI_SIGNKICK);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SIGNKICK_LEVEL, ci->name.c_str());
		}
		else if (param == "OFF")
		{
			ci->UnsetFlag(CI_SIGNKICK);
			ci->UnsetFlag(CI_SIGNKICK_LEVEL);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_SIGNKICK_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "SIGNKICK");

		return MOD_CONT;
	}

	CommandReturn DoSetOpNotice(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (param == "ON")
		{
			ci->SetFlag(CI_OPNOTICE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_OPNOTICE_ON, ci->name.c_str());
		}
		else if (param == "OFF")
		{
			ci->UnsetFlag(CI_OPNOTICE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_OPNOTICE_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "OPNOTICE");

		return MOD_CONT;
	}

#define CHECKLEV(lev) ((ci->levels[(lev)] != ACCESS_INVALID) && (access->level >= ci->levels[(lev)]))

	CommandReturn DoSetXOP(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (param == "ON")
		{
			if (!(ci->HasFlag(CI_XOP)))
			{
				ChanAccess *access;

				for (unsigned i = 0; i < ci->GetAccessCount(); i++)
				{
					access = ci->GetAccess(i);
					if (!access->in_use)
						continue;
					/* This will probably cause wrong levels to be set, but hey,
					 * it's better than losing it altogether.
					 */
					if (access->level == ACCESS_QOP)
						access->level = ACCESS_QOP;
					else if (CHECKLEV(CA_AKICK) || CHECKLEV(CA_SET))
						access->level = ACCESS_SOP;
					else if (CHECKLEV(CA_AUTOOP) || CHECKLEV(CA_OPDEOP) || CHECKLEV(CA_OPDEOPME))
						access->level = ACCESS_AOP;
					else if (ModeManager::FindChannelModeByName(CMODE_HALFOP) && (CHECKLEV(CA_AUTOHALFOP) || CHECKLEV(CA_HALFOP) || CHECKLEV(CA_HALFOPME)))
							access->level = ACCESS_HOP;
					else if (CHECKLEV(CA_AUTOVOICE) || CHECKLEV(CA_VOICE) || CHECKLEV(CA_VOICEME))
						access->level = ACCESS_VOP;
					else
					{
						ci->EraseAccess(i);
					}
				}

				/* The above may have set an access entry to not be in use, this will clean that up. */
				ci->CleanAccess();

				reset_levels(ci);
				ci->SetFlag(CI_XOP);
			}

			alog("%s: %s!%s@%s enabled XOP for %s", Config.s_ChanServ, u->nick.c_str(),
				 u->GetIdent().c_str(), u->host, ci->name.c_str());
			notice_lang(Config.s_ChanServ, u, CHAN_SET_XOP_ON, ci->name.c_str());
		}
		else if (param == "OFF")
		{
			ci->UnsetFlag(CI_XOP);

			alog("%s: %s!%s@%s disabled XOP for %s", Config.s_ChanServ, u->nick.c_str(),
				 u->GetIdent().c_str(), u->host, ci->name.c_str());
			notice_lang(Config.s_ChanServ, u, CHAN_SET_XOP_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "XOP");

		return MOD_CONT;
	}

#undef CHECKLEV

	CommandReturn DoSetPeace(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (param == "ON")
		{
			ci->SetFlag(CI_PEACE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_PEACE_ON, ci->name.c_str());
		}
		else if (param == "OFF")
		{
			ci->UnsetFlag(CI_PEACE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_PEACE_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "PEACE");

		return MOD_CONT;
	}

	CommandReturn DoSetPersist(User *u, ChannelInfo *ci, const ci::string &param)
	{
		ChannelMode *cm = ModeManager::FindChannelModeByName(CMODE_PERM);

		if (param == "ON")
		{
			if (!ci->HasFlag(CI_PERSIST))
			{
				ci->SetFlag(CI_PERSIST);

				/* Channel doesn't exist, create it internally */
				if (!ci->c)
				{
					new Channel(ci->name);
					if (ci->bi)
						bot_join(ci);
				}

				/* No botserv bot, no channel mode */
				if (!ci->bi && !cm)
				{
					/* Give them ChanServ
					 * Yes, this works fine with no Config.s_BotServ
					 */
					findbot(Config.s_ChanServ)->Assign(NULL, ci);
				}

				/* Set the perm mode */
				if (cm && ci->c && !ci->c->HasMode(CMODE_PERM))
				{
					ci->c->SetMode(NULL, cm);
				}
			}

			notice_lang(Config.s_ChanServ, u, CHAN_SET_PERSIST_ON, ci->name.c_str());
		}
		else if (param == "OFF")
		{
			if (ci->HasFlag(CI_PERSIST))
			{
				ci->UnsetFlag(CI_PERSIST);

				/* Unset perm mode */
				if (cm && ci->c && ci->c->HasMode(CMODE_PERM))
					ci->c->RemoveMode(NULL, cm);
				if (Config.s_BotServ && ci->bi && ci->c->usercount == Config.BSMinUsers - 1)
					ircdproto->SendPart(ci->bi, ci->c, NULL);

				/* No channel mode, no BotServ, but using ChanServ as the botserv bot
				 * which was assigned when persist was set on
				 */
				if (!cm && !Config.s_BotServ && ci->bi)
				{
					/* Unassign bot */
					findbot(Config.s_ChanServ)->UnAssign(NULL, ci);	
				}

				if (ci->c && !ci->c->users)
					delete ci->c;
			}

			notice_lang(Config.s_ChanServ, u, CHAN_SET_PERSIST_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "PERSIST");

		return MOD_CONT;
	}

	CommandReturn DoSetNoExpire(User * u, ChannelInfo * ci, const ci::string &param)
	{
		if (!u->nc->HasCommand("chanserv/set/noexpire"))
		{
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
			return MOD_CONT;
		}
		if (param == "ON")
		{
			ci->SetFlag(CI_NO_EXPIRE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_NOEXPIRE_ON, ci->name.c_str());
		}
		else if (param == "OFF")
		{
			ci->UnsetFlag(CI_NO_EXPIRE);
			notice_lang(Config.s_ChanServ, u, CHAN_SET_NOEXPIRE_OFF, ci->name.c_str());
		}
		else
			this->OnSyntaxError(u, "NOEXPIRE");

		return MOD_CONT;
	}

 public:
	CommandCSSet() : Command("SET", 2, 3)
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		ci::string cmd = params[1];
		ci::string param = params.size() > 2 ? params[2] : "";
		ChannelInfo *ci = cs_findchan(chan);
		bool is_servadmin = u->nc->HasPriv("chanserv/set");

		if (readonly) {
			notice_lang(Config.s_ChanServ, u, CHAN_SET_DISABLED);
			return MOD_CONT;
		}

		if (param.empty() && cmd != "SUCCESSOR" && cmd != "URL" && cmd != "EMAIL" && cmd != "ENTRYMSG" && cmd != "MLOCK")
			this->OnSyntaxError(u, cmd);
		else if (!is_servadmin && !check_access(u, ci, CA_SET))
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
		else if (cmd == "FOUNDER")
		{
			if (!is_servadmin && (ci->HasFlag(CI_SECUREFOUNDER) ? !IsRealFounder(u, ci) : !IsFounder(u, ci)))
				notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
			else
				DoSetFounder(u, ci, param);
		}
		else if (cmd == "SUCCESSOR")
		{
			if (!is_servadmin && (ci->HasFlag(CI_SECUREFOUNDER) ? !IsRealFounder(u, ci) : !IsFounder(u, ci)))
				notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
			else
				DoSetSuccessor(u, ci, param);
		}
		else if (cmd == "DESC")
			DoSetDesc(u, ci, param);
		else if (cmd == "URL")
			DoSetURL(u, ci, param);
		else if (cmd == "EMAIL")
			DoSetEMail(u, ci, param);
		else if (cmd == "ENTRYMSG")
			DoSetEntryMsg(u, ci, param);
		else if (cmd == "TOPIC")
			notice_lang(Config.s_ChanServ, u, OBSOLETE_COMMAND, "TOPIC");
		else if (cmd == "BANTYPE")
			DoSetBanType(u, ci, param.c_str());
		else if (cmd == "MLOCK")
			DoSetMLock(u, ci, param.c_str());
		else if (cmd == "KEEPTOPIC")
			DoSetKeepTopic(u, ci, param);
		else if (cmd == "TOPICLOCK")
			DoSetTopicLock(u, ci, param);
		else if (cmd == "PRIVATE")
			DoSetPrivate(u, ci, param);
		else if (cmd == "SECUREOPS")
			DoSetSecureOps(u, ci, param);
		else if (cmd == "SECUREFOUNDER")
		{
			if (!is_servadmin && (ci->HasFlag(CI_SECUREFOUNDER) ? !IsRealFounder(u, ci) : !IsFounder(u, ci)))
				notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
			else
				DoSetSecureFounder(u, ci, param);
		}
		else if (cmd == "RESTRICTED")
			DoSetRestricted(u, ci, param);
		else if (cmd == "SECURE")
			DoSetSecure(u, ci, param);
		else if (cmd == "SIGNKICK")
			DoSetSignKick(u, ci, param);
		else if (cmd == "OPNOTICE")
			DoSetOpNotice(u, ci, param);
		else if (cmd == "XOP")
		{
			if (!findModule("cs_xop"))
				notice_lang(Config.s_ChanServ, u, CHAN_XOP_NOT_AVAILABLE, cmd.c_str());
			else
				DoSetXOP(u, ci, param);
		}
		else if (cmd == "PEACE")
			DoSetPeace(u, ci, param);
		else if (cmd == "PERSIST")
			DoSetPersist(u, ci, param);
		else if (cmd == "NOEXPIRE")
			DoSetNoExpire(u, ci, param);
		else
		{
			notice_lang(Config.s_ChanServ, u, CHAN_SET_UNKNOWN_OPTION, cmd.c_str());
			notice_lang(Config.s_ChanServ, u, MORE_INFO, Config.s_ChanServ, "SET");
		}

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		if (subcommand.empty())
		{
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET);
			if (u->nc && u->nc->IsServicesOper())
				notice_help(Config.s_ChanServ, u, CHAN_SERVADMIN_HELP_SET);
		}
		else if (subcommand == "FOUNDER")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_FOUNDER);
		else if (subcommand == "SUCCESSOR")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_SUCCESSOR);
		else if (subcommand == "DESC")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_DESC);
		else if (subcommand == "URL")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_URL);
		else if (subcommand == "EMAIL")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_EMAIL);
		else if (subcommand == "ENTRYMSG")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_ENTRYMSG);
		else if (subcommand == "BANTYPE")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_BANTYPE);
		else if (subcommand == "PRIVATE")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_PRIVATE);
		else if (subcommand == "KEEPTOPIC")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_KEEPTOPIC);
		else if (subcommand == "TOPICLOCK")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_TOPICLOCK);
		else if (subcommand == "MLOCK")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_MLOCK);
		else if (subcommand == "RESTRICTED")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_RESTRICTED);
		else if (subcommand == "SECURE")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_SECURE, Config.s_NickServ);
		else if (subcommand == "SECUREOPS")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_SECUREOPS);
		else if (subcommand == "SECUREFOUNDER")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_SECUREFOUNDER);
		else if (subcommand == "SIGNKICK")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_SIGNKICK);
		else if (subcommand == "OPNOTICE")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_OPNOTICE);
		else if (subcommand == "XOP")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_XOP);
		else if (subcommand == "PEACE")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_PEACE);
		else if (subcommand == "PERSIST")
			notice_help(Config.s_ChanServ, u, CHAN_HELP_SET_PERSIST);
		else if (subcommand == "NOEXPIRE")
			notice_help(Config.s_ChanServ, u, CHAN_SERVADMIN_HELP_SET_NOEXPIRE);
		else
			return false;

		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		int reply = CHAN_SET_SYNTAX;
		ci::string command = "SET";

		if (!subcommand.empty())
		{
			if (subcommand == "KEEPTOPIC")
				reply = CHAN_SET_KEEPTOPIC_SYNTAX;
			else if (subcommand == "TOPICLOCK")
				reply = CHAN_SET_TOPICLOCK_SYNTAX;
			else if (subcommand == "PRIVATE")
				reply = CHAN_SET_PRIVATE_SYNTAX;
			else if (subcommand == "SECUREOPS")
				reply = CHAN_SET_SECUREOPS_SYNTAX;
			else if (subcommand == "SECUREFOUNDER")
				reply = CHAN_SET_SECUREFOUNDER_SYNTAX;
			else if (subcommand == "RESTRICTED")
				reply = CHAN_SET_RESTRICTED_SYNTAX;
			else if (subcommand == "SECURE")
				reply = CHAN_SET_SECURE_SYNTAX;
			else if (subcommand == "SIGNKICK")
				reply = CHAN_SET_SIGNKICK_SYNTAX;
			else if (subcommand == "OPNOTICE")
				reply = CHAN_SET_OPNOTICE_SYNTAX;
			else if (subcommand == "XOP")
				reply = CHAN_SET_XOP_SYNTAX;
			else if (subcommand == "PEACE")
				reply = CHAN_SET_PEACE_SYNTAX;
			else if (subcommand == "PERSIST")
				reply = CHAN_SET_PERSIST_SYNTAX;
			else if (subcommand == "NOEXPIRE")
				reply = CHAN_SET_NOEXPIRE_SYNTAX;

			if (reply != CHAN_SET_SYNTAX)
				command += " " + subcommand;
		}

		syntax_error(Config.s_ChanServ, u, command.c_str(), reply);
	}
};

class CSSet : public Module
{
 public:
	CSSet(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSSet());

		ModuleManager::Attach(I_OnChanServHelp, this);
	}
	void OnChanServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_SET);
	}
};

MODULE_INIT(CSSet)

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

class CommandCSClear : public Command
{
 public:
	CommandCSClear() : Command("CLEAR", 2, 2)
	{

	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *chan = params[0].c_str();
		ci::string what = params[1];
		Channel *c = findchan(chan);
		ChannelInfo *ci;
		ChannelMode *owner, *admin;
		std::string modebuf;

		owner = ModeManager::FindChannelModeByName(CMODE_OWNER);
		admin = ModeManager::FindChannelModeByName(CMODE_PROTECT);

		if (c)
			ci = c->ci;

		if (!c) {
			notice_lang(Config.s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
		} else if (!u || !check_access(u, ci, CA_CLEAR)) {
			notice_lang(Config.s_ChanServ, u, ACCESS_DENIED);
		} else if (what == "bans") {
			c->ClearBans();

			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_BANS, chan);
		} else if (ModeManager::FindChannelModeByName(CMODE_EXCEPT) && what == "excepts") {
			c->ClearExcepts();

			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_EXCEPTS, chan);

		} else if (ModeManager::FindChannelModeByName(CMODE_INVITE) && what == "invites") {
			c->ClearInvites();

			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_INVITES, chan);

		} else if (what == "modes") {
			c->ClearModes();
			check_modes(c);

			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_MODES, chan);
		} else if (what == "ops") {
			const char *av[6];  /* The max we have to hold: chan, ts, modes(max3), nick, nick, nick */
			int isop, isadmin, isown;
			struct c_userlist *cu, *bnext;

			if (ircd->svsmode_ucmode) {
				av[0] = chan;
				ircdproto->SendSVSModeChan(av[0], "-o", NULL);
				if (owner) {
					modebuf = '-';
					modebuf += owner->ModeChar;

					ircdproto->SendSVSModeChan(av[0], modebuf.c_str(), NULL);
				}
				if (admin) {
					modebuf = '-';
					modebuf += admin->ModeChar;

					ircdproto->SendSVSModeChan(av[0], modebuf.c_str(), NULL);
				}
				for (cu = c->users; cu; cu = bnext)
				{
					bnext = cu->next;
					isop = chan_has_user_status(c, cu->user, CUS_OP);
					isadmin = chan_has_user_status(c, cu->user, CUS_PROTECT);
					isown = chan_has_user_status(c, cu->user, CUS_OWNER);

					if (!isop && !isadmin && !isown)
						continue;

					if (isown)
						c->RemoveMode(NULL, CMODE_OWNER, cu->user->nick);
					if (admin)
						c->RemoveMode(NULL, CMODE_PROTECT, cu->user->nick);
					if (isop)
						c->RemoveMode(NULL, CMODE_OP, cu->user->nick);
				}
			} else {
				av[0] = chan;
				for (cu = c->users; cu; cu = bnext)
				{
					bnext = cu->next;
					isop = chan_has_user_status(c, cu->user, CUS_OP);
					isadmin = chan_has_user_status(c, cu->user, CUS_PROTECT);
					isown = chan_has_user_status(c, cu->user, CUS_OWNER);

					if (!isop && !isadmin && !isown)
						continue;

					if (isown)
						c->RemoveMode(NULL, CMODE_OWNER, cu->user->nick);
					if (isadmin)
						c->RemoveMode(NULL, CMODE_PROTECT, cu->user->nick);
					if (isop)
						c->RemoveMode(NULL, CMODE_OP, cu->user->nick);
				}
			}
			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_OPS, chan);
		} else if (ModeManager::FindChannelModeByName(CMODE_HALFOP) && what == "hops") {
			struct c_userlist *cu, *bnext;

			for (cu = c->users; cu; cu = bnext)
			{
				bnext = cu->next;

				if (!chan_has_user_status(c, cu->user, CUS_HALFOP))
					continue;

				c->RemoveMode(NULL, CMODE_HALFOP, cu->user->nick);
			}

			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_HOPS, chan);
		} else if (what == "voices") {
			struct c_userlist *cu, *bnext;

			for (cu = c->users; cu; cu = bnext)
			{
				bnext = cu->next;

				if (!chan_has_user_status(c, cu->user, CUS_VOICE))
					continue;

				c->RemoveMode(NULL, CMODE_VOICE, cu->user->nick);
			}

			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_VOICES, chan);
		} else if (what == "users") {
			const char *av[3];
			struct c_userlist *cu, *bnext;
			char buf[256];

			snprintf(buf, sizeof(buf), "CLEAR USERS command from %s", u->nick);

			for (cu = c->users; cu; cu = bnext) {
				bnext = cu->next;
				av[0] = sstrdup(chan);
				av[1] = sstrdup(cu->user->nick);
				av[2] = sstrdup(buf);
				ircdproto->SendKick(whosends(ci), av[0], av[1], av[2]);
				do_kick(Config.s_ChanServ, 3, av);
				delete [] av[2];
				delete [] av[1];
				delete [] av[0];
			}
			notice_lang(Config.s_ChanServ, u, CHAN_CLEARED_USERS, chan);
		} else {
			syntax_error(Config.s_ChanServ, u, "CLEAR", CHAN_CLEAR_SYNTAX);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_ChanServ, u, CHAN_HELP_CLEAR);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_ChanServ, u, "CLEAR", CHAN_CLEAR_SYNTAX);
	}
};

class CSClear : public Module
{
 public:
	CSClear(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSClear());

		ModuleManager::Attach(I_OnChanServHelp, this);
	}
	void OnChanServHelp(User *u)
	{
		notice_lang(Config.s_ChanServ, u, CHAN_HELP_CMD_CLEAR);
	}
};

MODULE_INIT(CSClear)

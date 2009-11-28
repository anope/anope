/* OperServ core functions
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

class CommandOSClearModes : public Command
{
 public:
	CommandOSClearModes() : Command("CLEARMODES", 1, 2, "operserv/clearmodes")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		const char *argv[2];
		const char *chan = params[0].c_str();
		Channel *c;
		int all = 0;
		struct c_userlist *cu, *next;
		ChannelMode *cm;
		std::string buf;

		if (!(c = findchan(chan)))
		{
			notice_lang(Config.s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
			return MOD_CONT;
		}
		else if (c->bouncy_modes)
		{
			notice_lang(Config.s_OperServ, u, OPER_BOUNCY_MODES_U_LINE);
			return MOD_CONT;
		}
		else
		{
			ci::string s = params.size() > 1 ? params[1] : "";
			if (!s.empty()) {
				if (s == "ALL")
					all = 1;
				else {
					this->OnSyntaxError(u, "");
					return MOD_CONT;
				}
			}

			if (Config.WallOSClearmodes)
				ircdproto->SendGlobops(Config.s_OperServ, "%s used CLEARMODES%s on %s", u->nick, all ? " ALL" : "", chan);
			if (all)
			{
				/* Clear mode +o */
				if (ircd->svsmode_ucmode)
					ircdproto->SendSVSModeChan(c->name, "-o", NULL);
				for (cu = c->users; cu; cu = next)
				{
					next = cu->next;
					if (!chan_has_user_status(c, cu->user, CUS_OP))
						continue;
					argv[0] = "-o";
					argv[1] = cu->user->nick;
					if (!ircd->svsmode_ucmode)
						ircdproto->SendMode(findbot(Config.s_OperServ), c->name, "-o %s", cu->user->nick);
					chan_set_modes(Config.s_OperServ, c, 2, argv, 0);
				}

				/* Clear mode +v */
				if (ircd->svsmode_ucmode)
					ircdproto->SendSVSModeChan(c->name, "-v", NULL);
				for (cu = c->users; cu; cu = next)
				{
					next = cu->next;
					if (!chan_has_user_status(c, cu->user, CUS_VOICE))
						continue;
					argv[0] = "-v";
					argv[1] = cu->user->nick;
					if (!ircd->svsmode_ucmode)
						ircdproto->SendMode(findbot(Config.s_OperServ), c->name, "-v %s", cu->user->nick);
					chan_set_modes(Config.s_OperServ, c, 2, argv, 0);
				}

				/* Clear mode +h */
				if (ModeManager::FindChannelModeByName(CMODE_HALFOP))
				{
					if (ircd->svsmode_ucmode)
						ircdproto->SendSVSModeChan(c->name, "-h", NULL);
					for (cu = c->users; cu; cu = next)
					{
						next = cu->next;
						if (!chan_has_user_status(c, cu->user, CUS_HALFOP))
							continue;
						argv[0] = "-h";
						argv[1] = cu->user->nick;
						if (!ircd->svsmode_ucmode)
							ircdproto->SendMode(findbot(Config.s_OperServ), c->name, "-h %s", cu->user->nick);
						chan_set_modes(Config.s_OperServ, c, 2, argv, 0);
					}
				}

				/* Clear mode Owners */
				if ((cm = ModeManager::FindChannelModeByName(CMODE_OWNER)))
				{
					buf = '-';
					buf += cm->ModeChar;

					if (ircd->svsmode_ucmode)
						ircdproto->SendSVSModeChan(c->name, buf.c_str(), NULL);
					for (cu = c->users; cu; cu = next)
					{
						next = cu->next;
						if (!chan_has_user_status(c, cu->user, CUS_OWNER))
							continue;
						argv[0] = buf.c_str();
						argv[1] = cu->user->nick;
						if (!ircd->svsmode_ucmode)
							ircdproto->SendMode(findbot(Config.s_OperServ), c->name, "%s %s", buf.c_str(), cu->user->nick);
						chan_set_modes(Config.s_OperServ, c, 2, argv, 0);
					}
				}

				/* Clear mode protected or admins */
				if ((cm = ModeManager::FindChannelModeByName(CMODE_PROTECT)))
				{
					buf = '-';
					buf += cm->ModeChar;

					if (ircd->svsmode_ucmode)
						ircdproto->SendSVSModeChan(c->name, buf.c_str(), NULL);
					for (cu = c->users; cu; cu = next)
					{
						next = cu->next;
						if (!chan_has_user_status(c, cu->user, CUS_PROTECT))
							continue;
						argv[0] = buf.c_str();
						argv[1] = cu->user->nick;
						if (!ircd->svsmode_ucmode)
							ircdproto->SendMode(findbot(Config.s_OperServ), c->name, "%s %s", buf.c_str(), cu->user->nick);
						chan_set_modes(Config.s_OperServ, c, 2, argv, 0);
					}
				}
			}


			c->ClearModes(Config.s_OperServ);

			c->ClearBans(Config.s_OperServ);
			c->ClearExcepts(Config.s_OperServ);
			c->ClearInvites(Config.s_OperServ);
		}

		if (all)
			notice_lang(Config.s_OperServ, u, OPER_CLEARMODES_ALL_DONE, chan);
		else
			notice_lang(Config.s_OperServ, u, OPER_CLEARMODES_DONE, chan);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_CLEARMODES);
		return true;
	}

	void OnSyntaxError(User *u, const ci::string &subcommand)
	{
		syntax_error(Config.s_OperServ, u, "CLEARMODES", OPER_CLEARMODES_SYNTAX);
	}
};

class OSClearModes : public Module
{
 public:
	OSClearModes(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSClearModes());

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_CLEARMODES);
	}
};

MODULE_INIT(OSClearModes)

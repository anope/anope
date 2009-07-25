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

	CommandReturn Execute(User *u, std::vector<ci::string> &params)
	{
		const char *argv[2];
		const char *chan = params[0].c_str();
		Channel *c;
		int all = 0;
		struct c_userlist *cu, *next;
		Entry *entry, *nexte;

		if (!(c = findchan(chan)))
		{
			notice_lang(s_OperServ, u, CHAN_X_NOT_IN_USE, chan);
			return MOD_CONT;
		}
		else if (c->bouncy_modes)
		{
			notice_lang(s_OperServ, u, OPER_BOUNCY_MODES_U_LINE);
			return MOD_CONT;
		}
		else
		{
			ci::string s = params.size() > 1 ? params[1] : "";
			if (!s.empty()) {
				if (s == "ALL")
					all = 1;
				else {
					this->OnSyntaxError(u);
					return MOD_CONT;
				}
			}

			if (WallOSClearmodes)
				ircdproto->SendGlobops(s_OperServ, "%s used CLEARMODES%s on %s", u->nick, all ? " ALL" : "", chan);
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
						ircdproto->SendMode(findbot(s_OperServ), c->name, "-o %s", cu->user->nick);
					chan_set_modes(s_OperServ, c, 2, argv, 0);
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
						ircdproto->SendMode(findbot(s_OperServ), c->name, "-v %s", cu->user->nick);
					chan_set_modes(s_OperServ, c, 2, argv, 0);
				}

				/* Clear mode +h */
				if (ircd->halfop)
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
							ircdproto->SendMode(findbot(s_OperServ), c->name, "-h %s", cu->user->nick);
						chan_set_modes(s_OperServ, c, 2, argv, 0);
					}
				}

				/* Clear mode Owners */
				if (ircd->owner)
				{
					if (ircd->svsmode_ucmode)
						ircdproto->SendSVSModeChan(c->name, ircd->ownerunset, NULL);
					for (cu = c->users; cu; cu = next)
					{
						next = cu->next;
						if (!chan_has_user_status(c, cu->user, CUS_OWNER))
							continue;
						argv[0] = ircd->ownerunset;
						argv[1] = cu->user->nick;
						if (!ircd->svsmode_ucmode)
							ircdproto->SendMode(findbot(s_OperServ), c->name, "%s %s", ircd->ownerunset, cu->user->nick);
						chan_set_modes(s_OperServ, c, 2, argv, 0);
					}
				}

				/* Clear mode protected or admins */
				if (ircd->protect || ircd->admin)
				{
					if (ircd->svsmode_ucmode)
						ircdproto->SendSVSModeChan(c->name, ircd->adminunset, NULL);
					for (cu = c->users; cu; cu = next)
					{
						next = cu->next;
						if (!chan_has_user_status(c, cu->user, CUS_PROTECT))
							continue;
						argv[0] = ircd->adminunset;
						argv[1] = cu->user->nick;
						if (!ircd->svsmode_ucmode)
							ircdproto->SendMode(findbot(s_OperServ), c->name, "%s %s", ircd->adminunset, cu->user->nick);
						chan_set_modes(s_OperServ, c, 2, argv, 0);
					}
				}
			}

			if (c->mode)
			{
				/* Clear modes the bulk of the modes */
				ircdproto->SendMode(findbot(s_OperServ), c->name, "%s", ircd->modestoremove);
				argv[0] = ircd->modestoremove;
				chan_set_modes(s_OperServ, c, 1, argv, 0);

				/* to prevent the internals from complaining send -k, -L, -f by themselves if we need
				   to send them - TSL */
				if (c->key)
				{
					ircdproto->SendMode(findbot(s_OperServ), c->name, "-k %s", c->key);
					argv[0] = "-k";
					argv[1] = c->key;
					chan_set_modes(s_OperServ, c, 2, argv, 0);
				}
				if (ircd->Lmode && c->redirect)
				{
					ircdproto->SendMode(findbot(s_OperServ), c->name, "-L %s", c->redirect);
					argv[0] = "-L";
					argv[1] = c->redirect;
					chan_set_modes(s_OperServ, c, 2, argv, 0);
				}
				if (ircd->fmode && c->flood)
				{
					if (flood_mode_char_remove)
					{
						ircdproto->SendMode(findbot(s_OperServ), c->name, "%s %s", flood_mode_char_remove, c->flood);
						argv[0] = flood_mode_char_remove;
						argv[1] = c->flood;
						chan_set_modes(s_OperServ, c, 2, argv, 0);
					}
					else
					{
						if (debug)
							alog("debug: flood_mode_char_remove was not set unable to remove flood/throttle modes");
					}
				}
			}

			/* Clear bans */
			if (c->bans && c->bans->count)
			{
				for (entry = c->bans->entries; entry; entry = nexte)
				{
					nexte = entry->next;
					argv[0] = "-b";
					argv[1] = entry->mask;
					ircdproto->SendMode(findbot(s_OperServ), c->name, "-b %s", entry->mask);
					chan_set_modes(s_OperServ, c, 2, argv, 0);
				}
			}

			/* Clear excepts */
			if (ircd->except && c->excepts && c->excepts->count)
			{
				for (entry = c->excepts->entries; entry; entry = nexte)
				{
					nexte = entry->next;
					argv[0] = "-e";
					argv[1] = entry->mask;
					ircdproto->SendMode(findbot(s_OperServ), c->name, "-e %s", entry->mask);
					chan_set_modes(s_OperServ, c, 2, argv, 0);
				}
			}

			/* Clear invites */
			if (ircd->invitemode && c->invites && c->invites->count)
			{
				for (entry = c->invites->entries; entry; entry = nexte)
				{
					nexte = entry->next;
					argv[0] = "-I";
					argv[1] = entry->mask;
					ircdproto->SendMode(findbot(s_OperServ), c->name, "-I %s", entry->mask);
					chan_set_modes(s_OperServ, c, 2, argv, 0);
				}
			}
		}

		if (all)
			notice_lang(s_OperServ, u, OPER_CLEARMODES_ALL_DONE, chan);
		else
			notice_lang(s_OperServ, u, OPER_CLEARMODES_DONE, chan);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_CLEARMODES);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_OperServ, u, "CLEARMODES", OPER_CLEARMODES_SYNTAX);
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

		this->AddCommand(OPERSERV, new CommandOSClearModes(), MOD_UNIQUE);
	}
	void OperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_CLEARMODES);
	}
};

MODULE_INIT("os_clearmodes", OSClearModes)

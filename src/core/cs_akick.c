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




/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User * u)
{
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_AKICK);
}

int akick_del(User * u, AutoKick * akick)
{
	if (!(akick->flags & AK_USED))
		return 0;
	if (akick->flags & AK_ISNICK) {
		akick->u.nc = NULL;
	} else {
		delete [] akick->u.mask;
		akick->u.mask = NULL;
	}
	if (akick->reason) {
		delete [] akick->reason;
		akick->reason = NULL;
	}
	if (akick->creator) {
		delete [] akick->creator;
		akick->creator = NULL;
	}
	akick->addtime = 0;
	akick->flags = 0;
	return 1;
}

int akick_del_callback(User * u, int num, va_list args)
{
	ChannelInfo *ci = va_arg(args, ChannelInfo *);
	int *last = va_arg(args, int *);

	*last = num;

	if (num < 1 || num > ci->akickcount)
		return 0;

	return akick_del(u, &ci->akick[num - 1]);
}


int akick_list(User * u, int index, ChannelInfo * ci, int *sent_header)
{
	AutoKick *akick = &ci->akick[index];

	if (!(akick->flags & AK_USED))
		return 0;
	if (!*sent_header) {
		notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_HEADER, ci->name);
		*sent_header = 1;
	}

	notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_FORMAT, index + 1,
				((akick->flags & AK_ISNICK) ? akick->u.nc->
				 display : akick->u.mask),
				(akick->reason ? akick->
				 reason : getstring(u, NO_REASON)));
	return 1;
}

int akick_list_callback(User * u, int num, va_list args)
{
	ChannelInfo *ci = va_arg(args, ChannelInfo *);
	int *sent_header = va_arg(args, int *);
	if (num < 1 || num > ci->akickcount)
		return 0;
	return akick_list(u, num - 1, ci, sent_header);
}

int akick_view(User * u, int index, ChannelInfo * ci, int *sent_header)
{
	AutoKick *akick = &ci->akick[index];
	char timebuf[64];
	struct tm tm;

	if (!(akick->flags & AK_USED))
		return 0;
	if (!*sent_header) {
		notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_HEADER, ci->name);
		*sent_header = 1;
	}

	if (akick->addtime) {
		tm = *localtime(&akick->addtime);
		strftime_lang(timebuf, sizeof(timebuf), u,
					  STRFTIME_SHORT_DATE_FORMAT, &tm);
	} else {
		snprintf(timebuf, sizeof(timebuf), "%s", getstring(u, UNKNOWN));
	}

	notice_lang(s_ChanServ, u,
				((akick->
				  flags & AK_STUCK) ? CHAN_AKICK_VIEW_FORMAT_STUCK :
				 CHAN_AKICK_VIEW_FORMAT), index + 1,
				((akick->flags & AK_ISNICK) ? akick->u.nc->
				 display : akick->u.mask),
				akick->creator ? akick->creator : getstring(u,
															UNKNOWN),
				timebuf,
				(akick->reason ? akick->
				 reason : getstring(u, NO_REASON)));
	return 1;
}

int akick_view_callback(User * u, int num, va_list args)
{
	ChannelInfo *ci = va_arg(args, ChannelInfo *);
	int *sent_header = va_arg(args, int *);
	if (num < 1 || num > ci->akickcount)
		return 0;
	return akick_view(u, num - 1, ci, sent_header);
}

int get_access_nc(NickCore *nc, ChannelInfo *ci)
{
	ChanAccess *access;
	if (!ci || !nc)
		return 0;

	if ((access = get_access_entry(nc, ci)))
		return access->level;
	return 0;
}

class CommandCSAKick : public Command
{
 public:
  CommandCSAKick() : Command("AKICK", 2, 4)
	{
	}

	CommandReturn Execute(User *u, std::vector<std::string> &params)
	{
		const char *chan = params[0].c_str();
		const char *cmd = params[1].c_str();
		const char *mask = params.size() > 2 ? params[2].c_str() : NULL;
		const char *reason = NULL;

		if (params.size() > 3)
		{
			params[3].resize(200); // XXX: is this right?
			reason = params[3].c_str();
		}

		ChannelInfo *ci;
		AutoKick *akick;
		int i;
		Channel *c;
		struct c_userlist *cu = NULL;
		struct c_userlist *unext;
		User *u2;
		const char *argv[3];
		int count = 0;

		if (!cmd || (!mask && (!stricmp(cmd, "ADD") || !stricmp(cmd, "STICK")
								 || !stricmp(cmd, "UNSTICK")
								 || !stricmp(cmd, "DEL")))) {

			syntax_error(s_ChanServ, u, "AKICK", CHAN_AKICK_SYNTAX);
		} else if (!(ci = cs_findchan(chan))) {
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, chan);
		} else if (ci-> flags & CI_FORBIDDEN) {
			notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, chan);
		} else if (!check_access(u, ci, CA_AKICK) && !is_services_admin(u)) {
			notice_lang(s_ChanServ, u, ACCESS_DENIED);
		} else if (stricmp(cmd, "ADD") == 0) {
			NickAlias *na = findnick(mask), *na2;
			NickCore *nc = NULL;
			char *nick, *user, *host;
			int freemask = 0;

			if (readonly) {
				notice_lang(s_ChanServ, u, CHAN_AKICK_DISABLED);
				return MOD_CONT;
			}

			if (!na) {
				split_usermask(mask, &nick, &user, &host);
				char *smask = new char[strlen(nick) + strlen(user) + strlen(host) + 3];
				freemask = 1;
				sprintf(smask, "%s!%s@%s", nick, user, host);
				mask = smask;
				delete [] nick;
				delete [] user;
				delete [] host;
			} else {
				if (na->status & NS_FORBIDDEN) {
					notice_lang(s_ChanServ, u, NICK_X_FORBIDDEN, mask);
					return MOD_CONT;
				}
				nc = na->nc;
			}

			/* Check excepts BEFORE we get this far */
			if (ircd->except) {
				if (is_excepted_mask(ci, mask) == 1) {
					notice_lang(s_ChanServ, u, CHAN_EXCEPTED, mask, chan);
					if (freemask)
						delete [] mask;
					return MOD_CONT;
				}
			}

			/* Check whether target nick has equal/higher access
			 * or whether the mask matches a user with higher/equal access - Viper */
			if ((ci->flags & CI_PEACE) && nc) {
				if ((nc == ci->founder) || (get_access_nc(nc, ci) >= get_access(u, ci))) {
					notice_lang(s_ChanServ, u, PERMISSION_DENIED);
					if (freemask)
						delete [] mask;
					return MOD_CONT;
				}
			} else if ((ci->flags & CI_PEACE)) {
				char buf[BUFSIZE];
				/* Match against all currently online users with equal or
				 * higher access. - Viper */
				for (i = 0; i < 1024; i++) {
					for (u2 = userlist[i]; u2; u2 = u2->next) {
						if (is_founder(u2, ci) || (get_access(u2, ci) >= get_access(u, ci))) {
							if (match_usermask(mask, u2)) {
								notice_lang(s_ChanServ, u, PERMISSION_DENIED);
								delete [] mask;
								return MOD_CONT;
							}
						}
					}
				}

				/* Match against the lastusermask of all nickalias's with equal
				 * or higher access. - Viper */
				for (i = 0; i < 1024; i++) {
					for (na2 = nalists[i]; na2; na2 = na2->next) {
						if (na2->status & NS_FORBIDDEN)
							continue;

						if (na2->nc && ((na2->nc == ci->founder) || (get_access_nc(na2->nc, ci)
								>= get_access(u, ci)))) {
							snprintf(buf, BUFSIZE, "%s!%s", na2->nick, na2->last_usermask);
							if (match_wild_nocase(mask, buf)) {
								notice_lang(s_ChanServ, u, PERMISSION_DENIED);
								delete [] mask;
								return MOD_CONT;
							}
						}
					}
				}
			}

			for (akick = ci->akick, i = 0; i < ci->akickcount; akick++, i++) {
				if (!(akick->flags & AK_USED))
					continue;
				if ((akick->flags & AK_ISNICK) ? akick->u.nc == nc
					: stricmp(akick->u.mask, mask) == 0) {
					notice_lang(s_ChanServ, u, CHAN_AKICK_ALREADY_EXISTS,
								(akick->flags & AK_ISNICK) ? akick->u.nc->
								display : akick->u.mask, chan);
					if (freemask)
						delete [] mask;
					return MOD_CONT;
				}
			}


			/* All entries should be in use so we don't have to go over
			 * the entire list. We simply add new entries at the end. */
			if (ci->akickcount >= CSAutokickMax) {
				notice_lang(s_ChanServ, u, CHAN_AKICK_REACHED_LIMIT, CSAutokickMax);
				if (freemask)
					delete [] mask;
				return MOD_CONT;
			}
			ci->akickcount++;
			ci->akick =
				static_cast<AutoKick *>(srealloc(ci->akick, sizeof(AutoKick) * ci->akickcount));
			akick = &ci->akick[i];
			akick->flags = AK_USED;
			if (nc) {
				akick->flags |= AK_ISNICK;
				akick->u.nc = nc;
			} else {
				akick->u.mask = sstrdup(mask);
			}
			akick->creator = sstrdup(u->nick);
			akick->addtime = time(NULL);
			if (reason) {
				akick->reason = sstrdup(reason);
			} else {
				akick->reason = NULL;
			}

			/* Auto ENFORCE #63 */
			c = findchan(ci->name);
			if (c) {
				cu = c->users;
				while (cu) {
					unext = cu->next;
					if (check_kick(cu->user, c->name, c->creation_time)) {
						argv[0] = sstrdup(c->name);
						argv[1] = sstrdup(cu->user->nick);
						if (akick->reason)
							argv[2] = sstrdup(akick->reason);
						else
							argv[2] = sstrdup("none");

						do_kick(s_ChanServ, 3, argv);

						delete [] argv[2];
						delete [] argv[1];
						delete [] argv[0];
						count++;

					}
					cu = unext;
				}
			}
			notice_lang(s_ChanServ, u, CHAN_AKICK_ADDED, mask, chan);

			if (count)
				notice_lang(s_ChanServ, u, CHAN_AKICK_ENFORCE_DONE, chan,
							count);

			if (freemask)
				delete [] mask;

		} else if (stricmp(cmd, "STICK") == 0) {
			NickAlias *na;
			NickCore *nc;

			if (readonly) {
				notice_lang(s_ChanServ, u, CHAN_AKICK_DISABLED);
				return MOD_CONT;
			}

			if (ci->akickcount == 0) {
				notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, ci->name);
				return MOD_CONT;
			}

			na = findnick(mask);
			nc = (na ? na->nc : NULL);

			for (akick = ci->akick, i = 0; i < ci->akickcount; akick++, i++) {
				if (!(akick->flags & AK_USED) || (akick->flags & AK_ISNICK))
					continue;
				if (!stricmp(akick->u.mask, mask))
					break;
			}

			if (i == ci->akickcount) {
				notice_lang(s_ChanServ, u, CHAN_AKICK_NOT_FOUND, mask,
							ci->name);
				return MOD_CONT;
			}

			akick->flags |= AK_STUCK;
			notice_lang(s_ChanServ, u, CHAN_AKICK_STUCK, akick->u.mask,
						ci->name);

			if (ci->c)
				stick_mask(ci, akick);
		} else if (stricmp(cmd, "UNSTICK") == 0) {
			NickAlias *na;
			NickCore *nc;

			if (readonly) {
				notice_lang(s_ChanServ, u, CHAN_AKICK_DISABLED);
				return MOD_CONT;
			}

			if (ci->akickcount == 0) {
				notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, ci->name);
				return MOD_CONT;
			}

			na = findnick(mask);
			nc = (na ? na->nc : NULL);

			for (akick = ci->akick, i = 0; i < ci->akickcount; akick++, i++) {
				if (!(akick->flags & AK_USED) || (akick->flags & AK_ISNICK))
					continue;
				if (!stricmp(akick->u.mask, mask))
					break;
			}

			if (i == ci->akickcount) {
				notice_lang(s_ChanServ, u, CHAN_AKICK_NOT_FOUND, mask,
							ci->name);
				return MOD_CONT;
			}

			akick->flags &= ~AK_STUCK;
			notice_lang(s_ChanServ, u, CHAN_AKICK_UNSTUCK, akick->u.mask,
						ci->name);

		} else if (stricmp(cmd, "DEL") == 0) {
			int deleted, a, b;

			if (readonly) {
				notice_lang(s_ChanServ, u, CHAN_AKICK_DISABLED);
				return MOD_CONT;
			}

			if (ci->akickcount == 0) {
				notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, chan);
				return MOD_CONT;
			}

			/* Special case: is it a number/list?  Only do search if it isn't. */
			if (isdigit(*mask) && strspn(mask, "1234567890,-") == strlen(mask)) {
				int last = -1;
				deleted = process_numlist(mask, &count, akick_del_callback, u,
											ci, &last);
				if (!deleted) {
					if (count == 1) {
						notice_lang(s_ChanServ, u, CHAN_AKICK_NO_SUCH_ENTRY,
									last, ci->name);
					} else {
						notice_lang(s_ChanServ, u, CHAN_AKICK_NO_MATCH,
									ci->name);
					}
				} else if (deleted == 1) {
					notice_lang(s_ChanServ, u, CHAN_AKICK_DELETED_ONE,
								ci->name);
				} else {
					notice_lang(s_ChanServ, u, CHAN_AKICK_DELETED_SEVERAL,
								deleted, ci->name);
				}
			} else {
				NickAlias *na = findnick(mask);
				NickCore *nc = (na ? na->nc : NULL);

				for (akick = ci->akick, i = 0; i < ci->akickcount;
					 akick++, i++) {
					if (!(akick->flags & AK_USED))
						continue;
					if (((akick->flags & AK_ISNICK) && akick->u.nc == nc)
						|| (!(akick->flags & AK_ISNICK)
							&& stricmp(akick->u.mask, mask) == 0))
						break;
				}
				if (i == ci->akickcount) {
					notice_lang(s_ChanServ, u, CHAN_AKICK_NOT_FOUND, mask,
								chan);
					return MOD_CONT;
				}
				notice_lang(s_ChanServ, u, CHAN_AKICK_DELETED, mask, chan);
				akick_del(u, akick);
				deleted = 1;
			}
			if (deleted) {
				/* Reordering - DrStein */
				for (b = 0; b < ci->akickcount; b++) {
					if (ci->akick[b].flags & AK_USED) {
						for (a = 0; a < ci->akickcount; a++) {
							if (a > b)
								break;
							if (!(ci->akick[a].flags & AK_USED)) {
								ci->akick[a].flags = ci->akick[b].flags;
								if (ci->akick[b].flags & AK_ISNICK) {
									ci->akick[a].u.nc = ci->akick[b].u.nc;
								} else {
									ci->akick[a].u.mask =
										sstrdup(ci->akick[b].u.mask);
								}
								/* maybe we should first check whether there
									 is a reason before we sstdrup it -Certus */
								if (ci->akick[b].reason)
									ci->akick[a].reason =
										sstrdup(ci->akick[b].reason);
								else
									ci->akick[a].reason = NULL;
								ci->akick[a].creator =
									sstrdup(ci->akick[b].creator);
								ci->akick[a].addtime = ci->akick[b].addtime;

								akick_del(u, &ci->akick[b]);
								break;
							}
						}
					}
				}
				/* After reordering only the entries at the end could still be empty.
				 * We ll free the places no longer in use... - Viper */
				for (i = ci->akickcount - 1; i >= 0; i--) {
					if (ci->akick[i].flags & AK_USED)
						break;

					ci->akickcount--;
				}
				ci->akick =
					static_cast<AutoKick *>(srealloc(ci->akick,sizeof(AutoKick) * ci->akickcount));
			}
		} else if (stricmp(cmd, "LIST") == 0) {
			int sent_header = 0;

			if (ci->akickcount == 0) {
				notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, chan);
				return MOD_CONT;
			}
			if (mask && isdigit(*mask) &&
				strspn(mask, "1234567890,-") == strlen(mask)) {
				process_numlist(mask, NULL, akick_list_callback, u, ci,
								&sent_header);
			} else {
				for (akick = ci->akick, i = 0; i < ci->akickcount;
					 akick++, i++) {
					if (!(akick->flags & AK_USED))
						continue;
					if (mask) {
						if (!(akick->flags & AK_ISNICK)
							&& !match_wild_nocase(mask, akick->u.mask))
							continue;
						if ((akick->flags & AK_ISNICK)
							&& !match_wild_nocase(mask, akick->u.nc->display))
							continue;
					}
					akick_list(u, i, ci, &sent_header);
				}
			}
			if (!sent_header)
				notice_lang(s_ChanServ, u, CHAN_AKICK_NO_MATCH, chan);

		} else if (stricmp(cmd, "VIEW") == 0) {
			int sent_header = 0;
			if (ci->akickcount == 0) {
				notice_lang(s_ChanServ, u, CHAN_AKICK_LIST_EMPTY, chan);
				return MOD_CONT;
			}
			if (mask && isdigit(*mask) &&
				strspn(mask, "1234567890,-") == strlen(mask)) {
				process_numlist(mask, NULL, akick_view_callback, u, ci,
								&sent_header);
			} else {
				for (akick = ci->akick, i = 0; i < ci->akickcount;
					 akick++, i++) {
					if (!(akick->flags & AK_USED))
						continue;
					if (mask) {
						if (!(akick->flags & AK_ISNICK)
							&& !match_wild_nocase(mask, akick->u.mask))
							continue;
						if ((akick->flags & AK_ISNICK)
							&& !match_wild_nocase(mask, akick->u.nc->display))
							continue;
					}
					akick_view(u, i, ci, &sent_header);
				}
			}
			if (!sent_header)
				notice_lang(s_ChanServ, u, CHAN_AKICK_NO_MATCH, chan);

		} else if (stricmp(cmd, "ENFORCE") == 0) {
			c = findchan(ci->name);
			cu = NULL;
			count = 0;

			if (!c) {
				notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, ci->name);
				return MOD_CONT;
			}

			cu = c->users;

			while (cu) {
				unext = cu->next;
				if (check_kick(cu->user, c->name, c->creation_time)) {
					argv[0] = sstrdup(c->name);
					argv[1] = sstrdup(cu->user->nick);
					argv[2] = sstrdup(CSAutokickReason);

					do_kick(s_ChanServ, 3, argv);

					delete [] argv[2];
					delete [] argv[1];
					delete [] argv[0];

					count++;
				}
				cu = unext;
			}

			notice_lang(s_ChanServ, u, CHAN_AKICK_ENFORCE_DONE, chan, count);

		} else if (stricmp(cmd, "CLEAR") == 0) {

			if (readonly) {
				notice_lang(s_ChanServ, u, CHAN_AKICK_DISABLED);
				return MOD_CONT;
			}

			for (akick = ci->akick, i = 0; i < ci->akickcount; akick++, i++) {
				if (!(akick->flags & AK_USED))
					continue;
				akick_del(u, akick);
			}

			free(ci->akick);
			ci->akick = NULL;
			ci->akickcount = 0;

			notice_lang(s_ChanServ, u, CHAN_AKICK_CLEAR, ci->name);

		} else {
			syntax_error(s_ChanServ, u, "AKICK", CHAN_AKICK_SYNTAX);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_help(s_ChanServ, u, CHAN_HELP_AKICK);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "AKICK", CHAN_AKICK_SYNTAX);
	}
};




class CSAKick : public Module
{
 public:
	CSAKick(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);
		this->AddCommand(CHANSERV, new CommandCSAKick(), MOD_UNIQUE);

		this->SetChanHelp(myChanServHelp);
	}
};

MODULE_INIT("cs_akick", CSAKick)

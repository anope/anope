/* Various routines to perform simple actions.
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

#include "services.h"

/*************************************************************************/

/**
 * Note a bad password attempt for the given user.  If they've used up
 * their limit, toss them off.
 * @param u the User to check
 * @return void
 */
void bad_password(User * u)
{
	time_t now = time(NULL);

	if (!u || !BadPassLimit) {
		return;
	}

	if (BadPassTimeout > 0 && u->invalid_pw_time > 0
		&& u->invalid_pw_time < now - BadPassTimeout)
		u->invalid_pw_count = 0;
	u->invalid_pw_count++;
	u->invalid_pw_time = now;
	if (u->invalid_pw_count >= BadPassLimit) {
		kill_user(NULL, u->nick, "Too many invalid passwords");
	}
}

/*************************************************************************/

/**
 * Remove a user from the IRC network.
 * @param source is the nick which should generate the kill, or NULL for a server-generated kill.
 * @param user to remove
 * @param reason for the kill
 * @return void
 */
void kill_user(const char *source, const char *user, const char *reason)
{
	char buf[BUFSIZE];

	if (!user || !*user) {
		return;
	}
	if (!source || !*source) {
		source = ServerName;
	}
	if (!reason) {
		reason = "";
	}

	snprintf(buf, sizeof(buf), "%s (%s)", source, reason);

	ircdproto->SendSVSKill(source, user, buf);

	if (!ircd->quitonkill && finduser(user)) {
		do_kill(user, buf);
	}
}

/*************************************************************************/

/**
 * Check and enforce SQlines
 * @param mask of the sqline
 * @param reason for the sqline
 * @return void
 */
void sqline(char *mask, char *reason)
{
	int i;
	Channel *c, *next;
	const char *av[3];
	struct c_userlist *cu, *cunext;

	if (ircd->chansqline) {
		if (*mask == '#') {
			ircdproto->SendSQLine(mask, reason);

			for (i = 0; i < 1024; i++) {
				for (c = chanlist[i]; c; c = next) {
					next = c->next;

					if (!match_wild_nocase(mask, c->name)) {
						continue;
					}
					for (cu = c->users; cu; cu = cunext) {
						cunext = cu->next;
						if (is_oper(cu->user)) {
							continue;
						}
						av[0] = c->name;
						av[1] = cu->user->nick;
						av[2] = reason;
						ircdproto->SendKick(findbot(s_OperServ), av[0], av[1],
									   "Q-Lined: %s", av[2]);
						do_kick(s_ChanServ, 3, av);
					}
				}
			}
		} else {
			ircdproto->SendSQLine(mask, reason);
		}
	} else {
		ircdproto->SendSQLine(mask, reason);
	}
}

/*************************************************************************/

/**
 * Unban the nick from a channel
 * @param ci channel info for the channel
 * @param nick to remove the ban for
 * @return void
 */
void common_unban(ChannelInfo * ci, char *nick)
{
	const char *av[4];
	char *host = NULL;
	char buf[BUFSIZE];
	int ac;
	uint32 ip = 0;
	User *u;
	Entry *ban, *next;

	if (!ci || !ci->c || !nick) {
		return;
	}

	if (!(u = finduser(nick))) {
		return;
	}

	if (!ci->c->bans || (ci->c->bans->count == 0))
		return;

	if (u->hostip == NULL) {
		host = host_resolve(u->host);
		/* we store the just resolved hostname so we don't
		 * need to do this again */
		if (host) {
			u->hostip = sstrdup(host);
		}
	} else {
		host = sstrdup(u->hostip);
	}
	/* Convert the host to an IP.. */
	if (host)
		ip = str_is_ip(host);

	if (ircd->svsmode_unban) {
		ircdproto->SendBanDel(ci->name, nick);
	} else {
		if (ircdcap->tsmode) {
			snprintf(buf, BUFSIZE - 1, "%ld", static_cast<long>(time(NULL)));
			av[0] = ci->name;
			av[1] = buf;
			av[2] = "-b";
			ac = 4;
		} else {
			av[0] = ci->name;
			av[1] = "-b";
			ac = 3;
		}

		for (ban = ci->c->bans->entries; ban; ban = next) {
			next = ban->next;
			if (entry_match(ban, u->nick, u->username, u->host, ip) ||
				entry_match(ban, u->nick, u->username, u->vhost, ip)) {
				ircdproto->SendMode(whosends(ci), ci->name, "-b %s", ban->mask);
				if (ircdcap->tsmode)
					av[3] = ban->mask;
				else
					av[2] = ban->mask;

				do_cmode(whosends(ci)->nick, ac, av);
			}
		}
	}
	/* host_resolve() sstrdup us this info so we gotta free it */
	if (host) {
		delete [] host;
	}
}

/*************************************************************************/

/**
 * Prepare to set SVSMODE and update internal user modes
 * @param u user to apply modes to
 * @param modes the modes to set on the user
 * @param arg the arguments for the user modes
 * @return void
 */
void common_svsmode(User * u, const char *modes, const char *arg)
{
	int ac = 1;
	const char *av[2];

	av[0] = modes;
	if (arg) {
		av[1] = arg;
		ac++;
	}

	ircdproto->SendSVSMode(u, ac, av);
	ircdproto->ProcessUsermodes(u, ac, av);
}

/*************************************************************************/

/**
 * Get the vhost for the user, if set else return the host, on ircds without
 * vhost this returns the host
 * @param u user to get the vhost for
 * @return vhost
 */
char *common_get_vhost(User * u)
{
	if (!u)
		return NULL;

	if (ircd->vhostmode && (u->mode & ircd->vhostmode))
		return u->vhost;
	else if (ircd->vhost && u->vhost)
		return u->vhost;
	else
		return u->host;
}

/*************************************************************************/

/**
 * Get the vident for the user, if set else return the ident, on ircds without
 * vident this returns the ident
 * @param u user to get info the vident for
 * @return vident
 */
char *common_get_vident(User * u)
{
	if (!u)
		return NULL;

	if (ircd->vhostmode && (u->mode & ircd->vhostmode))
		return u->vident;
	else if (ircd->vident && u->vident)
		return u->vident;
	else
		return u->username;
}

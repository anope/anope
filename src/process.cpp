/* Main processing code for Services.
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "modules.h"

/*************************************************************************/

/* Use ignore code? */
int allow_ignore = 1;

/* Masks to ignore. */
IgnoreData *ignore;

/*************************************************************************/

/**
 * Add a mask/nick to the ignorelits for delta seconds.
 * @param nick Nick or (nick!)user@host to add to the ignorelist.
 * @param delta Seconds untill new entry is set to expire. 0 for permanent.
 */
void add_ignore(const Anope::string &nick, time_t delta)
{
	IgnoreData *ign;
	Anope::string tmp, mask;
	size_t user, host;
	User *u;
	time_t now;
	if (nick.empty())
		return;
	now = time(NULL);
	/* If it s an existing user, we ignore the hostmask. */
	if ((u = finduser(nick)))
		mask = "*!*@" + u->host;
	/* Determine whether we get a nick or a mask. */
	else if ((host = nick.find('@')) != Anope::string::npos)
	{
		/* Check whether we have a nick too.. */
		if ((user = nick.find('!')) != Anope::string::npos)
		{
			/* this should never happen */
			if (user > host)
				return;
			mask = nick;
		}
		else
			/* We have user@host. Add nick wildcard. */
			mask = "*!" + nick;
	}
	/* We only got a nick.. */
	else
		mask = nick + "!*@*";
	/* Check if we already got an identical entry. */
	for (ign = ignore; ign; ign = ign->next)
		if (mask.equals_ci(ign->mask))
			break;
	/* Found one.. */
	if (ign)
	{
		if (!delta)
			ign->time = 0;
		else if (ign->time < now + delta)
			ign->time = now + delta;
	}
	/* Create new entry.. */
	else
	{
		ign = new IgnoreData;
		ign->mask = mask;
		ign->time = !delta ? 0 : now + delta;
		ign->prev = NULL;
		ign->next = ignore;
		if (ignore)
			ignore->prev = ign;
		ignore = ign;
		Alog(LOG_DEBUG) << "Added new ignore entry for " << mask;
	}
}

/*************************************************************************/

/**
 * Retrieve an ignorance record for a nick or mask.
 * If the nick isn't being ignored, we return NULL and if necesary
 * flush the record from the ignore list (i.e. ignore timed out).
 * @param nick Nick or (nick!)user@host to look for on the ignorelist.
 * @return Pointer to the ignore record, NULL if none was found.
 */
IgnoreData *get_ignore(const Anope::string &nick)
{
	IgnoreData *ign;
	Anope::string tmp;
	size_t user, host;
	time_t now;
	User *u;
	if (nick.empty())
		return NULL;
	/* User has disabled the IGNORE system */
	if (!allow_ignore)
		return NULL;
	now = time(NULL);
	u = finduser(nick);
	/* If we find a real user, match his mask against the ignorelist. */
	if (u)
	{
		/* Opers are not ignored, even if a matching entry may be present. */
		if (is_oper(u))
			return NULL;
		for (ign = ignore; ign; ign = ign->next)
			if (match_usermask(ign->mask, u))
				break;
	}
	else
	{
		/* We didn't get a user.. generate a valid mask. */
		if ((host = nick.find('@')) != Anope::string::npos)
		{
			if ((user = nick.find('!')) != Anope::string::npos)
			{
				/* this should never happen */
				if (user > host)
					return NULL;
				tmp = nick;
			}
			else
				/* We have user@host. Add nick wildcard. */
			tmp = "*!" + nick;
		}
		/* We only got a nick.. */
		else
			tmp = nick + "!*@*";
		for (ign = ignore; ign; ign = ign->next)
			if (Anope::Match(tmp, ign->mask))
				break;
	}
	/* Check whether the entry has timed out */
	if (ign && ign->time != 0 && ign->time <= now)
	{
		Alog(LOG_DEBUG) << "Expiring ignore entry " << ign->mask;
		if (ign->prev)
			ign->prev->next = ign->next;
		else if (ignore == ign)
			ignore = ign->next;
		if (ign->next)
			ign->next->prev = ign->prev;
		delete ign;
		ign = NULL;
	}
	if (ign && debug)
		Alog(LOG_DEBUG) << "Found ignore entry (" << ign->mask << ") for " << nick;
	return ign;
}

/*************************************************************************/

/**
 * Deletes a given nick/mask from the ignorelist.
 * @param nick Nick or (nick!)user@host to delete from the ignorelist.
 * @return Returns 1 on success, 0 if no entry is found.
 */
int delete_ignore(const Anope::string &nick)
{
	IgnoreData *ign;
	Anope::string tmp;
	size_t user, host;
	User *u;
	if (nick.empty())
		return 0;
	/* If it s an existing user, we ignore the hostmask. */
	if ((u = finduser(nick)))
		tmp = "*!*@" + u->host;
	/* Determine whether we get a nick or a mask. */
	else if ((host = nick.find('@')) != Anope::string::npos)
	{
		/* Check whether we have a nick too.. */
		if ((user = nick.find('!')) != Anope::string::npos)
		{
			/* this should never happen */
			if (user > host)
				return 0;
			tmp = nick;
		}
		else
			/* We have user@host. Add nick wildcard. */
			tmp = "*!" + nick;
	}
	/* We only got a nick.. */
	else
		tmp = nick + "!*@*";
	for (ign = ignore; ign; ign = ign->next)
		if (tmp.equals_ci(ign->mask))
			break;
	/* No matching ignore found. */
	if (!ign)
		return 0;
	Alog(LOG_DEBUG) << "Deleting ignore entry " << ign->mask;
	/* Delete the entry and all references to it. */
	if (ign->prev)
		ign->prev->next = ign->next;
	else if (ignore == ign)
		ignore = ign->next;
	if (ign->next)
		ign->next->prev = ign->prev;
	delete ign;
	ign = NULL;
	return 1;
}

/*************************************************************************/

/**
 * Clear the ignorelist.
 * @return The number of entries deleted.
 */
int clear_ignores()
{
	IgnoreData *ign, *next;
	int i = 0;
	if (!ignore)
		return 0;
	for (ign = ignore; ign; ign = next)
	{
		next = ign->next;
		Alog(LOG_DEBUG) << "Deleting ignore entry " << ign->mask;
		delete ign;
		++i;
	}
	ignore = NULL;
	return i;
}

/*************************************************************************/
/* split_buf:  Split a buffer into arguments and store the arguments in an
 *             argument vector pointed to by argv (which will be malloc'd
 *             as necessary); return the argument count.  If colon_special
 *             is non-zero, then treat a parameter with a leading ':' as
 *             the last parameter of the line, per the IRC RFC.  Destroys
 *             the buffer by side effect.
 */
int split_buf(char *buf, const char ***argv, int colon_special)
{
	int argvsize = 8;
	int argc;
	char *s;

	*argv = static_cast<const char **>(scalloc(sizeof(const char *) * argvsize, 1));
	argc = 0;
	while (*buf)
	{
		if (argc == argvsize)
		{
			argvsize += 8;
			*argv = static_cast<const char **>(srealloc(*argv, sizeof(const char *) * argvsize));
		}
		if (*buf == ':')
		{
			(*argv)[argc++] = buf + 1;
			buf = const_cast<char *>(""); // XXX: unsafe cast.
		}
		else
		{
			s = strpbrk(buf, " ");
			if (s)
			{
				*s++ = 0;
				while (*s == ' ')
					++s;
			}
			else
				s = buf + strlen(buf);
			(*argv)[argc++] = buf;
			buf = s;
		}
	}
	return argc;
}

/*************************************************************************/

/* process:  Main processing routine.  Takes the string in inbuf (global
 *           variable) and does something appropriate with it. */

void process(const Anope::string &buffer)
{
	int retVal = 0;
	char source[64] = "";
	char cmd[64] = "";
	char buf[1024] = ""; // XXX InspIRCd 2.0 can send messages longer than 512 characters to servers... how disappointing.
	char *s;
	int ac; /* Parameters for the command */
	const char **av;

	/* If debugging, log the buffer */
	Alog(LOG_DEBUG) << "Received: " << buffer;

	/* First make a copy of the buffer so we have the original in case we
	 * crash - in that case, we want to know what we crashed on. */
	strscpy(buf, buffer.c_str(), sizeof(buf));

	doCleanBuffer(buf);

	/* Split the buffer into pieces. */
	if (*buf == ':')
	{
		s = strpbrk(buf, " ");
		if (!s)
			return;
		*s = 0;
		while (isspace(*++s));
		strscpy(source, buf + 1, sizeof(source));
		memmove(buf, s, strlen(s) + 1);
	}
	else
		*source = 0;
	if (!*buf)
		return;
	s = strpbrk(buf, " ");
	if (s)
	{
		*s = 0;
		while (isspace(*++s));
	}
	else
		s = buf + strlen(buf);
	strscpy(cmd, buf, sizeof(cmd));
	ac = split_buf(s, &av, 1);

	if (protocoldebug)
	{
		if (*source)
			Alog() << "Source " << source;
		if (*cmd)
			Alog() << "Token " << cmd;
		if (ac)
		{
			int i;
			for (i = 0; i < ac; ++i)
				Alog() << "av[" << i << "] = " << av[i];
		}
		else
			Alog() << "av[0] = NULL";
	}

	/* Do something with the message. */
	std::vector<Message *> messages = Anope::FindMessage(cmd);

	if (!messages.empty())
	{
		retVal = MOD_CONT;

		for (std::vector<Message *>::iterator it = messages.begin(), it_end = messages.end(); retVal == MOD_CONT && it != it_end; ++it)
		{
			Message *m = *it;

			if (m->func)
				retVal = m->func(source, ac, av);
		}
	}
	else
		Alog(LOG_DEBUG) << "unknown message from server (" << buffer << ")";

	/* Free argument list we created */
	free(av);
}

/*************************************************************************/

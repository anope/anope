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
std::list<IgnoreData *> ignore;

/*************************************************************************/

/**
 * Add a mask/nick to the ignorelist for delta seconds.
 * @param nick Nick or (nick!)user@host to add to the ignorelist.
 * @param delta Seconds untill new entry is set to expire. 0 for permanent.
 */
void add_ignore(const Anope::string &nick, time_t delta)
{
	if (nick.empty())
		return;
	/* If it s an existing user, we ignore the hostmask. */
	Anope::string mask;
	User *u = finduser(nick);
	size_t user, host;
	if (u)
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
	std::list<IgnoreData *>::iterator ign = ignore.begin(), ign_end = ignore.end();
	for (; ign != ign_end; ++ign)
		if (mask.equals_ci((*ign)->mask))
			break;
	time_t now = time(NULL);
	/* Found one.. */
	if (ign != ign_end)
	{
		if (!delta)
			(*ign)->time = 0;
		else if ((*ign)->time < now + delta)
			(*ign)->time = now + delta;
	}
	/* Create new entry.. */
	else
	{
		IgnoreData *newign = new IgnoreData();
		newign->mask = mask;
		newign->time = delta ? now + delta : 0;
		ignore.push_front(newign);
		Log(LOG_DEBUG) << "Added new ignore entry for " << mask;
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
	if (nick.empty())
		return NULL;
	/* User has disabled the IGNORE system */
	if (!allow_ignore)
		return NULL;
	User *u = finduser(nick);
	std::list<IgnoreData *>::iterator ign = ignore.begin(), ign_end = ignore.end();
	/* If we find a real user, match his mask against the ignorelist. */
	if (u)
	{
		/* Opers are not ignored, even if a matching entry may be present. */
		if (is_oper(u))
			return NULL;
		for (; ign != ign_end; ++ign)
			if (match_usermask((*ign)->mask, u))
				break;
	}
	else
	{
		Anope::string tmp;
		size_t user, host;
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
		for (; ign != ign_end; ++ign)
			if (Anope::Match(tmp, (*ign)->mask))
				break;
	}
	time_t now = time(NULL);
	/* Check whether the entry has timed out */
	if (ign != ign_end && (*ign)->time && (*ign)->time <= now)
	{
		Log(LOG_DEBUG) << "Expiring ignore entry " << (*ign)->mask;
		delete *ign;
		ignore.erase(ign);
		ign = ign_end = ignore.end();
	}
	if (ign != ign_end)
		Log(LOG_DEBUG) << "Found ignore entry (" << (*ign)->mask << ") for " << nick;
	return ign == ign_end ? NULL : *ign;
}

/*************************************************************************/

/**
 * Deletes a given nick/mask from the ignorelist.
 * @param nick Nick or (nick!)user@host to delete from the ignorelist.
 * @return Returns 1 on success, 0 if no entry is found.
 */
int delete_ignore(const Anope::string &nick)
{
	if (nick.empty())
		return 0;
	/* If it s an existing user, we ignore the hostmask. */
	Anope::string tmp;
	size_t user, host;
	User *u = finduser(nick);
	if (u)
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
	std::list<IgnoreData *>::iterator ign = ignore.begin(), ign_end = ignore.end();
	for (; ign != ign_end; ++ign)
		if (tmp.equals_ci((*ign)->mask))
			break;
	/* No matching ignore found. */
	if (ign == ign_end)
		return 0;
	Log(LOG_DEBUG) << "Deleting ignore entry " << (*ign)->mask;
	/* Delete the entry and all references to it. */
	delete *ign;
	ignore.erase(ign);
	return 1;
}

/*************************************************************************/

/**
 * Clear the ignorelist.
 * @return The number of entries deleted.
 */
int clear_ignores()
{
	if (ignore.empty())
		return 0;
	for (std::list<IgnoreData *>::iterator ign = ignore.begin(), ign_end = ignore.end(); ign != ign_end; ++ign)
	{
		Log(LOG_DEBUG) << "Deleting ignore entry " << (*ign)->mask;
		delete *ign;
	}
	int deleted = ignore.size();
	ignore.clear();
	return deleted;
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
	Log(LOG_RAWIO) << "Received: " << buffer;

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
			Log() << "Source " << source;
		if (*cmd)
			Log() << "Token " << cmd;
		if (ac)
		{
			int i;
			for (i = 0; i < ac; ++i)
				Log() << "av[" << i << "] = " << av[i];
		}
		else
			Log() << "av[0] = NULL";
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
		Log(LOG_DEBUG) << "unknown message from server (" << buffer << ")";

	/* Free argument list we created */
	free(av);
}

/*************************************************************************/

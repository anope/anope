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
	/* Found one.. */
	if (ign != ign_end)
	{
		if (!delta)
			(*ign)->time = 0;
		else if ((*ign)->time < Anope::CurTime + delta)
			(*ign)->time = Anope::CurTime + delta;
	}
	/* Create new entry.. */
	else
	{
		IgnoreData *newign = new IgnoreData();
		newign->mask = mask;
		newign->time = delta ? Anope::CurTime + delta : 0;
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
		if (u->HasMode(UMODE_OPER))
			return NULL;
		for (; ign != ign_end; ++ign)
		{
			Entry ignore_mask((*ign)->mask);
			if (ignore_mask.Matches(u))
				break;
		}
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
	/* Check whether the entry has timed out */
	if (ign != ign_end && (*ign)->time && (*ign)->time <= Anope::CurTime)
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

/** Main process routine
 * @param buffer A raw line from the uplink to do things with
 */
void process(const Anope::string &buffer)
{
	/* If debugging, log the buffer */
	Log(LOG_RAWIO) << "Received: " << buffer;

	/* Strip all extra spaces */
	Anope::string buf = buffer;
	buf = buf.replace_all_cs("  ", " ");

	if (buf.empty())
		return;

	Anope::string source;
	if (buf[0] == ':')
	{
		size_t space = buf.find_first_of(" ");
		if (space == Anope::string::npos)
			return;
		source = buf.substr(1, space - 1);
		buf = buf.substr(space + 1);
		if (source.empty() || buf.empty())
			return;
	}

	spacesepstream buf_sep(buf);
	Anope::string buf_token;

	Anope::string command = buf;
	if (buf_sep.GetToken(buf_token))
		command = buf_token;
	
	std::vector<Anope::string> params;
	while (buf_sep.GetToken(buf_token))
	{
		if (buf_token[0] == ':')
		{
			if (!buf_sep.StreamEnd())
				params.push_back(buf_token.substr(1) + " " + buf_sep.GetRemaining());
			else
				params.push_back(buf_token.substr(1));
			break;
		}
		else
			params.push_back(buf_token);
	}

	if (protocoldebug)
	{
		Log() << "Source : " << (source.empty() ? "No source" : source);
		Log() << "Command: " << command;

		if (params.empty())
			Log() << "No params";
		else
			for (unsigned i = 0; i < params.size(); ++i)
				Log() << "params " << i << ": " << params[i];
	}

	std::vector<Message *> messages = Anope::FindMessage(command);

	if (!messages.empty())
	{
		bool retVal = true;

		for (std::vector<Message *>::iterator it = messages.begin(), it_end = messages.end(); retVal == true && it != it_end; ++it)
		{
			Message *m = *it;

			if (m->func)
				retVal = m->func(source, params);
		}
	}
	else
		Log(LOG_DEBUG) << "unknown message from server (" << buffer << ")";
}


 /* POSIX emulation layer for Windows.
 *
 * Copyright (C) 2008 Robin Burchell <w00t@inspircd.org>
 * Copyright (C) 2008 Anope Team <info@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */   

#ifdef WIN32
const char *dlerror()
{
	static char errbuf[513];
	DWORD err = GetLastError();
	if (err == 0)
		return NULL;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
				  FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, 0, errbuf, 512,
				  NULL);
	return errbuf;
}
#endif

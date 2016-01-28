/* POSIX emulation layer for Windows.
 *
 * (C) 2008-2016 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "dir.h"
#include <stdio.h>
 
DIR *opendir(const char *path)
{
	char real_path[MAX_PATH];
	_snprintf(real_path, sizeof(real_path), "%s/*", path);

	DIR *d = new DIR();
	d->handle = FindFirstFile(real_path, &d->data);
	d->read_first = false;

	if (d->handle == INVALID_HANDLE_VALUE)
	{
		delete d;
		return NULL;
	}

	return d;
}

dirent *readdir(DIR *d)
{
	if (d->read_first == false)
		d->read_first = true;
	else if (!FindNextFile(d->handle, &d->data))
		return NULL;

	d->ent.d_ino = 1;
	d->ent.d_name = d->data.cFileName;

	return &d->ent;
}

int closedir(DIR *d)
{
	FindClose(d->handle);
	delete d;
	return 0;
}

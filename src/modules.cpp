/* Modular support
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "modules.h"
#include "language.h"
#include "version.h"

std::multimap<std::string, Message *> MessageMap;
std::deque<Module *> Modules;

char *mod_current_buffer = NULL;

char *ModuleGetErrStr(int status)
{
	const char *module_err_str[] = {
		"Module, Okay - No Error",						/* MOD_ERR_OK */
		"Module Error, Allocating memory",				/* MOD_ERR_MEMORY */
		"Module Error, Not enough parameters",			/* MOD_ERR_PARAMS */
		"Module Error, Already loaded",					/* MOD_ERR_EXISTS */
		"Module Error, File does not exist",			/* MOD_ERR_NOEXIST */
		"Module Error, No User",						/* MOD_ERR_NOUSER */
		"Module Error, Error during load time or module returned MOD_STOP", /* MOD_ERR_NOLOAD */
		"Module Error, Unable to unload",				/* MOD_ERR_NOUNLOAD */
		"Module Error, Incorrect syntax",				/* MOD_ERR_SYNTAX */
		"Module Error, Unable to delete",				/* MOD_ERR_NODELETE */
		"Module Error, Unknown Error occuried",			/* MOD_ERR_UNKOWN */
		"Module Error, File I/O Error",					/* MOD_ERR_FILE_IO */
		"Module Error, No Service found for request",	/* MOD_ERR_NOSERVICE */
		"Module Error, No module name for request"		/* MOD_ERR_NO_MOD_NAME */
	};
	return const_cast<char *>(module_err_str[status]);
}

/************************************************/

/**
 * Load the ircd protocol module up
 **/
int protocol_module_init()
{
	int ret = 0;

	Alog() << "Loading IRCD Protocol Module: [" << Config.IRCDModule << "]";
	ret = ModuleManager::LoadModule(Config.IRCDModule, NULL);

	if (ret == MOD_ERR_OK)
	{
		FindModule(Config.IRCDModule)->SetType(PROTOCOL);
		/* This is really NOT the correct place to do config checks, but
		 * as we only have the ircd struct filled here, we have to over
		 * here. -GD
		 */
		if (ircd->ts6)
		{
			if (!Config.Numeric)
			{
				Alog() << "This IRCd protocol requires a server id to be set in Anope's configuration.";
				ret = -1;
			}
		}
	}

	return ret;
}

void Module::InsertLanguage(int langNumber, int ac, const char **av)
{
	int i;

	Alog(LOG_DEBUG) << this->name << "Adding " << ac << " texts for language " << langNumber;

	if (this->lang[langNumber].argc > 0)
		this->DeleteLanguage(langNumber);

	this->lang[langNumber].argc = ac;
	this->lang[langNumber].argv = new char *[ac];
	for (i = 0; i < ac; ++i)
		this->lang[langNumber].argv[i] = sstrdup(av[i]);
}

/**
 * Search the list of loaded modules for the given name.
 * @param name the name of the module to find
 * @return a pointer to the module found, or NULL
 */
Module *FindModule(const std::string &name)
{
	for (std::deque<Module *>::iterator it = Modules.begin(), it_end = Modules.end(); it != it_end; ++it)
	{
		Module *m = *it;

		if (m->name == name)
			return m;
	}

	return NULL;
}

Module *FindModule(const char *name)
{
	return FindModule(std::string(name));
}

Module *FindModule(const ci::string &name)
{
	return FindModule(std::string(name.c_str()));
}

/** Add a message to Anope
 * @param name The message name as sent by the IRCd
 * @param func A callback function that will be called when this message is received
 * @return The new message object
 */
Message *Anope::AddMessage(const std::string &name, int (*func)(const char *source, int ac, const char **av))
{
	Message *m = new Message;

	m->name = name;
	m->func = func;

	MessageMap.insert(std::make_pair(m->name, m));

	return m;
}

/** Deletes a message from Anope
 * XXX Im not sure what will happen if this function is called indirectly from a message function pointed to by this message and there
 * is more than one hook for this message.. must check
 * @param m The message
 * @return true if the message was found and deleted, else false
 */
bool Anope::DelMessage(Message *m)
{
	std::multimap<std::string, Message *>::iterator it = MessageMap.find(m->name);

	if (it == MessageMap.end())
		return false;

	std::multimap<std::string, Message *>::iterator upper = MessageMap.upper_bound(m->name);

	for (; it != upper; ++it)
	{
		if (it->second == m)
		{
			delete m;
			MessageMap.erase(it);
			return true;
		}
	}

	return false;
}

/*******************************************************************************
 * Command Functions
 *******************************************************************************/

int Module::AddCommand(BotInfo *bi, Command *c)
{
	if (!bi || !c)
		return MOD_ERR_PARAMS;

	c->module = this;
	c->service = bi;

	std::pair<std::map<ci::string, Command *>::iterator, bool> it = bi->Commands.insert(std::make_pair(c->name, c));

	if (it.second != true)
	{
		Alog() << "Error creating command " << c->name << ". Command already exists!";
		delete c;
		return MOD_ERR_EXISTS;
	}

	return MOD_ERR_OK;
}

/**
 * Delete a command from the service given.
 * @param cmdTable the cmdTable for the services to remove the command from
 * @param name the name of the command to delete from the service
 * @return returns MOD_ERR_OK on success
 */
int Module::DelCommand(BotInfo *bi, Command *c)
{
	if (!bi || !c)
		return MOD_ERR_PARAMS;

	if (!bi->Commands.erase(c->name))
		return MOD_ERR_NOEXIST;

	return MOD_ERR_OK;
}

/**
 * Find a message in the message table
 * @param name The name of the message were looking for
 * @return NULL if we cant find it, or a pointer to the Message if we can
 **/
std::vector<Message *> FindMessage(const std::string &name)
{
	std::vector<Message *> messages;

	std::multimap<std::string, Message *>::iterator it = MessageMap.find(name);

	if (it == MessageMap.end())
		return messages;

	std::multimap<std::string, Message *>::iterator upper = MessageMap.upper_bound(name);

	for (; it != upper; ++it)
		messages.push_back(it->second);

	return messages;
}

/*******************************************************************************
 * Module Callback Functions
 *******************************************************************************/

 /* Check the current version of anope against a given version number
 * Specifiying -1 for minor,patch or build
 * @param major The major version of anope, the first part of the verison number
 * @param minor The minor version of anope, the second part of the version number
 * @param patch The patch version of anope, the third part of the version number
 * @param build The build revision of anope from SVN
 * @return True if the version newer than the version specified.
 **/
bool moduleMinVersion(int major, int minor, int patch, int build)
{
	bool ret = false;
	if (VERSION_MAJOR > major) /* Def. new */
		ret = true;
	else if (VERSION_MAJOR == major) /* Might be newer */
	{
		if (minor == -1)
			return true; /* They dont care about minor */
		if (VERSION_MINOR > minor) /* Def. newer */
			ret = true;
		else if (VERSION_MINOR == minor) /* Might be newer */
		{
			if (patch == -1)
				return true; /* They dont care about patch */
			if (VERSION_PATCH > patch)
				ret = true;
			else if (VERSION_PATCH == patch)
			{
#if 0
// XXX
				if (build == -1)
					return true; /* They dont care about build */
				if (VERSION_BUILD >= build)
					ret = true;
#endif
			}
		}
	}
	return ret;
}

void Module::NoticeLang(const char *source, User *u, int number, ...)
{
	va_list va;
	char buffer[4096], outbuf[4096];
	char *fmt = NULL;
	int mlang = Config.NSDefLanguage;
	char *s, *t, *buf;

	/* Find the users lang, and use it if we can */
	if (u && u->Account())
		mlang = u->Account()->language;

	/* If the users lang isnt supported, drop back to English */
	if (!this->lang[mlang].argc)
		mlang = LANG_EN_US;

	/* If the requested lang string exists for the language */
	if (this->lang[mlang].argc > number)
	{
		fmt = this->lang[mlang].argv[number];

		buf = sstrdup(fmt);
		va_start(va, number);
		vsnprintf(buffer, 4095, buf, va);
		va_end(va);
		s = buffer;
		while (*s)
		{
			t = s;
			s += strcspn(s, "\n");
			if (*s)
				*s++ = '\0';
			strscpy(outbuf, t, sizeof(outbuf));
			u->SendMessage(source, "%s", outbuf);
		}
		delete [] buf;
	}
	else
		Alog() << this->name << ": INVALID language string call, language: [" << mlang << "], String [" << number << "]";
}

const char *Module::GetLangString(User *u, int number)
{
	int mlang = Config.NSDefLanguage;

	/* Find the users lang, and use it if we can */
	if (u && u->Account())
		mlang = u->Account()->language;

	/* If the users lang isnt supported, drop back to English */
	if (!this->lang[mlang].argc)
		mlang = LANG_EN_US;

	/* If the requested lang string exists for the language */
	if (this->lang[mlang].argc > number)
		return this->lang[mlang].argv[number];
	/* Return an empty string otherwise, because we might be used without
	 * the return value being checked. If we would return NULL, bad things
	 * would happen!
	 */
	else
	{
		Alog() << this->name << ": INVALID language string call, language: [" << mlang << "], String [" << number << "]";
		return "";
	}
}

void Module::DeleteLanguage(int langNumber)
{
	if (this->lang[langNumber].argc)
	{
		for (int idx = 0; idx > this->lang[langNumber].argc; ++idx)
			delete [] this->lang[langNumber].argv[idx];
		delete [] this->lang[langNumber].argv;
		this->lang[langNumber].argc = 0;
	}
}

void ModuleRunTimeDirCleanUp()
{
#ifndef _WIN32
	DIR *dirp;
	struct dirent *dp;
#else
	BOOL fFinished;
	HANDLE hList;
	TCHAR szDir[MAX_PATH + 1];
	WIN32_FIND_DATA FileData;
	char buffer[_MAX_PATH];
#endif
	char dirbuf[BUFSIZE];
	char filebuf[BUFSIZE];

	snprintf(dirbuf, BUFSIZE, "%s/modules/runtime", services_dir.c_str());

	Alog(LOG_DEBUG) << "Cleaning out Module run time directory (" << dirbuf << ") - this may take a moment please wait";

#ifndef _WIN32
	if (!(dirp = opendir(dirbuf)))
	{
		Alog(LOG_DEBUG) << "cannot open directory (" << dirbuf << ")";
		return;
	}
	while ((dp = readdir(dirp)))
	{
		if (!dp->d_ino)
			continue;
		if (!stricmp(dp->d_name, ".") || !stricmp(dp->d_name, ".."))
			continue;
		snprintf(filebuf, BUFSIZE, "%s/%s", dirbuf, dp->d_name);
		unlink(filebuf);
	}
	closedir(dirp);
#else
	/* Get the current working directory: */
	if (!_getcwd(buffer, _MAX_PATH))
		Alog(LOG_DEBUG) << "Unable to set Current working directory";
	snprintf(szDir, sizeof(szDir), "%s\\%s\\*", buffer, dirbuf);

	hList = FindFirstFile(szDir, &FileData);
	if (hList != INVALID_HANDLE_VALUE)
	{
		fFinished = FALSE;
		while (!fFinished)
		{
			if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				snprintf(filebuf, BUFSIZE, "%s/%s", dirbuf, FileData.cFileName);
				DeleteFile(filebuf);
			}
			if (!FindNextFile(hList, &FileData))
			{
				if (GetLastError() == ERROR_NO_MORE_FILES)
					fFinished = TRUE;
			}
		}
	}
	else
		Alog(LOG_DEBUG) << "Invalid File Handle. GetLastError() reports "<<  static_cast<int>(GetLastError());
	FindClose(hList);
#endif
	Alog(LOG_DEBUG) << "Module run time directory has been cleaned out";
}

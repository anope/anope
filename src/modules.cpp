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

message_map MessageMap;
std::list<Module *> Modules;

Anope::string ModuleGetErrStr(int status)
{
	Anope::string module_err_str[] = {
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
	return module_err_str[status];
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
			if (Config.Numeric.empty())
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

	Alog(LOG_DEBUG) << this->name << " Adding " << ac << " texts for language " << langNumber;

	if (this->lang[langNumber].argc > 0)
		this->DeleteLanguage(langNumber);

	this->lang[langNumber].argc = ac;
	this->lang[langNumber].argv = new char *[ac];
	for (i = 0; i < ac; ++i)
		this->lang[langNumber].argv[i] = strdup(av[i]);
}

/**
 * Search the list of loaded modules for the given name.
 * @param name the name of the module to find
 * @return a pointer to the module found, or NULL
 */
Module *FindModule(const Anope::string &name)
{
	for (std::list<Module *>::const_iterator it = Modules.begin(), it_end = Modules.end(); it != it_end; ++it)
	{
		Module *m = *it;

		if (m->name.equals_ci(name))
			return m;
	}

	return NULL;
}

/** Add a message to Anope
 * @param name The message name as sent by the IRCd
 * @param func A callback function that will be called when this message is received
 * @return The new message object
 */
Message *Anope::AddMessage(const Anope::string &name, int (*func)(const Anope::string &source, int ac, const char **av))
{
	Message *m = new Message();

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
	message_map::iterator it = MessageMap.find(m->name);

	if (it == MessageMap.end())
		return false;

	message_map::iterator upper = MessageMap.upper_bound(m->name);

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

/** Find  message in the message table
 * @param name The name of the message were looking for
 * @return NULL if we cant find it, or a pointer to the Message if we can
 **/
std::vector<Message *> Anope::FindMessage(const Anope::string &name)
{
	std::vector<Message *> messages;

	message_map::iterator it = MessageMap.find(name);

	if (it == MessageMap.end())
		return messages;

	message_map::iterator upper = MessageMap.upper_bound(name);

	for (; it != upper; ++it)
		messages.push_back(it->second);

	return messages;
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

	std::pair<CommandMap::iterator, bool> it = bi->Commands.insert(std::make_pair(c->name, c));

	if (it.second != true)
	{
		Alog() << "Error creating command " << c->name << ". Command already exists!";
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

void Module::NoticeLang(const Anope::string &source, const User *u, int number, ...) const
{
	/* Find the users lang, and use it if we can */
	int mlang = Config.NSDefLanguage;
	if (u && u->Account())
		mlang = u->Account()->language;

	/* If the users lang isnt supported, drop back to English */
	if (!this->lang[mlang].argc)
		mlang = LANG_EN_US;

	/* If the requested lang string exists for the language */
	if (this->lang[mlang].argc > number)
	{
		const char *fmt = this->lang[mlang].argv[number];

		va_list va;
		va_start(va, number);

		char buf[4096] = "";
		vsnprintf(buf, sizeof(buf), fmt, va);

		sepstream lines(buf, '\n');
		Anope::string line;
		while (lines.GetToken(line))
			u->SendMessage(source, "%s", line.empty() ? " " : line.c_str());
		va_end(va);
	}
	else
		Alog() << this->name << ": INVALID language string call, language: [" << mlang << "], String [" << number << "]";
}

const char *Module::GetLangString(User *u, int number)
{

	/* Find the users lang, and use it if we can */
	int mlang = Config.NSDefLanguage;
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
			free(this->lang[langNumber].argv[idx]); // XXX
		delete [] this->lang[langNumber].argv;
		this->lang[langNumber].argc = 0;
	}
}

void ModuleRunTimeDirCleanUp()
{
	Anope::string dirbuf = services_dir + "/modules/runtime";

	Alog(LOG_DEBUG) << "Cleaning out Module run time directory (" << dirbuf << ") - this may take a moment please wait";

#ifndef _WIN32
	DIR *dirp = opendir(dirbuf.c_str());
	if (!dirp)
	{
		Alog(LOG_DEBUG) << "Cannot open directory (" << dirbuf << ")";
		return;
	}
	struct dirent *dp;
	while ((dp = readdir(dirp)))
	{
		if (!dp->d_ino)
			continue;
		if (Anope::string(dp->d_name).equals_cs(".") || Anope::string(dp->d_name).equals_cs(".."))
			continue;
		Anope::string filebuf = dirbuf + "/" + dp->d_name;
		DeleteFile(filebuf.c_str());
	}
	closedir(dirp);
#else
	Anope::string szDir = dirbuf + "/*";

	WIN32_FIND_DATA FileData;
	HANDLE hList = FindFirstFile(szDir.c_str(), &FileData);
	if (hList != INVALID_HANDLE_VALUE)
	{
		bool fFinished = false;
		while (!fFinished)
		{
			if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				Anope::string filebuf = dirbuf + "/" + FileData.cFileName;
				if (!DeleteFile(filebuf.c_str()))
					Alog(LOG_DEBUG) << "Error deleting file " << filebuf << " - GetLastError() reports " << dlerror();
			}
			if (!FindNextFile(hList, &FileData))
			{
				if (GetLastError() == ERROR_NO_MORE_FILES)
					fFinished = true;
			}
		}
	}
	else
		Alog(LOG_DEBUG) << "Invalid File Handle. GetLastError() reports "<<  static_cast<int>(GetLastError());

	FindClose(hList);
#endif
	Alog(LOG_DEBUG) << "Module run time directory has been cleaned out";
}

Version Module::GetVersion() const
{
	return Version(VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
}

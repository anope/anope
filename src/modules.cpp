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
#include "version.h"
#include <libintl.h>

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

	Log() << "Loading IRCD Protocol Module: [" << Config->IRCDModule << "]";
	ret = ModuleManager::LoadModule(Config->IRCDModule, NULL);

	if (ret == MOD_ERR_OK)
	{
		FindModule(Config->IRCDModule)->SetType(PROTOCOL);
		/* This is really NOT the correct place to do config checks, but
		 * as we only have the ircd struct filled here, we have to over
		 * here. -GD
		 */
		if (ircd->ts6)
		{
			if (Config->Numeric.empty())
			{
				Log() << "This IRCd protocol requires a server id to be set in Anope's configuration.";
				ret = -1;
			}
		}
	}

	return ret;
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
		Log() << "Error creating command " << c->name << ". Command already exists!";
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

void ModuleRunTimeDirCleanUp()
{
	Anope::string dirbuf = services_dir + "/modules/runtime";

	Log(LOG_DEBUG) << "Cleaning out Module run time directory (" << dirbuf << ") - this may take a moment please wait";

#ifndef _WIN32
	DIR *dirp = opendir(dirbuf.c_str());
	if (!dirp)
	{
		Log(LOG_DEBUG) << "Cannot open directory (" << dirbuf << ")";
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
					Log(LOG_DEBUG) << "Error deleting file " << filebuf << " - GetLastError() reports " << LastError();
			}
			if (!FindNextFile(hList, &FileData))
			{
				if (GetLastError() == ERROR_NO_MORE_FILES)
					fFinished = true;
			}
		}
	}
	else
		Log(LOG_DEBUG) << "Invalid File Handle. GetLastError() reports "<<  static_cast<int>(GetLastError());

	FindClose(hList);
#endif
	Log(LOG_DEBUG) << "Module run time directory has been cleaned out";
}

Version Module::GetVersion() const
{
	return Version(VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
}

void Module::SendMessage(BotInfo *from, User *to, const char *fmt, ...)
{
	Anope::string language = (to && to->Account() ? to->Account()->language : "");
	/* For older databases */
	if (language == "en")
		language.clear();
	if (language.empty() && !Config->NSDefLanguage.empty())
		language = Config->NSDefLanguage;

	const char *message = fmt;
#if GETTEXT_FOUND
	if (!language.empty())
	{
		setlocale(LC_ALL, language.c_str());
		message = dgettext(this->name.c_str(), fmt);
		setlocale(LC_ALL, "");
	}
#endif

	va_list args;
	char buf[4096];
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf) - 1, message, args);
	va_end(args);

	sepstream sep(buf, '\n');
	Anope::string token;

	while (sep.GetToken(token))
		to->SendMessage(from->nick, token);
}

Service::Service(Module *o, const Anope::string &n) : owner(o), name(n)
{
	ModuleManager::RegisterService(this);
}

Service::~Service()
{
	ModuleManager::UnregisterService(this);
}


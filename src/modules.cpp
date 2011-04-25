/* Modular support
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "modules.h"

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

/** Message constructor, adds the message to Anope
 * @param n The message name
 * @param f A callback function
 */
Message::Message(const Anope::string &n, bool (*f)(const Anope::string &, const std::vector<Anope::string> &)) : name(n), func(f)
{
	MessageMap.insert(std::make_pair(this->name, this));
}

/** Message destructor
 */
Message::~Message()
{
	message_map::iterator it = MessageMap.find(this->name);

	if (it == MessageMap.end())
		return;

	message_map::iterator upper = MessageMap.upper_bound(this->name);

	for (; it != upper; ++it)
	{
		if (it->second == this)
		{
			MessageMap.erase(it);
			break;
		}
	}
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
	
	c->module = NULL;
	c->service = NULL;

	return MOD_ERR_OK;
}

void Module::SendMessage(CommandSource &source, const char *fmt, ...)
{
	Anope::string language = (source.u && source.u->Account() ? source.u->Account()->language : "");
	
	PushLanguage(this->name, language);
	fmt = anope_gettext(fmt);

	va_list args;
	char buf[4096];
	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf) - 1, fmt, args);
	va_end(args);

	sepstream sep(buf, '\n');
	Anope::string token;

	while (sep.GetToken(token))
		source.Reply(token);
	
	PopLanguage();
}

Service::Service(Module *o, const Anope::string &n) : owner(o), name(n)
{
}

Service::~Service()
{
	ModuleManager::UnregisterService(this);
}


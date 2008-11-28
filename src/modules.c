/* Modular support
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
#include "modules.h"
#include "language.h"
#include "version.h"

/**
 * Declare all the list's we want to use here
 **/
CommandHash *HOSTSERV[MAX_CMD_HASH];
CommandHash *BOTSERV[MAX_CMD_HASH];
CommandHash *MEMOSERV[MAX_CMD_HASH];
CommandHash *NICKSERV[MAX_CMD_HASH];
CommandHash *CHANSERV[MAX_CMD_HASH];
CommandHash *HELPSERV[MAX_CMD_HASH];
CommandHash *OPERSERV[MAX_CMD_HASH];
MessageHash *IRCD[MAX_CMD_HASH];
ModuleHash *MODULE_HASH[MAX_CMD_HASH];

Module *mod_current_module;
const char *mod_current_module_name = NULL;
char *mod_current_buffer = NULL;
User *mod_current_user;
ModuleCallBack *moduleCallBackHead = NULL;


char *ModuleGetErrStr(int status)
{
	const char *module_err_str[] = {
		"Module, Okay - No Error",						/* MOD_ERR_OK */
		"Module Error, Allocating memory",					/* MOD_ERR_MEMORY */
		"Module Error, Not enough parameters",					/* MOD_ERR_PARAMS */
		"Module Error, Already loaded",						/* MOD_ERR_EXISTS */
		"Module Error, File does not exist",					/* MOD_ERR_NOEXIST */
		"Module Error, No User",						/* MOD_ERR_NOUSER */
		"Module Error, Error during load time or module returned MOD_STOP",	/* MOD_ERR_NOLOAD */
		"Module Error, Unable to unload",					/* MOD_ERR_NOUNLOAD */
		"Module Error, Incorrect syntax",					/* MOD_ERR_SYNTAX */
		"Module Error, Unable to delete",					/* MOD_ERR_NODELETE */
		"Module Error, Unknown Error occuried",					/* MOD_ERR_UNKOWN */
		"Module Error, File I/O Error",						/* MOD_ERR_FILE_IO */
		"Module Error, No Service found for request",				/* MOD_ERR_NOSERVICE */
		"Module Error, No module name for request"				/* MOD_ERR_NO_MOD_NAME */
	};
	return const_cast<char *>(module_err_str[status]);
}


/************************************************/

/**
 *
 **/
int encryption_module_init(void) {
	int ret = 0;

	alog("Loading Encryption Module: [%s]", EncModule);
	ret = ModuleManager::LoadModule(EncModule, NULL);
	if (ret == MOD_ERR_OK)
		findModule(EncModule)->SetType(ENCRYPTION);
	mod_current_module = NULL;
	return ret;
}

/**
 * Load the ircd protocol module up
 **/
int protocol_module_init(void)
{
	int ret = 0;

	alog("Loading IRCD Protocol Module: [%s]", IRCDModule);
	ret = ModuleManager::LoadModule(IRCDModule, NULL);

	if (ret == MOD_ERR_OK)
	{
		findModule(IRCDModule)->SetType(PROTOCOL);
		/* This is really NOT the correct place to do config checks, but
		 * as we only have the ircd struct filled here, we have to over
		 * here. -GD
		 */
		if (ircd->ts6)
		{
			if (!Numeric)
			{
				alog("This IRCd protocol requires a server numeric to be set in Anope's configuration.");
				ret = -1;
			}
			else
			{
				ts6_uid_init();
			}
		}
	}

	mod_current_module = NULL;
	return ret;
}

/**
 * Unload ALL loaded modules, no matter what kind of module it is.
 * Do NEVER EVER, and i mean NEVER (and if that isn't clear enough
 * yet, i mean: NEVER AT ALL) call this unless we're shutting down,
 * or we'll fuck up Anope badly (protocol handling won't work for
 * example). If anyone calls this function without a justified need
 * for it, i reserve the right to break their legs in a painful way.
 * And if that isn't enough discouragement, you'll wake up with your
 * both legs broken tomorrow ;) -GD
 */
void modules_unload_all(bool fini, bool unload_proto)
{
	int idx;
	ModuleHash *mh, *next;

	if (!fini)
	{
		/*
		 * XXX: This was used to stop modules from executing destructors, we don't really
		 * support this now, so just return.. dirty. We need to rewrite the code that uses this param.
		 */
		return;
	}

	for (idx = 0; idx < MAX_CMD_HASH; idx++) {
		mh = MODULE_HASH[idx];
		while (mh) {
			next = mh->next;
			if (unload_proto || (mh->m->type != PROTOCOL))
				ModuleManager::UnloadModule(mh->m, NULL);
	   		mh = next;
		}
	}
}

void Module::InsertLanguage(int langNumber, int ac, const char **av)
{
	int i;

	if (debug)
		alog("debug: %s Adding %d texts for language %d", this->name.c_str(), ac, langNumber);

	if (this->lang[langNumber].argc > 0) {
		this->DeleteLanguage(langNumber);
	}

	this->lang[langNumber].argc = ac;
	this->lang[langNumber].argv = new char *[ac];
	for (i = 0; i < ac; i++) {
		this->lang[langNumber].argv[i] = sstrdup(av[i]);
	}
}

/**
 * Search the list of loaded modules for the given name.
 * @param name the name of the module to find
 * @return a pointer to the module found, or NULL
 */
Module *findModule(const char *name)
{
	int idx;
	ModuleHash *current = NULL;
	if (!name) {
		return NULL;
	}
	idx = CMD_HASH(name);

	for (current = MODULE_HASH[idx]; current; current = current->next) {
		if (stricmp(name, current->name) == 0) {
			return current->m;
		}
	}
	return NULL;

}

/*******************************************************************************
 * Command Functions
 *******************************************************************************/
/**
 * Create a Command struct ready for use in anope.
 * @param name the name of the command
 * @param func pointer to the function to execute when command is given
 * @param has_priv pointer to function to check user priv's
 * @param help_all help file index for all users
 * @param help_reg help file index for all regustered users
 * @param help_oper help file index for all opers
 * @param help_admin help file index for all admins
 * @param help_root help file indenx for all services roots
 * @return a "ready to use" Command struct will be returned
 */
Command *createCommand(const char *name, int (*func) (User * u),
					   int (*has_priv) (User * u), int help_all,
					   int help_reg, int help_oper, int help_admin,
					   int help_root)
{
	Command *c;
	if (!name || !*name) {
		return NULL;
	}

	if (!(c = new Command)) {
		fatal("Out of memory!");
	}
	c->name = sstrdup(name);
	c->routine = func;
	c->has_priv = has_priv;
	c->helpmsg_all = help_all;
	c->helpmsg_reg = help_reg;
	c->helpmsg_oper = help_oper;
	c->helpmsg_admin = help_admin;
	c->helpmsg_root = help_root;
	c->help_param1 = NULL;
	c->help_param2 = NULL;
	c->help_param3 = NULL;
	c->help_param4 = NULL;
	c->core = 0;
	c->next = NULL;
	c->mod_name = NULL;
	c->service = NULL;
	c->all_help = NULL;
	c->regular_help = NULL;
	c->oper_help = NULL;
	c->admin_help = NULL;
	c->root_help = NULL;
	return c;
}

/**
 * Destroy a command struct freeing any memory.
 * @param c Command to destroy
 * @return MOD_ERR_OK on success, anything else on fail
 */
int destroyCommand(Command * c)
{
	if (!c) {
		return MOD_ERR_PARAMS;
	}
	if (c->core == 1) {
		return MOD_ERR_UNKNOWN;
	}
	if (c->name) {
		delete [] c->name;
	}
	c->routine = NULL;
	c->has_priv = NULL;
	c->helpmsg_all = -1;
	c->helpmsg_reg = -1;
	c->helpmsg_oper = -1;
	c->helpmsg_admin = -1;
	c->helpmsg_root = -1;
	if (c->mod_name) {
		delete [] c->mod_name;
	}
	if (c->service) {
		delete [] c->service;
	}
	c->next = NULL;
	delete c;
	return MOD_ERR_OK;
}

/** Add a command to a command table. Only for internal use.
 * only add if were unique, pos = 0;
 * if we want it at the "head" of that command, pos = 1
 * at the tail, pos = 2
 * @param cmdTable the table to add the command to
 * @param c the command to add
 * @param pos the position in the cmd call stack to add the command
 * @return MOD_ERR_OK will be returned on success.
 */
static int internal_addCommand(Module *m, CommandHash * cmdTable[], Command * c, int pos)
{
	/* We can assume both param's have been checked by this point.. */
	int index = 0;
	CommandHash *current = NULL;
	CommandHash *newHash = NULL;
	CommandHash *lastHash = NULL;
	Command *tail = NULL;

	if (!cmdTable || !c || (pos < 0 || pos > 2)) {
		return MOD_ERR_PARAMS;
	}

	index = CMD_HASH(c->name);

	for (current = cmdTable[index]; current; current = current->next) {
		if ((c->service) && (current->c) && (current->c->service)
			&& (!strcmp(c->service, current->c->service) == 0)) {
			continue;
		}
		if ((stricmp(c->name, current->name) == 0)) {   /* the cmd exist's we are a addHead */
			if (pos == 1) {
				c->next = current->c;
				current->c = c;
				if (debug)
					alog("debug: existing cmd: (0x%p), new cmd (0x%p)",
						 static_cast<void *>(c->next), static_cast<void *>(c));
				return MOD_ERR_OK;
			} else if (pos == 2) {

				tail = current->c;
				while (tail->next)
					tail = tail->next;
				if (debug)
					alog("debug: existing cmd: (0x%p), new cmd (0x%p)",
						 static_cast<void *>(tail), static_cast<void *>(c));
				tail->next = c;
				c->next = NULL;

				return MOD_ERR_OK;
			} else
				return MOD_ERR_EXISTS;
		}
		lastHash = current;
	}

	if (!(newHash = new CommandHash)) {
		fatal("Out of memory");
	}
	newHash->next = NULL;
	newHash->name = sstrdup(c->name);
	newHash->c = c;

	if (lastHash == NULL)
		cmdTable[index] = newHash;
	else
		lastHash->next = newHash;

	return MOD_ERR_OK;
}

int Module::AddCommand(CommandHash * cmdTable[], Command * c, int pos)
{
	int status;

	if (!cmdTable || !c) {
		return MOD_ERR_PARAMS;
	}
	c->core = 0;
	if (!c->mod_name) {
		c->mod_name = sstrdup(this->name.c_str());
	}


	if (cmdTable == HOSTSERV) {
		if (s_HostServ) {
			c->service = sstrdup(s_HostServ);
		} else {
			return MOD_ERR_NOSERVICE;
		}
	} else if (cmdTable == BOTSERV) {
		if (s_BotServ) {
			c->service = sstrdup(s_BotServ);
		} else {
			return MOD_ERR_NOSERVICE;
		}
	} else if (cmdTable == MEMOSERV) {
		if (s_MemoServ) {
			c->service = sstrdup(s_MemoServ);
		} else {
			return MOD_ERR_NOSERVICE;
		}
	} else if (cmdTable == CHANSERV) {
		if (s_ChanServ) {
			c->service = sstrdup(s_ChanServ);
		} else {
			return MOD_ERR_NOSERVICE;
		}
	} else if (cmdTable == NICKSERV) {
		if (s_NickServ) {
			c->service = sstrdup(s_NickServ);
		} else {
			return MOD_ERR_NOSERVICE;
		}
	} else if (cmdTable == HELPSERV) {
		if (s_HelpServ) {
			c->service = sstrdup(s_HelpServ);
		} else {
			return MOD_ERR_NOSERVICE;
		}
	} else if (cmdTable == OPERSERV) {
		if (s_OperServ) {
			c->service = sstrdup(s_OperServ);
		} else {
			return MOD_ERR_NOSERVICE;
		}
	} else
		c->service = sstrdup("Unknown");

	status = internal_addCommand(this, cmdTable, c, pos);
	if (status != MOD_ERR_OK)
	{
		alog("ERROR! [%d]", status);
	}
	return status;
}


/** Remove a command from the command hash. Only for internal use.
 * @param cmdTable the command table to remove the command from
 * @param c the command to remove
 * @param mod_name the name of the module who owns the command
 * @return MOD_ERR_OK will be returned on success
 */
static int internal_delCommand(CommandHash * cmdTable[], Command * c, const char *mod_name)
{
	int index = 0;
	CommandHash *current = NULL;
	CommandHash *lastHash = NULL;
	Command *tail = NULL, *last = NULL;

	if (!c || !cmdTable) {
		return MOD_ERR_PARAMS;
	}

	index = CMD_HASH(c->name);
	for (current = cmdTable[index]; current; current = current->next) {
		if (stricmp(c->name, current->name) == 0) {
			if (!lastHash) {
				tail = current->c;
				if (tail->next) {
					while (tail) {
						if (mod_name && tail->mod_name
							&& (stricmp(mod_name, tail->mod_name) == 0)) {
							if (last) {
								last->next = tail->next;
							} else {
								current->c = tail->next;
							}
							return MOD_ERR_OK;
						}
						last = tail;
						tail = tail->next;
					}
				} else {
					cmdTable[index] = current->next;
					delete [] current->name;
					return MOD_ERR_OK;
				}
			} else {
				tail = current->c;
				if (tail->next) {
					while (tail) {
						if (mod_name && tail->mod_name
							&& (stricmp(mod_name, tail->mod_name) == 0)) {
							if (last) {
								last->next = tail->next;
							} else {
								current->c = tail->next;
							}
							return MOD_ERR_OK;
						}
						last = tail;
						tail = tail->next;
					}
				} else {
					lastHash->next = current->next;
					delete [] current->name;
					return MOD_ERR_OK;
				}
			}
		}
		lastHash = current;
	}
	return MOD_ERR_NOEXIST;
}

/**
 * Delete a command from the service given.
 * @param cmdTable the cmdTable for the services to remove the command from
 * @param name the name of the command to delete from the service
 * @return returns MOD_ERR_OK on success
 */
int Module::DelCommand(CommandHash * cmdTable[], const char *dname)
{
	Command *c = NULL;
	Command *cmd = NULL;
	int status = 0;

	c = findCommand(cmdTable, dname);
	if (!c) {
		return MOD_ERR_NOEXIST;
	}


	for (cmd = c; cmd; cmd = cmd->next)
	{
		if (cmd->mod_name && cmd->mod_name == this->name)
		{
			status = internal_delCommand(cmdTable, cmd, this->name.c_str());
		}
	}
	return status;
}

/**
 * Search the command table gieven for a command.
 * @param cmdTable the name of the command table to search
 * @param name the name of the command to look for
 * @return returns a pointer to the found command struct, or NULL
 */
Command *findCommand(CommandHash * cmdTable[], const char *name)
{
	int idx;
	CommandHash *current = NULL;
	if (!cmdTable || !name) {
		return NULL;
	}

	idx = CMD_HASH(name);

	for (current = cmdTable[idx]; current; current = current->next) {
		if (stricmp(name, current->name) == 0) {
			return current->c;
		}
	}
	return NULL;
}

/*******************************************************************************
 * Message Functions
 *******************************************************************************/

 /**
  * Create a new Message struct.
  * @param name the name of the message
  * @param func a pointer to the function to call when we recive this message
  * @return a new Message object
  **/
Message *createMessage(const char *name,
					   int (*func) (const char *source, int ac, const char **av))
{
	Message *m = NULL;
	if (!name || !func) {
		return NULL;
	}
	if (!(m = new Message)) {
		fatal("Out of memory!");
	}
	m->name = sstrdup(name);
	m->func = func;
	m->core = 0;
	m->next = NULL;
	return m;
}

/**
 * find a message in the given table.
 * Looks up the message <name> in the MessageHash given
 * @param MessageHash the message table to search for this command, will almost always be IRCD
 * @param name the name of the command were looking for
 * @return NULL if we cant find it, or a pointer to the Message if we can
 **/
Message *findMessage(MessageHash * msgTable[], const char *name)
{
	int idx;
	MessageHash *current = NULL;
	if (!msgTable || !name) {
		return NULL;
	}
	idx = CMD_HASH(name);

	for (current = msgTable[idx]; current; current = current->next) {
		if (stricmp(name, current->name) == 0) {
			return current->m;
		}
	}
	return NULL;
}

/**
 * Add a message to the MessageHash.
 * @param msgTable the MessageHash we want to add a message to
 * @param m the Message we want to add
 * @param pos the position we want to add the message to, E.G. MOD_HEAD, MOD_TAIL, MOD_UNIQUE
 * @return MOD_ERR_OK on a successful add.
 **/

int addMessage(MessageHash * msgTable[], Message * m, int pos)
{
	/* We can assume both param's have been checked by this point.. */
	int index = 0;
	MessageHash *current = NULL;
	MessageHash *newHash = NULL;
	MessageHash *lastHash = NULL;
	Message *tail = NULL;
	int match = 0;

	if (!msgTable || !m || (pos < 0 || pos > 2)) {
		return MOD_ERR_PARAMS;
	}

	index = CMD_HASH(m->name);

	for (current = msgTable[index]; current; current = current->next) {
		match = stricmp(m->name, current->name);
		if (match == 0) {	   /* the msg exist's we are a addHead */
			if (pos == 1) {
				m->next = current->m;
				current->m = m;
				if (debug)
					alog("debug: existing msg: (0x%p), new msg (0x%p)",
						 static_cast<void *>(m->next), static_cast<void *>(m));
				return MOD_ERR_OK;
			} else if (pos == 2) {
				tail = current->m;
				while (tail->next)
					tail = tail->next;
				if (debug)
					alog("debug: existing msg: (0x%p), new msg (0x%p)",
						 static_cast<void *>(tail), static_cast<void *>(m));
				tail->next = m;
				m->next = NULL;
				return MOD_ERR_OK;
			} else
				return MOD_ERR_EXISTS;
		}
		lastHash = current;
	}

	if (!(newHash = new MessageHash)) {
		fatal("Out of memory");
	}
	newHash->next = NULL;
	newHash->name = sstrdup(m->name);
	newHash->m = m;

	if (lastHash == NULL)
		msgTable[index] = newHash;
	else
		lastHash->next = newHash;
	return MOD_ERR_OK;
}

/**
 * Add the given message (m) to the MessageHash marking it as a core command
 * @param msgTable the MessageHash we want to add to
 * @param m the Message we are adding
 * @return MOD_ERR_OK on a successful add.
 **/
int addCoreMessage(MessageHash * msgTable[], Message * m)
{
	if (!msgTable || !m) {
		return MOD_ERR_PARAMS;
	}
	m->core = 1;
	return addMessage(msgTable, m, 0);
}

/**
 * remove the given message from the given message hash, for the given module
 * @param msgTable which MessageHash we are removing from
 * @param m the Message we want to remove
 * @return MOD_ERR_OK on success, althing else on fail.
 **/
int delMessage(MessageHash * msgTable[], Message * m)
{
	int index = 0;
	MessageHash *current = NULL;
	MessageHash *lastHash = NULL;
	Message *tail = NULL, *last = NULL;

	if (!m || !msgTable) {
		return MOD_ERR_PARAMS;
	}

	index = CMD_HASH(m->name);

	for (current = msgTable[index]; current; current = current->next) {
		if (stricmp(m->name, current->name) == 0) {
			if (!lastHash) {
				tail = current->m;
				if (tail->next) {
					while (tail) {
						if (last) {
							last->next = tail->next;
						} else {
							current->m = tail->next;
						}
						return MOD_ERR_OK;
						last = tail;
						tail = tail->next;
					}
				} else {
					msgTable[index] = current->next;
					delete [] current->name;
					return MOD_ERR_OK;
				}
			} else {
				tail = current->m;
				if (tail->next) {
					while (tail) {
						if (last) {
							last->next = tail->next;
						} else {
							current->m = tail->next;
						}
						return MOD_ERR_OK;
						last = tail;
						tail = tail->next;
					}
				} else {
					lastHash->next = current->next;
					delete [] current->name;
					return MOD_ERR_OK;
				}
			}
		}
		lastHash = current;
	}
	return MOD_ERR_NOEXIST;
}

/**
 * Destory a message, freeing its memory.
 * @param m the message to be destroyed
 * @return MOD_ERR_SUCCESS on success
 **/
int destroyMessage(Message * m)
{
	if (!m) {
		return MOD_ERR_PARAMS;
	}
	if (m->name) {
		delete [] m->name;
	}
	m->func = NULL;
	m->next = NULL;
	return MOD_ERR_OK;
}

/*******************************************************************************
 * Module Callback Functions
 *******************************************************************************/

int Module::AddCallback(const char *name, time_t when,
					  int (*func) (int argc, char *argv[]), int argc,
					  char **argv)
{
	ModuleCallBack *newcb, *tmp, *prev;
	int i;
	newcb = new ModuleCallBack;
	if (!newcb)
		return MOD_ERR_MEMORY;

	if (name)
		newcb->name = sstrdup(name);
	else
		newcb->name = NULL;
	newcb->when = when;
	newcb->owner_name = sstrdup(this->name.c_str());
	newcb->func = func;
	newcb->argc = argc;
	newcb->argv = new char *[argc];
	for (i = 0; i < argc; i++) {
		newcb->argv[i] = sstrdup(argv[i]);
	}
	newcb->next = NULL;

	if (moduleCallBackHead == NULL) {
		moduleCallBackHead = newcb;
	} else {					/* find place in list */
		tmp = moduleCallBackHead;
		prev = tmp;
		if (newcb->when < tmp->when) {
			newcb->next = tmp;
			moduleCallBackHead = newcb;
		} else {
			while (tmp && newcb->when >= tmp->when) {
				prev = tmp;
				tmp = tmp->next;
			}
			prev->next = newcb;
			newcb->next = tmp;
		}
	}
	if (debug)
		alog("debug: added module CallBack: [%s] due to execute at %ld",
			 newcb->name ? newcb->name : "?", static_cast<long>(newcb->when));
	return MOD_ERR_OK;
}

/**
 * Removes a entry from the modules callback list
 * @param prev a pointer to the previous entry in the list, NULL for the head
 **/
void moduleCallBackDeleteEntry(ModuleCallBack * prev)
{
	ModuleCallBack *tmp = NULL;
	int i;
	if (prev == NULL) {
		tmp = moduleCallBackHead;
		moduleCallBackHead = tmp->next;
	} else {
		tmp = prev->next;
		prev->next = tmp->next;
	}
	if (tmp->name)
		delete [] tmp->name;
	if (tmp->owner_name)
		delete [] tmp->owner_name;
	tmp->func = NULL;
	for (i = 0; i < tmp->argc; i++) {
		delete [] tmp->argv[i];
	}
	delete [] tmp->argv;
	tmp->argc = 0;
	tmp->next = NULL;
	delete tmp;
}

/**
 * Search the module callback list for a given module
 * @param mod_name the name of the module were looking for
 * @param found have we found it?
 * @return a pointer to the ModuleCallBack struct or NULL - dont forget to check the found paramter!
 **/
static ModuleCallBack *moduleCallBackFindEntry(const char *mod_name, bool * found)
{
	ModuleCallBack *prev = NULL, *current = NULL;
	*found = false;
	current = moduleCallBackHead;
	while (current != NULL) {
		if (current->owner_name
			&& (strcmp(mod_name, current->owner_name) == 0)) {
			*found = true;
			break;
		} else {
			prev = current;
			current = current->next;
		}
	}
	if (current == moduleCallBackHead) {
		return NULL;
	} else {
		return prev;
	}
}

void Module::DelCallback(const char *name)
{
	ModuleCallBack *current = NULL;
	ModuleCallBack *prev = NULL, *tmp = NULL;
	int del = 0;
	if (!name) {
		return;
	}
	current = moduleCallBackHead;
	while (current) {
		if ((current->owner_name) && (current->name)) {
			if ((strcmp(this->name.c_str(), current->owner_name) == 0)
				&& (strcmp(current->name, name) == 0)) {
				if (debug) {
					alog("debug: removing CallBack %s for module %s", name,
						 this->name.c_str());
				}
				tmp = current->next;	/* get a pointer to the next record, as once we delete this record, we'll lose it :) */
				moduleCallBackDeleteEntry(prev);		/* delete this record */
				del = 1;		/* set the record deleted flag */
			}
		}
		if (del == 1) {		 /* if a record was deleted */
			current = tmp;	  /* use the value we stored in temp */
			tmp = NULL;		 /* clear it for next time */
			del = 0;			/* reset the flag */
		} else {
			prev = current;	 /* just carry on as normal */
			current = current->next;
		}
	}
}

/**
 * Remove all outstanding module callbacks for the given module.
 * When a module is unloaded, any callbacks it had outstanding must be removed, else when they attempt to execute the func pointer will no longer be valid, and we'll seg.
 * @param mod_name the name of the module we are preping for unload
 **/
void moduleCallBackPrepForUnload(const char *mod_name)
{
	bool found = false;
	ModuleCallBack *tmp = NULL;

	tmp = moduleCallBackFindEntry(mod_name, &found);
	while (found) {
		if (debug) {
			alog("debug: removing CallBack for module %s", mod_name);
		}
		moduleCallBackDeleteEntry(tmp);
		tmp = moduleCallBackFindEntry(mod_name, &found);
	}
}

/**
 * Return a copy of the complete last buffer.
 * This is needed for modules who cant trust the strtok() buffer, as we dont know who will have already messed about with it.
 * @reutrn a pointer to a copy of the last buffer - DONT mess with this, copy if first if you must do things to it.
 **/
char *moduleGetLastBuffer(void)
{
	char *tmp = NULL;
	if (mod_current_buffer) {
		tmp = strchr(mod_current_buffer, ' ');
		if (tmp) {
			tmp++;
		}
	}
	return tmp;
}

/*******************************************************************************
 * Module HELP Functions
 *******************************************************************************/
 /**
  * Add help for Root admins.
  * @param c the Command to add help for
  * @param func the function to run when this help is asked for
  **/
int moduleAddRootHelp(Command * c, int (*func) (User * u))
{
	if (c) {
		c->root_help = func;
		return MOD_STOP;
	}
	return MOD_CONT;
}

 /**
  * Add help for Admins.
  * @param c the Command to add help for
  * @param func the function to run when this help is asked for
  **/
int moduleAddAdminHelp(Command * c, int (*func) (User * u))
{
	if (c) {
		c->admin_help = func;
		return MOD_STOP;
	}
	return MOD_CONT;
}

 /**
  * Add help for opers..
  * @param c the Command to add help for
  * @param func the function to run when this help is asked for
  **/
int moduleAddOperHelp(Command * c, int (*func) (User * u))
{
	if (c) {
		c->oper_help = func;
		return MOD_STOP;
	}
	return MOD_CONT;
}

/**
  * Add help for registered users
  * @param c the Command to add help for
  * @param func the function to run when this help is asked for
  **/
int moduleAddRegHelp(Command * c, int (*func) (User * u))
{
	if (c) {
		c->regular_help = func;
		return MOD_STOP;
	}
	return MOD_CONT;
}

/**
  * Add help for all users
  * @param c the Command to add help for
  * @param func the function to run when this help is asked for
  **/
int moduleAddHelp(Command * c, int (*func) (User * u))
{
	if (c) {
		c->all_help = func;
		return MOD_STOP;
	}
	return MOD_CONT;
}

void Module::SetNickHelp(void (*func)(User *))
{
	this->nickHelp = func;
}

void Module::SetChanHelp(void (*func)(User *))
{
	this->chanHelp = func;
}

void Module::SetMemoHelp(void (*func)(User *))
{
	this->memoHelp = func;
}

void Module::SetBotHelp(void (*func)(User *))
{
	this->botHelp = func;
}

void Module::SetOperHelp(void (*func)(User *))
{
	this->operHelp = func;
}

void Module::SetHostHelp(void (*func)(User *))
{
	this->hostHelp = func;
}

void Module::SetHelpHelp(void (*func)(User *))
{
	this->helpHelp = func;
}

/**
 * Display any extra module help for the given service.
 * @param services which services is help being dispalyed for?
 * @param u which user is requesting the help
 **/
void moduleDisplayHelp(int service, User * u)
{
	int idx;
	ModuleHash *current = NULL;

	for (idx = 0; idx != MAX_CMD_HASH; idx++) {
		for (current = MODULE_HASH[idx]; current; current = current->next) {
			if ((service == 1) && current->m->nickHelp) {
				current->m->nickHelp(u);
			} else if ((service == 2) && current->m->chanHelp) {
				current->m->chanHelp(u);
			} else if ((service == 3) && current->m->memoHelp) {
				current->m->memoHelp(u);
			} else if ((service == 4) && current->m->botHelp) {
				current->m->botHelp(u);
			} else if ((service == 5) && current->m->operHelp) {
				current->m->operHelp(u);
			} else if ((service == 6) && current->m->hostHelp) {
				current->m->hostHelp(u);
			} else if ((service == 7) && current->m->helpHelp) {
				current->m->helpHelp(u);
			}
		}
	}
}

/**
 * Check the current version of anope against a given version number
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
	if (VERSION_MAJOR > major) {		/* Def. new */
		ret = true;
	} else if (VERSION_MAJOR == major) {		/* Might be newer */
		if (minor == -1) {
			return true;
		}					   /* They dont care about minor */
		if (VERSION_MINOR > minor) {	/* Def. newer */
			ret = true;
		} else if (VERSION_MINOR == minor) {	/* Might be newer */
			if (patch == -1) {
				return true;
			}				   /* They dont care about patch */
			if (VERSION_PATCH > patch) {
				ret = true;
			} else if (VERSION_PATCH == patch) {
#if 0
// XXX
				if (build == -1) {
					return true;
				}			   /* They dont care about build */
				if (VERSION_BUILD >= build) {
					ret = true;
				}
#endif
			}
		}
	}
	return ret;
}

#ifdef _WIN32
const char *ano_moderr(void)
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

/**
 * Allow ircd protocol files to update the protect level info tables.
 **/
void updateProtectDetails(const char *level_info_protect_word,
						  const char *level_info_protectme_word,
						  const char *fant_protect_add, const char *fant_protect_del,
						  const char *level_protect_word, const char *protect_set_mode,
						  const char *protect_unset_mode)
{
	int i = 0;
	CSModeUtil ptr;
	LevelInfo l_ptr;

	ptr = csmodeutils[i];
	while (ptr.name) {
		if (strcmp(ptr.name, "PROTECT") == 0) {
			csmodeutils[i].bsname = sstrdup(fant_protect_add);
			csmodeutils[i].mode = sstrdup(protect_set_mode);
		} else if (strcmp(ptr.name, "DEPROTECT") == 0) {
			csmodeutils[i].bsname = sstrdup(fant_protect_del);
			csmodeutils[i].mode = sstrdup(protect_unset_mode);
		}
		ptr = csmodeutils[++i];
	}

	i = 0;
	l_ptr = levelinfo[i];
	while (l_ptr.what != -1) {
		if (l_ptr.what == CA_PROTECT) {
			levelinfo[i].name = sstrdup(level_info_protect_word);
		} else if (l_ptr.what == CA_PROTECTME) {
			levelinfo[i].name = sstrdup(level_info_protectme_word);
		} else if (l_ptr.what == CA_AUTOPROTECT) {
			levelinfo[i].name = sstrdup(level_protect_word);
		}
		l_ptr = levelinfo[++i];
	}
}

void Module::NoticeLang(char *source, User * u, int number, ...)
{
	va_list va;
	char buffer[4096], outbuf[4096];
	char *fmt = NULL;
	int lang = NSDefLanguage;
	char *s, *t, *buf;

	/* Find the users lang, and use it if we can */
	if (u && u->na && u->na->nc) {
		lang = u->na->nc->language;
	}

	/* If the users lang isnt supported, drop back to English */
	if (this->lang[lang].argc == 0) {
		lang = LANG_EN_US;
	}

	/* If the requested lang string exists for the language */
	if (this->lang[lang].argc > number) {
		fmt = this->lang[lang].argv[number];

		buf = sstrdup(fmt);
		va_start(va, number);
		vsnprintf(buffer, 4095, buf, va);
		va_end(va);
		s = buffer;
		while (*s) {
			t = s;
			s += strcspn(s, "\n");
			if (*s)
				*s++ = '\0';
			strscpy(outbuf, t, sizeof(outbuf));
			notice_user(source, u, "%s", outbuf);
		}
		delete [] buf;
	} else {
		alog("%s: INVALID language string call, language: [%d], String [%d]", this->name.c_str(), lang, number);
	}
}

const char *Module::GetLangString(User * u, int number)
{
	int lang = NSDefLanguage;

	/* Find the users lang, and use it if we can */
	if (u && u->na && u->na->nc)
		lang = u->na->nc->language;

	/* If the users lang isnt supported, drop back to English */
	if (this->lang[lang].argc == 0)
		lang = LANG_EN_US;

	/* If the requested lang string exists for the language */
	if (this->lang[lang].argc > number) {
		return this->lang[lang].argv[number];
	/* Return an empty string otherwise, because we might be used without
	 * the return value being checked. If we would return NULL, bad things
	 * would happen!
	 */
	} else {
		alog("%s: INVALID language string call, language: [%d], String [%d]", this->name.c_str(), lang, number);
		return "";
	}
}

void Module::DeleteLanguage(int langNumber)
{
	if (this->lang[langNumber].argc)
	{
		for (int idx = 0; idx > this->lang[langNumber].argc; idx++)
			delete [] this->lang[langNumber].argv[idx];
		delete [] this->lang[langNumber].argv;
		this->lang[langNumber].argc = 0;
	}
}

void ModuleRunTimeDirCleanUp(void)
{
#ifndef _WIN32
	DIR *dirp;
	struct dirent *dp;
#else
	BOOL fFinished;
	HANDLE hList;
	TCHAR szDir[MAX_PATH + 1];
	TCHAR szSubDir[MAX_PATH + 1];
	WIN32_FIND_DATA FileData;
	char buffer[_MAX_PATH];
#endif
	char dirbuf[BUFSIZE];
	char filebuf[BUFSIZE];


#ifndef _WIN32
	snprintf(dirbuf, BUFSIZE, "%s/modules/runtime", services_dir);
#else
	snprintf(dirbuf, BUFSIZE, "\\%s", "modules/runtime");
#endif

	if (debug) {
		alog("debug: Cleaning out Module run time directory (%s) - this may take a moment please wait", dirbuf);
	}

#ifndef _WIN32
	if ((dirp = opendir(dirbuf)) == NULL) {
		if (debug) {
			alog("debug: cannot open directory (%s)", dirbuf);
		}
		return;
	}
	while ((dp = readdir(dirp)) != NULL) {
		if (dp->d_ino == 0) {
			continue;
		}
		if (!stricmp(dp->d_name, ".") || !stricmp(dp->d_name, "..")) {
			continue;
		}
		snprintf(filebuf, BUFSIZE, "%s/%s", dirbuf, dp->d_name);
		unlink(filebuf);
	}
	closedir(dirp);
#else
	/* Get the current working directory: */
	if (_getcwd(buffer, _MAX_PATH) == NULL) {
		if (debug) {
			alog("debug: Unable to set Current working directory");
		}
	}
	snprintf(szDir, sizeof(szDir), "%s\\%s\\*", buffer, dirbuf);

	hList = FindFirstFile(szDir, &FileData);
	if (hList != INVALID_HANDLE_VALUE) {
		fFinished = FALSE;
		while (!fFinished) {
			if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				snprintf(filebuf, BUFSIZE, "%s/%s", dirbuf, FileData.cFileName);
				DeleteFile(filebuf);
			}
			if (!FindNextFile(hList, &FileData)) {
				if (GetLastError() == ERROR_NO_MORE_FILES) {
					fFinished = TRUE;
				}
			}
		}
	} else {
		if (debug) {
			alog("debug: Invalid File Handle. GetLastError reports %d\n", GetLastError());
		}
	}
	FindClose(hList);
#endif
	if (debug) {
		alog("debug: Module run time directory has been cleaned out");
	}
}

/**
 * Execute a stored call back
 **/
void ModuleManager::RunCallbacks()
{
	ModuleCallBack *tmp;

	while ((tmp = moduleCallBackHead) && (tmp->when <= time(NULL))) {
		if (debug)
			alog("debug: executing callback: %s", tmp->name ? tmp->name : "<unknown>");
		if (tmp->func) {
			mod_current_module_name = tmp->owner_name;
			tmp->func(tmp->argc, tmp->argv);
			mod_current_module = NULL;
			moduleCallBackDeleteEntry(NULL);
		}
	}
}

/* EOF */

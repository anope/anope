/* Events functions.
 *
 * (C) 2004-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 *
 */

#include "modules.h"
#include "language.h"
#include "version.h"

EvtHookHash *EVENTHOOKS[MAX_CMD_HASH];

EvtHook *find_eventhook(const char *name)
{
	EvtHook *m;
	m = findEventHook(EVENTHOOKS, name);
	return m;
}

void send_event(const char *name, int argc, ...)
{
	va_list va;
	char *a;
	int idx = 0;
	char **argv;

	argv = new char *[argc];
	va_start(va, argc);
	for (idx = 0; idx < argc; idx++) {
		a = va_arg(va, char *);
		argv[idx] = sstrdup(a);
	}
	va_end(va);

	if (debug)
		alog("debug: Emitting event \"%s\" (%d args)", name, argc);

	event_process_hook(name, argc, argv);

	/**
	 * Now that the events have seen the message, free it up
	 **/
	for (idx = 0; idx < argc; idx++) {
		delete [] argv[idx];
	}
	delete [] argv;
}

void event_process_hook(const char *name, int argc, char **argv)
{
	int retVal = 0;
	EvtHook *current = NULL;
	EvtHook *evh;

	/* Do something with the message. */
	evh = find_eventhook(name);
	if (evh) {
		if (evh->func) {
			retVal = evh->func(argc, argv);
			if (retVal == MOD_CONT) {
				current = evh->next;
				while (current && current->func && retVal == MOD_CONT) {
					retVal = current->func(argc, argv);
					current = current->next;
				}
			}
		}
	}
}

/**
 * Displays a message list for a given message.
 * Again this is of little use other than debugging.
 * @param m the message to display
 * @return 0 is returned and has no meaning
 */
int displayEventHook(EvtHook * evh)
{
	EvtHook *msg = NULL;
	int i = 0;
	alog("Displaying message list for %s", evh->name);
	for (msg = evh; msg; msg = msg->next) {
		alog("%d: 0x%p", ++i, static_cast<void *>(msg));
	}
	alog("end");
	return 0;
}

/**
 * Display the message call stak.
 * Prints the call stack for a message based on the message name, again useful for debugging and little lese :)
 * @param name the name of the message to print info for
 * @return the return int has no relevence atm :)
 */
int displayHookFromHash(char *name)
{
	EvtHookHash *current = NULL;
	int index = 0;
	index = CMD_HASH(name);
	if (debug > 1) {
		alog("debug: trying to display message %s", name);
	}
	for (current = EVENTHOOKS[index]; current; current = current->next) {
		if (stricmp(name, current->name) == 0) {
			displayEventHook(current->evh);
		}
	}
	if (debug > 1) {
		alog("debug: done displaying message %s", name);
	}
	return 0;
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
EvtHook *createEventHook(const char *name, int (*func) (int argc, char **argv))
{
	EvtHook *evh = NULL;
	if (!func) {
		return NULL;
	}
	if (!(evh = new EvtHook)) {
		fatal("Out of memory!");
	}
	evh->name = sstrdup(name);
	evh->func = func;
	evh->mod_name = NULL;
	evh->next = NULL;
	return evh;
}

/**
 * find a message in the given table.
 * Looks up the message <name> in the MessageHash given
 * @param MessageHash the message table to search for this command, will almost always be IRCD
 * @param name the name of the command were looking for
 * @return NULL if we cant find it, or a pointer to the Message if we can
 **/
EvtHook *findEventHook(EvtHookHash * hookEvtTable[], const char *name)
{
	int idx;
	EvtHookHash *current = NULL;
	if (!hookEvtTable || !name) {
		return NULL;
	}
	idx = CMD_HASH(name);

	for (current = hookEvtTable[idx]; current; current = current->next) {
		if (stricmp(name, current->name) == 0) {
			return current->evh;
		}
	}
	return NULL;
}

/**
 * Add a message to the MessageHash.
 * @param msgTable the MessageHash we want to add a message to
 * @param m the Message we want to add
 * @return MOD_ERR_OK on a successful add.
 **/
int addEventHook(EvtHookHash * hookEvtTable[], EvtHook * evh)
{
	/* We can assume both param's have been checked by this point.. */
	int index = 0;
	EvtHookHash *current = NULL;
	EvtHookHash *newHash = NULL;
	EvtHookHash *lastHash = NULL;

	if (!hookEvtTable || !evh) {
		return MOD_ERR_PARAMS;
	}

	index = CMD_HASH(evh->name);

	for (current = hookEvtTable[index]; current; current = current->next) {
		if (stricmp(evh->name, current->name) == 0) {   /* the msg exist's we are a addHead */
			evh->next = current->evh;
			current->evh = evh;
			if (debug)
				alog("debug: existing msg: (0x%p), new msg (0x%p)",
					 static_cast<void *>(evh->next), static_cast<void *>(evh));
			return MOD_ERR_OK;
		}
		lastHash = current;
	}

	if (!(newHash = new EvtHookHash)) {
		fatal("Out of memory");
	}
	newHash->next = NULL;
	newHash->name = sstrdup(evh->name);
	newHash->evh = evh;

	if (lastHash == NULL)
		hookEvtTable[index] = newHash;
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
int addCoreEventHook(EvtHookHash * hookEvtTable[], EvtHook * evh)
{
	if (!hookEvtTable || !evh) {
		return MOD_ERR_PARAMS;
	}
	evh->core = 1;
	return addEventHook(hookEvtTable, evh);
}

int Module::AddEventHook(EvtHook *evh)
{
	int status;

	if (!evh)
		return MOD_ERR_PARAMS;

	evh->core = 0;
	if (!evh->mod_name)
		evh->mod_name = sstrdup(this->name.c_str());

	status = addEventHook(EVENTHOOKS, evh);
	if (debug)
		displayHookFromHash(evh->name);

	return status;
}

int Module::DelEventHook(const char *sname)
{
	EvtHook *evh;
	int status;

	evh = findEventHook(EVENTHOOKS, sname);
	if (!evh) {
		return MOD_ERR_NOEXIST;
	}

	status = delEventHook(EVENTHOOKS, evh, this->name.c_str());
	if (debug) {
		displayHookFromHash(evh->name);
	}
	return status;
}


/**
 * remove the given message from the given message hash, for the given module
 * @param msgTable which MessageHash we are removing from
 * @param m the Message we want to remove
 * @mod_name the name of the module we are removing
 * @return MOD_ERR_OK on success, althing else on fail.
 **/
int delEventHook(EvtHookHash * hookEvtTable[], EvtHook * evh,
				 const char *mod_name)
{
	int index = 0;
	EvtHookHash *current = NULL;
	EvtHookHash *lastHash = NULL;
	EvtHook *tail = NULL, *last = NULL;

	if (!evh || !hookEvtTable) {
		return MOD_ERR_PARAMS;
	}

	index = CMD_HASH(evh->name);

	for (current = hookEvtTable[index]; current; current = current->next) {
		if (stricmp(evh->name, current->name) == 0) {
			if (!lastHash) {
				tail = current->evh;
				if (tail->next) {
					while (tail) {
						if (mod_name && tail->mod_name
							&& (stricmp(mod_name, tail->mod_name) == 0)) {
							if (last) {
								last->next = tail->next;
							} else {
								current->evh = tail->next;
							}
							return MOD_ERR_OK;
						}
						last = tail;
						tail = tail->next;
					}
				} else {
					hookEvtTable[index] = current->next;
					delete [] current->name;
					return MOD_ERR_OK;
				}
			} else {
				tail = current->evh;
				if (tail->next) {
					while (tail) {
						if (mod_name && tail->mod_name
							&& (stricmp(mod_name, tail->mod_name) == 0)) {
							if (last) {
								last->next = tail->next;
							} else {
								current->evh = tail->next;
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
 * Destory a message, freeing its memory.
 * @param m the message to be destroyed
 * @return MOD_ERR_SUCCESS on success
 **/
int destroyEventHook(EvtHook * evh)
{
	if (!evh) {
		return MOD_ERR_PARAMS;
	}
	if (evh->name) {
		delete [] evh->name;
	}
	evh->func = NULL;
	if (evh->mod_name) {
		delete [] evh->mod_name;
	}
	evh->next = NULL;
	return MOD_ERR_OK;
}

/* Events functions.
 *
 * (C) 2004-2011 Anope Team
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

char *mod_current_evtbuffer = NULL;

EvtMessageHash *EVENT[MAX_CMD_HASH];
EvtHookHash *EVENTHOOKS[MAX_CMD_HASH];

EvtMessage *find_event(const char *name)
{
    EvtMessage *m;
    m = findEventHandler(EVENT, name);
    return m;
}

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

    argv = (char **) malloc(sizeof(char *) * argc);
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
        free(argv[idx]);
    }
    free(argv);
}

void eventprintf(char *fmt, ...)
{
    va_list args;
    char buf[16384];            /* Really huge, to try and avoid truncation */
    char *event;

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    event = sstrdup(buf);
    event_message_process(event);
    va_end(args);
    if (event) {
        free(event);
    }
    return;
}

void event_message_process(char *eventbuf)
{
    int retVal = 0;
    EvtMessage *current = NULL;
    char source[64];
    char cmd[64];
    char buf[512];              /* Longest legal IRC command line */
    char *s;
    int ac;                     /* Parameters for the command */
    char **av;
    EvtMessage *evm;

    /* zero out the buffers before we do much else */
    *buf = '\0';
    *source = '\0';
    *cmd = '\0';

    strscpy(buf, eventbuf, sizeof(buf));

    doCleanBuffer((char *) buf);

    /* Split the buffer into pieces. */
    if (*buf == ':') {
        s = strpbrk(buf, " ");
        if (!s)
            return;
        *s = 0;
        while (isspace(*++s));
        strscpy(source, buf + 1, sizeof(source));
        memmove(buf, s, strlen(s) + 1);
    } else {
        *source = 0;
    }
    if (!*buf)
        return;
    s = strpbrk(buf, " ");
    if (s) {
        *s = 0;
        while (isspace(*++s));
    } else
        s = buf + strlen(buf);
    strscpy(cmd, buf, sizeof(cmd));
    ac = split_buf(s, &av, 1);

    /* Do something with the message. */
    evm = find_event(cmd);
    if (evm) {
        char *mod_current_module_name_save = mod_current_module_name;
        Module *mod_current_module_save = mod_current_module;
        if (evm->func) {
            mod_current_module_name = evm->mod_name;
            mod_current_module = findModule(evm->mod_name);
            retVal = evm->func(source, ac, av);
            if (retVal == MOD_CONT) {
                current = evm->next;
                while (current && current->func && retVal == MOD_CONT) {
                    mod_current_module_name = current->mod_name;
                    mod_current_module = findModule(current->mod_name);
                    retVal = current->func(source, ac, av);
                    current = current->next;
                }
            }
            mod_current_module_name = mod_current_module_name_save;
            mod_current_module = mod_current_module_save;
        }
    }
    /* Free argument list we created */
    free(av);
}

void event_process_hook(const char *name, int argc, char **argv)
{
    int retVal = 0;
    EvtHook *current = NULL;
    EvtHook *evh;
    if (mod_current_evtbuffer) {
        free(mod_current_evtbuffer);
    }
    /* Do something with the message. */
    evh = find_eventhook(name);
    if (evh) {
        if (evh->func) {
            char *mod_current_module_name_save = mod_current_module_name;
            Module *mod_current_module_save = mod_current_module;
            mod_current_module = findModule(evh->mod_name);
            mod_current_module_name = evh->mod_name;
            retVal = evh->func(argc, argv);
            if (retVal == MOD_CONT) {
                current = evh->next;
                while (current && current->func && retVal == MOD_CONT) {
                    mod_current_module = findModule(current->mod_name);
                    mod_current_module_name = current->mod_name;
                    retVal = current->func(argc, argv);
                    current = current->next;
                }
            }
            mod_current_module_name = mod_current_module_name_save;
            mod_current_module = mod_current_module_save;
        }
    }
}

/**
 * Displays a message list for a given message.
 * Again this is of little use other than debugging.
 * @param m the message to display
 * @return 0 is returned and has no meaning 
 */
int displayEventMessage(EvtMessage * evm)
{
    EvtMessage *msg = NULL;
    int i = 0;
    alog("Displaying message list for %s", evm->name);
    for (msg = evm; msg; msg = msg->next) {
        alog("%d: 0x%p", ++i, (void *) msg);
    }
    alog("end");
    return 0;
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
        alog("%d: 0x%p", ++i, (void *) msg);
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

/**
 * Display the message call stak.
 * Prints the call stack for a message based on the message name, again useful for debugging and little lese :)
 * @param name the name of the message to print info for
 * @return the return int has no relevence atm :)
 */
int displayEvtMessageFromHash(char *name)
{
    EvtMessageHash *current = NULL;
    int index = 0;
    index = CMD_HASH(name);
    if (debug > 1) {
        alog("debug: trying to display message %s", name);
    }
    for (current = EVENT[index]; current; current = current->next) {
        if (stricmp(name, current->name) == 0) {
            displayEventMessage(current->evm);
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
EvtMessage *createEventHandler(char *name,
                               int (*func) (char *source, int ac,
                                            char **av))
{
    EvtMessage *evm = NULL;
    if (!func) {
        return NULL;
    }
    if ((evm = malloc(sizeof(EvtMessage))) == NULL) {
        fatal("Out of memory!");
    }
    evm->name = sstrdup(name);
    evm->func = func;
    evm->mod_name = NULL;
    evm->next = NULL;
    return evm;
}

 /**
  * Create a new Message struct.
  * @param name the name of the message
  * @param func a pointer to the function to call when we recive this message
  * @return a new Message object
  **/
EvtHook *createEventHook(char *name, int (*func) (int argc, char **argv))
{
    EvtHook *evh = NULL;
    if (!func) {
        return NULL;
    }
    if ((evh = malloc(sizeof(EvtHook))) == NULL) {
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
EvtMessage *findEventHandler(EvtMessageHash * msgEvtTable[],
                             const char *name)
{
    int idx;
    EvtMessageHash *current = NULL;
    if (!msgEvtTable || !name) {
        return NULL;
    }
    idx = CMD_HASH(name);

    for (current = msgEvtTable[idx]; current; current = current->next) {
        if (stricmp(name, current->name) == 0) {
            return current->evm;
        }
    }
    return NULL;
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
 * Add the given message (m) to the MessageHash marking it as a core command
 * @param msgTable the MessageHash we want to add to
 * @param m the Message we are adding
 * @return MOD_ERR_OK on a successful add.
 **/
int addCoreEventHandler(EvtMessageHash * msgEvtTable[], EvtMessage * evm)
{
    if (!msgEvtTable || !evm) {
        return MOD_ERR_PARAMS;
    }
    evm->core = 1;
    return addEventHandler(msgEvtTable, evm);
}

/**
 * Add a message to the MessageHash.
 * @param msgTable the MessageHash we want to add a message to
 * @param m the Message we want to add
 * @return MOD_ERR_OK on a successful add.
 **/
int addEventHandler(EvtMessageHash * msgEvtTable[], EvtMessage * evm)
{
    /* We can assume both param's have been checked by this point.. */
    int index = 0;
    EvtMessageHash *current = NULL;
    EvtMessageHash *newHash = NULL;
    EvtMessageHash *lastHash = NULL;

    if (!msgEvtTable || !evm) {
        return MOD_ERR_PARAMS;
    }

    index = CMD_HASH(evm->name);

    for (current = msgEvtTable[index]; current; current = current->next) {
        if (stricmp(evm->name, current->name) == 0) {   /* the msg exist's we are a addHead */
            evm->next = current->evm;
            current->evm = evm;
            if (debug)
                alog("debug: existing msg: (0x%p), new msg (0x%p)",
                     (void *) evm->next, (void *) evm);
            return MOD_ERR_OK;
        }
        lastHash = current;
    }

    if ((newHash = malloc(sizeof(EvtMessageHash))) == NULL) {
        fatal("Out of memory");
    }
    newHash->next = NULL;
    newHash->name = sstrdup(evm->name);
    newHash->evm = evm;

    if (lastHash == NULL)
        msgEvtTable[index] = newHash;
    else
        lastHash->next = newHash;
    return MOD_ERR_OK;
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
                     (void *) evh->next, (void *) evh);
            return MOD_ERR_OK;
        }
        lastHash = current;
    }

    if ((newHash = malloc(sizeof(EvtHookHash))) == NULL) {
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

/**
 * Add a module message to the IRCD message hash
 * @param m the Message to add
 * @param pos the Position to add the message to, e.g. MOD_HEAD, MOD_TAIL, MOD_UNIQUE
 * @return MOD_ERR_OK on success, althing else on fail.
 **/
int moduleAddEventHandler(EvtMessage * evm)
{
    int status;

    if (!evm) {
        return MOD_ERR_PARAMS;
    }

    if (!mod_current_module) {
        return MOD_ERR_UNKNOWN;
    }                           /* shouldnt happen */

    evm->core = 0;
    if (!evm->mod_name) {
        evm->mod_name = sstrdup(mod_current_module->name);
    }

    status = addEventHandler(EVENT, evm);
    if (debug) {
        displayEvtMessageFromHash(evm->name);
    }
    return status;
}

/**
 * Add a module message to the IRCD message hash
 * @param m the Message to add
 * @param pos the Position to add the message to, e.g. MOD_HEAD, MOD_TAIL, MOD_UNIQUE
 * @return MOD_ERR_OK on success, althing else on fail.
 **/
int moduleAddEventHook(EvtHook * evh)
{
    int status;

    if (!evh) {
        return MOD_ERR_PARAMS;
    }

    if (!mod_current_module) {
        return MOD_ERR_UNKNOWN;
    }                           /* shouldnt happen */

    evh->core = 0;
    if (!evh->mod_name) {
        evh->mod_name = sstrdup(mod_current_module->name);
    }

    status = addEventHook(EVENTHOOKS, evh);
    if (debug) {
        displayHookFromHash(evh->name);
    }
    return status;
}

/**
 * remove the given message from the IRCD message hash
 * @param name the name of the message to remove
 * @return MOD_ERR_OK on success, althing else on fail.
 **/
int moduleEventDelHandler(char *name)
{
    EvtMessage *evm;
    int status;

    if (!mod_current_module) {
        return MOD_ERR_UNKNOWN;
    }
    evm = findEventHandler(EVENT, name);
    if (!evm) {
        return MOD_ERR_NOEXIST;
    }

    status = delEventHandler(EVENT, evm, mod_current_module->name);
    if (debug) {
        displayEvtMessageFromHash(evm->name);
    }
    return status;
}

/**
 * remove the given message from the IRCD message hash
 * @param name the name of the message to remove
 * @return MOD_ERR_OK on success, althing else on fail.
 **/
int moduleEventDelHook(const char *name)
{
    EvtHook *evh;
    int status;

    if (!mod_current_module) {
        return MOD_ERR_UNKNOWN;
    }
    evh = findEventHook(EVENTHOOKS, name);
    if (!evh) {
        return MOD_ERR_NOEXIST;
    }

    status = delEventHook(EVENTHOOKS, evh, mod_current_module->name);
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
int delEventHandler(EvtMessageHash * msgEvtTable[], EvtMessage * evm,
                    char *mod_name)
{
    int index = 0;
    EvtMessageHash *current = NULL;
    EvtMessageHash *lastHash = NULL;
    EvtMessage *tail = NULL, *last = NULL;

    if (!evm || !msgEvtTable) {
        return MOD_ERR_PARAMS;
    }

    index = CMD_HASH(evm->name);

    for (current = msgEvtTable[index]; current; current = current->next) {
        if (stricmp(evm->name, current->name) == 0) {
            if (!lastHash) {
                tail = current->evm;
                if (tail->next) {
                    while (tail) {
                        if (mod_name && tail->mod_name
                            && (stricmp(mod_name, tail->mod_name) == 0)) {
                            if (last) {
                                last->next = tail->next;
                            } else {
                                current->evm = tail->next;
                            }
                            return MOD_ERR_OK;
                        }
                        last = tail;
                        tail = tail->next;
                    }
                } else {
                    msgEvtTable[index] = current->next;
                    free(current->name);
                    return MOD_ERR_OK;
                }
            } else {
                tail = current->evm;
                if (tail->next) {
                    while (tail) {
                        if (mod_name && tail->mod_name
                            && (stricmp(mod_name, tail->mod_name) == 0)) {
                            if (last) {
                                last->next = tail->next;
                            } else {
                                current->evm = tail->next;
                            }
                            return MOD_ERR_OK;
                        }
                        last = tail;
                        tail = tail->next;
                    }
                } else {
                    lastHash->next = current->next;
                    free(current->name);
                    return MOD_ERR_OK;
                }
            }
        }
        lastHash = current;
    }
    return MOD_ERR_NOEXIST;
}


/**
 * remove the given message from the given message hash, for the given module
 * @param msgTable which MessageHash we are removing from
 * @param m the Message we want to remove
 * @mod_name the name of the module we are removing
 * @return MOD_ERR_OK on success, althing else on fail.
 **/
int delEventHook(EvtHookHash * hookEvtTable[], EvtHook * evh,
                 char *mod_name)
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
                    free(current->name);
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
                    free(current->name);
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
int destroyEventHandler(EvtMessage * evm)
{
    if (!evm) {
        return MOD_ERR_PARAMS;
    }
    if (evm->name) {
        free(evm->name);
    }
    evm->func = NULL;
    if (evm->mod_name) {
        free(evm->mod_name);
    }
    evm->next = NULL;
    return MOD_ERR_OK;
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
        free(evh->name);
    }
    evh->func = NULL;
    if (evh->mod_name) {
        free(evh->mod_name);
    }
    evh->next = NULL;
    return MOD_ERR_OK;
}

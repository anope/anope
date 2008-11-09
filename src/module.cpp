/* Modular support
 *
 * (C) 2008 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * $Id$
 *
 */
#include "modules.h"
#include "language.h"
#include "version.h"

#if !defined(_WIN32)
	#include <dlfcn.h>
#else
	const char *ano_moderr(void);
#endif

Module::Module(const std::string &mname, const std::string &creator)
{
	this->name = mname;				/* Our name */
	this->filename = NULL;
	this->nickHelp = NULL;
	this->chanHelp = NULL;
	this->memoHelp = NULL;
	this->botHelp = NULL;
	this->operHelp = NULL;
	this->hostHelp = NULL;
	this->helpHelp = NULL;
	this->type = THIRD;
	this->handle = NULL;

	for (int i = 0; i < NUM_LANGS; i++)
	{
		this->lang[i].argc = 0;
	}
}

Module::~Module()
{
	int i = 0;
	mod_current_module = this;

	for (i = 0; i < NUM_LANGS; i++)
		moduleDeleteLanguage(i);

	if (this->filename)
	{
		remove(this->filename);
		free(this->filename);
	}

	if (this->handle)
	{
		if ((ano_modclose(this->handle)) != 0)
			alog("%s", ano_moderr());
	}

    int idx;
    CommandHash *current = NULL;
    MessageHash *mcurrent = NULL;
    EvtMessageHash *ecurrent = NULL;
    EvtHookHash *ehcurrent = NULL;

    Command *c;
    Message *msg;
    EvtMessage *eMsg;
    EvtHook *eHook;
    int status = 0;

    /* Kill any active callbacks this module has */
    moduleCallBackPrepForUnload(this->name.c_str());

    /* Remove any stored data this module has */
    moduleDelAllDataMod(this);

    /**
     * ok, im going to walk every hash looking for commands we own, now, not exactly elegant or efficiant :)
     **/
    for (idx = 0; idx < MAX_CMD_HASH; idx++) {
        for (current = HS_cmdTable[idx]; current; current = current->next) {
            for (c = current->c; c; c = c->next) {
                if ((c->mod_name) && (strcmp(c->mod_name, this->name.c_str()) == 0)) {
                    this->DelCommand(HOSTSERV, c->name);
                }
            }
        }

        for (current = BS_cmdTable[idx]; current; current = current->next) {
            for (c = current->c; c; c = c->next) {
                if ((c->mod_name) && (strcmp(c->mod_name, this->name.c_str()) == 0)) {
                    this->DelCommand(BOTSERV, c->name);
                }
            }
        }

        for (current = MS_cmdTable[idx]; current; current = current->next) {
            for (c = current->c; c; c = c->next) {
                if ((c->mod_name) && (strcmp(c->mod_name, this->name.c_str()) == 0)) {
                    this->DelCommand(MEMOSERV, c->name);
                }
            }
        }

        for (current = NS_cmdTable[idx]; current; current = current->next) {
            for (c = current->c; c; c = c->next) {
                if ((c->mod_name) && (strcmp(c->mod_name, this->name.c_str()) == 0)) {
                    this->DelCommand(NICKSERV, c->name);
                }
            }
        }

        for (current = CS_cmdTable[idx]; current; current = current->next) {
            for (c = current->c; c; c = c->next) {
                if ((c->mod_name) && (strcmp(c->mod_name, this->name.c_str()) == 0)) {
                    this->DelCommand(CHANSERV, c->name);
                }
            }
        }

        for (current = HE_cmdTable[idx]; current; current = current->next) {
            for (c = current->c; c; c = c->next) {
                if ((c->mod_name) && (strcmp(c->mod_name, this->name.c_str()) == 0)) {
                    this->DelCommand(HELPSERV, c->name);
                }
            }
        }

        for (current = OS_cmdTable[idx]; current; current = current->next) {
            for (c = current->c; c; c = c->next) {
                if ((c->mod_name) && (stricmp(c->mod_name, this->name.c_str()) == 0)) {
                    this->DelCommand(OPERSERV, c->name);
                }
            }
        }

        for (mcurrent = IRCD[idx]; mcurrent; mcurrent = mcurrent->next) {
            for (msg = mcurrent->m; msg; msg = msg->next) {
                if ((msg->mod_name)
                    && (stricmp(msg->mod_name, this->name.c_str()) == 0)) {
                    moduleDelMessage(msg->name);
                }
            }
        }

        for (ecurrent = EVENT[idx]; ecurrent; ecurrent = ecurrent->next) {
            for (eMsg = ecurrent->evm; eMsg; eMsg = eMsg->next) {
                if ((eMsg->mod_name)
                    && (stricmp(eMsg->mod_name, this->name.c_str()) == 0)) {
                    status = delEventHandler(EVENT, eMsg, this->name.c_str());
                }
            }
        }
        for (ehcurrent = EVENTHOOKS[idx]; ehcurrent;
             ehcurrent = ehcurrent->next) {
            for (eHook = ehcurrent->evh; eHook; eHook = eHook->next) {
                if ((eHook->mod_name)
                    && (stricmp(eHook->mod_name, this->name.c_str()) == 0)) {
                    status = delEventHook(EVENTHOOKS, eHook, this->name.c_str());
                }
            }
        }

    }
}

void Module::SetType(MODType ntype)
{
	this->type = ntype;
}

void Module::SetVersion(const std::string &nversion)
{
	this->version = nversion;
}

void Module::SetAuthor(const std::string &nauthor)
{
	this->author = nauthor;
}

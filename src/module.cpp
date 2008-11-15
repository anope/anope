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

Module::Module(const std::string &mname, const std::string &creator)
{
	this->name = mname;				/* Our name */
	this->nickHelp = NULL;
	this->chanHelp = NULL;
	this->memoHelp = NULL;
	this->botHelp = NULL;
	this->operHelp = NULL;
	this->hostHelp = NULL;
	this->helpHelp = NULL;
	this->type = THIRD;
	this->handle = NULL;

	this->permanent = false;

	for (int i = 0; i < NUM_LANGS; i++)
	{
		this->lang[i].argc = 0;
	}

	int index = 0;
	ModuleHash *current = NULL;
	ModuleHash *newHash = NULL;
	ModuleHash *lastHash = NULL;

	index = CMD_HASH(this->name);

	for (current = MODULE_HASH[index]; current; current = current->next) {
		if (this->name ==current->name)
			throw CoreException("Module already exists!");
		lastHash = current;
	}

	if ((newHash = (ModuleHash *)malloc(sizeof(ModuleHash))) == NULL) {
		fatal("Out of memory");
	}
	this->created = time(NULL);
	newHash->next = NULL;
	newHash->name = sstrdup(this->name.c_str());
	newHash->m = this;

	if (lastHash == NULL)
		MODULE_HASH[index] = newHash;
	else
		lastHash->next = newHash;
}

Module::~Module()
{
	int i = 0;
	mod_current_module = this;

	for (i = 0; i < NUM_LANGS; i++)
		moduleDeleteLanguage(i);

	remove(this->filename.c_str());

	if (this->handle)
	{
		if ((ano_modclose(this->handle)) != 0)
			alog("%s", ano_moderr());
	}

	int idx;
	CommandHash *current = NULL;
	MessageHash *mcurrent = NULL;
	EvtHookHash *ehcurrent = NULL;

	Command *c;
	Message *msg;
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

	int index = 0;
	ModuleHash *lastHash = NULL;
	ModuleHash *mhash = NULL;

	index = CMD_HASH(this->name);

	for (mhash = MODULE_HASH[index]; mhash; mhash = mhash->next) {
		if (this->name == mhash->name) {
			if (!lastHash) {
				MODULE_HASH[index] = mhash->next;
			} else {
				lastHash->next = mhash->next;
			}
			free(mhash->name);
			free(mhash);
		}
		lastHash = mhash;
	}
}

void Module::SetType(MODType ntype)
{
	this->type = ntype;
}

void Module::SetPermanent(bool state)
{
	this->permanent = state;
}

bool Module::GetPermanent()
{
	return this->permanent;
}

void Module::SetVersion(const std::string &nversion)
{
	this->version = nversion;
}

void Module::SetAuthor(const std::string &nauthor)
{
	this->author = nauthor;
}

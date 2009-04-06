/* Modular support
 *
 * (C) 2009 Anope Team
 * Contact us at team@anope.org
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

	if (!(newHash = new ModuleHash)) {
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

	for (i = 0; i < NUM_LANGS; i++)
		this->DeleteLanguage(i);

	remove(this->filename.c_str());

	int idx;
	CommandHash *current = NULL;

	Command *c;

	/* Kill any active callbacks this module has */
	moduleCallBackPrepForUnload(this->name.c_str());

	/**
	 * ok, im going to walk every hash looking for commands we own, now, not exactly elegant or efficiant :)
	 **/
	for (idx = 0; idx < MAX_CMD_HASH; idx++) {
		for (current = HS_cmdTable[idx]; current; current = current->next) {
			for (c = current->c; c; c = c->next) {
				if ((c->mod_name) && (strcmp(c->mod_name, this->name.c_str()) == 0)) {
					this->DelCommand(HOSTSERV, c->name.c_str());
				}
			}
		}

		for (current = BS_cmdTable[idx]; current; current = current->next) {
			for (c = current->c; c; c = c->next) {
				if ((c->mod_name) && (strcmp(c->mod_name, this->name.c_str()) == 0)) {
					this->DelCommand(BOTSERV, c->name.c_str());
				}
			}
		}

		for (current = MS_cmdTable[idx]; current; current = current->next) {
			for (c = current->c; c; c = c->next) {
				if ((c->mod_name) && (strcmp(c->mod_name, this->name.c_str()) == 0)) {
					this->DelCommand(MEMOSERV, c->name.c_str());
				}
			}
		}

		for (current = NS_cmdTable[idx]; current; current = current->next) {
			for (c = current->c; c; c = c->next) {
				if ((c->mod_name) && (strcmp(c->mod_name, this->name.c_str()) == 0)) {
					this->DelCommand(NICKSERV, c->name.c_str());
				}
			}
		}

		for (current = CS_cmdTable[idx]; current; current = current->next) {
			for (c = current->c; c; c = c->next) {
				if ((c->mod_name) && (strcmp(c->mod_name, this->name.c_str()) == 0)) {
					this->DelCommand(CHANSERV, c->name.c_str());
				}
			}
		}

		for (current = OS_cmdTable[idx]; current; current = current->next) {
			for (c = current->c; c; c = c->next) {
				if ((c->mod_name) && (stricmp(c->mod_name, this->name.c_str()) == 0)) {
					this->DelCommand(OPERSERV, c->name.c_str());
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
			delete [] mhash->name;
			delete mhash;
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

/* StatServ core functions
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

#include "module.h"

class StatServ : public Service
{
 public:
	StatServ() : Service("StatServ", ServiceUser, ServiceHost, "Stats Service")
	{
	}
} *statserv = NULL;

int statserv_create(int argc, char **argv);

class SSMain : public Module
{
 public:
	SSMain(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		EvtHook *hook;

		hook = createEventHook(EVENT_SERVER_CONNECT, statserv_create);
		this->AddEventHook(hook);
	}
};

int statserv_create(int argc, char **argv)
{
	statserv = new StatServ();
	return MOD_CONT;
}

MODULE_INIT("ss_main", SSMain)

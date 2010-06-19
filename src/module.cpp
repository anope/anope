/* Modular support
 *
 * (C) 2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
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

	if (FindModule(this->name))
		throw CoreException("Module already exists!");
	
	this->created = time(NULL);

	Modules.push_back(this);
}

Module::~Module()
{
	int i = 0;

	for (i = 0; i < NUM_LANGS; i++)
		this->DeleteLanguage(i);

	remove(this->filename.c_str());

	/* Clear any active callbacks this module has */
	ModuleManager::ClearCallBacks(this);

	/**
	 * ok, im going to walk every hash looking for commands we own, now, not exactly elegant or efficiant :)
	 **/
	if (HostServ)
	{
		for (std::map<ci::string, Command *>::iterator it = HostServ->Commands.begin(); it != HostServ->Commands.end();)
		{
			Command *c = it->second;
			++it;

			if (c->module == this)
				this->DelCommand(HostServ, c);
		}
	}

	if (BotServ)
	{
		for (std::map<ci::string, Command *>::iterator it = BotServ->Commands.begin(); it != BotServ->Commands.end();)
		{
			Command *c = it->second;
			++it;

			if (c->module == this)
				this->DelCommand(BotServ, c);
		}
	}

	if (MemoServ)
	{
		for (std::map<ci::string, Command *>::iterator it = MemoServ->Commands.begin(); it != MemoServ->Commands.end();)
		{
			Command *c = it->second;
			++it;

			if (c->module == this)
				this->DelCommand(MemoServ, c);
		}
	}

	if (NickServ)
	{
		for (std::map<ci::string, Command *>::iterator it = NickServ->Commands.begin(); it != NickServ->Commands.end();)
		{
			Command *c = it->second;
			++it;

			if (c->module == this)
				this->DelCommand(NickServ, c);
		}
	}
		
	if (ChanServ)
	{
		for (std::map<ci::string, Command *>::iterator it = ChanServ->Commands.begin(); it != ChanServ->Commands.end();)
		{
			Command *c = it->second;
			++it;

			if (c->module == this)
				this->DelCommand(ChanServ, c);
		}
	}

	if (OperServ)
	{
		for (std::map<ci::string, Command *>::iterator it = OperServ->Commands.begin(); it != OperServ->Commands.end();)
		{
			Command *c = it->second;
			++it;

			if (c->module == this)
				this->DelCommand(OperServ, c);
		}
	}

	for (std::deque<Module *>::iterator it = Modules.begin(); it != Modules.end(); ++it)
	{
		if (*it == this)
		{
			Modules.erase(it);
			break;
		}
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

Version::Version(unsigned vMajor, unsigned vMinor, unsigned vBuild) : Major(vMajor), Minor(vMinor), Build(vBuild)
{
}

Version::~Version()
{
}

const unsigned Version::GetMajor()
{
	return Major;
}

const unsigned Version::GetMinor()
{
	return Minor;
}

const unsigned Version::GetBuild()
{
	return Build;
}

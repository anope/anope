/* Modular support
 *
 * (C) 2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "modules.h"
#include "language.h"

Module::Module(const Anope::string &mname, const Anope::string &creator)
{
	this->name = mname; /* Our name */
	this->type = THIRD;
	this->handle = NULL;

	this->permanent = false;

	for (int i = 0; i < NUM_LANGS; ++i)
		this->lang[i].argc = 0;

	if (FindModule(this->name))
		throw CoreException("Module already exists!");

	this->created = time(NULL);

	this->SetVersion(Anope::Version());

	Modules.push_back(this);
}

Module::~Module()
{
	int i = 0;

	for (i = 0; i < NUM_LANGS; ++i)
		this->DeleteLanguage(i);

	/* Clear any active callbacks this module has */
	ModuleManager::ClearCallBacks(this);

	/**
	 * ok, im going to walk every hash looking for commands we own, now, not exactly elegant or efficiant :)
	 **/
	if (HostServ)
	{
		for (CommandMap::iterator it = HostServ->Commands.begin(), it_end = HostServ->Commands.end(); it != it_end; )
		{
			Command *c = it->second;
			++it;

			if (c->module == this)
				this->DelCommand(HostServ, c);
		}
	}

	if (BotServ)
	{
		for (CommandMap::iterator it = BotServ->Commands.begin(), it_end = BotServ->Commands.end(); it != it_end; )
		{
			Command *c = it->second;
			++it;

			if (c->module == this)
				this->DelCommand(BotServ, c);
		}
	}

	if (MemoServ)
	{
		for (CommandMap::iterator it = MemoServ->Commands.begin(), it_end = MemoServ->Commands.end(); it != it_end; )
		{
			Command *c = it->second;
			++it;

			if (c->module == this)
				this->DelCommand(MemoServ, c);
		}
	}

	if (NickServ)
	{
		for (CommandMap::iterator it = NickServ->Commands.begin(), it_end = NickServ->Commands.end(); it != it_end; )
		{
			Command *c = it->second;
			++it;

			if (c->module == this)
				this->DelCommand(NickServ, c);
		}
	}

	if (ChanServ)
	{
		for (CommandMap::iterator it = ChanServ->Commands.begin(), it_end = ChanServ->Commands.end(); it != it_end; )
		{
			Command *c = it->second;
			++it;

			if (c->module == this)
				this->DelCommand(ChanServ, c);
		}
	}

	if (OperServ)
	{
		for (CommandMap::iterator it = OperServ->Commands.begin(), it_end = OperServ->Commands.end(); it != it_end; )
		{
			Command *c = it->second;
			++it;

			if (c->module == this)
				this->DelCommand(OperServ, c);
		}
	}

	std::list<Module *>::iterator it = std::find(Modules.begin(), Modules.end(), this);
	if (it != Modules.end())
		Modules.erase(it);
}

void Module::SetType(MODType ntype)
{
	this->type = ntype;
}

void Module::SetPermanent(bool state)
{
	this->permanent = state;
}

bool Module::GetPermanent() const
{
	return this->permanent;
}

void Module::SetVersion(const Anope::string &nversion)
{
	this->version = nversion;
}

void Module::SetAuthor(const Anope::string &nauthor)
{
	this->author = nauthor;
}

Version::Version(unsigned vMajor, unsigned vMinor, unsigned vBuild) : Major(vMajor), Minor(vMinor), Build(vBuild)
{
}

Version::~Version()
{
}

unsigned Version::GetMajor() const
{
	return this->Major;
}

unsigned Version::GetMinor() const
{
	return this->Minor;
}

unsigned Version::GetBuild() const
{
	return this->Build;
}

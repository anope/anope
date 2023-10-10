/*
 *
 * (C) 2008-2011 Robin Burchell <w00t@inspircd.org>
 * (C) 2008-2023 Anope Team <team@anope.org>
 *
 * Please read COPYING and README for further details.
 */

#include "services.h"
#include "anope.h"
#include "opertype.h"
#include "config.h"

std::vector<Oper *> Oper::opers;

Oper::Oper(const Anope::string &n, OperType *o) : name(n), ot(o)
{
	opers.push_back(this);
}

Oper::~Oper()
{
	std::vector<Oper *>::iterator it = std::find(opers.begin(), opers.end(), this);
	if (it != opers.end())
		opers.erase(it);
}

Oper *Oper::Find(const Anope::string &name)
{
	for (auto *o : opers)
	{
		if (o->name.equals_ci(name))
			return o;
	}

	return NULL;
}

OperType *OperType::Find(const Anope::string &name)
{
	for (auto *ot : Config->MyOperTypes)
	{
		if (ot->GetName() == name)
			return ot;
	}

	return NULL;
}

OperType::OperType(const Anope::string &nname) : name(nname)
{
}

bool OperType::HasCommand(const Anope::string &cmdstr) const
{
	for (const auto &command : this->commands)
	{
		if (!command.find('~') && Anope::Match(cmdstr, command.substr(1)))
			return false;
		else if (Anope::Match(cmdstr, command))
			return true;
	}
	for (auto *ot : this->inheritances)
	{
		if (ot->HasCommand(cmdstr))
			return true;
	}

	return false;
}

bool OperType::HasPriv(const Anope::string &privstr) const
{
	for (const auto &priv : this->privs)
	{
		if (!priv.find('~') && Anope::Match(privstr, priv.substr(1)))
			return false;
		else if (Anope::Match(privstr, priv))
			return true;
	}
	for (auto *ot : this->inheritances)
	{
		if (ot->HasPriv(privstr))
			return true;
	}

	return false;
}

void OperType::AddCommand(const Anope::string &cmdstr)
{
	this->commands.push_back(cmdstr);
}

void OperType::AddPriv(const Anope::string &privstr)
{
	this->privs.push_back(privstr);
}

const Anope::string &OperType::GetName() const
{
	return this->name;
}

void OperType::Inherits(OperType *ot)
{
	if (ot != this)
		this->inheritances.insert(ot);
}

const std::list<Anope::string> OperType::GetCommands() const
{
	std::list<Anope::string> cmd_list = this->commands;
	for (auto *ot : this->inheritances)
	{
		for (const auto &cmd : ot->GetCommands())
			cmd_list.push_back(cmd);
	}
	return cmd_list;
}

const std::list<Anope::string> OperType::GetPrivs() const
{
	std::list<Anope::string> priv_list = this->privs;
	for (auto *ot : this->inheritances)
	{
		for (const auto &priv : ot->GetPrivs())
			priv_list.push_back(priv);
	}
	return priv_list;
}

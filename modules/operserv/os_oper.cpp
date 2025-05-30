/* OperServ core functions
 *
 * (C) 2003-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/operserv/oper.h"

struct OSOperType
	: Serialize::Type
{
	OSOperType()
		: Serialize::Type("Oper")
	{
	}

	void Serialize(Serializable *obj, Serialize::Data &data) const override
	{
		const auto *myo = static_cast<const MyOper *>(obj);
		data.Store("name", myo->name);
		data.Store("type", myo->ot->GetName());
	}

	Serializable *Unserialize(Serializable *obj, Serialize::Data &data) const override
	{
		Anope::string stype, sname;

		data["type"] >> stype;
		data["name"] >> sname;

		OperType *ot = OperType::Find(stype);
		if (ot == NULL)
			return NULL;
		NickCore *nc = NickCore::Find(sname);
		if (nc == NULL)
			return NULL;

		MyOper *myo;
		if (obj)
			myo = anope_dynamic_static_cast<MyOper *>(obj);
		else
			myo = new MyOper(nc->display, ot);
		nc->o = myo;
		Log(LOG_NORMAL, "operserv/oper") << "Tied oper " << nc->display << " to type " << ot->GetName();
		return myo;
	}
};


class CommandOSOper final
	: public Command
{
	static bool HasPrivs(CommandSource &source, OperType *ot) 
	{
		for (const auto &command : ot->GetCommands())
		{
			if (!source.HasCommand(command))
				return false;
		}

		for (const auto &priv : ot->GetPrivs())
		{
			if (!source.HasPriv(priv))
				return false;
		}

		return true;
	}

public:
	CommandOSOper(Module *creator) : Command(creator, "operserv/oper", 1, 3)
	{
		this->SetDesc(_("View and change Services Operators"));
		this->SetSyntax(_("ADD \037oper\037 \037type\037"));
		this->SetSyntax(_("DEL \037oper\037"));
		this->SetSyntax(_("INFO [\037type\037]"));
		this->SetSyntax("LIST");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &subcommand = params[0];

		if (subcommand.equals_ci("ADD") && params.size() > 2)
		{
			const Anope::string &oper = params[1];
			const Anope::string &otype = params[2];

			if (!source.HasPriv("operserv/oper/modify"))
			{
				source.Reply(ACCESS_DENIED);
				return;
			}

			const NickAlias *na = NickAlias::Find(oper);
			if (na == NULL)
				source.Reply(NICK_X_NOT_REGISTERED, oper.c_str());
			else if (na->nc->o)
				source.Reply(_("Nick \002%s\002 is already an operator."), na->nick.c_str());
			else
			{
				OperType *ot = OperType::Find(otype);
				if (ot == NULL)
				{
					source.Reply(_("Oper type \002%s\002 has not been configured."), otype.c_str());
					return;
				}

				if (!HasPrivs(source, ot))
				{
					source.Reply(ACCESS_DENIED);
					return;
				}

				na->nc->o = new MyOper(na->nc->display, ot);

				if (Anope::ReadOnly)
					source.Reply(READ_ONLY_MODE);

				Log(LOG_ADMIN, source, this) << "ADD " << na->nick << " as type " << ot->GetName();
				source.Reply("%s (%s) added to the \002%s\002 list.", na->nick.c_str(), na->nc->display.c_str(), ot->GetName().c_str());
			}
		}
		else if (subcommand.equals_ci("DEL") && params.size() > 1)
		{
			const Anope::string &oper = params[1];

			if (!source.HasPriv("operserv/oper/modify"))
			{
				source.Reply(ACCESS_DENIED);
				return;
			}

			const NickAlias *na = NickAlias::Find(oper);
			if (na == NULL)
				source.Reply(NICK_X_NOT_REGISTERED, oper.c_str());
			else if (!na->nc || !na->nc->o)
				source.Reply(_("Nick \002%s\002 is not a Services Operator."), oper.c_str());
			else if (!HasPrivs(source, na->nc->o->ot))
				source.Reply(ACCESS_DENIED);
			else if (std::find(Config->Opers.begin(), Config->Opers.end(), na->nc->o) != Config->Opers.end())
				source.Reply(_("Oper \002%s\002 is configured in the configuration file(s) and can not be removed by this command."), na->nc->display.c_str());
			else
			{
				delete na->nc->o;
				na->nc->o = NULL;

				if (Anope::ReadOnly)
					source.Reply(READ_ONLY_MODE);

				Log(LOG_ADMIN, source, this) << "DEL " << na->nick;
				source.Reply(_("Oper privileges removed from %s (%s)."), na->nick.c_str(), na->nc->display.c_str());
			}
		}
		else if (subcommand.equals_ci("LIST"))
		{
			source.Reply(_("Name     Type"));
			for (const auto &[_, nc] : *NickCoreList)
			{
				if (!nc->o)
					continue;

				source.Reply(_("%-8s %s"), nc->o->name.c_str(), nc->o->ot->GetName().c_str());
				if (std::find(Config->Opers.begin(), Config->Opers.end(), nc->o) != Config->Opers.end())
					source.Reply(_("   This oper is configured in the configuration file."));
				for (auto *u : nc->users)
				{
						source.Reply(_("   %s is online using this oper block."), u->nick.c_str());
				}
			}
		}
		else if (subcommand.equals_ci("INFO"))
		{
			if (params.size() < 2)
			{
				source.Reply(_("Available opertypes:"));
				for (auto *ot : Config->MyOperTypes)
				{
					source.Reply("%s", ot->GetName().c_str());
				}
				return;
			}

			Anope::string fulltype = params[1];
			if (params.size() > 2)
				fulltype += " " + params[2];
			OperType *ot = OperType::Find(fulltype);
			if (ot == NULL)
				source.Reply(_("Oper type \002%s\002 has not been configured."), fulltype.c_str());
			else
			{
				if (ot->GetCommands().empty())
					source.Reply(_("Opertype \002%s\002 has no allowed commands."), ot->GetName().c_str());
				else
				{
					source.Reply(_("Available commands for \002%s\002:"), ot->GetName().c_str());
					Anope::string buf;
					std::list<Anope::string> cmds = ot->GetCommands();
					for (const auto &cmd : cmds)
					{
						buf += cmd + " ";
						if (buf.length() > 400)
						{
							source.Reply("%s", buf.c_str());
							buf.clear();
						}
					}
					if (!buf.empty())
					{
						source.Reply("%s", buf.c_str());
						buf.clear();
					}
				}
				if (ot->GetPrivs().empty())
					source.Reply(_("Opertype \002%s\002 has no allowed privileges."), ot->GetName().c_str());
				else
				{
					source.Reply(_("Available privileges for \002%s\002:"), ot->GetName().c_str());
					Anope::string buf;
					std::list<Anope::string> privs = ot->GetPrivs();
					for (const auto &priv : privs)
					{
						buf += priv + " ";
						if (buf.length() > 400)
						{
							source.Reply("%s", buf.c_str());
							buf.clear();
						}
					}
					if (!buf.empty())
					{
						source.Reply("%s", buf.c_str());
						buf.clear();
					}
				}
				if (!ot->modes.empty())
					source.Reply(_("Opertype \002%s\002 receives modes \002%s\002 once identified."), ot->GetName().c_str(), ot->modes.c_str());
			}
		}
		else
			this->OnSyntaxError(source, subcommand);

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Allows you to change and view Services Operators. "
			"Note that operators removed by this command but are still set in "
			"the configuration file are not permanently affected by this."
		));
		return true;
	}
};

class OSOper final
	: public Module
{
	OSOperType myoper_type;
	CommandOSOper commandosoper;

public:
	OSOper(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, VENDOR)
		, commandosoper(this)
	{
	}

	~OSOper() override
	{
		for (const auto &[_, nc] : *NickCoreList)
		{
			if (nc->o && dynamic_cast<MyOper *>(nc->o))
			{
				delete nc->o;
				nc->o = NULL;
			}
		}
	}

	void OnDelCore(NickCore *nc) override
	{
		if (nc->o && dynamic_cast<MyOper *>(nc->o))
		{
			delete nc->o;
			nc->o = NULL;
		}
	}
};

MODULE_INIT(OSOper)

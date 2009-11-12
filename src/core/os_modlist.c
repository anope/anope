/* OperServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

class CommandOSModList : public Command
{
 public:
	CommandOSModList() : Command("MODLIST", 0, 1, "operserv/modlist")
	{
	}

	CommandReturn Execute(User *u, const std::vector<ci::string> &params)
	{
		int idx;
		int count = 0;
		int showCore = 0;
		int showThird = 1;
		int showProto = 1;
		int showEnc = 1;
		int showSupported = 1;
		int showQA = 1;

		ci::string param = params.size() ? params[0] : "";
		ModuleHash *current = NULL;

		char core[] = "Core";
		char third[] = "3rd";
		char proto[] = "Protocol";
		char enc[] = "Encryption";
		char supported[] = "Supported";
		char qa[] = "QATested";

		if (!param.empty())
		{
			if (param == core)
			{
				showCore = 1;
				showThird = 0;
				showProto = 0;
				showEnc = 0;
				showSupported = 0;
				showQA = 0;
			}
			else if (param == third)
			{
				showCore = 0;
				showThird = 1;
				showSupported = 0;
				showQA = 0;
				showProto = 0;
				showEnc = 0;
			}
			else if (param == proto)
			{
				showCore = 0;
				showThird = 0;
				showProto = 1;
				showEnc = 0;
				showSupported = 0;
				showQA = 0;
			}
			else if (param == supported)
			{
				showCore = 0;
				showThird = 0;
				showProto = 0;
				showSupported = 1;
				showEnc = 0;
				showQA = 0;
			}
			else if (param == qa)
			{
				showCore = 0;
				showThird = 0;
				showProto = 0;
				showSupported = 0;
				showEnc = 0;
				showQA = 1;
			}
			else if (param == enc)
			{
				showCore = 0;
				showThird = 0;
				showProto = 0;
				showSupported = 0;
				showEnc = 1;
				showQA = 0;
			}
		}

		notice_lang(s_OperServ, u, OPER_MODULE_LIST_HEADER);

		for (idx = 0; idx != MAX_CMD_HASH; ++idx)
		{
			for (current = MODULE_HASH[idx]; current; current = current->next)
			{
				switch (current->m->type)
				{
					case CORE:
						if (showCore)
						{
							notice_lang(s_OperServ, u, OPER_MODULE_LIST, current->name, current->m->version.c_str(), core);
							++count;
						}
						break;
					case THIRD:
						if (showThird)
						{
							notice_lang(s_OperServ, u, OPER_MODULE_LIST, current->name, current->m->version.c_str(), third);
							++count;
						}
						break;
					case PROTOCOL:
						if (showProto)
						{
							notice_lang(s_OperServ, u, OPER_MODULE_LIST, current->name, current->m->version.c_str(), proto);
							++count;
						}
						break;
					case SUPPORTED:
						if (showSupported)
						{
							notice_lang(s_OperServ, u, OPER_MODULE_LIST, current->name, current->m->version.c_str(), supported);
							++count;
						}
						break;
					case QATESTED:
						if (showQA)
						{
							notice_lang(s_OperServ, u, OPER_MODULE_LIST, current->name, current->m->version.c_str(), qa);
							++count;
						}
						break;
					case ENCRYPTION:
						if (showEnc)
						{
							notice_lang(s_OperServ, u, OPER_MODULE_LIST, current->name, current->m->version.c_str(), enc);
							++count;
						}
				}
			}
		}
		if (!count)
			notice_lang(s_OperServ, u, OPER_MODULE_NO_LIST);
		else
			notice_lang(s_OperServ, u, OPER_MODULE_LIST_FOOTER, count);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const ci::string &subcommand)
	{
		notice_help(s_OperServ, u, OPER_HELP_MODLIST);
		return true;
	}
};

class OSModList : public Module
{
 public:
	OSModList(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(OPERSERV, new CommandOSModList());

		ModuleManager::Attach(I_OnOperServHelp, this);
	}
	void OnOperServHelp(User *u)
	{
		notice_lang(s_OperServ, u, OPER_HELP_CMD_MODLIST);
	}
};

MODULE_INIT(OSModList)

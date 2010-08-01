/* OperServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandOSModList : public Command
{
 public:
	CommandOSModList() : Command("MODLIST", 0, 1, "operserv/modlist")
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		int count = 0;
		int showCore = 0;
		int showThird = 1;
		int showProto = 1;
		int showEnc = 1;
		int showSupported = 1;
		int showQA = 1;
		int showDB = 1;
		int showSocketEngine = 1;

		Anope::string param = !params.empty() ? params[0] : "";

		char core[] = "Core";
		char third[] = "3rd";
		char proto[] = "Protocol";
		char enc[] = "Encryption";
		char supported[] = "Supported";
		char qa[] = "QATested";
		char db[] = "Database";
		char socketengine[] = "SocketEngine";

		if (!param.empty())
		{
			if (param.equals_ci(core))
			{
				showCore = 1;
				showThird = 0;
				showProto = 0;
				showEnc = 0;
				showSupported = 0;
				showQA = 0;
				showDB = 0;
				showSocketEngine = 0;
			}
			else if (param.equals_ci(third))
			{
				showCore = 0;
				showThird = 1;
				showSupported = 0;
				showQA = 0;
				showProto = 0;
				showEnc = 0;
				showDB = 0;
				showSocketEngine = 0;
			}
			else if (param.equals_ci(proto))
			{
				showCore = 0;
				showThird = 0;
				showProto = 1;
				showEnc = 0;
				showSupported = 0;
				showQA = 0;
				showDB = 0;
				showSocketEngine = 0;
			}
			else if (param.equals_ci(supported))
			{
				showCore = 0;
				showThird = 0;
				showProto = 0;
				showSupported = 1;
				showEnc = 0;
				showQA = 0;
				showDB = 0;
				showSocketEngine = 0;
			}
			else if (param.equals_ci(qa))
			{
				showCore = 0;
				showThird = 0;
				showProto = 0;
				showSupported = 0;
				showEnc = 0;
				showQA = 1;
				showDB = 0;
				showSocketEngine = 0;
			}
			else if (param.equals_ci(enc))
			{
				showCore = 0;
				showThird = 0;
				showProto = 0;
				showSupported = 0;
				showEnc = 1;
				showQA = 0;
				showDB = 0;
				showSocketEngine = 0;
			}
			else if (param.equals_ci(db))
			{
				showCore = 0;
				showThird = 0;
				showProto = 0;
				showSupported = 0;
				showEnc = 0;
				showQA = 0;
				showDB = 1;
				showSocketEngine = 0;
			}
			else if (param == socketengine)
			{
				showCore = showThird = showProto = showSupported = showEnc = showQA = showDB = 0;
				showSocketEngine = 1;
			}
		}

		notice_lang(Config.s_OperServ, u, OPER_MODULE_LIST_HEADER);

		for (std::list<Module *>::iterator it = Modules.begin(), it_end = Modules.end(); it != it_end; ++it)
		{
			Module *m = *it;

			switch (m->type)
			{
				case CORE:
					if (showCore)
					{
						notice_lang(Config.s_OperServ, u, OPER_MODULE_LIST, m->name.c_str(), m->version.c_str(), core);
						++count;
					}
					break;
				case THIRD:
					if (showThird)
					{
						notice_lang(Config.s_OperServ, u, OPER_MODULE_LIST, m->name.c_str(), m->version.c_str(), third);
						++count;
					}
					break;
				case PROTOCOL:
					if (showProto)
					{
						notice_lang(Config.s_OperServ, u, OPER_MODULE_LIST, m->name.c_str(), m->version.c_str(), proto);
						++count;
					}
					break;
				case SUPPORTED:
					if (showSupported)
					{
						notice_lang(Config.s_OperServ, u, OPER_MODULE_LIST, m->name.c_str(), m->version.c_str(), supported);
						++count;
					}
					break;
				case QATESTED:
					if (showQA)
					{
						notice_lang(Config.s_OperServ, u, OPER_MODULE_LIST, m->name.c_str(), m->version.c_str(), qa);
						++count;
					}
					break;
				case ENCRYPTION:
					if (showEnc)
					{
						notice_lang(Config.s_OperServ, u, OPER_MODULE_LIST, m->name.c_str(), m->version.c_str(), enc);
						++count;
					}
					break;
				case DATABASE:
					if (showDB)
					{
						notice_lang(Config.s_OperServ, u, OPER_MODULE_LIST, m->name.c_str(), m->version.c_str(), db);
						++count;
					}
					break;
				case SOCKETENGINE:
					if (showSocketEngine)
					{
						notice_lang(Config.s_OperServ, u, OPER_MODULE_LIST, m->name.c_str(), m->version.c_str(), socketengine);
						++count;
					}
					break;
			}
		}
		if (!count)
			notice_lang(Config.s_OperServ, u, OPER_MODULE_NO_LIST);
		else
			notice_lang(Config.s_OperServ, u, OPER_MODULE_LIST_FOOTER, count);

		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config.s_OperServ, u, OPER_HELP_MODLIST);
		return true;
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config.s_OperServ, u, OPER_HELP_CMD_MODLIST);
	}
};

class OSModList : public Module
{
	CommandOSModList commandosmodlist;

 public:
	OSModList(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(OperServ, &commandosmodlist);
	}
};

MODULE_INIT(OSModList)

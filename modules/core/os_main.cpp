/* OperServ core functions
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"
#include "operserv.h"

static BotInfo *OperServ = NULL;

class OperServBotInfo : public BotInfo
{
 public:
	OperServBotInfo(const Anope::string &bnick, const Anope::string &user = "", const Anope::string &bhost = "", const Anope::string &real = "") : BotInfo(bnick, user, bhost, real) { }
	
	void OnMessage(User *u, const Anope::string &message)
	{
		if (!u->HasMode(UMODE_OPER) && Config->OSOpersOnly)
		{
			u->SendMessage(OperServ, ACCESS_DENIED);
			if (Config->WallBadOS)
				ircdproto->SendGlobops(this, "Denied access to %s from %s!%s@%s (non-oper)", Config->s_OperServ.c_str(), u->nick.c_str(), u->GetIdent().c_str(), u->host.c_str());
		}
		else
		{
			BotInfo::OnMessage(u, message);
		}
	}
};

class MyOperServService : public OperServService
{
 public:
	MyOperServService(Module *m) : OperServService(m) { }

	BotInfo *Bot()
	{
		return OperServ;
	}
};

class OperServCore : public Module
{
	MyOperServService myoperserv;

 public:
	OperServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), myoperserv(this)
	{
		this->SetAuthor("Anope");
		this->SetPermanent(true); // Currently, /os modunload os_main explodes for obvious reasons

		Implementation i[] = { I_OnServerQuit, I_OnUserModeSet, I_OnUserModeUnset, I_OnUserConnect };
		ModuleManager::Attach(i, this, 4);

		ModuleManager::RegisterService(&this->myoperserv);
		
		OperServ = new OperServBotInfo(Config->s_OperServ, Config->ServiceUser, Config->ServiceHost, Config->desc_OperServ);
		OperServ->SetFlag(BI_CORE);

		/* Yes, these are in this order for a reason. Most violent->least violent. */
		XLineManager::RegisterXLineManager(SGLine = new SGLineManager());
		XLineManager::RegisterXLineManager(SZLine = new SZLineManager());
		XLineManager::RegisterXLineManager(SQLine = new SQLineManager());
		XLineManager::RegisterXLineManager(SNLine = new SNLineManager());

		spacesepstream coreModules(Config->OperCoreModules);
		Anope::string module;
		while (coreModules.GetToken(module))
			ModuleManager::LoadModule(module, NULL);
	}

	~OperServCore()
	{
		spacesepstream coreModules(Config->OperCoreModules);
		Anope::string module;
		while (coreModules.GetToken(module))
		{
			Module *m = ModuleManager::FindModule(module);
			if (m != NULL)
				ModuleManager::UnloadModule(m, NULL);
		}

		delete SGLine;
		SGLine = NULL;
		delete SZLine;
		SZLine = NULL;
		delete SQLine;
		SQLine = NULL;
		delete SNLine;
		SNLine = NULL;

		delete OperServ;
	}

	void OnServerQuit(Server *server)
	{
		if (server->HasFlag(SERVER_JUPED))
			ircdproto->SendGlobops(operserv->Bot(), "Received SQUIT for juped server %s", server->GetName().c_str());
	}

	void OnUserModeSet(User *u, UserModeName Name)
	{
		if (Name == UMODE_OPER)
		{
			if (Config->WallOper)
				ircdproto->SendGlobops(OperServ, "\2%s\2 is now an IRC operator.", u->nick.c_str());
			Log(OperServ) << u->nick << " is now an IRC operator";
		}
	}

	void OnUserModeUnset(User *u, UserModeName Name)
	{
		if (Name == UMODE_OPER)
			Log(OperServ) << u->nick << " is no longer an IRC operator";
	}

	void OnUserConnect(dynamic_reference<User> &u, bool &exempt)
	{
		if (u && !exempt)
			XLineManager::CheckAll(u);
	}
};

MODULE_INIT(OperServCore)


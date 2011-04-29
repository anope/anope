/* ChanServ core functions
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
#include "chanserv.h"

static BotInfo *ChanServ = NULL;

class ChanServBotInfo : public BotInfo
{
 public:
	ChanServBotInfo(const Anope::string &bnick, const Anope::string &user = "", const Anope::string &bhost = "", const Anope::string &real = "") : BotInfo(bnick, user, bhost, real) { }

	void OnMessage(User *u, const Anope::string &message)
	{
		PushLanguage("anope", u->Account() ? u->Account()->language : "");
		
		if (!u->HasMode(UMODE_OPER) && Config->CSOpersOnly)
		{
			u->SendMessage(ChanServ, _(ACCESS_DENIED));
			PopLanguage();
			return;
		}

		spacesepstream sep(message);
		Anope::string command, param;
		if (sep.GetToken(command) && sep.GetToken(param))
		{
			Command *c = FindCommand(this, command);
			if (c)
			{
				if (ircdproto->IsChannelValid(param))
				{
					ChannelInfo *ci = cs_findchan(param);
					if (ci)
					{
						if (ci->HasFlag(CI_FORBIDDEN) && !c->HasFlag(CFLAG_ALLOW_FORBIDDEN))
						{
							u->SendMessage(this, _(_(CHAN_X_FORBIDDEN)), ci->name.c_str());
							Log(LOG_COMMAND, "denied", this) << "Access denied for user " << u->GetMask() << " with command " << command << " because of FORBIDDEN channel " << ci->name;
							PopLanguage();
							return;
						}
						else if (ci->HasFlag(CI_SUSPENDED) && !c->HasFlag(CFLAG_ALLOW_SUSPENDED))
						{
							u->SendMessage(this, _(_(CHAN_X_FORBIDDEN)), ci->name.c_str());
							Log(LOG_COMMAND, "denied", this) << "Access denied for user " << u->GetMask() << " with command " << command << " because of SUSPENDED channel " << ci->name;
							PopLanguage();
							return;
						}
					}
					else if (!c->HasFlag(CFLAG_ALLOW_UNREGISTEREDCHANNEL))
					{
						u->SendMessage(this, _(CHAN_X_NOT_REGISTERED), param.c_str());
						PopLanguage();
						return;
					}
				}
				/* A user not giving a channel name for a param that should be a channel */
				else
				{
					u->SendMessage(this, _(CHAN_X_INVALID), param.c_str());
					PopLanguage();
					return;
				}
			}
		}

		PopLanguage();
		BotInfo::OnMessage(u, message);
	}
};

class MyChanServService : public ChanServService
{
 public:
	MyChanServService(Module *m) : ChanServService(m) { }

	BotInfo *Bot()
	{
		return ChanServ;
	}
};

class ExpireCallback : public CallBack
{
 public:
	ExpireCallback(Module *owner) : CallBack(owner, Config->ExpireTimeout, Anope::CurTime, true) { }

	void Tick(time_t)
	{
		if (!Config->CSExpire || noexpire || readonly)
			return;

		for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end; )
		{
			ChannelInfo *ci = it->second;
			++it;

			bool expire = false;
			if (ci->HasFlag(CI_SUSPENDED))
			{
				if (Config->CSSuspendExpire && Anope::CurTime - ci->last_used >= Config->CSSuspendExpire)
					expire = true;
			}
			else if (ci->HasFlag(CI_FORBIDDEN))
			{
				if (Config->CSForbidExpire && Anope::CurTime - ci->last_used >= Config->CSForbidExpire)
					expire = true;
			}
			else if (!ci->c && Anope::CurTime - ci->last_used >= Config->CSExpire)
				expire = true;

			if (ci->HasFlag(CI_NO_EXPIRE))
				expire = false;

			if (expire)
			{
				EventReturn MOD_RESULT;
				FOREACH_RESULT(I_OnPreChanExpire, OnPreChanExpire(ci));
				if (MOD_RESULT == EVENT_STOP)
					continue;

				Anope::string extra;
				if (ci->HasFlag(CI_FORBIDDEN))
					extra = "forbidden ";
				else if (ci->HasFlag(CI_SUSPENDED))
					extra = "suspended ";

				Log(LOG_NORMAL, "chanserv/expire", ChanServ) << "Expiring " << extra  << "channel " << ci->name << " (founder: " << (ci->founder ? ci->founder->display : "(none)") << ")";
				FOREACH_MOD(I_OnChanExpire, OnChanExpire(ci));
				delete ci;
			}
		}
	}
};

class ChanServCore : public Module
{
	MyChanServService mychanserv;
	ExpireCallback expires;

 public:
	ChanServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), mychanserv(this), expires(this)
	{
		this->SetAuthor("Anope");

		ModuleManager::RegisterService(&this->mychanserv);

		ChanServ = new ChanServBotInfo(Config->s_ChanServ, Config->ServiceUser, Config->ServiceHost, Config->desc_ChanServ);
		ChanServ->SetFlag(BI_CORE);

		Implementation i[] = { I_OnDelCore, I_OnDelChan };
		ModuleManager::Attach(i, this, 2);

		spacesepstream coreModules(Config->ChanCoreModules);
		Anope::string module;
		while (coreModules.GetToken(module))
			ModuleManager::LoadModule(module, NULL);
	}

	~ChanServCore()
	{
		spacesepstream coreModules(Config->ChanCoreModules);
		Anope::string module;
		while (coreModules.GetToken(module))
		{
			Module *m = ModuleManager::FindModule(module);
			if (m != NULL)
				ModuleManager::UnloadModule(m, NULL);
		}

		delete ChanServ;
	}

	void OnDelCore(NickCore *nc)
	{
		// XXX this is slightly inefficient
		for (registered_channel_map::const_iterator it = RegisteredChannelList.begin(), it_end = RegisteredChannelList.end(); it != it_end;)
		{
			ChannelInfo *ci = it->second;
			++it;

			if (ci->founder == nc)
			{
				NickCore *newowner = NULL;
				if (ci->successor && (ci->successor->IsServicesOper() || !Config->CSMaxReg || ci->successor->channelcount < Config->CSMaxReg))
					newowner = ci->successor;
				else
				{
					ChanAccess *highest = NULL;
					for (unsigned j = 0; j < ci->GetAccessCount(); ++j)
					{
						ChanAccess *ca = ci->GetAccess(j);
					
						if (!ca->nc || (!ca->nc->IsServicesOper() && Config->CSMaxReg && ca->nc->channelcount >= Config->CSMaxReg) || (ca->nc == nc))
							continue;
						if (!highest || ca->level > highest->level)
							highest = ca;
					}
					if (highest)
						newowner = highest->nc;
				}

				if (newowner)
				{
					Log(LOG_NORMAL, "chanserv/expire") << "Transferring foundership of " << ci->name << " from deleted nick " << nc->display << " to " << newowner->display;
					ci->founder = newowner;
					ci->successor = NULL;
					++newowner->channelcount;
				}
				else
				{
					Log(LOG_NORMAL, "chanserv/expire") << "Deleting channel " << ci->name << " owned by deleted nick " << nc->display;

					delete ci;
					continue;
				}
			}

			if (ci->successor == nc)
				ci->successor = NULL;

			ChanAccess *access = ci->GetAccess(nc);
			if (access)
				ci->EraseAccess(access);

			for (unsigned j = ci->GetAkickCount(); j > 0; --j)
			{
				AutoKick *akick = ci->GetAkick(j - 1);
				if (akick->HasFlag(AK_ISNICK) && akick->nc == nc)
					ci->EraseAkick(j - 1);
			}
		}
	}

	void OnDelChan(ChannelInfo *ci)
	{
		if (ci->c && ci->c->HasMode(CMODE_REGISTERED))
			ci->c->RemoveMode(NULL, CMODE_REGISTERED, "", false);
	}
};

MODULE_INIT(ChanServCore)


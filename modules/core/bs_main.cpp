/* BotServ core functions
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
#include "botserv.h"
#include "chanserv.h"

static BotInfo *BotServ = NULL;

class BotServBotInfo : public BotInfo
{
 public:
	BotServBotInfo(const Anope::string &bnick, const Anope::string &user = "", const Anope::string &bhost = "", const Anope::string &real = "") : BotInfo(bnick, user, bhost, real) { }

	void OnMessage(User *u, const Anope::string &message)
	{
		PushLanguage("anope", u->Account() ? u->Account()->language : "");

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
						u->SendMessage(this, _(_(CHAN_X_NOT_REGISTERED)), param.c_str());
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

class MyBotServService : public BotServService
{
 public:
	MyBotServService(Module *m) : BotServService(m) { }

	BotInfo *Bot()
	{
		return BotServ;
	}

	UserData *GetUserData(User *u, Channel *c)
	{
		UserData *ud = NULL;
		UserContainer *uc = c->FindUser(u);
		if (uc != NULL)
		{
			if (!uc->GetExtPointer("bs_main_userdata", ud))
			{
				ud = new UserData();
				uc->Extend("bs_main_userdata", new ExtensibleItemPointer<UserData>(ud));
			}
		}
		return ud;
	}

	BanData *GetBanData(User *u, Channel *c)
	{
		std::map<Anope::string, BanData> bandatamap;
		if (!c->GetExtRegular("bs_main_bandata", bandatamap));
			c->Extend("bs_main_bandata", new ExtensibleItemRegular<std::map<Anope::string, BanData> >(bandatamap));
		c->GetExtRegular("bs_main_bandata", bandatamap);

		BanData *bd = &bandatamap[u->GetMask()];
		if (bd->last_use && Anope::CurTime - bd->last_use > Config->BSKeepData)
			bd->Clear();
		bd->last_use = Anope::CurTime;
		return bd;
	}
};

class BanDataPurger : public CallBack
{
 public:
	BanDataPurger(Module *owner) : CallBack(owner, 300, Anope::CurTime, true) { }

	void Tick(time_t)
	{
		Log(LOG_DEBUG) << "bs_main: Running bandata purger";

		for (channel_map::iterator it = ChannelList.begin(), it_end = ChannelList.end(); it != it_end; ++it)
		{
			Channel *c = it->second;
			
			std::map<Anope::string, BanData> bandata;
			if (c->GetExtRegular("bs_main_bandata", bandata))
			{
				for (std::map<Anope::string, BanData>::iterator it2 = bandata.begin(), it2_end = bandata.end(); it2 != it2_end; ++it2)
				{
					BanData *bd = &it2->second;

					if (Anope::CurTime - bd->last_use > Config->BSKeepData)
					{
						bandata.erase(it2);
						continue;
					}
				}

				if (bandata.empty())
					c->Shrink("bs_main_bandata");
			}
		}
	}
};

class BotServCore : public Module
{
	MyBotServService mybotserv;
	BanDataPurger bdpurger;

 public:
	BotServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator), mybotserv(this), bdpurger(this)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		ModuleManager::RegisterService(&this->mybotserv);

		BotServ = new BotServBotInfo(Config->s_BotServ, Config->ServiceUser, Config->ServiceHost, Config->desc_BotServ);
		BotServ->SetFlag(BI_CORE);

		Implementation i[] = { I_OnPrivmsg };
		ModuleManager::Attach(i, this, 1);

		spacesepstream coreModules(Config->BotCoreModules);
		Anope::string module;
		while (coreModules.GetToken(module))
			ModuleManager::LoadModule(module, NULL);
	}

	~BotServCore()
	{
		spacesepstream coreModules(Config->BotCoreModules);
		Anope::string module;
		while (coreModules.GetToken(module))
		{
			Module *m = FindModule(module);
			if (m != NULL)
				ModuleManager::UnloadModule(m, NULL);
		}

		delete BotServ;
	}

	void OnPrivmsg(User *u, ChannelInfo *ci, Anope::string &msg)
	{
		if (!u || !ci || !ci->c || !ci->bi || msg.empty())
			return;

		/* Answer to ping if needed */
		if (msg.substr(0, 6).equals_ci("\1PING ") && msg[msg.length() - 1] == '\1')
		{
			Anope::string ctcp = msg;
			ctcp.erase(ctcp.begin());
			ctcp.erase(ctcp.length() - 1);
			ircdproto->SendCTCP(ci->bi, u->nick, "%s", ctcp.c_str());
		}

		bool was_action = false;

		Anope::string realbuf = msg;

		/* If it's a /me, cut the CTCP part because the ACTION will cause
		 * problems with the caps or badwords kicker
		 */
		if (realbuf.substr(0, 8).equals_ci("\1ACTION ") && realbuf[realbuf.length() - 1] == '\1')
		{
			realbuf.erase(0, 8);
			realbuf.erase(realbuf.length() - 1);
			was_action = true;
		}
	
		if (realbuf.empty())
			return;

		/* Fantaisist commands */
		if (ci->botflags.HasFlag(BS_FANTASY) && realbuf[0] == Config->BSFantasyCharacter[0] && !was_action && chanserv)
		{
			/* Strip off the fantasy character */
			realbuf.erase(realbuf.begin());

			size_t space = realbuf.find(' ');
			Anope::string command, rest;
			if (space == Anope::string::npos)
				command = realbuf;
			else
			{
				command = realbuf.substr(0, space);
				rest = realbuf.substr(space + 1);
			}

			if (check_access(u, ci, CA_FANTASIA))
			{
				Command *cmd = FindCommand(chanserv->Bot(), command);

				/* Command exists and can be called by fantasy */
				if (cmd && !cmd->HasFlag(CFLAG_DISABLE_FANTASY))
				{
					Anope::string params = rest;
					/* Some commands don't need the channel name added.. eg !help */
					if (!cmd->HasFlag(CFLAG_STRIP_CHANNEL))
						params = ci->name + " " + params;
					params = command + " " + params;
	
					mod_run_cmd(chanserv->Bot(), u, ci, params);
				}
	
				FOREACH_MOD(I_OnBotFantasy, OnBotFantasy(command, u, ci, rest));
			}
			else
			{
				FOREACH_MOD(I_OnBotNoFantasyAccess, OnBotNoFantasyAccess(command, u, ci, rest));
			}
		}
	}
};

MODULE_INIT(BotServCore)


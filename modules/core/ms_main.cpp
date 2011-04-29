/* MemoServ core functions
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
#include "memoserv.h"

static BotInfo *MemoServ = NULL;
static bool SendMemoMail(NickCore *nc, MemoInfo *mi, Memo *m)
{
	Anope::string message = Anope::printf(GetString(nc,
	"Hi %s\n"
	" \n"
	"You've just received a new memo from %s. This is memo number %d.\n"
	" \n"
	"Memo text:\n"
	" \n"
	"%s").c_str(), nc->display.c_str(), m->sender.c_str(), mi->GetIndex(m), m->text.c_str());

	return Mail(nc, GetString(nc, _("New memo")), message);
}

class MyMemoServService : public MemoServService
{
 public:
	MyMemoServService(Module *m) : MemoServService(m) { }

	BotInfo *Bot()
	{
		return MemoServ;
	}

 	MemoInfo *GetMemoInfo(const Anope::string &target, bool &ischan, bool &isforbid)
	{
		isforbid = false;

		if (!target.empty() && target[0] == '#')
		{
			ischan = true;
			ChannelInfo *ci = cs_findchan(target);
			if (ci != NULL)
			{
				isforbid = ci->HasFlag(CI_FORBIDDEN);
				return &ci->memos;
			}
		}
		else
		{
			ischan = false;
			NickAlias *na = findnick(target);
			if (na != NULL)
			{
				isforbid = na->HasFlag(NS_FORBIDDEN);
				return &na->nc->memos;
			}
		}

		return NULL;
	}

	MemoResult Send(const Anope::string &source, const Anope::string &target, const Anope::string &message, bool force)
	{
		bool ischan, isforbid;
		MemoInfo *mi = this->GetMemoInfo(target, ischan, isforbid);

		if (mi == NULL || isforbid == true)
			return MEMO_INVALID_TARGET;

		User *sender = finduser(source);
		if (sender != NULL && !sender->HasPriv("memoserv/no-limit") && !force)
		{
			if (Config->MSSendDelay > 0 && sender->lastmemosend + Config->MSSendDelay > Anope::CurTime)
				return MEMO_TOO_FAST;
			else if (!mi->memomax)
				return MEMO_TARGET_FULL;
			else if (mi->memomax > 0 && mi->memos.size() >= mi->memomax)
				return MEMO_TARGET_FULL;
			else if (mi->HasIgnore(sender))
				return MEMO_SUCCESS;
		}

		if (sender != NULL)
			sender->lastmemosend = Anope::CurTime;

		Memo *m = new Memo();
		mi->memos.push_back(m);
		m->sender = source;
		m->time = Anope::CurTime;
		m->text = message;
		m->SetFlag(MF_UNREAD);

		FOREACH_MOD(I_OnMemoSend, OnMemoSend(source, target, mi, m));

		if (ischan)
		{
			ChannelInfo *ci = cs_findchan(target);

			if (ci->c)
			{
				for (CUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
				{
					UserContainer *cu = *it;

					if (check_access(cu->user, ci, CA_MEMO))
					{
						if (cu->user->Account() && cu->user->Account()->HasFlag(NI_MEMO_RECEIVE))
							cu->user->SendMessage(MemoServ, _(MEMO_NEW_X_MEMO_ARRIVED), ci->name.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->s_MemoServ.c_str(), ci->name.c_str(), mi->memos.size());
					}
				}
			}
		}
		else
		{
			NickCore *nc = findnick(target)->nc;

			if (nc->HasFlag(NI_MEMO_RECEIVE))
			{
				for (std::list<NickAlias *>::iterator it = nc->aliases.begin(), it_end = nc->aliases.end(); it != it_end; ++it)
				{
					NickAlias *na = *it;
					User *user = finduser(na->nick);
					if (user && user->IsIdentified())
						user->SendMessage(MemoServ, _(MEMO_NEW_MEMO_ARRIVED), source.c_str(), Config->UseStrictPrivMsgString.c_str(), Config->s_MemoServ.c_str(), mi->memos.size());
				}
			}

			/* let's get out the mail if set in the nickcore - certus */
			if (nc->HasFlag(NI_MEMO_MAIL))
				SendMemoMail(nc, mi, m);
		}

		return MEMO_SUCCESS;
	}

	void Check(User *u)
	{
		NickCore *nc = u->Account();
		if (!nc)
			return;

		unsigned i = 0, end = nc->memos.memos.size(), newcnt = 0;
		for (; i < end; ++i)
		{
			if (nc->memos.memos[i]->HasFlag(MF_UNREAD))
				++newcnt;
		}
		if (newcnt > 0)
		{
			u->SendMessage(MemoServ, newcnt == 1 ? _("You have 1 new memo.") : _("You have %d new memos."), newcnt);
			if (newcnt == 1 && (nc->memos.memos[i - 1]->HasFlag(MF_UNREAD)))
				u->SendMessage(MemoServ, _("Type \002%s%s READ LAST\002 to read it."), Config->UseStrictPrivMsgString.c_str(), Config->s_MemoServ.c_str());
			else if (newcnt == 1)
			{
				for (i = 0; i < end; ++i)
				{
					if (nc->memos.memos[i]->HasFlag(MF_UNREAD))
						break;
				}
				u->SendMessage(MemoServ, _("Type \002%s%s READ %d\002 to read it."), Config->UseStrictPrivMsgString.c_str(), Config->s_MemoServ.c_str(), i);
			}
			else
				u->SendMessage(MemoServ, _("Type \002%s%s LIST NEW\002 to list them."), Config->UseStrictPrivMsgString.c_str(), Config->s_MemoServ.c_str());
		}
		if (nc->memos.memomax > 0 && nc->memos.memos.size() >= nc->memos.memomax)
		{
			if (nc->memos.memos.size() > nc->memos.memomax)
				u->SendMessage(MemoServ, _("You are over your maximum number of memos (%d). You will be unable to receive any new memos until you delete some of your current ones."), nc->memos.memomax);
			else
				u->SendMessage(MemoServ, _("You have reached your maximum number of memos (%d). You will be unable to receive any new memos until you delete some of your current ones."), nc->memos.memomax);
		}
	}
};

class MemoServCore : public Module
{
	MyMemoServService mymemoserv;

 public:
	MemoServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE), mymemoserv(this)
	{
		this->SetAuthor("Anope");

		Implementation i[] = { I_OnNickIdentify, I_OnJoinChannel, I_OnUserAway, I_OnNickUpdate };
		ModuleManager::Attach(i, this, 4);

		ModuleManager::RegisterService(&this->mymemoserv);
		
		MemoServ = new BotInfo(Config->s_MemoServ, Config->ServiceUser, Config->ServiceHost, Config->desc_MemoServ);
		MemoServ->SetFlag(BI_CORE);

		spacesepstream coreModules(Config->MemoCoreModules);
		Anope::string module;
		while (coreModules.GetToken(module))
			ModuleManager::LoadModule(module, NULL);
	}

	~MemoServCore()
	{
		spacesepstream coreModules(Config->MemoCoreModules);
		Anope::string module;
		while (coreModules.GetToken(module))
		{
			Module *m = ModuleManager::FindModule(module);
			if (m != NULL)
				ModuleManager::UnloadModule(m, NULL);
		}

		delete MemoServ;
	}

	void OnNickIdentify(User *u)
	{
		this->mymemoserv.Check(u);
	}

	void OnJoinChannel(User *u, Channel *c)
	{
		if (c->ci && check_access(u, c->ci, CA_MEMO) && c->ci->memos.memos.size() > 0)
		{
			if (c->ci->memos.memos.size() == 1)
				u->SendMessage(MemoServ, _("There is \002%d\002 memo on channel %s."), c->ci->memos.memos.size(), c->ci->name.c_str());
			else
				u->SendMessage(MemoServ, _("There are \002%d\002 memos on channel %s."), c->ci->memos.memos.size(), c->ci->name.c_str());
		}
	}

	void OnUserAway(User *u, const Anope::string &message)
	{
		if (message.empty())
			this->mymemoserv.Check(u);
	}

	void OnNickUpdate(User *u)
	{
		this->mymemoserv.Check(u);
	}
};

MODULE_INIT(MemoServCore)


/* MemoServ core functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"
#include "modules/ns_update.h"
#include "modules/help.h"
#include "modules/bs_bot.h"
#include "modules/memoserv.h"
//#include "memoinfo.h"
//#include "memo.h"
//#include "ignore.h"
#include "memotype.h"
#include "memoinfotype.h"
#include "ignoretype.h"

class MemoServCore : public Module, public MemoServ::MemoServService
	, public EventHook<Event::NickCoreCreate>
	, public EventHook<Event::CreateChan>
	, public EventHook<Event::BotDelete>
	, public EventHook<Event::NickIdentify>
	, public EventHook<Event::JoinChannel>
	, public EventHook<Event::UserAway>
	, public EventHook<Event::NickUpdate>
	, public EventHook<Event::Help>
{
	Reference<ServiceBot> MemoServ;
	EventHandlers<MemoServ::Event::MemoSend> onmemosend;
	EventHandlers<MemoServ::Event::MemoDel> onmemodel;

	MemoInfoType memoinfo_type;
	MemoType memo_type;
	IgnoreType ignore_type;

	bool SendMemoMail(NickServ::Account *nc, MemoServ::MemoInfo *mi, MemoServ::Memo *m)
	{
		Anope::string subject = Language::Translate(nc, Config->GetBlock("mail")->Get<Anope::string>("memo_subject").c_str()),
			message = Language::Translate(Config->GetBlock("mail")->Get<Anope::string>("memo_message").c_str());

		subject = subject.replace_all_cs("%n", nc->GetDisplay());
		subject = subject.replace_all_cs("%s", m->GetSender());
		subject = subject.replace_all_cs("%d", stringify(mi->GetIndex(m) + 1));
		subject = subject.replace_all_cs("%t", m->GetText());
		subject = subject.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<Anope::string>("networkname"));

		message = message.replace_all_cs("%n", nc->GetDisplay());
		message = message.replace_all_cs("%s", m->GetSender());
		message = message.replace_all_cs("%d", stringify(mi->GetIndex(m) + 1));
		message = message.replace_all_cs("%t", m->GetText());
		message = message.replace_all_cs("%N", Config->GetBlock("networkinfo")->Get<Anope::string>("networkname"));

		return Mail::Send(nc, subject, message);
	}

 public:
	MemoServCore(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, PSEUDOCLIENT | VENDOR)
		, MemoServ::MemoServService(this)
		, EventHook<Event::NickCoreCreate>()
		, EventHook<Event::CreateChan>()
		, EventHook<Event::BotDelete>()
		, EventHook<Event::NickIdentify>()
		, EventHook<Event::JoinChannel>()
		, EventHook<Event::UserAway>()
		, EventHook<Event::NickUpdate>()
		, EventHook<Event::Help>()
		, onmemosend(this)
		, onmemodel(this)
		, memoinfo_type(this)
		, memo_type(this)
		, ignore_type(this)
	{
	}

	MemoResult Send(const Anope::string &source, const Anope::string &target, const Anope::string &message, bool force) override
	{
		bool ischan, isregistered;
		MemoServ::MemoInfo *mi = GetMemoInfo(target, ischan, isregistered, true);

		if (mi == NULL)
			return MEMO_INVALID_TARGET;

		User *sender = User::Find(source);
		if (sender != NULL && !sender->HasPriv("memoserv/no-limit") && !force)
		{
			time_t send_delay = Config->GetModule("memoserv")->Get<time_t>("senddelay");
			if (send_delay > 0 && sender->lastmemosend + send_delay > Anope::CurTime)
				return MEMO_TOO_FAST;
			else if (!mi->GetMemoMax())
				return MEMO_TARGET_FULL;
			else if (mi->GetMemoMax() > 0 && mi->GetMemos().size() >= static_cast<unsigned>(mi->GetMemoMax()))
				return MEMO_TARGET_FULL;
			else if (mi->HasIgnore(sender))
				return MEMO_SUCCESS;
		}

		if (sender != NULL)
			sender->lastmemosend = Anope::CurTime;

		MemoServ::Memo *m = new MemoImpl(&memo_type);
		m->SetMemoInfo(mi);
		m->SetSender(source);
		m->SetTime(Anope::CurTime);
		m->SetText(message);
		m->SetUnread(true);

		this->onmemosend(&MemoServ::Event::MemoSend::OnMemoSend, source, target, mi, m);

		if (ischan)
		{
			ChanServ::Channel *ci = ChanServ::Find(target);

			if (ci->c)
			{
				for (Channel::ChanUserList::iterator it = ci->c->users.begin(), it_end = ci->c->users.end(); it != it_end; ++it)
				{
					ChanUserContainer *cu = it->second;

					if (ci->AccessFor(cu->user).HasPriv("MEMO"))
					{
						if (cu->user->Account() && cu->user->Account()->HasFieldS("MEMO_RECEIVE"))
							cu->user->SendMessage(*MemoServ, _("There is a new memo on channel \002{0}\002. Type \002{1}{2} READ {3} {4}\002 to read it."), ci->GetName(), Config->StrictPrivmsg, MemoServ->nick, ci->GetName(), mi->GetMemos().size()); // XXX
					}
				}
			}
		}
		else
		{
			NickServ::Account *nc = NickServ::FindNick(target)->GetAccount();

			if (nc->HasFieldS("MEMO_RECEIVE"))
				for (User *u : nc->users)
					u->SendMessage(*MemoServ, _("You have a new memo from \002{0}\002. Type \002{1}{2} READ {3}\002 to read it."), source, Config->StrictPrivmsg, MemoServ->nick, mi->GetMemos().size());//XXX

			/* let's get out the mail if set in the nickcore - certus */
			if (nc->HasFieldS("MEMO_MAIL"))
				SendMemoMail(nc, mi, m);
		}

		return MEMO_SUCCESS;
	}

	void Check(User *u) override
	{
		NickServ::Account *nc = u->Account();
		if (!nc || !nc->GetMemos())
			return;

		auto memos = nc->GetMemos()->GetMemos();
		unsigned i = 0, end = memos.size(), newcnt = 0;
		for (; i < end; ++i)
			if (memos[i]->GetUnread())
				++newcnt;
		if (newcnt > 0)
			u->SendMessage(*MemoServ, newcnt == 1 ? _("You have 1 new memo.") : _("You have %d new memos."), newcnt);
		if (nc->GetMemos()->GetMemoMax() > 0 && memos.size() >= static_cast<unsigned>(nc->GetMemos()->GetMemoMax()))
		{
			if (memos.size() > static_cast<unsigned>(nc->GetMemos()->GetMemoMax()))
				u->SendMessage(*MemoServ, _("You are over your maximum number of memos (%d). You will be unable to receive any new memos until you delete some of your current ones."), nc->GetMemos()->GetMemoMax());
			else
				u->SendMessage(*MemoServ, _("You have reached your maximum number of memos (%d). You will be unable to receive any new memos until you delete some of your current ones."), nc->GetMemos()->GetMemoMax());
		}
	}

	MemoServ::Memo *CreateMemo() override
	{
		return new MemoImpl(&memo_type);
	}

	MemoServ::Ignore *CreateIgnore() override
	{
		return new IgnoreImpl(&ignore_type);
	}

	MemoServ::MemoInfo *GetMemoInfo(const Anope::string &target, bool &is_registered, bool &ischan, bool create) override
	{
		if (!target.empty() && target[0] == '#')
		{
			ischan = true;
			ChanServ::Channel *ci = ChanServ::Find(target);
			if (ci != NULL)
			{
				is_registered = true;
				if (create && !ci->GetMemos())
				{
					MemoServ::MemoInfo *mi = new MemoInfoImpl(&memoinfo_type);
					mi->SetOwner(ci);
				}
				return ci->GetMemos();
			}
			else
				is_registered = false;
		}
		else
		{
			ischan = false;
			NickServ::Nick *na = NickServ::FindNick(target);
			if (na != NULL)
			{
				is_registered = true;
				if (create && !na->GetAccount()->GetMemos())
				{
					MemoServ::MemoInfo *mi = new MemoInfoImpl(&memoinfo_type);
					mi->SetOwner(na->GetAccount());
				}
				return na->GetAccount()->GetMemos();
			}
			else
				is_registered = false;
		}

		return NULL;
	}

	void OnReload(Configuration::Conf *conf) override
	{
		const Anope::string &msnick = conf->GetModule(this)->Get<Anope::string>("client");

		if (msnick.empty())
			throw ConfigException(Module::name + ": <client> must be defined");

		ServiceBot *bi = ServiceBot::Find(msnick, true);
		if (!bi)
			throw ConfigException(Module::name + ": no bot named " + msnick);

		MemoServ = bi;
	}

	void OnNickCoreCreate(NickServ::Account *nc) override
	{
		MemoServ::MemoInfo *mi = new MemoInfoImpl(&memoinfo_type);
		mi->SetOwner(mi);
		mi->SetMemoMax(Config->GetModule(this)->Get<int>("maxmemos"));
	}

	void OnCreateChan(ChanServ::Channel *ci) override
	{
		MemoServ::MemoInfo *mi = new MemoInfoImpl(&memoinfo_type);
		mi->SetOwner(ci);
		mi->SetMemoMax(Config->GetModule(this)->Get<int>("maxmemos"));
	}

	void OnBotDelete(ServiceBot *bi) override
	{
		if (bi == MemoServ)
			MemoServ = NULL;
	}

	void OnNickIdentify(User *u) override
	{
		this->Check(u);
	}

	void OnJoinChannel(User *u, Channel *c) override
	{
		if (u->server && u->server->IsSynced() && c->ci && c->ci->GetMemos() && !c->ci->GetMemos()->GetMemos().empty() && c->ci->AccessFor(u).HasPriv("MEMO"))
		{
			if (c->ci->GetMemos()->GetMemos().size() == 1)
				u->SendMessage(*MemoServ, _("There is \002{0}\002 memo on channel \002{1}\002."), c->ci->GetMemos()->GetMemos().size(), c->ci->GetName());
			else
				u->SendMessage(*MemoServ, _("There are \002{0}\002 memos on channel \002{1}\002."), c->ci->GetMemos()->GetMemos().size(), c->ci->GetName());
		}
	}

	void OnUserAway(User *u, const Anope::string &message) override
	{
		if (message.empty())
			this->Check(u);
	}

	void OnNickUpdate(User *u) override
	{
		this->Check(u);
	}

	EventReturn OnPreHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *MemoServ)
			return EVENT_CONTINUE;
		source.Reply(_("\002%s\002 is a utility allowing IRC users to send short\n"
			"messages to other IRC users, whether they are online at\n"
			"the time or not, or to channels(*). Both the sender's\n"
			"nickname and the target nickname or channel must be\n"
			"registered in order to send a memo.\n"
			"%s's commands include:"), MemoServ->nick.c_str(), MemoServ->nick.c_str());
		return EVENT_CONTINUE;
	}

	void OnPostHelp(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		if (!params.empty() || source.c || source.service != *MemoServ)
			return;
		source.Reply(_(" \n"
			"Type \002%s%s HELP \037command\037\002 for help on any of the\n"
			"above commands."), Config->StrictPrivmsg.c_str(), MemoServ->nick.c_str());
	}
};

MODULE_INIT(MemoServCore)


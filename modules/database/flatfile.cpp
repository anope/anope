/*
 * Anope IRC Services
 *
 * Copyright (C) 2011-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"
#include "modules/botserv/badwords.h"
#include "modules/chanserv/access.h"
#include "modules/chanserv/entrymsg.h"
#include "modules/chanserv/log.h"
#include "modules/chanserv/set_misc.h"
#include "modules/chanserv/suspend.h"
#include "modules/nickserv/access.h"
#include "modules/nickserv/ajoin.h"
#include "modules/nickserv/cert.h"
#include "modules/nickserv/set_misc.h"
#include "modules/nickserv/suspend.h"
#include "modules/operserv/forbid.h"
#include "modules/operserv/news.h"
#include "modules/operserv/info.h"

class DBFlatFile : public Module
	, public EventHook<Event::LoadDatabase>
{
	void LoadAccount(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		NickServ::Account *account = Serialize::New<NickServ::Account *>();

		if (account == nullptr)
			return;

		account->SetDisplay(data["display"]);
		account->SetPassword(data["pass"]);
		account->SetEmail(data["email"]);
		account->SetLanguage(data["language"]);
		account->SetOper(Oper::Find(account->GetDisplay()));

		spacesepstream sep = data["access"];
		for (Anope::string token; sep.GetToken(token);)
		{
			NickAccess *access = Serialize::New<NickAccess *>();
			if (access != nullptr)
			{
				access->SetAccount(account);
				access->SetMask(token);
			}
		}

		MemoServ::MemoInfo *memos = account->GetMemos();
		if (memos != nullptr)
		{
			try
			{
				int16_t memomax = convertTo<int16_t>(data["memomax"]);
				memos->SetMemoMax(memomax);
			}
			catch (const ConvertException &) { }
		}

		sep = data["memoignore"];
		for (Anope::string token; memos && sep.GetToken(token);)
		{
			MemoServ::Ignore *ign = Serialize::New<MemoServ::Ignore *>();
			if (ign != nullptr)
			{
				ign->SetMemoInfo(memos);
				ign->SetMask(token);
			}
		}

		sep = data["cert"];
		for (Anope::string token; sep.GetToken(token);)
		{
			NSCertEntry *e = Serialize::New<NSCertEntry *>();
			if (e != nullptr)
			{
				e->SetAccount(account);
				e->SetCert(token);
			}
		}

		// last_modes
	}

	void LoadNick(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		NickServ::Account *acc = NickServ::FindAccount(data["nc"]);
		if (acc == nullptr)
			return;

		NickServ::Nick *nick = Serialize::New<NickServ::Nick *>();

		if (nick == nullptr)
			return;

		nick->SetNick(data["nick"]);
		nick->SetLastQuit(data["last_quit"]);
		nick->SetLastRealname(data["last_realname"]);
		nick->SetLastUsermask(data["last_usermask"]);
		nick->SetLastRealhost(data["last_realhost"]);
		try
		{
			nick->SetTimeRegistered(convertTo<time_t>(data["time_registered"]));
		}
		catch (const ConvertException &) { }
		try
		{
			nick->SetLastSeen(convertTo<time_t>(data["last_seen"]));
		}
		catch (const ConvertException &) { }
		nick->SetAccount(acc);

		if (data["vhost_host"].empty() == false)
		{
			HostServ::VHost *vhost = Serialize::New<HostServ::VHost *>();
			if (vhost != nullptr)
			{
				vhost->SetOwner(acc);
				vhost->SetIdent(data["vhost_ident"]);
				vhost->SetHost(data["vhost_host"]);
				vhost->SetCreator(data["vhost_creator"]);
				try
				{
					vhost->SetCreated(convertTo<time_t>(data["vhost_time"]));
				}
				catch (const ConvertException &) { }
			}
		}
	}

	void LoadNSSuspend(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		NickServ::Nick *nick = NickServ::FindNick(data["nick"]);

		if (nick == nullptr)
			return;

		NSSuspendInfo *si = Serialize::New<NSSuspendInfo *>();
		if (si == nullptr)
			return;

		si->SetAccount(nick->GetAccount());
		si->SetBy(data["by"]);
		si->SetReason(data["reason"]);
		try
		{
			si->SetWhen(convertTo<time_t>(data["time"]));
		}
		catch (const ConvertException &) { }
		try
		{
			si->SetExpires(convertTo<time_t>(data["expires"]));
		}
		catch (const ConvertException &) { }
	}

	void LoadAJoin(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		NickServ::Nick *nick = NickServ::FindNick(data["nick"]);

		if (nick == nullptr)
			return;

		AutoJoin *entry = Serialize::New<AutoJoin *>();
		if (entry == nullptr)
			return;

		entry->SetAccount(nick->GetAccount());
		entry->SetChannel(data["channel"]);
		entry->SetKey(data["key"]);
	}

	void LoadNSMiscData(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		NickServ::Nick *nick = NickServ::FindNick(data["nc"]);

		if (nick == nullptr)
			return;

		NSMiscData *d = Serialize::New<NSMiscData *>();
		if (d == nullptr)
			return;

		d->SetAccount(nick->GetAccount());
		d->SetName(data["name"]);
		d->SetData(data["data"]);
	}

	void LoadChannel(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		ChanServ::Channel *chan = Serialize::New<ChanServ::Channel *>();

		if (chan == nullptr)
			return;

		chan->SetName(data["name"]);
		chan->SetFounder(NickServ::FindAccount(data["founder"]));
		chan->SetSuccessor(NickServ::FindAccount(data["successor"]));
		chan->SetDesc(data["description"]);
		try
		{
			chan->SetTimeRegistered(convertTo<time_t>(data["time_registered"]));
		}
		catch (const ConvertException &) { }
		try
		{
			chan->SetLastUsed(convertTo<time_t>(data["last_used"]));
		}
		catch (const ConvertException &) { }
		chan->SetLastTopic(data["last_topic"]);
		chan->SetLastTopicSetter(data["last_topic_setter"]);
		try
		{
			chan->SetLastTopicTime(convertTo<time_t>(data["last_topic_time"]));
		}
		catch (const ConvertException &) { }
		try
		{
			chan->SetBanType(convertTo<int16_t>(data["bantype"]));
		}
		catch (const ConvertException &) { }

		spacesepstream sep = data["levels"];
		for (Anope::string lname, lvalue; sep.GetToken(lname) && sep.GetToken(lvalue);)
		{
			try
			{
				chan->SetLevel(lname, convertTo<int16_t>(lvalue));
			}
			catch (const ConvertException &) { }
		}

		if (data["bi"].empty() == false)
		{
			ServiceBot *sb = ServiceBot::Find(data["bi"], true);
			if (sb != nullptr)
				chan->SetBI(sb->bi);
		}

		try
		{
			chan->SetBanExpire(convertTo<time_t>(data["banexpire"]));
		}
		catch (const ConvertException &) { }

		MemoServ::MemoInfo *memos = chan->GetMemos();
		if (memos != nullptr)
		{
			try
			{
				int16_t memomax = convertTo<int16_t>(data["memomax"]);
				memos->SetMemoMax(memomax);
			}
			catch (const ConvertException &) { }
		}

		sep = data["memoignore"];
		for (Anope::string token; memos && sep.GetToken(token);)
		{
			MemoServ::Ignore *ign = Serialize::New<MemoServ::Ignore *>();
			if (ign != nullptr)
			{
				ign->SetMemoInfo(memos);
				ign->SetMask(token);
			}
		}

		// last modes
		// kicker data

		// exts
	}

	void LoadBot(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		if (ServiceBot::Find(data["nick"], true))
			return;

		BotInfo *bot = Serialize::New<BotInfo *>();
		if (bot == nullptr)
			return;

		bot->SetNick(data["nick"]);
		bot->SetUser(data["user"]);
		bot->SetHost(data["host"]);
		bot->SetRealName(data["realname"]);
		try
		{
			bot->SetCreated(convertTo<time_t>(data["created"]));
		}
		catch (const ConvertException &) { }
		bot->SetOperOnly(data["oper_only"] != "0");

		ServiceBot *sb = new ServiceBot(bot->GetNick(), bot->GetUser(), bot->GetHost(), bot->GetRealName());

		sb->bi = bot;
		bot->bot = sb;
	}

	void LoadChanAccess(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		ChanServ::Channel *channel = ChanServ::Find(data["ci"]);
		if (channel == nullptr)
			return;

		Anope::string mask = data["mask"];
		if (mask.find('#') == 0)
			return;

		Anope::string provider = data["provider"];
		ChanServ::ChanAccess *access;

		if (provider == "access/access")
			access = Serialize::New<AccessChanAccess *>();
		else if (provider == "access/xop")
			access = Serialize::New<XOPChanAccess *>();
		else if (provider == "access/flags")
			access = Serialize::New<FlagsChanAccess *>();
		else
			return;

		if (access == nullptr)
			return;

		access->SetChannel(channel);
		access->SetCreator(data["creator"]);
		try
		{
			access->SetLastSeen(convertTo<time_t>(data["last_seen"]));
		}
		catch (const ConvertException &) { }
		try
		{
			access->SetCreated(convertTo<time_t>(data["created"]));
		}
		catch (const ConvertException &) { }
		access->SetMask(mask);

		NickServ::Nick *nick = NickServ::FindNick(mask);
		if (nick != nullptr)
			access->SetObj(nick->GetAccount());

		access->AccessUnserialize(data["data"]);
	}

	void LoadModeLock(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		ChanServ::Channel *channel = ChanServ::Find(data["ci"]);
		if (channel == nullptr)
			return;

		ModeLock *ml = Serialize::New<ModeLock *>();
		ml->SetChannel(channel);
		ml->SetSet(data["set"] == "1");
		ml->SetName(data["name"]);
		ml->SetParam(data["param"]);
		ml->SetSetter(data["setter"]);
		try
		{
			ml->SetCreated(convertTo<time_t>(data["created"]));
		}
		catch (const ConvertException &) { }
	}

	void LoadAutoKick(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		ChanServ::Channel *channel = ChanServ::Find(data["ci"]);
		if (channel == nullptr)
			return;

		NickServ::Nick *nick = nullptr;
		if (data["nc"].empty() == false)
		{
			nick = NickServ::FindNick(data["nc"]);

			if (nick == nullptr)
				return;
		}

		time_t added = 0, lastused = 0;

		try
		{
			added = convertTo<time_t>(data["addtime"]);
		}
		catch (const ConvertException &) { }

		try
		{
			lastused = convertTo<time_t>(data["last_used"]);
		}
		catch (const ConvertException &) { }

		if (nick != nullptr)
			channel->AddAkick(data["creator"], nick->GetAccount(), data["reason"], added, lastused);
		else
			channel->AddAkick(data["creator"], data["mask"], data["reason"], added, lastused);
	}

	void LoadBadWord(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		ChanServ::Channel *channel = ChanServ::Find(data["ci"]);
		if (channel == nullptr)
			return;

		BadWord *bw = Serialize::New<BadWord *>();
		if (bw == nullptr)
			return;

		bw->SetChannel(channel);
		bw->SetWord(data["word"]);
		try
		{
			bw->SetType(static_cast<BadWordType>(convertTo<int>(data["type"])));
		}
		catch (const ConvertException &) { }
	}

	void LoadCSSuspend(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		ChanServ::Channel *channel = ChanServ::Find(data["ci"]);
		if (channel == nullptr)
			return;

		CSSuspendInfo *si = Serialize::New<CSSuspendInfo *>();
		if (si == nullptr)
			return;

		si->SetChannel(channel);
		si->SetBy(data["by"]);
		si->SetReason(data["reason"]);
		try
		{
			si->SetWhen(convertTo<time_t>(data["time"]));
		}
		catch (const ConvertException &) { }
		try
		{
			si->SetExpires(convertTo<time_t>(data["expires"]));
		}
		catch (const ConvertException &) { }
	}

	void LoadEntrymsg(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		ChanServ::Channel *channel = ChanServ::Find(data["ci"]);
		if (channel == nullptr)
			return;

		EntryMsg *msg = Serialize::New<EntryMsg *>();
		if (msg == nullptr)
			return;

		msg->SetChannel(channel);
		msg->SetCreator(data["creator"]);
		msg->SetMessage(data["message"]);
	}

	void LoadLogSetting(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		ChanServ::Channel *channel = ChanServ::Find(data["ci"]);
		if (channel == nullptr)
			return;

		LogSetting *log = Serialize::New<LogSetting *>();
		if (log == nullptr)
			return;

		log->SetChannel(channel);
		log->SetServiceName(data["service_name"]);
		log->SetCommandService(data["command_service"]);
		log->SetCommandName(data["command_name"]);
		log->SetMethod(data["method"]);
		log->SetExtra(data["extra"]);
		try
		{
			log->SetCreated(convertTo<time_t>(data["created"]));
		}
		catch (const ConvertException &) { }
		log->SetCreator(data["creator"]);
	}

	void LoadCSMiscData(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		ChanServ::Channel *channel = ChanServ::Find(data["ci"]);
		if (channel == nullptr)
			return;

		CSMiscData *d = Serialize::New<CSMiscData *>();
		if (d == nullptr)
			return;

		d->SetChannel(channel);
		d->SetName(data["name"]);
		d->SetData(data["data"]);
	}

	void LoadForbid(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		ForbidData *d = Serialize::New<ForbidData *>();
		if (d == nullptr)
			return;

		d->SetMask(data["mask"]);
		d->SetCreator(data["creator"]);
		d->SetReason(data["reason"]);
		try
		{
			d->SetCreated(convertTo<time_t>(data["created"]));
		}
		catch (const ConvertException &) { }
		try
		{
			d->SetExpires(convertTo<time_t>(data["expires"]));
		}
		catch (const ConvertException &) { }
		try
		{
			d->SetType(static_cast<ForbidType>(convertTo<int>(data["type"])));
		}
		catch (const ConvertException &) { }
	}

	void LoadNews(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		NewsItem *ni = Serialize::New<NewsItem *>();
		if (ni == nullptr)
			return;

		try
		{
			ni->SetNewsType(static_cast<NewsType>(convertTo<int>(data["type"])));
		}
		catch (const ConvertException &) { }
		ni->SetText(data["text"]);
		try
		{
			ni->SetTime(convertTo<time_t>(data["time"]));
		}
		catch (const ConvertException &) { }
		ni->SetWho(data["who"]);
	}

	void LoadOperInfo(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		Serialize::Object *target = NickServ::FindAccount(data["target"]);
		if (target == nullptr)
			target = ChanServ::Find(data["target"]);

		if (target == nullptr)
			return;

		OperInfo *o = Serialize::New<OperInfo *>();
		if (o == nullptr)
			return;

		o->SetTarget(target);
		o->SetInfo(data["info"]);
		o->SetCreator(data["adder"]);
		try
		{
			o->SetCreated(convertTo<time_t>(data["created"]));
		}
		catch (const ConvertException &) { }
	}

	void LoadXLine(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		XLine *x = Serialize::New<XLine *>();
		if (x == nullptr)
			return;

		x->SetMask(data["mask"]);
		x->SetBy(data["by"]);
		try
		{
			x->SetExpires(convertTo<time_t>(data["expires"]));
		}
		catch (const ConvertException &) { }
		try
		{
			x->SetCreated(convertTo<time_t>(data["created"]));
		}
		catch (const ConvertException &) { }
		x->SetReason(data["reason"]);
		x->SetID(data["uid"]);
		x->SetType(data["manager"]);
	}

	void LoadOper(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
#warning "oper needs account ref"
#if 0
		data["name"] << this->name;
		data["type"] << this->ot->GetName();
		Oper *o = Serialize::New<Oper *>();
		o->SetName(na->GetAccount()->GetDisplay());
		o->SetType(ot);
		o->SetRequireOper(true);
#endif
	}

	void LoadDNSZone(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
#if 0
		DNSZone *z = Serialize::New<DNSZone *>();
		z->SetName(data["name"]);

		unsigned int count = 0;
		while (data["server" + stringify(count)].empty() == false)
		{
			DNSZoneMembership *mem = Serialize::New<DNSZoneMembership *>();
			mem->SetZone(z);
			mem->SetServer(s);

			++count;
		}
//		data["name"] << name;
//		unsigned count = 0;
//		for (std::set<Anope::string, ci::less>::iterator it = servers.begin(), it_end = servers.end(); it != it_end; ++it)
//			data["server" + stringify(count++)] << *it;
#endif
	}

	void LoadDNSServer(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
#if 0
		s = Serialize::New<DNSServer *>();
		s->SetName(data["server_name"]);

		data["server_name"] << server_name;
		for (unsigned i = 0; i < ips.size(); ++i)
			data["ip" + stringify(i)] << ips[i];
		try
		{
			s->SetLimit(convertTo<unsigned>(data["limit"]));
		}
		catch (const ConvertException &) { }
		try
		{
			s->SetPooled(data["pooled"] == "1");
		}
		catch (const ConvertException &) { }

		unsigned int count = 0;
		while (data["zone" + stringify(count)].empty() == false)
		{
		}
//		for (std::set<Anope::string, ci::less>::iterator it = zones.begin(), it_end = zones.end(); it != it_end; ++it)
//			data["zone" + stringify(count++)] << *it;
#endif
	}

	void LoadMemo(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		MemoServ::MemoInfo *mi = nullptr;

		Anope::string owner = data["owner"];

		NickServ::Nick *nick = NickServ::FindNick(owner);
		ChanServ::Channel *chan = ChanServ::Find(owner);
		if (nick != nullptr)
			mi = nick->GetAccount()->GetMemos();
		else if (chan != nullptr)
			mi = chan->GetMemos();

		if (mi == nullptr)
			return;

		MemoServ::Memo *m = Serialize::New<MemoServ::Memo *>();
		if (m == nullptr)
			return;

		m->SetMemoInfo(mi);
		m->SetSender(data["sender"]);
		try
		{
			m->SetTime(convertTo<time_t>(data["time"]));
		}
		catch (const ConvertException &) { }
		m->SetText(data["text"]);
		m->SetUnread(data["unread"] == "1");
		// receipt
	}

	void LoadType(const Anope::string &type, std::map<Anope::string, Anope::string> &data)
	{
		if (type == "NickCore")
			LoadAccount(type, data);
		else if (type == "NickAlias")
			LoadNick(type, data);
		else if (type == "NSSuspend")
			LoadNSSuspend(type, data);
		else if (type == "AJoinEntry")
			LoadAJoin(type, data);
		else if (type == "NSMiscData")
			LoadNSMiscData(type, data);

		else if (type == "BotInfo")
			LoadBot(type, data);
		else if (type == "BadWord")
			LoadBadWord(type, data);

		else if (type == "ChannelInfo")
			LoadChannel(type, data);
		else if (type == "ChanAccess")
			LoadChanAccess(type, data);
		else if (type == "ModeLock")
			LoadModeLock(type, data);
		else if (type == "AutoKick")
			LoadAutoKick(type, data);
		else if (type == "CSSuspendInfo")
			LoadCSSuspend(type, data);
		else if (type == "EntryMsg")
			LoadEntrymsg(type, data);
		else if (type == "LogSetting")
			LoadLogSetting(type, data);
		else if (type == "CSMiscData")
			LoadCSMiscData(type, data);

		else if (type == "ForbidData")
			LoadForbid(type, data);
		else if (type == "NewsItem")
			LoadNews(type, data);
		else if (type == "OperInfo")
			LoadOperInfo(type, data);
		else if (type == "XLine")
			LoadXLine(type, data);
		else if (type == "Oper")
			LoadOper(type, data);
		else if (type == "DNSZone")
			LoadDNSZone(type, data);
		else if (type == "DNSServer")
			LoadDNSServer(type, data);

		else if (type == "Memo")
			LoadMemo(type, data);
	}

 public:
	DBFlatFile(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, DATABASE | VENDOR)
		, EventHook<Event::LoadDatabase>(this)
	{

	}

	EventReturn OnLoadDatabase() override
	{
		const Anope::string &db_name = Anope::DataDir + "/" + Config->GetModule(this)->Get<Anope::string>("database", "anope.db");

		std::fstream fd(db_name.c_str(), std::ios_base::in | std::ios_base::binary);
		if (!fd.is_open())
		{
			Log(this) << "Unable to open " << db_name << " for reading!";
			return EVENT_STOP;
		}

		Anope::string type;
		std::map<Anope::string, Anope::string> data;
		for (Anope::string buf; std::getline(fd, buf.str());)
		{
			if (buf.find("OBJECT ") == 0)
			{
				type = buf.substr(7);
			}
			else if (buf.find("DATA ") == 0)
			{
				size_t sp = buf.find(' ', 5); // Skip DATA
				if (sp == Anope::string::npos)
					continue;

				Anope::string key = buf.substr(5, sp - 5), value = buf.substr(sp + 1);

				data[key] = value;
			}
			else if (buf.find("END") == 0)
			{
				LoadType(type, data);

				type.empty();
				data.empty();
			}
		}

		fd.close();

		return EVENT_STOP;
	}
};

MODULE_INIT(DBFlatFile)



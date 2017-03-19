/*
 * Anope IRC Services
 *
 * Copyright (C) 2003-2016 Anope Team <team@anope.org>
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

class ChannelImpl : public ChanServ::Channel
{
	friend class ChannelType;

	Serialize::Storage<NickServ::Account *> founder, successor;
	Serialize::Storage<Anope::string> name, desc;
	Serialize::Storage<time_t> time_registered, last_used;
	Serialize::Storage<Anope::string> last_topic, last_topic_setter;
	Serialize::Storage<time_t> last_topic_time;
	Serialize::Storage<int16_t> bantype;
	Serialize::Storage<time_t> banexpire;
	Serialize::Storage<BotInfo *> bi;
	Serialize::Storage<bool> greet;
	Serialize::Storage<bool> fantasy;
	Serialize::Storage<bool> noautoop;
	Serialize::Storage<bool> peace;
	Serialize::Storage<bool> securefounder;
	Serialize::Storage<bool> restricted;
	Serialize::Storage<bool> secure;
	Serialize::Storage<bool> secureops;
	Serialize::Storage<bool> signkick;
	Serialize::Storage<bool> signkicklevel;
	Serialize::Storage<bool> noexpire;
	Serialize::Storage<bool> keepmodes;
	Serialize::Storage<bool> persist;
	Serialize::Storage<bool> topiclock;
	Serialize::Storage<bool> keeptopic;
	Serialize::Storage<bool> _private;

 public:
	using ChanServ::Channel::Channel;
	~ChannelImpl();
	void Delete() override;

	Anope::string GetName() override;
	void SetName(const Anope::string &) override;

	Anope::string GetDesc() override;
	void SetDesc(const Anope::string &) override;

	time_t GetTimeRegistered() override;
	void SetTimeRegistered(time_t) override;

	time_t GetLastUsed() override;
	void SetLastUsed(time_t) override;

	Anope::string GetLastTopic() override;
	void SetLastTopic(const Anope::string &) override;

	Anope::string GetLastTopicSetter() override;
	void SetLastTopicSetter(const Anope::string &) override;

	time_t GetLastTopicTime() override;
	void SetLastTopicTime(time_t) override;

	int16_t GetBanType() override;
	void SetBanType(int16_t) override;

	time_t GetBanExpire() override;
	void SetBanExpire(time_t) override;

	BotInfo *GetBI() override;
	void SetBI(BotInfo *) override;

	ServiceBot *GetBot() override;
	void SetBot(ServiceBot *) override;

	MemoServ::MemoInfo *GetMemos() override;

	void SetFounder(NickServ::Account *nc) override;
	NickServ::Account *GetFounder() override;

	void SetSuccessor(NickServ::Account *nc) override;
	NickServ::Account *GetSuccessor() override;

	bool IsGreet() override;
	void SetGreet(bool) override;

	bool IsFantasy() override;
	void SetFantasy(bool) override;

	bool IsNoAutoop() override;
	void SetNoAutoop(bool) override;

	bool IsPeace() override;
	void SetPeace(bool) override;

	bool IsSecureFounder() override;
	void SetSecureFounder(bool) override;

	bool IsRestricted() override;
	void SetRestricted(bool) override;

	bool IsSecure() override;
	void SetSecure(bool) override;

	bool IsSecureOps() override;
	void SetSecureOps(bool) override;

	bool IsSignKick() override;
	void SetSignKick(bool) override;

	bool IsSignKickLevel() override;
	void SetSignKickLevel(bool) override;

	bool IsNoExpire() override;
	void SetNoExpire(bool) override;

	bool IsKeepModes() override;
	void SetKeepModes(bool) override;

	bool IsPersist() override;
	void SetPersist(bool) override;

	bool IsTopicLock() override;
	void SetTopicLock(bool) override;

	bool IsKeepTopic() override;
	void SetKeepTopic(bool) override;

	bool IsPrivate() override;
	void SetPrivate(bool) override;

	bool IsFounder(const User *user) override;
	ServiceBot *WhoSends() override;
	ChanServ::ChanAccess *GetAccess(unsigned index) override;
	ChanServ::AccessGroup AccessFor(const User *u, bool = true) override;
	ChanServ::AccessGroup AccessFor(NickServ::Account *nc, bool = true) override;
	unsigned GetAccessCount() override;
	void ClearAccess() override;
	AutoKick* AddAkick(const Anope::string &user, NickServ::Account *akicknc, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0) override;
	AutoKick* AddAkick(const Anope::string &user, const Anope::string &mask, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0) override;
	AutoKick* GetAkick(unsigned index) override;
	unsigned GetAkickCount() override;
	void ClearAkick() override;
	int16_t GetLevel(const Anope::string &priv) override;
	void SetLevel(const Anope::string &priv, int16_t level) override;
	void RemoveLevel(const Anope::string &priv) override;
	void ClearLevels() override;
	Anope::string GetIdealBan(User *u) override;
};

/*
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 */

#pragma once

#include "event.h"
#include "channels.h"
#include "modules/nickserv.h"
#include "modules/memoserv.h"
#include "bots.h"

namespace ChanServ
{
	static struct
	{
		Anope::string name;
		Anope::string desc;
	} descriptions[] = {
		{"ACCESS_CHANGE", _("Allowed to modify the access list")},
		{"ACCESS_LIST", _("Allowed to view the access list")},
		{"AKICK", _("Allowed to use the AKICK command")},
		{"ASSIGN", _("Allowed to assign/unassign a bot")},
		{"AUTOHALFOP", _("Automatic halfop upon join")},
		{"AUTOOP", _("Automatic channel operator status upon join")},
		{"AUTOOWNER", _("Automatic owner upon join")},
		{"AUTOPROTECT", _("Automatic protect upon join")},
		{"AUTOVOICE", _("Automatic voice on join")},
		{"BADWORDS", _("Allowed to modify channel badwords list")},
		{"BAN", _("Allowed to ban users")},
		{"FANTASIA", _("Allowed to use fantasy commands")},
		{"FOUNDER", _("Allowed to issue commands restricted to channel founders")},
		{"GETKEY", _("Allowed to use GETKEY command")},
		{"GREET", _("Greet message displayed on join")},
		{"HALFOP", _("Allowed to (de)halfop users")},
		{"HALFOPME", _("Allowed to (de)halfop him/herself")},
		{"INFO", _("Allowed to get full INFO output")},
		{"INVITE", _("Allowed to use the INVITE command")},
		{"KICK", _("Allowed to use the KICK command")},
		{"MEMO", _("Allowed to read channel memos")},
		{"MODE", _("Allowed to use the MODE command")},
		{"NOKICK", _("Prevents users being kicked by Services")},
		{"OP", _("Allowed to (de)op users")},
		{"OPME", _("Allowed to (de)op him/herself")},
		{"OWNER", _("Allowed to (de)owner users")},
		{"OWNERME", _("Allowed to (de)owner him/herself")},
		{"PROTECT", _("Allowed to (de)protect users")},
		{"PROTECTME", _("Allowed to (de)protect him/herself")},
		{"SAY", _("Allowed to use SAY and ACT commands")},
		{"SET", _("Allowed to set channel settings")},
		{"SIGNKICK", _("No signed kick when SIGNKICK LEVEL is used")},
		{"TOPIC", _("Allowed to change channel topics")},
		{"UNBAN", _("Allowed to unban users")},
		{"VOICE", _("Allowed to (de)voice users")},
		{"VOICEME", _("Allowed to (de)voice him/herself")}
	};

	/* A privilege, probably configured using a privilege{} block. Most
	 * commands require specific privileges to be executed. The AccessProvider
	 * backing each ChanAccess determines whether that ChanAccess has a given
	 * privilege.
	 */
	struct CoreExport Privilege
	{
		Anope::string name;
		Anope::string desc;
		/* Rank relative to other privileges */
		int rank;

		Privilege(const Anope::string &n, const Anope::string &d, int r) : name(n), desc(d), rank(r)
		{
			if (this->desc.empty())
				for (unsigned j = 0; j < sizeof(descriptions) / sizeof(*descriptions); ++j)
					if (descriptions[j].name.equals_ci(name))
						this->desc = descriptions[j].desc;
		}

		bool operator==(const Privilege &other) const
		{
			return this->name.equals_ci(other.name);
		}
	};

	class Channel;
	using registered_channel_map = Anope::hash_map<Channel *>;

	class AccessProvider;
	class ChanServService : public Service
	{
	 public:
		ChanServService(Module *m) : Service(m, "ChanServService", "ChanServ")
		{
		}

		virtual Channel *Create(const Anope::string &name) anope_abstract;
		virtual Channel *Create(const Channel &) anope_abstract;
		virtual Channel *Find(const Anope::string &name) anope_abstract;
		virtual registered_channel_map& GetChannels() anope_abstract;

		/* Have ChanServ hold the channel, that is, join and set +nsti and wait
		 * for a few minutes so no one can join or rejoin.
		 */
		virtual void Hold(::Channel *c) anope_abstract;

		virtual void AddPrivilege(Privilege p) anope_abstract;
		virtual void RemovePrivilege(Privilege &p) anope_abstract;
		virtual Privilege *FindPrivilege(const Anope::string &name) anope_abstract;
		virtual std::vector<Privilege> &GetPrivileges() anope_abstract;
		virtual void ClearPrivileges() anope_abstract;

		virtual std::vector<AccessProvider *>& GetProviders() anope_abstract;

		virtual void Destruct(ChanAccess *) anope_abstract;
		virtual void Serialize(const ChanAccess *, Serialize::Data &data) anope_abstract;
		//XXX
		typedef std::multimap<const ChanAccess *, const ChanAccess *> Set;
		typedef std::pair<Set, Set> Path;
		virtual bool Matches(const ChanAccess *, const User *u, const NickServ::Account *acc, Path &p) anope_abstract;
	};
	static ServiceReference<ChanServService> service("ChanServService", "ChanServ");

	inline Channel *Find(const Anope::string name)
	{
		return service ? service->Find(name) : nullptr;
	}

	namespace Event
	{
		struct CoreExport PreChanExpire : Events
		{
			/** Called before a channel expires
			 * @param ci The channel
			 * @param expire Set to true to allow the chan to expire
			 */
			virtual void OnPreChanExpire(Channel *ci, bool &expire) anope_abstract;
		};
		static EventHandlersReference<PreChanExpire> OnPreChanExpire("OnPreChanExpire");

		struct CoreExport ChanExpire : Events
		{
			/** Called before a channel expires
			 * @param ci The channel
			 */
			virtual void OnChanExpire(Channel *ci) anope_abstract;
		};
		static EventHandlersReference<ChanExpire> OnChanExpire("OnChanExpire");
	}

	/* It matters that Base is here before Extensible (it is inherited by Serializable)
	 */
	class CoreExport Channel : public Serializable, public Extensible
	{
	 public:
		/* channels who reference this one */
		Anope::map<int> references;
		Serialize::Reference<NickServ::Account> founder;					/* Channel founder */
		Serialize::Reference<NickServ::Account> successor;                               /* Who gets the channel if the founder nick is dropped or expires */
		Serialize::Checker<std::vector<ChanAccess *> > access;			/* List of authorized users */
		Serialize::Checker<std::vector<AutoKick *> > akick;			/* List of users to kickban */
		Anope::map<int16_t> levels;

		Anope::string name;                       /* Channel name */
		Anope::string desc;

		time_t time_registered;
		time_t last_used;

		Anope::string last_topic;                 /* The last topic that was set on this channel */
		Anope::string last_topic_setter;          /* Setter */
		time_t last_topic_time;	                  /* Time */

		::Channel::ModeList last_modes;             /* The last modes set on this channel */

		int16_t bantype;

		MemoServ::MemoInfo *memos = nullptr;

		::Channel *c;                               /* Pointer to channel, if the channel exists */

		/* For BotServ */
		Serialize::Reference<BotInfo> bi;         /* Bot used on this channel */

		time_t banexpire;                       /* Time bans expire in */

	 protected:
		Channel() : Serializable("ChannelInfo"), access("ChanAccess"), akick("AutoKick") { }

	 public:
		/** Is the user the real founder?
		 * @param user The user
		 * @return true or false
		 */
		virtual bool IsFounder(const User *user) anope_abstract;

		/** Change the founder of the channek
		 * @params nc The new founder
		 */
		virtual void SetFounder(NickServ::Account *nc) anope_abstract;

		/** Get the founder of the channel
		 * @return The founder
		 */
		virtual NickServ::Account *GetFounder() const anope_abstract;

		virtual void SetSuccessor(NickServ::Account *nc) anope_abstract;
		virtual NickServ::Account *GetSuccessor() const anope_abstract;

		/** Find which bot should send mode/topic/etc changes for this channel
		 * @return The bot
		 */
		virtual BotInfo *WhoSends() const anope_abstract;

		/** Add an entry to the channel access list
		 * @param access The entry
		 */
		virtual void AddAccess(ChanAccess *access) anope_abstract;

		/** Get an entry from the channel access list by index
		 *
		 * @param index The index in the access list vector
		 * @return A ChanAccess struct corresponding to the index given, or NULL if outside the bounds
		 *
		 * Retrieves an entry from the access list that matches the given index.
		 */
		virtual ChanAccess *GetAccess(unsigned index) const anope_abstract;

		/** Retrieve the access for a user or group in the form of a vector of access entries
		 * (as multiple entries can affect a single user).
		 */
		virtual AccessGroup AccessFor(const User *u) anope_abstract;
		virtual AccessGroup AccessFor(const NickServ::Account *nc) anope_abstract;

		/** Get the size of the accss vector for this channel
		 * @return The access vector size
		 */
		virtual unsigned GetAccessCount() const anope_abstract;

		/** Get the number of access entries for this channel,
		 * including those that are on other channels.
		 */
		virtual unsigned GetDeepAccessCount() const anope_abstract;

		/** Erase an entry from the channel access list
		 *
		 * @param index The index in the access list vector
		 *
		 * @return The erased entry
		 */
		virtual ChanAccess *EraseAccess(unsigned index) anope_abstract;

		/** Clear the entire channel access list
		 *
		 * Clears the entire access list by deleting every item and then clearing the vector.
		 */
		virtual void ClearAccess() anope_abstract;

		/** Add an akick entry to the channel by NickServ::Account
		 * @param user The user who added the akick
		 * @param akicknc The nickcore being akicked
		 * @param reason The reason for the akick
		 * @param t The time the akick was added, defaults to now
		 * @param lu The time the akick was last used, defaults to never
		 */
		virtual AutoKick* AddAkick(const Anope::string &user, NickServ::Account *akicknc, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0) anope_abstract;

		/** Add an akick entry to the channel by reason
		 * @param user The user who added the akick
		 * @param mask The mask of the akick
		 * @param reason The reason for the akick
		 * @param t The time the akick was added, defaults to now
		 * @param lu The time the akick was last used, defaults to never
		 */
		virtual AutoKick* AddAkick(const Anope::string &user, const Anope::string &mask, const Anope::string &reason, time_t t = Anope::CurTime, time_t lu = 0) anope_abstract;

		/** Get an entry from the channel akick list
		 * @param index The index in the akick vector
		 * @return The akick structure, or NULL if not found
		 */
		virtual AutoKick* GetAkick(unsigned index) const anope_abstract;

		/** Get the size of the akick vector for this channel
		 * @return The akick vector size
		 */
		virtual unsigned GetAkickCount() const anope_abstract;

		/** Erase an entry from the channel akick list
		 * @param index The index of the akick
		 */
		virtual void EraseAkick(unsigned index) anope_abstract;

		/** Clear the whole akick list
		 */
		virtual void ClearAkick() anope_abstract;

		/** Get the level for a privilege
		 * @param priv The privilege name
		 * @return the level
		 * @throws CoreException if priv is not a valid privilege
		 */
		virtual int16_t GetLevel(const Anope::string &priv) const anope_abstract;

		/** Set the level for a privilege
		 * @param priv The privilege priv
		 * @param level The new level
		 */
		virtual void SetLevel(const Anope::string &priv, int16_t level) anope_abstract;

		/** Remove a privilege from the channel
		 * @param priv The privilege
		 */
		virtual void RemoveLevel(const Anope::string &priv) anope_abstract;

		/** Clear all privileges from the channel
		 */
		virtual void ClearLevels() anope_abstract;

		/** Gets a ban mask for the given user based on the bantype
		 * of the channel.
		 * @param u The user
		 * @return A ban mask that affects the user
		 */
		virtual Anope::string GetIdealBan(User *u) const anope_abstract;

		virtual void AddChannelReference(const Anope::string &what) anope_abstract;
		virtual void RemoveChannelReference(const Anope::string &what) anope_abstract;
		virtual void GetChannelReferences(std::deque<Anope::string> &chans) anope_abstract;
	};

	enum
	{
		ACCESS_INVALID = -10000,
		ACCESS_FOUNDER = 10001
	};

	/* A provider of access. Only used for creating ChanAccesses, as
	 * they contain pure virtual functions.
	 */
	class CoreExport AccessProvider : public Service
	{
	 public:
		AccessProvider(Module *o, const Anope::string &n) : Service(o, "AccessProvider", n)
		{
			std::vector<AccessProvider *>& providers = service->GetProviders();
			providers.push_back(this);
		}

		virtual ~AccessProvider()
		{
			std::vector<AccessProvider *>& providers = service->GetProviders();
			std::vector<AccessProvider *>::iterator it = std::find(providers.begin(), providers.end(), this);
			if (it != providers.end())
				providers.erase(it);
		}

		/** Creates a new ChanAccess entry using this provider.
		 * @return The new entry
		 */
		virtual ChanAccess *Create() anope_abstract;
	};

	/* Represents one entry of an access list on a channel. */
	class CoreExport ChanAccess : public Serializable
	{
		Anope::string mask;
		/* account this access entry is for, if any */
		Serialize::Reference<NickServ::Account> nc;

	 public:
		typedef std::multimap<const ChanAccess *, const ChanAccess *> Set;
		/* shows the 'path' taken to determine if an access entry matches a user
		 * .first are access entries checked
		 * .second are access entries which match
		 */
		typedef std::pair<Set, Set> Path;

		/* The provider that created this access entry */
		AccessProvider *provider;
		/* Channel this access entry is on */
		Serialize::Reference<Channel> ci;
		Anope::string creator;
		time_t last_seen;
		time_t created;

		ChanAccess(AccessProvider *p) : Serializable("ChanAccess"), provider(p) { }

		virtual ~ChanAccess()
		{
			service->Destruct(this);
		}

		void SetMask(const Anope::string &mask, Channel *c)
		{
			if (nc != NULL)
				 nc->RemoveChannelReference(this->ci);
			else if (!this->mask.empty())
			 {
				Channel *targc = ChanServ::Find(this->mask);
				if (targc)
					targc->RemoveChannelReference(this->ci->name);
			}

			ci = c;
			this->mask.clear();
			nc = NULL;

			const NickServ::Nick *na = NickServ::FindNick(mask);
			if (na != NULL)
			{
				nc = na->nc;
				nc->AddChannelReference(ci);
			}
			else
			{
				this->mask = mask;

				Channel *targci = ChanServ::Find(mask);
				if (targci != NULL)
					targci->AddChannelReference(ci->name);
			}
		}

		const Anope::string &Mask() const
		{
			if (nc)
				return nc->display;
			else
				return mask;
		}

		NickServ::Account *GetAccount() const { return nc; }

		void Serialize(Serialize::Data &data) const override
		{
			service->Serialize(this, data);
		}

		/** Check if this access entry matches the given user or account
		 * @param u The user
		 * @param nc The account
		 * @param p The path to the access object which matches will be put here
		 */
		virtual bool Matches(const User *u, const NickServ::Account *acc, Path &p) const
		{
			return service->Matches(this, u, acc, p);
		}

		/** Check if this access entry has the given privilege.
		 * @param name The privilege name
		 */
		virtual bool HasPriv(const Anope::string &name) const anope_abstract;

		/** Serialize the access given by this access entry into a human
		 * readable form. chanserv/access will return a number, chanserv/xop
		 * will be AOP, SOP, etc.
		 */
		virtual Anope::string AccessSerialize() const anope_abstract;

		/** Unserialize this access entry from the given data. This data
		 * will be fetched from AccessSerialize.
		 */
		virtual void AccessUnserialize(const Anope::string &data) anope_abstract;

		/* Comparison operators to other Access entries */
		virtual bool operator>(const ChanAccess &other) const
		{
			const std::vector<Privilege> &privs = service->GetPrivileges();
			for (unsigned i = privs.size(); i > 0; --i)
			{
				bool this_p = this->HasPriv(privs[i - 1].name),
					other_p = other.HasPriv(privs[i - 1].name);

				if (!this_p && !other_p)
					continue;

				return this_p && !other_p;
			}

			return false;
		}

		virtual bool operator<(const ChanAccess &other) const
		{
			const std::vector<Privilege> &privs = service->GetPrivileges();
			for (unsigned i = privs.size(); i > 0; --i)
			{
				bool this_p = this->HasPriv(privs[i - 1].name),
					other_p = other.HasPriv(privs[i - 1].name);

				if (!this_p && !other_p)
					continue;

				return !this_p && other_p;
			}

			return false;
		}

		bool operator>=(const ChanAccess &other) const
		{
			return !(*this < other);
		}

		bool operator<=(const ChanAccess &other) const
		{
			return !(*this > other);
		}
	};

	/* A group of access entries. This is used commonly, for example with ChanServ::Channel::AccessFor,
	 * to show what access a user has on a channel because users can match multiple access entries.
	 */
	class CoreExport AccessGroup : public std::vector<ChanAccess *>
	{
	 public:
		/* Channel these access entries are on */
		const ChanServ::Channel *ci;
		/* Path from these entries to other entries that they depend on */
		ChanAccess::Path path;
		/* Account these entries affect, if any */
		const NickServ::Account *nc;
		/* super_admin always gets all privs. founder is a special case where ci->founder == nc */
		bool super_admin, founder;

		AccessGroup()
		{
			this->ci = NULL;
			this->nc = NULL;
			this->super_admin = this->founder = false;
		}

	 private:
		bool HasPriv(const ChanAccess *access, const Anope::string &name) const
		{
			EventReturn MOD_RESULT = ::Event::OnCheckPriv(&::Event::CheckPriv::OnCheckPriv, access, name);
			if (MOD_RESULT == EVENT_ALLOW || access->HasPriv(name))
			{
				typedef std::multimap<const ChanAccess *, const ChanAccess *> P;
				std::pair<P::const_iterator, P::const_iterator> it = this->path.second.equal_range(access);
				if (it.first != it.second)
					/* check all of the paths for this entry */
					for (; it.first != it.second; ++it.first)
					{
						const ChanAccess *a = it.first->second;
						/* if only one path fully matches then we are ok */
						if (HasPriv(a, name))
							return true;
					}
				else
					/* entry is the end of a chain, all entries match, ok */
					return true;
			}

			/* entry does not match or none of the chains fully match */
			return false;
		}

	 public:
		/** Check if this access group has a certain privilege. Eg, it
		 * will check every ChanAccess entry of this group for any that
		 * has the given privilege.
		 * @param priv The privilege
		 * @return true if any entry has the given privilege
		 */
		bool HasPriv(const Anope::string &priv) const
		{
			if (this->super_admin)
				return true;
			else if (!ci || ci->GetLevel(priv) == ACCESS_INVALID)
				return false;

			/* Privileges prefixed with auto are understood to be given
			 * automatically. Sometimes founders want to not automatically
			 * obtain privileges, so we will let them */
			bool auto_mode = !priv.find("AUTO");

			/* Only grant founder privilege if this isn't an auto mode or if they don't match any entries in this group */
			if ((!auto_mode || this->empty()) && this->founder)
				return true;

			EventReturn MOD_RESULT;
			MOD_RESULT = ::Event::OnGroupCheckPriv(&::Event::GroupCheckPriv::OnGroupCheckPriv, this, priv);
			if (MOD_RESULT != EVENT_CONTINUE)
				return MOD_RESULT == EVENT_ALLOW;

			for (unsigned i = this->size(); i > 0; --i)
			{
				ChanAccess *access = this->at(i - 1);

				if (HasPriv(access, priv))
					return true;
			}

			return false;
		}

		/** Get the "highest" access entry from this group of entries.
		 * The highest entry is determined by the entry that has the privilege
		 * with the highest rank (see Privilege::rank).
		 * @return The "highest" entry
		 */
		const ChanAccess *Highest() const
		{
			ChanAccess *highest = NULL;
			for (unsigned i = 0; i < this->size(); ++i)
				if (highest == NULL || *this->at(i) > *highest)
					highest = this->at(i);
			return highest;
		}

		/* Comparison operators to other AccessGroups */
		bool operator>(const AccessGroup &other) const
		{
			if (other.super_admin)
				return false;
			else if (this->super_admin)
				return true;
			else if (other.founder)
				return false;
			else if (this->founder)
				return true;

			const std::vector<Privilege> &privs = service->GetPrivileges();
			for (unsigned i = privs.size(); i > 0; --i)
			{
				bool this_p = this->HasPriv(privs[i - 1].name),
					other_p = other.HasPriv(privs[i - 1].name);

				if (!this_p && !other_p)
					continue;

				return this_p && !other_p;
			}

			return false;
		}

		bool operator<(const AccessGroup &other) const
		{
			if (this->super_admin)
				return false;
			else if (other.super_admin)
				return true;
			else if (this->founder)
				return false;
			else if (other.founder)
				return true;

			const std::vector<Privilege> &privs = service->GetPrivileges();
			for (unsigned i = privs.size(); i > 0; --i)
			{
				bool this_p = this->HasPriv(privs[i - 1].name),
					other_p = other.HasPriv(privs[i - 1].name);

				if (!this_p && !other_p)
					continue;

				return !this_p && other_p;
			}

			return false;
		}

		bool operator>=(const AccessGroup &other) const
		{
			return !(*this < other);
		}

		bool operator<=(const AccessGroup &other) const
		{
			return !(*this > other);
		}
	};
}

/*
 * Anope IRC Services
 *
 * Copyright (C) 2013-2017 Anope Team <team@anope.org>
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

class NSCertEntry;

class CertService : public Service
{
 public:
	static constexpr const char *NAME = "certs";
	
	CertService(Module *c) : Service(c, NAME) { }

	virtual NickServ::Account* FindAccountFromCert(const Anope::string &cert) anope_abstract;

	virtual bool Matches(User *, NickServ::Account *) anope_abstract;

	virtual NSCertEntry *FindCert(const std::vector<NSCertEntry *> &cl, const Anope::string &certfp) anope_abstract;
};

class NSCertEntry : public Serialize::Object
{
 public:
	static constexpr const char *NAME = "nscert";

	using Serialize::Object::Object;

	virtual NickServ::Account *GetAccount() anope_abstract;
	virtual void SetAccount(NickServ::Account *) anope_abstract;

	virtual Anope::string GetCert() anope_abstract;
	virtual void SetCert(const Anope::string &) anope_abstract;
};

namespace Event
{
	struct CoreExport NickCertEvents : Events
	{
		static constexpr const char *NAME = "nickcertevents";

		using Events::Events;

		/** Called when a user adds an entry to their cert list
		 * @param nc The nick
		 * @param entry The entry
		 */
		virtual void OnNickAddCert(NickServ::Account *nc, const Anope::string &entry) anope_abstract;

		/** Called from NickServ::Account::EraseCert()
		 * @param nc pointer to the NickServ::Account
		 * @param entry The fingerprint
		 */
		virtual void OnNickEraseCert(NickServ::Account *nc, const Anope::string &entry) anope_abstract;

		/** called from NickServ::Account::ClearCert()
		 * @param nc pointer to the NickServ::Account
		 */
		virtual void OnNickClearCert(NickServ::Account *nc) anope_abstract;
	};
}


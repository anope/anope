/*
 *
 * (C) 2012-2025 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/encryption.h"
#include "modules/sql.h"

namespace
{
	Module *me;

	ServiceReference<Encryption::Provider> encryption;
	Anope::string password_hash, password_field;
}
class SQLAuthenticationResult final
	: public SQL::Interface
{
	Reference<User> user;
	IdentifyRequest *req;

	void OnFail(const Anope::string &reason)
	{
		Log(LOG_DEBUG) << "sql_authentication: Unsuccessful authentication for " << req->GetAccount() << ": " << reason;
		delete this;
		return;
	}

public:
	SQLAuthenticationResult(User *u, IdentifyRequest *r) : SQL::Interface(me), user(u), req(r)
	{
		req->Hold(me);
	}

	~SQLAuthenticationResult()
	{
		req->Release(me);
	}

	void OnResult(const SQL::Result &r) override
	{
		if (r.Rows() == 0)
			return OnFail("no rows");

		if (!password_hash.empty())
		{
			if (!encryption)
				return OnFail("encryption provider not loaded");

			auto success = false;
			for (auto i = 0; i < r.Rows(); ++i)
			{
				if (encryption->Compare(r.Get(i, password_field), req->GetPassword()))
				{
					success = true;
					break;
				}
			}

			if (!success)
				return OnFail("no matching passwords");
		}

		Log(LOG_DEBUG) << "sql_authentication: Successful authentication for " << req->GetAccount();

		Anope::string email;
		try
		{
			email = r.Get(0, "email");
		}
		catch (const SQL::Exception &) { }

		NickAlias *na = NickAlias::Find(req->GetAccount());
		BotInfo *NickServ = Config->GetClient("NickServ");
		if (na == NULL)
		{
			na = new NickAlias(req->GetAccount(), new NickCore(req->GetAccount()));
			FOREACH_MOD(OnNickRegister, (user, na, ""));
			if (user && NickServ)
				user->SendMessage(NickServ, _("Your account \002%s\002 has been successfully created."), na->nick.c_str());
		}

		if (!email.empty() && email != na->nc->email)
		{
			na->nc->email = email;
			if (user && NickServ)
				user->SendMessage(NickServ, _("Your email has been updated to \002%s\002."), email.c_str());
		}

		req->Success(me);
		delete this;
	}

	void OnError(const SQL::Result &r) override
	{
		Log(this->owner) << "sql_authentication: Error executing query " << r.GetQuery().query << ": " << r.GetError();
		delete this;
	}
};

class ModuleSQLAuthentication final
	: public Module
{
	Anope::string engine;
	Anope::string query;
	Anope::string disable_reason, disable_email_reason;

	ServiceReference<SQL::Provider> SQL;

public:
	ModuleSQLAuthentication(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR)
	{
		me = this;

	}

	void OnReload(Configuration::Conf &conf) override
	{
		const auto &config = conf.GetModule(this);
		this->engine = config.Get<const Anope::string>("engine");
		this->query =  config.Get<const Anope::string>("query");
		this->disable_reason = config.Get<const Anope::string>("disable_reason");
		this->disable_email_reason = config.Get<Anope::string>("disable_email_reason");

		this->SQL = ServiceReference<SQL::Provider>("SQL::Provider", this->engine);

		password_hash = config.Get<const Anope::string>("password_hash");
		if (!password_hash.empty())
		{
			password_field = config.Get<const Anope::string>("password_field", "password");
			encryption = ServiceReference<Encryption::Provider>("Encryption::Provider", password_hash);
		}
	}

	EventReturn OnPreCommand(CommandSource &source, Command *command, std::vector<Anope::string> &params) override
	{
		if (!this->disable_reason.empty() && (command->name == "nickserv/register" || command->name == "nickserv/group"))
		{
			source.Reply(this->disable_reason);
			return EVENT_STOP;
		}

		if (!this->disable_email_reason.empty() && command->name == "nickserv/set/email")
		{
			source.Reply(this->disable_email_reason);
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void OnCheckAuthentication(User *u, IdentifyRequest *req) override
	{
		if (!this->SQL)
		{
			Log(this) << "Unable to find SQL engine";
			return;
		}

		SQL::Query q(this->query);
		q.SetValue("a", req->GetAccount());
		q.SetValue("i", req->GetAddress().str());
		q.SetValue("n", u ? u->nick : "");
		q.SetValue("p", req->GetPassword());

		this->SQL->Run(new SQLAuthenticationResult(u, req), q);

		Log(LOG_DEBUG) << "sql_authentication: Checking authentication for " << req->GetAccount();
	}

	void OnPreNickExpire(NickAlias *na, bool &expire) override
	{
		// We can't let nicks expire if they still have a group or
		// there will be a zombie account left over that can't be
		// authenticated to.
		if (na->nick == na->nc->display && na->nc->aliases->size() > 1)
			expire = false;
	}
};

MODULE_INIT(ModuleSQLAuthentication)

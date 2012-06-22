/* NickServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandNSCert : public Command
{
 private:
	void DoServAdminList(CommandSource &source, const NickCore *nc)
	{
		if (nc->cert.empty())
		{
			source.Reply(_("Certificate list for \002%s\002 is empty."), nc->display.c_str());
			return;
		}

		if (nc->HasFlag(NI_SUSPENDED))
		{
			source.Reply(NICK_X_SUSPENDED, nc->display.c_str());
			return;
		}

		ListFormatter list;
		list.addColumn("Certificate");

		for (unsigned i = 0, end = nc->cert.size(); i < end; ++i)
		{
			Anope::string fingerprint = nc->GetCert(i);
			ListFormatter::ListEntry entry;
			entry["Certificate"] = fingerprint;
			list.addEntry(entry);
		}

		source.Reply(_("Certificate list for \002%s\002:"), nc->display.c_str());

		std::vector<Anope::string> replies;
		list.Process(replies);
		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);

		return;
	}

	void DoAdd(CommandSource &source, NickCore *nc, const Anope::string &mask)
	{

		if (nc->cert.size() >= Config->NSAccessMax)
		{
			source.Reply(_("Sorry, you can only have %d certificate entries for a nickname."), Config->NSAccessMax);
			return;
		}

		if (source.GetUser() && !source.GetUser()->fingerprint.empty() && !nc->FindCert(source.GetUser()->fingerprint))
		{
			nc->AddCert(source.GetUser()->fingerprint);
			source.Reply(_("\002%s\002 added to your certificate list."), source.GetUser()->fingerprint.c_str());
			return;
		}

		if (mask.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return;
		}

		if (nc->FindCert(mask))
		{
			source.Reply(_("Fingerprint \002%s\002 already present on your certificate list."), mask.c_str());
			return;
		}

		nc->AddCert(mask);
		source.Reply(_("\002%s\002 added to your certificate list."), mask.c_str());
		return;
	}

	void DoDel(CommandSource &source, NickCore *nc, const Anope::string &mask)
	{
		if (source.GetUser() && !source.GetUser()->fingerprint.empty() && nc->FindCert(source.GetUser()->fingerprint))
		{
			nc->EraseCert(source.GetUser()->fingerprint);
			source.Reply(_("\002%s\002 deleted from your certificate list."), source.GetUser()->fingerprint.c_str());
			return;
		}

		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return;
		}

		if (!nc->FindCert(mask))
		{
			source.Reply(_("\002%s\002 not found on your certificate list."), mask.c_str());
			return;
		}

		source.Reply(_("\002%s\002 deleted from your certificate list."), mask.c_str());
		nc->EraseCert(mask);

		return;
	}

	void DoList(CommandSource &source, const NickCore *nc)
	{

		if (nc->cert.empty())
		{
			source.Reply(_("Your certificate list is empty."));
			return;
		}

		ListFormatter list;
		list.addColumn("Certificate");

		for (unsigned i = 0, end = nc->cert.size(); i < end; ++i)
		{
			ListFormatter::ListEntry entry;
			entry["Certificate"] = nc->GetCert(i);
			list.addEntry(entry);
		}

		source.Reply(_("Certificate list:"));
		std::vector<Anope::string> replies;
		list.Process(replies);
		for (unsigned i = 0; i < replies.size(); ++i)
			source.Reply(replies[i]);
	}

 public:
	CommandNSCert(Module *creator) : Command(creator, "nickserv/cert", 1, 2)
	{
		this->SetDesc("Modify the nickname client certificate list");
		this->SetSyntax("ADD \037fingerprint\037");
		this->SetSyntax("DEL \037fingerprint\037");
		this->SetSyntax("LIST");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &cmd = params[0];
		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		const NickAlias *na;
		if (cmd.equals_ci("LIST") && source.IsServicesOper() && !mask.empty() && (na = findnick(mask)))
			return this->DoServAdminList(source, na->nc);

		NickCore *nc = source.nc;

		if (source.nc->HasFlag(NI_SUSPENDED))
			source.Reply(NICK_X_SUSPENDED, source.nc->display.c_str());
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, nc, mask);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, nc, mask);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, nc);
		else
			this->OnSyntaxError(source, cmd);

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Modifies or displays the certificate list for your nick.\n"
				"If you connect to IRC and provide a client certificate with a\n"
				"matching fingerprint in the cert list, your nick will be\n"
				"automatically identified to %s.\n"
				" \n"), Config->NickServ.c_str(), Config->NickServ.c_str());
		source.Reply(_("Examples:\n"
				" \n"
				"    \002CERT ADD <fingerprint>\002\n"
				"        Adds this fingerprint to the certificate list and\n"
				"        automatically identifies you when you connect to IRC\n"
				"        using this certificate.\n"
				" \n"
				"    \002CERT DEL <fingerprint>\002\n"
				"        Reverses the previous command.\n"
				" \n"
				"    \002CERT LIST\002\n"
				"        Displays the current certificate list."), Config->NickServ.c_str());
		return true;
	}
};

class NSCert : public Module
{
	CommandNSCert commandnscert;

	void DoAutoIdentify(User *u)
	{
		const BotInfo *bi = findbot(Config->NickServ);
		NickAlias *na = findnick(u->nick);
		if (!bi || !na)
			return;
		if (u->IsIdentified() && u->Account() == na->nc)
			return;
		if (na->nc->HasFlag(NI_SUSPENDED))
			return;
		if (!na->nc->FindCert(u->fingerprint))
			return;

		u->Identify(na);
		u->SendMessage(bi, _("SSL Fingerprint accepted. You are now identified."));
		Log(u) << "automatically identified for account " << na->nc->display << " using a valid SSL fingerprint";
		return;
	}

 public:
	NSCert(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnscert(this)
	{
		this->SetAuthor("Anope");

		if (!ircd || !ircd->certfp)
			throw ModuleException("Your IRCd does not support ssl client certificates");

		Implementation i[] = { I_OnFingerprint };
		ModuleManager::Attach(i, this, sizeof(i) / sizeof(Implementation));

	}

	void OnFingerprint(User *u) anope_override
	{
		DoAutoIdentify(u);
	}
};

MODULE_INIT(NSCert)

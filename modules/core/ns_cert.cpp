/* NickServ core functions
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

class CommandNSCert : public Command
{
 private:
	CommandReturn DoServAdminList(CommandSource &source, NickCore *nc)
	{
		if (nc->cert.empty())
		{
			source.Reply(_("Certificate list for \002%s\002 is empty."), nc->display.c_str());
			return MOD_CONT;
		}

		if (nc->HasFlag(NI_SUSPENDED))
		{
			source.Reply(_(NICK_X_SUSPENDED), nc->display.c_str());
			return MOD_CONT;
		}

		source.Reply(_("Certificate list for \002%s\002:"), nc->display.c_str());
		for (unsigned i = 0, end = nc->cert.size(); i < end; ++i)
		{
			Anope::string fingerprint = nc->GetCert(i);
			source.Reply("    %s", fingerprint.c_str());
		}

		return MOD_CONT;
	}

	CommandReturn DoAdd(CommandSource &source, NickCore *nc, const Anope::string &mask)
	{

		if (nc->cert.size() >= Config->NSAccessMax)
		{
			source.Reply(_("Sorry, you can only have %d certificate entries for a nickname."), Config->NSAccessMax);
			return MOD_CONT;
		}

		if (!source.u->fingerprint.empty() && !nc->FindCert(source.u->fingerprint))
		{
			nc->AddCert(source.u->fingerprint);
			source.Reply(_("\002%s\002 added to your certificate list"), source.u->fingerprint.c_str());
			return MOD_CONT;
		}

		if (mask.empty())
		{
			this->OnSyntaxError(source, "ADD");
			return MOD_CONT;
		}

		if (nc->FindCert(mask))
		{
			source.Reply(_("Fingerprint \002%s\002 already present on your certificate list."), mask.c_str());
			return MOD_CONT;
		}

		nc->AddCert(mask);
		source.Reply(_("\002%s\002 added to your certificate list."), mask.c_str());
		return MOD_CONT;
	}

	CommandReturn DoDel(CommandSource &source, NickCore *nc, const Anope::string &mask)
	{

		if (!source.u->fingerprint.empty() && nc->FindCert(source.u->fingerprint))
		{
			nc->EraseCert(source.u->fingerprint);
			source.Reply(_("\002%s\002 deleted from your certificate list"), source.u->fingerprint.c_str());
			return MOD_CONT;
		}

		if (mask.empty())
		{
			this->OnSyntaxError(source, "DEL");
			return MOD_CONT;
		}

		if (!nc->FindCert(mask))
		{
			source.Reply(_("\002%s\002 not found on your certificate list."), mask.c_str());
			return MOD_CONT;
		}

		source.Reply(_("\002%s\002 deleted from your certificate list."), mask.c_str());
		nc->EraseCert(mask);

		return MOD_CONT;
	}

	CommandReturn DoList(CommandSource &source, NickCore *nc)
	{
		User *u = source.u;

		if (nc->cert.empty())
		{
			source.Reply(_("Your certificate list is empty."), u->nick.c_str());
			return MOD_CONT;
		}

		source.Reply(_("Cert list:"));
		for (unsigned i = 0, end = nc->cert.size(); i < end; ++i)
		{
			Anope::string fingerprint = nc->GetCert(i);
			source.Reply("    %s", fingerprint.c_str());
		}

		return MOD_CONT;
	}

 public:
	CommandNSCert() : Command("CERT", 1, 2)
	{
		this->SetDesc("Modify the nickname client certificate list");
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &cmd = params[0];
		const Anope::string &mask = params.size() > 1 ? params[1] : "";

		NickAlias *na;
		if (cmd.equals_ci("LIST") && u->IsServicesOper() && !mask.empty() && (na = findnick(mask)))
			return this->DoServAdminList(source, na->nc);

		if (u->Account()->HasFlag(NI_SUSPENDED))
			source.Reply(_(NICK_X_SUSPENDED), u->Account()->display.c_str());
		else if (cmd.equals_ci("ADD"))
			return this->DoAdd(source, u->Account(), mask);
		else if (cmd.equals_ci("DEL"))
			return this->DoDel(source, u->Account(), mask);
		else if (cmd.equals_ci("LIST"))
			return this->DoList(source, u->Account());
		else
			this->OnSyntaxError(source, cmd);

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002CERT ADD [\037fingerprint\037]\002\n"
				"        \002CERT DEL [\037<fingerprint>\037]\002\n"
				"        \002CERT LIST\002\n"
				" \n"
				"Modifies or displays the certificate list for your nick.\n"
				"If you connect to IRC and provide a client certificate with a\n"
				"matching fingerprint in the cert list, your nick will be\n"
				"automatically identified to %s.\n"
				" \n"), NickServ->nick.c_str(), NickServ->nick.c_str());
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
				"        Displays the current certificate list."), NickServ->nick.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "CERT", _("CERT {ADD|DEL|LIST} [\037fingerprint\037]"));
	}
};

class NSCert : public Module
{
	CommandNSCert commandnscert;

	void DoAutoIdentify(User *u)
	{
		NickAlias *na = findnick(u->nick);
		if (!na)
			return;
		if (u->IsIdentified() && u->Account() == na->nc)
			return;
		if (na->HasFlag(NS_FORBIDDEN) || na->nc->HasFlag(NI_SUSPENDED))
			return;
		if (!na->nc->FindCert(u->fingerprint))
			return;

		u->Identify(na);
		u->SendMessage(NickServ, _("SSL Fingerprint accepted. You are now identified."));
		return;
	}

 public:
	NSCert(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		if (!ircd->certfp)
			throw ModuleException("Your IRCd does not support ssl client certificates");

		Implementation i[] = { I_OnUserNickChange, I_OnFingerprint };
		ModuleManager::Attach(i, this, 2);

		this->AddCommand(NickServ, &commandnscert);
	}

	void OnFingerprint(User *u)
	{
		DoAutoIdentify(u);
	}

	void OnUserNickChange(User *u, const Anope::string &oldnick)
	{
		if (!u->fingerprint.empty())
			DoAutoIdentify(u);
	}
};

MODULE_INIT(NSCert)

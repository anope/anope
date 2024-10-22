/* NickServ core functions
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "module.h"

class CommandNSDrop final
	: public Command
{
private:
	PrimitiveExtensibleItem<Anope::string> dropcode;

public:
	CommandNSDrop(Module *creator)
		: Command(creator, "nickserv/drop", 1, 2)
		, dropcode(creator, "nickname-dropcode")
	{
		this->SetSyntax(_("\037nickname\037 [\037code\037]"));
		this->SetDesc(_("Cancel the registration of a nickname"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &nick = params[0];

		if (Anope::ReadOnly && !source.HasPriv("nickserv/drop"))
		{
			source.Reply(READ_ONLY_MODE);
			return;
		}

		NickAlias *na = NickAlias::Find(nick);
		if (!na)
		{
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
			return;
		}

		bool is_mine = source.GetAccount() == na->nc;

		if (!is_mine && !source.HasPriv("nickserv/drop"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (Config->GetModule("nickserv")->Get<bool>("secureadmins", "yes") && !is_mine && na->nc->IsServicesOper())
		{
			source.Reply(_("You may not drop other Services Operators' nicknames."));
			return;
		}

		if (na->nc->na == na && na->nc->aliases->size() > 1 && Config->GetModule("nickserv")->Get<bool>("preservedisplay") && !source.HasPriv("nickserv/drop/display"))
		{
			source.Reply(_("You may not drop \002%s\002 as it is the display nick for the account."), na->nick.c_str());
			return;
		}

		auto *code = dropcode.Get(na);
		if (params.size() < 2 || ((!code || !code->equals_ci(params[1])) && (!source.HasPriv("nickserv/drop/override") || params[1] != "OVERRIDE")))
		{
			if (!code)
			{
				code = na->Extend<Anope::string>("nickname-dropcode");
				*code = Anope::Random(15);
			}

			source.Reply(CONFIRM_DROP, na->nick.c_str(), source.service->GetQueryCommand().c_str(),
				na->nick.c_str(), code->c_str());
			return;
		}

		FOREACH_MOD(OnNickDrop, (source, na));

		Log(!is_mine ? LOG_ADMIN : LOG_COMMAND, source, this) << "to drop nickname " << na->nick << " (group: " << na->nc->display << ") (email: " << (!na->nc->email.empty() ? na->nc->email : "none") << ")";
		delete na;

		source.Reply(_("Nickname \002%s\002 has been dropped."), nick.c_str());
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Drops the given nick from the database. Once your nickname\n"
				"is dropped you may lose all of your access and channels that\n"
				"you may own. Any other user will be able to gain control of\n"
				"this nick."));

		source.Reply(" ");
		if (!source.HasPriv("nickserv/drop"))
			source.Reply(_("You may drop any nick within your group."));
		else
			source.Reply(_("As a Services Operator, you may drop any nick."));

		source.Reply(" ");
		if (source.HasPriv("nickserv/drop/override"))
			source.Reply(_("Additionally, Services Operators with the \037nickserv/drop/override\037 permission can\n"
				"replace \037code\037 with \002OVERRIDE\002 to drop without a confirmation code."));
		return true;
	}
};

class NSDrop final
	: public Module
{
	CommandNSDrop commandnsdrop;

public:
	NSDrop(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandnsdrop(this)
	{

	}
};

MODULE_INIT(NSDrop)

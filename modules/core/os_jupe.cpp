/* OperServ core functions
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
#include "operserv.h"

class CommandOSJupe : public Command
{
 public:
	CommandOSJupe() : Command("JUPE", 1, 2, "operserv/jupe")
	{
		this->SetDesc(_("\"Jupiter\" a server"));
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		const Anope::string &jserver = params[0];
		const Anope::string &reason = params.size() > 1 ? params[1] : "";
		Server *server = Server::Find(jserver);

		if (!isValidHost(jserver, 3))
			source.Reply(_("Please use a valid server name when juping"));
		else if (server && (server == Me || server == Me->GetLinks().front()))
			source.Reply(_("You can not jupe your services server or your uplink server."));
		else
		{
			Anope::string rbuf = "Juped by " + u->nick + (!reason.empty() ? ": " + reason : "");
			if (server)
				ircdproto->SendSquit(jserver, rbuf);
			Server *juped_server = new Server(Me, jserver, 1, rbuf, ircd->ts6 ? ts6_sid_retrieve() : "", SERVER_JUPED);
			ircdproto->SendServer(juped_server);

			Log(LOG_ADMIN, u, this) << "on " << jserver << " (" << rbuf << ")";
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: \002JUPE \037server\037 [\037reason\037]\002\n"
				" \n"
				"Tells Services to jupiter a server -- that is, to create\n"
				"a fake \"server\" connected to Services which prevents\n"
				"the real server of that name from connecting.  The jupe\n"
				"may be removed using a standard \002SQUIT\002. If a reason is\n"
				"given, it is placed in the server information field;\n"
				"otherwise, the server information field will contain the\n"
				"text \"Juped by <nick>\", showing the nickname of the\n"
				"person who jupitered the server."));
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		SyntaxError(source, "JUPE", _("JUPE \037servername\037 [\037reason\037]"));
	}
};

class OSJupe : public Module
{
	CommandOSJupe commandosjupe;

 public:
	OSJupe(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		if (!operserv)
			throw ModuleException("OperServ is not loaded!");

		this->AddCommand(operserv->Bot(), &commandosjupe);
	}
};

MODULE_INIT(OSJupe)

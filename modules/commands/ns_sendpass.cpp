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

static bool SendPassMail(User *u, const NickAlias *na, const BotInfo *bi, const Anope::string &pass);

class CommandNSSendPass : public Command
{
 public:
	CommandNSSendPass(Module *creator) : Command(creator, "nickserv/sendpass", 1, 1)
	{
		this->SetFlag(CFLAG_ALLOW_UNREGISTERED);
		this->SetDesc(_("Forgot your password? Try this"));
		this->SetSyntax(_("\037nickname\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		const Anope::string &nick = params[0];
		const NickAlias *na;

		if (Config->RestrictMail && !source.HasCommand("nickserv/sendpass"))
			source.Reply(ACCESS_DENIED);
		else if (!(na = findnick(nick)))
			source.Reply(NICK_X_NOT_REGISTERED, nick.c_str());
		else
		{
			Anope::string tmp_pass;
			if (enc_decrypt(na->nc->pass, tmp_pass) == 1)
			{
				if (SendPassMail(source.GetUser(), na, source.owner, tmp_pass))
				{
					Log(Config->RestrictMail ? LOG_ADMIN : LOG_COMMAND, source, this) << "for " << na->nick;
					source.Reply(_("Password of \002%s\002 has been sent."), nick.c_str());
				}
			}
			else
				source.Reply(_("%s command unavailable because encryption is in use."), source.command.c_str());
		}

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Send the password of the given nickname to the e-mail address\n"
				"set in the nickname record. This command is really useful\n"
				"to deal with lost passwords.\n"
				" \n"
				"May be limited to \002IRC operators\002 on certain networks."));
		return true;
	}
};

class NSSendPass : public Module
{
	CommandNSSendPass commandnssendpass;

 public:
	NSSendPass(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandnssendpass(this)
	{
		this->SetAuthor("Anope");

		if (!Config->UseMail)
			throw ModuleException("Not using mail.");

		Anope::string tmp_pass = "plain:tmp";
		if (enc_decrypt(tmp_pass, tmp_pass) == -1)
			throw ModuleException("Incompatible with the encryption module being used");

	}
};

static bool SendPassMail(User *u, const NickAlias *na, const BotInfo *bi, const Anope::string &pass)
{
	Anope::string subject = translate(na->nc, Config->MailSendpassSubject.c_str());
	Anope::string message = translate(na->nc, Config->MailSendpassMessage.c_str());

	subject = subject.replace_all_cs("%n", na->nick);
	subject = subject.replace_all_cs("%N", Config->NetworkName);
	subject = subject.replace_all_cs("%p", pass);

	message = message.replace_all_cs("%n", na->nick);
	message = message.replace_all_cs("%N", Config->NetworkName);
	message = message.replace_all_cs("%p", pass);

	return Mail(u, na->nc, bi, subject, message);
}

MODULE_INIT(NSSendPass)

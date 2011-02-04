/* ChanServ core functions
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

class CommandCSSetSuccessor : public Command
{
 public:
	CommandCSSetSuccessor(const Anope::string &cpermission = "") : Command("SUCCESSOR", 1, 2, cpermission)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		User *u = source.u;
		ChannelInfo *ci = source.ci;
		if (!ci)
			throw CoreException("NULL ci in CommandCSSetSuccessor");

		if (this->permission.empty() && ci->HasFlag(CI_SECUREFOUNDER) ? !IsFounder(u, ci) : !check_access(u, ci, CA_FOUNDER))
		{
			source.Reply(LanguageString::ACCESS_DENIED);
			return MOD_CONT;
		}

		NickCore *nc;

		if (params.size() > 1)
		{
			NickAlias *na = findnick(params[1]);

			if (!na)
			{
				source.Reply(LanguageString::NICK_X_NOT_REGISTERED, params[1].c_str());
				return MOD_CONT;
			}
			if (na->HasFlag(NS_FORBIDDEN))
			{
				source.Reply(LanguageString::NICK_X_FORBIDDEN, na->nick.c_str());
				return MOD_CONT;
			}
			if (na->nc == ci->founder)
			{
				source.Reply(_("%s cannot be the successor on channel %s because he is its founder."), na->nick.c_str(), ci->name.c_str());
				return MOD_CONT;
			}
			nc = na->nc;
		}
		else
			nc = NULL;

		Log(!this->permission.empty() ? LOG_ADMIN : LOG_COMMAND, u, this, ci) << "to change the successor from " << (ci->successor ? ci->successor->display : "none") << " to " << (nc ? nc->display : "none");

		ci->successor = nc;

		if (nc)
			source.Reply(_("Successor for %s changed to \002%s\002."), ci->name.c_str(), nc->display.c_str());
		else
			source.Reply(_("Successor for \002%s\002 unset."), ci->name.c_str());

		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &)
	{
		source.Reply(_("Syntax: \002%s \037channel\037 SUCCESSOR \037nick\037\002\n"
				" \n"
				"Changes the successor of a channel. If the founder's\n"
				"nickname expires or is dropped while the channel is still\n"
				"registered, the successor will become the new founder of the\n"
				"channel.  However, if the successor already has too many\n"
				"channels registered (%d), the channel will be dropped\n"
				"instead, just as if no successor had been set.  The new\n"
				"nickname must be a registered one."), this->name.c_str());
		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SET", LanguageString::CHAN_SET_SYNTAX);
	}

	void OnServHelp(CommandSource &source)
	{
		source.Reply(_("    SUCCESSOR     Set the successor for a channel"));
	}
};

class CommandCSSASetSuccessor : public CommandCSSetSuccessor
{
 public:
	CommandCSSASetSuccessor() : CommandCSSetSuccessor("chanserv/saset/successor")
	{
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &)
	{
		// XXX
		SyntaxError(source, "SASET", LanguageString::CHAN_SASET_SYNTAX);
	}
};

class CSSetSuccessor : public Module
{
	CommandCSSetSuccessor commandcssetsuccessor;
	CommandCSSASetSuccessor commandcssasetsuccessor;

 public:
	CSSetSuccessor(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->AddSubcommand(this, &commandcssetsuccessor);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->AddSubcommand(this, &commandcssasetsuccessor);
	}

	~CSSetSuccessor()
	{
		Command *c = FindCommand(ChanServ, "SET");
		if (c)
			c->DelSubcommand(&commandcssetsuccessor);

		c = FindCommand(ChanServ, "SASET");
		if (c)
			c->DelSubcommand(&commandcssasetsuccessor);
	}
};

MODULE_INIT(CSSetSuccessor)

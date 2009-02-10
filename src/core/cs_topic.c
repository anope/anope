/* ChanServ core functions
 *
 * (C) 2003-2009 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 * $Id$
 *
 */
/*************************************************************************/

#include "module.h"

void myChanServHelp(User *u);

class CommandCSTopic : public Command
{
 public:
	CommandCSTopic() : Command("TOPIC", 1, 2)
	{
	}

	CommandResult Execute(User *u, std::vector<std::string> &params)
	{
		char *chan = params[0].c_str();
		char *topic = params.size() > 1 ? params[1].c_str() : NULL;

		Channel *c;
		ChannelInfo *ci;

		if (!(c = findchan(chan)))
			notice_lang(s_ChanServ, u, CHAN_X_NOT_IN_USE, chan);
		else if (!(ci = c->ci))
			notice_lang(s_ChanServ, u, CHAN_X_NOT_REGISTERED, c->name);
		else if (ci->flags & CI_FORBIDDEN)
			notice_lang(s_ChanServ, u, CHAN_X_FORBIDDEN, ci->name);
		else if (!is_services_admin(u) && !check_access(u, ci, CA_TOPIC))
			notice_lang(s_ChanServ, u, PERMISSION_DENIED);
		else
		{
			if (ci->last_topic)
				delete [] ci->last_topic;
			ci->last_topic = topic ? sstrdup(topic) : NULL;
			strscpy(ci->last_topic_setter, u->nick, NICKMAX);
			ci->last_topic_time = time(NULL);

			if (c->topic)
				delete [] c->topic;
			c->topic = topic ? sstrdup(topic) : NULL;
			strscpy(c->topic_setter, u->nick, NICKMAX);
			if (ircd->topictsbackward)
				c->topic_time = c->topic_time - 1;
			else
				c->topic_time = ci->last_topic_time;

			if (is_services_admin(u) && !check_access(u, ci, CA_TOPIC))
				alog("%s: %s!%s@%s changed topic of %s as services admin.", s_ChanServ, u->nick, u->GetIdent().c_str(), u->host, c->name);
			if (ircd->join2set && whosends(ci) == findbot(s_ChanServ))
			{
				ircdproto->SendJoin(findbot(s_ChanServ), c->name, c->creation_time);
				ircdproto->SendMode(NULL, c->name, "+o %s", s_ChanServ);
			}
			ircdproto->SendTopic(whosends(ci), c->name, u->nick, topic ? topic : "", c->topic_time);
			if (ircd->join2set && whosends(ci) == findbot(s_ChanServ))
				ircdproto->SendPart(findbot(s_ChanServ), c->name, NULL);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const std::string &subcommand)
	{
		notice_lang(s_ChanServ, u, CHAN_HELP_TOPIC);
		return true;
	}

	void OnSyntaxError(User *u)
	{
		syntax_error(s_ChanServ, u, "TOPIC", CHAN_TOPIC_SYNTAX);
	}
};

class CSTopic : public Module
{
 public:
	CSTopic(const std::string &modname, const std::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetVersion("$Id$");
		this->SetType(CORE);

		this->AddCommand(CHANSERV, new CommandCSTopic(), MOD_UNIQUE);

		this->SetChanHelp(myChanServHelp);
	}
};

/**
 * Add the help response to anopes /cs help output.
 * @param u The user who is requesting help
 **/
void myChanServHelp(User *u)
{
	notice_lang(s_ChanServ, u, CHAN_HELP_CMD_TOPIC);
}

MODULE_INIT("cs_topic", CSTopic)

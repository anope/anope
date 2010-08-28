/* ChanServ core functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandCSTopic : public Command
{
 public:
	CommandCSTopic() : Command("TOPIC", 1, 2)
	{
	}

	CommandReturn Execute(User *u, const std::vector<Anope::string> &params)
	{
		Anope::string chan = params[0];
		Anope::string topic = params.size() > 1 ? params[1] : "";

		ChannelInfo *ci = cs_findchan(params[0]);
		Channel *c = ci->c;

		if (!c)
			notice_lang(Config->s_ChanServ, u, CHAN_X_NOT_IN_USE, chan.c_str());
		else if (!check_access(u, ci, CA_TOPIC) && !u->Account()->HasCommand("chanserv/topic"))
			notice_lang(Config->s_ChanServ, u, ACCESS_DENIED);
		else
		{
			ci->last_topic = topic;
			ci->last_topic_setter = u->nick;
			ci->last_topic_time = time(NULL);

			c->topic = topic;
			c->topic_setter = u->nick;
			if (ircd->topictsbackward)
				c->topic_time = c->topic_time - 1;
			else
				c->topic_time = ci->last_topic_time;

			bool override = !check_access(u, ci, CA_TOPIC);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "to change the topic to " << (!topic.empty() ? topic : "No topic");

			if (ircd->join2set && whosends(ci) == ChanServ) // XXX what if the service bot is chanserv?
			{
				ChanServ->Join(c);
				ircdproto->SendMode(NULL, c, "+o %s", Config->s_ChanServ.c_str()); // XXX
			}
			ircdproto->SendTopic(whosends(ci), c, u->nick, topic);
			if (ircd->join2set && whosends(ci) == ChanServ)
				ChanServ->Part(c);
		}
		return MOD_CONT;
	}

	bool OnHelp(User *u, const Anope::string &subcommand)
	{
		notice_help(Config->s_ChanServ, u, CHAN_HELP_TOPIC);
		return true;
	}

	void OnSyntaxError(User *u, const Anope::string &subcommand)
	{
		syntax_error(Config->s_ChanServ, u, "TOPIC", CHAN_TOPIC_SYNTAX);
	}

	void OnServHelp(User *u)
	{
		notice_lang(Config->s_ChanServ, u, CHAN_HELP_CMD_TOPIC);
	}
};

class CSTopic : public Module
{
	CommandCSTopic commandcstopic;

 public:
	CSTopic(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		this->SetAuthor("Anope");
		this->SetType(CORE);

		this->AddCommand(ChanServ, &commandcstopic);
	}
};

MODULE_INIT(CSTopic)

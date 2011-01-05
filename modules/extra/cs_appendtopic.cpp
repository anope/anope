/* cs_appendtopic.c - Add text to a channels topic
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Based on the original module by SGR <Alex_SGR@ntlworld.com>
 * Included in the Anope module pack since Anope 1.7.9
 * Anope Coder: GeniusDex <geniusdex@anope.org>
 *
 * Please read COPYING and README for further details.
 *
 * Send bug reports to the Anope Coder instead of the module
 * author, because any changes since the inclusion into anope
 * are not supported by the original author.
 */

/*************************************************************************/

#include "module.h"

#define AUTHOR "SGR"

/* ------------------------------------------------------------
 * Name: cs_appendtopic
 * Author: SGR <Alex_SGR@ntlworld.com>
 * Date: 31/08/2003
 * ------------------------------------------------------------
 *
 * This module has no configurable options. For information on
 * this module, load it and refer to /ChanServ APPENDTOPIC HELP
 *
 * Thanks to dengel, Rob and Certus for all there support.
 * Especially Rob, who always manages to show me where I have
 * not allocated any memory. Even if it takes a few weeks of
 * pestering to get him to look at it.
 *
 * ------------------------------------------------------------
 */

/* ---------------------------------------------------------------------- */
/* DO NOT EDIT BELOW THIS LINE UNLESS YOU KNOW WHAT YOU ARE DOING         */
/* ---------------------------------------------------------------------- */

static Module *me;

class CommandCSAppendTopic : public Command
{
 public:
	CommandCSAppendTopic() : Command("APPENDTOPIC", 2, 2)
	{
	}

	CommandReturn Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &newtopic = params[1];

		User *u = source.u;
		ChannelInfo *ci = source.ci;
		Channel *c = ci->c;

		if (!c)
			u->SendMessage(ChanServ, CHAN_X_NOT_IN_USE, ci->name.c_str());
		else if (!check_access(u, ci, CA_TOPIC))
			u->SendMessage(ChanServ, ACCESS_DENIED);
		else
		{
			Anope::string topic;
			if (!ci->last_topic.empty())
			{
				topic = ci->last_topic + " " + newtopic;
				ci->last_topic.clear();
			}
			else
				topic = newtopic;

			bool has_topiclock = ci->HasFlag(CI_TOPICLOCK);
			ci->UnsetFlag(CI_TOPICLOCK);
			c->ChangeTopic(u->nick, topic, Anope::CurTime);
			if (has_topiclock)
				ci->SetFlag(CI_TOPICLOCK);

			bool override = !check_access(u, ci, CA_TOPIC);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, ci) << "changed topic to " << topic;
		}
		return MOD_CONT;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		me->SendMessage(source, _("Syntax: APPENDTOPIC channel text"));
		source.Reply(" ");
		me->SendMessage(source, _("This command allows users to append text to a currently set\n"
			"channel topic. When TOPICLOCK is on, the topic is updated and\n"
			"the new, updated topic is locked."));

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		me->SendMessage(source, _("Syntax: APPENDTOPIC channel text"));
	}

	void OnServHelp(CommandSource &source)
	{
		me->SendMessage(source, _("   APPENDTOPIC Add text to a channels topic"));
	}
};

class CSAppendTopic : public Module
{
	CommandCSAppendTopic commandcsappendtopic;

 public:
	CSAppendTopic(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator)
	{
		me = this;

		this->SetAuthor(AUTHOR);
		this->SetType(SUPPORTED);

		this->AddCommand(ChanServ, &commandcsappendtopic);
	}
};

MODULE_INIT(CSAppendTopic)

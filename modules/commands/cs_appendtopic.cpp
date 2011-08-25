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

class CommandCSAppendTopic : public Command
{
 public:
	CommandCSAppendTopic(Module *creator) : Command(creator, "chanserv/appendtopic", 2, 2)
	{
		this->SetDesc(_("Add text to a channels topic"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params)
	{
		const Anope::string &newtopic = params[1];

		User *u = source.u;
		Channel *c = findchan(params[0]);;

		if (!c)
			source.Reply(CHAN_X_NOT_IN_USE, params[0].c_str());
		else if (!c->ci)
			source.Reply(CHAN_X_NOT_REGISTERED, c->name.c_str());
		else if (!c->ci->AccessFor(u).HasPriv(CA_TOPIC))
			source.Reply(ACCESS_DENIED);
		else
		{
			Anope::string topic;
			if (!c->ci->last_topic.empty())
			{
				topic = c->ci->last_topic + " " + newtopic;
				c->ci->last_topic.clear();
			}
			else
				topic = newtopic;

			bool has_topiclock = c->ci->HasFlag(CI_TOPICLOCK);
			c->ci->UnsetFlag(CI_TOPICLOCK);
			c->ChangeTopic(u->nick, topic, Anope::CurTime);
			if (has_topiclock)
				c->ci->SetFlag(CI_TOPICLOCK);

			bool override = c->ci->AccessFor(u).HasPriv(CA_TOPIC);
			Log(override ? LOG_OVERRIDE : LOG_COMMAND, u, this, c->ci) << "changed topic to " << topic;
		}
		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: APPENDTOPIC channel text"));
		source.Reply(" ");
		source.Reply(("This command allows users to append text to a currently set\n"
			"channel topic. When TOPICLOCK is on, the topic is updated and\n"
			"the new, updated topic is locked."));

		return true;
	}

	void OnSyntaxError(CommandSource &source, const Anope::string &subcommand)
	{
		source.Reply(_("Syntax: APPENDTOPIC channel text"));
	}
};

class CSAppendTopic : public Module
{
	CommandCSAppendTopic commandcsappendtopic;

 public:
	CSAppendTopic(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcsappendtopic(this)
	{
		this->SetAuthor("SGR");

	}
};

MODULE_INIT(CSAppendTopic)

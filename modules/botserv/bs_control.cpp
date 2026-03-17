// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include "module.h"

class CommandBSSay final
	: public Command
{
public:
	CommandBSSay(Module *creator) : Command(creator, "botserv/say", 2, 2)
	{
		this->SetDesc(_("Makes the bot say the specified text on the specified channel"));
		this->SetSyntax(_("\037channel\037 \037text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		const Anope::string &text = params[1];

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!source.AccessFor(ci).HasPriv("SAY") && !source.HasPriv("botserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (!ci->bi)
		{
			source.Reply(BOT_NOT_ASSIGNED);
			return;
		}

		if (!ci->c || !ci->c->FindUser(ci->bi))
		{
			source.Reply(BOT_NOT_ON_CHANNEL, ci->name.c_str());
			return;
		}

		if (text[0] == '\001')
		{
			this->OnSyntaxError(source, "");
			return;
		}

		IRCD->SendPrivmsg(*ci->bi, ci->name, text);
		ci->bi->lastmsg = Anope::CurTime;

		bool override = !source.AccessFor(ci).HasPriv("SAY");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to say: " << text;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Makes the bot say the specified text on the specified channel."));

		ExampleWrapper()
			.AddEntry(_("#chat hello all"), _(
				"Sends a message to \035#chat\035 saying \035hello all\035."
			))
			.SendTo(source);

		return true;
	}
};

class CommandBSAct final
	: public Command
{
public:
	CommandBSAct(Module *creator) : Command(creator, "botserv/act", 2, 2)
	{
		this->SetDesc(_("Makes the bot do the equivalent of a \"/me\" command"));
		this->SetSyntax(_("\037channel\037 \037text\037"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) override
	{
		Anope::string message = params[1];

		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}

		if (!source.AccessFor(ci).HasPriv("SAY") && !source.HasPriv("botserv/administration"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (!ci->bi)
		{
			source.Reply(BOT_NOT_ASSIGNED);
			return;
		}

		if (!ci->c || !ci->c->FindUser(ci->bi))
		{
			source.Reply(BOT_NOT_ON_CHANNEL, ci->name.c_str());
			return;
		}

		IRCD->SendPrivmsg(*ci->bi, ci->name, Anope::FormatCTCP("ACTION", message));
		ci->bi->lastmsg = Anope::CurTime;

		bool override = !source.AccessFor(ci).HasPriv("SAY");
		Log(override ? LOG_OVERRIDE : LOG_COMMAND, source, this, ci) << "to say: " << message;
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_(
			"Makes the bot do the equivalent of a \"/me\" command on the specified channel using "
			"the specified text."
		));

		ExampleWrapper()
			.AddEntry(_("#chat is eating pizza"), _(
				"Sends an action message to \035#chat\035 saying \035is eating pizza\035."
			))
			.SendTo(source);

		return true;
	}
};

class BSControl final
	: public Module
{
	CommandBSSay commandbssay;
	CommandBSAct commandbsact;

public:
	BSControl(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, VENDOR),
		commandbssay(this), commandbsact(this)
	{

	}
};

MODULE_INIT(BSControl)

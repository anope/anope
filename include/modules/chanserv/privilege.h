/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#pragma once

namespace ChanServ
{

static struct
{
	Anope::string name;
	Anope::string desc;
} descriptions[] = {
	{"ACCESS_CHANGE", _("Allowed to modify the access list")},
	{"ACCESS_LIST", _("Allowed to view the access list")},
	{"AKICK", _("Allowed to use the AKICK command")},
	{"ASSIGN", _("Allowed to assign/unassign a bot")},
	{"AUTOHALFOP", _("Automatic halfop upon join")},
	{"AUTOOP", _("Automatic channel operator status upon join")},
	{"AUTOOWNER", _("Automatic owner upon join")},
	{"AUTOPROTECT", _("Automatic protect upon join")},
	{"AUTOVOICE", _("Automatic voice on join")},
	{"BADWORDS", _("Allowed to modify channel badwords list")},
	{"BAN", _("Allowed to ban users")},
	{"FANTASIA", _("Allowed to use fantasy commands")},
	{"FOUNDER", _("Allowed to issue commands restricted to channel founders")},
	{"GETKEY", _("Allowed to use GETKEY command")},
	{"GREET", _("Greet message displayed on join")},
	{"HALFOP", _("Allowed to (de)halfop users")},
	{"HALFOPME", _("Allowed to (de)halfop him/herself")},
	{"INFO", _("Allowed to get full INFO output")},
	{"INVITE", _("Allowed to use the INVITE command")},
	{"KICK", _("Allowed to use the KICK command")},
	{"MEMO", _("Allowed to read channel memos")},
	{"MODE", _("Allowed to use the MODE command")},
	{"NOKICK", _("Prevents users being kicked by Services")},
	{"OP", _("Allowed to (de)op users")},
	{"OPME", _("Allowed to (de)op him/herself")},
	{"OWNER", _("Allowed to (de)owner users")},
	{"OWNERME", _("Allowed to (de)owner him/herself")},
	{"PROTECT", _("Allowed to (de)protect users")},
	{"PROTECTME", _("Allowed to (de)protect him/herself")},
	{"SAY", _("Allowed to use SAY and ACT commands")},
	{"SET", _("Allowed to set channel settings")},
	{"SIGNKICK", _("No signed kick when SIGNKICK LEVEL is used")},
	{"TOPIC", _("Allowed to change channel topics")},
	{"UNBAN", _("Allowed to unban users")},
	{"VOICE", _("Allowed to (de)voice users")},
	{"VOICEME", _("Allowed to (de)voice him/herself")}
};

/* A privilege, probably configured using a privilege{} block. Most
 * commands require specific privileges to be executed.
 */
struct CoreExport Privilege
{
	Anope::string name;
	Anope::string desc;
	/* Rank relative to other privileges */
	int rank;
	int level;

	Privilege(const Anope::string &n, const Anope::string &d, int r, int l) : name(n), desc(d), rank(r), level(l)
	{
		if (this->desc.empty())
			for (unsigned j = 0; j < sizeof(descriptions) / sizeof(*descriptions); ++j)
				if (descriptions[j].name.equals_ci(name))
					this->desc = descriptions[j].desc;
	}

	bool operator==(const Privilege &other) const
	{
		return this->name.equals_ci(other.name);
	}
};

} // namespace ChanServ
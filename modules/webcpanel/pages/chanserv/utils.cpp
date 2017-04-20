/*
 * Anope IRC Services
 *
 * Copyright (C) 2012-2017 Anope Team <team@anope.org>
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

#include "../../webcpanel.h"

namespace
{
	bool ChannelSort(ChanServ::Channel *ci1, ChanServ::Channel *ci2)
	{
		return ci::less()(ci1->GetName(), ci2->GetName());
	}
}

namespace WebCPanel
{

namespace ChanServ
{

void BuildChanList(::NickServ::Nick *na, TemplateFileServer::Replacements &replacements)
{
	std::vector<::ChanServ::Channel *> chans = na->GetAccount()->GetRefs<::ChanServ::Channel *>();
	std::sort(chans.begin(), chans.end(), ChannelSort);

	for (::ChanServ::Channel *ci : chans)
	{
		if (na->GetAccount() != ci->GetFounder() && ci->AccessFor(na->GetAccount()).empty())
			continue;

		replacements["CHANNEL_NAMES"] = ci->GetName();
		replacements["ESCAPED_CHANNEL_NAMES"] = HTTPUtils::URLEncode(ci->GetName());
	}
}

}

}


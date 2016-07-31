/*
 * Anope IRC Services
 *
 * Copyright (C) 2014-2016 Anope Team <team@anope.org>
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

#include "main/chanaccess.h"

namespace Event
{
	struct CoreExport LevelChange : Events
	{
		static constexpr const char *NAME = "levelchange";

		using Events::Events;
		
		/** Called when a level for a channel is changed
		 * @param source The source of the command
		 * @param ci The channel the level was changed on
		 * @param priv The privilege changed
		 * @param what The new level
		 */
		virtual void OnLevelChange(CommandSource &source, ChanServ::Channel *ci, const Anope::string &priv, int16_t what) anope_abstract;
	};
}

class AccessChanAccess : public ChanAccessImpl
{
 public:
	static constexpr const char *NAME = "accesschanaccess";

	using ChanAccessImpl::ChanAccessImpl;

	virtual int GetLevel() anope_abstract;
	virtual void SetLevel(const int &) anope_abstract;
};

class XOPChanAccess : public ChanAccessImpl
{
 public:
	static constexpr const char *NAME = "xopchanaccess";

	using ChanAccessImpl::ChanAccessImpl;

	virtual const Anope::string &GetType() anope_abstract;
	virtual void SetType(const Anope::string &) anope_abstract;
};

class FlagsChanAccess : public ChanAccessImpl
{
  public:
	static constexpr const char *NAME = "flagschanaccess";

	using ChanAccessImpl::ChanAccessImpl;

	virtual const Anope::string &GetFlags() anope_abstract;
	virtual void SetFlags(const Anope::string &) anope_abstract;
};


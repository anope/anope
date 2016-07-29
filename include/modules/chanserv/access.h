/*
 *
 * (C) 2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
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


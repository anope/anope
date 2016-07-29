#include "level.h"

class LevelType : public Serialize::Type<LevelImpl>
{
 public:
	Serialize::ObjectField<LevelImpl, ChanServ::Channel *> channel;
	Serialize::Field<LevelImpl, Anope::string> name;
	Serialize::Field<LevelImpl, int> level;

	LevelType(Module *creator) : Serialize::Type<LevelImpl>(creator)
		, channel(this, "channel", &LevelImpl::channel, true)
		, name(this, "name", &LevelImpl::name)
		, level(this, "level", &LevelImpl::level)
	{
	}
};

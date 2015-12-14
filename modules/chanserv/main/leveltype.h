#include "level.h"

class LevelType : public Serialize::Type<LevelImpl>
{
 public:
	Serialize::ObjectField<LevelImpl, ChanServ::Channel *> channel;
	Serialize::Field<LevelImpl, Anope::string> name;
	Serialize::Field<LevelImpl, int> level;

	LevelType(Module *creator) : Serialize::Type<LevelImpl>(creator, "Level")
		, channel(this, "channel", true)
		, name(this, "name")
		, level(this, "level")
	{
	}
};

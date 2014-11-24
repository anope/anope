#include "mode.h"

class CSModeType : public Serialize::Type<ModeImpl>
{
 public:
	Serialize::ObjectField<ModeImpl, ChanServ::Channel *> channel;
	Serialize::Field<ModeImpl, Anope::string> mode, param;

	CSModeType(Module *creator) : Serialize::Type<ModeImpl>(creator, "CSKeepMode")
		, channel(this, "channel", true)
		, mode(this, "mode")
		, param(this, "param")
	{
	}
};

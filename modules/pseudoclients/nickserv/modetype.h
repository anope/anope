#include "mode.h"

class NSModeType : public Serialize::Type<ModeImpl>
{
 public:
	Serialize::ObjectField<ModeImpl, NickServ::Account *> account;
	Serialize::Field<ModeImpl, Anope::string> mode;

	NSModeType(Module *creator) : Serialize::Type<ModeImpl>(creator, "NSKeepMode")
		, account(this, "account", true)
		, mode(this, "mode")
	{
	}
};

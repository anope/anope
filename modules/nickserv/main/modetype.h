#include "mode.h"

class NSModeType : public Serialize::Type<ModeImpl>
{
 public:
	Serialize::ObjectField<ModeImpl, NickServ::Account *> account;
	Serialize::Field<ModeImpl, Anope::string> mode;

	NSModeType(Module *creator) : Serialize::Type<ModeImpl>(creator)
		, account(this, "account", &ModeImpl::account, true)
		, mode(this, "mode", &ModeImpl::mode)
	{
	}
};

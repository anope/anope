#include "account.h"

class AccountType : public Serialize::Type<AccountImpl>
{
 public:
	/* Name of the account */
	struct Display : Serialize::Field<AccountImpl, Anope::string>
	{
		using Serialize::Field<AccountImpl, Anope::string>::Field;

		void SetField(AccountImpl *s, const Anope::string &) override;
	} display;
	/* User password in form of hashm:data */
	Serialize::Field<AccountImpl, Anope::string> pass;
	Serialize::Field<AccountImpl, Anope::string> email;
	/* Locale name of the language of the user. Empty means default language */
	Serialize::Field<AccountImpl, Anope::string> language;


 	AccountType(Module *);

	NickServ::Account *FindAccount(const Anope::string &nick);
};

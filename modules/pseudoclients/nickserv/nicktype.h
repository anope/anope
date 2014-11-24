#include "nick.h"

class NickType : public Serialize::Type<NickImpl>
{
 public:
	struct Nick : Serialize::Field<NickImpl, Anope::string>
	{
		using Serialize::Field<NickImpl, Anope::string>::Field;

		void SetField(NickImpl *s, const Anope::string &value) override;
	} nick;
	Serialize::Field<NickImpl, Anope::string> last_quit;
	Serialize::Field<NickImpl, Anope::string> last_realname;
	/* Last usermask this nick was seen on, eg user@host */
	Serialize::Field<NickImpl, Anope::string> last_usermask;
	/* Last uncloaked usermask, requires nickserv/auspex to see */
	Serialize::Field<NickImpl, Anope::string> last_realhost;
	Serialize::Field<NickImpl, time_t> time_registered;
	Serialize::Field<NickImpl, time_t> last_seen;

	Serialize::Field<NickImpl, Anope::string> vhost_ident;
	Serialize::Field<NickImpl, Anope::string> vhost_host;
	Serialize::Field<NickImpl, Anope::string> vhost_creator;
	Serialize::Field<NickImpl, time_t> vhost_created;

	/* Account this nick is tied to. Multiple nicks can be tied to a single account. */
	Serialize::ObjectField<NickImpl, NickServ::Account *> nc;

 	NickType(Module *);

	NickServ::Nick *FindNick(const Anope::string &nick);
};

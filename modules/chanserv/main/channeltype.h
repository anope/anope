#include "channel.h"

class ChannelType : public Serialize::Type<ChannelImpl>
{
 public:
	/* Channel name */
	struct Name : Serialize::Field<ChannelImpl, Anope::string>
	{
		using Serialize::Field<ChannelImpl, Anope::string>::Field;

		void SetField(ChannelImpl *c, const Anope::string &value) override;
	} name;
	Serialize::Field<ChannelImpl, Anope::string> desc;
	Serialize::Field<ChannelImpl, time_t> time_registered;
	Serialize::Field<ChannelImpl, time_t> last_used;

	Serialize::Field<ChannelImpl, Anope::string> last_topic;
	Serialize::Field<ChannelImpl, Anope::string> last_topic_setter;
	Serialize::Field<ChannelImpl, time_t> last_topic_time;

	Serialize::Field<ChannelImpl, int16_t> bantype;
	Serialize::Field<ChannelImpl, time_t> banexpire;

	/* Channel founder */
	Serialize::ObjectField<ChannelImpl, NickServ::Account *> founder;
	/* Who gets the channel if the founder nick is dropped or expires */
	Serialize::ObjectField<ChannelImpl, NickServ::Account *> successor;

	Serialize::ObjectField<ChannelImpl, BotInfo *> bi;

 	ChannelType(Module *);

	ChanServ::Channel *FindChannel(const Anope::string &);
};

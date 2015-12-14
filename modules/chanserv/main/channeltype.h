#include "channel.h"

class ChannelType : public Serialize::Type<ChannelImpl>
{
 public:
	/* Channel name */
	struct Name : Serialize::Field<ChanServ::Channel, Anope::string>
	{
		using Serialize::Field<ChanServ::Channel, Anope::string>::Field;

		void SetField(ChanServ::Channel *c, const Anope::string &value) override;
	} name;
	Serialize::Field<ChanServ::Channel, Anope::string> desc;
	Serialize::Field<ChanServ::Channel, time_t> time_registered;
	Serialize::Field<ChanServ::Channel, time_t> last_used;

	Serialize::Field<ChanServ::Channel, Anope::string> last_topic;
	Serialize::Field<ChanServ::Channel, Anope::string> last_topic_setter;
	Serialize::Field<ChanServ::Channel, time_t> last_topic_time;

	Serialize::Field<ChanServ::Channel, int16_t> bantype;
	Serialize::Field<ChanServ::Channel, time_t> banexpire;

	/* Channel founder */
	Serialize::ObjectField<ChanServ::Channel, NickServ::Account *> founder;
	/* Who gets the channel if the founder nick is dropped or expires */
	Serialize::ObjectField<ChanServ::Channel, NickServ::Account *> successor;

	Serialize::ObjectField<ChanServ::Channel, BotInfo *> bi;

 	ChannelType(Module *);

	ChanServ::Channel *FindChannel(const Anope::string &);
};

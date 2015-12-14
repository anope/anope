
class ChanAccessType : public Serialize::AbstractType
{
 public:
	Serialize::ObjectField<ChanServ::ChanAccess, ChanServ::Channel *> ci;
	Serialize::Field<ChanServ::ChanAccess, Anope::string> mask;
	Serialize::ObjectField<ChanServ::ChanAccess, Serialize::Object *> obj;
	Serialize::Field<ChanServ::ChanAccess, Anope::string> creator;
	Serialize::Field<ChanServ::ChanAccess, time_t> last_seen;
	Serialize::Field<ChanServ::ChanAccess, time_t> created;

	ChanAccessType(Module *me, const Anope::string &name) : Serialize::AbstractType(me, name)
		, ci(this, "ci", true)
		, mask(this, "mask")
		, obj(this, "obj", true)
		, creator(this, "creator")
		, last_seen(this, "last_seen")
		, created(this, "created")
	{
	}
};


template<typename T>
class ChanAccessType : public Serialize::Type<T>
{
	static_assert(std::is_base_of<ChanServ::ChanAccess, T>::value, "");

 public:
	Serialize::ObjectField<ChanServ::ChanAccess, ChanServ::Channel *> ci;
	Serialize::Field<ChanServ::ChanAccess, Anope::string> mask;
	Serialize::ObjectField<ChanServ::ChanAccess, Serialize::Object *> obj;
	Serialize::Field<ChanServ::ChanAccess, Anope::string> creator;
	Serialize::Field<ChanServ::ChanAccess, time_t> last_seen;
	Serialize::Field<ChanServ::ChanAccess, time_t> created;

	ChanAccessType(Module *me) : Serialize::Type<T>(me)
		, ci(this, "ci", &ChanServ::ChanAccess::channel, true)
		, mask(this, "mask", &ChanServ::ChanAccess::mask)
		, obj(this, "obj", &ChanServ::ChanAccess::object, true)
		, creator(this, "creator", &ChanServ::ChanAccess::creator)
		, last_seen(this, "last_seen", &ChanServ::ChanAccess::last_seen)
		, created(this, "created", &ChanServ::ChanAccess::created)
	{
	}
};


#pragma once

class Stats : public Serialize::Object
{
 public:
	static constexpr const char *const NAME = "stats";

	using Serialize::Object::Object;

	virtual unsigned int GetMaxUserCount() anope_abstract;
	virtual void SetMaxUserCount(unsigned int i) anope_abstract;

	virtual time_t GetMaxUserTime() anope_abstract;
	virtual void SetMaxUserTime(time_t t) anope_abstract;
};


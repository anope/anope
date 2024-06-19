/*
 *
 * (C) 2011-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#pragma once

struct MyOper final
	: Oper
	, Serializable
{
	MyOper(const Anope::string &n, OperType *o) : Oper(n, o), Serializable("Oper") { }

	void Serialize(Serialize::Data &data) const override
	{
		data["name"] << this->name;
		data["type"] << this->ot->GetName();
	}

	static Serializable *Unserialize(Serializable *obj, Serialize::Data &data)
	{
		Anope::string stype, sname;

		data["type"] >> stype;
		data["name"] >> sname;

		OperType *ot = OperType::Find(stype);
		if (ot == NULL)
			return NULL;
		NickCore *nc = NickCore::Find(sname);
		if (nc == NULL)
			return NULL;

		MyOper *myo;
		if (obj)
			myo = anope_dynamic_static_cast<MyOper *>(obj);
		else
			myo = new MyOper(nc->display, ot);
		nc->o = myo;
		Log(LOG_NORMAL, "operserv/oper") << "Tied oper " << nc->display << " to type " << ot->GetName();
		return myo;
	}
};

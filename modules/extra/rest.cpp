/*
 * Anope IRC Services
 *
 * Copyright (C) 2016 Anope Team <team@anope.org>
 *
 * This file is part of Anope. Anope is free software; you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see see <http://www.gnu.org/licenses/>.
 */

#include "module.h"
#include "modules/httpd.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

class RESTService : public HTTPPage
{
 public:
	RESTService(Module *creator, const Anope::string &sname) : HTTPPage("/rest", "application/json")
	{
	}

	bool OnRequest(HTTPProvider *provider, const Anope::string &page_name, HTTPClient *client, HTTPMessage &message, HTTPReply &reply) override
	{
		sepstream sep(page_name, '/');
		Anope::string token;

		sep.GetToken(token); // /rest
		sep.GetToken(token); // type

		if (token == "types")
		{
			GetTypes(provider, client, message, reply);
			return true;
		}

		Serialize::TypeBase *type = Serialize::TypeBase::Find(token);
		if (type == nullptr)
		{
			reply.error = HTTP_PAGE_NOT_FOUND;
			reply.Write("Unknown type");
			return true;
		}

		if (message.method == httpd::Method::POST)
		{
			Post(provider, client, message, reply, type, sep);
		}
		else if (message.method == httpd::Method::GET)
		{
			Get(provider, client, message, reply, type, sep);
		}
		else if (message.method == httpd::Method::PUT)
		{
			Put(provider, client, message, reply, type, sep);
		}
		else if (message.method == httpd::Method::DELETE)
		{
			Delete(provider, client, message, reply, type, sep);
		}
		else
		{
			reply.error = HTTP_NOT_SUPPORTED;
			reply.Write("Not Supported");
		}

		return true;
	}

 private:
	/** Get all object type names
	 */
	void GetTypes(HTTPProvider *provider, HTTPClient *client, HTTPMessage &message, HTTPReply &reply)
	{
		boost::property_tree::ptree tree, children;

		for (Serialize::TypeBase *stype : Serialize::TypeBase::GetTypes())
		{
			boost::property_tree::ptree child;
			child.put("", stype->GetName().str());
			children.push_back(std::make_pair("", child));
		}

		tree.add_child("types", children);

		std::stringstream ss;
		boost::property_tree::write_json(ss, tree);

		reply.Write(ss.str());
	}

	void Post(HTTPProvider *provider, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, Serialize::TypeBase *type, sepstream &sep)
	{
		boost::property_tree::ptree tree;
		std::stringstream ss(message.content.str());

		try
		{
			boost::property_tree::read_json(ss, tree);
		}
		catch (const boost::property_tree::json_parser::json_parser_error &)
		{
			reply.error = HTTP_BAD_REQUEST;
			reply.Write("Unable to parse JSON");
			return;
		}

		for (auto &p : tree)
		{
			const std::string &key = p.first;

			Serialize::FieldBase *field = type->GetField(key);
			if (field == nullptr)
			{
				reply.error = HTTP_BAD_REQUEST;
				reply.Write("No such field " + field->serialize_name);
				return;
			}
		}

		Serialize::Object *object = type->Create();
		if (object == nullptr)
		{
			reply.error = HTTP_INTERNAL_SERVER_ERROR;
			reply.Write("Unable to create new object");
			return;
		}

		for (auto &p : tree)
		{
			const std::string &key = p.first;
			const boost::property_tree::ptree &pt = p.second;

			Serialize::FieldBase *field = type->GetField(key);
			if (field == nullptr)
			{
				reply.error = HTTP_INTERNAL_SERVER_ERROR;
				reply.Write("Missing field");
				return;
			}

			field->UnserializeFromString(object, pt.get_value<std::string>());
		}

		reply.error = HTTP_CREATED;
		reply.Write(stringify(object->id));
	}

	void Get(HTTPProvider *provider, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, Serialize::TypeBase *type, sepstream &sep)
	{
		Anope::string token;
		sep.GetToken(token);

		if (token.empty())
		{
			GetList(reply, type);
			return;
		}

		if (token.is_pos_number_only())
		{
			GetID(message, reply, type, token);
			return;
		}

		GetField(reply, type, token, sep);
	}

	boost::property_tree::ptree ToJson(Serialize::Object *object)
	{
		boost::property_tree::ptree tree;

		for (Serialize::FieldBase *field : object->GetSerializableType()->GetFields())
		{
			if (field->HasFieldS(object)) // for ext
				tree.put(field->serialize_name.str(), field->SerializeToString(object).str());
		}

		tree.put("id", object->id);
		return tree;
	}

	/** Get a list of all objects of a type
	 */
	void GetList(HTTPReply &reply, Serialize::TypeBase *type)
	{
		boost::property_tree::ptree tree, children;

		for (Serialize::Object *object : Serialize::GetObjects_<Serialize::Object *>(type))
		{
			boost::property_tree::ptree child = ToJson(object);;
			children.push_back(std::make_pair("", child));
		}

		tree.add_child(type->GetName().str(), children);

		std::stringstream ss;
		boost::property_tree::write_json(ss, tree);

		reply.Write(ss.str());
	}

	/** Get object by id
	 */
	void GetID(HTTPMessage &message, HTTPReply &reply, Serialize::TypeBase *type, const Anope::string &token)
	{
		Serialize::ID id;
		try
		{
			id = convertTo<Serialize::ID>(token);
		}
		catch (const ConvertException &ex)
		{
			reply.error = HTTP_BAD_REQUEST;
			reply.Write("Invalid id");
			return;
		}

		if (message.get_data.count("refs") > 0)
		{
			Serialize::TypeBase *ref_type = Serialize::TypeBase::Find(message.get_data["refs"]);

			if (ref_type == nullptr)
			{
				reply.error = HTTP_PAGE_NOT_FOUND;
				reply.Write("Unknown reference type");
				return;
			}

			GetRefs(reply, type, id, ref_type);
			return;
		}

		GetID(reply, type, id);
	}

	void GetID(HTTPReply &reply, Serialize::TypeBase *type, Serialize::ID id)
	{
		Serialize::Object *object = type->Require(id);
		if (object == nullptr)
		{
			/* This has to be a cached object with type/id mismatch */
			reply.error = HTTP_PAGE_NOT_FOUND;
			reply.Write("Type/id mismatch");
			return;
		}

		boost::property_tree::ptree tree = ToJson(object);

		std::stringstream ss;
		boost::property_tree::write_json(ss, tree);

		reply.Write(ss.str());
	}

	/** Get object by field and value
	 */
	void GetField(HTTPReply &reply, Serialize::TypeBase *type, const Anope::string &token, sepstream &sep)
	{
		Serialize::FieldBase *field = type->GetField(token);
		if (field == nullptr)
		{
			reply.error = HTTP_PAGE_NOT_FOUND;
			reply.Write("No such field");
			return;
		}

		Anope::string value;
		sep.GetToken(value);

		Serialize::ID id;
		EventReturn result = EventManager::Get()->Dispatch(&Event::SerializeEvents::OnSerializeFind, type, field, value, id);
		if (result != EVENT_ALLOW)
		{
			reply.error = HTTP_PAGE_NOT_FOUND;
			reply.Write("Not found");
			return;
		}

		GetID(reply, type, id);
	}

	/** Find refs of type ref_type to object id of type type
	 */
	void GetRefs(HTTPReply &reply, Serialize::TypeBase *type, Serialize::ID id, Serialize::TypeBase *ref_type)
	{
		Serialize::Object *object = type->Require(id);
		if (object == nullptr)
		{
			reply.error = HTTP_PAGE_NOT_FOUND;
			reply.Write("Type/id mismatch");
			return;
		}

		boost::property_tree::ptree tree, children;

		for (Serialize::Object *ref : object->GetRefs(ref_type))
		{
			boost::property_tree::ptree child = ToJson(ref);
			children.push_back(std::make_pair("", child));
		}

		tree.add_child(type->GetName().str(), children);

		std::stringstream ss;
		boost::property_tree::write_json(ss, tree);

		reply.Write(ss.str());
	}

	void Put(HTTPProvider *provider, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, Serialize::TypeBase *type, sepstream &sep)
	{
		Anope::string token;
		Anope::string fieldname;

		if (!sep.GetToken(token) || !sep.GetToken(fieldname))
		{
			reply.error = HTTP_BAD_REQUEST;
			reply.Write("No id and/or field");
			return;
		}

		Serialize::ID id;
		try
		{
			id = convertTo<Serialize::ID>(token);
		}
		catch (const ConvertException &ex)
		{
			reply.error = HTTP_BAD_REQUEST;
			reply.Write("Invalid id");
			return;
		}

		Serialize::Object *object = type->Require(id);
		if (object == nullptr)
		{
			reply.error = HTTP_PAGE_NOT_FOUND;
			reply.Write("Type/id mismatch");
			return;
		}

		Serialize::FieldBase *field = type->GetField(fieldname);
		if (field == nullptr)
		{
			reply.error = HTTP_BAD_REQUEST;
			reply.Write("No such field");
			return;
		}

		field->UnserializeFromString(object, message.content);
	}

	void Delete(HTTPProvider *provider, HTTPClient *client, HTTPMessage &message, HTTPReply &reply, Serialize::TypeBase *type, sepstream &sep)
	{
		Anope::string token;
		sep.GetToken(token);

		Serialize::ID id;
		try
		{
			id = convertTo<Serialize::ID>(token);
		}
		catch (const ConvertException &ex)
		{
			reply.error = HTTP_BAD_REQUEST;
			reply.Write("Invalid id");
			return;
		}

		Serialize::Object *object = type->Require(id);
		if (object == nullptr)
		{
			reply.error = HTTP_PAGE_NOT_FOUND;
			reply.Write("Type/id mismatch");
			return;
		}

		object->Delete();
	}
};

class ModuleREST : public Module
{
	ServiceReference<HTTPProvider> httpref;

 public:
	RESTService restinterface;

	ModuleREST(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR)
		, restinterface(this, "rest")
	{

	}

	~ModuleREST()
	{
		if (httpref)
			httpref->UnregisterPage(&restinterface);
	}

	void OnReload(Configuration::Conf *conf) override
	{
		if (httpref)
			httpref->UnregisterPage(&restinterface);

		this->httpref = ServiceReference<HTTPProvider>(conf->GetModule(this)->Get<Anope::string>("server", "httpd/main"));

		if (!httpref)
			throw ConfigException("Unable to find http reference, is m_httpd loaded?");

		httpref->RegisterPage(&restinterface);
	}
};

MODULE_INIT(ModuleREST)

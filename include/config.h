/*
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#pragma once

#include "users.h"
#include "opertype.h"

namespace Configuration
{
	class CoreExport Block
	{
		friend struct Conf;

	 public:
		typedef Anope::map<Anope::string> item_map;
		typedef Anope::multimap<Block> block_map;

	 private:
		Anope::string name;
		item_map items;
		block_map blocks;
		int linenum;

	 public:
	 	Block(const Anope::string &);
		const Anope::string &GetName() const;
		int CountBlock(const Anope::string &name);
		Block* GetBlock(const Anope::string &name, int num = 0);

		template<typename T> inline T Get(const Anope::string &tag)
		{
			return this->Get<T>(tag, "");
		}
		/* VS 2008 has an issue with having a default argument here (def = ""), which is why the above
		 * function exists.
		 */
		template<typename T> T Get(const Anope::string &tag, const Anope::string &def) const
		{
			const Anope::string &value = this->Get<Anope::string>(tag, def);
			if (!value.empty())
				try
				{
					return convertTo<T>(value);
				}
				catch (const ConvertException &) { }
			return T();
		}

		bool Set(const Anope::string &tag, const Anope::string &value);
		const item_map* GetItems() const;
	};

	template<> CoreExport Anope::string Block::Get(const Anope::string &tag, const Anope::string& def) const;
	template<> CoreExport time_t Block::Get(const Anope::string &tag, const Anope::string &def) const;
	template<> CoreExport bool Block::Get(const Anope::string &tag, const Anope::string &def) const;
	template<> CoreExport unsigned int Block::Get(const Anope::string &tag, const Anope::string &def) const;

	/** Represents a configuration file
	 */
	class File
	{
		Anope::string name;
		bool executable;
		FILE *fp;
	 public:
		File(const Anope::string &, bool);
		~File();
		const Anope::string &GetName() const;

		bool IsOpen() const;
		bool Open();
		void Close();
		bool End() const;
		Anope::string Read();
	};

	struct Uplink;
	struct Usermode;
	struct Channelmode;

	struct CoreExport Conf : Block
	{
		/* options:readtimeout */
		time_t ReadTimeout;
		/* options:useprivmsg */
		bool UsePrivmsg;
		/* If we should default to privmsging clients */
		bool DefPrivmsg;
		/* Default language */
		Anope::string DefLanguage;
		/* options:timeoutcheck */
		time_t TimeoutCheck;
		/* options:usestrictprivmsg */
		bool UseStrictPrivmsg;
		/* flag for options:regexengine */
		std::regex::flag_type regex_flags;
		/* networkinfo:nickchars */
		Anope::string NickChars;

		/* either "/msg " or "/" */
		Anope::string StrictPrivmsg;
		/* List of uplink servers to try and connect to */
		std::vector<Uplink> Uplinks;
		/* A vector of our logfile options */
		std::vector<LogInfo> LogInfos;
		/* Array of ulined servers */
		std::vector<Anope::string> Ulines;
		/* List of available opertypes */
		std::vector<OperType *> MyOperTypes;
		/* names of opers configured in the configuration */
		std::vector<Anope::string> Opers;
		/* Map of fantasy commands */
		CommandInfo::map Fantasy;
		/* Command groups */
		std::vector<CommandGroup> CommandGroups;
		/* List of modules to autoload */
		std::vector<Anope::string> ModulesAutoLoad;
		/* After how many characters do we wrap lines? */
		unsigned int LineWrap;
		std::vector<Usermode> Usermodes;
		std::vector<Channelmode> Channelmodes;
		unsigned char CaseMapUpper[256] = { 0 }, CaseMapLower[256] = { 0 };

		/* module configuration blocks */
		std::map<Anope::string, Block *> modules;
		Anope::map<Anope::string> bots;

		Conf();
		~Conf();

		void LoadConf(File &file);
		void Post(Conf *old);

		Block *GetModule(Module *);
		Block *GetModule(const Anope::string &name);

		ServiceBot *GetClient(const Anope::string &name);

		Block *GetCommand(CommandSource &);
	};

	struct Uplink
	{
		Anope::string host;
		unsigned port;
		Anope::string password;
		bool ipv6;

		Uplink(const Anope::string &_host, int _port, const Anope::string &_password, bool _ipv6) : host(_host), port(_port), password(_password), ipv6(_ipv6) { }
		inline bool operator==(const Uplink &other) const { return host == other.host && port == other.port && password == other.password && ipv6 == other.ipv6; }
		inline bool operator!=(const Uplink &other) const { return !(*this == other); }
	};

	struct Usermode
	{
		Anope::string name;
		char character;
		bool param;
		bool oper_only, setable;
	};

	struct Channelmode
	{
		Anope::string name, param_regex;
		char character;
		char status; /* status char, if any +/@ */
		int level; /* relative level */
		bool oper_only, list, param, param_unset, setable;

	};
}

class ConfigException : public CoreException
{
 public:
	/** Default constructor, just uses the error mesage 'Config threw an exception'.
	 */
	ConfigException() : CoreException("Config threw an exception", "Config Parser") { }
	/** This constructor can be used to specify an error message before throwing.
	 */
	ConfigException(const Anope::string &message) : CoreException(message, "Config Parser") { }

	virtual ~ConfigException() throw() = default;
};

extern Configuration::File ServicesConf;
extern CoreExport Configuration::Conf *Config;


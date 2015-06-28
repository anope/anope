/* Configuration file handling.
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

#include "services.h"
#include "config.h"
#include "opertype.h"
#include "channels.h"
#include "hashcomp.h"
#include "event.h"
#include "bots.h"
#include "modules.h"
#include "servers.h"
#include "protocol.h"
#include "modules/nickserv.h"

using namespace Configuration;

File ServicesConf("anope.conf", false); // Services configuration file name
Conf *Config = NULL;

Block::Block(const Anope::string &n) : name(n), linenum(-1)
{
}

const Anope::string &Block::GetName() const
{
	return name;
}

int Block::CountBlock(const Anope::string &bname)
{
	if (!this)
		return 0;

	return blocks.count(bname);
}

Block* Block::GetBlock(const Anope::string &bname, int num)
{
	if (!this)
		return NULL;

	std::pair<block_map::iterator, block_map::iterator> it = blocks.equal_range(bname);

	for (int i = 0; it.first != it.second; ++it.first, ++i)
		if (i == num)
			return &it.first->second;
	return NULL;
}

bool Block::Set(const Anope::string &tag, const Anope::string &value)
{
	if (!this)
		return false;

	items[tag] = value;
	return true;
}

const Block::item_map* Block::GetItems() const
{
	if (this)
		return &items;
	else
		return NULL;
}

template<> Anope::string Block::Get(const Anope::string &tag, const Anope::string& def) const
{
	if (!this)
		return def;

	Anope::map<Anope::string>::const_iterator it = items.find(tag);
	if (it != items.end())
		return it->second;

	return def;
}

template<> time_t Block::Get(const Anope::string &tag, const Anope::string &def) const
{
	return Anope::DoTime(Get<Anope::string>(tag, def));
}

template<> bool Block::Get(const Anope::string &tag, const Anope::string &def) const
{
	const Anope::string &str = Get<Anope::string>(tag, def);
	return !str.empty() && !str.equals_ci("no") && !str.equals_ci("off") && !str.equals_ci("false") && !str.equals_ci("0");
}

template<> unsigned int Block::Get(const Anope::string &tag, const Anope::string &def) const
{
	const Anope::string &str = Get<Anope::string>(tag, def);
	std::size_t pos = str.length();
	unsigned long l;

	try
	{
		l = std::stoul(str.str(), &pos, 0);
	}
	catch (...)
	{
		return 0;
	}

	if (pos != str.length())
		return 0;

	return l;
}

static void ValidateNotEmpty(const Anope::string &block, const Anope::string &name, const Anope::string &value)
{
	if (value.empty())
		throw ConfigException("The value for <" + block + ":" + name + "> cannot be empty!");
}

static void ValidateNoSpaces(const Anope::string &block, const Anope::string &name, const Anope::string &value)
{
	if (value.find(' ') != Anope::string::npos)
		throw ConfigException("The value for <" + block + ":" + name + "> may not contain spaces!");
}

template<typename T> static void ValidateNotZero(const Anope::string &block, const Anope::string &name, T value)
{
	if (!value)
		throw ConfigException("The value for <" + block + ":" + name + "> cannot be zero!");
}

Conf::Conf() : Block("")
{
	ReadTimeout = 0;
	UsePrivmsg = DefPrivmsg = false;

	this->LoadConf(ServicesConf);

	for (int i = 0; i < this->CountBlock("include"); ++i)
	{
		Block *include = this->GetBlock("include", i);

		const Anope::string &type = include->Get<Anope::string>("type"),
					&file = include->Get<Anope::string>("name");

		ValidateNotEmpty("include", "name", file);

		File f(file, type == "executable");
		this->LoadConf(f);
	}

	for (Module *m : ModuleManager::Modules)
		m->OnReload(this);

	/* Check for modified values that aren't allowed to be modified */
	if (Config)
	{
		struct
		{
			Anope::string block;
			Anope::string name;
		} noreload[] = {
			{"serverinfo", "name"},
			{"serverinfo", "description"},
			{"serverinfo", "localhost"},
			{"serverinfo", "id"},
			{"serverinfo", "pid"},
			{"networkinfo", "nicklen"},
			{"networkinfo", "userlen"},
			{"networkinfo", "hostlen"},
			{"networkinfo", "chanlen"},
		};

		for (unsigned i = 0; i < sizeof(noreload) / sizeof(noreload[0]); ++i)
			if (this->GetBlock(noreload[i].block)->Get<Anope::string>(noreload[i].name) != Config->GetBlock(noreload[i].block)->Get<Anope::string>(noreload[i].name))
				throw ConfigException("<" + noreload[i].block + ":" + noreload[i].name + "> can not be modified once set");
	}

	Block *serverinfo = this->GetBlock("serverinfo"), *options = this->GetBlock("options"),
		*mail = this->GetBlock("mail"), *networkinfo = this->GetBlock("networkinfo");

	ValidateNotEmpty("serverinfo", "name", serverinfo->Get<Anope::string>("name"));
	ValidateNotEmpty("serverinfo", "description", serverinfo->Get<Anope::string>("description"));
	ValidateNotEmpty("serverinfo", "pid", serverinfo->Get<Anope::string>("pid"));
	ValidateNotEmpty("serverinfo", "motd", serverinfo->Get<Anope::string>("motd"));

	ValidateNotZero("options", "readtimeout", options->Get<time_t>("readtimeout"));
	ValidateNotZero("options", "warningtimeout", options->Get<time_t>("warningtimeout"));

	ValidateNotZero("networkinfo", "nicklen", networkinfo->Get<unsigned>("nicklen"));
	ValidateNotZero("networkinfo", "userlen", networkinfo->Get<unsigned>("userlen"));
	ValidateNotZero("networkinfo", "hostlen", networkinfo->Get<unsigned>("hostlen"));
	ValidateNotZero("networkinfo", "chanlen", networkinfo->Get<unsigned>("chanlen"));

	spacesepstream(options->Get<Anope::string>("ulineservers")).GetTokens(this->Ulines);

	if (mail->Get<bool>("usemail"))
	{
		Anope::string check[] = { "sendmailpath", "sendfrom", "registration_subject", "registration_message", "emailchange_subject", "emailchange_message", "memo_subject", "memo_message" };
		for (unsigned i = 0; i < sizeof(check) / sizeof(Anope::string); ++i)
			ValidateNotEmpty("mail", check[i], mail->Get<Anope::string>(check[i]));
	}

	this->ReadTimeout = options->Get<time_t>("readtimeout");
	this->UsePrivmsg = options->Get<bool>("useprivmsg");
	this->UseStrictPrivmsg = options->Get<bool>("usestrictprivmsg");
	this->StrictPrivmsg = !UseStrictPrivmsg ? "/msg " : "/";
	{
		std::vector<Anope::string> defaults;
		spacesepstream(this->GetModule("nickserv")->Get<Anope::string>("defaults")).GetTokens(defaults);
		this->DefPrivmsg = std::find(defaults.begin(), defaults.end(), "msg") != defaults.end();
	}
	this->DefLanguage = options->Get<Anope::string>("defaultlanguage");
	this->TimeoutCheck = options->Get<time_t>("timeoutcheck");
	this->NickChars = networkinfo->Get<Anope::string>("nick_chars");

	for (int i = 0; i < this->CountBlock("uplink"); ++i)
	{
		Block *uplink = this->GetBlock("uplink", i);

		const Anope::string &host = uplink->Get<Anope::string>("host");
		bool ipv6 = uplink->Get<bool>("ipv6");
		int port = uplink->Get<int>("port");
		const Anope::string &password = uplink->Get<Anope::string>("password");

		ValidateNotEmpty("uplink", "host", host);
		ValidateNotZero("uplink", "port", port);
		ValidateNotEmpty("uplink", "password", password);

		this->Uplinks.push_back(Uplink(host, port, password, ipv6));
	}

	for (int i = 0; i < this->CountBlock("module"); ++i)
	{
		Block *module = this->GetBlock("module", i);

		const Anope::string &modname = module->Get<Anope::string>("name");

		ValidateNotEmpty("module", "name", modname);

		this->ModulesAutoLoad.push_back(modname);
	}

	for (int i = 0; i < this->CountBlock("opertype"); ++i)
	{
		Block *opertype = this->GetBlock("opertype", i);

		const Anope::string &oname = opertype->Get<Anope::string>("name"),
				&modes = opertype->Get<Anope::string>("modes"),
				&inherits = opertype->Get<Anope::string>("inherits"),
				&commands = opertype->Get<Anope::string>("commands"),
				&privs = opertype->Get<Anope::string>("privs");

		ValidateNotEmpty("opertype", "name", oname);

		OperType *ot = new OperType(oname);
		ot->modes = modes;

		spacesepstream cmdstr(commands);
		for (Anope::string str; cmdstr.GetToken(str);)
			ot->AddCommand(str);

		spacesepstream privstr(privs);
		for (Anope::string str; privstr.GetToken(str);)
			ot->AddPriv(str);

		commasepstream inheritstr(inherits);
		for (Anope::string str; inheritstr.GetToken(str);)
		{
			/* Strip leading ' ' after , */
			if (str.length() > 1 && str[0] == ' ')
				str.erase(str.begin());
			for (unsigned j = 0; j < this->MyOperTypes.size(); ++j)
			{
				OperType *ot2 = this->MyOperTypes[j];

				if (ot2->GetName().equals_ci(str))
				{
					Log() << "Inheriting commands and privs from " << ot2->GetName() << " to " << ot->GetName();
					ot->Inherits(ot2);
					break;
				}
			}
		}

		this->MyOperTypes.push_back(ot);
	}

	for (int i = 0; i < this->CountBlock("oper"); ++i)
	{
		Block *oper = this->GetBlock("oper", i);

		const Anope::string &nname = oper->Get<Anope::string>("name"),
					&type = oper->Get<Anope::string>("type"),
					&password = oper->Get<Anope::string>("password"),
					&certfp = oper->Get<Anope::string>("certfp"),
					&host = oper->Get<Anope::string>("host"),
					&vhost = oper->Get<Anope::string>("vhost");
		bool require_oper = oper->Get<bool>("require_oper");

		ValidateNotEmpty("oper", "name", nname);
		ValidateNotEmpty("oper", "type", type);

		OperType *ot = NULL;
		for (unsigned j = 0; j < this->MyOperTypes.size(); ++j)
			if (this->MyOperTypes[j]->GetName() == type)
				ot = this->MyOperTypes[j];
		if (ot == NULL)
			throw ConfigException("Oper block for " + nname + " has invalid oper type " + type);

		Oper *o = operblock.Create();
		o->conf = this;
		o->SetName(nname);
		o->SetType(ot);
		o->SetRequireOper(require_oper);
		o->SetPassword(password);
		o->SetCertFP(certfp);
		o->SetHost(host);
		o->SetVhost(vhost);
	}

	for (BotInfo *bi : Serialize::GetObjects<BotInfo *>(botinfo))
		bi->conf = nullptr;
	for (int i = 0; i < this->CountBlock("service"); ++i)
	{
		Block *service = this->GetBlock("service", i);

		const Anope::string &nick = service->Get<Anope::string>("nick"),
					&user = service->Get<Anope::string>("user"),
					&host = service->Get<Anope::string>("host"),
					&gecos = service->Get<Anope::string>("gecos"),
					&modes = service->Get<Anope::string>("modes"),
					&channels = service->Get<Anope::string>("channels");

		ValidateNotEmpty("service", "nick", nick);
		ValidateNotEmpty("service", "user", user);
		ValidateNotEmpty("service", "host", host);
		ValidateNotEmpty("service", "gecos", gecos);
		ValidateNoSpaces("service", "channels", channels);

		if (User *u = User::Find(nick, true))
		{
			if (u->type != UserType::BOT)
			{
				u->Kill(Me, "Nickname required by services");
			}
		}

		ServiceBot *sb = ServiceBot::Find(nick, true);
		if (!sb)
			sb = new ServiceBot(nick, user, host, gecos, modes);

		sb->bi->conf = service;
	}

	for (int i = 0; i < this->CountBlock("log"); ++i)
	{
		Block *log = this->GetBlock("log", i);

		int logage = log->Get<int>("logage");
		bool rawio = log->Get<bool>("rawio");
		bool debug = log->Get<bool>("debug");

		LogInfo l(logage, rawio, debug);

		l.bot = ServiceBot::Find(log->Get<Anope::string>("bot", "Global"), true);
		spacesepstream(log->Get<Anope::string>("target")).GetTokens(l.targets);
		spacesepstream(log->Get<Anope::string>("source")).GetTokens(l.sources);
		spacesepstream(log->Get<Anope::string>("admin")).GetTokens(l.admin);
		spacesepstream(log->Get<Anope::string>("override")).GetTokens(l.override);
		spacesepstream(log->Get<Anope::string>("commands")).GetTokens(l.commands);
		spacesepstream(log->Get<Anope::string>("servers")).GetTokens(l.servers);
		spacesepstream(log->Get<Anope::string>("channels")).GetTokens(l.channels);
		spacesepstream(log->Get<Anope::string>("users")).GetTokens(l.users);
		spacesepstream(log->Get<Anope::string>("other")).GetTokens(l.normal);

		this->LogInfos.push_back(l);
	}

	for (std::pair<Anope::string, User *> p : UserListByNick)
	{
		User *u = p.second;
		if (u->type != UserType::BOT)
			continue;

		ServiceBot *bi = anope_dynamic_static_cast<ServiceBot *>(u);
		bi->commands.clear();
	}
	for (int i = 0; i < this->CountBlock("command"); ++i)
	{
		Block *command = this->GetBlock("command", i);

		const Anope::string &service = command->Get<Anope::string>("service"),
					&nname = command->Get<Anope::string>("name"),
					&cmd = command->Get<Anope::string>("command"),
					&permission = command->Get<Anope::string>("permission"),
					&group = command->Get<Anope::string>("group");
		bool hide = command->Get<bool>("hide");

		ValidateNotEmpty("command", "service", service);
		ValidateNotEmpty("command", "name", nname);
		ValidateNotEmpty("command", "command", cmd);

		ServiceBot *bi = this->GetClient(service);
		if (!bi)
			continue;

		CommandInfo &ci = bi->SetCommand(nname, cmd, permission);
		ci.group = group;
		ci.hide = hide;
	}

	for (int i = 0; i < this->CountBlock("fantasy"); ++i)
	{
		Block *fantasy = this->GetBlock("fantasy", i);

		const Anope::string &nname = fantasy->Get<Anope::string>("name"),
					&service = fantasy->Get<Anope::string>("command"),
					&permission = fantasy->Get<Anope::string>("permission"),
					&group = fantasy->Get<Anope::string>("group");
		bool hide = fantasy->Get<bool>("hide"),
			prepend_channel = fantasy->Get<bool>("prepend_channel", "yes");

		ValidateNotEmpty("fantasy", "name", nname);
		ValidateNotEmpty("fantasy", "command", service);

		CommandInfo &c = this->Fantasy[nname];
		c.name = service;
		c.cname = nname;
		c.permission = permission;
		c.group = group;
		c.hide = hide;
		c.prepend_channel = prepend_channel;
	}

	for (int i = 0; i < this->CountBlock("command_group"); ++i)
	{
		Block *command_group = this->GetBlock("command_group", i);

		const Anope::string &nname = command_group->Get<Anope::string>("name"),
					&description = command_group->Get<Anope::string>("description");

		CommandGroup gr;
		gr.name = nname;
		gr.description = description;

		this->CommandGroups.push_back(gr);
	}

	for (int i = 0; i < this->CountBlock("usermode"); ++i)
	{
		Block *usermode = this->GetBlock("usermode", i);

		Anope::string nname = usermode->Get<Anope::string>("name"),
				character = usermode->Get<Anope::string>("character");
		bool oper_only = usermode->Get<bool>("oper_only"),
				setable = usermode->Get<bool>("setable"),
				param = usermode->Get<bool>("param");

		ValidateNotEmpty("usermode", "name", nname);

		if (character.length() != 1)
			throw ConfigException("Usermode character length must be 1");

		Usermode um;
		um.name = nname;
		um.character = character[0];
		um.param = param;
		um.oper_only = oper_only;
		um.setable = setable;

		this->Usermodes.push_back(um);
	}

	for (int i = 0; i < this->CountBlock("channelmode"); ++i)
	{
		Block *channelmode = this->GetBlock("channelmode", i);

		Anope::string nname = channelmode->Get<Anope::string>("name"),
				character = channelmode->Get<Anope::string>("character"),
				status = channelmode->Get<Anope::string>("status"),
				param_regex = channelmode->Get<Anope::string>("param_regex");
		bool oper_only = channelmode->Get<bool>("oper_only"),
				setable = channelmode->Get<bool>("setable"),
				list = channelmode->Get<bool>("list"),
				param = channelmode->Get<bool>("param"),
				param_unset = channelmode->Get<bool>("param_unset", "yes");
		int level = channelmode->Get<int>("level");

		ValidateNotEmpty("usermode", "name", nname);

		if (character.length() != 1)
			throw ConfigException("Channelmode character length must be 1");

		if (status.length() > 1)
			throw ConfigException("Channelmode status must be at most one character");

		if (list || !param_regex.empty() || param_unset || !status.empty())
			param = true;

		Channelmode cm;
		cm.name = nname;
		cm.character = character[0];
		cm.status = !status.empty() ? status[0] : 0;
		cm.param_regex = param_regex;
		cm.oper_only = oper_only;
		cm.setable = setable;
		cm.list = list;
		cm.param = param;
		cm.param_unset = param_unset;
		cm.level = level;

		this->Channelmodes.push_back(cm);
	}

	for (int i = 0; i < this->CountBlock("casemap"); ++i)
	{
		Block *casemap = this->GetBlock("casemap", i);

		unsigned char upper = casemap->Get<unsigned int>("upper"),
				lower = casemap->Get<unsigned int>("lower");

		if (!upper)
		{
			Anope::string s = casemap->Get<Anope::string>("upper");
			if (s.length() == 1)
				upper = s[0];
		}

		if (!lower)
		{
			Anope::string s = casemap->Get<Anope::string>("lower");
			if (s.length() == 1)
				lower = s[0];
		}

		if (upper && lower)
		{
			CaseMapUpper[lower] = CaseMapUpper[upper] = upper;
			CaseMapLower[lower] = CaseMapLower[upper] = lower;
		}
	}

	/* Below here can't throw */

	/* Clear existing conf opers */
	if (Config)
		for (Oper *o : Serialize::GetObjects<Oper *>(operblock))
			if (o->conf == Config)
				o->Delete();
	/* Apply new opers */
	if (NickServ::service)
		for (Oper *o : Serialize::GetObjects<Oper *>(operblock))
		{
			NickServ::Nick *na = NickServ::service->FindNick(o->GetName());
			if (!na)
				continue;

			na->GetAccount()->o = o;
			Log() << "Tied oper " << na->GetAccount()->GetDisplay() << " to type " << o->GetType()->GetName();
		}

	/* Check the user keys */
	if (!options->Get<unsigned>("seed"))
		Log() << "Configuration option options:seed should be set. It's for YOUR safety! Remember that!";

	/* check regexengine */
	const Anope::string &regex_engine = options->Get<Anope::string>("regexengine");
	if (regex_engine == "ecmascript")
		regex_flags = std::regex::ECMAScript;
	else if (regex_engine == "basic")
		regex_flags = std::regex::basic;
	else if (regex_engine == "extended")
		regex_flags = std::regex::extended;
	else if (regex_engine == "awk")
		regex_flags = std::regex::awk;
	else if (regex_engine == "grep")
		regex_flags = std::regex::grep;
	else if (regex_engine == "egrep")
		regex_flags = std::regex::egrep;
	else
		regex_flags = static_cast<std::basic_regex<char>::flag_type>(0);
	/* always enable icase and optimize */
	if (regex_flags)
		regex_flags |= std::regex::icase | std::regex::optimize;

	this->LineWrap = options->Get<unsigned>("linewrap", "200");
}

Conf::~Conf()
{
	for (unsigned i = 0; i < MyOperTypes.size(); ++i)
		delete MyOperTypes[i];
}

void Conf::Post(Conf *old)
{
	/* Apply module changes */
	for (unsigned i = 0; i < old->ModulesAutoLoad.size(); ++i)
		if (std::find(this->ModulesAutoLoad.begin(), this->ModulesAutoLoad.end(), old->ModulesAutoLoad[i]) == this->ModulesAutoLoad.end())
			ModuleManager::UnloadModule(ModuleManager::FindModule(old->ModulesAutoLoad[i]), NULL);
	for (unsigned i = 0; i < this->ModulesAutoLoad.size(); ++i)
		if (std::find(old->ModulesAutoLoad.begin(), old->ModulesAutoLoad.end(), this->ModulesAutoLoad[i]) == old->ModulesAutoLoad.end())
			ModuleManager::LoadModule(this->ModulesAutoLoad[i], NULL);

	ModeManager::Apply(old);

	/* Apply opertype changes, as non-conf opers still point to the old oper types */
	for (Oper *o : Serialize::GetObjects<Oper *>(operblock))
	{
		/* Oper's type is in the old config, so update it */
		if (std::find(old->MyOperTypes.begin(), old->MyOperTypes.end(), o->GetType()) != old->MyOperTypes.end())
		{
			OperType *ot = o->GetType();
			o->SetType(nullptr);

			for (unsigned j = 0; j < MyOperTypes.size(); ++j)
				if (ot->GetName() == MyOperTypes[j]->GetName())
					o->SetType(MyOperTypes[j]);

			if (o->GetType() == NULL)
			{
				/* Oper block has lost type */
				o->Delete();
			}
		}
	}

	for (BotInfo *bi : Serialize::GetObjects<BotInfo *>(botinfo))
	{
		if (!bi->conf)
		{
			bi->Delete();
			continue;
		}

		for (int i = 0; i < bi->conf->CountBlock("channel"); ++i)
		{
			Block *channel = bi->conf->GetBlock("channel", i);

			const Anope::string &chname = channel->Get<Anope::string>("name"),
				&modes = channel->Get<Anope::string>("modes");

			bi->bot->Join(chname);

			Channel *c = Channel::Find(chname);
			if (!c)
				continue; // Can't happen

			c->botchannel = true;

			/* Remove all existing modes */
			ChanUserContainer *cu = c->FindUser(bi->bot);
			if (cu != NULL)
				for (size_t j = 0; j < cu->status.Modes().length(); ++j)
					c->RemoveMode(bi->bot, ModeManager::FindChannelModeByChar(cu->status.Modes()[j]), bi->bot->GetUID());
			/* Set the new modes */
			for (unsigned j = 0; j < modes.length(); ++j)
			{
				ChannelMode *cm = ModeManager::FindChannelModeByChar(modes[j]);
				if (cm == NULL)
					cm = ModeManager::FindChannelModeByChar(ModeManager::GetStatusChar(modes[j]));
				if (cm && cm->type == MODE_STATUS)
					c->SetMode(bi->bot, cm, bi->bot->GetUID());
			}
		}
	}
}

Block *Conf::GetModule(Module *m)
{
	if (!m)
		return NULL;

	return GetModule(m->name);
}

Block *Conf::GetModule(const Anope::string &mname)
{
	std::map<Anope::string, Block *>::iterator it = modules.find(mname);
	if (it != modules.end())
		return it->second;

	Block* &block = modules[mname];

	/* Search for the block */
	for (std::pair<block_map::iterator, block_map::iterator> iters = blocks.equal_range("module"); iters.first != iters.second; ++iters.first)
	{
		Block *b = &iters.first->second;

		if (b->Get<Anope::string>("name") == mname)
		{
			block = b;
			break;
		}
	}

	return GetModule(mname);
}

ServiceBot *Conf::GetClient(const Anope::string &cname)
{
	Anope::map<Anope::string>::iterator it = bots.find(cname);
	if (it != bots.end())
		return ServiceBot::Find(!it->second.empty() ? it->second : cname, true);

	Block *block = GetModule(cname.lower());
	const Anope::string &client = block->Get<Anope::string>("client");
	bots[cname] = client;
	return GetClient(cname);
}

Block *Conf::GetCommand(CommandSource &source)
{
	const Anope::string &block_name = source.c ? "fantasy" : "command";

	for (std::pair<block_map::iterator, block_map::iterator> iters = blocks.equal_range(block_name); iters.first != iters.second; ++iters.first)
	{
		Block *b = &iters.first->second;

		if (b->Get<Anope::string>("name") == source.command)
			return b;
	}

	return NULL;
}

File::File(const Anope::string &n, bool e) : name(n), executable(e), fp(NULL)
{
}

File::~File()
{
	this->Close();
}

const Anope::string &File::GetName() const
{
	return this->name;
}

bool File::IsOpen() const
{
	return this->fp != NULL;
}

bool File::Open()
{
	this->Close();
	this->fp = (this->executable ? popen(this->name.c_str(), "r") : fopen((Anope::ConfigDir + "/" + this->name).c_str(), "r"));
	return this->fp != NULL;
}

void File::Close()
{
	if (this->fp != NULL)
	{
		if (this->executable)
			pclose(this->fp);
		else
			fclose(this->fp);
		this->fp = NULL;
	}
}

bool File::End() const
{
	return !this->IsOpen() || feof(this->fp);
}

Anope::string File::Read()
{
	Anope::string ret;
	char buf[BUFSIZE];
	while (fgets(buf, sizeof(buf), this->fp) != NULL)
	{
		char *nl = strchr(buf, '\n');
		if (nl != NULL)
			*nl = 0;
		else if (!this->End())
		{
			ret += buf;
			continue;
		}

		ret = buf;
		break;
	}

	return ret;
}

void Conf::LoadConf(File &file)
{
	if (!file.Open())
		throw ConfigException("File " + file.GetName() + " could not be opened.");

	Anope::string itemname, wordbuffer;
	std::stack<Block *> block_stack;
	int linenumber = 0;
	bool in_word = false, in_quote = false, in_comment = false;

	Log(LOG_DEBUG) << "Start to read conf " << file.GetName();
	// Start reading characters...
	while (!file.End())
	{
		Anope::string line = file.Read();
		++linenumber;

		/* If this line is completely empty and we are in a quote, just append a newline */
		if (line.empty() && in_quote)
			wordbuffer += "\n";

		for (unsigned c = 0, len = line.length(); c < len; ++c)
		{
			char ch = line[c];
			if (in_quote)
			{
				/* Strip leading white spaces from multi line quotes */
				if (c == 0)
				{
					while (c < len && isspace(line[c]))
						++c;
					ch = line[c];
				}

				/* Allow \" in quotes */
				if (ch == '\\' && c + 1 < len && line[c + 1] == '"')
					wordbuffer += line[++c];
				else if (ch == '"')
					in_quote = in_word = false;
				else if (ch)
					wordbuffer += ch;
			}
			else if (in_comment)
			{
				if (ch == '*' && c + 1 < len && line[c + 1] == '/')
				{
					in_comment = false;
					++c;
					// We might be at an eol, so continue on and process it
				}
				else
					continue;
			}
			else if (ch == '#' || (ch == '/' && c + 1 < len && line[c + 1] == '/'))
				c = len - 1; // Line comment, ignore the rest of the line (much like this one!)
			else if (ch == '/' && c + 1 < len && line[c + 1] == '*')
			{
				// Multiline (or less than one line) comment
				in_comment = true;
				++c;
				continue;
			}
			else if (!in_word && (ch == '(' || ch == '_' || ch == ')'))
				;
			else if (ch == '"')
			{
				// Quotes are valid only in the value position
				if (block_stack.empty() || itemname.empty())
				{
					file.Close();
					throw ConfigException("Unexpected quoted string: " + file.GetName() + ":" + stringify(linenumber));
				}
				if (in_word || !wordbuffer.empty())
				{
					file.Close();
					throw ConfigException("Unexpected quoted string (prior unhandled words): " + file.GetName() + ":" + stringify(linenumber));
				}
				in_quote = in_word = true;
			}
			else if (ch == '=')
			{
				if (block_stack.empty())
				{
					file.Close();
					throw ConfigException("Config item outside of section (or stray '='): " + file.GetName() + ":" + stringify(linenumber));
				}

				if (!itemname.empty() || wordbuffer.empty())
				{
					file.Close();
					throw ConfigException("Stray '=' sign or item without value: " + file.GetName() + ":" + stringify(linenumber));
				}

				in_word = false;
				itemname = wordbuffer;
				wordbuffer.clear();
			}
			else if (ch == '{')
			{
				if (wordbuffer.empty())
				{
					block_stack.push(NULL);
					// Commented or unnamed section
					continue;
				}

				if (!block_stack.empty() && !block_stack.top())
				{
					// Named block inside of a commented block
					in_word = false;
					wordbuffer.clear();
					block_stack.push(NULL);
					continue;
				}

				Block *b = block_stack.empty() ? this : block_stack.top();
				block_map::iterator it = b->blocks.insert(std::make_pair(wordbuffer, Configuration::Block(wordbuffer)));
				b = &it->second;
				b->linenum = linenumber;
				block_stack.push(b);

				in_word = false;
				wordbuffer.clear();
				continue;
			}
			else if (ch == ' ' || ch == '\r' || ch == '\t')
			{
				// Terminate word
				in_word = false;
			}
			else if (ch == ';' || ch == '}')
				;
			else
			{
				if (!in_word && !wordbuffer.empty())
				{
					file.Close();
					throw ConfigException("Unexpected word: " + file.GetName() + ":" + stringify(linenumber));
				}
				wordbuffer += ch;
				in_word = true;
			}

			if (ch == ';' || ch == '}' || c + 1 >= len)
			{
				bool eol = c + 1 >= len;

				if (!eol && in_quote)
					// Allow ; and } in quoted strings
					continue;

				if (in_quote)
				{
					// Quotes can span multiple lines; all we need to do is go to the next line without clearing things
					wordbuffer += "\n";
					continue;
				}

				in_word = false;
				if (!itemname.empty())
				{
					if (block_stack.empty())
					{
						file.Close();
						throw ConfigException("Stray ';' outside of block: " + file.GetName() + ":" + stringify(linenumber));
					}

					Block *b = block_stack.top();

					if (b)
						Log(LOG_DEBUG) << "ln " << linenumber << " EOL: s='" << b->name << "' '" << itemname << "' set to '" << wordbuffer << "'";

					/* Check defines */
					for (int i = 0; i < this->CountBlock("define"); ++i)
					{
						Block *define = this->GetBlock("define", i);

						const Anope::string &dname = define->Get<Anope::string>("name");

						if (dname == wordbuffer && define != b)
							wordbuffer = define->Get<Anope::string>("value");
					}

					if (b)
						b->items[itemname] = wordbuffer;

					wordbuffer.clear();
					itemname.clear();
				}

				if (ch == '}')
				{
					if (block_stack.empty())
					{
						file.Close();
						throw ConfigException("Stray '}': " + file.GetName() + ":" + stringify(linenumber));
					}

					block_stack.pop();
				}
			}
		}
	}

	file.Close();

	if (in_comment)
		throw ConfigException("Unterminated multiline comment at end of file: " + file.GetName());
	if (in_quote)
		throw ConfigException("Unterminated quote at end of file: " + file.GetName());
	if (!itemname.empty() || !wordbuffer.empty())
		throw ConfigException("Unexpected garbage at end of file: " + file.GetName());
	if (!block_stack.empty())
		throw ConfigException("Unterminated block at end of file: " + file.GetName() + ". Block was opened on line " + stringify(block_stack.top()->linenum));
}


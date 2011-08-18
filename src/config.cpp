/* Configuration file handling.
 *
 * (C) 2003-2011 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include "services.h"
#include "config.h"
#include "module.h"

/*************************************************************************/

ConfigurationFile services_conf("services.conf", false); // Services configuration file name
ServerConfig *Config = NULL;

static Anope::string UlineServers;
static Anope::string OSNotifications;
static Anope::string BSDefaults;
static Anope::string CSDefaults;
static Anope::string NSDefaults;

/*************************************************************************/

ServerConfig::ServerConfig() : config_data(), NSDefFlags(NickCoreFlagStrings), CSDefFlags(ChannelInfoFlagStrings), BSDefFlags(BotServFlagStrings)
{
	this->Read();

	if (NSDefaults.empty())
	{
		this->NSDefFlags.SetFlag(NI_SECURE);
		this->NSDefFlags.SetFlag(NI_MEMO_SIGNON);
		this->NSDefFlags.SetFlag(NI_MEMO_RECEIVE);
	}
	else if (!NSDefaults.equals_ci("none"))
	{
		spacesepstream options(NSDefaults);
		Anope::string option;
		while (options.GetToken(option))
		{
			if (option.equals_ci("kill"))
				this->NSDefFlags.SetFlag(NI_KILLPROTECT);
			else if (option.equals_ci("killquick"))
				this->NSDefFlags.SetFlag(NI_KILL_QUICK);
			else if (option.equals_ci("secure"))
				this->NSDefFlags.SetFlag(NI_SECURE);
			else if (option.equals_ci("private"))
				this->NSDefFlags.SetFlag(NI_PRIVATE);
			else if (option.equals_ci("msg"))
			{
				if (!this->UsePrivmsg)
					Log() << "msg in <nickserv:defaults> can only be used when UsePrivmsg is set";
				else
					this->NSDefFlags.SetFlag(NI_MSG);
			}
			else if (option.equals_ci("hideemail"))
				this->NSDefFlags.SetFlag(NI_HIDE_EMAIL);
			else if (option.equals_ci("hideusermask"))
				this->NSDefFlags.SetFlag(NI_HIDE_MASK);
			else if (option.equals_ci("hidequit"))
				this->NSDefFlags.SetFlag(NI_HIDE_QUIT);
			else if (option.equals_ci("memosignon"))
				this->NSDefFlags.SetFlag(NI_MEMO_SIGNON);
			else if (option.equals_ci("memoreceive"))
				this->NSDefFlags.SetFlag(NI_MEMO_RECEIVE);
			else if (option.equals_ci("autoop"))
				this->NSDefFlags.SetFlag(NI_AUTOOP);
		}
	}

	if (this->CSDefBantype < 0 || this->CSDefBantype > 3)
	{
		throw ConfigException("Value of CSDefBantype must be between 0 and 3 included");
	}

	if (CSDefaults.empty())
	{
		this->CSDefFlags.SetFlag(CI_KEEPTOPIC);
		this->CSDefFlags.SetFlag(CI_SECURE);
		this->CSDefFlags.SetFlag(CI_SECUREFOUNDER);
		this->CSDefFlags.SetFlag(CI_SIGNKICK);
	}
	else if (!CSDefaults.equals_ci("none"))
	{
		spacesepstream options(CSDefaults);
		Anope::string option;
		while (options.GetToken(option))
		{
			if (option.equals_ci("keeptopic"))
				this->CSDefFlags.SetFlag(CI_KEEPTOPIC);
			else if (option.equals_ci("topiclock"))
				this->CSDefFlags.SetFlag(CI_TOPICLOCK);
			else if (option.equals_ci("private"))
				this->CSDefFlags.SetFlag(CI_PRIVATE);
			else if (option.equals_ci("restricted"))
				this->CSDefFlags.SetFlag(CI_RESTRICTED);
			else if (option.equals_ci("secure"))
				this->CSDefFlags.SetFlag(CI_SECURE);
			else if (option.equals_ci("secureops"))
				this->CSDefFlags.SetFlag(CI_SECUREOPS);
			else if (option.equals_ci("securefounder"))
				this->CSDefFlags.SetFlag(CI_SECUREFOUNDER);
			else if (option.equals_ci("signkick"))
				this->CSDefFlags.SetFlag(CI_SIGNKICK);
			else if (option.equals_ci("signkicklevel"))
				this->CSDefFlags.SetFlag(CI_SIGNKICK_LEVEL);
			else if (option.equals_ci("opnotice"))
				this->CSDefFlags.SetFlag(CI_OPNOTICE);
			else if (option.equals_ci("peace"))
				this->CSDefFlags.SetFlag(CI_PEACE);
			else if (option.equals_ci("persist"))
				this->CSDefFlags.SetFlag(CI_PERSIST);
		}
	}

	if (UseStrictPrivMsg)
		UseStrictPrivMsgString = "/";
	else
		UseStrictPrivMsgString ="/msg ";


	if (!BSDefaults.empty())
	{
		spacesepstream options(BSDefaults);
		Anope::string option;
		while (options.GetToken(option))
		{
			if (option.equals_ci("dontkickops"))
				this->BSDefFlags.SetFlag(BS_DONTKICKOPS);
			else if (option.equals_ci("dontkickvoices"))
				this->BSDefFlags.SetFlag(BS_DONTKICKVOICES);
			else if (option.equals_ci("greet"))
				this->BSDefFlags.SetFlag(BS_GREET);
			else if (option.equals_ci("fantasy"))
				this->BSDefFlags.SetFlag(BS_FANTASY);
		}
	}

	this->WallOper = this->WallBadOS = this->WallAkillExpire = this->WallSNLineExpire = this->WallSQLineExpire =
	this->WallSZLineExpire = this->WallExceptionExpire = false;
	if (!OSNotifications.empty())
	{
		spacesepstream notifications(OSNotifications);
		Anope::string notice;
		while (notifications.GetToken(notice))
		{
			if (notice.equals_ci("oper"))
				this->WallOper = true;
			else if (notice.equals_ci("bados"))
				this->WallBadOS = true;
			else if (notice.equals_ci("akillexpire"))
				this->WallAkillExpire = true;
			else if (notice.equals_ci("snlineexpire"))
				this->WallSNLineExpire = true;
			else if (notice.equals_ci("sqlineexpire"))
				this->WallSQLineExpire = true;
			else if (notice.equals_ci("szlineexpire"))
				this->WallSZLineExpire = true;
			else if (notice.equals_ci("exceptionexpire"))
				this->WallExceptionExpire = true;
		}
	}

	/* Ulines */
	if (!UlineServers.empty())
	{
		this->Ulines.clear();

		spacesepstream ulines(UlineServers);
		Anope::string uline;
		while (ulines.GetToken(uline))
			this->Ulines.push_back(uline);
	}

	if (this->LimitSessions)
	{
		if (this->MaxSessionKill && !this->SessionAutoKillExpiry)
			this->SessionAutoKillExpiry = 1800; /* 30 minutes */
	}

	/* Check the user keys */
	if (this->UserKey1 == this->UserKey2 || this->UserKey1 == this->UserKey3 || this->UserKey3 == this->UserKey2)
		Log() << "Every UserKey must be different. It's for YOUR safety! Remember that!";

	SetDefaultMLock(this);

	if (IsFile(this->NameServer))
	{
		std::ifstream f(this->NameServer.c_str());
		Anope::string server;
		bool success = false;

		while (f.is_open() && getline(f, server.str()))
		{
			if (server.find("nameserver") == 0)
			{
				size_t ip = server.find_first_of("123456789");
				if (ip != Anope::string::npos)
				{
					if (server.substr(ip).is_pos_number_only())
					{
						this->NameServer = server.substr(ip);
						Log(LOG_DEBUG) << "Nameserver set to " << this->NameServer;
						success = true;
						break;
					}
				}
			}
		}

		if (f.is_open())
			f.close();

		if (!success)
		{
			Log() << "Unable to find nameserver, defaulting to 127.0.0.1";
			this->NameServer = "127.0.0.1";
		}
	}
}

bool ServerConfig::CheckOnce(const Anope::string &tag)
{
	int count = ConfValueEnum(config_data, tag);
	if (count > 1)
		throw ConfigException("You have more than one <" + tag + "> tag, this is not permitted.");
	if (count < 1)
		throw ConfigException("You have not defined a <" + tag + "> tag, this is required.");
	return true;
}

bool NoValidation(ServerConfig *, const Anope::string &, const Anope::string &, ValueItem &)
{
	return true;
}

void ServerConfig::ValidateNoSpaces(const Anope::string &p, const Anope::string &tag, const Anope::string &val) const
{
	for (Anope::string::const_iterator ptr = p.begin(), end = p.end(); ptr != end; ++ptr)
		if (*ptr == ' ')
			throw ConfigException("The value of <" + tag + ":" + val + "> cannot contain spaces");
}

/* NOTE: Before anyone asks why we're not using inet_pton for this, it is because inet_pton and friends do not return so much detail,
 * even in LastError(). They just return 'yes' or 'no' to an address without such detail as to whats WRONG with the address.
 * Because ircd users arent as technical as they used to be (;)) we are going to give more of a useful error message.
 */
void ServerConfig::ValidateIP(const Anope::string &p, const Anope::string &tag, const Anope::string &val, bool wild) const
{
	int num_dots = 0, num_seps = 0;
	bool not_numbers = false, not_hex = false;

	if (!p.empty())
	{
		if (p[0] == '.')
			throw ConfigException("The value of <" + tag + ":" + val + "> is not an IP address");

		for (Anope::string::const_iterator ptr = p.begin(), end = p.end(); ptr != end; ++ptr)
		{
			if (wild && (*ptr == '*' || *ptr == '?' || *ptr == '/'))
				continue;

			if (*ptr != ':' && *ptr != '.' && (*ptr < '0' || *ptr > '9'))
			{
				not_numbers = true;
				if (toupper(*ptr) < 'A' || toupper(*ptr) > 'F')
					not_hex = true;
			}
			switch (*ptr)
			{
				case ' ':
					throw ConfigException("The value of <" + tag + ":" + val + "> is not an IP address");
				case '.':
					++num_dots;
					break;
				case ':':
					++num_seps;
			}
		}
		if (num_dots > 3)
			throw ConfigException("The value of <" + tag + ":" + val + "> is an IPv4 address with too many fields!");

		if (num_seps > 8)
			throw ConfigException("The value of <" + tag + ":" + val + "> is an IPv6 address with too many fields!");

		if (!num_seps && num_dots < 3 && !wild)
			throw ConfigException("The value of <" + tag + ":" + val + "> looks to be a malformed IPv4 address");

		if (!num_seps && num_dots == 3 && not_numbers)
			throw ConfigException("The value of <" + tag + ":" + val + "> contains non-numeric characters in an IPv4 address");

		if (num_seps && not_hex)
			throw ConfigException("The value of <" + tag + ":" + val + "> contains non-hexdecimal characters in an IPv6 address");

		if (num_seps && num_dots != 3 && num_dots && !wild)
			throw ConfigException("The value of <" + tag + ":" + val + "> is a malformed IPv6 4in6 address");
	}
}

void ServerConfig::ValidateHostname(const Anope::string &p, const Anope::string &tag, const Anope::string &val) const
{
	if (p.equals_ci("localhost"))
		return;

	int num_dots = 0, num_seps = 0;
	if (!p.empty())
	{
		if (p[0] == '.')
			throw ConfigException("The value of <" + tag + ":" + val + "> is not a valid hostname");
		for (unsigned i = 0, end = p.length(); i < end; ++i)
		{
			switch (p[i])
			{
				case ' ':
					throw ConfigException("The value of <" + tag + ":" + val + "> is not a valid hostname");
				case '.':
					++num_dots;
					break;
				case ':':
					++num_seps;
					break;
			}
		}
		if (!num_dots && !num_seps)
			throw ConfigException("The value of <" + tag + ":" + val + "> is not a valid hostname");
	}
}

bool ValidateNotEmpty(ServerConfig *, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (data.GetValue().empty())
		throw ConfigException("The value for <" + tag + ":" + value + "> cannot be empty!");
	return true;
}

bool ValidateNotZero(ServerConfig *, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (!data.GetInteger() && dotime(data.GetValue()) <= 0)
		throw ConfigException("The value for <" + tag + ":" + value + "> must be non-zero!");
	return true;
}

bool ValidateEmailReg(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (config->NSEmailReg)
	{
		if (value.equals_ci("unconfirmedexpire"))
		{
			if (!data.GetInteger() && dotime(data.GetValue()) <= 0)
				throw ConfigException("The value for <" + tag + ":" + value + "> must be non-zero when e-mail registration are enabled!");
		}
		else
		{
			if (!data.GetBool())
				throw ConfigException("The value for <" + tag + ":" + value + "> must be set to yes when e-mail registrations are enabled!");
		}
	}
	return true;
}

bool ValidatePort(ServerConfig *, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	int port = data.GetInteger();
	if (!port)
		return true;
	if (port < 1 || port > 65535)
		throw ConfigException("The value for <" + tag + ":" + value + "> is not a value port, it must be between 1 and 65535!");
	return true;
}

bool ValidateBantype(ServerConfig *, const Anope::string &, const Anope::string &, ValueItem &data)
{
	int bantype = data.GetInteger();
	if (bantype < 0 || bantype > 3)
		throw ConfigException("The value for <chanserv:defbantype> must be between 0 and 3!");
	return true;
}

bool ValidateNickServ(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (!config->NickServ.empty())
	{
		if (value.equals_ci("releasetimeout") || value.equals_ci("accessmax") || value.equals_ci("listmax"))
			return ValidateNotZero(config, tag, value, data);
		else if (value.equals_ci("enforceruser") || value.equals_ci("enforcerhost"))
			return ValidateNotEmpty(config, tag, value, data);
		else if (value.equals_ci("guestnickprefix"))
		{
			ValidateNotEmpty(config, tag, value, data);
			if (data.GetValue().length() > 21)
				throw ConfigException("The value for <nickserv:guestnickprefix> cannot exceed 21 characters in length!");
		}
	}
	return true;
}

bool ValidateChanServ(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (!config->ChanServ.empty())
	{
		if ((value.equals_ci("decription") || value.equals_ci("autokickreason")) && data.GetValue().empty())
			throw ConfigException("The value for <" + tag + ":" + value + "> cannot be empty when ChanServ is enabled!");
		else if (value.equals_ci("defbantype"))
			return ValidateBantype(config, tag, value, data);
		else if (value.equals_ci("accessmax") || value.equals_ci("autokickmax") || value.equals_ci("inhabit") || value.equals_ci("listmax"))
			return ValidateNotZero(config, tag, value, data);
	}
	return true;
}

bool ValidateMemoServ(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (!config->MemoServ.empty())
	{
		if (value.equals_ci("description"))
		{
			if (data.GetValue().empty())
				throw ConfigException("The value for <" + tag + ":" + value + "> cannot be empty when MemoServ is enabled!");
		}
	}
	return true;
}

bool ValidateBotServ(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (!config->BotServ.empty())
	{
		if (value.equals_ci("description"))
		{
			if (data.GetValue().empty())
				throw ConfigException("The value for <" + tag + ":" + value + "> cannot be empty when BotServ is enabled!");
		}
		else if (value.equals_ci("badwordsmax") || value.equals_ci("keepdata"))
		{
			if (!data.GetInteger() && dotime(data.GetValue()) <= 0)
				throw ConfigException("The value for <" + tag + ":" + value + "> must be non-zero when BotServ is enabled!");
		}
		else if (value.equals_ci("minusers"))
		{
			if (data.GetInteger() < 0)
				throw ConfigException("The value for <" + tag + ":" + value + "> must be greater than or equal to zero!");
		}
	}
	return true;
}

bool ValidateHostServ(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (!config->HostServ.empty())
	{
		if (value.equals_ci("description") && data.GetValue().empty())
			throw ConfigException("The value for <" + tag + ":" + value + "> cannot be empty when HostServ is enabled!");
	}
	return true;
}

bool ValidateLimitSessions(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (config->LimitSessions)
	{
		if (value.equals_ci("maxsessionlimit") || value.equals_ci("exceptionexpiry"))
		{
			if (!data.GetInteger() && dotime(data.GetValue()) <= 0)
				throw ConfigException("The value for <" + tag + ":" + value + "> must be non-zero when session limiting is enabled!");
		}
	}
	return true;
}

bool ValidateOperServ(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (!config->OperServ.empty())
	{
		if (value.equals_ci("description") && data.GetValue().empty())
			throw ConfigException("The value for <" + tag + ":" + value + "> cannot be empty when OperServ is enabled!");
		else if (value.equals_ci("autokillexpiry") || value.equals_ci("chankillexpiry") || value.equals_ci("snlineexpiry") || value.equals_ci("szlineexpiry") || value.equals_ci("sqlineexpiry"))
			return ValidateNotZero(config, tag, value, data);
		else if (value.equals_ci("maxsessionlimit") || value.equals_ci("exceptionexpiry"))
			return ValidateLimitSessions(config, tag, value, data);
	}
	return true;
}

bool ValidateGlobal(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (!config->Global.empty())
	{
		if (value.equals_ci("description") && data.GetValue().empty())
			throw ConfigException("The value for <" + tag + ":" + value + "> cannot be empty when Global is enabled!");
	}
	return true;
}

bool ValidateNickLen(ServerConfig *, const Anope::string &, const Anope::string &, ValueItem &data)
{
	int nicklen = data.GetInteger();
	if (!nicklen)
	{
		Log() << "You have not defined the <networkinfo:nicklen> directive. It is strongly";
		Log() << "adviced that you do configure this correctly in your services.conf";
		data.Set(31);
	}
	else if (nicklen < 1)
	{
		Log() << "<networkinfo:nicklen> has an invalid value; setting to 31";
		data.Set(31);
	}
	return true;
}

bool ValidateMail(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (config->UseMail)
	{
		if (value.equals_ci("sendmailpath") || value.equals_ci("sendfrom"))
		{
			if (data.GetValue().empty())
				throw ConfigException("The value for <" + tag + ":" + value + "> cannot be empty when e-mail is enabled!");
		}
	}
	return true;
}

bool ValidateGlobalOnCycle(ServerConfig *config, const Anope::string &tag, const Anope::string &value, ValueItem &data)
{
	if (config->GlobalOnCycle)
	{
		if (data.GetValue().empty())
		{
			Log() << "<" << tag << ":" << value << "> was undefined, disabling <options:globaloncycle>";
			config->GlobalOnCycle = false;
		}
	}
	return true;
}

bool InitUplinks(ServerConfig *config, const Anope::string &)
{
	if (!config->Uplinks.empty())
	{
		std::vector<Uplink *>::iterator curr_uplink = config->Uplinks.begin(), end_uplink = config->Uplinks.end();
		for (; curr_uplink != end_uplink; ++curr_uplink)
			delete *curr_uplink;
	}
	config->Uplinks.clear();
	return true;
}

static bool DoUplink(ServerConfig *config, const Anope::string &, const Anope::string *, ValueList &values, int *)
{
	// Validation variables
	Anope::string host = values[0].GetValue(), password = values[3].GetValue();
	int port = values[2].GetInteger();
	bool ipv6 = values[1].GetBool();
	ValueItem vi_host(host), vi_port(port), vi_password(password);
	// Validate the host to make sure it is not empty
	if (!ValidateNotEmpty(config, "uplink", "host", vi_host))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	// Validate the port to make sure it is a valid port
	if (!ValidatePort(config, "uplink", "port", vi_port))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	// Validate the password to make sure it is not empty
	if (!ValidateNotEmpty(config, "uplink", "password", vi_password))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	// If we get here, all the values are valid, we'll add it to the Uplinks list
	config->Uplinks.push_back(new Uplink(host, port, password, ipv6));
	return true;
}

static bool DoneUplinks(ServerConfig *config, const Anope::string &)
{
	if (config->Uplinks.empty())
		throw ConfigException("You must define at least one uplink block!");
	return true;
}

static bool InitOperTypes(ServerConfig *config, const Anope::string &)
{
	for (std::list<OperType *>::iterator it = config->MyOperTypes.begin(), it_end = config->MyOperTypes.end(); it != it_end; ++it)
		delete *it;

	config->MyOperTypes.clear();
	return true;
}

static bool DoOperType(ServerConfig *config, const Anope::string &, const Anope::string *, ValueList &values, int *)
{
	Anope::string name = values[0].GetValue();
	Anope::string inherits = values[1].GetValue();
	Anope::string commands = values[2].GetValue();
	Anope::string privs = values[3].GetValue();
	Anope::string modes = values[4].GetValue();

	ValueItem vi(name);
	if (!ValidateNotEmpty(config, "opertype", "name", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");

	OperType *ot = new OperType(name);
	ot->modes = modes;

	Anope::string tok;
	spacesepstream cmdstr(commands);
	while (cmdstr.GetToken(tok))
		ot->AddCommand(tok);

	spacesepstream privstr(privs);
	while (privstr.GetToken(tok))
		ot->AddPriv(tok);

	commasepstream inheritstr(inherits);
	while (inheritstr.GetToken(tok))
	{
		/* Strip leading ' ' after , */
		if (tok.length() > 1 && tok[0] == ' ')
			tok.erase(tok.begin());
		for (std::list<OperType *>::iterator it = config->MyOperTypes.begin(), it_end = config->MyOperTypes.end(); it != it_end; ++it)
		{
			if ((*it)->GetName().equals_ci(tok))
			{
				Log() << "Inheriting commands and privs from " << (*it)->GetName() << " to " << ot->GetName();
				ot->Inherits(*it);
				break;
			}
		}
	}

	config->MyOperTypes.push_back(ot);
	return true;
}

static bool DoneOperTypes(ServerConfig *, const Anope::string &)
{
	return true;
}

/*************************************************************************/

static bool InitOpers(ServerConfig *config, const Anope::string &)
{
	for (nickcore_map::const_iterator it = NickCoreList.begin(), it_end = NickCoreList.end(); it != it_end; ++it)
		it->second->o = NULL;

	for (unsigned i = 0; i < config->Opers.size(); ++i)
		delete config->Opers[i];
	config->Opers.clear();

	return true;
}

static bool DoOper(ServerConfig *config, const Anope::string &, const Anope::string *, ValueList &values, int *)
{
	Anope::string name = values[0].GetValue();
	Anope::string type = values[1].GetValue();
	Anope::string password = values[2].GetValue();
	Anope::string certfp = values[3].GetValue();

	ValueItem vi(name);
	if (!ValidateNotEmpty(config, "oper", "name", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");

	ValueItem vi2(type);
	if (!ValidateNotEmpty(config, "oper", "type", vi2))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	
	OperType *ot = NULL;
	for (std::list<OperType *>::iterator it = config->MyOperTypes.begin(), it_end = config->MyOperTypes.end(); it != it_end; ++it)
		if ((*it)->GetName() == type)
			ot = *it;
	if (ot == NULL)
		throw ConfigException("Oper block for " + name + " has invalid oper type " + type);
	
	Oper *o = new Oper(name, password, certfp, ot);
	config->Opers.push_back(o);

	return true;
}

static bool DoneOpers(ServerConfig *config, const Anope::string &)
{
	for (unsigned i = 0; i < config->Opers.size(); ++i)
	{
		Oper *o = config->Opers[i];

		NickAlias *na = findnick(o->name);
		if (!na)
			// Nonexistant nick
			continue;

		na->nc->o = o;
		Log() << "Tied oper " << na->nc->display << " to type " << o->ot->GetName();
	}

	return true;
}

/*************************************************************************/

static std::map<Anope::string, Anope::string> defines;
static bool InitDefine(ServerConfig *config, const Anope::string &)
{
	defines.clear();
	return true;
}

static bool DoDefine(ServerConfig *config, const Anope::string &, const Anope::string *, ValueList &values, int *)
{
	Anope::string name = values[0].GetValue(), value = values[1].GetValue();
	defines[name] = value;
	return true;
}

static bool DoneDefine(ServerConfig *config, const Anope::string &)
{
	return true;
}

/*************************************************************************/

static bool InitInclude(ServerConfig *config, const Anope::string &)
{
	return true;
}

static bool DoInclude(ServerConfig *config, const Anope::string &, const Anope::string *, ValueList &values, int *)
{
	Anope::string type = values[0].GetValue();
	Anope::string file = values[1].GetValue();

	if (type != "file" && type != "executable")
		throw ConfigException("include:type must be either \"file\" or \"executable\"");
	
	ConfigurationFile f(file, type == "executable");
	config->LoadConf(f);

	return true;
}

static bool DoneInclude(ServerConfig *config, const Anope::string &)
{
	return true;
}

/*************************************************************************/

bool InitModules(ServerConfig *, const Anope::string &)
{
	return true;
}

static bool DoModule(ServerConfig *conf, const Anope::string &, const Anope::string *, ValueList &values, int *)
{
	// First we validate that there was a name in the module block
	Anope::string module = values[0].GetValue();
	ValueItem vi(module);
	if (!ValidateNotEmpty(conf, "module", "name", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	conf->ModulesAutoLoad.push_back(module);
	return true;
}

static bool DoneModules(ServerConfig *config, const Anope::string &)
{
	if (Config)
	{
		for (std::list<Anope::string>::iterator it = Config->ModulesAutoLoad.begin(); it != Config->ModulesAutoLoad.end(); ++it)
			if (std::find(config->ModulesAutoLoad.begin(), config->ModulesAutoLoad.end(), *it) == config->ModulesAutoLoad.end())
				ModuleManager::UnloadModule(ModuleManager::FindModule(*it), NULL);
		for (std::list<Anope::string>::iterator it = config->ModulesAutoLoad.begin(); it != config->ModulesAutoLoad.end(); ++it)
			if (std::find(Config->ModulesAutoLoad.begin(), Config->ModulesAutoLoad.end(), *it) == Config->ModulesAutoLoad.end())
				ModuleManager::LoadModule(*it, NULL);
	}
	return true;
}

bool InitLogs(ServerConfig *config, const Anope::string &)
{
	for (unsigned i = 0; i < config->LogInfos.size(); ++i)
	{
		LogInfo *l = config->LogInfos[i];

		for (std::list<Anope::string>::const_iterator sit = l->Targets.begin(), sit_end = l->Targets.end(); sit != sit_end; ++sit)
		{
			const Anope::string &target = *sit;

			if (target[0] == '#')
			{
				Channel *c = findchan(target);
				if (c && c->HasFlag(CH_LOGCHAN))
				{
					for (CUserList::const_iterator cit = c->users.begin(), cit_end = c->users.end(); cit != cit_end; ++cit)
					{
						UserContainer *uc = *cit;
						BotInfo *bi = findbot(uc->user->nick);

						if (bi)
							bi->Part(c, "Reloading configuration");
					}

					c->UnsetFlag(CH_PERSIST);
					c->UnsetFlag(CH_LOGCHAN);
					if (c->users.empty())
						delete c;
				}
			}
		}

		delete config->LogInfos[i];
	}
	config->LogInfos.clear();

	return true;
}

static bool DoLogs(ServerConfig *config, const Anope::string &, const Anope::string *, ValueList &values, int *)
{
	//{"target", "source", "logage", "inhabit", "admin", "override", "commands", "servers", "channels", "users", "other", "rawio", "debug"},
	Anope::string targets = values[0].GetValue();
	ValueItem vi(targets);
	if (!ValidateNotEmpty(config, "log", "target", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	
	Anope::string source = values[1].GetValue();
	int logage = values[2].GetInteger();
	bool inhabit = values[3].GetBool();
	Anope::string admin = values[4].GetValue();
	Anope::string override = values[5].GetValue();
	Anope::string commands = values[6].GetValue();
	Anope::string servers = values[7].GetValue();
	Anope::string channels = values[8].GetValue();
	Anope::string users = values[9].GetValue();
	Anope::string normal = values[10].GetValue();
	bool rawio = values[11].GetBool();
	bool ldebug = values[12].GetBool();

	LogInfo *l = new LogInfo(logage, inhabit, rawio, ldebug);
	l->Targets = BuildStringList(targets);
	l->Sources = BuildStringList(source);
	l->Admin = BuildStringList(admin);
	l->Override = BuildStringList(override);
	l->Commands = BuildStringList(commands);
	l->Servers = BuildStringList(servers);
	l->Channels = BuildStringList(channels);
	l->Users = BuildStringList(users);
	l->Normal = BuildStringList(normal);

	config->LogInfos.push_back(l);

	return true;
}

static bool DoneLogs(ServerConfig *config, const Anope::string &)
{
	Log() << "Loaded " << config->LogInfos.size() << " log blocks";

	for (unsigned i = 0; i < config->LogInfos.size(); ++i)
	{
		LogInfo *l = config->LogInfos[i];

		if ((!ircd || !ircd->join2msg) && !l->Inhabit)
			continue;

		for (std::list<Anope::string>::const_iterator sit = l->Targets.begin(), sit_end = l->Targets.end(); sit != sit_end; ++sit)
		{
			const Anope::string &target = *sit;

			if (target[0] == '#')
			{
				Channel *c = findchan(target);
				if (!c)
					c = new Channel(target);
				c->SetFlag(CH_LOGCHAN);
				c->SetFlag(CH_PERSIST);

				for (Anope::insensitive_map<BotInfo *>::const_iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
				{
					BotInfo *bi = it->second;
					if (!c->FindUser(bi))
						bi->Join(c, NULL);
				}
			}
		}
	}

	return true;
}

/*************************************************************************/

static bool InitCommands(ServerConfig *config, const Anope::string &)
{
	for (botinfo_map::iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end; ++it)
		it->second->commands.clear();
	return true;
}

static bool DoCommands(ServerConfig *config, const Anope::string &, const Anope::string *, ValueList &values, int *)
{
	Anope::string service = values[0].GetValue();
	Anope::string name = values[1].GetValue();
	Anope::string command = values[2].GetValue();
	Anope::string permission = values[3].GetValue();

	ValueItem vi(service);
	if (!ValidateNotEmpty(config, "command", "service", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	
	vi = ValueItem(name);
	if (!ValidateNotEmpty(config, "command", "name", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	
	vi = ValueItem(command);
	if (!ValidateNotEmpty(config, "command", "command", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");
	
	BotInfo *bi = findbot(service);
	if (bi == NULL)
		throw ConfigException("Command " + name + " exists for nonexistant service " + service);

	if (bi->commands.count(name))
		throw ConfigException("Command name " + name + " already exists on " + bi->nick);

	bi->SetCommand(name, command, permission);
	return true;
}

static bool DoneCommands(ServerConfig *config, const Anope::string &)
{
	return true;
}

/*************************************************************************/

static std::set<Anope::string> services;
static bool InitServices(ServerConfig *config, const Anope::string &)
{
	services.clear();
	return true;
}

static bool DoServices(ServerConfig *config, const Anope::string &, const Anope::string *, ValueList &values, int *)
{
	Anope::string nick = values[0].GetValue();
	Anope::string user = values[1].GetValue();
	Anope::string host = values[2].GetValue();
	Anope::string gecos = values[3].GetValue();
	Anope::string modes = values[4].GetValue();

	ValueItem vi(nick);
	if (!ValidateNotEmpty(config, "service", "nick", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");

	vi = ValueItem(user);
	if (!ValidateNotEmpty(config, "service", "user", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");

	vi = ValueItem(host);
	if (!ValidateNotEmpty(config, "service", "host", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");

	vi = ValueItem(gecos);
	if (!ValidateNotEmpty(config, "service", "gecos", vi))
		throw ConfigException("One or more values in your configuration file failed to validate. Please see your log for more information.");

	services.insert(nick);
	BotInfo *bi = findbot(nick);
	if (bi != NULL)
		return true;
	bi = new BotInfo(nick, user, host, gecos, modes);
	bi->SetFlag(BI_CONF);
	return true;
}

static bool DoneServices(ServerConfig *config, const Anope::string &)
{
	for (botinfo_map::iterator it = BotListByNick.begin(), it_end = BotListByNick.end(); it != it_end;)
	{
		BotInfo *bi = it->second;
		++it;

		if (bi->HasFlag(BI_CONF) && services.count(bi->nick) == 0)
			delete bi;
	}
	services.clear();
	return true;
}

/*************************************************************************/

ConfigurationFile::ConfigurationFile(const Anope::string &n, bool e) : name(n), executable(e), fp(NULL)
{
}

ConfigurationFile::~ConfigurationFile()
{
	this->Close();
}

const Anope::string &ConfigurationFile::GetName() const
{
	return this->name;
}

bool ConfigurationFile::IsOpen() const
{
	return this->fp != NULL;
}

bool ConfigurationFile::Open()
{
	this->Close();
	this->fp = (this->executable ? popen(this->name.c_str(), "r") : fopen(this->name.c_str(), "r"));
	return this->fp != NULL;
}

void ConfigurationFile::Close()
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

bool ConfigurationFile::End() const
{
	return !this->IsOpen() || feof(this->fp);
}

Anope::string ConfigurationFile::Read()
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

ConfigItems::ConfigItems(ServerConfig *conf)
{
	// These tags can occur ONCE or not at all
	const Item Items[] = {
		/* The following comments are from CyberBotX to w00t as examples to use:
		 *
		 * The last argument, for validation, must use one of the functions with the following signature:
		 * bool <function>(ServerConfig *, const char *, const char *, ValueItem &)
		 * Examples are: NoValidation, ValidateNotEmpty, etc.
		 *
		 * If you want to create a directive using an integer:
		 * int blarg;
		 * {"tag", "value", "0", new ValueContainerInt(&conf->blarg), DT_INTEGER, <validation>},
		 *
		 * If you want to create a directive using an unsigned integer:
		 * unsigned blarg;
		 * {"tag", "value", "0", new ValueContainerUInt(&conf->blarg), DT_UINTEGER, <validation>},
		 *
		 * If you want to create a directive using a string:
		 * Anope::string blarg;
		 * {"tag", "value", "", new ValueContainerString(&conf->blarg), DT_STRING, <validation>},
		 *
		 * If you want to create a directive using a boolean:
		 * bool blarg;
		 * {"tag", "value", "no", new ValueContainerBool(&conf->blarg), DT_BOOLEAN, <validation>},
		 *
		 * If you want to create a directive using a character pointer specifically to hold a hostname (this will call ValidateHostname automatically):
		 * char *blarg;
		 * {"tag", "value", "", new ValueContainerChar(&conf->blarg), DT_HOSTNAME, <validation>},
		 *
		 * If you want to create a directive using a character pointer that specifically can not have spaces in it (this will call ValidateNoSpaces automatically):
		 * char *blarg;
		 * {"tag", "value", "", new ValueContainerChar(&conf->blarg), DT_NOSPACES, <validation>},
		 *
		 * If you want to create a directive using a character pointer specifically to hold an IP address (this will call ValidateIP automatically):
		 * char *blarg;
		 * {"tag", "value", "", new ValueContainerChar(&conf->blarg), DT_IPADDRESS, <validation>},
		 *
		 * If you want to create a directive using a time (a time_t variable converted from a string):
		 * time_t blarg;
		 * {"tag", "value", "", new ValueContainterTime(&conf->blarg), DT_TIME, <validation>},
		 *
		 * For the second-to-last argument, you can or (|) in the following values:
		 * DT_NORELOAD - The variable can't be changed on a reload of the configuration
		 * DT_ALLOW_WILD - Allows wildcards/CIDR in DT_IPADDRESS
		 * DT_ALLOW_NEWLINE - Allows new line characters in DT_STRING
		 *
		 * We may need to add some other validation functions to handle certain things, we can handle that later.
		 * Any questions about these, w00t, feel free to ask. */
		{"serverinfo", "name", "", new ValueContainerString(&conf->ServerName), DT_HOSTNAME | DT_NORELOAD, ValidateNotEmpty},
		{"serverinfo", "description", "", new ValueContainerString(&conf->ServerDesc), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"serverinfo", "localhost", "", new ValueContainerString(&conf->LocalHost), DT_HOSTNAME | DT_NORELOAD, NoValidation},
		{"serverinfo", "id", "", new ValueContainerString(&conf->Numeric), DT_NOSPACES | DT_NORELOAD, NoValidation},
		{"serverinfo", "pid", "services.pid", new ValueContainerString(&conf->PIDFilename), DT_STRING | DT_NORELOAD, ValidateNotEmpty},
		{"serverinfo", "motd", "services.motd", new ValueContainerString(&conf->MOTDFilename), DT_STRING, ValidateNotEmpty},
		{"networkinfo", "networkname", "", new ValueContainerString(&conf->NetworkName), DT_STRING, ValidateNotEmpty},
		{"networkinfo", "nicklen", "31", new ValueContainerUInt(&conf->NickLen), DT_UINTEGER | DT_NORELOAD, ValidateNickLen},
		{"networkinfo", "userlen", "10", new ValueContainerUInt(&conf->UserLen), DT_UINTEGER | DT_NORELOAD, NoValidation},
		{"networkinfo", "hostlen", "64", new ValueContainerUInt(&conf->HostLen), DT_UINTEGER | DT_NORELOAD, NoValidation},
		{"options", "passlen", "32", new ValueContainerUInt(&conf->PassLen), DT_UINTEGER | DT_NORELOAD, NoValidation},
		{"options", "userkey1", "0", new ValueContainerLUInt(&conf->UserKey1), DT_LUINTEGER, NoValidation},
		{"options", "userkey2", "0", new ValueContainerLUInt(&conf->UserKey2), DT_LUINTEGER, NoValidation},
		{"options", "userkey3", "0", new ValueContainerLUInt(&conf->UserKey3), DT_LUINTEGER, NoValidation},
		{"options", "nobackupokay", "no", new ValueContainerBool(&conf->NoBackupOkay), DT_BOOLEAN, NoValidation},
		{"options", "strictpasswords", "no", new ValueContainerBool(&conf->StrictPasswords), DT_BOOLEAN, NoValidation},
		{"options", "badpasslimit", "0", new ValueContainerUInt(&conf->BadPassLimit), DT_UINTEGER, NoValidation},
		{"options", "badpasstimeout", "0", new ValueContainerTime(&conf->BadPassTimeout), DT_TIME, NoValidation},
		{"options", "updatetimeout", "0", new ValueContainerTime(&conf->UpdateTimeout), DT_TIME, ValidateNotZero},
		{"options", "expiretimeout", "0", new ValueContainerTime(&conf->ExpireTimeout), DT_TIME, ValidateNotZero},
		{"options", "readtimeout", "0", new ValueContainerTime(&conf->ReadTimeout), DT_TIME, ValidateNotZero},
		{"options", "warningtimeout", "0", new ValueContainerTime(&conf->WarningTimeout), DT_TIME, ValidateNotZero},
		{"options", "timeoutcheck", "0", new ValueContainerTime(&conf->TimeoutCheck), DT_TIME, NoValidation},
		{"options", "keepbackups", "0", new ValueContainerInt(&conf->KeepBackups), DT_INTEGER, NoValidation},
		{"options", "forceforbidreason", "no", new ValueContainerBool(&conf->ForceForbidReason), DT_BOOLEAN, NoValidation},
		{"options", "useprivmsg", "no", new ValueContainerBool(&conf->UsePrivmsg), DT_BOOLEAN, NoValidation},
		{"options", "usestrictprivmsg", "no", new ValueContainerBool(&conf->UseStrictPrivMsg), DT_BOOLEAN, NoValidation},
		{"options", "hidestatso", "no", new ValueContainerBool(&conf->HideStatsO), DT_BOOLEAN, NoValidation},
		{"options", "nickregdelay", "0", new ValueContainerUInt(&conf->NickRegDelay), DT_UINTEGER, NoValidation},
		{"options", "restrictopernicks", "no", new ValueContainerBool(&conf->RestrictOperNicks), DT_BOOLEAN, NoValidation},
		{"options", "newscount", "3", new ValueContainerUInt(&conf->NewsCount), DT_UINTEGER, NoValidation},
		{"options", "ulineservers", "", new ValueContainerString(&UlineServers), DT_STRING, NoValidation},
		{"options", "mlock", "+nrt", new ValueContainerString(&conf->MLock), DT_STRING, NoValidation},
		{"options", "nomlock", "", new ValueContainerString(&conf->NoMLock), DT_STRING, NoValidation},
		{"options", "botmodes", "", new ValueContainerString(&conf->BotModes), DT_STRING, NoValidation},
		{"options", "retrywait", "60", new ValueContainerInt(&conf->RetryWait), DT_INTEGER, ValidateNotZero},
		{"options", "hideprivilegedcommands", "no", new ValueContainerBool(&conf->HidePrivilegedCommands), DT_BOOLEAN, NoValidation},
		{"nickserv", "name", "", new ValueContainerString(&conf->NickServ), DT_STRING, NoValidation},
		{"nickserv", "emailregistration", "no", new ValueContainerBool(&conf->NSEmailReg), DT_BOOLEAN, NoValidation},
		{"nickserv", "forceemail", "no", new ValueContainerBool(&conf->NSForceEmail), DT_BOOLEAN, ValidateEmailReg},
		{"nickserv", "confirmemailchanges", "no", new ValueContainerBool(&conf->NSConfirmEmailChanges), DT_BOOLEAN, NoValidation},
		{"nickserv", "defaults", "secure memosignon memoreceive", new ValueContainerString(&NSDefaults), DT_STRING, NoValidation},
		{"nickserv", "languages", "", new ValueContainerString(&conf->Languages), DT_STRING, NoValidation},
		{"nickserv", "defaultlanguage", "0", new ValueContainerString(&conf->NSDefLanguage), DT_STRING, NoValidation},
		{"nickserv", "regdelay", "0", new ValueContainerTime(&conf->NSRegDelay), DT_TIME, NoValidation},
		{"nickserv", "resenddelay", "0", new ValueContainerTime(&conf->NSResendDelay), DT_TIME, NoValidation},
		{"nickserv", "expire", "21d", new ValueContainerTime(&conf->NSExpire), DT_TIME, NoValidation},
		{"nickserv", "suspendexpire", "0", new ValueContainerTime(&conf->NSSuspendExpire), DT_TIME, NoValidation},
		{"nickserv", "unconfirmedexpire", "0", new ValueContainerTime(&conf->NSUnconfirmedExpire), DT_TIME, ValidateEmailReg},
		{"nickserv", "maxaliases", "0", new ValueContainerUInt(&conf->NSMaxAliases), DT_UINTEGER, NoValidation},
		{"nickserv", "accessmax", "0", new ValueContainerUInt(&conf->NSAccessMax), DT_UINTEGER, ValidateNickServ},
		{"nickserv", "enforceruser", "", new ValueContainerString(&conf->NSEnforcerUser), DT_STRING, ValidateNickServ},
		{"nickserv", "enforcerhost", "", new ValueContainerString(&conf->NSEnforcerHost), DT_STRING, ValidateNickServ},
		{"nickserv", "releasetimeout", "0", new ValueContainerTime(&conf->NSReleaseTimeout), DT_TIME, ValidateNickServ},
		{"nickserv", "allowkillimmed", "no", new ValueContainerBool(&conf->NSAllowKillImmed), DT_BOOLEAN | DT_NORELOAD, NoValidation},
		{"nickserv", "nogroupchange", "no", new ValueContainerBool(&conf->NSNoGroupChange), DT_BOOLEAN, NoValidation},
		{"nickserv", "listmax", "0", new ValueContainerUInt(&conf->NSListMax), DT_UINTEGER, ValidateNickServ},
		{"nickserv", "guestnickprefix", "", new ValueContainerString(&conf->NSGuestNickPrefix), DT_STRING, ValidateNickServ},
		{"nickserv", "secureadmins", "no", new ValueContainerBool(&conf->NSSecureAdmins), DT_BOOLEAN, NoValidation},
		{"nickserv", "strictprivileges", "no", new ValueContainerBool(&conf->NSStrictPrivileges), DT_BOOLEAN, NoValidation},
		{"nickserv", "modeonid", "no", new ValueContainerBool(&conf->NSModeOnID), DT_BOOLEAN, NoValidation},
		{"nickserv", "addaccessonreg", "no", new ValueContainerBool(&conf->NSAddAccessOnReg), DT_BOOLEAN, NoValidation},
		{"nickserv", "ajoinmax", "10", new ValueContainerUInt(&conf->AJoinMax), DT_UINTEGER, NoValidation},
		{"mail", "usemail", "no", new ValueContainerBool(&conf->UseMail), DT_BOOLEAN, ValidateEmailReg},
		{"mail", "sendmailpath", "", new ValueContainerString(&conf->SendMailPath), DT_STRING, ValidateMail},
		{"mail", "sendfrom", "", new ValueContainerString(&conf->SendFrom), DT_STRING, ValidateMail},
		{"mail", "restrict", "no", new ValueContainerBool(&conf->RestrictMail), DT_BOOLEAN, NoValidation},
		{"mail", "delay", "0", new ValueContainerTime(&conf->MailDelay), DT_TIME, NoValidation},
		{"mail", "dontquoteaddresses", "no", new ValueContainerBool(&conf->DontQuoteAddresses), DT_BOOLEAN, NoValidation},
		{"dns", "nameserver", "127.0.0.1", new ValueContainerString(&conf->NameServer), DT_STRING, NoValidation},
		{"dns", "timeout", "5", new ValueContainerTime(&conf->DNSTimeout), DT_TIME, NoValidation},
		{"chanserv", "name", "", new ValueContainerString(&conf->ChanServ), DT_STRING, NoValidation},
		{"chanserv", "defaults", "keeptopic secure securefounder signkick", new ValueContainerString(&CSDefaults), DT_STRING, ValidateChanServ},
		{"chanserv", "maxregistered", "0", new ValueContainerUInt(&conf->CSMaxReg), DT_UINTEGER, ValidateChanServ},
		{"chanserv", "expire", "14d", new ValueContainerTime(&conf->CSExpire), DT_TIME, ValidateChanServ},
		{"chanserv", "suspendexpire", "0", new ValueContainerTime(&conf->CSSuspendExpire), DT_TIME, NoValidation},
		{"chanserv", "forbidexpire", "0", new ValueContainerTime(&conf->CSForbidExpire), DT_TIME, NoValidation},
		{"chanserv", "defbantype", "2", new ValueContainerInt(&conf->CSDefBantype), DT_INTEGER, ValidateChanServ},
		{"chanserv", "accessmax", "0", new ValueContainerUInt(&conf->CSAccessMax), DT_UINTEGER, ValidateChanServ},
		{"chanserv", "autokickmax", "0", new ValueContainerUInt(&conf->CSAutokickMax), DT_UINTEGER, ValidateChanServ},
		{"chanserv", "autokickreason", "User has been banned from the channel", new ValueContainerString(&conf->CSAutokickReason), DT_STRING, ValidateChanServ},
		{"chanserv", "inhabit", "0", new ValueContainerTime(&conf->CSInhabit), DT_TIME, ValidateChanServ},
		{"chanserv", "listmax", "0", new ValueContainerUInt(&conf->CSListMax), DT_UINTEGER, ValidateChanServ},
		{"chanserv", "opersonly", "no", new ValueContainerBool(&conf->CSOpersOnly), DT_BOOLEAN, ValidateChanServ},
		{"memoserv", "name", "", new ValueContainerString(&conf->MemoServ), DT_STRING, NoValidation},
		{"memoserv", "maxmemos", "0", new ValueContainerUInt(&conf->MSMaxMemos), DT_UINTEGER, NoValidation},
		{"memoserv", "senddelay", "0", new ValueContainerTime(&conf->MSSendDelay), DT_TIME, NoValidation},
		{"memoserv", "notifyall", "no", new ValueContainerBool(&conf->MSNotifyAll), DT_BOOLEAN, NoValidation},
		{"memoserv", "memoreceipt", "0", new ValueContainerUInt(&conf->MSMemoReceipt), DT_UINTEGER, NoValidation},
		{"hostserv", "name", "", new ValueContainerString(&conf->HostServ), DT_STRING, NoValidation},
		{"botserv", "name", "", new ValueContainerString(&conf->BotServ), DT_STRING, NoValidation},
		{"botserv", "defaults", "", new ValueContainerString(&BSDefaults), DT_STRING, NoValidation},
		{"botserv", "minusers", "0", new ValueContainerUInt(&conf->BSMinUsers), DT_UINTEGER, ValidateBotServ},
		{"botserv", "badwordsmax", "0", new ValueContainerUInt(&conf->BSBadWordsMax), DT_UINTEGER, ValidateBotServ},
		{"botserv", "keepdata", "0", new ValueContainerTime(&conf->BSKeepData), DT_TIME, ValidateBotServ},
		{"botserv", "smartjoin", "no", new ValueContainerBool(&conf->BSSmartJoin), DT_BOOLEAN, NoValidation},
		{"botserv", "gentlebadwordreason", "no", new ValueContainerBool(&conf->BSGentleBWReason), DT_BOOLEAN, NoValidation},
		{"botserv", "casesensitive", "no", new ValueContainerBool(&conf->BSCaseSensitive), DT_BOOLEAN, NoValidation},
		{"botserv", "fantasycharacter", "!", new ValueContainerString(&conf->BSFantasyCharacter), DT_STRING, NoValidation},
		{"operserv", "name", "", new ValueContainerString(&conf->OperServ), DT_STRING, NoValidation},
		{"operserv", "superadmin", "no", new ValueContainerBool(&conf->SuperAdmin), DT_BOOLEAN, NoValidation},
		{"operserv", "autokillexpiry", "0", new ValueContainerTime(&conf->AutokillExpiry), DT_TIME, ValidateOperServ},
		{"operserv", "chankillexpiry", "0", new ValueContainerTime(&conf->ChankillExpiry), DT_TIME, ValidateOperServ},
		{"operserv", "snlineexpiry", "0", new ValueContainerTime(&conf->SNLineExpiry), DT_TIME, ValidateOperServ},
		{"operserv", "sqlineexpiry", "0", new ValueContainerTime(&conf->SQLineExpiry), DT_TIME, ValidateOperServ},
		{"operserv", "szlineexpiry", "0", new ValueContainerTime(&conf->SZLineExpiry), DT_TIME, ValidateOperServ},
		{"operserv", "akillonadd", "no", new ValueContainerBool(&conf->AkillOnAdd), DT_BOOLEAN, NoValidation},
		{"operserv", "killonsnline", "no", new ValueContainerBool(&conf->KillonSNline), DT_BOOLEAN, NoValidation},
		{"operserv", "killonsqline", "no", new ValueContainerBool(&conf->KillonSQline), DT_BOOLEAN, NoValidation},
		{"operserv", "notifications", "", new ValueContainerString(&OSNotifications), DT_STRING, NoValidation},
		{"operserv", "limitsessions", "no", new ValueContainerBool(&conf->LimitSessions), DT_BOOLEAN, NoValidation},
		{"operserv", "defaultsessionlimit", "0", new ValueContainerUInt(&conf->DefSessionLimit), DT_UINTEGER, NoValidation},
		{"operserv", "maxsessionlimit", "0", new ValueContainerUInt(&conf->MaxSessionLimit), DT_UINTEGER, ValidateOperServ},
		{"operserv", "exceptionexpiry", "0", new ValueContainerTime(&conf->ExceptionExpiry), DT_TIME, ValidateOperServ},
		{"operserv", "sessionlimitexceeded", "", new ValueContainerString(&conf->SessionLimitExceeded), DT_STRING, NoValidation},
		{"operserv", "sessionlimitdetailsloc", "", new ValueContainerString(&conf->SessionLimitDetailsLoc), DT_STRING, NoValidation},
		{"operserv", "maxsessionkill", "0", new ValueContainerUInt(&conf->MaxSessionKill), DT_UINTEGER, NoValidation},
		{"operserv", "sessionautokillexpiry", "0", new ValueContainerTime(&conf->SessionAutoKillExpiry), DT_TIME, NoValidation},
		{"operserv", "addakiller", "no", new ValueContainerBool(&conf->AddAkiller), DT_BOOLEAN, NoValidation},
		{"operserv", "opersonly", "no", new ValueContainerBool(&conf->OSOpersOnly), DT_BOOLEAN, NoValidation},
		{"global", "name", "", new ValueContainerString(&conf->Global), DT_STRING, NoValidation},
		{"global", "globaloncycle", "no", new ValueContainerBool(&conf->GlobalOnCycle), DT_BOOLEAN, NoValidation},
		{"global", "globaloncycledown", "", new ValueContainerString(&conf->GlobalOnCycleMessage), DT_STRING, ValidateGlobalOnCycle},
		{"global", "globaloncycleup", "", new ValueContainerString(&conf->GlobalOnCycleUP), DT_STRING, ValidateGlobalOnCycle},
		{"global", "anonymousglobal", "no", new ValueContainerBool(&conf->AnonymousGlobal), DT_BOOLEAN, NoValidation},
		{"", "", "", NULL, DT_NOTHING, NoValidation}
	};

	/* These tags can occur multiple times, and therefore they have special code to read them
	 * which is different to the code for reading the singular tags listed above. */
	MultiItem MultiItems[] = {
		{"define",
			{"name", "value", ""},
			{"", "", ""},
			{DT_STRING, DT_STRING},
			InitDefine, DoDefine, DoneDefine},
		/* Include must be first so we can pull in the extra files before processing
		 * anything else! */
		{"include",
			{"type", "name", ""},
			{"", "", ""},
			{DT_STRING, DT_STRING},
			InitInclude, DoInclude, DoneInclude},
		{"uplink",
			{"host", "ipv6", "port", "password", ""},
			{"", "no", "0", "", ""},
			{DT_HOSTNAME | DT_NORELOAD, DT_BOOLEAN | DT_NORELOAD, DT_UINTEGER | DT_NORELOAD, DT_NOSPACES | DT_NORELOAD},
			InitUplinks, DoUplink, DoneUplinks},
		{"module",
			{"name", ""},
			{"", ""},
			{DT_STRING},
			InitModules, DoModule, DoneModules},
		{"opertype",
			{"name", "inherits", "commands", "privs", "modes", ""},
			{"", "", "", "", "", ""},
			{DT_STRING, DT_STRING, DT_STRING, DT_STRING, DT_STRING},
			InitOperTypes, DoOperType, DoneOperTypes},
		{"oper",
			{"name", "type", "password", "certfp", ""},
			{"", "", "", "", ""},
			{DT_STRING, DT_STRING, DT_STRING, DT_STRING},
			InitOpers, DoOper, DoneOpers},
		{"service",
			{"nick", "user", "host", "gecos", "modes", ""},
			{"", "", "", "", "", ""},
			{DT_STRING, DT_STRING, DT_STRING, DT_STRING, DT_STRING},
			InitServices, DoServices, DoneServices},
		{"log",
			{"target", "source", "logage", "inhabitlogchannel", "admin", "override", "commands", "servers", "channels", "users", "other", "rawio", "debug", ""},
			{"", "", "7", "yes", "", "", "", "", "", "", "", "no", "no", ""},
			{DT_STRING, DT_STRING, DT_INTEGER, DT_BOOLEAN, DT_STRING, DT_STRING, DT_STRING, DT_STRING, DT_STRING, DT_STRING, DT_STRING, DT_BOOLEAN, DT_BOOLEAN},
			InitLogs, DoLogs, DoneLogs},
		{"command",
			{"service", "name", "command", "permission", ""},
			{"", "", "", "", ""},
			{DT_STRING, DT_STRING, DT_STRING, DT_STRING},
			InitCommands, DoCommands, DoneCommands},
		{"",
			{""},
			{""},
			{0},
			NULL, NULL, NULL}
	};

	this->Values = new Item[sizeof(Items) / sizeof(Item)];
	for (unsigned i = 0; i < sizeof(Items) / sizeof(Item); ++i)
		this->Values[i] = Items[i];

	this->MultiValues = new MultiItem[sizeof(MultiItems) / sizeof(MultiItem)];
	for (unsigned i = 0; i < sizeof(MultiItems) / sizeof(MultiItem); ++i)
		this->MultiValues[i] = MultiItems[i];
}

ConfigItems::~ConfigItems()
{
	for (unsigned i = 0; !this->Values[i].tag.empty(); ++i)
		delete this->Values[i].val;
	delete [] this->Values;
	delete [] this->MultiValues;
}

void ServerConfig::Read()
{
	// These tags MUST occur and must ONLY occur once in the config file
	static const Anope::string Once[] = {"serverinfo", "networkinfo", "options", ""};

	this->LoadConf(services_conf);

	ConfigItems configitems(this);

	/* Read the multiple-tag items (class tags, connect tags, etc)
	 * and call the callbacks associated with them. We have three
	 * callbacks for these, a 'start', 'item' and 'end' callback. */
	for (int Index = 0; !configitems.MultiValues[Index].tag.empty(); ++Index)
	{
		configitems.MultiValues[Index].init_function(this, configitems.MultiValues[Index].tag);
		int number_of_tags = ConfValueEnum(config_data, configitems.MultiValues[Index].tag);
		for (int tagnum = 0; tagnum < number_of_tags; ++tagnum)
		{
			ValueList vl;
			vl.clear();
			for (int valuenum = 0; !configitems.MultiValues[Index].items[valuenum].empty(); ++valuenum)
			{
				int dt = configitems.MultiValues[Index].datatype[valuenum];
				bool allow_newlines =  dt & DT_ALLOW_NEWLINE, allow_wild = dt & DT_ALLOW_WILD, noreload = dt & DT_NORELOAD;
				dt &= ~DT_ALLOW_NEWLINE;
				dt &= ~DT_ALLOW_WILD;
				dt &= ~DT_NORELOAD;

				ConfigDataHash &hash = (noreload && Config ? Config->config_data : this->config_data);
				Anope::string item;
				bool has_value = ConfValue(hash, configitems.MultiValues[Index].tag, configitems.MultiValues[Index].items[valuenum], configitems.MultiValues[Index].items_default[valuenum], tagnum, item, allow_newlines);
				if (defines.count(item) > 0)
					item = defines[item];
				switch (dt)
				{
					case DT_NOSPACES:
					{
						if (has_value)
							vl.push_back(ValueItem(item));
						else
							vl.push_back(ValueItem());
						ValidateNoSpaces(vl[vl.size() - 1].GetValue(), configitems.MultiValues[Index].tag, configitems.MultiValues[Index].items[valuenum]);
						break;
					}
					case DT_HOSTNAME:
					{
						if (has_value)
							vl.push_back(ValueItem(item));
						else
							vl.push_back(ValueItem());
						ValidateHostname(vl[vl.size() - 1].GetValue(), configitems.MultiValues[Index].tag, configitems.MultiValues[Index].items[valuenum]);
						break;
					}
					case DT_IPADDRESS:
					{
						if (has_value)
							vl.push_back(ValueItem(item));
						else
							vl.push_back(ValueItem());
						ValidateIP(vl[vl.size() - 1].GetValue(), configitems.MultiValues[Index].tag, configitems.MultiValues[Index].items[valuenum], allow_wild);
						break;
					}
					case DT_STRING:
					{
						if (has_value)
							vl.push_back(ValueItem(item));
						else
							vl.push_back(ValueItem());
						break;
					}
					case DT_INTEGER:
					case DT_UINTEGER:
					case DT_LUINTEGER:
					{
						int titem = 0;
						if (ConfValueInteger(hash, configitems.MultiValues[Index].tag, configitems.MultiValues[Index].items[valuenum], configitems.MultiValues[Index].items_default[valuenum], tagnum, titem))
							vl.push_back(ValueItem(titem));
						else
							vl.push_back(ValueItem(0));
						break;
					}
					case DT_TIME:
					{
						if (has_value)
						{
#ifdef _WIN32
							long time = static_cast<long>(dotime(item));
#else
							time_t time = dotime(item);
#endif
							vl.push_back(ValueItem(time));
						}
						else
							vl.push_back(ValueItem(0));
						break;
					}
					case DT_BOOLEAN:
					{
						bool titem = ConfValueBool(hash, configitems.MultiValues[Index].tag, configitems.MultiValues[Index].items[valuenum], configitems.MultiValues[Index].items_default[valuenum], tagnum);
						vl.push_back(ValueItem(titem));
					}
				}
			}
			configitems.MultiValues[Index].validation_function(this, configitems.MultiValues[Index].tag, configitems.MultiValues[Index].items, vl, configitems.MultiValues[Index].datatype);
		}
		configitems.MultiValues[Index].finish_function(this, configitems.MultiValues[Index].tag);
	}

	// Read the values of all the tags which occur once or not at all, and call their callbacks.
	for (int Index = 0; !configitems.Values[Index].tag.empty(); ++Index)
	{
		Anope::string item;
		int dt = configitems.Values[Index].datatype;
		bool allow_newlines = dt & DT_ALLOW_NEWLINE, allow_wild = dt & DT_ALLOW_WILD, noreload = dt & DT_NORELOAD;
		dt &= ~DT_ALLOW_NEWLINE;
		dt &= ~DT_ALLOW_WILD;
		dt &= ~DT_NORELOAD;

		ConfigDataHash &hash = (noreload && Config ? Config->config_data : this->config_data);
		ConfValue(hash, configitems.Values[Index].tag, configitems.Values[Index].value, configitems.Values[Index].default_value, 0, item, allow_newlines);
		if (defines.count(item) > 0)
			item = defines[item];
		ValueItem vi(item);

		if (!configitems.Values[Index].validation_function(this, configitems.Values[Index].tag, configitems.Values[Index].value, vi))
			throw ConfigException("One or more values in your configuration file failed to validate. Please see your logfiles for more information.");

		switch (dt)
		{
			case DT_NOSPACES:
			{
				ValueContainerString *vcs = debug_cast<ValueContainerString *>(configitems.Values[Index].val);
				ValidateNoSpaces(vi.GetValue(), configitems.Values[Index].tag, configitems.Values[Index].value);
				vcs->Set(vi.GetValue());
				break;
			}
			case DT_HOSTNAME:
			{
				ValueContainerString *vcs = debug_cast<ValueContainerString *>(configitems.Values[Index].val);
				ValidateHostname(vi.GetValue(), configitems.Values[Index].tag, configitems.Values[Index].value);
				vcs->Set(vi.GetValue());
				break;
			}
			case DT_IPADDRESS:
			{
				ValueContainerString *vcs = debug_cast<ValueContainerString *>(configitems.Values[Index].val);
				ValidateIP(vi.GetValue(), configitems.Values[Index].tag, configitems.Values[Index].value, allow_wild);
				vcs->Set(vi.GetValue());
				break;
			}
			case DT_STRING:
			{
				ValueContainerString *vcs = debug_cast<ValueContainerString *>(configitems.Values[Index].val);
				vcs->Set(vi.GetValue());
				break;
			}
			case DT_INTEGER:
			{
				int val = vi.GetInteger();
				ValueContainerInt *vci = debug_cast<ValueContainerInt *>(configitems.Values[Index].val);
				vci->Set(&val, sizeof(int));
				break;
			}
			case DT_UINTEGER:
			{
				unsigned val = vi.GetInteger();
				ValueContainerUInt *vci = debug_cast<ValueContainerUInt *>(configitems.Values[Index].val);
				vci->Set(&val, sizeof(unsigned));
				break;
			}
			case DT_LUINTEGER:
			{
				unsigned long val = vi.GetInteger();
				ValueContainerLUInt *vci = debug_cast<ValueContainerLUInt *>(configitems.Values[Index].val);
				vci->Set(&val, sizeof(unsigned long));
				break;
			}
			case DT_TIME:
			{
				time_t time = dotime(vi.GetValue());
				ValueContainerTime *vci = debug_cast<ValueContainerTime *>(configitems.Values[Index].val);
				vci->Set(&time, sizeof(time_t));
				break;
			}
			case DT_BOOLEAN:
			{
				bool val = vi.GetBool();
				ValueContainerBool *vcb = debug_cast<ValueContainerBool *>(configitems.Values[Index].val);
				vcb->Set(&val, sizeof(bool));
				break;
			}
			default:
				break;
		}
	}

	Log(LOG_DEBUG) << "End config " << services_conf.GetName();
	for (int Index = 0; !Once[Index].empty(); ++Index)
		CheckOnce(Once[Index]);
	Log() << "Done reading configuration file " << services_conf.GetName();
}

void ServerConfig::LoadConf(ConfigurationFile &file)
{
	Anope::string section, wordbuffer, itemname;
	int linenumber = 0;
	bool in_word = false, in_quote = false, in_ml_comment = false;
	KeyValList sectiondata;
	if (!file.Open())
	{
		throw ConfigException("File " + file.GetName() + " could not be opened.");
	}
	Log(LOG_DEBUG) << "Start to read conf " << file.GetName();
	// Start reading characters...
	while (!file.End())
	{	
		Anope::string line = file.Read();
		++linenumber;
		unsigned c = 0, len = line.length();
		for (; c < len; ++c)
		{
			char ch = line[c];
			if (in_quote)
			{
				if (ch == '"')
					in_quote = in_word = false;
				else
					wordbuffer += ch;
			}
			else if (in_ml_comment)
			{
				if (ch == '*' && c + 1 < len && line[c + 1] == '/')
				{
					in_ml_comment = false;
					++c;
				}
				continue;
			}
			else if (ch == '#' || (ch == '/' && c + 1 < len && line[c + 1] == '/'))
				c = len - 1; // Line comment, ignore the rest of the line (much like this one!)
			else if (ch == '/' && c + 1 < len && line[c + 1] == '*')
			{
				// Multiline (or less than one line) comment
				in_ml_comment = true;
				++c;
				continue;
			}
			else if (ch == '"')
			{
				// Quotes are valid only in the value position
				if (section.empty() || itemname.empty())
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
				if (section.empty())
				{
					file.Close();
					throw ConfigException("Config item outside of section (or stray '='): " + file.GetName() + ":" + stringify(linenumber));
				}
				if (!itemname.empty())
				{
					file.Close();
					throw ConfigException("Stray '=' sign or item without value: " + file.GetName() + ":" + stringify(linenumber));
				}
				if (in_word)
					in_word = false;
				itemname = wordbuffer;
				wordbuffer.clear();
			}
			else if (ch == '{')
			{
				if (!section.empty())
				{
					file.Close();
					throw ConfigException("Section inside another section: " + file.GetName() + ":" + stringify(linenumber));
				}
				if (wordbuffer.empty())
				{
					file.Close();
					throw ConfigException("Section without a name or unexpected '{': " + file.GetName() + ":" + stringify(linenumber));
				}
				if (in_word)
					in_word = false;
				section = wordbuffer;
				wordbuffer.clear();
				continue;
			}
			else if (ch == ' ' || ch == '\r' || ch == '\t')
			{
				// Terminate word
				in_word = false;
			}
			else if (!in_word && ch == ';')
				;
			else if (ch == '}')
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

			if (ch == ';' || ch == '}' || c + 1 == len)
			{
				if (in_quote)
				{
					// Quotes can span multiple lines; all we need to do is go to the next line without clearing things
					wordbuffer += "\n";
					continue;
				}
				in_word = false;
				if (!itemname.empty())
				{
					if (wordbuffer.empty())
					{
						file.Close();
						throw ConfigException("Item without value: " + file.GetName() + ":" + stringify(linenumber));
					}
					Log(LOG_DEBUG) << "ln " << linenumber << " EOL: s='" << section << "' '" << itemname << "' set to '" << wordbuffer << "'";
					sectiondata.push_back(KeyVal(itemname, wordbuffer));
					wordbuffer.clear();
					itemname.clear();
				}

				if (ch == '}')
				{
					if (section.empty())
					{
						file.Close();
						throw ConfigException("Stray '}': " + file.GetName() + ":" + stringify(linenumber));
					}
					this->config_data.insert(std::pair<Anope::string, KeyValList>(section, sectiondata));
					section.clear();
					sectiondata.clear();
				}
			}
		}
	}
	if (in_ml_comment)
	{
		file.Close();
		throw ConfigException("Unterminated multiline comment at end of file: " + file.GetName());
	}
	if (in_quote)
	{
		file.Close();
		throw ConfigException("Unterminated quote at end of file: " + file.GetName());
	}
	if (!itemname.empty() || !wordbuffer.empty())
	{
		file.Close();
		throw ConfigException("Unexpected garbage at end of file: " + file.GetName());
	}
	if (!section.empty())
	{
		file.Close();
		throw ConfigException("Unterminated section at end of file: " + file.GetName());
	}

	file.Close();
}

bool ServerConfig::ConfValue(ConfigDataHash &target, const Anope::string &tag, const Anope::string &var, int index, Anope::string &result, bool allow_linefeeds)
{
	return ConfValue(target, tag, var, "", index, result, allow_linefeeds);
}

bool ServerConfig::ConfValue(ConfigDataHash &target, const Anope::string &tag, const Anope::string &var, const Anope::string &default_value, int index, Anope::string &result, bool allow_linefeeds)
{
	ConfigDataHash::size_type pos = index;
	if (pos < target.count(tag))
	{
		ConfigDataHash::iterator iter = target.find(tag);

		for (int i = 0; i < index; ++i)
			++iter;

		KeyValList::iterator j = iter->second.begin(), jend = iter->second.end();
		for (; j != jend; ++j)
		{
			if (j->first.equals_ci(var))
			{
				if (!allow_linefeeds && j->second.find('\n') != Anope::string::npos)
				{
					Log(LOG_DEBUG) << "Value of <" << tag << ":" << var << "> contains a linefeed, and linefeeds in this value are not permitted -- stripped to spaces.";
					j->second.replace_all_cs("\n", " ");
				}
				else
				{
					result = j->second;
					return true;
				}
			}
		}
		if (!default_value.empty())
		{
			result = default_value;
			return true;
		}
	}
	else if (!pos)
	{
		if (!default_value.empty())
		{
			result = default_value;
			return true;
		}
	}
	return false;
}

bool ServerConfig::ConfValueInteger(ConfigDataHash &target, const Anope::string &tag, const Anope::string &var, int index, int &result)
{
	return ConfValueInteger(target, tag, var, "", index, result);
}

bool ServerConfig::ConfValueInteger(ConfigDataHash &target, const Anope::string &tag, const Anope::string &var, const Anope::string &default_value, int index, int &result)
{
	Anope::string value;
	std::istringstream stream;
	bool r = ConfValue(target, tag, var, default_value, index, value);
	stream.str(value.str());
	if (!(stream >> result))
		return false;
	else
	{
		if (!value.empty())
		{
			if (value.substr(0, 2).equals_ci("0x"))
			{
				char *endptr;

				value.erase(0, 2);
				result = strtol(value.c_str(), &endptr, 16);

				/* No digits found */
				if (endptr == value.c_str())
					return false;
			}
			else
			{
				char denominator = *(value.end() - 1);
				switch (toupper(denominator))
				{
					case 'K':
						// Kilobytes -> bytes
						result = result * 1024;
						break;
					case 'M':
						// Megabytes -> bytes
						result = result * 1048576;
						break;
					case 'G':
						// Gigabytes -> bytes
						result = result * 1073741824;
				}
			}
		}
	}
	return r;
}

bool ServerConfig::ConfValueBool(ConfigDataHash &target, const Anope::string &tag, const Anope::string &var, int index)
{
	return ConfValueBool(target, tag, var, "", index);
}

bool ServerConfig::ConfValueBool(ConfigDataHash &target, const Anope::string &tag, const Anope::string &var, const Anope::string &default_value, int index)
{
	Anope::string result;
	if (!ConfValue(target, tag, var, default_value, index, result))
		return false;

	return result.equals_ci("yes") || result.equals_ci("true") || result.equals_ci("1");
}

int ServerConfig::ConfValueEnum(const ConfigDataHash &target, const Anope::string &tag)
{
	return target.count(tag);
}

int ServerConfig::ConfVarEnum(ConfigDataHash &target, const Anope::string &tag, int index)
{
	ConfigDataHash::size_type pos = index;

	if (pos < target.count(tag))
	{
		ConfigDataHash::const_iterator iter = target.find(tag);

		for (int i = 0; i < index; ++i)
			++iter;

		return iter->second.size();
	}

	return 0;
}

ValueItem::ValueItem() { }

ValueItem::ValueItem(int value) : v("")
{
	std::stringstream n;
	n << value;
	v = n.str();
}

ValueItem::ValueItem(long value) : v("")
{
	std::stringstream n;
	n << value;
	v = n.str();
}

ValueItem::ValueItem(bool value) : v("")
{
	std::stringstream n;
	n << value;
	v = n.str();
}

ValueItem::ValueItem(const Anope::string &value) : v(value) { }

void ValueItem::Set(const Anope::string &value)
{
	v = value;
}

void ValueItem::Set(int value)
{
	std::stringstream n;
	n << value;
	v = n.str();
}

int ValueItem::GetInteger() const
{
	if (v.empty() || !v.is_number_only())
		return 0;
	return convertTo<int>(v);
}

const char *ValueItem::GetString() const
{
	return v.c_str();
}

bool ValueItem::GetBool() const
{
	return GetInteger() || v == "yes" || v == "true";
}



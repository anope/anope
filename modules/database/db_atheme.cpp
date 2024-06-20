/*
 *
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

#include <functional>

#include "module.h"
#include "modules/bs_badwords.h"
#include "modules/bs_kick.h"
#include "modules/cs_entrymsg.h"
#include "modules/cs_mode.h"
#include "modules/hs_request.h"
#include "modules/info.h"
#include "modules/ns_cert.h"
#include "modules/os_forbid.h"
#include "modules/os_oper.h"
#include "modules/os_session.h"
#include "modules/suspend.h"

// Handles reading from an Atheme database row.
class AthemeRow final
{
private:
	// The number of failed reads.
	unsigned error = 0;

	// The underlying token stream.
	spacesepstream stream;

public:
	AthemeRow(const Anope::string &str)
		: stream(str)
	{
	}

	operator bool() const
	{
		return !error;
	}

	// Retrieves the next parameter.
	Anope::string Get()
	{
		Anope::string token;
		if (!stream.GetToken(token))
			error++;
		return token;
	}

	// Retrieves the next parameter as a number.
	template<typename Numeric>
	std::enable_if_t<std::is_arithmetic_v<Numeric>, Numeric> GetNum()
	{
		return Anope::Convert<Numeric>(Get(), 0);
	}

	// Retrieves the entire row.
	Anope::string GetRow()
	{
		return stream.GetString();
	}

	// Retrieves the remaining data in the row.
	Anope::string GetRemaining()
	{
		auto remaining = stream.GetRemaining();
		if (remaining.empty())
			error++;
		return remaining;
	}

	bool LogError(Module *mod)
	{
		Log(mod) << "Malformed database line (expected " << error << " fields): " << GetRow();
		return false;
	}
};

struct ModeData final
{
	char letter;
	Anope::string name;
	Anope::string value;
	bool set;

	ModeData(const Anope::string &n, bool s, const Anope::string &v = "")
		: letter(0)
		, name(n)
		, value(v)
		, set(s)
	{
	}

	ModeData(char l, const Anope::string &v = "")
		: letter(l)
		, value(v)
		, set(true)
	{
	}

	Anope::string str() const
	{
		std::stringstream buf;
		buf << '+' << (name.empty() ? letter : name);
		if (value.empty())
			buf << ' ' << value;
		return buf.str();
	}
};

struct ChannelData final
{
	Anope::unordered_map<AutoKick *> akicks;
	Anope::string bot;
	Anope::string info_adder;
	Anope::string info_message;
	time_t info_ts = 0;
	std::vector<ModeData> mlocks;
	Anope::string suspend_by;
	Anope::string suspend_reason;
	time_t suspend_ts = 0;
};

struct UserData final
{
	bool kill = false;
	Anope::string info_adder;
	Anope::string info_message;
	time_t info_ts = 0;
	Anope::string last_mask;
	Anope::string last_quit;
	Anope::string last_real_mask;
	bool noexpire = false;
	Anope::string suspend_by;
	Anope::string suspend_reason;
	time_t suspend_ts = 0;
	Anope::string vhost;
	Anope::string vhost_creator;
	Anope::map<Anope::string> vhost_nick;
	time_t vhost_ts = 0;
};

namespace
{
	// Whether we can safely import clones.
	bool import_clones = true;
}

class DBAtheme final
	: public Module
{
private:
	ServiceReference<AccessProvider> accessprov;
	PrimitiveExtensibleItem<ChannelData> chandata;
	std::map<Anope::string, char> flags;
	PrimitiveExtensibleItem<UserData> userdata;
	ServiceReference<XLineManager> sglinemgr;
	ServiceReference<XLineManager> snlinemgr;
	ServiceReference<XLineManager> sqlinemgr;

	Anope::map<std::function<bool(DBAtheme*,AthemeRow&)>> rowhandlers = {
		{ "AC",         &DBAtheme::HandleIgnore    },
		{ "AR",         &DBAtheme::HandleIgnore    },
		{ "BE",         &DBAtheme::HandleBE        },
		{ "BLE",        &DBAtheme::HandleIgnore    },
		{ "BOT",        &DBAtheme::HandleBOT       },
		{ "BOT-COUNT",  &DBAtheme::HandleIgnore    },
		{ "BW",         &DBAtheme::HandleBW        },
		{ "CA",         &DBAtheme::HandleCA        },
		{ "CF",         &DBAtheme::HandleIgnore    },
		{ "CFCHAN",     &DBAtheme::HandleIgnore    },
		{ "CFDBV",      &DBAtheme::HandleIgnore    },
		{ "CFMD",       &DBAtheme::HandleIgnore    },
		{ "CFOP",       &DBAtheme::HandleIgnore    },
		{ "CLONES-CD",  &DBAtheme::HandleIgnore    },
		{ "CLONES-CK",  &DBAtheme::HandleIgnore    },
		{ "CLONES-DBV", &DBAtheme::HandleCLONESDBV },
		{ "CLONES-EX",  &DBAtheme::HandleCLONESEX  },
		{ "CLONES-GR",  &DBAtheme::HandleIgnore    },
		{ "CSREQ",      &DBAtheme::HandleIgnore    },
		{ "CSREQ",      &DBAtheme::HandleIgnore    },
		{ "DBV",        &DBAtheme::HandleDBV       },
		{ "GACL",       &DBAtheme::HandleIgnore    },
		{ "GDBV",       &DBAtheme::HandleIgnore    },
		{ "GE",         &DBAtheme::HandleIgnore    },
		{ "GFA",        &DBAtheme::HandleIgnore    },
		{ "GRP",        &DBAtheme::HandleIgnore    },
		{ "GRVER",      &DBAtheme::HandleGRVER     },
		{ "HE",         &DBAtheme::HandleIgnore    },
		{ "HO",         &DBAtheme::HandleIgnore    },
		{ "HR",         &DBAtheme::HandleHR        },
		{ "JM",         &DBAtheme::HandleIgnore    },
		{ "KID",        &DBAtheme::HandleIgnore    },
		{ "KL",         &DBAtheme::HandleKL        },
		{ "LUID",       &DBAtheme::HandleIgnore    },
		{ "MC",         &DBAtheme::HandleMC        },
		{ "MCFP",       &DBAtheme::HandleMCFP      },
		{ "MDA",        &DBAtheme::HandleMDA       },
		{ "MDC",        &DBAtheme::HandleMDC       },
		{ "MDEP",       &DBAtheme::HandleIgnore    },
		{ "MDG",        &DBAtheme::HandleIgnore    },
		{ "MDN",        &DBAtheme::HandleMDN       },
		{ "MDU",        &DBAtheme::HandleMDU       },
		{ "ME",         &DBAtheme::HandleME        },
		{ "MI",         &DBAtheme::HandleMI        },
		{ "MM",         &DBAtheme::HandleMM        },
		{ "MN",         &DBAtheme::HandleMN        },
		{ "MU",         &DBAtheme::HandleMU        },
		{ "NAM",        &DBAtheme::HandleNAM       },
		{ "QID",        &DBAtheme::HandleIgnore    },
		{ "QL",         &DBAtheme::HandleQL        },
		{ "RM",         &DBAtheme::HandleIgnore    },
		{ "RR",         &DBAtheme::HandleIgnore    },
		{ "RW",         &DBAtheme::HandleIgnore    },
		{ "SI",         &DBAtheme::HandleIgnore    },
		{ "SO",         &DBAtheme::HandleSO        },
		{ "TS",         &DBAtheme::HandleIgnore    },
		{ "XID",        &DBAtheme::HandleIgnore    },
		{ "XL",         &DBAtheme::HandleXL        },
	};

	void ApplyAccess(Anope::string &in, char flag, Anope::string &out, std::initializer_list<const char*> privs)
	{
		for (const auto *priv : privs)
		{
			auto pos = in.find(flag);
			if (pos != Anope::string::npos)
			{
				auto privchar = flags.find(priv);
				if (privchar != flags.end())
				{
					out.push_back(privchar->second);
					in.erase(pos, 1);
				}
			}
		}
	}

	void ApplyFlags(Extensible *ext, Anope::string &flags, char flag, const char* extname, bool extend = true)
	{
		auto pos = flags.find(flag);
		auto has_flag = (pos != Anope::string::npos);

		if (has_flag == extend)
			ext->Extend<bool>(extname);
		else
			ext->Shrink<bool>(extname);

		if (has_flag)
			flags.erase(pos, 1);
	}

	void ApplyLocks(ChannelInfo *ci, unsigned locks, const Anope::string &limit, const Anope::string &key, bool set)
	{
		auto *data = chandata.Require(ci);

		// Start off with the standard mode values.
		if (locks & 0x1u)
			data->mlocks.emplace_back("INVITE", set);
		if (locks & 0x2u)
			data->mlocks.emplace_back("KEY", set, key);
		if (locks & 0x4u)
			data->mlocks.emplace_back("LIMIT", set, limit);
		if (locks & 0x8u)
			data->mlocks.emplace_back("MODERATED", set);
		if (locks & 0x10u)
			data->mlocks.emplace_back("NOEXTERNAL", set);
		if (locks & 0x40u)
			data->mlocks.emplace_back("PRIVATE", set);
		if (locks & 0x80u)
			data->mlocks.emplace_back("SECRET", set);
		if (locks & 0x100u)
			data->mlocks.emplace_back("TOPIC", set);
		if (locks & 0x200u)
			data->mlocks.emplace_back("REGISTERED", set);

		// Atheme also supports per-ircd values here (ew).
		if (IRCD->owner->name == "bahamut")
		{
			if (locks & 0x1000u)
				data->mlocks.emplace_back("BLOCKCOLOR", set);
			if (locks & 0x2000u)
				data->mlocks.emplace_back("REGMODERATED", set);
			if (locks & 0x4000u)
				data->mlocks.emplace_back("REGISTEREDONLY", set);
			if (locks & 0x8000u)
				data->mlocks.emplace_back("OPERONLY", set);

			// Anope doesn't recognise the following Bahamut modes currently:
			// - 0x10000u ('A')
			// - 0x20000u ('P')
		}
		else if (IRCD->owner->name == "inspircd")
		{
			if (locks & 0x1000u)
				data->mlocks.emplace_back("BLOCKCOLOR", set);
			if (locks & 0x2000u)
				data->mlocks.emplace_back("REGMODERATED", set);
			if (locks & 0x4000u)
				data->mlocks.emplace_back("REGISTEREDONLY", set);
			if (locks & 0x8000u)
				data->mlocks.emplace_back("OPERONLY", set);
			if (locks & 0x10000u)
				data->mlocks.emplace_back("NONOTICE", set);
			if (locks & 0x20000u)
				data->mlocks.emplace_back("NOKICK", set);
			if (locks & 0x40000u)
				data->mlocks.emplace_back("STRIPCOLOR", set);
			if (locks & 0x80000u)
				data->mlocks.emplace_back("NOKNOCK", set);
			if (locks & 0x100000u)
				data->mlocks.emplace_back("ALLINVITE", set);
			if (locks & 0x200000u)
				data->mlocks.emplace_back("NOCTCP", set);
			if (locks & 0x400000u)
				data->mlocks.emplace_back("AUDITORIUM", set);
			if (locks & 0x800000u)
				data->mlocks.emplace_back("SSL", set);
			if (locks & 0x100000u)
				data->mlocks.emplace_back("NONICK", set);
			if (locks & 0x200000u)
				data->mlocks.emplace_back("CENSOR", set);
			if (locks & 0x400000u)
				data->mlocks.emplace_back("BLOCKCAPS", set);
			if (locks & 0x800000u)
				data->mlocks.emplace_back("PERM", set);
			if (locks & 0x2000000u)
				data->mlocks.emplace_back("DELAYEDJOIN", set);
		}
		else if (IRCD->owner->name == "ngircd")
		{
			if (locks & 0x1000u)
				data->mlocks.emplace_back("REGISTEREDONLY", set);
			if (locks & 0x2000u)
				data->mlocks.emplace_back("OPERONLY", set);
			if (locks & 0x4000u)
				data->mlocks.emplace_back("PERM", set);
			if (locks & 0x8000u)
				data->mlocks.emplace_back("SSL", set);
		}
		else if (IRCD->owner->name == "solanum")
		{
			if (locks & 0x1000u)
				data->mlocks.emplace_back("BLOCKCOLOR", set);
			if (locks & 0x2000u)
				data->mlocks.emplace_back("REGISTEREDONLY", set);
			if (locks & 0x4000u)
				data->mlocks.emplace_back("OPMODERATED", set);
			if (locks & 0x8000u)
				data->mlocks.emplace_back("ALLINVITE", set);
			if (locks & 0x10000u)
				data->mlocks.emplace_back("LBAN", set);
			if (locks & 0x20000u)
				data->mlocks.emplace_back("PERM", set);
			if (locks & 0x40000u)
				data->mlocks.emplace_back("ALLOWFORWARD", set);
			if (locks & 0x80000u)
				data->mlocks.emplace_back("NOFORWARD", set);
			if (locks & 0x100000u)
				data->mlocks.emplace_back("NOCTCP", set);
			if (locks & 0x400000u)
				data->mlocks.emplace_back("SSL", set);
			if (locks & 0x800000u)
				data->mlocks.emplace_back("OPERONLY", set);
			if (locks & 0x1000000u)
				data->mlocks.emplace_back("ADMINONLY", set);
			if (locks & 0x2000000u)
				data->mlocks.emplace_back("NONOTICE", set);
			if (locks & 0x4000000u)
				data->mlocks.emplace_back("PROTECTED", set);
			if (locks & 0x8000000)
				data->mlocks.emplace_back("NOFILTER", set);
			if (locks & 0x10000000U)
				data->mlocks.emplace_back("REGMODERATED", set);
		}
		else if (IRCD->owner->name == "unrealircd")
		{
			if (locks & 0x1000u)
				data->mlocks.emplace_back("BLOCKCOLOR", set);
			if (locks & 0x2000u)
				data->mlocks.emplace_back("REGMODERATED", set);
			if (locks & 0x4000u)
				data->mlocks.emplace_back("REGISTEREDONLY", set);
			if (locks & 0x8000u)
				data->mlocks.emplace_back("OPERONLY", set);
			if (locks & 0x20000u)
				data->mlocks.emplace_back("NOKICK", set);
			if (locks & 0x40000u)
				data->mlocks.emplace_back("STRIPCOLOR", set);
			if (locks & 0x80000u)
				data->mlocks.emplace_back("NOKNOCK", set);
			if (locks & 0x100000u)
				data->mlocks.emplace_back("NOINVITE", set);
			if (locks & 0x200000u)
				data->mlocks.emplace_back("NOCTCP", set);
			if (locks & 0x800000u)
				data->mlocks.emplace_back("SSL", set);
			if (locks & 0x1000000u)
				data->mlocks.emplace_back("NONICK", set);
			if (locks & 0x4000000u)
				data->mlocks.emplace_back("CENSOR", set);
			if (locks & 0x8000000u)
				data->mlocks.emplace_back("PERM", set);
			if (locks & 0x1000000u)
				data->mlocks.emplace_back("NONOTICE", set);
			if (locks & 0x2000000u)
				data->mlocks.emplace_back("DELAYJOIN", set);
		}
		else if (IRCD->owner->name != "ratbox")
		{
			Log(this) << "Unable to import mode locks for " << IRCD->GetProtocolName();
		}
	}

	void ApplyPassword(NickCore *nc, Anope::string &flags, const Anope::string &pass)
	{
		auto pos = flags.find('C');
		if (pos == Anope::string::npos)
		{
			// Password is unencrypted so we can use it.
			Anope::Encrypt(pass, nc->pass);
			return;
		}

		// Atheme supports several password hashing methods. We can only import
		// some of them currently.
		//
		// anope-enc-sha256  Converted to enc_sha256
		// argon2            Converted to enc_argon2
		// base64            Converted to the first encryption algorithm
		// bcrypt            Converted to enc_bcrypt
		// crypt3-des        NO
		// crypt3-md5        Converted to enc_posix
		// crypt3-sha2-256   Converted to enc_posix
		// crypt3-sha2-512   Converted to enc_posix
		// ircservices       Converted to enc_old
		// pbkdf2            NO
		// pbkdf2v2          NO
		// rawmd5            Converted to enc_md5
		// rawsha1           Converted to enc_sha1
		// rawsha2-256       Converted to enc_sha2
		// rawsha2-512       Converted to enc_sha2
		// scrypt            NO
		if (pass.compare(0, 18, "$anope$enc_sha256$", 18) == 0)
		{
			auto sep = pass.find('$', 18);
			Anope::string iv, pass;
			Anope::B64Decode(pass.substr(18, sep - 18), iv);
			Anope::B64Decode(pass.substr(sep + 1), pass);
			nc->pass = "sha256:" + Anope::Hex(pass) + ":" + Anope::Hex(iv);
		}

		else if (pass.compare(0, 9, "$argon2d$", 9) == 0)
			nc->pass = "argon2d:" + pass;

		else if (pass.compare(0, 9, "$argon2i$", 9) == 0)
			nc->pass = "argon2i:" + pass;

		else if (pass.compare(0, 10, "$argon2id$", 10) == 0)
			nc->pass = "argon2id:" + pass;

		else if (pass.compare(0, 8, "$base64$", 8) == 0)
		{
			Anope::string rawpass;
			Anope::B64Decode(pass.substr(8), rawpass);
			Anope::Encrypt(rawpass, nc->pass);
		}

		else if (pass.compare(0, 13, "$ircservices$", 13) == 0)
			nc->pass = "oldmd5:" + pass.substr(13);

		else if (pass.compare(0, 8, "$rawmd5$", 8) == 0)
			nc->pass = "md5:" + pass.substr(8);

		else if (pass.compare(0, 9, "$rawsha1$", 9) == 0)
			nc->pass = "sha1:" + pass.substr(9);

		else if (pass.compare(0, 11, "$rawsha256$", 11) == 0)
			nc->pass = "raw-sha256:" + pass.substr(11);

		else if (pass.compare(0, 11, "$rawsha512$", 11) == 0)
			nc->pass = "raw-sha512:" + pass.substr(11);

		else if (pass.compare(0, 3, "$1$", 3) == 0 || pass.compare(0, 3, "$5", 3) == 0 || pass.compare(0, 3, "$6", 3) == 0)
			nc->pass = "posix:" + pass;

		else if (pass.compare(0, 4, "$2a$", 4) == 0 || pass.compare(0, 4, "$2b$", 4) == 0)
			nc->pass = "bcrypt:" + pass;

		else
		{
			// Generate a new password as we can't use the old one.
			auto maxpasslen = Config->GetModule("nickserv")->Get<unsigned>("maxpasslen", "50");
			Anope::Encrypt(Anope::Random(maxpasslen), nc->pass);
			Log(this) << "Unable to convert the password for " << nc->display << " as Anope does not support the format!";
		}

	}

	bool HandleBE(AthemeRow &row)
	{
		// BE <email <created> <creator> <reason>
		auto email = row.Get();
		auto created = row.GetNum<time_t>();
		auto creator = row.Get();
		auto reason = row.GetRemaining();

		if (!row)
			return row.LogError(this);

		if (!forbid_service)
		{
			Log(this) << "Unable to convert forbidden email " << email << " as os_forbid is not loaded";
			return true;
		}

		auto *forbid = forbid_service->CreateForbid();
		forbid->created = created;
		forbid->creator = creator;
		forbid->mask = email;
		forbid->reason = reason;
		forbid->type = FT_EMAIL;
		forbid_service->AddForbid(forbid);
		return true;
	}

	bool HandleBOT(AthemeRow &row)
	{
		// BOT <nick> <user> <host> <operonly> <created> <real>
		auto nick = row.Get();
		auto user = row.Get();
		auto host = row.Get();
		auto operonly = row.GetNum<unsigned>();
		auto created = row.GetNum<time_t>();
		auto real = row.GetRemaining();

		if (!row)
			return row.LogError(this);

		auto *bi = new BotInfo(nick, user, host, real);
		bi->oper_only = operonly;
		bi->created = created;
		return true;
	}

	bool HandleBW(AthemeRow &row)
	{
		// BW <badword> <added> <creator> <channel> <action>
		auto badword = row.Get();
		/* auto added = */ row.GetNum<time_t>();
		/* auto creator = */ row.Get();
		auto channel = row.Get();
		/* auto action = */ row.Get();

		if (!row)
			return row.LogError(this);

		auto *ci = ChannelInfo::Find(channel);
		if (!ci)
		{
			Log(this) << "Missing ChannelInfo for BW: " << channel;
			return false;
		}

		auto *bw = ci->Require<BadWords>("badwords");
		if (!bw)
		{
			Log(this) << "Unable to import badwords for " << ci->name << " as bs_kick is not loaded";
			return true;
		}

		auto *kd = ci->Require<KickerData>("kickerdata");
		if (kd)
		{
			kd->badwords = true;
			kd->ttb[TTB_BADWORDS] = 0;
		}

		bw->AddBadWord(badword, BW_ANY);
		return true;
	}

	bool HandleCA(AthemeRow &row)
	{
		// CA <channel> <account/mask> <flags> <modifiedtime> <setter>
		auto channel = row.Get();
		auto mask = row.Get();
		auto flags = row.Get();
		auto modifiedtime = row.GetNum<time_t>();
		auto setter = row.Get();

		if (!row)
			return row.LogError(this);

		auto *ci = ChannelInfo::Find(channel);
		if (!ci)
		{
			Log(this) << "Missing ChannelInfo for CA: " << channel;
			return false;
		}

		auto *nc = NickCore::Find(mask);
		if (flags.find('b') != Anope::string::npos)
		{
			auto *data = chandata.Require(ci);
			if (nc)
				data->akicks[mask] = ci->AddAkick(setter, nc, "", modifiedtime, modifiedtime);
			else
				data->akicks[mask] = ci->AddAkick(setter, mask, "", modifiedtime, modifiedtime);
			return true;
		}

		if (!accessprov)
		{
			Log(this) << "Unable to import channel access for " << ci->name << " as cs_flags is not loaded";
			return true;
		}

		Anope::string accessflags;
		ApplyAccess(flags, 'A', accessflags, { "ACCESS_LIST" });
		ApplyAccess(flags, 'a', accessflags, { "AUTOPROTECT", "PROTECT", "PROTECTME" });
		ApplyAccess(flags, 'e', accessflags, { "GETKEY", "NOKICK", "UNBANME" });
		ApplyAccess(flags, 'f', accessflags, { "ACCESS_CHANGE" });
		ApplyAccess(flags, 'F', accessflags, { "FOUNDER" });
		ApplyAccess(flags, 'H', accessflags, { "AUTOHALFOP" });
		ApplyAccess(flags, 'h', accessflags, { "HALFOP", "HALFOPME" });
		ApplyAccess(flags, 'i', accessflags, { "INVITE" });
		ApplyAccess(flags, 'O', accessflags, { "AUTOOP" });
		ApplyAccess(flags, 'o', accessflags, { "OP", "OPME" });
		ApplyAccess(flags, 'q', accessflags, { "AUTOOWNER", "OWNER", "OWNERME" });
		ApplyAccess(flags, 'r', accessflags, { "KICK" });
		ApplyAccess(flags, 's', accessflags, { "SET" });
		ApplyAccess(flags, 't', accessflags, { "TOPIC" });
		ApplyAccess(flags, 'V', accessflags, { "AUTOVOICE" });
		ApplyAccess(flags, 'v', accessflags, { "VOICE", "VOICEME" });
		if (!accessflags.empty())
		{
			auto *access = accessprov->Create();
			access->SetMask(mask, ci);
			access->creator = setter;
			access->description = "Imported from Atheme";
			access->last_seen = modifiedtime;
			access->created = modifiedtime;
			access->AccessUnserialize(accessflags);
			ci->AddAccess(access);
		}

		if (flags != "+")
			Log(this) << "Unable to convert channel access flags " << flags << " for " << mask << " on " << ci->name;

		return true;
	}

	bool HandleCLONESDBV(AthemeRow &row)
	{
		// CLONES-DBV <version>
		auto version = row.GetNum<unsigned>();
		if (version != 3)
		{
			Log(this) << "Clones database is version " << version << " which is not supported!";
			import_clones = false;
		}
		return true;
	}

	bool HandleCLONESEX(AthemeRow &row)
	{
		if (!import_clones)
			return HandleIgnore(row);

		// CLONES-EX <ip> <allowed> <warn> <expires> <reason>
		auto ip = row.Get();
		auto allowed = row.GetNum<unsigned>();
		/* auto warn = */ row.GetNum<unsigned>();
		auto expires = row.GetNum<time_t>();
		auto reason = row.GetRemaining();

		if (!row)
			return row.LogError(this);

		if (!session_service)
		{
			Log(this) << "Unable to import session limit for " << ip << " as os_session is not loaded";
			return true;
		}

		auto *exception = session_service->CreateException();
		exception->mask = ip;
		exception->limit = allowed;
		exception->who = "Unknown";
		exception->time = Anope::CurTime;
		exception->expires = expires;
		exception->reason = reason;
		session_service->AddException(exception);
		return true;
	}

	bool HandleDBV(AthemeRow &row)
	{
		// DBV <version>
		auto version = row.GetNum<unsigned>();
		if (version != 12)
		{
			Log(this) << "Database is version " << version << " which is not supported!";
			return false;
		}
		return true;
	}

	bool HandleGRVER(AthemeRow &row)
	{
		// GRVER <version>
		auto version = row.GetNum<unsigned>();
		if (version != 1)
		{
			Log(this) << "Database grammar is version " << version << " which is not supported!";
			return false;
		}
		return true;
	}

	bool HandleHR(AthemeRow &row)
	{
		// HR <nick> <host> <reqts> <creator>
		auto nick = row.Get();
		auto host = row.Get();
		auto reqts = row.GetNum<time_t>();
		/* auto creator = */ row.Get();

		if (!row)
			return row.LogError(this);

		auto *na = NickAlias::Find(nick);
		if (!na)
		{
			Log(this) << "Missing NickAlias for HR: " << nick;
			return false;
		}

		auto *hr = na->Require<HostRequest>("hostrequest");
		if (!hr)
		{
			Log(this) << "Unable to convert host request for " << na->nick << " as hs_request is not loaded";
			return true;
		}

		hr->nick = na->nick;
		hr->ident.clear();
		hr->host = host;
		hr->time = reqts;
		return true;
	}

	bool HandleKL(AthemeRow &row)
	{
		// KL <id> <user> <host> <duration> <settime> <setby> <reason>
		/* auto id = */ row.GetNum<unsigned>();
		auto user = row.Get();
		auto host = row.Get();
		auto duration = row.GetNum<unsigned>();
		auto settime = row.GetNum<time_t>();
		auto setby = row.Get();
		auto reason = row.GetRemaining();

		if (!row)
			return row.LogError(this);

		if (!sglinemgr)
		{
			Log(this) << "Unable to import K-line on " << user << "@" << host << " as operserv is not loaded";
			return true;
		}

		auto *xl = new XLine(user + "@" + host, setby, settime + duration, reason);
		sglinemgr->AddXLine(xl);
		return true;
	}

	bool HandleIgnore(AthemeRow &row)
	{
		Log(LOG_DEBUG_3) << "Intentionally ignoring Atheme database row: " << row.GetRow();
		return true;
	}

	bool HandleIgnoreMetadata(const Anope::string &target, const Anope::string &key, const Anope::string &value)
	{
		Log(LOG_DEBUG_3) << "Intentionally ignoring Atheme database metadata for " << target << ": " << key << " = " << value;
		return true;
	}

	bool HandleMC(AthemeRow &row)
	{
		// MC <channel> <regtime> <used> <flags> <mlock-on> <mlock-off> <mlock-limit> [<mlock-key>]
		auto channel = row.Get();
		auto regtime = row.GetNum<time_t>();
		/* auto used = */ row.GetNum<time_t>();
		auto flags = row.Get();
		auto mlock_on = row.GetNum<unsigned>();
		auto mlock_off = row.GetNum<unsigned>();
		auto mlock_limit = row.Get();

		if (!row)
			return row.LogError(this);

		auto mlock_key = row.Get(); // May not exist.

		auto *ci = new ChannelInfo(channel);
		ci->time_registered = regtime;

		// No equivalent: elnv
		ApplyFlags(ci, flags, 'h', "CS_NO_EXPIRE");
		ApplyFlags(ci, flags, 'k', "KEEPTOPIC");
		ApplyFlags(ci, flags, 'o', "NOAUTOOP");
		ApplyFlags(ci, flags, 'p', "CS_PRIVATE");
		ApplyFlags(ci, flags, 'r', "RESTRICTED");
		ApplyFlags(ci, flags, 't', "TOPICLOCK");
		ApplyFlags(ci, flags, 'z', "SECUREOPS");

		auto pos = flags.find('a');
		if (pos != Anope::string::npos)
		{
			ci->SetLevel("ACCESS_CHANGE", 0);
			flags.erase(pos, 1);
		}

		pos = flags.find('f');
		if (pos != Anope::string::npos)
		{
			auto *kd = ci->Require<KickerData>("kickerdata");
			if (kd)
			{
				kd->flood = true;
				kd->floodlines = 10;
				kd->floodsecs = 60;
				kd->ttb[TTB_FLOOD] = 0;
				flags.erase(pos, 1);
			}
			else
			{
				Log(this) << "Unable to convert the 'f' flag for " << ci->name << " as bs_kick is not loaded";
			}
		}

		pos = flags.find('g');
		if (pos != Anope::string::npos)
		{
			auto *bi = Config->GetClient("ChanServ");
			if (bi)
			{
				bi->Assign(nullptr, ci);
				flags.erase(pos, 1);
			}
			else
				Log(this) << "Unable to convert the 'g' flag for " << ci->name << " as chanserv is not loaded";
		}

		if (flags != "+")
			Log(this) << "Unable to convert channel flags " << flags << " for " << ci->name;

		ApplyLocks(ci, mlock_on, mlock_limit, mlock_key, true);
		ApplyLocks(ci, mlock_off, mlock_limit, mlock_key, false);
		return true;
	}

	bool HandleMCFP(AthemeRow &row)
	{
		// MCFP <display> <fingerprint>
		auto display = row.Get();
		auto fingerprint = row.Get();

		if (!row)
			return row.LogError(this);

		auto *nc = NickCore::Find(display);
		if (!nc)
		{
			Log(this) << "Missing NickCore for MCFP: " << display;
			return false;
		}

		auto *cl = nc->Require<NSCertList>("certificates");
		if (!cl)
		{
			Log(this) << "Unable to convert certificate for " << nc->display << " as ns_cert is not loaded";
			return true;
		}

		cl->AddCert(fingerprint);
		return true;
	}

	bool HandleMDA(AthemeRow &row)
	{
		// MDA <channel> <account/mask> <key> <value>
		auto channel = row.Get();
		auto mask = row.Get();
		auto key = row.Get();
		auto value = row.GetRemaining();

		if (!row)
			return row.LogError(this);

		auto *ci = ChannelInfo::Find(channel);
		if (!ci)
		{
			Log(this) << "Missing ChannelInfo for MDA: " << channel;
			return false;
		}

		if (key == "reason")
		{
			auto *data = chandata.Require(ci);
			auto akick = data->akicks.find(mask);
			if (akick != data->akicks.end())
				akick->second->reason = value;
		}
		else
			Log(this) << "Unknown channel access metadata " << key << " = " << value;

		return true;
	}

	bool HandleMDC(AthemeRow &row)
	{
		// MDC <channel> <key> <value>
		auto channel = row.Get();
		auto key = row.Get();
		auto value = row.GetRemaining();

		if (!row)
			return row.LogError(this);

		auto *ci = ChannelInfo::Find(channel);
		if (!ci)
		{
			Log(this) << "Missing ChannelInfo for MDC: " << channel;
			return false;
		}

		auto *data = chandata.Require(ci);
		if (key == "private:botserv:bot-assigned")
			data->bot = value;
		else if (key == "private:botserv:bot-handle-fantasy")
			ci->Extend<bool>("BS_FANTASY");
		else if (key == "private:botserv:no-bot")
			ci->Extend<bool>("BS_NOBOT");
		else if (key == "private:channelts")
			return HandleIgnoreMetadata(ci->name, key, value);
		else if (key == "private:close:closer")
			data->suspend_by = value;
		else if (key == "private:close:reason")
			data->suspend_reason = value;
		else if (key == "private:close:timestamp")
			data->suspend_ts = Anope::Convert<time_t>(value, 0);
		else if (key == "private:entrymsg")
		{
			auto *eml = ci->Require<EntryMessageList>("entrymsg");
			if (!eml)
			{
				Log(this) << "Unable to convert entry message for " << ci->name << " as cs_mode is not loaded";
				return true;
			}

			auto *msg = eml->Create();
			msg->chan = ci->name;
			msg->creator = "Unknown";
			msg->message = value;
			msg->when = Anope::CurTime;
			(*eml)->push_back(msg);
		}
		else if (key == "private:klinechan:closer")
			data->suspend_by = value;
		else if (key == "private:klinechan:reason")
			data->suspend_reason = value;
		else if (key == "private:klinechan:timestamp")
			data->suspend_ts = Anope::Convert<time_t>(value, 0);
		else if (key == "private:mark:reason")
			data->info_message = value;
		else if (key == "private:mark:setter")
			data->info_adder = value;
		else if (key == "private:mark:timestamp")
			data->info_ts = Anope::Convert<time_t>(value, 0);
		else if (key == "private:mlockext")
		{
			spacesepstream mlocks(value);
			for (Anope::string mlock; mlocks.GetToken(mlock); )
				data->mlocks.emplace_back(mlock[0], mlock.substr(1));
		}
		else if (key == "private:templates")
			return HandleIgnoreMetadata(ci->name, key, value);
		else if (key == "private:topic:setter")
			ci->last_topic_setter = value;
		else if (key == "private:topic:text")
			ci->last_topic = value;
		else if (key == "private:topic:ts")
			ci->last_topic_time = Anope::Convert<time_t>(value, 0);
		else
			Log(this) << "Unknown channel metadata " << key << " = " << value;

		return true;
	}

	bool HandleMDN(AthemeRow &row)
	{
		// MDN <nick> <key> <value>
		auto nick = row.Get();
		auto key = row.Get();
		auto value = row.GetRemaining();

		if (!row)
			return row.LogError(this);

		if (!forbid_service)
		{
			Log(this) << "Unable to convert forbidden nick " << nick << " metadata as os_forbid is not loaded";
			return true;
		}

		auto *forbid = forbid_service->FindForbidExact(nick, FT_NICK);
		if (!forbid)
		{
			Log(this) << "Missing forbid for MDN: " << nick;
			return false;
		}

		if (key == "private:mark:reason")
			forbid->reason = value;
		else if (key == "private:mark:setter")
			forbid->creator = value;
		else if (key == "private:mark:timestamp")
			forbid->created = Anope::Convert<time_t>(value, 0);
		else
			Log(this) << "Unknown forbidden nick metadata " << key << " = " << value;
		return true;
	}

	bool HandleMDU(AthemeRow &row)
	{
		// MDU <display> <key> <value>
		auto display = row.Get();
		auto key = row.Get();
		auto value = row.GetRemaining();

		if (!row)
			return row.LogError(this);

		auto *nc = NickCore::Find(display);
		if (!nc)
		{
			Log(this) << "Missing NickCore for MDU: " << display;
			return false;
		}

		auto *data = userdata.Require(nc);
		if (key == "private:autojoin")
			return true; // TODO
		else if (key == "private:doenforce")
			data->kill = true;
		else if (key == "private:enforcetime")
		{
			if (!data->kill)
				return true; // Don't apply this.

			auto kill = Config->GetModule("nickserv")->Get<time_t>("kill", "60s");
			auto killquick = Config->GetModule("nickserv")->Get<time_t>("killquick", "20s");
			auto secs = Anope::Convert<time_t>(value, kill);
			if (secs >= kill)
				nc->Extend<bool>("KILLPROTECT");
			else if (secs >= killquick)
				nc->Shrink<bool>("KILL_QUICK");
			else
				nc->Shrink<bool>("KILL_IMMED");
		}
		else if (key == "private:freeze:freezer")
			data->suspend_by = value;
		else if (key == "private:freeze:reason")
			data->suspend_reason = value;
		else if (key == "private:freeze:timestamp")
			data->suspend_ts = Anope::Convert<time_t>(value, 0);
		else if (key == "private:host:actual")
			data->last_real_mask = value;
		else if (key == "private:host:vhost")
			data->last_mask = value;
		else if (key == "private:lastquit:message")
			data->last_quit = value;
		else if (key == "private:loginfail:failnum")
			return HandleIgnoreMetadata(nc->display, key, value);
		else if (key == "private:loginfail:lastfailaddr")
			return HandleIgnoreMetadata(nc->display, key, value);
		else if (key == "private:loginfail:lastfailtime")
			return HandleIgnoreMetadata(nc->display, key, value);
		else if (key == "private:mark:reason")
			data->info_message = value;
		else if (key == "private:mark:setter")
			data->info_adder = value;
		else if (key == "private:mark:timestamp")
			data->info_ts = Anope::Convert<time_t>(value, 0);
		else if (key == "private:usercloak")
			data->vhost = value;
		else if (key == "private:usercloak-assigner")
			data->vhost_creator = value;
		else if (key == "private:usercloak-timestamp")
			data->vhost_ts = Anope::Convert<time_t>(value, 0);
		else if (key.compare(0, 18, "private:usercloak:", 18) == 0)
			data->vhost_nick[key.substr(18)] = value;
		else
			Log(this) << "Unknown account metadata " << key << " = " << value;

		return true;
	}

	bool HandleME(AthemeRow &row)
	{
		// ME <target> <source> <sent> <flags> <text>
		auto target = row.Get();
		auto source = row.Get();
		auto sent = row.GetNum<time_t>();
		auto flags = row.GetNum<unsigned>();
		auto text = row.GetRemaining();

		if (!row)
			return row.LogError(this);

		auto *nc = NickCore::Find(target);
		if (!nc)
		{
			Log(this) << "Missing NickCore for ME: " << source;
			return false;
		}

		auto *m = new Memo();
		m->mi = &nc->memos;
		m->owner = nc->display;
		m->sender = source;
		m->time = sent;
		m->text = text;
		m->unread = flags & 0x1;
		nc->memos.memos->push_back(m);
		return true;
	}

	bool HandleMI(AthemeRow &row)
	{
		// MI <display> <ignored>
		auto display = row.Get();
		auto ignored = row.Get();

		if (!row)
			return row.LogError(this);

		auto *nc = NickCore::Find(display);
		if (!nc)
		{
			Log(this) << "Missing NickCore for MI: " << display;
			return false;
		}

		nc->memos.ignores.push_back(ignored);
		return true;
	}

	bool HandleMM(AthemeRow &row)
	{
		// MM <id> <setterid> <setteraccount> <markedid> <markedaccount> <setts> <num> <message>
		/* auto id = */ row.Get();
		/* auto setterid = */ row.Get();
		auto setteraccount = row.Get();
		/* auto markedid = */ row.Get();
		auto markedaccount = row.Get();
		auto setts = row.GetNum<time_t>();
		/* auto num = */ row.Get();
		auto message = row.GetRemaining();

		if (!row)
			return row.LogError(this);

		auto *nc = NickCore::Find(markedaccount);
		if (!nc)
		{
			Log(this) << "Missing NickCore for MM: " << markedaccount;
			return false;
		}

		auto *oil = nc->Require<OperInfoList>("operinfo");
		if (oil)
		{
			auto *info = oil->Create();
			info->target = nc->display;
			info->info = message;
			info->adder = setteraccount;
			info->created = setts;
			(*oil)->push_back(info);
		}
		else
		{
			Log(this) << "Unable to convert oper info for " << nc->display << " as os_info is not loaded";
		}
		return true;
	}

	bool HandleMN(AthemeRow &row)
	{
		// MU <display> <nick> <regtime> <lastseen>
		auto display = row.Get();
		auto nick = row.Get();
		auto regtime = row.GetNum<time_t>();
		auto lastseen = row.GetNum<time_t>();

		if (!row)
			return row.LogError(this);

		auto *nc = NickCore::Find(display);
		if (!nc)
		{
			Log(this) << "Missing NickCore for MN: " << display;
			return false;
		}

		auto *na = new NickAlias(nick, nc);
		na->time_registered = regtime;
		na->last_seen = lastseen ? regtime : na->time_registered;

		auto *data = userdata.Get(nc);
		if (data)
		{
			if (!data->last_mask.empty())
				na->last_usermask = data->last_mask;

			if (!data->last_quit.empty())
				na->last_quit = data->last_quit;

			if (!data->last_real_mask.empty())
				na->last_realhost = data->last_real_mask;

			if (data->noexpire)
				na->Extend<bool>("NS_NO_EXPIRE");

			auto vhost = data->vhost;
			auto nick_vhost = data->vhost_nick.find(nick);
			if (nick_vhost != data->vhost_nick.end())
				vhost = nick_vhost->second;
			if (!vhost.empty())
				na->SetVHost("", vhost, data->vhost_creator, data->vhost_ts);
		}

		return true;
	}

	bool HandleMU(AthemeRow &row)
	{
		// MU <id> <display> <pass> <email> <regtime> <lastlogin> <flags> <language>
		/* auto id = */ row.Get();
		auto display = row.Get();
		auto pass = row.Get();
		auto email = row.Get();
		auto regtime = row.GetNum<time_t>();
		/* auto lastlogin = */ row.Get();
		auto flags = row.Get();
		auto language = row.Get();

		if (!row)
			return row.LogError(this);

		auto *nc = new NickCore(display);
		nc->email = email;
		nc->time_registered = regtime;
		ApplyPassword(nc, flags, pass);

		// No equivalent: bglmNQrS
		ApplyFlags(nc, flags, 'E', "KILLPROTECT");
		ApplyFlags(nc, flags, 'e', "MEMO_MAIL");
		ApplyFlags(nc, flags, 'n', "NEVEROP");
		ApplyFlags(nc, flags, 'o', "AUTOOP", false);
		ApplyFlags(nc, flags, 'P', "MSG");
		ApplyFlags(nc, flags, 'p', "NS_PRIVATE");
		ApplyFlags(nc, flags, 's', "HIDE_EMAIL");
		ApplyFlags(nc, flags, 'W', "UNCONFIRMED");

		// If an Atheme account was awaiting confirmation but Anope is not
		// configured to use confirmation then autoconfirm it.
		const auto &nsregister = Config->GetModule("ns_register")->Get<const Anope::string>("registration");
		if (nsregister.equals_ci("none"))
			nc->Shrink<bool>("UNCONFIRMED");

		auto pos = flags.find('h');
		if (pos != Anope::string::npos)
		{
			userdata.Require(nc)->noexpire = true;
			flags.erase(pos, 1);
		}


		if (flags != "+")
			Log(this) << "Unable to convert account flags " << flags << " for " << nc->display;

		// No translations yet: bg, cy, da.
		if (language == "de")
			nc->language = "de_DE.UTF-8";
		else if (language == "en")
			nc->language = "en_US.UTF-8";
		else if (language == "es")
			nc->language = "es_ES.UTF-8";
		else if (language == "fr")
			nc->language = "fr_FR.UTF-8";
		else if (language == "ru")
			nc->language = "ru_RU.UTF-8";
		else if (language == "tr")
			nc->language = "tr_TR.UTF-8";
		else if (language != "default")
		{
			Log(this) << "Unable to convert language " << language << " for " << nc->display;
		}

		return true;
	}

	bool HandleNAM(AthemeRow &row)
	{
		// NAM <nick>
		auto nick = row.Get();

		if (!row)
			return row.LogError(this);

		if (!forbid_service)
		{
			Log(this) << "Unable to convert forbidden nick " << nick << " as os_forbid is not loaded";
			return true;
		}

		auto *forbid = forbid_service->CreateForbid();
		forbid->creator = "Unknown";
		forbid->mask = nick;
		forbid->reason = "Unknown";
		forbid->type = FT_NICK;
		forbid_service->AddForbid(forbid);
		return true;
	}

	bool HandleQL(AthemeRow &row)
	{
		// QL <nick> <host> <duration> <settime> <setby> <reason>
		/* auto id = */ row.GetNum<unsigned>();
		auto nick = row.Get();
		auto duration = row.GetNum<unsigned>();
		auto settime = row.GetNum<time_t>();
		auto setby = row.Get();
		auto reason = row.GetRemaining();

		if (!row)
			return row.LogError(this);

		if (!sglinemgr)
		{
			Log(this) << "Unable to import Q-line on " << nick << " as operserv is not loaded";
			return true;
		}

		auto *xl = new XLine(nick, setby, settime + duration, reason);
		sqlinemgr->AddXLine(xl);
		return true;
	}

	bool HandleSO(AthemeRow &row)
	{
		// SO <display> <type> <flags>
		auto display = row.Get();
		auto type = row.Get();
		auto flags = row.Get();

		if (!row)
			return row.LogError(this);

		auto *nc = NickCore::Find(display);
		if (!nc)
		{
			Log(this) << "Missing NickCore for SO: " << display;
			return false;
		}

		auto ot = OperType::Find(type);
		if (!ot)
		{
			// Attempt to convert oper types.
			if (type == "sra")
				ot = OperType::Find("Services Root");
			else if (type == "ircop")
				ot = OperType::Find("Services Operator");
		}

		if (!ot)
		{
			Log(this) << "Unable to convert operator status for " << nc->display << " as there is no equivalent oper type: " << type;
			return true;
		}

		nc->o = new MyOper(nc->display, ot);
		return true;
	}

	bool HandleXL(AthemeRow &row)
	{
		// XL <id> <real> <duration> <settime> <setby> <reason>
		/* auto id = */ row.GetNum<unsigned>();
		auto real = row.Get();
		auto duration = row.GetNum<unsigned>();
		auto settime = row.GetNum<time_t>();
		auto setby = row.Get();
		auto reason = row.GetRemaining();

		if (!row)
			return row.LogError(this);

		if (!sglinemgr)
		{
			Log(this) << "Unable to import X-line on " << real << " as operserv is not loaded";
			return true;
		}

		auto *xl = new XLine(real, setby, settime + duration, reason);
		snlinemgr->AddXLine(xl);
		return true;
	}

public:
	DBAtheme(const Anope::string &modname, const Anope::string &creator)
		: Module(modname, creator, DATABASE | VENDOR)
		, accessprov("AccessProvider", "access/flags")
		, chandata(this, "ATHEME_CHANDATA")
		, userdata(this, "ATHEME_USERDATA")
		, sglinemgr("XLineManager","xlinemanager/sgline")
		, snlinemgr("XLineManager","xlinemanager/snline")
		, sqlinemgr("XLineManager","xlinemanager/sqline")
	{
	}

	void OnReload(Configuration::Conf *conf) override
	{
		flags.clear();
		for (int i = 0; i < Config->CountBlock("privilege"); ++i)
		{
			Configuration::Block *priv = Config->GetBlock("privilege", i);
			const Anope::string &name = priv->Get<const Anope::string>("name");
			const Anope::string &value = priv->Get<const Anope::string>("flag");
			if (!name.empty() && !value.empty())
				flags[name] = value[0];
		}
	}

	EventReturn OnLoadDatabase() override
	{
		const auto dbname = Anope::ExpandData(Config->GetModule(this)->Get<const Anope::string>("database", "atheme.db"));
		std::ifstream fd(dbname.str());
		if (!fd.is_open())
		{
			Log(this) << "Unable to open " << dbname << " for reading!";
			return EVENT_STOP;
		}

		for (Anope::string buf; std::getline(fd, buf.str()); )
		{
			AthemeRow row(buf);

			auto rowtype = row.Get();
			if (!row)
				continue; // Empty row.

			auto rowhandler = rowhandlers.find(rowtype);
			if (rowhandler == rowhandlers.end())
			{
				Log(this) << "Unknown row type: " << row.GetRow();
				continue;
			}

			if (!rowhandler->second(this, row))
				break;
		}

		for (const auto &[_, ci] : *RegisteredChannelList)
		{
			auto *data = chandata.Get(ci);
			if (!data)
				continue;

			if (!data->bot.empty())
			{
				auto *bi = BotInfo::Find(data->bot);
				if (bi)
					bi->Assign(nullptr, ci);
			}

			if (!data->info_message.empty())
			{
				auto *oil = ci->Require<OperInfoList>("operinfo");
				if (oil)
				{
					auto *info = oil->Create();
					info->target = ci->name;
					info->info = data->info_message;
					info->adder = data->info_adder.empty() ? "Unknown" : data->info_adder;
					info->created = data->info_ts;
					(*oil)->push_back(info);
				}
				else
				{
					Log(this) << "Unable to convert oper info for " << ci->name << " as os_info is not loaded";
				}
			}

			if (!data->suspend_reason.empty())
			{
				SuspendInfo si;
				si.by = data->suspend_by.empty() ? "Unknown" : data->suspend_by;
				si.expires = 0;
				si.reason = data->suspend_reason;
				si.what = ci->name;
				si.when = data->suspend_ts;
				ci->Extend("CS_SUSPENDED", si);
			}
		}

		for (const auto &[_, nc] : *NickCoreList)
		{
			auto *data = userdata.Get(nc);
			if (!data)
				continue;

			if (!data->info_message.empty())
			{
				auto *oil = nc->Require<OperInfoList>("operinfo");
				if (oil)
				{
					auto *info = oil->Create();
					info->target = nc->display;
					info->info = data->info_message;
					info->adder = data->info_adder.empty() ? "Unknown" : data->info_adder;
					info->created = data->info_ts;
					(*oil)->push_back(info);
				}
				else
				{
					Log(this) << "Unable to convert oper info for " << nc->display << " as os_info is not loaded";
				}
			}

			if (!data->suspend_reason.empty())
			{
				SuspendInfo si;
				si.by = data->suspend_by.empty() ? "Unknown" : data->suspend_by;
				si.expires = 0;
				si.reason = data->suspend_reason;
				si.what = nc->display;
				si.when = data->suspend_ts;
				nc->Extend("NS_SUSPENDED", si);
			}
		}

		return EVENT_STOP;
	}

	void OnUplinkSync(Server *s) override
	{
		for (auto &[_, ci] : *RegisteredChannelList)
		{
			auto *data = chandata.Get(ci);
			if (!data)
				continue;

			auto *ml = ci->Require<ModeLocks>("modelocks");
			if (!ml)
			{
				Log(this) << "Unable to convert mode locks for " << ci->name << " as cs_mode is not loaded";
				continue;
			}

			for (const auto &mlock : data->mlocks)
			{
				ChannelMode *mh;
				if (mlock.name.empty())
					mh = ModeManager::FindChannelModeByChar(mlock.letter);
				else
					mh = ModeManager::FindChannelModeByName(mlock.name);
				if (!mh)
				{
					Log(this) << "Unable to find mode while importing mode lock on " << ci->name << ": " << mlock.str();
					continue;
				}

				ml->SetMLock(mh, mlock.set, mlock.value, "Unknown");
			}
		}
		Anope::SaveDatabases();
	}
};

MODULE_INIT(DBAtheme)

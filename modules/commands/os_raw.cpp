#include "module.h"
#include <numeric>

class CommandOsRaw : public Command
{
 public:
	CommandOsRaw(Module *creator, const Anope::string &sname = "operserv/raw") : Command(creator, sname, 0, 0)
	{
		this->SetDesc(_("Send a raw command to the uplink server - \037DANGEROUS!\037"));
		this->SetSyntax("\037<command>\037");
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		if (source.GetUser()->super_admin) {
			UplinkSocket::Message() << std::accumulate(params.begin(), params.end(), Anope::string(""));
		}else{
			source.Reply("Must be SuperAdmin!");
		}
	}

	bool OnHelp(CommandSource &source, const Anope::string &subcommand) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Send a raw command to the uplink server."));
		source.Reply(" ");
		source.Reply(_("\037This is a dangerous command. Use only\037\n"
						"\037if you know what you are doing!\037"));

		return true;
	}
};

class ModuleOsRaw : public Module
{
	CommandOsRaw commandosraw;
 public:
	ModuleOsRaw (const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, THIRD), commandosraw(this)
	{
		this->SetAuthor("Zoddo");
		this->SetVersion("1.0.0");
	}
};

MODULE_INIT(ModuleOsRaw)

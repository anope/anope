/*
 * (C) 2003-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "webcpanel.h"

Module *me;
Anope::string provider_name, template_base, page_title;

class ModuleWebCPanel final
	: public Module
{
	ServiceReference<HTTPProvider> provider;
	Panel panel;
	PrimitiveExtensibleItem<Anope::string> id, ip;
	PrimitiveExtensibleItem<time_t> last_login;

	StaticFileServer style_css, logo_png, cubes_png, favicon_ico;

	WebCPanel::Index index;
	WebCPanel::Logout logout;
	WebCPanel::Register _register;
	WebCPanel::Confirm confirm;

	WebCPanel::NickServ::Info nickserv_info;
	WebCPanel::NickServ::Cert nickserv_cert;
	WebCPanel::NickServ::Alist nickserv_alist;
	WebCPanel::NickServ::Confirm nickserv_confirm;

	WebCPanel::ChanServ::Info chanserv_info;
	WebCPanel::ChanServ::Set chanserv_set;
	WebCPanel::ChanServ::Access chanserv_access;
	WebCPanel::ChanServ::Akick chanserv_akick;
	WebCPanel::ChanServ::Modes chanserv_modes;
	WebCPanel::ChanServ::Drop chanserv_drop;

	WebCPanel::MemoServ::Memos memoserv_memos;

	WebCPanel::HostServ::Request hostserv_request;

	WebCPanel::OperServ::Akill operserv_akill;


public:
	ModuleWebCPanel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR),
		panel(this, "webcpanel"),
		id(this, "webcpanel_id"), ip(this, "webcpanel_ip"), last_login(this, "webcpanel_last_login"),
		style_css("style.css", "/static/style.css", "text/css"), logo_png("logo.png", "/static/logo.png", "image/png"), cubes_png("cubes.png", "/static/cubes.png", "image/png"), favicon_ico("favicon.ico", "/favicon.ico", "image/x-icon"),
		index("/"), logout("/logout"), _register("/register"), confirm("/confirm"),
		nickserv_info("NickServ", "/nickserv/info"), nickserv_cert("NickServ", "/nickserv/cert"), nickserv_alist("NickServ", "/nickserv/alist"), nickserv_confirm("NickServ", "/nickserv/confirm"),
		chanserv_info("ChanServ", "/chanserv/info"), chanserv_set("ChanServ", "/chanserv/set"), chanserv_access("ChanServ", "/chanserv/access"), chanserv_akick("ChanServ", "/chanserv/akick"),
		chanserv_modes("ChanServ", "/chanserv/modes"), chanserv_drop("ChanServ", "/chanserv/drop"), memoserv_memos("MemoServ", "/memoserv/memos"), hostserv_request("HostServ", "/hostserv/request"),
		operserv_akill("OperServ", "/operserv/akill")
	{

		me = this;

		Configuration::Block *block = Config->GetModule(this);
		provider_name = block->Get<const Anope::string>("server", "httpd/main");
		template_base = Anope::ExpandData(block->Get<const Anope::string>("template_dir", "webcpanel/templates/default"));
		page_title = block->Get<const Anope::string>("title", "Anope IRC Services");

		provider = ServiceReference<HTTPProvider>("HTTPProvider", provider_name);
		if (!provider)
			throw ModuleException("Unable to find HTTPD provider. Is httpd loaded?");

		provider->RegisterPage(&this->style_css);
		provider->RegisterPage(&this->logo_png);
		provider->RegisterPage(&this->cubes_png);
		provider->RegisterPage(&this->favicon_ico);

		provider->RegisterPage(&this->index);
		provider->RegisterPage(&this->logout);
		provider->RegisterPage(&this->_register);
		provider->RegisterPage(&this->confirm);

		BotInfo *NickServ = Config->GetClient("NickServ");
		if (NickServ)
		{
			Section s;
			s.name = NickServ->nick;

			SubSection ss;
			ss.name = "Information";
			ss.url = "/nickserv/info";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->nickserv_info);

			if (IRCD && IRCD->CanCertFP)
			{
				ss.name = "SSL Certificates";
				ss.url = "/nickserv/cert";
				s.subsections.push_back(ss);
				provider->RegisterPage(&this->nickserv_cert);
			}

			ss.name = "AList";
			ss.url = "/nickserv/alist";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->nickserv_alist);

			ss.name = "Confirm";
			ss.url = "/nickserv/confirm";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->nickserv_confirm);

			panel.sections.push_back(s);
		}

		BotInfo *ChanServ = Config->GetClient("ChanServ");
		if (ChanServ)
		{
			Section s;
			s.name = ChanServ->nick;

			SubSection ss;
			ss.name = "Channels";
			ss.url = "/chanserv/info";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->chanserv_info);

			ss.name = "Settings";
			ss.url = "/chanserv/set";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->chanserv_set);

			ss.name = "Access";
			ss.url = "/chanserv/access";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->chanserv_access);

			ss.name = "Akick";
			ss.url = "/chanserv/akick";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->chanserv_akick);

			ss.name = "Modes";
			ss.url = "/chanserv/modes";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->chanserv_modes);

			ss.name = "Drop";
			ss.url = "/chanserv/drop";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->chanserv_drop);

			panel.sections.push_back(s);
		}

		BotInfo *MemoServ = Config->GetClient("MemoServ");
		if (MemoServ)
		{
			Section s;
			s.name = MemoServ->nick;

			SubSection ss;
			ss.name = "Memos";
			ss.url = "/memoserv/memos";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->memoserv_memos);

			panel.sections.push_back(s);
		}

		BotInfo *HostServ = Config->GetClient("HostServ");
		if (HostServ)
		{
			Section s;
			s.name = HostServ->nick;

			SubSection ss;
			ss.name = "VHost Request";
			ss.url = "/hostserv/request";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->hostserv_request);

			panel.sections.push_back(s);
		}

		BotInfo *OperServ = Config->GetClient("OperServ");
		if (OperServ)
		{
			Section s;
			s.name = OperServ->nick;

			SubSection ss;
			ss.name = "Akill";
			ss.url = "/operserv/akill";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->operserv_akill);

			panel.sections.push_back(s);
		}
	}

	~ModuleWebCPanel() override
	{
		if (provider)
		{
			provider->UnregisterPage(&this->style_css);
			provider->UnregisterPage(&this->logo_png);
			provider->UnregisterPage(&this->cubes_png);
			provider->UnregisterPage(&this->favicon_ico);

			provider->UnregisterPage(&this->index);
			provider->UnregisterPage(&this->logout);
			provider->UnregisterPage(&this->_register);
			provider->UnregisterPage(&this->confirm);

			provider->UnregisterPage(&this->nickserv_info);
			provider->UnregisterPage(&this->nickserv_cert);
			provider->UnregisterPage(&this->nickserv_alist);
			provider->UnregisterPage(&this->nickserv_confirm);

			provider->UnregisterPage(&this->chanserv_info);
			provider->UnregisterPage(&this->chanserv_set);
			provider->UnregisterPage(&this->chanserv_access);
			provider->UnregisterPage(&this->chanserv_akick);
			provider->UnregisterPage(&this->chanserv_modes);
			provider->UnregisterPage(&this->chanserv_drop);

			provider->UnregisterPage(&this->memoserv_memos);

			provider->UnregisterPage(&this->hostserv_request);

			provider->UnregisterPage(&this->operserv_akill);
		}
	}
};

namespace WebPanel
{
	void RunCommand(HTTPClient *client, const Anope::string &user, NickCore *nc, const Anope::string &service, const Anope::string &c, std::vector<Anope::string> &params, TemplateFileServer::Replacements &r, const Anope::string &key)
	{
		ServiceReference<Command> cmd("Command", c);
		if (!cmd)
		{
			r[key] = "Unable to find command " + c;
			return;
		}

		if (params.size() < cmd->min_params)
			return;

		BotInfo *bi = Config->GetClient(service);
		if (!bi)
		{
			if (BotListByNick->empty())
				return;
			bi = BotListByNick->begin()->second; // Pick one...
		}

		struct MyComandReply final
			: CommandReply
		{
			TemplateFileServer::Replacements &re;
			const Anope::string &k;

			MyComandReply(TemplateFileServer::Replacements &_r, const Anope::string &_k) : re(_r), k(_k) { }

			void SendMessage(BotInfo *source, const Anope::string &msg) override
			{
				re[k] = msg;
			}
		}
		my_reply(r, key);

		CommandSource source(user, NULL, nc, &my_reply, bi);
		source.ip = client->GetIP();

		CommandInfo info;
		info.name = c;

		cmd->Run(source, "", info, params);
	}

	void RunCommandWithName(HTTPClient *client, NickCore *nc, const Anope::string &service, const Anope::string &c, const Anope::string &cmdname, std::vector<Anope::string> &params, TemplateFileServer::Replacements &r, const Anope::string &key)
	{
		ServiceReference<Command> cmd("Command", c);
		if (!cmd)
		{
			r[key] = "Unable to find command " + c;
			return;
		}

		BotInfo *bi = Config->GetClient(service);
		if (!bi)
			return;

		CommandInfo *info = bi->GetCommand(cmdname);
		if (!info)
			return;

		struct MyComandReply final
			: CommandReply
		{
			TemplateFileServer::Replacements &re;
			const Anope::string &k;

			MyComandReply(TemplateFileServer::Replacements &_r, const Anope::string &_k) : re(_r), k(_k) { }

			void SendMessage(BotInfo *source, const Anope::string &msg) override
			{
				re[k] = msg;
			}
		}
		my_reply(r, key);

		CommandSource source(nc->display, NULL, nc, &my_reply, bi);
		source.ip = client->GetIP();

		cmd->Run(source, cmdname, *info, params);
	}
}

MODULE_INIT(ModuleWebCPanel)

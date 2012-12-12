/*
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "webcpanel.h"

Module *me;
Anope::string provider_name, template_name, template_base, page_title;
bool use_ssl = false;

class ModuleWebCPanel : public Module
{
	Panel panel;

	StaticFileServer style_css, logo_png, favicon_ico;

	WebCPanel::Index index;
	WebCPanel::Logout logout;
	WebCPanel::Register _register;
	WebCPanel::Confirm confirm;

	WebCPanel::NickServ::Info nickserv_info;
	WebCPanel::NickServ::Cert nickserv_cert;
	WebCPanel::NickServ::Access nickserv_access;
	WebCPanel::NickServ::Alist nickserv_alist;

	WebCPanel::ChanServ::Info chanserv_info;
	WebCPanel::ChanServ::Set chanserv_set;
	WebCPanel::ChanServ::Access chanserv_access;
	WebCPanel::ChanServ::Akick chanserv_akick;

	WebCPanel::MemoServ::Memos memoserv_memos;

	WebCPanel::HostServ::Request hostserv_request;

	WebCPanel::OperServ::Akill operserv_akill;


 public:
	ModuleWebCPanel(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, SUPPORTED),
		panel(this, "webcpanel"),
		style_css("style.css", "/static/style.css", "text/css"), logo_png("logo.png", "/static/logo.png", "image/png"), favicon_ico("favicon.ico", "/favicon.ico", "image/x-icon"),
		index("/"), logout("/logout"), _register("/register"), confirm("/confirm"),
		nickserv_info(Config->NickServ, "/nickserv/info"), nickserv_cert(Config->NickServ, "/nickserv/cert"), nickserv_access(Config->NickServ, "/nickserv/access"), nickserv_alist(Config->NickServ, "/nickserv/alist"),
		chanserv_info(Config->ChanServ, "/chanserv/info"), chanserv_set(Config->ChanServ, "/chanserv/set"), chanserv_access(Config->ChanServ, "/chanserv/access"), chanserv_akick(Config->ChanServ, "/chanserv/akick"),
		memoserv_memos(Config->MemoServ, "/memoserv/memos"), hostserv_request(Config->HostServ, "/hostserv/request"), operserv_akill(Config->OperServ, "/operserv/akill")
	{
		this->SetAuthor("Anope");

		me = this;

		ConfigReader reader;
		provider_name = reader.ReadValue("webcpanel", "server", "httpd/main", 0);
		template_name = reader.ReadValue("webcpanel", "template", "template", 0);
		template_base = Anope::DataDir + "/modules/webcpanel/templates/" + template_name;
		page_title = reader.ReadValue("webcpanel", "title", "Anope IRC Services", 0);
		use_ssl = reader.ReadFlag("webcpanel", "ssl", "no", 0); // This is dumb, is there a better way to do this?

		ServiceReference<HTTPProvider> provider("HTTPProvider", provider_name);
		if (!provider)
			throw ModuleException("Unable to find HTTPD provider. Is m_httpd loaded?");

		provider->RegisterPage(&this->style_css);
		provider->RegisterPage(&this->logo_png);
		provider->RegisterPage(&this->favicon_ico);

		provider->RegisterPage(&this->index);
		provider->RegisterPage(&this->logout);
		provider->RegisterPage(&this->_register);
		provider->RegisterPage(&this->confirm);

		if (Config->NickServ.empty() == false)
		{
			Section s;
			s.name = Config->NickServ;

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

			ss.name = "Access";
			ss.url = "/nickserv/access";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->nickserv_access);

			ss.name = "AList";
			ss.url = "/nickserv/alist";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->nickserv_alist);

			panel.sections.push_back(s);
		}
		if (Config->ChanServ.empty() == false)
		{
			Section s;
			s.name = Config->ChanServ;

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

			panel.sections.push_back(s);
		}

		if (Config->MemoServ.empty() == false)
		{
			Section s;
			s.name = Config->MemoServ;

			SubSection ss;
			ss.name = "Memos";
			ss.url = "/memoserv/memos";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->memoserv_memos);

			panel.sections.push_back(s);
		}

		if (Config->HostServ.empty() == false)
		{
			Section s;
			s.name = Config->HostServ;

			SubSection ss;
			ss.name = "vHost Request";
			ss.url = "/hostserv/request";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->hostserv_request);

			panel.sections.push_back(s);
		}

		if (Config->OperServ.empty() == false)
		{
			Section s;
			s.name = Config->OperServ;

			SubSection ss;
			ss.name = "Akill";
			ss.url = "/operserv/akill";
			s.subsections.push_back(ss);
			provider->RegisterPage(&this->operserv_akill);

			panel.sections.push_back(s);
		}
	}

	~ModuleWebCPanel()
	{
		ServiceReference<HTTPProvider> provider("HTTPProvider", provider_name);
		if (provider)
		{
			provider->UnregisterPage(&this->style_css);
			provider->UnregisterPage(&this->logo_png);
			provider->UnregisterPage(&this->favicon_ico);

			provider->UnregisterPage(&this->index);
			provider->UnregisterPage(&this->logout);
			provider->UnregisterPage(&this->_register);
			provider->UnregisterPage(&this->confirm);

			provider->UnregisterPage(&this->nickserv_info);
			provider->UnregisterPage(&this->nickserv_cert);
			provider->UnregisterPage(&this->nickserv_access);
			provider->UnregisterPage(&this->nickserv_alist);

			provider->UnregisterPage(&this->chanserv_info);
			provider->UnregisterPage(&this->chanserv_set);
			provider->UnregisterPage(&this->chanserv_access);
			provider->UnregisterPage(&this->chanserv_akick);

			provider->UnregisterPage(&this->memoserv_memos);
			
			provider->UnregisterPage(&this->hostserv_request);
		}
	}
};

namespace WebPanel
{
	void RunCommand(const Anope::string &user, NickCore *nc, const Anope::string &service, const Anope::string &c, const std::vector<Anope::string> &params, TemplateFileServer::Replacements &r)
	{
		ServiceReference<Command> cmd("Command", c);
		if (!cmd)
		{
			r["MESSAGES"] = "Unable to find command " + c;
			return;
		}

		BotInfo *bi = BotInfo::Find(service);
		if (!bi)
		{
			if (BotListByNick->empty())
				return;
			bi = BotListByNick->begin()->second; // Pick one...
		}

		struct MyComandReply : CommandReply
		{
			TemplateFileServer::Replacements &re;

			MyComandReply(TemplateFileServer::Replacements &_r) : re(_r) { }

			void SendMessage(const BotInfo *source, const Anope::string &msg) anope_override
			{
				re["MESSAGES"] = msg;
			}
		}
		my_reply(r);

		CommandSource source(user, NULL, nc, &my_reply, bi);
		cmd->Execute(source, params);
	}
}

MODULE_INIT(ModuleWebCPanel)

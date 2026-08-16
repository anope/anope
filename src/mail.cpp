// Anope IRC Services <https://www.anope.org/>
//
// Copyright (C) 2003-2026 Anope Contributors
//
// Anope is free software. You can use, modify, and/or distribute it under the
// terms of version 2 of the GNU General Public License. See docs/LICENSE.txt
// for the complete terms of this license and docs/AUTHORS.txt for a list of
// contributors.
//
// Based on the original code of Epona by Lara
// Based on the original code of Services by Andy Church
//
// SPDX-License-Identifier: GPL-2.0-only

#include <cerrno>

#include "anope.h"
#include "services.h"
#include "mail.h"
#include "config.h"
#include "language.h"

Mail::Message::Message(const Anope::string &sf, const Anope::string &mailto, const Anope::string &a, const Anope::string &s, const Anope::string &m, const Mail::HeaderMap& hm)
	: Thread()
	, sendmail_path(Config->GetBlock("mail").Get<const Anope::string>("sendmailpath", "/usr/sbin/sendmail -it"))
	, send_from(sf)
	, mail_to(mailto)
	, addr(a)
	, subject(s)
	, message(m)
	, headers(hm)
	, dont_quote_addresses(Config->GetBlock("mail").Get<bool>("dontquoteaddresses"))
	, eol(Config->GetBlock("mail").Get<bool>("dontincludecr") ? "\n" : "\r\n")
{
	// These are a safe UTF-8 default if nothing else has been specified.
	this->headers.emplace("Content-Transfer-Encoding", "8bit");
	this->headers.emplace("Content-Type", "text/plain; charset=UTF-8");
}

Mail::Message::~Message()
{
	if (error.empty())
		Log(LOG_NORMAL, "mail") << "Successfully delivered mail for " << mail_to << " (" << addr << ")";
	else
		Log(LOG_NORMAL, "mail") << "Error delivering mail for " << mail_to << " (" << addr << "): " << error;
}

void Mail::Message::Run()
{
	errno = 0;
	auto *pipe = popen(sendmail_path.c_str(), "w");
	if (!pipe)
	{
		error = strerror(errno);
		SetExitState();
		return;
	}

	fprintf(pipe, "From: %s%s", send_from.c_str(), eol.c_str());
	if (this->dont_quote_addresses)
		fprintf(pipe, "To: %s <%s>%s", mail_to.c_str(), addr.c_str(), eol.c_str());
	else
		fprintf(pipe, "To: \"%s\" <%s>%s", mail_to.replace_all_cs("\\", "\\\\").c_str(), addr.c_str(), eol.c_str());
	fprintf(pipe, "Subject: %s%s", subject.c_str(), eol.c_str());
	for (const auto &[hname, hvalue] : this->headers)
		fprintf(pipe, "%s: %s%s", hname.c_str(), hvalue.c_str(), eol.c_str());
	fprintf(pipe, "%s", eol.c_str());

	std::stringstream stream(message.str());
	for (Anope::string line; std::getline(stream, line.str()); )
		fprintf(pipe, "%s%s", line.c_str(), eol.c_str());
	fprintf(pipe, "%s", eol.c_str());

	auto result = pclose(pipe);
	if (result > 0)
		error = "Sendmail exited with code " + Anope::ToString(result);

	SetExitState();
}

Mail::Template::Template(const Anope::string &c)
	: config(c)
{
}

void Mail::Template::ParseSubject(const Configuration::Block &conf, Anope::map<Anope::string>& newsubjects,
		const Anope::string &language, const Anope::string &ckey) const
{
	auto newsubject = conf.Get<const Anope::string>(ckey);
	newsubject.trim();
	if (newsubject.empty())
	{
		if (language.empty())
			throw ConfigException(Anope::Format("The mail:%s:%s field must be specified!", this->config.c_str(), ckey.c_str()));
		return; // No translation for the current type.
	}
	newsubjects[language] = newsubject;
}

void Mail::Template::ParseMessages(const Configuration::Block &conf, Anope::multimap<MessagePart> &newmessages,
	const Anope::string &language, const Anope::string &ckey) const
{
	for (const auto &[_, block] : conf.GetBlocks("message"))
	{
		MessagePart message;
		message.content_type = block.Get<const Anope::string>("content_type", "text/plain; charset=UTF-8");

		const auto file = Anope::ExpandConfig(block.Get<const Anope::string>(ckey));
		if (file.empty())
		{
			if (language.empty())
				throw ConfigException(Anope::Format("The mail:%s:message:%s field must be specified!", this->config.c_str(), ckey.c_str()));
			continue; // No translation for the current type.
		}

		std::ifstream ifile(file.str());
		if (!ifile.is_open())
			throw ConfigException(Anope::Format("The %s file specified in the mail:%s:message:%s field is not readable!", file.c_str(), this->config.c_str(), ckey.c_str()));

		std::stringstream buffer;
		buffer << ifile.rdbuf();
		message.body = Anope::string(buffer.str()).trim();

		if (message.body.empty())
			throw ConfigException(Anope::Format("The %s file specified in the mail:%s:message:%s field is empty!", file.c_str(), this->config.c_str(), ckey.c_str()));

		newmessages.emplace(language, std::move(message));
	}
}

void Mail::Template::Reload(const Configuration::Conf &conf)
{
	const auto &mconf = conf.GetBlock("mail");
	if (!mconf.Get<bool>("usemail"))
	{
		this->subjects.clear();
		this->messages.clear();
		return; // Email is disabled.
	}

	const auto &tconf = mconf.GetBlock(this->config);

	Anope::map<Anope::string> newsubjects;
	Anope::multimap<MessagePart> newmessages;

	// These are the defaults for if a user has no language set.
	ParseSubject(tconf, newsubjects, "", "subject");
	ParseMessages(tconf, newmessages, "", "file");

	// These are the per-language translations for users with a language set.
	for (const auto &language : Language::Languages)
	{
		ParseSubject(tconf, newsubjects, language, Anope::Format("subject[%s]", language.c_str()));
		ParseMessages(tconf, newmessages, language, Anope::Format("file[%s]", language.c_str()));
	}

	// Apply the new configuration.
	std::swap(this->subjects, newsubjects);
	std::swap(this->messages, newmessages);
}

namespace
{
	// Retrieves the default language code.
	Anope::string GetLanguage(NickCore *nc)
	{
		if (!nc->language.empty())
		{
			// The user has a language configured, check whether it is currently supported.
			if (std::find(Language::Languages.begin(), Language::Languages.end(), nc->language) != Language::Languages.end())
				return nc->language;
		}

		// Fall back to the default language.
		return Config->DefLanguage;
	}
}

bool Mail::Template::FormatMessage(NickCore* to, const Anope::map<Anope::string> &vars,
	Anope::string &subject, Anope::string &message, Mail::HeaderMap &headers) const
{
	// Create a HTML-safe version of the template vars.
	auto fullvars = vars;
	for (const auto &[name, value] : vars)
	{
		auto& buffer = fullvars[name + ".html"];
		for (auto chr : value)
		{
			switch (chr)
			{
				case '<':
					buffer.append("&lt;");
					break;
				case '>':
					buffer.append("&gt;");
					break;
				case '&':
					buffer.append("&amp;");
					break;
				case '"':
					buffer.append("&quot;");
					break;
				case '\'':
					buffer.append("&apos;");
					break;
				default:
					buffer.push_back(chr);
					break;
			}
		}
	}

	// Get the translated subject.
	auto translated_subject = subjects.find(GetLanguage(to));
	if (translated_subject == subjects.end())
		translated_subject = subjects.find(""); // Fallback to no translation.
	if (translated_subject == subjects.end())
		return false; // No messages to send (should never happen).

	// Get the translated messages.
	auto translated_messages = Anope::equal_range(this->messages, GetLanguage(to));
	if (translated_messages.empty())
		translated_messages = Anope::equal_range(this->messages, ""); // Fallback to no translation.
	if (translated_messages.empty())
		return false; // No messages to send (should never happen).

	// Create the multipart separator for use later. We may not actually use
	// this if the email only has one part.
	const auto separator = Anope::Random(50); // As long as it can be without linewrapping.

	const auto use_multipart = (translated_messages.count() != 1);
	if (use_multipart)
		headers.emplace("Content-Type", "multipart/alternative; boundary=" + separator);

	std::stringstream messagebuf;
	for (const auto& [_, message] : translated_messages)
	{
		if (use_multipart)
		{
			messagebuf << "--" << separator << std::endl
				<< "Content-Transfer-Encoding: 8bit" << std::endl
				<< "Content-Type: " << message.content_type << std::endl
				<< std::endl;
		}
		else
		{
			headers.emplace("Content-Type", message.content_type);
		}

		messagebuf << Anope::Template(message.body, fullvars) << std::endl
			<< std::endl;
	}
	if (use_multipart)
		messagebuf << "--" << separator << "--" << std::endl;

	subject = Anope::Template(translated_subject->second, fullvars);
	message = messagebuf.str();
	return true;
}


bool Mail::Template::Send(User *from, NickCore *to, BotInfo *service, const Anope::map<Anope::string> &vars) const
{
	if (!to || !service)
		return false; // Malformed.

	Anope::string real_subject, real_message;
	Mail::HeaderMap headers;
	if (!FormatMessage(to, vars, real_subject, real_message, headers))
		return false; // Malformed

	return Mail::Send(from, to, service, real_subject, real_message, headers);
}

bool Mail::Template::Send(NickCore *to, const Anope::map<Anope::string> &vars) const
{
	if (!to)
		return false; // Malformed.

	Anope::string real_subject, real_message;
	Mail::HeaderMap headers;
	if (!FormatMessage(to, vars, real_subject, real_message, headers))
		return false; // Malformed

	return Mail::Send(to, real_subject, real_message, headers);
}

bool Mail::Send(User *u, NickCore *nc, BotInfo *service, const Anope::string &subject, const Anope::string &message, const Mail::HeaderMap& hm)
{
	if (!nc || !service || subject.empty() || message.empty())
		return false;

	const auto &b = Config->GetBlock("mail");

	if (!u)
	{
		if (!b.Get<bool>("usemail") || b.Get<const Anope::string>("sendfrom").empty())
			return false;
		else if (nc->email.empty())
			return false;

		nc->lastmail = Anope::CurTime;
		auto *t = new Mail::Message(b.Get<const Anope::string>("sendfrom"), nc->display, nc->email, subject, message, hm);
		t->Start();
		return true;
	}
	else
	{
		if (!b.Get<bool>("usemail") || b.Get<const Anope::string>("sendfrom").empty())
			u->SendMessage(service, _("Services have been configured to not send mail."));
		else if (Anope::CurTime - u->lastmail < b.Get<time_t>("delay", "5m"))
		{
			const auto delay = b.Get<time_t>("delay") - (Anope::CurTime - u->lastmail);
			u->SendMessage(service, _("Please wait \002%s\002 and retry."), Anope::Duration(delay, u->Account()).c_str());
		}
		else if (nc->email.empty())
			u->SendMessage(service, _("Email for \002%s\002 is invalid."), nc->display.c_str());
		else
		{
			u->lastmail = nc->lastmail = Anope::CurTime;
			auto *t = new Mail::Message(b.Get<const Anope::string>("sendfrom"), nc->display, nc->email, subject, message, hm);
			t->Start();
			return true;
		}

		return false;
	}
}

bool Mail::Send(NickCore *nc, const Anope::string &subject, const Anope::string &message, const Mail::HeaderMap& hm)
{
	const auto &b = Config->GetBlock("mail");
	if (!b.Get<bool>("usemail") || b.Get<const Anope::string>("sendfrom").empty() || !nc || nc->email.empty() || subject.empty() || message.empty())
		return false;

	nc->lastmail = Anope::CurTime;
	auto *t = new Mail::Message(b.Get<const Anope::string>("sendfrom"), nc->display, nc->email, subject, message, hm);
	t->Start();

	return true;
}

/**
 * Checks whether we have a valid, common email address.
 * This is NOT entirely RFC compliant, and won't be so, because I said
 * *common* cases. ;) It is very unlikely that email addresses that
 * are really being used will fail the check.
 *
 * @param email Email to Validate
 * @return bool
 */
bool Mail::Validate(const Anope::string &email)
{
	bool has_period = false;

	static char specials[] = {'(', ')', '<', '>', '@', ',', ';', ':', '\\', '\"', '[', ']', ' '};

	if (email.empty())
		return false;
	Anope::string copy = email;

	size_t at = copy.find('@');
	if (at == Anope::string::npos)
		return false;
	Anope::string domain = copy.substr(at + 1);
	copy = copy.substr(0, at);

	/* Don't accept empty copy or domain. */
	if (copy.empty() || domain.empty())
		return false;

	/* Check for forbidden characters in the name */
	for (auto chr : copy)
	{
		if (chr <= 31 || chr >= 127)
			return false;
		for (auto special : specials)
		{
			if (chr == special)
				return false;
		}
	}

	/* Check for forbidden characters in the domain */
	for (unsigned i = 0, end = domain.length(); i < end; ++i)
	{
		if (domain[i] <= 31 || domain[i] >= 127)
			return false;
		for (auto special : specials)
		{
			if (domain[i] == special)
				return false;
		}
		if (domain[i] == '.')
		{
			if (!i || i == end - 1)
				return false;
			has_period = true;
		}
	}

	return has_period;
}

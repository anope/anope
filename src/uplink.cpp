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

#include "uplink.h"
#include "logger.h"
#include "config.h"
#include "protocol.h"
#include "servers.h"

UplinkSocket *UplinkSock = NULL;

class ReconnectTimer final
	: public Timer
{
public:
	ReconnectTimer(time_t wait)
		: Timer(wait)
	{
	}

	void Tick() override
	{
		try
		{
			Uplink::Connect();
		}
		catch (const SocketException &ex)
		{
			Log(LOG_TERMINAL) << "Unable to connect to uplink #" << (Anope::CurrentUplink + 1) << " (" << Config->Uplinks[Anope::CurrentUplink].str() << "): " << ex.GetReason();
		}
	}
};

void Uplink::Connect()
{
	if (Config->Uplinks.empty())
	{
		Log() << "Warning: There are no configured uplinks.";
		return;
	}

	if (++Anope::CurrentUplink >= Config->Uplinks.size())
		Anope::CurrentUplink = 0;

	Configuration::Uplink &u = Config->Uplinks[Anope::CurrentUplink];

	new UplinkSocket();
	if (!Config->GetBlock("serverinfo").Get<const Anope::string>("localhost").empty())
		UplinkSock->Bind(Config->GetBlock("serverinfo").Get<const Anope::string>("localhost"));
	FOREACH_MOD(OnPreServerConnect, ());
	Anope::string ip = Anope::Resolve(u.host, u.protocol);
	Log(LOG_TERMINAL) << "Attempting to connect to uplink #" << (Anope::CurrentUplink + 1) << " " << ip << " (" << u.str() << ")";
	UplinkSock->Connect(ip, u.port);
}

void Uplink::SendInternal(const Anope::map<Anope::string> &tags, const MessageSource &source, const Anope::string &command, const std::vector<Anope::string> &params)
{
	if (!UplinkSock)
	{
		Log(LOG_DEBUG) << "Attempted to send \"" << command << "\" from " << source.GetName() << " with a null uplink socket";
		return;
	}

	Anope::string message;
	if (!IRCD->Format(message, tags, source, command, params))
		return;

	UplinkSock->Write(message);

	Log(LOG_RAWIO) << "Sent " << message;
	if (Anope::ProtocolDebug)
	{
		if (tags.empty())
			Log() << "\tNo tags";
		else
		{
			for (const auto &[tname, tvalue] : tags)
				Log() << "\tTag " << tname << ": " << tvalue;
		}

		if (source.GetSource().empty())
			Log() << "\tNo source";
		else
			Log() << "\tSource: " << source.GetSource();

		Log() << "\tCommand: " << command;

		if (params.empty())
			Log() << "\tNo params";
		else
		{
			for (size_t i = 0; i < params.size(); ++i)
				Log() << "\tParam " << i << ": " << params[i];
		}
	}
}

UplinkSocket::UplinkSocket() : Socket(-1, Config->Uplinks[Anope::CurrentUplink].protocol), ConnectionSocket(), BufferedSocket()
{
	error = false;
	UplinkSock = this;
}

UplinkSocket::~UplinkSocket()
{
	if (!error && !Anope::Quitting)
	{
		this->OnError("");

		std::vector<Anope::string> advice = {
			"You are connecting Anope to an SSL-only port without configuring SSL (or vice versa).",
		};
		if (IRCD)
		{
			advice.push_back(Anope::Format("You are using the wrong protocol module (currently %s for %s).",
				IRCD->owner->name.c_str(), IRCD->GetProtocolName().c_str()));
			IRCD->GetLinkAdvice(advice);
		}

		Log(LOG_TERMINAL) << "Potential causes include:";
		for (const auto &line : advice)
			Log(LOG_TERMINAL) << "* " << line;
	}

	if (IRCD && Servers::GetUplink() && Servers::GetUplink()->IsSynced())
	{
		FOREACH_MOD(OnServerDisconnect, ());

		for (const auto &[_, u] : UserListByNick)
		{
			if (u->server == Me)
			{
				/* Don't use quitmsg here, it may contain information you don't want people to see */
				const auto *reason = Anope::Restarting ? "Restarting" : "Shutting down";
				IRCD->SendQuit(u, reason, Anope::QuitReason);
				BotInfo *bi = BotInfo::Find(u->GetUID());
				if (bi != NULL)
					bi->introduced = false;
			}
		}

		IRCD->SendSquit(Me, Anope::QuitReason);
	}

	for (unsigned i = Me->GetLinks().size(); i > 0; --i)
		if (!Me->GetLinks()[i - 1]->IsJuped())
			Me->GetLinks()[i - 1]->Delete(Me->GetName() + " " + Me->GetLinks()[i - 1]->GetName());

	this->ProcessWrite(); // Write out the last bit
	UplinkSock = NULL;

	Me->Unsync();

	if (Anope::AtTerm())
	{
		if (static_cast<unsigned>(Anope::CurrentUplink + 1) == Config->Uplinks.size())
		{
			Anope::QuitReason = "Unable to connect to any uplink";
			Anope::Quitting = true;
			Anope::ReturnValue = -1;
		}
		else
		{
			new ReconnectTimer(1);
		}
	}
	else if (!Anope::Quitting)
	{
		time_t retry = Config->GetBlock("options").Get<time_t>("retrywait");

		Log() << "Disconnected, retrying in " << retry << " seconds";
		new ReconnectTimer(retry);
	}
}

bool UplinkSocket::ProcessRead()
{
	bool b = BufferedSocket::ProcessRead();
	for (Anope::string buf; !(buf = this->GetLine()).empty();)
	{
		Anope::Process(buf);
		User::QuitUsers();
		Channel::DeleteChannels();
	}
	return b;
}

void UplinkSocket::OnConnect()
{
	Log(LOG_TERMINAL) << "Successfully connected to uplink #" << (Anope::CurrentUplink + 1) << " " << Config->Uplinks[Anope::CurrentUplink].str();
	IRCD->SendConnect();
	FOREACH_MOD(OnServerConnect, ());
}

void UplinkSocket::OnError(const Anope::string &err)
{
	Anope::string what = !this->flags[SF_CONNECTED] ? "Unable to connect to" : "Lost connection from";
	Log(LOG_TERMINAL) << what << " uplink #" << (Anope::CurrentUplink + 1) << " (" << Config->Uplinks[Anope::CurrentUplink].str() << ")" << (!err.empty() ? (": " + err) : "");
	error |= !err.empty();
}

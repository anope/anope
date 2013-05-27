/*
 *
 * (C) 2003-2013 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 *
 */

#include "uplink.h"
#include "logger.h"
#include "config.h"
#include "protocol.h"
#include "servers.h"

UplinkSocket *UplinkSock = NULL;

class ReconnectTimer : public Timer
{
 public:
	ReconnectTimer(int wait) : Timer(wait) { }

	void Tick(time_t)
	{
		try
		{
			Uplink::Connect();
		}
		catch (const SocketException &ex)
		{
			Log(LOG_TERMINAL) << "Unable to connect to uplink #" << (Anope::CurrentUplink + 1) << " (" << Config->Uplinks[Anope::CurrentUplink].host << ":" << Config->Uplinks[Anope::CurrentUplink].port << "): " << ex.GetReason();
		}
	}
};

void Uplink::Connect()
{
	if (static_cast<unsigned>(++Anope::CurrentUplink) >= Config->Uplinks.size())
		Anope::CurrentUplink = 0;

	Configuration::Uplink &u = Config->Uplinks[Anope::CurrentUplink];

	new UplinkSocket();
	if (!Config->GetBlock("serverinfo")->Get<const Anope::string>("localhost").empty())
		UplinkSock->Bind(Config->GetBlock("serverinfo")->Get<const Anope::string>("localhost"));
	FOREACH_MOD(OnPreServerConnect, ());
	Anope::string ip = Anope::Resolve(u.host, u.ipv6 ? AF_INET6 : AF_INET);
	Log(LOG_TERMINAL) << "Attempting to connect to uplink #" << (Anope::CurrentUplink + 1) << " " << u.host << " (" << ip << "), port " << u.port;
	UplinkSock->Connect(ip, u.port);
}


UplinkSocket::UplinkSocket() : Socket(-1, Config->Uplinks[Anope::CurrentUplink].ipv6), ConnectionSocket(), BufferedSocket()
{
	UplinkSock = this;
}

UplinkSocket::~UplinkSocket()
{
	if (IRCD && Servers::GetUplink() && Servers::GetUplink()->IsSynced())
	{
		FOREACH_MOD(OnServerDisconnect, ());

		for (user_map::const_iterator it = UserListByNick.begin(); it != UserListByNick.end(); ++it)
		{
			User *u = it->second;

			if (u->server == Me)
			{
				/* Don't use quitmsg here, it may contain information you don't want people to see */
				IRCD->SendQuit(u, "Shutting down");
				BotInfo* bi = BotInfo::Find(u->nick);
				if (bi != NULL)
					bi->introduced = false;
			}
		}

		IRCD->SendSquit(Me, Anope::QuitReason);

		this->ProcessWrite(); // Write out the last bit
	}

	if (Me)
		for (unsigned i = Me->GetLinks().size(); i > 0; --i)
			if (!Me->GetLinks()[i - 1]->IsJuped())
				Me->GetLinks()[i - 1]->Delete(Me->GetName() + " " + Me->GetLinks()[i - 1]->GetName());

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
		time_t retry = Config->GetBlock("options")->Get<time_t>("retrywait");

		Log() << "Disconnected, retrying in " << retry << " seconds";
		new ReconnectTimer(retry);
	}
}

bool UplinkSocket::ProcessRead()
{
	bool b = BufferedSocket::ProcessRead();
	for (Anope::string buf; (buf = this->GetLine()).empty() == false;)
	{
		Anope::Process(buf);
		User::QuitUsers();
	}
	return b;
}

void UplinkSocket::OnConnect()
{
	Log(LOG_TERMINAL) << "Successfully connected to uplink #" << (Anope::CurrentUplink + 1) << " " << Config->Uplinks[Anope::CurrentUplink].host << ":" << Config->Uplinks[Anope::CurrentUplink].port;
	IRCD->SendConnect();
	FOREACH_MOD(OnServerConnect, ());
}

void UplinkSocket::OnError(const Anope::string &error)
{
	Log(LOG_TERMINAL) << "Unable to connect to uplink #" << (Anope::CurrentUplink + 1) << " (" << Config->Uplinks[Anope::CurrentUplink].host << ":" << Config->Uplinks[Anope::CurrentUplink].port << ")" << (!error.empty() ? (": " + error) : "");
}

UplinkSocket::Message::Message() : server(NULL), user(NULL)
{
}

UplinkSocket::Message::Message(const Server *s) : server(s), user(NULL)
{
}

UplinkSocket::Message::Message(const User *u) : server(NULL), user(u)
{
	if (!u)
		server = Me;
}

UplinkSocket::Message::~Message()
{
	Anope::string message_source = "";

	if (this->server != NULL)
	{
		if (this->server != Me && !this->server->IsJuped())
		{
			Log(LOG_DEBUG) << "Attempted to send \"" << this->buffer.str() << "\" from " << this->server->GetName() << " who is not from me?";
			return;
		}

		message_source = this->server->GetSID();
	}
	else if (this->user != NULL)
	{
		if (this->user->server != Me && !this->user->server->IsJuped())
		{
			Log(LOG_DEBUG) << "Attempted to send \"" << this->buffer.str() << "\" from " << this->user->nick << " who is not from me?";
			return;
		}

		const BotInfo *bi = BotInfo::Find(this->user->nick);
		if (bi != NULL && bi->introduced == false)
		{
			Log(LOG_DEBUG) << "Attempted to send \"" << this->buffer.str() << "\" from " << bi->nick << " when not introduced";
			return;
		}

		message_source = this->user->GetUID();
	}

	if (!UplinkSock)
	{
		if (!message_source.empty())
			Log(LOG_DEBUG) << "Attempted to send \"" << message_source << " " << this->buffer.str() << "\" with UplinkSock NULL";
		else
			Log(LOG_DEBUG) << "Attempted to send \"" << this->buffer.str() << "\" with UplinkSock NULL";
		return;
	}

	if (!message_source.empty())
	{
		UplinkSock->Write(":" + message_source + " " + this->buffer.str());
		Log(LOG_RAWIO) << "Sent: :" << message_source << " " << this->buffer.str();
	}
	else
	{
		UplinkSock->Write(this->buffer.str());
		Log(LOG_RAWIO) << "Sent: " << this->buffer.str();
	}
}

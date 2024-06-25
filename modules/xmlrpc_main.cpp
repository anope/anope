/*
 *
 * (C) 2010-2024 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 */

#include "module.h"
#include "modules/xmlrpc.h"

static Module *me;

class XMLRPCIdentifyRequest final
    : public IdentifyRequest
{
    XMLRPCRequest request;
    HTTPReply repl; /* Request holds a reference to the HTTPReply, because we might exist long enough to invalidate it
                       we'll copy it here then reset the reference before we use it */
    Reference<HTTPClient> client;
    Reference<XMLRPCServiceInterface> xinterface;

public:
    XMLRPCIdentifyRequest(Module *m, XMLRPCRequest &req, HTTPClient *c, XMLRPCServiceInterface *iface, const Anope::string &acc, const Anope::string &pass)
        : IdentifyRequest(m, acc, pass), request(req), repl(request.r), client(c), xinterface(iface) { }

    void OnSuccess() override
    {
        if (!xinterface || !client)
            return;

        request.r = this->repl;

        request.reply("result", "Success");
        request.reply("account", GetAccount());

        xinterface->Reply(request);
        client->SendReply(&request.r);
    }

    void OnFail() override
    {
        if (!xinterface || !client)
            return;

        request.r = this->repl;

        if (!NickAlias::Find(this->GetAccount()))
        {
            request.reply("error", "Account does not exist");
        }
        else
        {
            request.reply("error", "Invalid password");
        }

        xinterface->Reply(request);
        client->SendReply(&request.r);
    }
};

class MyXMLRPCEvent final
    : public XMLRPCEvent
{
public:
    bool Run(XMLRPCServiceInterface *iface, HTTPClient *client, XMLRPCRequest &request) override
    {
        if (request.name == "command")
            this->DoCommand(iface, client, request);
        else if (request.name == "checkAuthentication")
            return this->DoCheckAuthentication(iface, client, request);
        else if (request.name == "stats")
            this->DoStats(iface, client, request);
        else if (request.name == "channel")
            this->DoChannel(iface, client, request);
        else if (request.name == "user")
            this->DoUser(iface, client, request);
        else if (request.name == "opers")
            this->DoOperType(iface, client, request);
        else if (request.name == "notice")
            this->DoNotice(iface, client, request);

        return true;
    }

private:
    void DoCommand(XMLRPCServiceInterface *iface, HTTPClient *client, XMLRPCRequest &request)
    {
        Anope::string service = request.data.size() > 0 ? request.data[0] : "";
        Anope::string user = request.data.size() > 1 ? request.data[1] : "";
        Anope::string command = request.data.size() > 2 ? request.data[2] : "";

        if (service.empty() || user.empty() || command.empty())
            request.reply("error", "Invalid parameters");
        else
        {
            BotInfo *bi = BotInfo::Find(service, true);
            if (!bi)
                request.reply("error", "Invalid service");
            else
            {
                request.reply("result", "Success");

                NickAlias *na = NickAlias::Find(user);

                Anope::string out;

                struct XMLRPCommandReply final
                    : CommandReply
                {
                    Anope::string &str;

                    XMLRPCommandReply(Anope::string &s) : str(s) { }

                    void SendMessage(BotInfo *source, const Anope::string &msg) override
                    {
                        str += msg + "\n";
                    };
                }
                reply(out);

                User *u = User::Find(user, true);
                CommandSource source(user, u, na ? *na->nc : NULL, &reply, bi);
                Command::Run(source, command);

                if (!out.empty())
                    request.reply("return", iface->Sanitize(out));
            }
        }
    }

    static bool DoCheckAuthentication(XMLRPCServiceInterface *iface, HTTPClient *client, XMLRPCRequest &request)
    {
        Anope::string username = request.data.size() > 0 ? request.data[0] : "";
        Anope::string password = request.data.size() > 1 ? request.data[1] : "";

        if (username.empty() || password.empty())
            request.reply("error", "Invalid parameters");
        else
        {
            auto *req = new XMLRPCIdentifyRequest(me, request, client, iface, username, password);
            FOREACH_MOD(OnCheckAuthentication, (NULL, req));
            req->Dispatch();
            return false;
        }

        return true;
    }

    static void DoStats(XMLRPCServiceInterface *iface, HTTPClient *client, XMLRPCRequest &request)
    {
        request.reply("uptime", Anope::ToString(Anope::CurTime - Anope::StartTime));
        request.reply("uplinkname", Me->GetLinks().front()->GetName());
        {
            Anope::string buf;
            for (const auto &capab : Servers::Capab)
                buf += " " + capab;
            if (!buf.empty())
                buf.erase(buf.begin());
            request.reply("uplinkcapab", buf);
        }
        request.reply("usercount", Anope::ToString(UserListByNick.size()));
        request.reply("maxusercount", Anope::ToString(MaxUserCount));
        request.reply("channelcount", Anope::ToString(ChannelList.size()));
    }

    static void DoChannel(XMLRPCServiceInterface *iface, HTTPClient *client, XMLRPCRequest &request)
    {
        if (request.data.empty())
            return;

        Channel *c = Channel::Find(request.data[0]);

        request.reply("name", iface->Sanitize(c ? c->name : request.data[0]));

        if (c)
        {
            request.reply("bancount", Anope::ToString(c->HasMode("BAN")));
            int count = 0;
            for (auto &ban : c->GetModeList("BAN"))
                request.reply("ban" + Anope::ToString(++count), iface->Sanitize(ban));

            request.reply("exceptcount", Anope::ToString(c->HasMode("EXCEPT")));
            count = 0;
            for (auto &except : c->GetModeList("EXCEPT"))
                request.reply("except" + Anope::ToString(++count), iface->Sanitize(except));

            request.reply("invitecount", Anope::ToString(c->HasMode("INVITEOVERRIDE")));
            count = 0;
            for (auto &invite : c->GetModeList("INVITEOVERRIDE"))
                request.reply("invite" + Anope::ToString(++count), iface->Sanitize(invite));

            Anope::string users;
            for (Channel::ChanUserList::const_iterator it = c->users.begin(); it != c->users.end(); ++it)
            {
                ChanUserContainer *uc = it->second;
                users += uc->status.BuildModePrefixList() + uc->user->nick + " ";
            }
            if (!users.empty())
            {
                users.erase(users.length() - 1);
                request.reply("users", iface->Sanitize(users));
            }

            if (!c->topic.empty())
                request.reply("topic", iface->Sanitize(c->topic));

            if (!c->topic_setter.empty())
                request.reply("topicsetter", iface->Sanitize(c->topic_setter));

            request.reply("topictime", Anope::ToString(c->topic_time));
            request.reply("topicts", Anope::ToString(c->topic_ts));
        }
    }

    static void DoUser(XMLRPCServiceInterface *iface, HTTPClient *client, XMLRPCRequest &request)
    {
        if (request.data.empty())
            return;

        User *u = User::Find(request.data[0]);

        request.reply("nick", iface->Sanitize(u ? u->nick : request.data[0]));

        if (u)
        {
            request.reply("ident", iface->Sanitize(u->GetIdent()));
            request.reply("vident", iface->Sanitize(u->GetVIdent()));
            request.reply("host", iface->Sanitize(u->host));
            if (!u->vhost.empty())
                request.reply("vhost", iface->Sanitize(u->vhost));
            if (!u->chost.empty())
                request.reply("chost", iface->Sanitize(u->chost));
            request.reply("ip", u->ip.addr());
            request.reply("timestamp", Anope::ToString(u->timestamp));
            request.reply("signon", Anope::ToString(u->signon));
            if (u->Account())
            {
                request.reply("account", iface->Sanitize(u->Account()->display));
                if (u->Account()->o)
                    request.reply("opertype", iface->Sanitize(u->Account()->o->ot->GetName()));
            }

            Anope::string channels;
            for (User::ChanUserList::const_iterator it = u->chans.begin(); it != u->chans.end(); ++it)
            {
                ChanUserContainer *cc = it->second;
                channels += cc->status.BuildModePrefixList() + cc->chan->name + " ";
            }
            if (!channels.empty())
            {
                channels.erase(channels.length() - 1);
                request.reply("channels", channels);
            }
        }
    }

    static void DoOperType(XMLRPCServiceInterface *iface, HTTPClient *client, XMLRPCRequest &request)
    {
        for (auto *ot : Config->MyOperTypes)
        {
            Anope::string perms;
            for (const auto &priv : ot->GetPrivs())
                perms += " " + priv;
            for (const auto &command : ot->GetCommands())
                perms += " " + command;
            request.reply(ot->GetName(), perms);
        }
    }

    static void DoNotice(XMLRPCServiceInterface *iface, HTTPClient *client, XMLRPCRequest &request)
    {
        Anope::string from = request.data.size() > 0 ? request.data[0] : "";
        Anope::string to = request.data.size() > 1 ? request.data[1] : "";
        Anope::string message = request.data.size() > 2 ? request.data[2] : "";

        BotInfo *bi = BotInfo::Find(from, true);
        User *u = User::Find(to, true);

        if (!bi || !u || message.empty())
            return;

        u->SendMessage(bi, message);

        request.reply("result", "Success");
    }
};

class ModuleXMLRPCMain final
    : public Module
{
    ServiceReference<XMLRPCServiceInterface> xmlrpc;

    MyXMLRPCEvent stats;

public:
    ModuleXMLRPCMain(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, EXTRA | VENDOR), xmlrpc("XMLRPCServiceInterface", "xmlrpc")
    {
        me = this;

        if (!xmlrpc)
            throw ModuleException("Unable to find xmlrpc reference, is xmlrpc loaded?");

        xmlrpc->Register(&stats);
    }

    ~ModuleXMLRPCMain() override
    {
        if (xmlrpc)
            xmlrpc->Unregister(&stats);
    }
};

MODULE_INIT(ModuleXMLRPCMain)

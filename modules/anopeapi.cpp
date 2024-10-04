#include "module.h"
#include "modules/httplib.h"
#include "modules/ssl.h"
#include <algorithm>
#include <iostream>
#include <jwt-cpp/traits/nlohmann-json/defaults.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <unordered_map>

using json = nlohmann::json;

class AccessTokens
{
  std::string issuer, secret;
  int token_life;

public:
  AccessTokens()
  {
    issuer = Config->GetModule("anopeapi")->Get<std::string>("issuer", "AnopeAPI");
    secret = Config->GetModule("anopeapi")->Get<std::string>("secret", "oh-no");
    token_life = Config->GetModule("anopeapi")->Get<int>("token_life", "36000");
  }

  std::string GrantUserToken(const std::string& nickname)
  {
    // create a user token
    auto token = jwt::create()
      .set_issuer(issuer)
      .set_type("JWT")
      .set_issued_now()
      .set_expires_in(std::chrono::seconds{token_life})
      .set_payload_claim("nickname", jwt::claim(nickname))
      .sign(jwt::algorithm::hs256{generateSecret(nickname)});

    return token;
  }

  // token validation
  bool IsTokenValid(const std::string& token, const std::string& nickname)
  {
    try {
      auto verifier =
        jwt::verify()
        .with_issuer(issuer)
        .with_type("JWT")
        .allow_algorithm(jwt::algorithm::hs256{generateSecret(nickname)})
        .with_claim("nickname", jwt::claim(nickname));

      auto decoded_token = jwt::decode(token);
      verifier.verify(decoded_token);

      json payload = decoded_token.get_payload_json();

      return true;

    } catch (const std::exception &e) {
      return false;

    }

    return false;
  }

private:
  std::string generateSecret(const std::string& nickname)
  {
    std::string secret_hash = secret;

    NickAlias* na = NickAlias::Find(nickname);

    if (na && na->nc) {
      std::string user_hash = na->nc->pass.c_str();
      secret_hash += user_hash.substr(user_hash.rfind(':') + 1);
    }

    return secret_hash;
  }
};

AccessTokens atoken;

class TokenProtectedRoute
{
  AccessTokens& access_tokens;

public:
  TokenProtectedRoute(AccessTokens& atoken) : access_tokens(atoken)
  {
  }

  std::function<void(const httplib::Request&, httplib::Response&)> Wrap(std::function<void(const httplib::Request&, httplib::Response&)> routeHandler)
  {
    return [this, routeHandler](const httplib::Request& req, httplib::Response& resp) {
      if (!ValidateAnopeToken(req, resp)) {
        return;
      }

      routeHandler(req, resp);
    };
  }

  void SendUnauthorizedResponse(httplib::Response& resp)
  {
    resp.status = 401;
    resp.set_content(json{
      {"error", "Unauthorized"},
      {"message", "Authentication required or token invalid"}
    }.dump(), "application/json");
  }

  void SendClientErrorResponse(httplib::Response& resp, const std::string& msg = "Request body could not be read properly.")
  {
    resp.status = 400;
    resp.set_content(json{
      {"error", "Bad Request"},
      {"message", msg},
    }.dump(), "application/json");
  }

private:
  bool ValidateAnopeToken(const httplib::Request& req, httplib::Response& resp)
  {
    if (req.has_header("Anope-Token") && req.has_header("Nickname")) {
      std::string token = req.get_header_value("Anope-Token");
      std::string nickname = req.get_header_value("Nickname");

      if (access_tokens.IsTokenValid(token, nickname)) {
        return true;
      } else {
        SendUnauthorizedResponse(resp);
        return false;
      }
    } else {
      SendUnauthorizedResponse(resp);
      return false;
    }
  }

};

class APIIdentifyRequest final : public IdentifyRequest
{
  json request_data;
  httplib::Response* response;

  std::string acc, pass;

public:
  APIIdentifyRequest(Module* o, const json& req, httplib::Response& resp, const std::string& acc, const std::string& pass)
  : IdentifyRequest(o, acc, pass), request_data(req), response(&resp), acc(acc)
  {
  }

  void OnSuccess() override
  {
    response->set_content(json{
      {"token", atoken.GrantUserToken(acc)}
    }.dump(), "application/json");
  }

  void OnFail() override
  {
    response->status = 401;
    response->set_content(json{
      {"error", "Unauthorized"},
      {"message", "Invalid nickname or password."}
    }.dump(), "application/json");
  }

};

class AnopeAPIModule final : public Module
{
  httplib::Server api_server;
  std::thread server_thread;
  TokenProtectedRoute router;

  std::string bind_ip;
  unsigned int port;
  bool allow_registration;
  std::string base_uri;

public:
  AnopeAPIModule(const Anope::string& modname, const Anope::string& creator)
  : Module(modname, creator, EXTRA), router(atoken)
  {
    server_thread = std::thread([this]() { this->StartServer(); });
    server_thread.detach();

    Log(LOG_NORMAL, "module") << "Anope API Module - Loaded and API HTTP server started.";
  }

  ~AnopeAPIModule() override
  { 
    api_server.stop();
  }

  void OnReload(Configuration::Conf* conf) override
  {
    Configuration::Block* block = conf->GetModule(this);

    bind_ip = block->Get<std::string>("bind_ip");
    if (bind_ip.empty())
      throw ConfigException(this->name + "bind_ip cannot be empty.");

    port = block->Get<int>("port", "8118");
    allow_registration = block->Get<bool>("allow_registration", false);
    base_uri = block->Get<std::string>("base_uri", "/api");

  }

  void StartServer()
  {
    // =============================================================
    // API Routes
    // =============================================================

    // route: /auth
    api_server.Post(base_uri + "/auth", [this](const httplib::Request& req, httplib::Response& resp) {
      try {
        json request_data = json::parse(req.body);

        std::string nickname = request_data.value("nickname", "");
        std::string password = request_data.value("password", "");

        NickAlias *na = NickAlias::Find(nickname);

        if (na && na->nc->HasExt("NS_SUSPENDED")) {
          resp.status = 401;
          resp.set_content(json{
            {"error", "Unauthorized"},
            {"message", "Nickname suspended."}
          }.dump(), "application/json");

          return;
        }

        auto *apireq = new APIIdentifyRequest(this, request_data, resp, nickname, password);
        FOREACH_MOD(OnCheckAuthentication, (NULL, apireq));
        apireq->Dispatch();

      } catch(json::exception &e) {
        router.SendClientErrorResponse(resp);

      }
    });

    // route: /cmd
    // notes: requires a token
    api_server.Post(base_uri + "/cmd", router.Wrap([this](const httplib::Request& req, httplib::Response& resp) {
      try {
        json request_data = json::parse(req.body);

        // make sure we have "service" and "cmd" at the very least.
        if (!request_data.contains("service") || !request_data.contains("cmd")) {
          router.SendClientErrorResponse(resp);
          return;
        }

        std::string nickname = req.get_header_value("Nickname");
        std::string service = request_data["service"];
        std::string cmd = request_data["cmd"];
        std::string params;

        if (request_data.contains("params") && request_data["params"].is_array()) {
          params = " " + std::accumulate(request_data["params"].begin(), request_data["params"].end(), std::string(), [](const std::string& a, const std::string& b) {
            return a + (a.empty() ? "" : " ") + b;
          });
        }

        BotInfo *bi = BotInfo::Find(service, true);

        if (!bi) {
          router.SendClientErrorResponse(resp, "Missing or unknown service.");
          return;
        }

        NickAlias* na = NickAlias::Find(nickname);
				Anope::string out;

				struct APICommandReply final
				: CommandReply
				{
					Anope::string& str;

					APICommandReply(Anope::string& s) : str(s) { }

					void SendMessage(BotInfo* source, const Anope::string& msg) override
					{
						str += msg + "\n";
					};
				}
				reply(out);

				User *u = User::Find(nickname, true);
				CommandSource source(nickname, u, na ? *na->nc : NULL, &reply, bi);
				Command::Run(source, cmd + params);

				if (!out.empty()) {
          resp.set_content(json{
            {"response", splitAndTrim(out.c_str())}
          }.dump(), "application/json");
				}

      } catch(json::exception &e) {
        router.SendClientErrorResponse(resp);

      }
    }));

    try {
      if (!api_server.listen(bind_ip, port)) {
        throw std::runtime_error("Failed to bind " + bind_ip + " to port " + std::to_string(port) + ". Another process might be using it.");
      }

      Log(LOG_NORMAL, "module") << "Anope API server listening on " << bind_ip << ":" << port;
    } catch (const std::exception &e) {
      Log(LOG_NORMAL, "module") << "Error starting server: " << e.what();
    }
  }

  // trim removes whitespace's from anope server respones
  std::string trim(const std::string& str)
  {
    const std::string whitespace = "\n\r\t\f\v";
    size_t start = str.find_first_not_of(whitespace);
    size_t end = str.find_last_not_of(whitespace);

    return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
  }

  // we also have to remove STX characters
  std::string removeControlChars(const std::string& str)
  {
    std::string result;
    std::copy_if(str.begin(), str.end(), std::back_inserter(result),
                 [](char c) { return std::isprint(static_cast<unsigned char>(c)); });

    return result;
  }

  // no we split the response by new lines so we can make the vector into
  // a json array.
  std::vector<std::string> splitAndTrim(const std::string& input)
  {
    std::vector<std::string> result;
    std::istringstream stream(input);
    std::string line;

    // split and trim each line
    while (std::getline(stream, line)) {
      std::string trimmedLine = trim(removeControlChars(line));
      if (!trimmedLine.empty()) {
        result.push_back(trimmedLine);
      }
    }

    return result;
  }

};

MODULE_INIT(AnopeAPIModule)

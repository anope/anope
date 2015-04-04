<?php

/**
 * XMLRPC Functions
 *
 * (C) 2003-2015 Anope Team
 * Contact us at team@anope.org
 */
class AnopeXMLRPC
{
    /**
     * The XMLRPC host
     *
     * @var string
     */
    private $host;

    /**
     * Initiate a new AnopeXMLRPC instance
     *
     * @param $host
     */
    public function __construct($host)
    {
        $this->host = $host;
    }

    /**
     * Run an XMLRPC command. Name should be a query name and params an array of parameters, eg:
     * $this->raw("checkAuthentication", ["adam", "qwerty"]);
     * If successful returns back an array of useful information.
     *
     * Note that $params["id"] is reserved for query ID, you may set it to something if you wish.
     * If you do, the same ID will be passed back with the reply from Anope.
     *
     * @param $name
     * @param $params
     * @return array|null
     */
    public function run($name, $params)
    {
        $xmlquery = xmlrpc_encode_request($name, $params);
        $context = stream_context_create(["http" => [
            "method" => "POST",
            "header" => "Content-Type: text/xml",
            "content" => $xmlquery]]);

        $inbuf = file_get_contents($this->host, false, $context);
        $response = xmlrpc_decode($inbuf);

        if ($response) {
            return $response;
        }

        return null;
    }

    /**
     * Do Command on Service as User, eg:
     * $anope->command("ChanServ", "Adam", "REGISTER #adam");
     * Returns an array of information regarding the command execution, if
     * If 'online' is set to yes, then the reply to the command was sent to the user on IRC.
     * If 'online' is set to no, then the reply to the command is in the array member 'return'
     *
     * @param $service
     * @param $user
     * @param $command
     * @return array|null
     */
    public function command($service, $user, $command)
    {
        return $this->run("command", [$service, $user, $command]);
    }

    /**
     * Check an account/nick name and password to see if they are valid
     * Returns the account display name if valid
     *
     * @param $account
     * @param $pass
     * @return string|null
     */
    public function auth($account, $pass)
    {
        $ret = $this->run("checkAuthentication", [$account, $pass]);

        if ($ret && $ret["result"] == "Success") {
            return $ret["account"];
        }

        return null;
    }

    /**
     * Returns an array of misc stats regarding Anope
     *
     * @return array|null
     */
    public function stats()
    {
        return $this->run("stats", null);
    }

    /**
     * Look up data for a channel
     * Returns an array containing channel information, or an array of size one
     * (just containing the name) if the channel does not exist
     *
     * @param $channel
     * @return array|null
     */
    public function channel($channel)
    {
        return $this->run("channel", [$channel]);
    }

    /**
     * Like channel(), but different.
     *
     * @param $user
     * @return array|null
     */
    public function user($user)
    {
        return $this->run("user", [$user]);
    }
}

$anope = new AnopeXMLRPC("http://127.0.0.1:8080/xmlrpc");

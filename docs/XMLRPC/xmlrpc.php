<?php
/* XMLRPC Functions
 *
 * (C) 2003-2014 Anope Team
 * Contact us at team@anope.org
 *
 */

class AnopeXMLRPC
{
    private $Host;

    public function __construct($Host)
    {
        $this->Host = $Host;
    }

    /** Run an XMLRPC command. Name should be a query name and params an array of parameters, eg:
     * $this->RunXMLRPC("checkAuthentication", array("adam", "qwerty"));
     * If successful returns back an array of useful information.
     *
     * Note that $params["id"] is reserved for query ID, you may set it to something if you wish.
     * If you do, the same ID will be passed back with the reply from Anope.
     */
    public function RunXMLRPC($name, $params)
    {
        $xmlquery = xmlrpc_encode_request($name, $params);
        $context = stream_context_create(array("http" => array(
            "method" => "POST",
            "header" => "Content-Type: text/xml",
            "content" => $xmlquery)));

        $inbuf = file_get_contents($this->Host, false, $context);
        $response = xmlrpc_decode($inbuf);

        if (isset($response)) {
            return $response;
        }

        return null;
    }

    /** Do Command on Service as User, eg:
     * $anope->DoCommand("ChanServ", "Adam", "REGISTER #adam");
     * Returns an array of information regarding the command execution, if
     * If 'online' is set to yes, then the reply to the command was sent to the user on IRC.
     * If 'online' is set to no, then the reply to the command is in the array member 'return'
     */
    public function DoCommand($Service, $User, $Command)
    {
        return $this->RunXMLRPC("command", array($Service, $User, $Command));
    }

    /** Check an account/nick name and password to see if they are valid
     * Returns the account display name if valid
     */
    public function CheckAuthentication($Account, $Pass)
    {
        $ret = $this->RunXMLRPC("checkAuthentication", array($Account, $Pass));

        if ($ret && $ret["result"] == "Success") {
            return $ret["account"];
        }

        return null;
    }

    /* Returns an array of misc stats regarding Anope
     */
    public function DoStats()
    {
        return $this->RunXMLRPC("stats", null);
    }

    /* Look up data for a channel
     * Returns an array containing channel information, or an array of size one
     * (just containing the name) if the channel does not exist
     */
    public function DoChannel($Channel)
    {
        return $this->RunXMLRPC("channel", array($Channel));
    }

    /* Like DoChannel(), but different.
     */
    public function DoUser($User)
    {
        return $this->RunXMLRPC("user", array($User));
    }
}

$anopexmlrpc = new AnopeXMLRPC("http://127.0.0.1:8080/xmlrpc");

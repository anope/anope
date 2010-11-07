<?php

/* XMLRPC Functions
 *
 * (C) 2003-2010 Anope Team
 * Contact us at team@anope.org
 *
 */

class AnopeXMLRPC
{
	private $Host, $Bind, $Port, $Username, $Pass;

	private $Socket;

	function __construct($Host, $Port, $Bind = NULL, $Username = NULL, $Pass = NULL)
	{
		$this->Host = $Host;
		$this->Port = $Port;
		$this->Bind = $Bind;
		$this->Username = $Username;
		$this->Pass = $Pass;

		$this->Socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
		if ($Bind && socket_bind($this->Socket, $this->Bind) == false)
			$his->Socket = false;
		if (socket_connect($this->Socket, $this->Host, $this->Port) == false)
			$this->Socket = false;

		if ($Username && $Pass)
		{
			/* Login to XMLRPC, if required */
			$query = array("login", array($Username, $Pass));
			$xmlquery = xmlrpc_encode_request($query);
			socket_write($this->Socket, $xmlquery);
		}
	}

	function __destruct()
	{
		@socket_close($this->Socket);
	}

	/** Run an XMLRPC command. Name should be a query name and params an array of parameters, eg:
	 * $this->RunXMLRPC("checkAuthentication", array("adam", "qwerty"));
	 * If successful returns back an array of useful information.
	 *
	 * Note that $params["id"] is reserved for query ID, you may set it to something if you wish.
	 * If you do, the same ID will be passed back with the reply from Anope.
	 */
	private function RunXMLRPC($name, $params)
	{
		$xmlquery = xmlrpc_encode_request($name, $params);
		socket_write($this->Socket, $xmlquery);

		$inbuf = socket_read($this->Socket, 4096);
		$inxmlrpc = xmlrpc_decode($inbuf);

		if (isset($inxmlrpc[0]))
			return $inxmlrpc[0];
		return NULL;
	}

	/** Do Command on Service as User, eg:
	 * $anope->DoCommand("ChanServ", "Adam", "REGISTER #adam");
	 * Returns an array of information regarding the command execution, if
	 * If 'online' is set to yes, then the reply to the command was sent to the user on IRC.
	 * If 'online' is set to no, then the reply to the command is in the array member 'return'
	 */
	function DoCommand($Service, $User, $Command)
	{
		return $this->RunXMLRPC("command", array($Service, $User, $Command));
	}

	/** Check an account/nick name and password to see if they are valid
	 * Returns the account display name if valid
	 */
	function CheckAuthentication($Account, $Pass)
	{
		$ret = $this->RunXMLRPC("checkAuthentication", array($Account, $Pass));

		if ($ret && $ret["result"] == "Success")
			return $ret["account"];
		return NULL;
	}

	/* Returns an array of misc stats regarding Anope
	 */
	function DoStats()
	{
		return $this->RunXMLRPC("stats", NULL);
	}

	/* Look up data for a channel
	 * Returns an array containing channel information, or an array of size one
	 * (just containing the name) if the channel does not exist
	 */
	function DoChannel($Channel)
	{
		return $this->RunXMLRPC("channel");
	}

	/* Like DoChannel(), but different.
	 */
	function DoUser($User)
	{
		return $this->RunXMLRPC("channel", array($User));
	}
}

?>

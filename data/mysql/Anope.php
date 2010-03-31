<?php
/*
 * (C) 2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 *
 */

/** Object representing a MySQL query
 */
 class MySQLQuery
 {
 	/* Our query */
 	private $Query;
	/* The result */
	private $Result;
 	/* Socket used to connect to MySQL */
 	private $MysqlSock;

	/** Constructor
	 * @param MysqlSock The MySQL socket
	 */
 	function __construct($MysqlSock)
	{
		$this->MysqlSock = $MysqlSock;
	}

	/** Destructor
	 */
	function __destruct()
	{
	}

	/** Execute a query
	 * @return true or false
	 */
	private function Execute()
	{
		$Res = mysql_query($this->Query, $this->MysqlSock);
		$this->Result = array();

		if (!empty($Res))
		{
			while (($Result = @mysql_fetch_assoc($Res)))
			{
				$this->Result[] = $Result;
			}

			return true;
		}
		
		return false;
	}

	/** Get the result for the query
	 * @return The result
	 */
	public function Result()
	{
		return $this->Result;
	}

	/** Execute a query
	 * @param A formatted string
	 * @param ... Args
	 * @return true or false
	 */
	public function Query($String, $P1 = NULL, $P2 = NULL, $P3 = NULL, $P4 = NULL, $P5 = NULL, $P6 = NULL, $P7 = NULL,
						$P8 = NULL, $P9 = NULL, $P10 = NULL, $P11 = NULL, $P12 = NULL, $P13 = NULL, $P14 = NULL)
	{
		$this->Query = sprintf($String, $P1, $P2, $P3, $P4, $P5, $P6, $P7, $P8, $P9, $P10, $P11, $P12, $P13, $P14);
		return $this->Execute();

	}

	/** Escape a string to by MySQL safe
	 * @return A new, MySQL safe string
	 */
	public function Escape($String)
	{
		return mysql_real_escape_string($String, $this->MysqlSock);
	}
 }

/** Main Anope class
 */
 class Anope
 {
 	/* Socket used to connect to MySQL */
 	private $MysqlSock;
	/* True if we were able to connect successfully */
	private $Connected;

	/** Constructor
	 * @param MysqlHost The host of the MySQLd server, port can be included on this too
	 * @param MysqlUser The username to authenticate to MySQL with
	 * @param MysqlPassword The password to authenticate with
	 * @param MysqlDatabase The name of the Anope database
	 */
 	function __construct($MysqlHost, $MysqlUser, $MysqlPassword, $MysqlDatabase)
	{
		$this->Connected = false;
		$this->MysqlSock = @mysql_connect($MysqlHost, $MysqlUser, $MysqlPassword);
		if ($this->MysqlSock)
			$this->Connected = @mysql_select_db($MysqlDatabase, $this->MysqlSock);
	}

	/** Destructor
	 * Closes the connection to the MySQL server
	 */
	function __destruct()
	{
		if ($this->MysqlSock)
			@mysql_close($this->MysqlSock);
	}

	/** Check if we are connected successfully
	 * @return true or false
	 */
	public function Connected()
	{
		return $this->Connected;
	}

	/** Retrieve a new query object
	 * @return A new Query object
	 */
	public function Query()
	{
		return new MySQLQuery($this->MysqlSock);
	}

	/** Anope Functions **/

	/** Execute a command in Anope
	 * For more information read docs/MYSQL
	 * @param Nick The nickname to execute the command from
	 * @param Service The service to execute the command on
	 * @param Command The command to execute
	 */
	public function Command($Nick, $Service, $Command)
	{
		$Query = $this->Query();
		return $Query->Query("INSERT DELAYED INTO `anope_commands` (nick, service, command) VALUES('%s', '%s', '%s')", $Query->Escape($Nick), $Query->Escape($Service), $Query->Escape($Command));
	}

	/** Register a nick
	 * @param Nick The nick to be registered
	 * @param Password The password
	 * @param Email The email address to use, defaults to NULL
	 * @param Returns a message confirming or denying the registration process
	 */
	public function Register($Nick, $Password, $Email = NULL)
	{
		$Query = $this->Query();
		$Query->Query("SELECT nick FROM `anope_ns_alias` WHERE `nick` = '%s'", $Query->Escape($Nick));
		$Result = $Query->Result();
		if (isset($Result[0]['nick']))
		{
			return "Nickname already registered";
		}

		if ($this->Command($Nick, "NickServ", "REGISTER ".$Password." ".$Email))
		{
			return "Nick registration successful. If your network has email registration enabled check your inbox for the next step of the registration process.";
		}
		
		return "Error registering nick";
	}
 }

?>

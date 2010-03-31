<?php
/*
 * (C) 2010 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for furhter details.
 *
 *
 *
 */

/* This is an example functional webpage showing how to use Anopes MySQL
 * Execute feature to register a nickname online
 */

/* Include Anope's PHP API */
include('Anope.php');

function IsValidEmail($Email)
{
	return eregi("^[_a-z0-9-]+(\.[_a-z0-9-]+)*@[a-z0-9-]+(\.[a-z0-9-]+)*(\.[a-z]{2,3})$", $Email);
}

if (!empty($_POST['nick']) && !empty($_POST['password1']) && !empty($_POST['password2']) && !empty($_POST['email']))
{
	if ($_POST['password1'] != $_POST['password2'])
	{
		echo 'Passwords do not match';
	}
	else if (!IsValidEmail($_POST['email']))
	{
		echo 'Invalid email address';
	}
	else
	{
		/* Create a new connection, arguments are hostname, username, password, and database name */
		$Anope = new Anope("localhost", "anope", "anoperules", "anope");
		/* Check if we connected */
		if (!$Anope->Connected())
		{
			echo "Error connecting to MySQL database";
		}
		else
		{
			echo $Anope->Register($_POST['nick'], $_POST['password1'], $_POST['email']);
		}
		die;
	}
}
?>
</br>
<form method="post" action="Register.php">
Nick: <input type="text" name="nick"></br>
Password: <input type="password" name="password1"></br>
Confirm Password: <input type="password" name="password2"></br>
Email: <input type="text" name="email"></br>
<input type="submit" value="Submit">
</form>


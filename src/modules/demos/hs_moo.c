/**
 * This is an EXAMPLE module, which adds the "moo" command to HostServ
 *
 * This command does NOT do anything useful at all!
 *
 * Please visit http://modules.anope.org for useful modules! 
 *
 **/
#include "module.h"

#define AUTHOR "Anope"									/* Set the Author for a modinfo reply */
#define VERSION VERSION_STRING									/* Set the version for a modinfo reply */

int hs_moo_show(User * u);									/* Function to use when a /hs moo command is recived */
int test(int argc, char **argv);
void myHostServHelp(User *u);  								/* Function to display out help in a /hs help response */
int myHostServMooHelp(User *u); 								/* Function to display help to _everyone_ when a /hs help moo is called*/
int myHostServMooRegHelp(User *u);   							 /* Function to display extra help to regular-users when a /hs help moo is called*/
int myHostServMooOperHelp(User *u);   						 /* Function to display extra help to opers when a /hs help moo is called*/
int myHostServMooAdminHelp(User *u);   						 /* Function to display extra help to admins when a /hs help moo is called*/
int myHostServMooRootHelp(User *u);   						 /* Function to display extra help to roors when a /hs help moo is called*/

int AnopeInit(int argc, char **argv)							/* This will be executed when the module is loaded */
{
    Command *c;											/* Pointer to a Command */
    int status = 0;											/* the status of our new command */
    c = createCommand("moo", hs_moo_show, NULL, -1, -1, -1, -1, -1);	/* Create a new command "moo" pointing to hs_moo */

    moduleAddHelp(c,myHostServMooHelp);						/* add help for all users to this command */
    moduleAddRegHelp(c,myHostServMooRegHelp);				/* add extra regular-user only help to this command */
    moduleAddOperHelp(c,myHostServMooOperHelp);				/* add extra oper only help to this command */
    moduleAddAdminHelp(c,myHostServMooAdminHelp);				/* add extra admin only help to this command */
    moduleAddRootHelp(c,myHostServMooRootHelp);				/* add extra root only help to this command */

    moduleSetHostHelp(myHostServHelp);						/* add us to the .hs help list */

    status = moduleAddCommand(HOSTSERV, c, MOD_HEAD);			/* Add the command to the HOSTSERV cmd table */

    /* Check if we have any argv's */
    if(argc>0) {
	/* we do, the first will be the nick of the person modload'ing us */
	/* or NULL if we were auto-loaded */
	if(argv[0]) {
		alog("hs_moo was modloaded by: [%s]",argv[0]);
	} else {
		alog("hs_moo was automatically loaded by anope");
	}
    }
    alog("hs_moo.so: Add Command 'moo' Status: %d",status);			/* Log the command being added */
         
    moduleAddCallback("test",time(NULL)+dotime("15s"),test,0,NULL);		/* set a call-back function to exec in 3 mins time */
    moduleDelCallback("test");
    moduleAddAuthor(AUTHOR);								/* tell Anope about the author */
    moduleAddVersion(VERSION);								/* Tell Anope about the verison */

    if(status!=MOD_ERR_OK) {
	return MOD_STOP;
    }
    return MOD_CONT;
}

int hs_moo_show(User * u)
{
    notice(s_HostServ, u->nick, "MOO! - This command was loaded via a module!");	/* Just notice the user */
    return MOD_STOP;										/* MOD_STOP means we will NOT pass control back to other */
}														/* modules waiting to handle the /hs moo command! */

int test(int argc, char **argv) {
	alog("CallBack from hs_moo with %d paramaters",argc);
	return MOD_CONT;
}

void AnopeFini(void)
{
  /* module is unloading */
}

/***************************************************************************************************************************************/
/* The code below here shows various ways of dealing with the module help system                                                      */
/***************************************************************************************************************************************/

void myHostServHelp(User *u) {
	notice(s_HostServ,u->nick, "    MOO         Moo's at the user!");		/* this will appear in the help list */
}

int myHostServMooHelp(User *u) {
	notice(s_HostServ,u->nick,"Syntax: Moo");					/* this will be sent to everyone who does /msg hostserv help moo */
	notice(s_HostServ,u->nick,"This command is an example provided");
	notice(s_HostServ,u->nick,"by the Anope development team.");
	return MOD_CONT;										/* allow any other module's with help for /hs moo to run */
}

int myHostServMooRootHelp(User *u) {								/* this will only be sent to ROOTS ONLY who /msg hostserv moo */
	myHostServMooAdminHelp(u);								/* this line lets us show roots the ADMIN help as well as the root help */
	notice(s_HostServ,u->nick,"Only roots will see this part of the help");
	return MOD_CONT;
}

int myHostServMooAdminHelp(User *u) {							/* this will only be sent to ADMINS ONLY who /msg hostserv moo */
	myHostServMooOperHelp(u);								/* this line lets us show admins the OPER help as well as the admin help */
	notice(s_HostServ,u->nick,"Only admins will see this part of the help");
	notice(s_HostServ,u->nick,"why not visit us on www.anope.org ?");
	return MOD_CONT;
}

int myHostServMooOperHelp(User *u) {								/* this will only be sent to OPERS ONLY who /msg hostserv moo */
	notice(s_HostServ,u->nick,"Only opers will see this part of the help");
	notice(s_HostServ,u->nick,"for more help/support with modules");
	notice(s_HostServ,u->nick,"visit us on irc.anope.org #anope! :)");
	return MOD_CONT;
}

int myHostServMooRegHelp(User *u) {								/* this will only be sent to REGULAR USERS ONLY who /msg hostserv moo */
	notice(s_HostServ,u->nick,"Only non-opers will see this part of the help");
	notice(s_HostServ,u->nick,"as we've left it hidden from opers");
	return MOD_CONT;
}

/* EOF */

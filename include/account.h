
/* NickServ nickname structures. */

/** XXX: this really needs to die with fire and be merged with metadata into NickCore or something.
 */
class NickRequest
{
 public:
	NickRequest *next, *prev;
	char *nick;
	char *passcode;
	char password[PASSMAX];
	char *email;
	time_t requested;
	time_t lastmail;			/* Unsaved */
};

class NickCore;

class NickAlias
{
 public:
	NickAlias *next, *prev;
	char *nick;				/* Nickname */
	char *last_quit;			/* Last quit message */
	char *last_realname;			/* Last realname */
	char *last_usermask;			/* Last usermask */
	time_t time_registered;			/* When the nick was registered */
	time_t last_seen;			/* When it was seen online for the last time */
	uint16 status;				/* See NS_* below */
	NickCore *nc;				/* I'm an alias of this */

	/* Not saved */
	ModuleData *moduleData; 		/* Module saved data attached to the nick alias */
	User *u;				/* Current online user that has me */
};

class NickCore
{
 public:
	NickCore *next, *prev;

	char *display;				/* How the nick is displayed */
	char pass[PASSMAX];				/* Password of the nicks */
	char *email;				/* E-mail associated to the nick */
	char *greet;				/* Greet associated to the nick */
	uint32 icq;				/* ICQ # associated to the nick */
	char *url;				/* URL associated to the nick */
	uint32 flags;				/* See NI_* below */
	uint16 language;			/* Language selected by nickname owner (LANG_*) */
    uint16 accesscount;			/* # of entries */
    char **access;				/* Array of strings */
    MemoInfo memos;
    uint16 channelcount;			/* Number of channels currently registered */

    /* Unsaved data */
    ModuleData *moduleData;	  	/* Module saved data attached to the NickCore */
    time_t lastmail;			/* Last time this nick record got a mail */
    SList aliases;				/* List of aliases */
};


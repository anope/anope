/* Modular support
 *
 * (C) 2008 Anope Team
 * Contact us at info@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * $Id$
 *
 */
 
class ChannelInfo : public Extensible
{
 public:
	ChannelInfo()
	{
		next = prev = NULL;
		founderpass[0] = name[0] = last_topic_setter[0] = '\0';
		founder = successor = NULL;
		desc = url = email = last_topic = forbidby = forbidreason = NULL;
		time_registered = last_used = last_topic_time = 0;
		flags = 0;
		bantype = accesscount = akickcount = 0;
		levels = NULL;
		access = NULL;
		akick = NULL;
		mlock_on = mlock_off = mlock_limit = 0;
		mlock_key = mlock_flood = mlock_redirect = entry_message = NULL;
		c = NULL;
		bi = NULL;
		botflags = 0;
		ttb = NULL;
		bwcount = 0;
		badwords = NULL;
		capsmin = capspercent = 0;
		floodlines = floodsecs = 0;
		repeattimes = 0;
	}
	
	ChannelInfo *next, *prev;
	char name[CHANMAX];
	NickCore *founder;
	NickCore *successor;		/* Who gets the channel if the founder
					 			 * nick is dropped or expires */
	char founderpass[PASSMAX];
	char *desc;
	char *url;
	char *email;

	time_t time_registered;
	time_t last_used;
	char *last_topic;			/* Last topic on the channel */
	char last_topic_setter[NICKMAX];	/* Who set the last topic */
	time_t last_topic_time;		/* When the last topic was set */

	uint32 flags;				/* See below */
	char *forbidby;
	char *forbidreason;

	int16 bantype;
	int16 *levels;				/* Access levels for commands */

	uint16 accesscount;
	ChanAccess *access;			/* List of authorized users */
	uint16 akickcount;
	AutoKick *akick;			/* List of users to kickban */

	uint32 mlock_on, mlock_off;		/* See channel modes below */
	uint32 mlock_limit;			/* 0 if no limit */
	char *mlock_key;			/* NULL if no key */
	char *mlock_flood;			/* NULL if no +f */
	char *mlock_redirect;		/* NULL if no +L */

	char *entry_message;		/* Notice sent on entering channel */

	MemoInfo memos;

	struct channel_ *c;			/* Pointer to channel record (if   *
					 			 *	channel is currently in use) */

	/* For BotServ */

	BotInfo *bi;					/* Bot used on this channel */
	uint32 botflags;				/* BS_* below */
	int16 *ttb;						/* Times to ban for each kicker */

	uint16 bwcount;
	BadWord *badwords;				/* For BADWORDS kicker */
	int16 capsmin, capspercent;		/* For CAPS kicker */
	int16 floodlines, floodsecs;	/* For FLOOD kicker */
	int16 repeattimes;				/* For REPEAT kicker */
};



-- If you need to create your db, uncomment the following lines.
-- 
-- CREATE DATABASE anope;
-- USE anope;

-- 
-- Table structure for table 'anope_bs_core'
-- 

DROP TABLE IF EXISTS anope_bs_core;
CREATE TABLE anope_bs_core (
  bs_id int(11) NOT NULL auto_increment,
  nick varchar(255) NOT NULL default '',
  `user` text NOT NULL,
  host text NOT NULL,
  rname text NOT NULL,
  flags int(11) NOT NULL default '0',
  created int(11) NOT NULL default '0',
  chancount int(11) NOT NULL default '0',
  active int(1) NOT NULL default '1',
  PRIMARY KEY  (bs_id),
  UNIQUE KEY nick (nick)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_cs_access'
-- 

DROP TABLE IF EXISTS anope_cs_access;
CREATE TABLE anope_cs_access (
  ca_id int(11) NOT NULL auto_increment,
  in_use int(11) NOT NULL default '0',
  `level` int(11) NOT NULL default '0',
  display varchar(255) NOT NULL default '',
  channel varchar(255) NOT NULL default '',
  last_seen int(11) NOT NULL default '0',
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (ca_id),
  UNIQUE KEY channel (channel,display)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_cs_akicks'
-- 

DROP TABLE IF EXISTS anope_cs_akicks;
CREATE TABLE anope_cs_akicks (
  ck_id int(11) NOT NULL auto_increment,
  channel varchar(255) NOT NULL default '',
  flags int(11) NOT NULL default '0',
  dmask varchar(255) NOT NULL,
  reason text NOT NULL,
  creator text NOT NULL,
  addtime int(11) NOT NULL default '0',
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (ck_id),
  UNIQUE KEY channel (channel,dmask)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_cs_badwords'
-- 

DROP TABLE IF EXISTS anope_cs_badwords;
CREATE TABLE anope_cs_badwords (
  cw_id int(11) NOT NULL auto_increment,
  channel varchar(255) NOT NULL default '',
  word varchar(255) NOT NULL,
  `type` int(11) NOT NULL default '0',
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (cw_id),
  UNIQUE KEY channel (channel,word)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_cs_info'
-- 

DROP TABLE IF EXISTS anope_cs_info;
CREATE TABLE anope_cs_info (
  ci_id int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL default '',
  founder text NOT NULL,
  successor text NOT NULL,
  founderpass tinyblob NOT NULL,
  descr text NOT NULL,
  url text NOT NULL,
  email text NOT NULL,
  time_registered int(10) unsigned NOT NULL default '0',
  last_used int(10) unsigned NOT NULL default '0',
  last_topic text NOT NULL,
  last_topic_setter text NOT NULL,
  last_topic_time int(10) unsigned NOT NULL default '0',
  flags int(10) unsigned NOT NULL default '0',
  forbidby text NOT NULL,
  forbidreason text NOT NULL,
  bantype smallint(6) NOT NULL default '0',
  accesscount smallint(6) NOT NULL default '0',
  akickcount smallint(6) NOT NULL default '0',
  mlock_on int(10) unsigned NOT NULL default '0',
  mlock_off int(10) unsigned NOT NULL default '0',
  mlock_limit int(10) unsigned NOT NULL default '0',
  mlock_key text NOT NULL,
  mlock_flood text NOT NULL,
  mlock_redirect text NOT NULL,
  entry_message text NOT NULL,
  memomax smallint(5) unsigned NOT NULL default '0',
  botnick varchar(255) NOT NULL default '',
  botflags int(10) unsigned NOT NULL default '0',
  bwcount smallint(6) NOT NULL default '0',
  capsmin smallint(6) NOT NULL default '0',
  capspercent smallint(6) NOT NULL default '0',
  floodlines smallint(6) NOT NULL default '0',
  floodsecs smallint(6) NOT NULL default '0',
  repeattimes smallint(6) NOT NULL default '0',
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (ci_id),
  UNIQUE KEY `name` (`name`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_cs_levels'
-- 

DROP TABLE IF EXISTS anope_cs_levels;
CREATE TABLE anope_cs_levels (
  cl_id int(11) NOT NULL auto_increment,
  channel varchar(255) NOT NULL default '',
  position int(11) NOT NULL default '0',
  `level` int(11) NOT NULL default '0',
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (cl_id),
  UNIQUE KEY channel (channel,position)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_cs_ttb'
-- 

DROP TABLE IF EXISTS anope_cs_ttb;
CREATE TABLE anope_cs_ttb (
  ct_id int(11) NOT NULL auto_increment,
  channel varchar(255) NOT NULL default '',
  ttb_id int(11) NOT NULL default '0',
  `value` int(11) NOT NULL default '0',
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (ct_id),
  UNIQUE KEY channel (channel,ttb_id)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_hs_core'
-- 

DROP TABLE IF EXISTS anope_hs_core;
CREATE TABLE anope_hs_core (
  bs_id int(11) NOT NULL auto_increment,
  nick varchar(255) NOT NULL default '',
  vident text NOT NULL,
  vhost text NOT NULL,
  creator text NOT NULL,
  `time` int(11) NOT NULL default '0',
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (bs_id),
  UNIQUE KEY nick (nick)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_info'
-- 

DROP TABLE IF EXISTS anope_info;
CREATE TABLE anope_info (
  version int(11) default NULL,
  `date` datetime default NULL
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_ms_info'
-- 

DROP TABLE IF EXISTS anope_ms_info;
CREATE TABLE anope_ms_info (
  nm_id int(11) NOT NULL auto_increment,
  receiver varchar(255) NOT NULL,
  number int(11) NOT NULL default '0',
  flags int(11) NOT NULL default '0',
  `time` int(11) NOT NULL default '0',
  sender text NOT NULL,
  `text` blob NOT NULL,
  serv enum('NICK','CHAN') NOT NULL default 'NICK',
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (nm_id),
  UNIQUE KEY nm_id (nm_id,serv),
  KEY receiver (receiver,serv)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_ns_access'
-- 

DROP TABLE IF EXISTS anope_ns_access;
CREATE TABLE anope_ns_access (
  na_id int(11) NOT NULL auto_increment,
  display varchar(255) NOT NULL default '',
  access text NOT NULL,
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (na_id)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_ns_alias'
-- 

DROP TABLE IF EXISTS anope_ns_alias;
CREATE TABLE anope_ns_alias (
  na_id int(11) NOT NULL auto_increment,
  display varchar(255) NOT NULL default '',
  nick varchar(255) NOT NULL default '',
  time_registered int(10) unsigned NOT NULL default '0',
  last_seen int(10) unsigned NOT NULL default '0',
  `status` int(11) unsigned NOT NULL default '0',
  last_usermask text NOT NULL,
  last_realname text NOT NULL,
  last_quit text NOT NULL,
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (na_id),
  UNIQUE KEY nick (nick)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_ns_core'
-- 

DROP TABLE IF EXISTS anope_ns_core;
CREATE TABLE anope_ns_core (
  nc_id int(11) NOT NULL auto_increment,
  display varchar(255) NOT NULL default '',
  pass tinyblob NOT NULL,
  email text NOT NULL,
  icq int(10) unsigned NOT NULL default '0',
  url text NOT NULL,
  flags int(11) NOT NULL default '0',
  `language` smallint(5) unsigned NOT NULL default '0',
  accesscount smallint(6) NOT NULL default '0',
  memocount smallint(5) unsigned NOT NULL default '0',
  memomax smallint(5) unsigned NOT NULL default '0',
  channelcount smallint(5) unsigned NOT NULL default '0',
  channelmax smallint(5) unsigned NOT NULL default '0',
  greet text NOT NULL,
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (nc_id),
  UNIQUE KEY display (display)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_ns_request'
-- 

DROP TABLE IF EXISTS anope_ns_request;
CREATE TABLE anope_ns_request (
  nr_id int(11) NOT NULL auto_increment,
  nick varchar(255) NOT NULL default '',
  passcode text NOT NULL,
  `password` tinyblob NOT NULL,
  email text NOT NULL,
  requested int(11) NOT NULL default '0',
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (nr_id),
  UNIQUE KEY nick (nick)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_os_akills'
-- 

DROP TABLE IF EXISTS anope_os_akills;
CREATE TABLE anope_os_akills (
  ok_id int(11) NOT NULL auto_increment,
  `user` varchar(255) NOT NULL,
  host varchar(255) NOT NULL,
  xby text NOT NULL,
  reason text NOT NULL,
  seton int(11) NOT NULL default '0',
  expire int(11) NOT NULL default '0',
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (ok_id),
  UNIQUE KEY `user` (`user`,host)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_os_core'
-- 

DROP TABLE IF EXISTS anope_os_core;
CREATE TABLE anope_os_core (
  oc_id int(11) NOT NULL auto_increment,
  maxusercnt int(11) NOT NULL default '0',
  maxusertime int(11) NOT NULL default '0',
  akills_count int(11) NOT NULL default '0',
  sglines_count int(11) NOT NULL default '0',
  sqlines_count int(11) NOT NULL default '0',
  szlines_count int(11) NOT NULL default '0',
  PRIMARY KEY  (oc_id)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_os_exceptions'
-- 

DROP TABLE IF EXISTS anope_os_exceptions;
CREATE TABLE anope_os_exceptions (
  oe_id int(11) NOT NULL auto_increment,
  mask varchar(255) NOT NULL,
  lim int(11) NOT NULL default '0',
  who text NOT NULL,
  reason text NOT NULL,
  `time` int(11) NOT NULL default '0',
  expires int(11) NOT NULL default '0',
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (oe_id),
  UNIQUE KEY mask (mask)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_os_news'
-- 

DROP TABLE IF EXISTS anope_os_news;
CREATE TABLE anope_os_news (
  on_id int(11) NOT NULL auto_increment,
  `type` int(11) NOT NULL default '0',
  num int(11) NOT NULL default '0',
  ntext text NOT NULL,
  who text NOT NULL,
  `time` int(11) NOT NULL default '0',
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (on_id),
  UNIQUE KEY `type` (`type`,num,`time`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_os_sglines'
-- 

DROP TABLE IF EXISTS anope_os_sglines;
CREATE TABLE anope_os_sglines (
  og_id int(11) NOT NULL auto_increment,
  mask varchar(255) NOT NULL,
  xby text NOT NULL,
  reason text NOT NULL,
  seton int(11) NOT NULL default '0',
  expire int(11) NOT NULL default '0',
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (og_id),
  UNIQUE KEY mask (mask)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_os_sqlines'
-- 

DROP TABLE IF EXISTS anope_os_sqlines;
CREATE TABLE anope_os_sqlines (
  og_id int(11) NOT NULL auto_increment,
  mask varchar(255) NOT NULL,
  xby text NOT NULL,
  reason text NOT NULL,
  seton int(11) NOT NULL default '0',
  expire int(11) NOT NULL default '0',
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (og_id),
  UNIQUE KEY mask (mask)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

-- --------------------------------------------------------

-- 
-- Table structure for table 'anope_os_szlines'
-- 

DROP TABLE IF EXISTS anope_os_szlines;
CREATE TABLE anope_os_szlines (
  og_id int(11) NOT NULL auto_increment,
  mask varchar(255) NOT NULL,
  xby text NOT NULL,
  reason text NOT NULL,
  seton int(11) NOT NULL default '0',
  expire int(11) NOT NULL default '0',
  active tinyint(1) NOT NULL default '1',
  PRIMARY KEY  (og_id),
  UNIQUE KEY mask (mask)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

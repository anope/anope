-- $Id$

-- If you need to create your db, uncomment the following lines.
-- 
-- CREATE DATABASE anope;
-- USE anope;

-- 
-- Tabellenstruktur für Tabelle `anope_bs_badwords`
-- 

CREATE TABLE `anope_bs_badwords` (
  `channel` varchar(255) NOT NULL default '',
  `word` varchar(255) NOT NULL,
  `type` varchar(50) NOT NULL,
  UNIQUE KEY `channel` (`channel`,`word`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_bs_badwords`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_bs_core`
-- 

CREATE TABLE `anope_bs_core` (
  `nick` varchar(255) NOT NULL default '',
  `user` varchar(255) NOT NULL default '',
  `host` text NOT NULL,
  `rname` text NOT NULL,
  `flags` text NOT NULL,
  `created` int(11) NOT NULL default '0',
  `chancount` int(11) NOT NULL default '0',
  PRIMARY KEY  (`nick`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_bs_core`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_bs_info_metadata`
-- 

CREATE TABLE `anope_bs_info_metadata` (
  `botname` varchar(255) NOT NULL default '',
  `name` varchar(255) NOT NULL default '',
  `value` text NOT NULL,
  UNIQUE KEY `botname` (`botname`,`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_bs_info_metadata`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_commands`
-- 

CREATE TABLE `anope_commands` (
  `nick` varchar(255) NOT NULL default '',
  `service` varchar(255) NOT NULL default '',
  `command` text NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_commands`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_cs_access`
-- 

CREATE TABLE `anope_cs_access` (
  `level` int(11) NOT NULL default '0',
  `display` varchar(255) NOT NULL default '',
  `channel` varchar(255) NOT NULL default '',
  `last_seen` int(11) NOT NULL default '0',
  `creator` varchar(255) NOT NULL default '',
  UNIQUE KEY `channel` (`channel`,`display`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_cs_access`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_cs_akick`
-- 

CREATE TABLE `anope_cs_akick` (
  `channel` varchar(255) NOT NULL default '',
  `flags` varchar(255) NOT NULL default '',
  `mask` varchar(255) NOT NULL default '',
  `reason` text NOT NULL,
  `creator` varchar(255) NOT NULL default '',
  `created` int(11) NOT NULL default '0',
  `last_used` int(11) NOT NULL default '0',
  UNIQUE KEY `channel` (`channel`,`mask`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_cs_akick`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_cs_info`
-- 

CREATE TABLE `anope_cs_info` (
  `name` varchar(255) NOT NULL default '',
  `founder` text NOT NULL,
  `successor` text NOT NULL,
  `descr` text NOT NULL,
  `url` text NOT NULL,
  `email` text NOT NULL,
  `time_registered` int(11) unsigned NOT NULL default '0',
  `last_used` int(10) unsigned NOT NULL default '0',
  `last_topic` text NOT NULL,
  `last_topic_setter` text NOT NULL,
  `last_topic_time` int(11) unsigned NOT NULL default '0',
  `flags` text NOT NULL,
  `forbidby` text NOT NULL,
  `forbidreason` text NOT NULL,
  `bantype` smallint(6) NOT NULL default '0',
  `mlock_on` text NOT NULL,
  `mlock_off` text NOT NULL,
  `mlock_params` text NOT NULL,
  `entry_message` text NOT NULL,
  `memomax` smallint(5) unsigned NOT NULL default '0',
  `botnick` varchar(255) NOT NULL default '',
  `botflags` text NOT NULL,
  `capsmin` smallint(6) NOT NULL default '0',
  `capspercent` smallint(6) NOT NULL default '0',
  `floodlines` smallint(6) NOT NULL default '0',
  `floodsecs` smallint(6) NOT NULL default '0',
  `repeattimes` smallint(6) NOT NULL default '0',
  PRIMARY KEY  (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_cs_info`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_cs_info_metadata`
-- 

CREATE TABLE `anope_cs_info_metadata` (
  `channel` varchar(255) NOT NULL default '',
  `name` varchar(255) NOT NULL default '',
  `value` text NOT NULL,
  UNIQUE KEY `channel` (`channel`,`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_cs_info_metadata`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_cs_levels`
-- 

CREATE TABLE `anope_cs_levels` (
  `channel` varchar(255) NOT NULL default '',
  `position` int(11) NOT NULL default '0',
  `level` int(11) NOT NULL default '0',
  UNIQUE KEY `channel` (`channel`,`position`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_cs_levels`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_cs_ttb`
-- 

CREATE TABLE `anope_cs_ttb` (
  `channel` varchar(255) NOT NULL default '',
  `ttb_id` int(11) NOT NULL default '0',
  `value` int(11) NOT NULL default '0',
  UNIQUE KEY `channel` (`channel`,`ttb_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_cs_ttb`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_extra`
-- 

CREATE TABLE `anope_extra` (
  `data` text NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_extra`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_info`
-- 

CREATE TABLE `anope_info` (
  `version` int(11) default NULL,
  `date` datetime default NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_info`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_metadata`
-- 

CREATE TABLE `anope_metadata` (
  `name` varchar(255) NOT NULL default '',
  `value` text NOT NULL,
  PRIMARY KEY  (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_metadata`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_ms_info`
-- 

CREATE TABLE `anope_ms_info` (
  `receiver` varchar(255) NOT NULL,
  `number` int(11) NOT NULL default '0',
  `flags` int(11) NOT NULL default '0',
  `time` int(11) NOT NULL default '0',
  `sender` text NOT NULL,
  `text` blob NOT NULL,
  `serv` enum('NICK','CHAN') NOT NULL default 'NICK',
  UNIQUE KEY `receiver` (`receiver`,`number`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_ms_info`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_ns_access`
-- 

CREATE TABLE `anope_ns_access` (
  `display` varchar(255) NOT NULL default '',
  `access` varchar(160) NOT NULL,
  UNIQUE KEY `display` (`display`,`access`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_ns_access`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_ns_alias`
-- 

CREATE TABLE `anope_ns_alias` (
  `nick` varchar(255) NOT NULL default '',
  `last_quit` text NOT NULL,
  `last_realname` text NOT NULL,
  `last_usermask` text NOT NULL,
  `time_registered` int(11) unsigned NOT NULL default '0',
  `last_seen` int(10) unsigned NOT NULL default '0',
  `flags` text NOT NULL,
  `display` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`nick`),
  KEY `display` (`display`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_ns_alias`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_ns_alias_metadata`
-- 

CREATE TABLE `anope_ns_alias_metadata` (
  `nick` varchar(255) NOT NULL default '',
  `name` varchar(255) NOT NULL default '',
  `value` text NOT NULL,
  PRIMARY KEY  (`name`),
  KEY `nick` (`nick`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_ns_alias_metadata`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_ns_core`
-- 

CREATE TABLE `anope_ns_core` (
  `display` varchar(255) NOT NULL default '',
  `pass` text NOT NULL,
  `email` text NOT NULL,
  `greet` text NOT NULL,
  `icq` int(10) unsigned NOT NULL default '0',
  `url` text NOT NULL,
  `flags` text NOT NULL,
  `language` smallint(5) unsigned NOT NULL default '0',
  `channelcount` smallint(5) unsigned NOT NULL default '0',
  `memomax` smallint(5) unsigned NOT NULL default '0',
  PRIMARY KEY  (`display`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_ns_core`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_ns_core_metadata`
-- 

CREATE TABLE `anope_ns_core_metadata` (
  `nick` varchar(255) NOT NULL default '',
  `name` varchar(255) NOT NULL default '',
  `value` text NOT NULL,
  PRIMARY KEY  (`name`),
  KEY `nick` (`nick`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_ns_core_metadata`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_ns_request`
-- 

CREATE TABLE `anope_ns_request` (
  `nick` varchar(255) NOT NULL default '',
  `passcode` text NOT NULL,
  `password` text NOT NULL,
  `email` text NOT NULL,
  `requested` int(11) NOT NULL default '0',
  PRIMARY KEY  (`nick`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_ns_request`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_os_akills`
-- 

CREATE TABLE `anope_os_akills` (
  `user` varchar(255) NOT NULL,
  `host` varchar(255) NOT NULL,
  `xby` text NOT NULL,
  `reason` text NOT NULL,
  `seton` int(11) NOT NULL default '0',
  `expire` int(11) NOT NULL default '0',
  UNIQUE KEY `user` (`user`,`host`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_os_akills`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_os_core`
-- 

CREATE TABLE `anope_os_core` (
  `maxusercnt` int(11) NOT NULL default '0',
  `maxusertime` int(11) NOT NULL default '0',
  `akills_count` int(11) NOT NULL default '0',
  `sglines_count` int(11) NOT NULL default '0',
  `sqlines_count` int(11) NOT NULL default '0',
  `szlines_count` int(11) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_os_core`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_os_exceptions`
-- 

CREATE TABLE `anope_os_exceptions` (
  `mask` varchar(255) NOT NULL,
  `limit` int(11) NOT NULL default '0',
  `who` text NOT NULL,
  `reason` text NOT NULL,
  `time` int(11) NOT NULL default '0',
  `expires` int(11) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_os_exceptions`
-- 


-- --------------------------------------------------------

-- 
-- Tabellenstruktur für Tabelle `anope_os_sxlines`
-- 

CREATE TABLE `anope_os_sxlines` (
  `type` varchar(20) NOT NULL,
  `mask` varchar(255) NOT NULL,
  `xby` text NOT NULL,
  `reason` text NOT NULL,
  `seton` int(11) NOT NULL default '0',
  `expire` int(11) NOT NULL default '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- 
-- Daten für Tabelle `anope_os_sxlines`
-- 


-- 
-- Constraints der exportierten Tabellen
-- 

-- 
-- Constraints der Tabelle `anope_bs_badwords`
-- 
ALTER TABLE `anope_bs_badwords`
  ADD CONSTRAINT `anope_bs_badwords_ibfk_1` FOREIGN KEY (`channel`) REFERENCES `anope_cs_info` (`name`) ON DELETE CASCADE ON UPDATE CASCADE;

-- 
-- Constraints der Tabelle `anope_bs_info_metadata`
-- 
ALTER TABLE `anope_bs_info_metadata`
  ADD CONSTRAINT `anope_bs_info_metadata_ibfk_1` FOREIGN KEY (`botname`) REFERENCES `anope_bs_core` (`nick`) ON DELETE CASCADE ON UPDATE CASCADE;

-- 
-- Constraints der Tabelle `anope_cs_access`
-- 
ALTER TABLE `anope_cs_access`
  ADD CONSTRAINT `anope_cs_access_ibfk_1` FOREIGN KEY (`channel`) REFERENCES `anope_cs_info` (`name`) ON DELETE CASCADE ON UPDATE CASCADE;

-- 
-- Constraints der Tabelle `anope_cs_akick`
-- 
ALTER TABLE `anope_cs_akick`
  ADD CONSTRAINT `anope_cs_akick_ibfk_1` FOREIGN KEY (`channel`) REFERENCES `anope_cs_info` (`name`) ON DELETE CASCADE ON UPDATE CASCADE;

-- 
-- Constraints der Tabelle `anope_cs_info_metadata`
-- 
ALTER TABLE `anope_cs_info_metadata`
  ADD CONSTRAINT `anope_cs_info_metadata_ibfk_1` FOREIGN KEY (`channel`) REFERENCES `anope_cs_info` (`name`) ON DELETE CASCADE ON UPDATE CASCADE;

-- 
-- Constraints der Tabelle `anope_cs_levels`
-- 
ALTER TABLE `anope_cs_levels`
  ADD CONSTRAINT `anope_cs_levels_ibfk_1` FOREIGN KEY (`channel`) REFERENCES `anope_cs_info` (`name`) ON DELETE CASCADE ON UPDATE CASCADE;

-- 
-- Constraints der Tabelle `anope_cs_ttb`
-- 
ALTER TABLE `anope_cs_ttb`
  ADD CONSTRAINT `anope_cs_ttb_ibfk_1` FOREIGN KEY (`channel`) REFERENCES `anope_cs_info` (`name`) ON DELETE CASCADE ON UPDATE CASCADE;

-- 
-- Constraints der Tabelle `anope_ms_info`
-- 
ALTER TABLE `anope_ms_info`
  ADD CONSTRAINT `anope_ms_info_ibfk_1` FOREIGN KEY (`receiver`) REFERENCES `anope_ns_core` (`display`) ON DELETE CASCADE ON UPDATE CASCADE;

-- 
-- Constraints der Tabelle `anope_ns_access`
-- 
ALTER TABLE `anope_ns_access`
  ADD CONSTRAINT `anope_ns_access_ibfk_1` FOREIGN KEY (`display`) REFERENCES `anope_ns_core` (`display`) ON DELETE CASCADE ON UPDATE CASCADE;

-- 
-- Constraints der Tabelle `anope_ns_alias`
-- 
ALTER TABLE `anope_ns_alias`
  ADD CONSTRAINT `anope_ns_alias_ibfk_1` FOREIGN KEY (`display`) REFERENCES `anope_ns_core` (`display`) ON DELETE CASCADE ON UPDATE CASCADE;

-- 
-- Constraints der Tabelle `anope_ns_alias_metadata`
-- 
ALTER TABLE `anope_ns_alias_metadata`
  ADD CONSTRAINT `anope_ns_alias_metadata_ibfk_1` FOREIGN KEY (`nick`) REFERENCES `anope_ns_alias` (`nick`) ON DELETE CASCADE ON UPDATE CASCADE;

-- 
-- Constraints der Tabelle `anope_ns_core_metadata`
-- 
ALTER TABLE `anope_ns_core_metadata`
  ADD CONSTRAINT `anope_ns_core_metadata_ibfk_1` FOREIGN KEY (`nick`) REFERENCES `anope_ns_core` (`display`) ON DELETE CASCADE ON UPDATE CASCADE;

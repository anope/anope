-- phpMyAdmin SQL Dump
-- version 3.3.5
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Apr 28, 2011 at 07:08 PM
-- Server version: 5.1.50
-- PHP Version: 5.3.3-pl1-gentoo

SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";

--
-- Database: `anope`
--

-- --------------------------------------------------------

--
-- Table structure for table `anope_bs_badwords`
--

CREATE TABLE IF NOT EXISTS `anope_bs_badwords` (
  `channel` varchar(255) NOT NULL DEFAULT '',
  `word` varchar(255) NOT NULL,
  `type` varchar(50) NOT NULL,
  UNIQUE KEY `channel` (`channel`,`word`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_bs_core`
--

CREATE TABLE IF NOT EXISTS `anope_bs_core` (
  `nick` varchar(255) NOT NULL DEFAULT '',
  `user` varchar(255) NOT NULL DEFAULT '',
  `host` text NOT NULL,
  `rname` text NOT NULL,
  `flags` text NOT NULL,
  `created` int(10) unsigned NOT NULL DEFAULT '0',
  `chancount` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`nick`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_bs_info_metadata`
--

CREATE TABLE IF NOT EXISTS `anope_bs_info_metadata` (
  `botname` varchar(255) NOT NULL DEFAULT '',
  `name` varchar(255) NOT NULL DEFAULT '',
  `value` text NOT NULL,
  KEY `FK_anope_bs_info_metadata_botname` (`botname`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_cs_access`
--

CREATE TABLE IF NOT EXISTS `anope_cs_access` (
  `level` int(11) NOT NULL DEFAULT '0',
  `display` varchar(255) NOT NULL DEFAULT '',
  `channel` varchar(255) NOT NULL DEFAULT '',
  `last_seen` int(10) unsigned NOT NULL DEFAULT '0',
  `creator` varchar(255) NOT NULL DEFAULT '',
  UNIQUE KEY `channel` (`channel`,`display`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_cs_akick`
--

CREATE TABLE IF NOT EXISTS `anope_cs_akick` (
  `channel` varchar(255) NOT NULL DEFAULT '',
  `flags` varchar(255) NOT NULL DEFAULT '',
  `mask` varchar(255) NOT NULL DEFAULT '',
  `reason` text NOT NULL,
  `creator` varchar(255) NOT NULL DEFAULT '',
  `created` int(10) unsigned NOT NULL DEFAULT '0',
  `last_used` int(10) unsigned NOT NULL DEFAULT '0',
  UNIQUE KEY `channel` (`channel`,`mask`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_cs_info`
--

CREATE TABLE IF NOT EXISTS `anope_cs_info` (
  `name` varchar(255) NOT NULL DEFAULT '',
  `founder` text NOT NULL,
  `successor` text NOT NULL,
  `descr` text NOT NULL,
  `time_registered` int(10) unsigned NOT NULL DEFAULT '0',
  `last_used` int(10) unsigned NOT NULL DEFAULT '0',
  `last_topic` text NOT NULL,
  `last_topic_setter` text NOT NULL,
  `last_topic_time` int(10) unsigned NOT NULL DEFAULT '0',
  `flags` text NOT NULL,
  `bantype` smallint(6) NOT NULL DEFAULT '0',
  `memomax` smallint(5) unsigned NOT NULL DEFAULT '0',
  `botnick` varchar(255) NOT NULL DEFAULT '',
  `botflags` text NOT NULL,
  `capsmin` smallint(6) NOT NULL DEFAULT '0',
  `capspercent` smallint(6) NOT NULL DEFAULT '0',
  `floodlines` smallint(6) NOT NULL DEFAULT '0',
  `floodsecs` smallint(6) NOT NULL DEFAULT '0',
  `repeattimes` smallint(6) NOT NULL DEFAULT '0',
  PRIMARY KEY (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_cs_info_metadata`
--

CREATE TABLE IF NOT EXISTS `anope_cs_info_metadata` (
  `channel` varchar(255) NOT NULL DEFAULT '',
  `name` varchar(255) NOT NULL DEFAULT '',
  `value` text NOT NULL,
  KEY `FK_anope_cs_info_metadata_channel` (`channel`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_cs_levels`
--

CREATE TABLE IF NOT EXISTS `anope_cs_levels` (
  `channel` varchar(255) NOT NULL DEFAULT '',
  `position` int(11) NOT NULL DEFAULT '0',
  `level` int(11) NOT NULL DEFAULT '0',
  UNIQUE KEY `channel` (`channel`,`position`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_cs_mlock`
--

CREATE TABLE IF NOT EXISTS `anope_cs_mlock` (
  `channel` varchar(255) NOT NULL,
  `mode` varchar(127) NOT NULL,
  `status` int(11) NOT NULL,
  `setter` varchar(255) NOT NULL,
  `created` int(11) NOT NULL,
  `param` varchar(255) NOT NULL,
  UNIQUE KEY `entry` (`channel`,`mode`,`status`,`setter`,`param`),
  KEY `FK_anope_cs_mlock` (`channel`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_cs_ttb`
--

CREATE TABLE IF NOT EXISTS `anope_cs_ttb` (
  `channel` varchar(255) NOT NULL DEFAULT '',
  `ttb_id` int(11) NOT NULL DEFAULT '0',
  `value` int(11) NOT NULL DEFAULT '0',
  UNIQUE KEY `channel` (`channel`,`ttb_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_extra`
--

CREATE TABLE IF NOT EXISTS `anope_extra` (
  `data` text NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_hs_core`
--

CREATE TABLE IF NOT EXISTS `anope_hs_core` (
  `nick` varchar(255) NOT NULL,
  `vident` varchar(64) NOT NULL,
  `vhost` varchar(255) NOT NULL,
  `creator` varchar(255) NOT NULL,
  `time` int(11) NOT NULL,
  KEY `FK_anope_hs_core_nick` (`nick`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_info`
--

CREATE TABLE IF NOT EXISTS `anope_info` (
  `version` int(11) DEFAULT NULL,
  `date` datetime DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_ms_info`
--

CREATE TABLE IF NOT EXISTS `anope_ms_info` (
  `receiver` varchar(255) NOT NULL,
  `flags` int(11) NOT NULL DEFAULT '0',
  `time` int(10) unsigned NOT NULL DEFAULT '0',
  `sender` text NOT NULL,
  `text` blob NOT NULL,
  `serv` enum('NICK','CHAN') NOT NULL DEFAULT 'NICK',
  KEY `FK_anope_ms_info_receiver` (`receiver`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_ns_access`
--

CREATE TABLE IF NOT EXISTS `anope_ns_access` (
  `display` varchar(255) NOT NULL DEFAULT '',
  `access` varchar(160) NOT NULL,
  KEY `FK_anope_ns_access_display` (`display`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_ns_alias`
--

CREATE TABLE IF NOT EXISTS `anope_ns_alias` (
  `nick` varchar(255) NOT NULL DEFAULT '',
  `last_quit` text NOT NULL,
  `last_realname` text NOT NULL,
  `last_usermask` text NOT NULL,
  `time_registered` int(10) unsigned NOT NULL DEFAULT '0',
  `last_seen` int(10) unsigned NOT NULL DEFAULT '0',
  `flags` text NOT NULL,
  `display` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`nick`),
  KEY `FK_anope_ns_alias_display` (`display`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_ns_alias_metadata`
--

CREATE TABLE IF NOT EXISTS `anope_ns_alias_metadata` (
  `nick` varchar(255) NOT NULL DEFAULT '',
  `name` varchar(255) NOT NULL DEFAULT '',
  `value` text NOT NULL,
  KEY `FK_anope_ns_alias_metadata_nick` (`nick`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_ns_core`
--

CREATE TABLE IF NOT EXISTS `anope_ns_core` (
  `display` varchar(255) NOT NULL DEFAULT '',
  `pass` text NOT NULL,
  `email` text NOT NULL,
  `greet` text NOT NULL,
  `flags` text NOT NULL,
  `language` varchar(5) NOT NULL DEFAULT '',
  `memomax` smallint(5) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`display`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_ns_core_metadata`
--

CREATE TABLE IF NOT EXISTS `anope_ns_core_metadata` (
  `nick` varchar(255) NOT NULL DEFAULT '',
  `name` varchar(255) NOT NULL DEFAULT '',
  `value` text NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_os_core`
--

CREATE TABLE IF NOT EXISTS `anope_os_core` (
  `maxusercnt` int(11) NOT NULL DEFAULT '0',
  `maxusertime` int(10) unsigned NOT NULL DEFAULT '0',
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_os_exceptions`
--

CREATE TABLE IF NOT EXISTS `anope_os_exceptions` (
  `mask` varchar(255) NOT NULL,
  `slimit` int(11) NOT NULL DEFAULT '0',
  `who` text NOT NULL,
  `reason` text NOT NULL,
  `time` int(10) unsigned NOT NULL DEFAULT '0',
  `expires` int(10) unsigned NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `anope_os_xlines`
--

CREATE TABLE IF NOT EXISTS `anope_os_xlines` (
  `type` varchar(1) NOT NULL,
  `mask` varchar(255) NOT NULL,
  `xby` text NOT NULL,
  `reason` text NOT NULL,
  `seton` int(10) unsigned NOT NULL DEFAULT '0',
  `expire` int(10) unsigned NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `anope_bs_badwords`
--
ALTER TABLE `anope_bs_badwords`
  ADD CONSTRAINT `FK_anope_bs_badwords_channel` FOREIGN KEY (`channel`) REFERENCES `anope_cs_info` (`name`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `anope_bs_info_metadata`
--
ALTER TABLE `anope_bs_info_metadata`
  ADD CONSTRAINT `FK_anope_bs_info_metadata_botname` FOREIGN KEY (`botname`) REFERENCES `anope_bs_core` (`nick`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `anope_cs_access`
--
ALTER TABLE `anope_cs_access`
  ADD CONSTRAINT `FK_anope_cs_access_channel` FOREIGN KEY (`channel`) REFERENCES `anope_cs_info` (`name`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `anope_cs_akick`
--
ALTER TABLE `anope_cs_akick`
  ADD CONSTRAINT `FK_anope_cs_akick_channel` FOREIGN KEY (`channel`) REFERENCES `anope_cs_info` (`name`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `anope_cs_info_metadata`
--
ALTER TABLE `anope_cs_info_metadata`
  ADD CONSTRAINT `FK_anope_cs_info_metadata_channel` FOREIGN KEY (`channel`) REFERENCES `anope_cs_info` (`name`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `anope_cs_levels`
--
ALTER TABLE `anope_cs_levels`
  ADD CONSTRAINT `FK_anope_cs_levels_channel` FOREIGN KEY (`channel`) REFERENCES `anope_cs_info` (`name`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `anope_cs_mlock`
--
ALTER TABLE `anope_cs_mlock`
  ADD CONSTRAINT `FK_anope_cs_mlock_channel` FOREIGN KEY (`channel`) REFERENCES `anope_cs_info` (`name`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `anope_cs_ttb`
--
ALTER TABLE `anope_cs_ttb`
  ADD CONSTRAINT `FK_anope_cs_ttb_channel` FOREIGN KEY (`channel`) REFERENCES `anope_cs_info` (`name`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `anope_hs_core`
--
ALTER TABLE `anope_hs_core`
  ADD CONSTRAINT `FK_anope_hs_core_nick` FOREIGN KEY (`nick`) REFERENCES `anope_ns_alias` (`nick`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `anope_ms_info`
--
ALTER TABLE `anope_ms_info`
  ADD CONSTRAINT `FK_anope_ms_info_receiver` FOREIGN KEY (`receiver`) REFERENCES `anope_ns_alias` (`nick`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `anope_ns_access`
--
ALTER TABLE `anope_ns_access`
  ADD CONSTRAINT `FK_anope_ns_access_display` FOREIGN KEY (`display`) REFERENCES `anope_ns_core` (`display`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `anope_ns_alias`
--
ALTER TABLE `anope_ns_alias`
  ADD CONSTRAINT `FK_anope_ns_alias_display` FOREIGN KEY (`display`) REFERENCES `anope_ns_core` (`display`) ON DELETE CASCADE ON UPDATE CASCADE;

--
-- Constraints for table `anope_ns_alias_metadata`
--
ALTER TABLE `anope_ns_alias_metadata`
  ADD CONSTRAINT `FK_anope_ns_alias_metadata_nick` FOREIGN KEY (`nick`) REFERENCES `anope_ns_alias` (`nick`) ON DELETE CASCADE ON UPDATE CASCADE;


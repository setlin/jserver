-- MySQL dump 10.13  Distrib 5.6.23, for Linux (x86_64)
--
-- Host: localhost    Database: jsystem
-- ------------------------------------------------------
-- Server version	5.6.23-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `jsaccountinfo`
--

DROP TABLE IF EXISTS `jsaccountinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `jsaccountinfo` (
  `charid` int(11) NOT NULL AUTO_INCREMENT,
  `passwd` varchar(64) NOT NULL DEFAULT '' COMMENT 'encrypted by MD5',
  `isValid` int(1) NOT NULL DEFAULT '0' COMMENT 'account is valid or not, 0 for valid, 1 for invalid',
  PRIMARY KEY (`charid`)
) ENGINE=MyISAM AUTO_INCREMENT=1000 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `jscharinfo`
--

DROP TABLE IF EXISTS `jscharinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `jscharinfo` (
  `charid` int(11) NOT NULL DEFAULT '0' COMMENT 'id',
  `charname` varchar(64) NOT NULL DEFAULT 'JSystem' COMMENT 'name',
  `gender` int(1) NOT NULL DEFAULT '0' COMMENT 'gender, 0 means male, 1 means female',
  `age` int(3) NOT NULL DEFAULT '0' COMMENT 'age',
  `locality` varchar(32) NOT NULL DEFAULT 'CHINA' COMMENT 'locality',
  `signature` varchar(256) NOT NULL DEFAULT 'I WOULDD RATHER DO SOMETHING GREAT AND FAIL THAN DO NOTHING AND SUCCEED' COMMENT 'signature',
  `grouplist` varchar(1024) NOT NULL DEFAULT 'JSYSTEM' COMMENT 'join group list, like: 1_2_',
  PRIMARY KEY (`charid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `jsfriendinfo`
--

DROP TABLE IF EXISTS `jsfriendinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `jsfriendinfo` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `charid` int(11) NOT NULL DEFAULT '0',
  `friendid` int(11) NOT NULL DEFAULT '0',
  `isdelete` int(1) NOT NULL DEFAULT '0' COMMENT 'the flag of relation, 0 friend; 1 request; 2 delete',
  `addtime` int(11) NOT NULL DEFAULT '0' COMMENT 'time of being friendship',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `jsgroupinfo`
--

DROP TABLE IF EXISTS `jsgroupinfo`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `jsgroupinfo` (
  `groupid` int(11) NOT NULL AUTO_INCREMENT,
  `groupname` varchar(64) NOT NULL DEFAULT 'JSYSTEM' COMMENT 'name',
  `ownerid` int(11) NOT NULL DEFAULT '0' COMMENT 'the owner id of the group',
  `intro` varchar(256) NOT NULL DEFAULT 'JSYSTEM' COMMENT 'introduction of the group',
  `memberlist` varchar(4096) NOT NULL DEFAULT 'JSYSTEM' COMMENT 'member list of the group, like: 1001_1002_',
  PRIMARY KEY (`groupid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2015-04-09  0:00:08

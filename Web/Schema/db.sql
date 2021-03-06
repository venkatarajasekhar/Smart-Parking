-- phpMyAdmin SQL Dump
-- version 4.1.3
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Jan 07, 2014 at 06:03 AM
-- Server version: 5.6.15-log
-- PHP Version: 5.4.23

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";

--
-- Database: `parking`
--

DELIMITER $$
--
-- Functions
--
DROP FUNCTION IF EXISTS `calcDistance`$$
CREATE FUNCTION `calcDistance`(lat1 DECIMAL(10,8), lng1 DECIMAL(11,8), lat2 DECIMAL(10,8), lng2 DECIMAL(11,8)) RETURNS decimal(7,2)
BEGIN
  RETURN 3959 * acos (
          cos ( radians(lat1) )
          * cos( radians( lat2 ) )
          * cos( radians( lng2 ) - radians(lng1) )
          + sin ( radians(lat1) )
          * sin( radians( lat2 ) )
         );
END$$

DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `api_keys`
--

DROP TABLE IF EXISTS `api_keys`;
CREATE TABLE IF NOT EXISTS `api_keys` (
  `key` varchar(32) NOT NULL,
  `actions` int(11) NOT NULL DEFAULT '1',
  UNIQUE KEY `key` (`key`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `api_keys`
--

INSERT INTO `api_keys` (`key`, `actions`) VALUES
('c51088dad22ea00758e0bdcc2a29bf5c', 7),
('e1d1b85eef174d89121da2407f7bb15b', 1);

-- --------------------------------------------------------

--
-- Table structure for table `history`
--

DROP TABLE IF EXISTS `history`;
CREATE TABLE IF NOT EXISTS `history` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `node_id` varchar(5) NOT NULL,
  `updated` datetime NOT NULL,
  `total` tinyint(4) NOT NULL,
  `available` tinyint(4) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=9 ;

--
-- Dumping data for table `history`
--

INSERT INTO `history` (`id`, `node_id`, `updated`, `total`, `available`) VALUES
(1, 'CCCCC', '2014-02-21 14:59:00', 5, 2),
(2, 'CCCCC', '2014-02-21 15:00:00', 5, 4),
(3, 'CCCCC', '2014-02-21 15:01:00', 5, 5),
(4, 'CCCCC', '2014-02-21 15:02:00', 5, 0),
(5, 'CCCCC', '2014-02-21 15:03:00', 5, 1),
(6, 'CCCCC', '2014-02-21 15:04:00', 5, 0),
(7, 'CCCCC', '2014-02-21 19:08:03', 5, 2),
(8, 'EEEEE', '2014-02-21 19:08:42', 2, 0);

-- --------------------------------------------------------

--
-- Table structure for table `nodes`
--

DROP TABLE IF EXISTS `nodes`;
CREATE TABLE IF NOT EXISTS `nodes` (
  `id` varchar(5) NOT NULL,
  `lat` decimal(10,8) NOT NULL,
  `lng` decimal(11,8) NOT NULL,
  `total` tinyint(4) NOT NULL,
  `available` tinyint(4) DEFAULT '0',
  `error` tinyint(4) DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `nodes`
--

INSERT INTO `nodes` (`id`, `lat`, `lng`, `total`, `available`, `error`) VALUES
('CCCCC', '38.56050207', '-121.42112508', 5, 1, 0),
('DDDDD', '38.56050784', '-121.42088770', 7, 0, 0),
('EEEEE', '38.56071024', '-121.42106608', 2, 0, 1),
('FFFFF', '38.56032222', '-121.42095209', 15, 5, 3),
('laket', '38.78554795', '-121.31922662', 1, 0, 0),
('test1', '38.56053248', '-121.42139196', 4, 2, 0),
('testa', '38.55971031', '-121.41534090', 1, 0, 0),
('testb', '38.56020530', '-121.40928983', 1, 0, 0),
('testc', '38.56123719', '-121.40289545', 1, 0, 0),
('testd', '38.56686621', '-121.38278962', 1, 1, 0),
('teste', '38.57464209', '-121.35103226', 1, 0, 0),
('testf', '38.62857195', '-121.27610207', 1, 0, 0);

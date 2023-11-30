DROP TABLE IF EXISTS `mod_auctionhousebot`;
CREATE TABLE `mod_auctionhousebot` (
  `auctionhouse` int(11) NOT NULL DEFAULT '0' COMMENT 'mapID of the auctionhouse.',
  `name` char(25) DEFAULT NULL COMMENT 'Text name of the auctionhouse.',
  `minitems` int(11) DEFAULT '0' COMMENT 'This is the minimum number of items you want to keep in the auction house. a 0 here will make it the same as the maximum.',
  `maxitems` int(11) DEFAULT '0' COMMENT 'This is the number of items you want to keep in the auction house.',
  `buyerbiddinginterval` int(11) DEFAULT '1' COMMENT 'Interval how frequently AHB bids on each AH. Time in minutes',
  `buyerbidsperinterval` int(11) DEFAULT '1' COMMENT 'number of bids to put in per bidding interval',
  PRIMARY KEY (`auctionhouse`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- AHBot auctionhouse configuration
INSERT INTO `mod_auctionhousebot` (`auctionhouse`, `name`, `minitems`, `maxitems`, `buyerbiddinginterval`, `buyerbidsperinterval`)
VALUES
(2,'Alliance',10000,10000,1,1),
(6,'Horde',10000,10000,1,1),
(7,'Neutral',10000,10000,1,1);
[1] create databases:
mysql> create database jsystem;

[2] create table:
[2.1] account information
create table jsaccountinfo(
    charid int(11) NOT NULL AUTO_INCREMENT,
    passwd varchar(64) NOT NULL DEFAULT '' COMMENT 'encrypted by MD5',
    isValid int(1) NOT NULL DEFAULT 0 COMMENT 'account is valid or not, 0 for valid, 1 for invalid',
    PRIMARY KEY(charid)
)ENGINE=MyISAM AUTO_INCREMENT = 1000 DEFAULT CHARSET=utf8;

[2.2] character information
create table jscharinfo(
    charid int(11) NOT NULL DEFAULT 0 COMMENT 'id',
    charname varchar(64) NOT NULL DEFAULT 'JSystem' COMMENT 'name',
    gender int(1) NOT NULL DEFAULT 0 COMMENT 'gender, 0 means male, 1 means female',
    age int(3) NOT NULL DEFAULT 0 COMMENT 'age',
    locality varchar(32) NOT NULL DEFAULT 'CHINA' COMMENT 'locality',
    signature varchar(256) NOT NULL DEFAULT 'I WOULDD RATHER DO SOMETHING GREAT AND FAIL THAN DO NOTHING AND SUCCEED' COMMENT 'signature',
    grouplist varchar(1024) NOT NULL DEFAULT 'JSYSTEM' COMMENT 'join group list, like: 1_2_',
    PRIMARY KEY(charid)
)ENGINE=MyISAM DEFAULT CHARSET=utf8;


[2.3] friend information
create table jsfriendinfo(
    id int(11) NOT NULL AUTO_INCREMENT,
    charid int(11) NOT NULL DEFAULT 0,
    friendid int(1) NOT NULL DEFAULT 0,
    isdelete int(1) NOT NULL DEFAULT 0 COMMENT 'the flag of relation, 0 friend; 1 request; 2 delete',
    addtime int(11) NOT NULL DEFAULT 0 COMMENT 'time of being friendship',
    PRIMARY KEY(id)
)ENGINE=MyISAM DEFAULT CHARSET=utf8;

[2.4] group information
create table jsgroupinfo(
    groupid int(11) NOT NULL AUTO_INCREMENT,
    groupname varchar(64) NOT NULL DEFAULT 'JSYSTEM' COMMENT 'name',
    ownerid int(11) NOT NULL DEFAULT 0 COMMENT 'the owner id of the group',
    intro varchar(256) NOT NULL DEFAULT 'JSYSTEM' COMMENT 'introduction of the group',
    memberlist varchar(4096) NOT NULL DEFAULT 'JSYSTEM' COMMENT 'member list of the group, like: 1001_1002_',
    PRIMARY KEY(groupid)
)ENGINE=MyISAM DEFAULT CHARSET=utf8;


[3]
create database jslogin;


[3.1] gateway information
create table jsgatewayinfo(
    gatewayid int(11) NOT NULL AUTO_INCREMENT,
    gatewayip varchar(64) NOT NULL DEFAULT '127.0.0.1' COMMENT 'gateway ip address',
    gatewayport int(11) NOT NULL DEFAULT 6300 COMMENT 'gateway port',
    PRIMARY KEY(gatewayid)
)ENGINE=MyISAM DEFAULT CHARSET=utf8;









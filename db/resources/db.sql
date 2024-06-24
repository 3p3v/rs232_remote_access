-- Adminer 4.8.1 MySQL 8.0.33 dump

SET NAMES utf8;
SET time_zone = '+00:00';
SET foreign_key_checks = 0;
SET sql_mode = 'NO_AUTO_VALUE_ON_ZERO';

DROP TABLE IF EXISTS `access_types`;
CREATE TABLE `access_types` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(9) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  UNIQUE KEY `name_UNIQUE` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

INSERT INTO `access_types` (`id`, `name`) VALUES
(1,	'readonly'),
(4,	'subscribe'),
(2,	'writeonly');

DROP TABLE IF EXISTS `channel_types`;
CREATE TABLE `channel_types` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(1) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  UNIQUE KEY `type_name_UNIQUE` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

INSERT INTO `channel_types` (`id`, `name`) VALUES
(1,	'd'),
(2,	'i'),
(3,	's');

DROP TABLE IF EXISTS `devices`;
CREATE TABLE `devices` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(100) NOT NULL,
  `password` varchar(100) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  UNIQUE KEY `name_UNIQUE` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

INSERT INTO `devices` (`id`, `name`, `password`) VALUES
(1,	'B0:B2:1C:0B:15:CC',	'$2a$10$1.1CBKX3ilkLzxB4wNiMGuS7HhyGUTWLSa9M63pzgWwkT8YgrZFYG');

DROP TABLE IF EXISTS `user_acls`;
CREATE TABLE `user_acls` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `device_id` int unsigned NOT NULL,
  `user_id` int unsigned NOT NULL,
  `rw` int unsigned NOT NULL,
  `channel_type_id` int unsigned NOT NULL,
  PRIMARY KEY (`device_id`,`user_id`,`channel_type_id`,`rw`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  KEY `user_acls_device_id_fk_idx` (`device_id`),
  KEY `user_acls_user_id_fk_idx` (`user_id`),
  KEY `user_acls_channel_type_id_fk_idx` (`channel_type_id`),
  KEY `user_acls_access_types_id_fk_idx` (`rw`),
  CONSTRAINT `user_acls_access_types_id_fk` FOREIGN KEY (`rw`) REFERENCES `access_types` (`id`),
  CONSTRAINT `user_acls_channel_type_id_fk` FOREIGN KEY (`channel_type_id`) REFERENCES `channel_types` (`id`),
  CONSTRAINT `user_acls_device_id_fk` FOREIGN KEY (`device_id`) REFERENCES `devices` (`id`),
  CONSTRAINT `user_acls_user_id_fk` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

INSERT INTO `user_acls` (`id`, `device_id`, `user_id`, `rw`, `channel_type_id`) VALUES
(2,	1,	1,	1,	1),
(3,	1,	1,	2,	1),
(4,	1,	1,	4,	1),
(5,	1,	1,	1,	2),
(6,	1,	1,	2,	2),
(7,	1,	1,	4,	2),
(8,	1,	1,	1,	3),
(9,	1,	1,	2,	3),
(10,	1,	1,	4,	3);

DROP TABLE IF EXISTS `users`;
CREATE TABLE `users` (
  `id` int unsigned NOT NULL AUTO_INCREMENT,
  `username` varchar(100) NOT NULL,
  `password` varchar(100) NOT NULL,
  `is_admin` tinyint unsigned NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id_UNIQUE` (`id`),
  UNIQUE KEY `username_UNIQUE` (`username`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;

INSERT INTO `users` (`id`, `username`, `password`, `is_admin`) VALUES
(1,	'admin',	'$2a$10$1.1CBKX3ilkLzxB4wNiMGuS7HhyGUTWLSa9M63pzgWwkT8YgrZFYG',	1);

-- 2024-06-24 16:05:49

SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0;
SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0;
SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='TRADITIONAL';

DROP SCHEMA IF EXISTS gateway;
CREATE SCHEMA gateway;
USE gateway;

--
-- Table structure for table `end_device`
--设备表，end_id为自增主键，end_addr为zigbee节点的短地址，sensor_id为节点对应的传感器类型
--

CREATE TABLE end_device (
	end_id INT UNSIGNED AUTO_INCREMENT,
	end_addr INT UNSIGNED NOT NULL,
	sensor_id SMALLINT UNSIGNED NOT NULL,
	PRIMARY KEY (end_id),
	FOREIGN KEY (sensor_id) REFERENCES sensor(sensor_id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Table structure for table `sensor`
--传感器类型表，snesor_id为类型ID，sensor_name为传感器对应的中文名
--

CREATE TABLE sensor (
	sensor_id SMALLINT UNSIGNED NOT NULL,
	sensor_name VARCHAR(50) NOT NULL,
	PRIMARY KEY (sensor_id)
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Table structure for table `sensor_data`
--传感器数据表保存zigbee节点发送来的传感器数据，end_id是发送数据的zigbee节点
--data是传感器数据，set_time为数据发送的时间
--

CREATE TABLE sensor_data (
	sensor_id SMALLINT UNSIGNED NOT NULL,
	end_id INT UNSIGNED NOT NULL,
	data INT NOT NULL,
	set_time DATETIME NOT NULL,
	PRIMARY KEY (end_id, set_time),
	FOREIGN KEY (sensor_id) REFERENCES sensor(sensor_id),
	FOREIGN KEY (end_id) REFERENCES end_device(end_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--插入传感器类型
SET AUTOCOMMIT=0;
INSERT INTO sensor VALUES (1,'温度'),
(2,'湿度'),
(3,'光照强度'),
(4,'烟雾'),
(5,'磁感应'),
(6,'继电器');
COMMIT;









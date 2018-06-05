#ifndef __DATA_H
#define __DATA_H

typedef unsigned short uint16;
typedef unsigned char uint8;

#define MAX_BUF 1000
#define MAX_MESSAGE 30
//#define BUFFER_SIZE 200

extern struct message MESSAGE;
extern char write_buf[MAX_BUF];

//定义传感器类型的宏定义
#define LIGHT_SENSOR            0x01		//光敏
#define SMOKE_SENSOR            0x02		//烟雾
#define RELAY_SENSOR            0x03		//继电器
#define DHT11_SENSOR_TEM        0x04		//温度
#define DHT11_SENSOR_HU         0x05		//湿度
#define MAGNETISM_SENSOR        0x06		//磁感应

typedef struct 
{
  uint16     	End_ID;			//zigbee节点的短地址
  uint8     	sensor_type;	//节点的传感器类型
  uint8        	data[2];		//传感器的数据
} socket_type;

#endif

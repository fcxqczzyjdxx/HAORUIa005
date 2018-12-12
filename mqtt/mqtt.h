#ifndef _MQTT_H_
#define _MQTT_H_
#include "typedef.h"

#include "typedef.h"
#include "uart.h"
#include "sim800c.h"

#define MOSQ_MSB(A)         (uint8)((A & 0xFF00) >> 8)
#define MOSQ_LSB(A)         (uint8)(A & 0x00FF)


//建立连接结构体
typedef struct
{
  	c8	*_data;		//协议数据
	
	uint32	_len;		//写入的数据长度  数据总长度
	
	uint32	_size;		//缓存总大小  接下来的长度
  
  uint8  type ;  //连接类型  1
  uint8  remainedLength;  //接下来的数据长度 
  uint16 protocolNameLength; //连接类型名的长度 4
  uint8  protocolName[4];  //MQTT
  uint8  protocolLevel; //4
  uint8  connectFlag;   //192
  uint16 keepAlive;     //120  //连接时间 
  
  uint16 deviceIdLen  ;  //设备ID长度
  uint8 *deviceId;
  
  uint16 productIdLen;  //product ID
  uint8 * productId;
  
  uint16 authInfoLen;  //鉴权信息长度
  uint8 * authInfo;
  
}MQTT_CONNECT_TYPE;


extern  MQTT_CONNECT_TYPE mqttConnect ;
extern  c8 mqttConnectBuf[100];
extern state_type mqttConnectFlag;  ///mqtt连接标志位  1是连接


//发送数据结构体
typedef struct
{
  c8	*_data;		//协议数据
  uint32	_len;		//写入的数据长度  数据总长度
	uint32	_size;		//缓存总大小  接下来的长度
}MQTT_DATA_TYPE;


extern MQTT_DATA_TYPE  mqttData;
extern c8 mqttDataBuf[1024];



state_type MQTT_PacketConnect (const int8 *deviceId,
                                const int8 *productId, 
                                const int8 *authInfo, 
                                const uint16 cTime);

                                
state_type MQTT_PacketData(c8 *buf ,uint32 len);
#endif

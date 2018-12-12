#include "mqtt.h"
#include "typedef.h"
#include "uart.h"
#include "sim800c.h"
#include "delay.h"

//C库
#include <string.h>

MQTT_CONNECT_TYPE  mqttConnect ;
c8 mqttConnectBuf[100];
state_type mqttConnectFlag=0;  ///mqtt连接标志位  1是连接


/*
参数1  product ID
参数2  鉴权信息
参数3  设备ID
参数4  连接时间
返回 1 连接成功
*/

state_type MQTT_PacketConnect (const int8 *deviceId,
                                const int8 *productId, 
                                const int8 *authInfo, 
                                const uint16 cTime)
{
	uint8 * p1;  //接收到的数据
	uint16 a=10;  //计时
	size_t  total_len=0; //总长度

	mqttConnectFlag=0;

	mqttConnect.type=0x10;
	mqttConnect.protocolNameLength=4;
	mqttConnect.protocolName[0]='M';
	mqttConnect.protocolName[1]='Q';
	mqttConnect.protocolName[2]='T';
	mqttConnect.protocolName[3]='T';
	mqttConnect.protocolLevel=4;

	total_len +=16; //len2+neam+flag+keep+len+len+len

	total_len+= strlen(deviceId);
	total_len+= strlen(productId);
	total_len+= strlen(authInfo);

	memset(mqttConnectBuf,0,100);

	mqttConnect._data=mqttConnectBuf;
	mqttConnect._len=0;
	mqttConnect._size=0;


	mqttConnect._data[mqttConnect._len++]=mqttConnect.type;  //连接类型
	mqttConnect._data[mqttConnect._len++]= total_len;// mqttConnect.remainedLength;  //数据长度
	mqttConnect._data[mqttConnect._len++]=0;  //连接类型名字长度
	mqttConnect._data[mqttConnect._len++]=4;  //连接类型名字长度
	mqttConnect._data[mqttConnect._len++] = 'M';
	mqttConnect._data[mqttConnect._len++] = 'Q';
	mqttConnect._data[mqttConnect._len++] = 'T';
	mqttConnect._data[mqttConnect._len++] = 'T';

	mqttConnect._data[mqttConnect._len++] = 4; //
	mqttConnect._data[mqttConnect._len++]= 192; //0xc0  connectFlag
	mqttConnect._data[mqttConnect._len++]=MOSQ_MSB(cTime);  //连接时间
	mqttConnect._data[mqttConnect._len++]=MOSQ_LSB(cTime); 

	mqttConnect._data[mqttConnect._len++]=MOSQ_MSB(strlen(deviceId));
	mqttConnect._data[mqttConnect._len++]=MOSQ_LSB(strlen(deviceId));
	strncat((int8 *)mqttConnect._data + mqttConnect._len, deviceId, strlen(deviceId));
	mqttConnect._len += strlen(deviceId);

	mqttConnect._data[mqttConnect._len++]=MOSQ_MSB(strlen(productId));
	mqttConnect._data[mqttConnect._len++]=MOSQ_LSB(strlen(productId));
	strncat((int8 *)mqttConnect._data + mqttConnect._len, productId, strlen(productId));
	mqttConnect._len += strlen(productId);

	mqttConnect._data[mqttConnect._len++]=MOSQ_MSB(strlen(authInfo));
	mqttConnect._data[mqttConnect._len++]=MOSQ_LSB(strlen(authInfo));
	strncat((int8 *)mqttConnect._data + mqttConnect._len, authInfo, strlen(authInfo));
	mqttConnect._len += strlen(authInfo);

	mqttConnect._size=total_len;

	COM_print(COM1 ,"\r\n显示要发送的数据\r\n");
	for(int i=0;i<mqttConnect._len;i++)
	COM_print(COM1 ,"0x%02x ",mqttConnect._data[i]);

	mqttConnectFlag=sim800c_SendData(mqttConnect._data,mqttConnect._len);
	if(mqttConnectFlag==0)
	{
		mqttConnectFlag=0;
		return 0;
	}
	else if(mqttConnectFlag==2)
	{
		mqttConnectFlag=0;
		return 2;
	}
	COM_print(COM1 ,"\r\n连接数据已经发送\r\n");
	//COM_Read(COM3,&sim800cBuf);
	//	sim800c_SendCmd("","END",0);
	//while(!sim800c_SendCmd("","END",10))
	while(!sim800c_WaitRecive()&& a--)  //等待发送结束标志
	{
		delay_100ms(10);
		COM_print(COM1 ,"\r\n发送未结束1\r\n ");
	}
	a=10;
	COM_print(COM1 ,"\r\n发送结束1\r\n ");
//	if(sim800cBuf.RxNum)
//	{
//		 for(int i=0;i< sim800cBuf.RxNum;i++)
//		 COM_print(COM1 ,"\r\n接收到数据 %0x\r\n",sim800cBuf.RxBuf[i]);
		
		p1=(uint8_t*)strstr((const char*)sim800cBuf.RxBuf,"+IPD");//等待平台返回连接成功数据
		if(!p1)
		{
			 while(!sim800c_WaitRecive()&& a--)  //等待平台返回连接成功数据
			{
				delay_100ms(10);
				COM_print(COM1 ,"\r\n返回数据\r\n ");
			} 
			p1=(uint8_t*)strstr((const char*)sim800cBuf.RxBuf,"+IPD");
		}
	 
		//COM_print(COM1 ,"\r\n数据地址  %s\r\n",p1);
		if(p1)
		{
			p1=(uint8_t*)strstr((const char*)sim800cBuf.RxBuf,":");
			p1++;
			//COM_print(COM1 ,"\r\n打印接收到的数据 \r\n");
			if(p1[0]==0x20 && p1[1]==0x02 && p1[2]==0x00 && p1[3]==0x00)  //0x20 0x02 0x00 0x00 
			{ 
				COM_print(COM1 ,"\r\n登陆平台成功\r\n");
				mqttConnectFlag=1;
			}
			else 
			{
				COM_print(COM1 ,"\r\n登陆平台未成功\r\n");
				mqttConnectFlag=0;
			}
//			for(int i=0;i< 4;i++)
//			COM_print(COM1 ,"0x%02x ",p1[i]);
//			mqttConnectFlag=1;
//		}
		
	}

	return mqttConnectFlag;
}

//计算数据和不固定头长度
int32 MQTT_DumpLength(size_t len, c8 *buf)
{

	int32 i = 0;

	for(i = 1; i <= 4; ++i)
	{
		*buf = len % 128;
		len >>= 7;
		if(len > 0)
		{
			*buf |= 128;
			++buf;
		}
		else
		{
			return i;
		}
	}

	return -1;
}

MQTT_DATA_TYPE  mqttData;
c8 mqttDataBuf[1024];
//state_type mqttConnectFlag=0;  ///mqtt连接标志位  1是连接
state_type MQTT_PacketData(c8 *buf ,uint32 len)
{
	uint32 total_len=0;  //除固定头外的数据长度,,包括 "$dp"
	uint32 buf_len =0;
	c8 topic[]="$dp";
	uint32 topic_len=0 ;

	buf_len=strlen(buf);
	topic_len=strlen(topic);



	total_len = 5 + 1+2+buf_len;  //00 03 $dp  + 03+ buflen 的大小 +buflen
	memset(mqttDataBuf,0,1024);
	mqttData._data = mqttDataBuf;
	//  memset(mqttData._data,0,strlen(mqttDataBuf));
	mqttData._len=0;

	/*************************************固定头部***********************************************/
	mqttData._data[mqttData._len++]=0x30; //固定开头
	mqttData._len+=  MQTT_DumpLength(total_len, mqttData._data+mqttData._len);
	/*************************************可变头部***********************************************/

	mqttData._data[mqttData._len++]=MOSQ_MSB(topic_len);
	mqttData._data[mqttData._len++]=MOSQ_LSB(topic_len);
	strncat((int8 *)mqttData._data + mqttData._len, topic, topic_len);
	mqttData._len += topic_len;

	mqttData._data[mqttData._len++]=0x03;

	mqttData._data[mqttData._len++]=MOSQ_MSB(buf_len);
	mqttData._data[mqttData._len++]=MOSQ_LSB(buf_len);

	COM_print(COM1 ,"\r\n%s\r\n",buf);
	strncat((int8 *)mqttData._data + mqttData._len, buf, buf_len);
	mqttData._len += buf_len;

	COM_print(COM1 ,"\r\n 发送的数据\r\n " );
	for(int i=0; i<mqttData._len;i++)
	{
		COM_print(COM1 ,"0x%02X ",mqttData._data[i]);
	}

	return 2;
}








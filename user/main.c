/*******************************************************************************
文 件 名: main.c
功    能: 主程序
创建日期: 
作    者:
邮    箱:
详    细: 
********************************************************************************
注意事项
*******************************************************************************/

#include "system.h"
#include "stm32f10x.h"
#include "delay.h"
#include "timer.h"
#include "uart.h"
#include "my_lib.h"
#include "out_led.h"
#include "out_relay.h"
#include "in_photocoupler.h"
#include "in_adc.h"
#include "sim800c.h"
#include "config.h"
#include "mqtt.h"
#include "onenet.h"

COM_RxTypeDef Com_Data;
c8 buf[100];
c8 dataUploadSoData[2048]; //要上传的数据
state_type  dataGatherFlag=0; //数据采集成功标准位,,采集成功是1,,其他是不成功
#define DATA_GATHER_BEGIN  		0000
#define DATA_GATHER_ADC_1 		1001
#define DATA_GATHER_ADC_2 		1002
#define DATA_GATHER_ADC_3 		1003
#define DATA_GATHER_ADC_4 		1004
#define DATA_GATHER_DIN_1 		2001
#define DATA_GATHER_DIN_2     2002
#define DATA_GATHER_DIN_3     2003
#define DATA_GATHER_DIN_4     2004
#define DATA_GATHER_DOUT_1    3001
#define DATA_GATHER_DOUT_2    3002
#define DATA_GATHER_DOUT_3    3003
#define DATA_GATHER_DOUT_4    3004
#define DATA_GATHER_MODBUS_1  4001
#define DATA_GATHER_MODBUS_2  4002
#define DATA_GATHER_MODBUS_3  4003
#define DATA_GATHER_MODBUS_4  4004
#define DATA_GATHER_EDN				0001

void dataGather(void);

/*******************************************************************************
函数名字: int main()
函数功能: 
创建时间: 
作    者: 
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/
int main()
{
	static _Bool cnfMode=0;
	unsigned short timeCount = 0;	//发送间隔变量

	u8 i=0;
	c8 disconnect[]={0x0e,0x00};  //断开连接
	delay_init();
	delay_ms(1000);
	TIM3_Init();
	UART_Init(COM1 ,115200);
	UART_Init(COM3 ,115200); //SIM800C
	/*设置NVIC中断分组2:2位抢占优先级，2位响应优先级*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 

	LED_Init(LED1);
	LED_Init(LED2);
	LED_Init(LED3);

	RLY_Init(RLY1);
	RLY_Init(RLY2);
	RLY_Init(RLY3);
	RLY_Init(RLY4);

	PTCP_Init(PTCP1);
	LED_Off(LED1);
	LED_On(LED2);
	LED_Off(LED3); 
	RLY_OpenOff(RLY1);
	RLY_OpenOff(RLY2);

	AD_DMA_Init();
	CNF_Init();
	//  sim800c_Init();
		
/*主循环*/
  while(1)
  {
      
   // sim800c_SendData("HRXXKJ",6);
		//ADC_ConvertedToCurrent
		#if(1)
    dataGather();
		#endif
		#if(1)
    if(COM_Read(COM1,&Com_Data))
    {

      
      cnfMode=CNF_mode(&Com_Data);
      
      if(cnfMode)//进入配置模式
      {
        CNF_Data(&Com_Data , Com_Data.RxBuf);/*读取写入参数*/
				
      }
      else  //非配置模式接收到的数据
      {
      
      }
      memset(&Com_Data,0,sizeof(Com_Data));/*清空接收*/
    }
   
    if(cnfMode==0)  //非配置模式下运行的程序
    {
      if(sim800cInterFlag==1)  //连接成功是发送数据
      {
        if(mqttConnectFlag==0)  //平台是否登陆,,,没有登陆进行登陆
        {
					//COM_print(COM1 ,"登陆状态: mqttConnectFlag %d\r\n",mqttConnectFlag);
          if(MQTT_PacketConnect(DEVICE_ID,PRODUCT_ID,AUTH_INFO,300)==2)
          {
              sim800c_SendData(disconnect,2);
              mqttConnectFlag=0;
              sim800cInterFlag=0;
          }
        }
        else if(mqttConnectFlag ==1)//已经登陆  可以发送数据
        {
					//COM_print(COM1 ,"登陆状态: mqttConnectFlag %d\r\n",mqttConnectFlag);
					
					++timeCount;
          if(timeCount >= 500 )									//发送间隔1s
          {
						//COM_print(COM1 ,"计时 timeCount:%d\r\n",timeCount);
						timeCount = 0;
						if(dataGatherFlag==1)
						{
							//COM_print(COM1 ,"采集状态  dataGatherFlag:%d\r\n",dataGatherFlag);
							dataGatherFlag=0;
							//sprintf(buf,"{\"data\":\"%d\"}",i);++i;
							
							//MQTT_PacketData( buf,1 );
							 MQTT_PacketData( dataUploadSoData,1 );
							if(sim800c_SendData(mqttData._data,mqttData._len)==2)  //如果连接被断开
							{
								sim800c_SendData(disconnect,2);
								mqttConnectFlag=0;
								sim800cInterFlag=0;
							}
						}
						
            //memset(buf,0,strlen(buf));
          }
					//COM_print(COM1 ,"计时 timeCount:%d\r\n",timeCount);
          delay_ms(10);
					sim800c_GetIPD(1);
					
        }
 
      }
      else //没有链接或正在连接时候 初始化连接
      {
         sim800cInterFlag=sim800c_Init();
      }
    }
		#endif

  }
}


void dataGather(void)
{
	static u16 step=DATA_GATHER_BEGIN; //记录采集的步骤
	static c8 amongSoData[2048]; //中间数组,,采集的数据存到这个里面,所有数据采集完后复制到上传数组里面
	c8 data[100];

	switch(step)
	{
		case DATA_GATHER_BEGIN:  //初始化
			memset(amongSoData,0,1024);
		  sprintf(data,"{");
		  strcat(amongSoData, data);
			step=DATA_GATHER_ADC_1;
			break;
		case DATA_GATHER_ADC_1:
				if(gtp_CnfTyp->AnalogInInfo[0].Flag==1 )//使能
				{
					if(gtp_CnfTyp->AnalogInInfo[0].Compute==0)
					{
						sprintf(data,"\"ADC_1\":\"%d\"",ADC1_ConvertedToCurrent());
					}
					else if(gtp_CnfTyp->AnalogInInfo[0].Compute==1)
					{
						sprintf(data,"\"ADC_1\":\"%f\"",(((float)ADC1_ConvertedToCurrent())/4096*33));
						//COM_print(COM1 ,"ADC_1data测试:%s\r\n",data);
					}
					strcat(amongSoData, data);
					strcat(amongSoData, ",");
					
				}
//				else
//				{
//					sprintf(data,"{\"ADC_1\":\"0\"}");
//				}
				//COM_print(COM1 ,"ADC_1测试:%s\r\n",amongSoData);
				step=DATA_GATHER_ADC_2;
			break;
				
		case DATA_GATHER_ADC_2:
				if(gtp_CnfTyp->AnalogInInfo[1].Flag==1 )//使能
				{
					if(gtp_CnfTyp->AnalogInInfo[1].Compute==0)
					{
						sprintf(data,"\"ADC_2\":\"%d\"",ADC2_ConvertedToCurrent());
					}
					else if(gtp_CnfTyp->AnalogInInfo[1].Compute==1)
					{
						sprintf(data,"\"ADC_2\":\"%f\"",((float)ADC2_ConvertedToCurrent())/4096*33);
					}
					strcat(amongSoData, data);
					strcat(amongSoData, ",");
				}
//				else
//				{
//					sprintf(data,"{\"ADC_2\":\"0\"}");
//				}
				//COM_print(COM1 ,"ADC_2测试:%s\r\n",amongSoData);
				step=DATA_GATHER_ADC_3;
			break;
		case DATA_GATHER_ADC_3:
				if(gtp_CnfTyp->AnalogInInfo[2].Flag==1 )//使能
				{
					if(gtp_CnfTyp->AnalogInInfo[2].Compute==0)
					{
						sprintf(data,"\"ADC_3\":\"%d\"",ADC3_ConvertedToCurrent());
					}
					else if(gtp_CnfTyp->AnalogInInfo[2].Compute==1)
					{
						sprintf(data,"\"ADC_3\":\"%f\"",((float)ADC3_ConvertedToCurrent())/4096*33);
					}
					strcat(amongSoData, data);
					strcat(amongSoData, ",");
				}
//				else
//				{
//					sprintf(data,"\"ADC_3\":\"0\"");
//				}
				//COM_print(COM1 ,"ADC_2测试:%s\r\n",amongSoData);
				step=DATA_GATHER_ADC_4;
			break;
		case DATA_GATHER_ADC_4:
				if(gtp_CnfTyp->AnalogInInfo[3].Flag==1 )//使能
				{
					if(gtp_CnfTyp->AnalogInInfo[3].Compute==0)
					{
						sprintf(data,"\"ADC_4\":\"%d\"",ADC4_ConvertedToCurrent());
					}
					else if(gtp_CnfTyp->AnalogInInfo[3].Compute==1)
					{
						sprintf(data,"\"ADC_4\":\"%f\"",((float)ADC4_ConvertedToCurrent())/4096*33);
					}
					strcat(amongSoData, data);
					strcat(amongSoData, ",");
				}
//				else
//				{
//					sprintf(data,"\"ADC_4\":\"0\"");
//				}
				//COM_print(COM1 ,"ADC_2测试:%s\r\n",amongSoData);
				step=DATA_GATHER_DIN_1;
			break;
		case DATA_GATHER_DIN_1:
			if(gtp_CnfTyp->DigitalInInfo[0].Flag ==1 )
			{
				sprintf(data,"\"DIN_1\":\"%d\"",PTCP_GetStatus(PTCP1));
				strcat(amongSoData, data);
				strcat(amongSoData, ",");
			}
			step=DATA_GATHER_DIN_2;
			break;
		case DATA_GATHER_DIN_2:
			if(gtp_CnfTyp->DigitalInInfo[1].Flag ==1 )
			{
				sprintf(data,"\"DIN_2\":\"%d\"",PTCP_GetStatus(PTCP2));
				strcat(amongSoData, data);
				strcat(amongSoData, ",");
			}
			step=DATA_GATHER_DIN_3;
			break;
		case DATA_GATHER_DIN_3:
			if(gtp_CnfTyp->DigitalInInfo[2].Flag ==1 )
			{
				sprintf(data,"\"DIN_3\":\"%d\"",PTCP_GetStatus(PTCP3));
				strcat(amongSoData, data);
				strcat(amongSoData, ",");
			}
			step=DATA_GATHER_DIN_4;
			break;
		case DATA_GATHER_DIN_4:
			if(gtp_CnfTyp->DigitalInInfo[3].Flag ==1 )
			{
				sprintf(data,"\"DIN_4\":\"%d\"",PTCP_GetStatus(PTCP4));
				strcat(amongSoData, data);
				strcat(amongSoData, ",");
			}
			step=DATA_GATHER_DOUT_1;
			break;
		case DATA_GATHER_DOUT_1:
			if(gtp_CnfTyp->DigitalOutInfo[0].Flag ==1 )
			{
				RLY_CloseOn(RLY1);
				sprintf(data,"\"DOUT_1\":\"%d\"",RLY_OpenStatus(RLY1));
				strcat(amongSoData, data);
				strcat(amongSoData, ",");
			}
			step=DATA_GATHER_DOUT_2;
			break;
		case DATA_GATHER_DOUT_2:
			if(gtp_CnfTyp->DigitalOutInfo[1].Flag ==1 )
			{
				RLY_CloseOn(RLY2);
				sprintf(data,"\"DOUT_2\":\"%d\"",RLY_OpenStatus(RLY2));
				strcat(amongSoData, data);
				strcat(amongSoData, ",");
			}
			step=DATA_GATHER_DOUT_3;
			break;
		case DATA_GATHER_DOUT_3:
			if(gtp_CnfTyp->DigitalOutInfo[2].Flag ==1 )
			{
				sprintf(data,"\"DOUT_3\":\"%d\"",RLY_OpenStatus(RLY3));
				strcat(amongSoData, data);
				strcat(amongSoData, ",");
			}
			step=DATA_GATHER_DOUT_4;
			break;
		case DATA_GATHER_DOUT_4:
			if(gtp_CnfTyp->DigitalOutInfo[3].Flag ==1 )
			{
				sprintf(data,"\"DOUT_4\":\"%d\"",RLY_OpenStatus(RLY4));
				strcat(amongSoData, data);
				strcat(amongSoData, ",");
			}
			step=DATA_GATHER_EDN;
			break;
		case DATA_GATHER_MODBUS_1:
			break;
		case DATA_GATHER_MODBUS_2:
			break;
		case DATA_GATHER_MODBUS_3:
			break;
		case DATA_GATHER_MODBUS_4:
			break;
		case DATA_GATHER_EDN:
			
			if (amongSoData[strlen(amongSoData) - 1] == ',')  //最后一个字符是 ',' 删除改成 '\0'
				amongSoData[strlen(amongSoData) - 1]  = '\0';
			sprintf(data,"}");
		  strcat(amongSoData, data);
			if(strcpy(dataUploadSoData,amongSoData))
			{
//			 	COM_print(COM1 ,"END测试:%s\r\n",dataUploadSoData);
//			  COM_print(COM1 ,"END测试:%d\r\n",strlen(dataUploadSoData) );
				dataGatherFlag=1;
			}
		  
			memset(amongSoData,0,1024);
			step=DATA_GATHER_BEGIN;
			break;
		default :  //意外情况,,返回初始化
			break;
	}
//  COM_print(COM1 ,"测试:%s\r\n",dataUploadSoData);
//	COM_print(COM1 ,"测试:%d\r\n",strlen(dataUploadSoData) );
}








#if(0)  /*2018年9月13日之前写的*/
COM_RxTypeDef com1;
uint8_t buf[500]={0x01,0x02,0x03,0x04,0x05,0x06,0xaa};
uint8_t  abc=0;
int main()
{
  delay_init();
  delay_ms(1000);
  TIM3_Init();
  UART_Init(COM1 ,115200);
  /*设置NVIC中断分组2:2位抢占优先级，2位响应优先级*/
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
  
  LED_Init(LED1);
  LED_Init(LED2);
  LED_Init(LED3);
  
  RLY_Init(RLY1);

  PTCP_Init(PTCP1);
  LED_Off(LED1);
  LED_On(LED2);
  LED_Off(LED3); 
  RLY_OpenOff(RLY1);
  RLY_OpenOn(RLY2);
  
  AD_DMA_Init();

  while(1)
  {
    ADC_ConvertedToCurrent();
    /*每次接收数据前需要清空com1  防止 历史数据干扰*/
     memset( &com1, 0, sizeof(com1)); 
    if(COM_Read(COM1,&com1))
    {
      //COM_Write(COM1,buf,7);
      if(com1.RxBuf[0]=='1')
      {
        RLY_CloseOn(RLY1);
        COM_print(COM1,"常闭 闭合\r\n");
        COM_print(COM1,"常闭继电器状态 : %d\r\n\r\n",RLY_CloseStatus(RLY1));
        LED_On(LED1);
      }
      if(com1.RxBuf[0]=='2')
      {
        RLY_CloseOff(RLY1);
        COM_print(COM1,"常闭 断开\r\n");
        COM_print(COM1,"常闭继电器状态 : %d\r\n\r\n",RLY_CloseStatus(RLY1));
        LED_Off(LED1);
      }
      if(com1.RxBuf[0]=='3')
      {
        RLY_OpenOn(RLY1);
        COM_print(COM1,"常开 闭合\r\n");
        COM_print(COM1,"常开继电器状态 : %d\r\n\r\n",RLY_OpenStatus(RLY1));
        LED_Off(LED1);
      }
      if(com1.RxBuf[0]=='4')
      {
        RLY_OpenOff(RLY1);
        COM_print(COM1,"常开 断开\r\n");
        COM_print(COM1,"常开继电器状态 : %d\r\n\r\n",RLY_OpenStatus(RLY1));
        LED_Off(LED1);
      }
      if(com1.RxBuf[0]=='5')
      {
        COM_print(COM1,"AD1 = %f,AD2 = %f,AD3 = %f,AD4 = %f\r\n\r\n",
        gfv_ADCConvertedCurrent[0],gfv_ADCConvertedCurrent[1],gfv_ADCConvertedCurrent[2],
        gfv_ADCConvertedCurrent[3]);
      }
      
    }
  }
}
#endif




//断言调试,在串口上打印错误
void assert_failed(uint8_t* file, uint32_t line)
{ /* User can add his own implementation to report the file name and linenumber, 
ex: */ 
  COM_print(COM1,"Wrong parameters value: file %s on line %d\r\n", file,line) ;
  //printf("Wrong parameters value: file %s on line %d\r\n", file,line) ;
/* Infinite loop */ 
while (1) { } 
}


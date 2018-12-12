#include "sim800c.h"
#include "uart.h"
#include "delay.h"
#include "config.h"
#include "stm32f10x_conf.h"

#include "stm32f10x.h"
//#include "config.h"

//#include "bsp_timer.h"
//#include "my_lib.h"

COM_RxTypeDef sim800cBuf;
state_type sim800cInterFlag=0;

/*
--清空缓存
--
*/
void sim800c_Clear(void)
{
  sim800cBuf.RxNum=0;
  memset(sim800cBuf.RxBuf, 0, sizeof(sim800cBuf.RxBuf));
}


/*
-- 查看是否接收到数据 等待接收完成
- -FALSE-接收未完成		TRUE-接收完成
*/
state_type sim800c_WaitRecive(void)
{
  return COM_Read(COM3,&sim800cBuf);  //读取sim800c返回的数据  //
}



//==========================================================
//	函数名称：sim800c_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//       
//	返回参数：	0 未成功  1成功   
//
//	说明：		
//==========================================================
state_type sim800c_SendCmd(char *cmd, char *res,uint32_t timeOut)
{
	
	unsigned char num = 5;

	COM_Write(COM3, (c8*)cmd, strlen(cmd));
	
	while(num--)
	{
    delay_100ms(timeOut);
		if(sim800c_WaitRecive() ==TRUE )							//如果收到数据
		{
      COM_print(COM1 ,"%s\r\n",sim800cBuf.RxBuf);
			if(strstr((const char *)sim800cBuf.RxBuf, res) != NULL)		//如果检索到关键词
			{
        
				  sim800c_Clear();										//清空缓存
				
				return 1;
			}
		}
		
	}
	
	return 0;
}

//==========================================================
//	函数名称：	sim800c_SendData
//
//	函数功能：	GPRS发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
state_type sim800c_SendData(c8 *data, unsigned short len)
{

	char cmdBuf[32];
	static c8 flag=0;  //多次没发送成功就重新链接
  state_type aa=0;
  
  if(sim800cInterFlag==1 && flag<10)
  {
    
    sim800c_Clear();								//清空接收缓存
    COM_print(COM1 ,"AT+CIPSEND= 发送数据个数 %d\r\n",len);
    sprintf(cmdBuf, "AT+CIPSEND=%d\r\n",len);	//发送命令
    //sprintf(cmdBuf, "AT+IPSEND=%d\r\n", len);	//发送命令  带数据长度
    
    if(sim800c_SendCmd(cmdBuf, ">",30))				//收到‘>’时可以发送数据
    {
      flag=0;
      //发送设备连接请求数据
      COM_Write(COM3, (c8*)data, len);
      //delay_ms(10);						//必须加延时
       //sim800c_SendCmd((u8*)0X1A,0) ;       //CTRL+Z,结束数据发送,启动一次传输
//      if(sim800c_SendCmd("", "END",30))
//      {
//        COM_print(COM1 ,"发送成功\r\n");
//      }
       COM_print(COM1 ,"发送成功\r\n");
      aa=1;
    }
    else
    {
      flag++;
      COM_print(COM1 ,"发送失败 %d 次\r\n",flag);
      if(strstr((const char *)sim800cBuf.RxBuf, "CLOSED")==NULL)
      {
        COM_print(COM1 ,"\r\n%s\r\n",sim800cBuf.RxBuf);
        aa=2;  //连接断开
      }
      if(flag==5)
      {
        COM_print(COM1 ,"\r\nAT+CIPCLOSE 关闭连接\r\n");
        if(sim800c_SendCmd("AT+CIPCLOSE\r\n","OK",20))
        {
          COM_print(COM1 ,"\r\nAT+CIPCLOSE 关闭连接 成功\r\n");
        }
        COM_print(COM1 ,"\r\nAT+CIPSHUT 关闭移动场景\r\n");
        if(sim800c_SendCmd("AT+CIPSHUT\r\n","OK",20))
        {
          COM_print(COM1 ,"\r\nAT+CIPSHUT 关闭移动场景 成功\r\n");
        }
        aa=2;
        flag=0;
        sim800cInterFlag=0;
      }
    }
	}
  else
  {
    flag=0;
    sim800cInterFlag=0;
  }
  memset(cmdBuf,0,sizeof(cmdBuf));
  return aa;
}

//==========================================================
//	函数名称：	sim800c_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	timeOut等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//==========================================================
unsigned char *sim800c_GetIPD(unsigned short timeOut)
{
	  if(COM_Read(COM3,&sim800cBuf)) //读取sim800c返回的数据  //
			COM_print(COM1 ,"接收到数据 %s\r\n",sim800cBuf.RxBuf );
}


//==========================================================
//	函数名称：	sim800c_Init
//
//	函数功能：	初始化M6312
//
//	入口参数：	无
//
//	返回参数：	0 未成功  1成功     其他 正在连接
//
//	说明：		
//==========================================================
state_type sim800c_Init(void)
{
  static uint8_t i=5; //发送多次命令
  char Buf[100];
  static uint8_t initStep=0;  //步骤
  state_type flag=2;  //要返回的状态
  
  sim800c_Clear();  //清空接收缓存
   memset(Buf,0,sizeof(Buf));
  //sim800cInterFlag=0;  //状态
  
  
  switch (initStep)
  {
    case 0:
         //COM_print(COM1 ,"\r\n0.复位sim800c \r\n");
         initStep=1; //进入下一步
         i=5;
      break;
    case 1:   
      COM_print(COM1 ,"\r\n1.AT\r\n");
      if(!sim800c_SendCmd("AT\r\n","OK",5))
      {
         if(!--i) //多次没有对指令返回 从新开始
        {
          COM_print(COM1 ,"\r\n1.AT 超时\r\n");//超时 返回未成功
        }  
      }
      else if(i) 
      {
        sim800c_SendCmd("ATE0\r\n","OK",10);
        i=5; initStep=2;//进入下一步
        COM_print(COM1 ,"1.AT 成功 \r\n");
        delay_1000ms(10);
        if(sim800c_WaitRecive() ==TRUE )							//如果收到数据
        {
            sim800c_SendCmd("","SMS Ready",0);
            COM_print(COM1 ,"%s\r\n",sim800cBuf.RxBuf);
            sim800c_Clear();										//清空缓存
        }
      }
      break;
    case 2:  
        COM_print(COM1 ,"\r\n2.AT+CPIN? 检查sim状态\r\n");
        if(!sim800c_SendCmd("AT+CPIN?\r\n","OK",5))
        {
           if(!--i) //多次没有对指令返回 从新开始
           {
             COM_print(COM1 ,"\r\n2.AT+CPIN? 检查sim状态 失败\r\n");//超时 返回未成功
           }  
        }
        else if(i)
        {
          i=5; initStep=3;COM_print(COM1 ,"\r\n2.AT+CPIN? 检查sim状态 成功\r\n");
        }
        
      break;
    case 3 :
      COM_print(COM1 ,"\r\n3.AT+CIPCLOSE 关闭连接\r\n");
      if(sim800c_SendCmd("AT+CIPCLOSE\r\n","OK",20))
      {
        COM_print(COM1 ,"\r\n3.AT+CIPCLOSE 关闭连接 成功\r\n");
      }
      i=5; initStep=4;
      break;
    case 4:
      COM_print(COM1 ,"\r\n4.AT+CIPSHUT 关闭移动场景\r\n");
      if(sim800c_SendCmd("AT+CIPSHUT\r\n","OK",20))
      {
        COM_print(COM1 ,"\r\n4.AT+CIPSHUT 关闭移动场景 成功\r\n");
      }
      i=5; initStep=5;
      break;
    case 5:
      COM_print(COM1 ,"\r\n5.AT+CGCLASS=\"B\" 设置GPRS移动台类别为B\r\n");
//    sim800c_SendCmd("AT+CGCLASS=\"B\"\r\n","OK");
      if(!sim800c_SendCmd("AT+CGCLASS=\"B\"\r\n","OK",60))
      { 
        if(!--i) 
        {
          COM_print(COM1 ,"\r\n5.AT+CGCLASS=\"B\" 设置GPRS移动台类别为B 失败\r\n");//超时 返回未成功
        }
      }
      else if(i)
      {
        i=5; initStep=6;COM_print(COM1 ,"\r\n5.AT+CGCLASS=\"B\" 设置GPRS移动台类别为B 成功\r\n");
      }
      break;
    case 6:
      COM_print(COM1 ,"\r\n6.AT+CGDCONT=1,\"IP\",\"CMNET\"  设置PDP上下文,互联网接协议,接入点等信息  \r\n");
      if(! sim800c_SendCmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n","OK",40) )//设置PDP上下文,互联网接协议,接入点等信息
      {
        if(!--i) 
        { COM_print(COM1 ,"\r\n6.AT+CGDCONT=1,\"IP\",\"CMNET\"失败\r\n"); }  //超时 返回未成功
      }
      else if(i)
      {
        i=5; initStep=7;COM_print(COM1 ,"6.AT+CGDCONT=1,\"IP\",\"CMNET\"成功\r\n\r\n");
      }
      break;
    case 7:
      COM_print(COM1 ,"\r\n7.AT+CGATT=1 附着GPRS业务\r\n");
      if(sim800c_SendCmd("AT+CGATT=1\r\n","OK",30))
      {
        COM_print(COM1 ,"\r\n7.AT+CGATT=1 附着GPRS业务 成功\r\n");
      }
      i=5; initStep=8;
      
//     if(!sim800c_SendCmd("AT+CGATT=1\r\n","OK",70))//附着GPRS业务
//     {
//      if(!--i) {  COM_print(COM1 ,"\r\n7.AT+CGATT=1 附着GPRS业务 失败\r\n");return FALSE; }  //超时 返回未成功
//     }
//     else if(i)
//     {
//        i=5; initStep=8;COM_print(COM1 ,"\r\n7.AT+CGATT=1 附着GPRS业务 成功\r\n");
//     }
      break;
    case 8:
      COM_print(COM1 ,"\r\n8.AT+CIPCSGP=1,\"CMNET\" 设置模块连接方式为GPRS链接方式，接入点为“CMNET”（对于移动和联通一样\r\n");
      if(! sim800c_SendCmd("AT+CIPCSGP=1,\"CMNET\"\r\n","OK",20) )// 
      {
        if(!--i) 
        { COM_print(COM1 ,"\r\n8.AT+CIPCSGP=1,\"CMNET\" 设置模块连接方式为GPRS链接方式，接入点为“CMNET”（对于移动和联通一样 失败\r\n"); }  //超时 返回未成功
      }
      else if(i)
      {
        i=5; initStep=9;COM_print(COM1 ,"8.AT+CIPCSGP=1,\"CMNET\" 设置模块连接方式为GPRS链接方式，接入点为“CMNET”（对于移动和联通一样) 成功\r\n");
      }
      break;
    case 9:
      COM_print(COM1 ,"\r\n9.AT+CIPSTATUS  查询是否可以建立连接\r\n");
      if(! sim800c_SendCmd("AT+CIPSTATUS\r\n","OK",30))// 
      {
        if(!--i) 
        { COM_print(COM1 ,"\r\n9.AT+CIPSTATUS  查询是否可以建立连接 失败\r\n"); }  //超时 返回未成功
      }
      else if(i)
      {
        i=5; initStep=10;COM_print(COM1 ,"\r\n9.AT+CIPSTATUS  查询是否可以建立连接 成功\r\n");
      }
      break;
    case 10:
      COM_print(COM1 ,"\r\n10.AT+CIPHEAD=1 设置接收数据显示IP头(方便判断数据来源)回复数据有+IPD,n:开头\r\n");
      if(! sim800c_SendCmd("AT+CIPHEAD=1\r\n","OK",10))// 
      {
        if(!--i) 
        { COM_print(COM1 ,"\r\n10.AT+CIPHEAD=1 设置接收数据显示IP头(方便判断数据来源)回复数据有+IPD,n:开头 失败\r\n"); }  //超时 返回未成功
      }
      else if(i)
      {
        i=5; initStep=11;COM_print(COM1 ,"\r\n10.AT+CIPHEAD=1 设置接收数据显示IP头(方便判断数据来源)回复数据有+IPD,n:开头 成功\r\n");
      }
      break;
    case 11:
      COM_print(COM1 ,"\r\n11.AT+CIPSTART=\"TCP\",\"%s\",\"%s\" 连接地址\r\n",gtp_CnfTyp->GprsInfo.Ip,gtp_CnfTyp->GprsInfo.Port);
      sprintf(Buf, "AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r\n",gtp_CnfTyp->GprsInfo.Ip,gtp_CnfTyp->GprsInfo.Port  );	//要发送的命令  
      if(! sim800c_SendCmd(Buf,"OK",50))// "CONNECT OK"
      {
        if(!--i) 
        { COM_print(COM1 ,"10.AT+CIPSTART=\"TCP\",\"%s\",\"%s\" 失败\r\n",gtp_CnfTyp->GprsInfo.Ip,gtp_CnfTyp->GprsInfo.Port); }  //超时 返回未成功
      }
      else if(i)
      {
        i=5; initStep=0;COM_print(COM1 ,"10.AT+CIPSTART=\"TCP\",\"%s\",\"%s\" 成功\r\n\r\n",gtp_CnfTyp->GprsInfo.Ip,gtp_CnfTyp->GprsInfo.Port);
        flag=1;  // 连接上标志
      }
      break;
      initStep=0;
      i=5;
    default :
      break;
  }
  if(i==0)
  {
     i=5; initStep=0;/*失败重新开始*/
     flag= 0; //未连接flag=0 ;
  }
  return flag;
}



//SIM800C发送命令后,检测接收到的应答
//flag: 是否使用上次的数据,0 重新读取数据,1使用上次的数据
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//其他,期待应答结果的位置(str的位置)
uint8_t * sim800c_check_cmd(uint8_t flag,uint8_t *str)
{
	char *strx=0;
//  static  uint8 flag;
  if(0==flag)//读取新数据
  {
    sim800c_Clear(); //清空接收缓存 方便接收数据
    COM_Read(COM3,&sim800cBuf);  //读取sim800c返回的数据
    COM_print(COM1,"回复的数据: %s\r\n",sim800cBuf.RxBuf);	//发送到串口
  }
  else
  {
    COM_print(COM1,"回复的数据: %s\r\n",sim800cBuf.RxBuf);	//发送到串口
  }
  strx=strstr((const char*)sim800cBuf.RxBuf,(const char*)str);//找到和str相同的字符串,返回是否找到
  return (uint8_t*)strx;
}
/*
--SIM800C发送命令
--cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
--ack:期待的应答结果,如果为空,则表示不需要等待应答
--waittime:等待时间(单位:10ms)
--返回值:1,发送成功(得到了期待的应答结果)
         0,发送失败
*/
uint8_t sim800c_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime)
{
  //u8 res=0; //返回
	//uart3_rx_frame_flag=0;//串口3的接收标志位清除
 // uart3_rx_num=0;    //串口3接收数据个数清除
  
  COM_TxRxTypeDefInit(COM3);
  
  if((u32)cmd<=0XFF)
	{
		while((USART3->SR&0X40)==0);//等待上一次数据发送完成  
		USART3->DR=(u32)cmd;
   COM_print(COM1,"发送命令: %x\r\n",(u32)cmd);
	}
  else//发送命令
  {
    COM_print(COM3,"%s\r\n",cmd);
    COM_print(COM1,"发送命令: %s\r\n",cmd);
  }
  if(ack&&waittime)		//需要等待应答//ack和时间都不是空就需要应答
  {
    while(--waittime)
    {
      delay_ms(10);
      if(gsv_COM_TxRx[COM3].RxFlag) //检查是否有接收到一帧数据
      {
        if(sim800c_check_cmd(0,ack))  
        {
          
          COM_print(COM1,"回复命令:%s\r\n",ack);return 1;
          //return 0; ////得到有效数据 退出循环
        }
        else
        {
          return 0;
        }
      }
      //
    }
    if(waittime==0)  return 0;
  }
 // printf("回复命令22222:\r\n");
  return 1;
}

////返回1可用   返回0不可用
//uint8  sim800c_realm_init()
//{
//  uint8 p[100];
//  app_ReadCnf(GPRS_CONFIG_BASE,0);
//  if(g_gprs_cnf_type.label[0]=='1')
//  {
//    if(!sim800c_send_cmd("AT","OK",300)) return 0;
//    //bsp_DelayMs(1000);
//    

//      sim800c_send_cmd("AT+CIPCLOSE","CLOSE OK",100);	//关闭连接
//      if(!sim800c_send_cmd("AT+CIPSHUT","SHUT OK",100))return 0;		//关闭移动场景
//      //bsp_DelayMs(1000);
//      
//      if(!sim800c_send_cmd("AT+CGCLASS=\"B\"","OK",1000)) return 0;//设置GPRS移动台类别为B,支持包交换和数据交换 
//      //bsp_DelayMs(1000);
//      if(!sim800c_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",1000))return 0;//设置PDP上下文,互联网接协议,接入点等信息
//      //bsp_DelayMs(1000);
//      sim800c_send_cmd("AT+CGATT=1","OK",1000);  //附着GPRS业务
//      //bsp_DelayMs(1000);
//      sim800c_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",1000); //设置为GPRS连接模式  CMNET CMWAP
//      //bsp_DelayMs(1000);
//    sim800c_send_cmd("AT+CIPSTATUS","OK",500);	//查询连接状态
//    if((sim800c_check_cmd(1,"IP INITIAL")||sim800c_check_cmd(1,"IP CLOSE")||sim800c_check_cmd(1,"IP CLOSE"))==0)return 0;
//      //if(sim800c_send_cmd("AT+CIPSTATUS","OK",1000))return 0;  //检测连接情况
//      //if(sim800c_send_cmd("AT+CIPHEAD=1","OK",500))return 0;	 				//设置接收数据显示IP头(方便判断数据来源)
//      //bsp_DelayMs(1000); 
//      if(g_gprs_cnf_type.choose[0]=='1')
//      {
//        sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"",g_gprs_cnf_type.ip,g_gprs_cnf_type.ip_port);
//      }
//      else if(g_gprs_cnf_type.choose[0]=='2')
//      {
//        sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"",g_gprs_cnf_type.realm,g_gprs_cnf_type.realm_port);
//      }
//      if(!sim800c_send_cmd(p,"OK",500)==0)return 1;
//       // else if(sim800c_check_cmd())
//      //bsp_DelayMs(1000);

//  }
//  return 0;
//}

// 传入发送的数据数组,传出接收到的数据指针
uint8_t * sim800c_realm_write( uint8_t * pBuf1,uint8_t * pBuf2,uint8_t * pBuf3)
{ 
  uint8_t p[100];
//  uint8 p1[100];  //*p1=0;  //发送到服务器数据处理 
  uint8_t *p2=0; //接收到服务器数据进行处理
//  uint8 i=0x1a;
  static u16 timex=0;//重连和心跳包  计时
  static u16 timen=0;//发送数据还是心跳包计时
  
  static uint16_t beat_time=0; //心跳间隔时间
  static uint16_t data_time=0; //数据间隔时间
  
  static uint8_t step=0;  //0初始化,1工作
  static uint8_t connectsta=0; //连接状态 0正在连接, 1 连接成功 2是断开
  static uint8_t connectnum=0;  //多次连接不上重新初始化 
  static uint8_t hbeaterrcnt=0;   //心跳错误计数器,连续10次心跳信号无应答,则重新连接
  static uint8_t section=0;  //分段发送数据  第一段发送pbuf1,第二段发送pbuf2,第三段发送pbuf3
//  memset((uint8_t*)&g_sim_get_buf,'\0',1024);
  if(step==0)
  {
        if(!gtp_CnfTyp->GprsInfo.Choose) //如果设置GPRS不可用 就退出
        {
          
          return 0;
        }
//      if(g_gprs_cnf_type.label[0]!='1')  return 0;
//      if(g_gprs_cnf_type.choose[0]=='1')  //选择ip还是域名连接
//      {
//        sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"",g_gprs_cnf_type.ip,g_gprs_cnf_type.ip_port);
//      }
//      else if(g_gprs_cnf_type.choose[0]=='2')
//      {
//        sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"",g_gprs_cnf_type.realm,g_gprs_cnf_type.realm_port);
//      }
//      beat_time=str_to_int((char*)g_gprs_cnf_type.beat_time)*10;  //获取心跳间隔时间 单位 10ms
//      data_time=str_to_int((char*)g_gprs_cnf_type.data_time)*10;  //获取数据间隔时间
      //printf("%d%d\r\n",beat_time,data_time);
      if(! sim800c_send_cmd("AT","OK",300)) return  0;
      sim800c_send_cmd("AT+CIPCLOSE","CLOSE OK",100);	//关闭连接
      sim800c_send_cmd("AT+CIPSHUT","SHUT OK",100);		//关闭移动场景
      if(! sim800c_send_cmd("AT+CGCLASS=\"B\"","OK",1000))
      { 
        if(sim800c_check_cmd(1,"ERROR"))
        {
           //拉低PWRKEY 1s 重启模块
        }
        return  0;//设置GPRS移动台类别为B,支持包交换和数据交换 
      }
      if(! sim800c_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",1000))return  0;//设置PDP上下文,互联网接协议,接入点等信息
      sim800c_send_cmd("AT+CGATT=1","OK",1000);  //附着GPRS业务
      sim800c_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",1000); //设置为GPRS连接模式 CMWAP CMNET 
      sim800c_send_cmd("AT+CIPSTATUS","OK",500);	//查询连接状态
      if((sim800c_check_cmd(1,"IP INITIAL")||sim800c_check_cmd(1,"IP CLOSE")||sim800c_check_cmd(1,"IP CLOSE"))==0)return  0;
      //if(sim800c_send_cmd("AT+CIPSTATUS","OK",1000))return 0;  //检测连接情况
      if(! sim800c_send_cmd("AT+CIPHEAD=1","OK",500))return  0;	 				//设置接收数据显示IP头(方便判断数据来源)回复数据有+IPD,n:开头
      if( sim800c_send_cmd(p,"OK",500))
      {
        step=1;
        connectnum=0;
        connectsta=0;
        hbeaterrcnt=0;
      }
  }
  else if(step==1)
  {
      if(connectsta==2||hbeaterrcnt>20)//连接中断了,或者连续10次心跳没有正确发送成功,则重新连接
      {
        step=0;
        connectsta=0;
        connectnum=0;
        hbeaterrcnt=0;
        
        
//        app_ReadCnf(GPRS_CONFIG_BASE,GPRS_CONFIG_OFFSET,(uint8*)&g_gprs_cnf_type,0,0,0);
//        //app_ReadCnf(GPRS_CONFIG_BASE,0);
//        if(g_gprs_cnf_type.choose[0]=='1')  //选择ip还是域名连接
//          {
//            sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"",g_gprs_cnf_type.ip,g_gprs_cnf_type.ip_port);
//          }
//          else if(g_gprs_cnf_type.choose[0]=='2')
//          {
//            sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"",g_gprs_cnf_type.realm,g_gprs_cnf_type.realm_port);
//          }
//        sim800c_send_cmd("AT+CIPCLOSE=1","CLOSE OK",500);	//关闭连接
//        sim800c_send_cmd("AT+CIPSHUT","SHUT OK",500);		//关闭移动场景 
//        sim800c_send_cmd(p,"OK",500);						//尝试重新连接
//        connectsta=0;	
//        hbeaterrcnt=0;
      }
      if(connectsta ==0&&(timex%200)==0)  //如果没有建立连接 重新建立连接
      {
        //printf("步骤==========%d,连接状态======%d,尝试通讯次数===%d\r\n",step,connectsta,connectnum);
        sim800c_send_cmd("AT+CIPSTATUS","OK",500);	//查询连接状态

        if(sim800c_check_cmd(1,"PDP DEACT"))   {timex=0; timen=0; connectsta=0;connectnum=0;step=0; hbeaterrcnt=0;}
        if(sim800c_check_cmd(1,"CLOSED"))      connectsta=2;
        if(sim800c_check_cmd(1,"CONNECT OK")) 
          {
            connectsta=1;
            timex=beat_time/2;
            timen=data_time/2;
          }
       if(sim800c_check_cmd(1,"OK"))
        {
            connectsta=1;
            timex=beat_time;
            timen=data_time;
           delay_ms(300);
          return 0;
        }
        connectnum++;
        if(connectnum>10)            ////////尝试连接次数
        {
          connectnum=0;
          step=0;
        }
      }
      if(connectsta==1&&timex>=beat_time)//连接正常的时候,每6秒发送一次心跳
      {
        //printf("步骤==========%d,连接状态======%d,尝试通讯次数===%d\r\n",step,connectsta,connectnum);
        timex=0;
        hbeaterrcnt++;
        if(sim800c_send_cmd("AT+CIPSEND",">",200))//发送数据
        {
          connectnum=0;

            //printf("beat bag\r\n");
            //u3_printf("beat bag\r\n");
            sim800c_send_cmd("beat bag",0,0);	//发送数据:0X00 
          delay_ms(10);						//必须加延时
          sim800c_send_cmd((u8*)0X1A,0,0) ;       //CTRL+Z,结束数据发送,启动一次传输
          //hbeaterrcnt=0;  //心跳正常
        }else 
        {
          sim800c_send_cmd((u8*)0X1B,0,0);	//ESC,取消发送 		
        }
        delay_ms(10);						//必须加延时
        //printf("hbeaterrcnt:%d\r\n",hbeaterrcnt);//方便调试代码
      }
      if(connectsta==1&&timen>=data_time)//连接正常的时候发送数据
      {
        printf("等待时间%d\r\n",data_time);
        //printf("步骤==========%d,连接状态======%d,尝试通讯次数===%d\r\n",step,connectsta,connectnum);
        timen=0;
        hbeaterrcnt++;
        if(sim800c_send_cmd("AT+CIPSEND",">",200))//发送数据
        {
            connectnum=0;
            //sprintf((char*)p1,"hello,my name is fangchenxing\r\n");//
            //printf("%s\r\n",pBuf);
            //u3_printf("%s\r\n",pBuf);
          switch(section)
          {
            case 0:
                sim800c_send_cmd(pBuf1,0,0);
                delay_ms(10);						//必须加延时
                sim800c_send_cmd((u8*)0X1A,0,0) ;       //CTRL+Z,结束数据发送,启动一次传输
                section=0; break;
            case 1:
              sim800c_send_cmd(pBuf2,0,0);
              delay_ms(10);						//必须加延时
              sim800c_send_cmd((u8*)0X1A,0,0) ;       //CTRL+Z,结束数据发送,启动一次传输
               section=2; break;
            case 2:
              sim800c_send_cmd(pBuf3,0,0);
              delay_ms(10);						//必须加延时
              sim800c_send_cmd((u8*)0X1A,0,0) ;       //CTRL+Z,结束数据发送,启动一次传输
              section=0;break;
          }
          //hbeaterrcnt=0;  //心跳正常
        }else 
        {
          sim800c_send_cmd((u8*)0X1B,0,0);	//ESC,取消发送 		
        }
        //printf("hbeaterrcnt:%d\r\n",hbeaterrcnt);//方便调试代码
      }
      
//      if(uart3_rx_frame_flag)//读取服务器发过来的数据
//      {
//        g_sim_get_len=bsp_Usart3Read(g_sim_get_buf);
//        if(hbeaterrcnt)							//需要检测心跳应答
//        {
//          if(strstr((const char*)g_sim_get_buf,"SEND OK"))
//          {            
//            hbeaterrcnt=0;//心跳正常
//            //printf("心跳正常\r\n");
//          }
//        }
//        p2=(uint8_t*)strstr((const char*)g_sim_get_buf,"+IPD");
//        if(p2)
//        {
//          p2=(uint8_t*)strstr((const char*)g_sim_get_buf,":");
//          p2++;
//        }
//      }
//      bsp_DelayMs(1);
      timex++;
      timen++;
  }
  return p2;
}


/*******************************************************************************
文 件 名: uart.c
功    能: 
创建日期: 
作    者: 
详    细: 
********************************************************************************
注意事项
*******************************************************************************/

#include "uart.h"
#include "my_lib.h"

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
//#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
int _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 


/*串口号,串口时钟,串口中断*/
USART_TypeDef  *COM_USART[]   =    {COM1_UARTn,COM2_UARTn,COM3_UARTn,
                                    COM4_UARTn,COM5_UARTn};
const uint32_t  COM_UART_CLK[]=    {COM1_CLK,COM2_CLK,COM3_CLK,
                                    COM4_CLK,COM5_CLK};
const uint8_t   COM_IRQn[]    =    {COM1_IRQn,COM2_IRQn,COM3_IRQn,
                                    COM4_IRQn,COM5_IRQn};
/*TX 的 GPIO  PIN  CLK*/
GPIO_TypeDef   *COM_TX_PORT[] =    {COM1_TX_GPIO_PORT,COM2_TX_GPIO_PORT,
                                    COM3_TX_GPIO_PORT,COM4_TX_GPIO_PORT,
                                    COM5_TX_GPIO_PORT};
const uint16_t  COM_TX_PIN[]  =    {COM1_TX_PIN,COM2_TX_PIN,COM3_TX_PIN,
                                    COM4_TX_PIN,COM5_TX_PIN};
const uint32_t  COM_TX_PORT_CLK[]= {COM1_TX_GPIO_CLK,COM2_TX_GPIO_CLK,
                                    COM3_TX_GPIO_CLK,COM4_TX_GPIO_CLK,
                                    COM5_TX_GPIO_CLK};
/*RX 的 GPIO  PIN  CLK*/
GPIO_TypeDef   *COM_RX_PORT[] =    {COM1_RX_GPIO_PORT,COM2_RX_GPIO_PORT,
                                    COM3_RX_GPIO_PORT,COM4_RX_GPIO_PORT,
                                    COM5_RX_GPIO_PORT};
const uint16_t  COM_RX_PIN[]  =    {COM1_RX_PIN,COM2_RX_PIN,COM3_RX_PIN,
                                    COM4_RX_PIN,COM5_RX_PIN};
const uint32_t  COM_RX_PORT_CLK[]= {COM1_RX_GPIO_CLK,COM2_RX_GPIO_CLK,
                                    COM3_RX_GPIO_CLK,COM4_RX_GPIO_CLK,
                                    COM5_RX_GPIO_CLK};

/*串口数据接收发送的 结构数据, 初始化每个使用到的COM使用到的结构,在串口初始化里面调用*/
COM_TxRxTypeDef gsv_COM_TxRx[COMn];

/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: 初始化 接收发送数据类型
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/
void COM_TxRxTypeDefInit(COM_TypeDef com)
{
  gsv_COM_TxRx[com].RxFlag    = 0;
  gsv_COM_TxRx[com].RxNum     = 0;
  memset( gsv_COM_TxRx[com].RxBuf, 0, RX_MAX+1);  
  memset( gsv_COM_TxRx[com].TxBuf, 0, TX_MAX+1);
}

/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: 串口初始化,UART 不是 USATR  其USART 1 2 3 使用的是UART功能,既没有时钟线
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/
void UART_Init(COM_TypeDef com, uint32_t bound)
{
  /*GPIO端口设置*/
  GPIO_InitTypeDef GPIO_InitStructure;
  /*串口模式配置*/
  USART_InitTypeDef USART_InitStructure;
  /*中断配置*/
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd (COM_TX_PORT_CLK[com],ENABLE);
  RCC_APB2PeriphClockCmd (COM_RX_PORT_CLK[com],ENABLE);
  if(com==COM1)
  {
    RCC_APB2PeriphClockCmd(COM_UART_CLK[com],ENABLE);
  }
  else
  {
    RCC_APB1PeriphClockCmd(COM_UART_CLK[com],ENABLE);
  }
  /*TX*/
  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[com];
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(COM_TX_PORT[com], &GPIO_InitStructure);

  /*RX*/
  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[com];
  /*本来应该浮空输入GPIO_Mode_IN_FLOATING,由于没有外上拉电阻,这里用上拉输入*/
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(COM_RX_PORT[com], &GPIO_InitStructure);
  
  /* NVIC 配置 */
  NVIC_InitStructure.NVIC_IRQChannel = COM_IRQn[com];
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
  /* 串口模式 */
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(COM_USART[com], &USART_InitStructure); //初始化串口1
  USART_ITConfig(COM_USART[com], USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(COM_USART[com], ENABLE);                    //使能串口1

  COM_TxRxTypeDefInit(com);  /*初始化串口用到的结构变量*/
  /*设置每一帧数据的间隔时间  单位是ms*/
  gsv_COM_TxRx[com].RxInterval =((10*1000000/bound)*4.5) /1000;
  
}

/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: 串口数据接收
依    赖: 
参    数: 一个COM接收发送的结构 , 其中包含的数据有 接收到数据的长度,数据buf
返    回: 接收到数据返回1 没有接收到数据返回0
详细说明: 
*******************************************************************************/
_Bool COM_Read(COM_TypeDef com, COM_RxTypeDef *rx)
{
  if (!gsv_COM_TxRx[com].RxFlag)/*判断是否接收到一帧*/
  {
    return FALSE;  /*返回0*/
  }
  gsv_COM_TxRx[com].RxFlag = 0;/*情况一帧标志位*/
  
  rx->RxNum = gsv_COM_TxRx[com].RxNum; /*把数据个数复制出来*/
  
  gsv_COM_TxRx[com].RxNum = 0; //把接个数清空
  /*把数据复制出来*/
  memcpy(rx->RxBuf, gsv_COM_TxRx[com].RxBuf,rx->RxNum);
  
  memset(gsv_COM_TxRx[com].RxBuf, 0, sizeof(gsv_COM_TxRx[com].RxBuf));//把接收缓存清空
  return TRUE;
}

/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: 
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/
//COM_RxTypeDef  COM_Read(COM_TypeDef com)
//{
////  uint8_t num=0;
//  COM_RxTypeDef COM_RxStructure;
//  /*清空这个结构,,防止数据里面包含了上一次的数据*/
//  memset(&COM_RxStructure,0,sizeof(COM_RxTypeDef));

//  if(gsv_COM_TxRx[com].RxFlag==1)   /*如果接收到一帧*/
//  {
//    do /*begin while(--gsv_COM_TxRx[com].RxNum)*/
//    {
//      /*把数据转存到返回结构*/
//      COM_RxStructure.RxBuf[COM_RxStructure.RxNum++] = *gsv_COM_TxRx[com].PRxBuf++;
//    } while(--gsv_COM_TxRx[com].RxNum); /*数据个数计数*/
//    
//    gsv_COM_TxRx[com].RxFlag  = 0;  /*清除接收一帧标志位,最后清除他是因为 接收监控里面对他有判断*/
//    gsv_COM_TxRx[com].PRxBuf  = gsv_COM_TxRx[com].RxBuf;/*接收缓存指针回归*/
//  } /*end if(lsv_ComTxRx[com].COM_RxFlag==1)*/
//  
//  return COM_RxStructure;
//}


/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: 串口接收监视,有空闲时间判断帧结束,需要在定时器中调用,MS定时器时间间隔
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/

void COM_RxMonitor (COM_TypeDef com)
{
static unsigned int  cntbkp[5]  = {0,0,0,0,0}; // 接收数据的个数 计数比较
static unsigned int  idletmr[5] = {0,0,0,0,0}; //接收数据后空闲时间 ms计时
  if(gsv_COM_TxRx[com].RxNum>0)   /*如果计数器大于0 说明开始接受数据了*/
  {
    if(cntbkp[com] != gsv_COM_TxRx[com].RxNum)  /*接受到新数据,,清除计时*/
    {
      cntbkp[com]  = gsv_COM_TxRx[com].RxNum; /*同步接收个数*/
      idletmr[com] = 0;   /*计时器清零*/
    }
    else /*没有接收到新数据,,计时增加*/
    {
      if(!gsv_COM_TxRx[com].RxFlag) /*如果没有接收完一帧*/
      {
        idletmr[com] += 1; /*计时器 加1*/
        if(idletmr[com] >= gsv_COM_TxRx[com].RxInterval)
        {
          gsv_COM_TxRx[com].RxFlag = 1;/*一帧接收完标志*/
        } /*end if(idletmr[com]>=gsv_COM_TxRx[com].RxInterval)*/
      } /*end if(!gsv_COM_TxRx[com].RxFlag)*/
    } /*end if(cntbkp[com] != gsv_COM_TxRx[com].RxNum)*/
  } /*end if(gsv_COM_TxRx[com].RxNum>0)*/
  else /*不接收数据的时候,, 把接收个数和*/
  {
    cntbkp[com]  = 0;
    idletmr[com] = 0;
  }
}

/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: 串口打印函数
依    赖: 
参    数: 要发送的数据数组
返    回: 
详细说明: 
*******************************************************************************/
void COM_print (COM_TypeDef com ,const char* fmt,...)
{
  uint16_t i,j;
  va_list ap;
  memset(gsv_COM_TxRx[com].TxBuf,0,TX_MAX);
  
	va_start(ap,fmt);
	vsprintf((char*)gsv_COM_TxRx[com].TxBuf,fmt,ap);
	va_end(ap);
	i=strlen((const char*)gsv_COM_TxRx[com].TxBuf);		//此次发送数据的长度
  for(j=0;j<i;j++)
  {
    while(USART_GetFlagStatus(COM_USART[com],USART_FLAG_TC)==RESET)
    {}
    USART_SendData(COM_USART[com],gsv_COM_TxRx[com].TxBuf[j]);
  }
}

/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: 串口发送十六进制数据
依    赖: 
参    数: 参数1:数组
          参数2:数组长度
返    回: 
详细说明: 
*******************************************************************************/
void COM_Write (COM_TypeDef com , c8* dat,u16 len)
{
  u16 t;
    for(t=0;t<len;t++)
    {
      /*等待发送结束 写在全面是为了防止前面有数据没有发送完*/
      while(USART_GetFlagStatus(COM_USART[com],USART_FLAG_TC)!=SET);

      USART_SendData(COM_USART[com], dat[t]);//向串口发送数据
      
    }
}

/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: 串口中断统一处理函数 被中断调用
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/
void UART_RxCallback (COM_TypeDef com)
{
  /*接收到一个数据*/
  if(USART_GetITStatus(COM_USART[com], USART_IT_RXNE) != RESET)  //接收到一个字节中斿
  {
//    if(gsv_COM_TxRx[com].RxFlag) /*上一条数据没有读取,又来一条新数据,上一条数据覆盖*/
//    {
//      gsv_COM_TxRx[com].RxFlag=0;
//    }
    /*一帧数据没有接收完 继续接收,第一帧数据没有读取出去就忽略这一帧数据*/
//    if(0==gsv_COM_TxRx[com].RxFlag) 
//    {
      if(gsv_COM_TxRx[com].RxNum<RX_MAX)/*小于最大接收数目*/
      {
        gsv_COM_TxRx[com].RxFlag=0;
        gsv_COM_TxRx[com].RxBuf[gsv_COM_TxRx[com].RxNum++]=COM_USART[com]->DR;//USART_ReceiveData(COM_USART[com]);
      }
      else
      {
        gsv_COM_TxRx[com].RxFlag=1; /*如果接收数据的数量大于等于RX_MAX 就不再接收*/
      }
//    }
  }
}

/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: 串口1~5中断函数
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/
void USART1_IRQHandler(void)
{
  UART_RxCallback(COM1);
}
void USART2_IRQHandler(void)
{
  UART_RxCallback(COM2);
}
void USART3_IRQHandler(void)
{
  UART_RxCallback(COM3);
}
void UART4_IRQHandler(void)
{
  UART_RxCallback(COM4);
}
void UART5_IRQHandler(void)
{
  UART_RxCallback(COM5);
}




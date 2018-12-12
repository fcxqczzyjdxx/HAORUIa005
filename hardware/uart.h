/*******************************************************************************
文 件 名: uart.h
功    能: 
创建日期: 
作    者:
详    细: 
********************************************************************************
注意事项
*******************************************************************************/

/*******************************************************************************
连接方式
uart1  调试使用和新版做配置
  TX -> PA9
  RX -> PA10
uart2  485使用 PA1 用在RD控制
  TX -> PA2
  RX -> PA3
uart3  SIM800c芯片通讯
  TX -> PB10
  RX -> PB11
uart4  SP3232 TTL转232 暂时没有用到
  TX -> PC10
  RX -> PC11
uart5 未使用
  TX -> PC12
  RX -> PD2
*******************************************************************************/

#ifndef __UART_H__
#define __UART_H__

#include "stm32f10x.h"

typedef enum
{
  COM1 = 0,
  COM2 = 1,
  COM3 = 2,
  COM4 = 3,
  COM5 = 4  /*没有使用*/
}COM_TypeDef;

#define COMn         5
#define IS_UART(COM) (((COM)==COM1)||((COM)==COM2)||((COM)==COM3)||\
                      ((COM)==COM4)||((COM)==COM5))

#define COM1_UARTn          USART1
#define COM1_CLK            RCC_APB2Periph_USART1
#define COM1_IRQn           USART1_IRQn
#define COM1_TX_GPIO_PORT   GPIOA
#define COM1_TX_PIN         GPIO_Pin_9
#define COM1_TX_GPIO_CLK    RCC_APB2Periph_GPIOA
#define COM1_RX_GPIO_PORT   GPIOA
#define COM1_RX_PIN         GPIO_Pin_10
#define COM1_RX_GPIO_CLK    RCC_APB2Periph_GPIOA


#define COM2_UARTn          USART2
#define COM2_CLK            RCC_APB1Periph_USART2
#define COM2_IRQn           USART2_IRQn
#define COM2_TX_GPIO_PORT   GPIOA
#define COM2_TX_PIN         GPIO_Pin_2
#define COM2_TX_GPIO_CLK    RCC_APB2Periph_GPIOA
#define COM2_RX_GPIO_PORT   GPIOA
#define COM2_RX_PIN         GPIO_Pin_3
#define COM2_RX_GPIO_CLK    RCC_APB2Periph_GPIOA


#define COM3_UARTn          USART3
#define COM3_CLK            RCC_APB1Periph_USART3
#define COM3_IRQn           USART3_IRQn
#define COM3_TX_GPIO_PORT   GPIOB
#define COM3_TX_PIN         GPIO_Pin_10
#define COM3_TX_GPIO_CLK    RCC_APB2Periph_GPIOB
#define COM3_RX_GPIO_PORT   GPIOB
#define COM3_RX_PIN         GPIO_Pin_11
#define COM3_RX_GPIO_CLK    RCC_APB2Periph_GPIOB


#define COM4_UARTn          UART4
#define COM4_CLK            RCC_APB1Periph_UART4
#define COM4_IRQn           UART4_IRQn
#define COM4_TX_GPIO_PORT   GPIOC
#define COM4_TX_PIN         GPIO_Pin_10
#define COM4_TX_GPIO_CLK    RCC_APB2Periph_GPIOC
#define COM4_RX_GPIO_PORT   GPIOC
#define COM4_RX_PIN         GPIO_Pin_11
#define COM4_RX_GPIO_CLK    RCC_APB2Periph_GPIOC


#define COM5_UARTn          UART5
#define COM5_CLK            RCC_APB1Periph_UART5
#define COM5_IRQn           UART5_IRQn
#define COM5_TX_GPIO_PORT   GPIOC
#define COM5_TX_PIN         GPIO_Pin_12
#define COM5_TX_GPIO_CLK    RCC_APB2Periph_GPIOC
#define COM5_RX_GPIO_PORT   GPIOD
#define COM5_RX_PIN         GPIO_Pin_2
#define COM5_RX_GPIO_CLK    RCC_APB2Periph_GPIOD



/*这些变量用在数据的发送和接收,发送主要用printf类似,发送缓存基本不用*/
#define RX_MAX  2040  /*接收最大个数*/
#define TX_MAX  2040    /*发送最大个数*/

/*接收和发送的MAX 实际数值是MAX+1 为了最后一位是空,以防溢出*/
/*串口数据标志位和缓冲区*/
typedef struct { 
  uint8_t  RxFlag;      /*接收一帧标志位*/
  u16  RxNum;       /*接收数据个数*/
  c8  RxBuf[RX_MAX+1];  /*接收数据缓存*/
  uint8_t  RxInterval;  /*一帧间隔时间*/

  c8  TxBuf[TX_MAX+1];  /*发送数据缓存*/
} COM_TxRxTypeDef;

/*串口接收返回的数据*/
typedef struct{
 u16 RxNum;       /*接收数据个数*/
 c8 RxBuf[RX_MAX+1];
}COM_RxTypeDef;


/*串口句柄,用在 串口回调函数*/
extern USART_TypeDef  *COM_USART[];
/*串口数据接收发送的 结构数据, 初始化每个使用到的COM使用到的结构,在串口初始化里面调用*/
extern COM_TxRxTypeDef gsv_COM_TxRx[COMn];

void COM_TxRxTypeDefInit(COM_TypeDef com);
/*串口初始化*/
extern void UART_Init(COM_TypeDef com, uint32_t bound);
/*读取串口数据*/
_Bool COM_Read(COM_TypeDef com, COM_RxTypeDef *rx);
//extern COM_RxTypeDef  COM_Read(COM_TypeDef com);

/*串口接收一帧监视函数,用在定时器回调函数*/
extern void COM_RxMonitor (COM_TypeDef com);
/*串口打印*/
extern void COM_print (COM_TypeDef com ,const char* fmt,...);
/*串口发送十六进制数据*/
void COM_Write (COM_TypeDef com , c8* dat,u16 len);















#endif

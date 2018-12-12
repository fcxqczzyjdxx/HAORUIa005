/*******************************************************************************
文 件 名: out_relay.h
功    能: 
创建日期: 
作    者:
邮    箱:
详    细: 
********************************************************************************
注意事项
*******************************************************************************/
#ifndef __OUT_RELAY_H__
#define __OUT_RELAY_H__

#include "stm32f10x.h"


/*******************************************************************************
连接方式
OUT1 -> PE12
OUT2 -> PE13
OUT3 -> PE14  //没有启用
OUT4 -> PE15  //没有启用
*******************************************************************************/


typedef enum
{
  RLY1=0,
  RLY2=1,
  RLY3=2,
  RLY4=3
}RLY_TypeDef;

#define IS_RLY(PLY)    (((PLY)==RLY1)||((PLY)==RLY2)||  \
                          ((PLY)==RLY3)||((PLY)==RLY4))


//#define RLYn  4

#define RLY1_PIN          GPIO_Pin_12
#define RLY1_GPIO_PORT    GPIOE
#define RLY1_GPIIO_CLK    RCC_APB2Periph_GPIOE

#define RLY2_PIN          GPIO_Pin_13
#define RLY2_GPIO_PORT    GPIOE
#define RLY2_GPIIO_CLK    RCC_APB2Periph_GPIOE

#define RLY3_PIN          GPIO_Pin_14
#define RLY3_GPIO_PORT    GPIOE
#define RLY3_GPIIO_CLK    RCC_APB2Periph_GPIOE

#define RLY4_PIN          GPIO_Pin_15
#define RLY4_GPIO_PORT    GPIOE
#define RLY4_GPIIO_CLK    RCC_APB2Periph_GPIOE

void RLY_Init(RLY_TypeDef rly);    //初始化

void RLY_OpenOn(RLY_TypeDef rly);  //常开闭合
void RLY_OpenOff(RLY_TypeDef rly); //常开断开
uint8_t RLY_OpenStatus(RLY_TypeDef rly);  //常开读取状态

void RLY_CloseOn(RLY_TypeDef rly);  //常闭 闭合
void RLY_CloseOff(RLY_TypeDef rly); //常闭 闭合
uint8_t RLY_CloseStatus(RLY_TypeDef rly);  //常闭读取状态

#endif

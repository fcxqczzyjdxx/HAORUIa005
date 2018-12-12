/*******************************************************************************
文 件 名: in_photocoupler.h
功    能: 
创建日期: 
作    者:
详    细: 
********************************************************************************
注意事项
*******************************************************************************/


/*******************************************************************************
连接方式
IN1 -> PE8
IN2 -> PE9   //没有启用
IN3 -> PE10  //没有启用
IN4 -> PE11  //没有启用
*******************************************************************************/

#ifndef __IN_PHOTOCOUPLER_H__
#define __IN_PHOTOCOUPLER_H__

#include "stm32f10x.h"


typedef enum
{
  PTCP1=0,
  PTCP2=1,
  PTCP3=2,
  PTCP4=3
}PTCP_TypeDef;

#define IS_PTCP(PTCP)    (((PTCP)==PTCP1)||((PTCP)==PTCP2)||  \
                          ((PTCP)==PTCP3)||((PTCP)==PTCP4))


//#define PTCPn  4
//photocoupling
#define   PTCP1_PIN          GPIO_Pin_8
#define   PTCP1_GPIO_PORT    GPIOE
#define   PTCP1_GPIIO_CLK    RCC_APB2Periph_GPIOE

#define PTCP2_PIN          GPIO_Pin_9
#define PTCP2_GPIO_PORT    GPIOE
#define PTCP2_GPIIO_CLK    RCC_APB2Periph_GPIOE

#define PTCP3_PIN          GPIO_Pin_10
#define PTCP3_GPIO_PORT    GPIOE
#define PTCP3_GPIIO_CLK    RCC_APB2Periph_GPIOE

#define PTCP4_PIN          GPIO_Pin_11
#define PTCP4_GPIO_PORT    GPIOE
#define PTCP4_GPIIO_CLK    RCC_APB2Periph_GPIOE

extern void PTCP_Init(PTCP_TypeDef ptcp);
extern uint8_t PTCP_GetStatus(PTCP_TypeDef ptcp);




#endif

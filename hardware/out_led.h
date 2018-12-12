/*******************************************************************************
文 件 名: out_led.h
功    能: 
创建日期: 
作    者:
详    细: 
********************************************************************************
注意事项
*******************************************************************************/

/*******************************************************************************
连接方式
LED1 -> PB7
LED2 -> PB6
LED3 -> PB5
*******************************************************************************/

#ifndef __OUT_LED_H__
#define __OUT_LED_H__

#include "stm32f10x.h"
#include "gpio.h"

typedef enum 
{
  LED1 = 0,
  LED2 = 1,
  LED3 = 2,
//  LED4 = 3
} LED_TypeDef;

#define IS_LED(LED)    (((LED)==LED1)||((LED)==LED2)||((LED)==LED3))
#define LEDn                   3

#define LED1_GPIO_PORT         GPIOB
#define LED1_PIN               GPIO_Pin_7
#define LED1_GPIO_CLK          RCC_APB2Periph_GPIOB

#define LED2_GPIO_PORT         GPIOB
#define LED2_PIN               GPIO_Pin_6
#define LED2_GPIO_CLK          RCC_APB2Periph_GPIOB 

#define LED3_GPIO_PORT         GPIOB
#define LED3_PIN               GPIO_Pin_5 
#define LED3_GPIO_CLK          RCC_APB2Periph_GPIOB  


extern uint8_t g_led[4];

void LED_Init(LED_TypeDef led);//初始化
void LED_On(LED_TypeDef Led);  //打开led
void LED_Off(LED_TypeDef Led); //关闭led

#endif

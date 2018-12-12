/*******************************************************************************
文 件 名: out_led.c
功    能: LED
创建日期: 2018-8-1
作    者: 
详    细: 
修改日期: 
修改原因: 
作    者: 
详    细: 
********************************************************************************
注意事项
*******************************************************************************/
#include "out_led.h"

uint8_t g_led[4];
/* 指针数组,, 表示 该数组里面存放了 n个指针 */
GPIO_TypeDef*  LED_GPIO_PORT[] = {LED1_GPIO_PORT, LED2_GPIO_PORT,
                                 LED3_GPIO_PORT};
const uint16_t LED_GPIO_PIN[] = {LED1_PIN, LED2_PIN, LED3_PIN};
const uint32_t LED_GPIO_CLK[] = {LED1_GPIO_CLK, LED2_GPIO_CLK,
                                 LED3_GPIO_CLK};
/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: LED初始化
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/
void LED_Init(LED_TypeDef led)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  assert_param(IS_LED(led));

  RCC_APB2PeriphClockCmd(LED_GPIO_CLK[led], ENABLE);	 //使能PB端口时钟

  GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN[led];				 //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(LED_GPIO_PORT[led], &GPIO_InitStructure);					    //根据设定参数初始化
  
  GPIO_SetBits(LED_GPIO_PORT[led],LED_GPIO_PIN[led]);   //输出高电平 默认熄灭led
}
/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: LDE打开
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/
void LED_On(LED_TypeDef led)
{
  assert_param(IS_LED(led));
  LED_GPIO_PORT[led]->BRR = LED_GPIO_PIN[led];  
}
/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: LED关闭
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/
void LED_Off(LED_TypeDef led)
{
  assert_param(IS_LED(led));
  LED_GPIO_PORT[led]->BSRR = LED_GPIO_PIN[led]; 
}



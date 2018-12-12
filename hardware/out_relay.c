/*******************************************************************************
文 件 名: out_relay.c
功    能: 继电器
创建日期: 2018-8-1
作    者: 
详    细: 
********************************************************************************
注意事项
*******************************************************************************/

#include "out_relay.h"

/*GPIO 分组*/
GPIO_TypeDef* RLY_GPIO_PORT[] = {RLY1_GPIO_PORT, RLY1_GPIO_PORT,RLY4_GPIO_PORT,
                                 RLY1_GPIO_PORT};
/*GPIO pin*/
const uint16_t RLY_GPIO_PIN[] = {RLY1_PIN, RLY2_PIN, RLY3_PIN,RLY4_PIN};
/*GPIO 时钟*/
const uint32_t RLY_GPIO_CLK[] = {RLY1_GPIIO_CLK, RLY2_GPIIO_CLK,RLY3_GPIIO_CLK,
                                 RLY4_GPIIO_CLK};

/*******************************************************************************
函数名字: 
函数功能: 继电器初始化
创建时间: 
参    数: 
返    回: 
依    赖: 
作    者: 
详细说明: 
*******************************************************************************/
void RLY_Init(RLY_TypeDef rly)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  assert_param(IS_RLY(rly));
  
  RCC_APB2PeriphClockCmd(RLY_GPIO_CLK[rly], ENABLE);	 //使能PB端口时钟
  
  GPIO_InitStructure.GPIO_Pin = RLY_GPIO_PIN[rly];				 //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
  GPIO_Init(RLY_GPIO_PORT[rly], &GPIO_InitStructure);					    //根据设定参数初始化
  
  GPIO_SetBits(RLY_GPIO_PORT[rly],RLY_GPIO_PIN[rly]);   //输出高电
}

/*******************************************************************************
函数名字: 
函数功能: 常开闭合
创建时间: 
参    数: 
返    回: 
依    赖: 
作    者: 
详细说明: 
*******************************************************************************/
void RLY_OpenOn(RLY_TypeDef rly)
{
  assert_param(IS_RLY(rly));
  RLY_GPIO_PORT[rly]->BSRR = RLY_GPIO_PIN[rly];
}
/*******************************************************************************
函数名字: 
函数功能: 常开断开
创建时间: 
参    数: 
返    回: 
依    赖: 
作    者: 
详细说明: 
*******************************************************************************/
void RLY_OpenOff(RLY_TypeDef rly)
{
  assert_param(IS_RLY(rly));
  RLY_GPIO_PORT[rly]->BRR = RLY_GPIO_PIN[rly];
}

/*******************************************************************************
函数名字: 
函数功能: 常开读取状态
创建时间: 
参    数: 
返    回: 
依    赖: 
作    者: 
详细说明: 
*******************************************************************************/
uint8_t RLY_OpenStatus(RLY_TypeDef rly)
{
  assert_param(IS_RLY(rly));
 // return (RLY_GPIO_PORT[rly]->IDR & RLY_GPIO_PIN[rly]);/* !! 转换成bool类型*/
	return GPIO_ReadInputDataBit(RLY_GPIO_PORT[rly],RLY_GPIO_PIN[rly]);//将改引脚单独读回
}

/*******************************************************************************
函数名字: 
函数功能: 常闭闭合
创建时间: 
参    数: 
返    回: 
依    赖: 
作    者: 
详细说明: 
*******************************************************************************/
void RLY_CloseOn(RLY_TypeDef rly)
{
  assert_param(IS_RLY(rly));
  RLY_GPIO_PORT[rly]->BSRR = RLY_GPIO_PIN[rly];
}
/*******************************************************************************
函数名字: 
函数功能: 常闭断开
创建时间: 
参    数: 
返    回: 
依    赖: 
作    者: 
详细说明: 
*******************************************************************************/
void RLY_CloseOff(RLY_TypeDef rly)
{
  assert_param(IS_RLY(rly));
  RLY_GPIO_PORT[rly]->BRR = RLY_GPIO_PIN[rly];
}

/*******************************************************************************
函数名字: 
函数功能: 常闭读取状态
创建时间: 
参    数: 
返    回: 
依    赖: 
作    者: 
详细说明: 
修改时间: 
修改原因: 
参    数: 
返    回: 
依    赖: 
作    者: 
详细说明: 
*******************************************************************************/
uint8_t RLY_CloseStatus(RLY_TypeDef rly)
{
  assert_param(IS_RLY(rly));
 // return !(RLY_GPIO_PORT[rly]->IDR & RLY_GPIO_PIN[rly]);
	return !!GPIO_ReadInputDataBit(RLY_GPIO_PORT[rly],RLY_GPIO_PIN[rly]);//将改引脚单独读回
}




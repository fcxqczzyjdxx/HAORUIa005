/*******************************************************************************
文 件 名: in_photocoupler.c
功    能: 
创建日期: 
作    者:
详    细: 光耦
修改日期: 
修改原因:
作    者:
详    细: 
********************************************************************************
注意事项
*******************************************************************************/

#include "in_photocoupler.h"

/*GPIO 分组*/
GPIO_TypeDef*  PTCP_GPIO_PORT[] = {PTCP1_GPIO_PORT, PTCP2_GPIO_PORT,PTCP3_GPIO_PORT,
                             PTCP4_GPIO_PORT};
/*GPIO pin*/
const uint16_t PTCP_GPIO_PIN[] = {PTCP1_PIN, PTCP2_PIN, PTCP3_PIN,PTCP4_PIN};
/*GPIO 时钟*/
const uint32_t PTCP_GPIO_CLK[] = {PTCP1_GPIIO_CLK, PTCP2_GPIIO_CLK,PTCP3_GPIIO_CLK,
                             PTCP4_GPIIO_CLK};

/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: 光耦输入初始化
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/
void PTCP_Init(PTCP_TypeDef ptcp)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  assert_param(IS_PTCP(ptcp));

  RCC_APB2PeriphClockCmd(PTCP_GPIO_CLK[ptcp], ENABLE);   //使能PB端口时钟

  GPIO_InitStructure.GPIO_Pin = PTCP_GPIO_PIN[ptcp];     //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;          //上拉输入
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      //IO口速度为50MHz
  GPIO_Init(PTCP_GPIO_PORT[ptcp], &GPIO_InitStructure);  //根据设定参数初始化
  
//  GPIO_SetBits(PTCP_GPIO_PORT[ptcp],PTCP_GPIO_PIN[ptcp]);
}



/*******************************************************************************
函数名字: 
函数功能: 读取光耦状态
创建时间: 
参    数: 
返    回: 
依    赖: 
作    者: 
详细说明: 
*******************************************************************************/
uint8_t PTCP_GetStatus(PTCP_TypeDef ptcp)
{
  /*参考 库 GPIO_ReadOutputDataBit 函数*/
  /*低电平触发, 当读到是1的时候, 说明没有触发, 返回0*/
//  if((PTCP_GPIO_PORT[ptcp]-> IDR & PTCP_GPIO_PIN[ptcp]) != Bit_RESET)
//  {
//    return 0;  
//  }
  return !(PTCP_GPIO_PORT[ptcp]-> IDR & PTCP_GPIO_PIN[ptcp]);
}








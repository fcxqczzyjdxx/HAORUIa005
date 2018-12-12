/*******************************************************************************
文 件 名: timer.c
功    能: 
创建日期: 2018-8-1
作    者: 
详    细: 
********************************************************************************
注意事项
*******************************************************************************/
#include "timer.h"
#include "uart.h"

TIM_TypeDef TIM3_Handler;  

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
void TIM3_Init(void)
{
  TIM_TimeBaseInitTypeDef  tim_init_structure;
  NVIC_InitTypeDef nvic_init_structure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

  tim_init_structure.TIM_ClockDivision  =TIM_CKD_DIV1;
  tim_init_structure.TIM_CounterMode    =TIM_CounterMode_Up;
  tim_init_structure.TIM_Period         =99;//99;//自动装载值  99
  tim_init_structure.TIM_Prescaler      =719;  //设置用来作为TIMx时钟频率除数的预分频值   71
  //tim_init_structure.TIM_RepetitionCounter=
  TIM_TimeBaseInit(TIM3, &tim_init_structure); //根据指定的参数初始化TIMx的时间基数单位

  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIMx中断,允许更新中断


  //中断优先级NVIC设置
  nvic_init_structure.NVIC_IRQChannel = TIM3_IRQn;  //TIM1中断
  nvic_init_structure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
  nvic_init_structure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
  nvic_init_structure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
  NVIC_Init(&nvic_init_structure);
  TIM_Cmd(TIM3, ENABLE);  //使能TIMx
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
}

/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: TIME 中断 被TIME中断调用   1ms 
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/
 void TIM_ElapsedCallback(TIM_TypeDef *handle)
{
  if(handle == TIM3)
  {
    COM_RxMonitor(COM1);
    COM_RxMonitor(COM2);
    COM_RxMonitor(COM3);
    COM_RxMonitor(COM4);
    //COM_RxMonitor(COM5);
  }
}
/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: TIM3 中断
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/
void TIM3_IRQHandler(void)  //1000us
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM1更新中断发生与否
  {
    TIM3->SR = (uint16_t)~TIM_IT_Update;
    //TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );     //清除TIMx更新中断标志 
   TIM_ElapsedCallback(TIM3);
  }
}

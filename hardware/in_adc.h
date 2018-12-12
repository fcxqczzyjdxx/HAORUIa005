/*******************************************************************************
文 件 名: in_adc.h
功    能: 
创建日期: 
作    者:
详    细: 
********************************************************************************
注意事项
*******************************************************************************/


/*******************************************************************************
硬件连接
AD1 -> PC0 ADC123_IN10
AD2 -> PC1 ADC123_IN11
AD3 -> PC2 ADC123_IN12
AD4 -> PC3 ADC123_IN13

*******************************************************************************/

#ifndef __IN_ADC_H__
#define __IN_ADC_H__

#include "stm32f10x.h"
#include "gpio.h"



/*外设地址 数据手册 2.3 */
#define ADC1_ADDRESS  ADC1_BASE
//#define ADC2_ADDRESS  ADC2_BASE
//#define ADC3_ADDRESS  ADC3_BASE

typedef enum
{
  AD1 = 0,
  AD2 = 1,
  AD3 = 2,
  AD4 = 3
} AD_TypeDef;

#define IS_AD(AD)   (((AD)==AD1)||((AD)==AD2)||((AD)==AD3)||((AD)==AD4))
#define ADn         4


/*采集到的数据*/
extern  uint16_t ADCConvertedValue[ADn];
/*转换后的*/
extern  float gfv_ADCConvertedCurrent[ADn];


/*AD DMA相关*/
#define AD_DMA_CLK     RCC_AHBPeriph_DMA1
/*外设地址 数据手册 11.12.14  0X4C是DR寄存器*/
#define AD_DR_ADDRESS  ((u32)ADC1_ADDRESS+0x4c)
#define AD_DMA_CHANNEL DMA1_Channel1  /*DMA1通道1*/
#define AD_DMA_BUF_SIZE   ADn  /*循环数组的数据个数*/


#define AD_CHANNEL_NUM    ADn/*AD需要转换的通道个数*/

#define AD_PCLK        RCC_PCLK2_Div8
#define AD_GPIO_PORT   GPIOC
#define AD_GPIO_CLK    RCC_APB2Periph_GPIOC
/*AD1 模式*/
#define AD1_ADCn        ADC1
#define AD1_CLK         RCC_APB2Periph_ADC1
#define AD1_CHANNEL     ADC_Channel_10
/*AD1 管脚相关*/
#define AD1_PIN         GPIO_Pin_0


/*AD2 模式*/
#define AD2_ADCn        ADC1
#define AD2_CLK         RCC_APB2Periph_ADC1
#define AD2_CHANNEL     ADC_Channel_11
/*AD2 管脚相关*/
#define AD2_GPIO_PORT   GPIOC
#define AD2_PIN         GPIO_Pin_1
#define AD2_GPIO_CLK    RCC_APB2Periph_GPIOC

/*AD3 模式*/
#define AD3_ADCn        ADC1
#define AD3_CLK         RCC_APB2Periph_ADC1
#define AD3_CHANNEL     ADC_Channel_12
/*AD3 管脚相关*/
#define AD3_GPIO_PORT   GPIOC
#define AD3_PIN         GPIO_Pin_2
#define AD3_GPIO_CLK    RCC_APB2Periph_GPIOC

/*AD4 模式*/
#define AD4_ADCn        ADC1
#define AD4_CLK         RCC_APB2Periph_ADC1
#define AD4_CHANNEL     ADC_Channel_13
/*AD4 管脚相关*/
#define AD4_GPIO_PORT   GPIOC
#define AD4_PIN         GPIO_Pin_3
#define AD4_GPIO_CLK    RCC_APB2Periph_GPIOC


void AD_DMA_Init(void);
void ADC_ConvertedToCurrent(void);


uint16_t ADC1_ConvertedToCurrent(void);
uint16_t ADC2_ConvertedToCurrent(void);
uint16_t ADC3_ConvertedToCurrent(void);
uint16_t ADC4_ConvertedToCurrent(void);

























#endif

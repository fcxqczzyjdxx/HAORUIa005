/*******************************************************************************
文 件 名: in_adc.c
功    能: 
创建日期: 2018-8-1
作    者:
详    细: 
********************************************************************************
注意事项
*******************************************************************************/


/*******************************************************************************
连接方式
ADC1 -> PC0 ADC123_IN10
ADC2 -> PC1 ADC123_IN11
ADC3 -> PC2 ADC123_IN12
ADC4 -> PC3 ADC123_IN13

*******************************************************************************/

//__IO uint16_t ADC_ConvertedValue[ADn];  //内存数组

#include "in_adc.h"

uint16_t ADCConvertedValue[ADn];
 float gfv_ADCConvertedCurrent[ADn];

/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: ADC DMA
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/
void AD_DMA_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  ADC_InitTypeDef ADC_InitStructure;
  DMA_InitTypeDef DMA_InitStructure;
  
  /* Configure PC.01 (ADC Channel11) as analog input -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	//adc模式必须是模拟输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);//执行上面的操作
 
  
  /*初始化DMA时钟*/
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  /* Enable ADC1 and GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  
  
/* DMA1 channel1 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel1); //选择DMA的通道1
  //设定从ADC外设的数据寄存器（ADC1_DR_Address）转移到内存（ADCConcertedValue）
  //每次传输大小16位，使用DMA循环传输模式
  DMA_InitStructure.DMA_PeripheralBaseAddr = AD_DR_ADDRESS;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCConvertedValue;//数据缓冲区的地址
  //外设为数据源
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  /*传输总数据个数  4个通道需要4个数据*/  
  DMA_InitStructure.DMA_BufferSize = AD_DMA_BUF_SIZE;
  // 外设地址固定
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  //内存地址增加，多组adc时，使能，数据传输时，内存增加
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  //半字
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  //DMA循环传输
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  //优先级高
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  //??
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  //执行
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  /* ADC1 configuration ------------------------------------------------------*/
  //ADC独立模式	 相对于双重模式
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  //扫描模式用于多通道采集
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  //开启连续转换模式   当转换完本组（可能是一个）继续重新开始执行
  //相对于单次模式：转换一次后就结束
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  //不使用外部触发转换
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  //采集数据右对齐
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  //转换组的通道数目
  ADC_InitStructure.ADC_NbrOfChannel =AD_CHANNEL_NUM;
  //初始化
  ADC_Init(ADC1, &ADC_InitStructure);
  
  //配置ADC时钟，为PCLK2的8分频，即9Hz
  RCC_ADCCLKConfig(RCC_PCLK2_Div8);
  
  //ADC1,ch10,序号1,55.5.。。
  ADC_RegularChannelConfig(ADC1, ADC_Channel_10,1, ADC_SampleTime_239Cycles5);
  //ADC1,ch11,序号1,55.5.。。
  ADC_RegularChannelConfig(ADC1, ADC_Channel_11,2, ADC_SampleTime_239Cycles5);
  //ADC1,ch12,序号1,55.5.。。
  ADC_RegularChannelConfig(ADC1, ADC_Channel_12,3, ADC_SampleTime_239Cycles5);
  //ADC1,ch13,序号1,55.5.。。
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13,4, ADC_SampleTime_239Cycles5);


  //----------------------使能温度传感器----------------------------
  ADC_TempSensorVrefintCmd(ENABLE);

  /* Enable ADC1 DMA */
  //使能ADC_DMA
  ADC_DMACmd(ADC1, ENABLE);
  
  /* Enable ADC1 */
  //使能ADC
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibration register */ 
  //使能ADC1的复位校准寄存器  
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  //等待校准完成
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  //使能ADC1的开始校准寄存器
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  //等待完成
  while(ADC_GetCalibrationStatus(ADC1));
     
  /* Start ADC1 Software Conversion */ 
  //使用软件触发，由于没有采用外部触发
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
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
void ADC_ConvertedToCurrent(void)
{
	gfv_ADCConvertedCurrent[0]=((float)ADCConvertedValue[0])/4096*33;
	gfv_ADCConvertedCurrent[1]=((float)ADCConvertedValue[1])/4096*33;
	gfv_ADCConvertedCurrent[2]=((float)ADCConvertedValue[2])/4096*33;
	gfv_ADCConvertedCurrent[3]=((float)ADCConvertedValue[2])/4096*33;
}
uint16_t ADC1_ConvertedToCurrent(void)
{
	return ADCConvertedValue[0];
}
uint16_t ADC2_ConvertedToCurrent(void)
{
	return ADCConvertedValue[1];
}
uint16_t ADC3_ConvertedToCurrent(void)
{
	return ADCConvertedValue[2];
}
uint16_t ADC4_ConvertedToCurrent(void)
{
	return ADCConvertedValue[3];
}

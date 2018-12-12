/*******************************************************************************
文 件 名: stm32_flash.h
功    能: 内部flash
创建日期: 
作    者:
详    细: 
********************************************************************************
注意事项
*******************************************************************************/

#ifndef __STM32_HFLASH___
#define __STM32_HFLASH___
#include "stm32f10x.h"

//用户根据自己的需要设置
#define INNER_FLASH_SIZE 512 	 		//所选STM32的FLASH容量大小(单位为K)  
#define INNER_FLASH_WREN 1              //使能FLASH写入(0，不是能;1，使能)

//一个扇区的字节个数
#if INNER_FLASH_SIZE<256    
#define INNER_FLASH_SECTOR_SIZE 1024 //字节
#else 
#define INNER_FLASH_SECTOR_SIZE	2048
#endif


//FLASH起始地址
#define INNER_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址



//读取
uint8_t read_inner_flash_buf(uint32_t _ulFlashAddr, uint8_t *_ucpDst, uint32_t _ulSize);

//写入
void write_inner_flash(uint32_t WriteAddr,uint8_t *pBuffer,uint32_t Len);





#endif

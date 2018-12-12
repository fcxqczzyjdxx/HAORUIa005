/*******************************************************************************
文 件 名: flash.c
功    能: 
创建日期: 2018-8-1
作    者: 
详    细: flash
********************************************************************************
注意事项
*******************************************************************************/
#include "stm32_flash.h"



/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: 根据地址计算扇区首地址
依    赖: 
参    数: 
返    回: 扇区首地址
详细说明: 
*******************************************************************************/
uint32_t get_flashInner_sector(uint32_t _ulWrAddr)
{
	uint32_t sector = 0;

	sector = _ulWrAddr & 0xfffff800;

	return sector;
}


/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: 读取stm32内部Flash的内容
依    赖: 
参    数: ucpDst : 目标缓冲区
        _ulFlashAddr : 起始地址
         _ulSize : 数据大小（单位是字节）
返    回:  1=成功，0=失败
详细说明: 
*******************************************************************************/
uint8_t read_inner_flash_buf(uint32_t _ulFlashAddr, uint8_t *_ucpDst, uint32_t _ulSize)
{
	uint32_t i;

	/* 如果偏移地址超过芯片容量，则不改写输出缓冲区 */
	if ((_ulFlashAddr<INNER_FLASH_BASE))//||( _ulFlashAddr + _ulSize > INNER_FLASH_BASE + INNER_FLASH_SIZE*INNER_FLASH_SECTOR_SIZE )
	{
		return 0;
	}

	/* 长度为0时不继续操作,否则起始地址为奇地址会出错 */
	if (_ulSize == 0)
	{
		return 0;
	}

	for (i = 0; i < _ulSize; i++)
	{
		*_ucpDst++ = *(uint8_t *)_ulFlashAddr++;
	}

	return 1;
}



u8 inner_flash_buf[INNER_FLASH_SECTOR_SIZE];

/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: flash 内部的 写入一个数组
          一次写入2个字节  长度注意
依    赖: 
参    数: WriteAddr 起始地址  pBuffer数组地址  Len长度
返    回: 
详细说明: 
*******************************************************************************/
void  writeInner_flash_buf(uint32_t WriteAddr,uint8_t *pBuffer,uint32_t Len)
{
  uint16_t i=0;
  uint16_t usTemp;
  for(i=0;i<Len/2;i++)
  {
    usTemp=pBuffer[2*i];
    usTemp |= (pBuffer[2*i+1]<<8);
      
    if(FLASH_ProgramHalfWord(WriteAddr,usTemp) !=FLASH_COMPLETE)
    {
      break;
    }
    WriteAddr+=2;//地址增加2.
  }
}

/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: 
依    赖: 
参    数:  参数1  flash地址
    参数2  需要写入的数据数组
    参数3  写入的数据个数
返    回: 
详细说明: 
*******************************************************************************/
void write_inner_flash(uint32_t WriteAddr,uint8_t *pBuffer,uint32_t Len)
{
	u32 secpos;	   //扇区地址
	u32 secoff;	   //扇区内偏移地址(16位字计算)
	u32 secremain; //扇区内剩余地址(16位字计算)	   
 	u32 i;    
	u32 offaddr;   //去掉0X08000000后的地址
  
	if(WriteAddr<INNER_FLASH_BASE||(WriteAddr>=(INNER_FLASH_BASE+1024*INNER_FLASH_SIZE)))return;//非法地址
	FLASH_Unlock();						//解锁
  
	offaddr=WriteAddr-INNER_FLASH_BASE;		//实际偏移地址.
	secpos=offaddr/INNER_FLASH_SECTOR_SIZE;			//扇区地址  0~254 for STM32F103RBT6
	secoff=(offaddr%INNER_FLASH_SECTOR_SIZE);		//在扇区内的偏移
	secremain=INNER_FLASH_SECTOR_SIZE-secoff;		//扇区剩余空间大小   
  
	if(Len<=secremain)
    secremain=Len;   //不大于该扇区范围
  
	while(1) 
	{	
//    //读取扇区内容,从写入地址到扇区末尾
		read_inner_flash_buf(secpos*INNER_FLASH_SECTOR_SIZE+INNER_FLASH_BASE,inner_flash_buf,INNER_FLASH_SECTOR_SIZE);//读出整个扇区的内容
//		for(i=0;i<secremain;i++)//校验数据
//		{
//			if(inner_flash_buf[secoff+i]!=0XFF)break;//需要擦除  	  
//		}

//		if(i<secremain)//需要擦除
//		{
			FLASH_ErasePage(secpos*INNER_FLASH_SECTOR_SIZE+INNER_FLASH_BASE);//擦除这个扇区
			for(i=0;i<secremain;i++)//复制
			{
				inner_flash_buf[i+secoff]=pBuffer[i];	  //把后半部分复制到数组中
			}
			writeInner_flash_buf(secpos*INNER_FLASH_SECTOR_SIZE+INNER_FLASH_BASE,inner_flash_buf,INNER_FLASH_SECTOR_SIZE);//写入整个扇区  
//		}
//    else  //不需要擦除就直接写入 
//    {
//      
//      writeInner_flash_buf(WriteAddr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
//		}
    
    if(Len==secremain)  //一个扇区
    {
      break;//写入结束了
    }
		else//写入未结束  多个扇区
		{
			secpos++;				//扇区地址增1
			secoff=0;				//偏移位置为0 	 
		   	pBuffer+=secremain;  	//指针偏移
			WriteAddr+=secremain;	//写地址偏移	   
		   	Len-=secremain;	//字节(16位)数递减
			if(Len>(INNER_FLASH_SECTOR_SIZE))secremain=INNER_FLASH_SECTOR_SIZE;//下一个扇区还是写不完
			else secremain=Len;//下一个扇区可以写完了
		}	 
	};	
	FLASH_Lock();//上锁
}






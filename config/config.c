/*******************************************************************************
文 件 名: config.c
功    能: 设置
创建日期: 2018-8-1
作    者:
详    细: 
********************************************************************************
注意事项
*******************************************************************************/
#include "my_lib.h"
#include "config.h"
#include "uart.h"
#include "stm32_flash.h"

typedef int (*CNF_fun)(int argc, char *argv[]);  /*函数指针*/

#if(CONFIG_MALLOC) /**/
ConfigTapeDef *gtv_CnfTd[CNF_TD_NUM];/*指针数组 存放的都是ConfigTapeDef指针*/
uint8_t content[CNF_CONTENT_NUM];  /*所有配置共用 临时存储 设置内容 写入flash后清空*/

/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能: 动态内存方式创建各种配置
依    赖: 
参    数: 
返    回: 
详细说明: 
详细说明: 
修改时间: 2018-9-17
作    者: 
修改原因: 由于需要大量的堆内存,弃用
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/
void CNF_Init(ConfigTapeDef **ct)
{
  uint8_t i;
  for(i=0;i<CNF_TD_NUM;i++)
  {
    if( ( ct[i]=(ConfigTapeDef*)(malloc(sizeof(ConfigTapeDef))) ) == NULL )
    {
      COM_print(COM1,"cn[%d]内存分配不成功\r\n",i);
    }
    else{
     COM_print(COM1,"cn[%d]内存分配成功\r\n",i); 
     memset( ct[i], 0, sizeof(ConfigTapeDef) );/**/
     ct[i]->con_all.p_content=content;
    }
  }
}
#endif /**/


CNF_TYP *gtp_CnfTyp; /*配置结构指针 在初始化中动态分配*/

/*******************************************************************************
文 件 名: 
功    能: 
创建日期: 
作    者: 
详    细: 
********************************************************************************
注意事项
*******************************************************************************/
void CNF_Init(void)
{
  if( (gtp_CnfTyp=(CNF_TYP*)(malloc(sizeof(CNF_TYP)))) ==NULL)
  {
    COM_print(COM1,"gtp_CNF_Info 内存分配不成功\r\n");
  }
  else
  {
    COM_print(COM1,"gtp_CnfTyp 内存分配成功\r\n");
    read_inner_flash_buf(CNF_ADDRESS,(u8*)gtp_CnfTyp,sizeof(CNF_TYP)); /*读flash*/
    
    COM_print(COM1,"gtp_CnfTyp 内存占用大小 %d\r\n",sizeof(CNF_TYP));
    //COM_print(COM1,"gtp_CnfTyp  %s\r\n", gtp_CnfTyp->GprsInfo.Port);
  }
}
/************************        存储和读取       *****************************/
/*写入数据对比 然后读取 进行对比,,在各模块的写配置里面调用*/
c8 CNF_WriteContrast(c8 *buf)
{
    write_inner_flash(CNF_ADDRESS,(u8*)gtp_CnfTyp,sizeof(CNF_TYP));/*写入flash*/
    /*读取flash  保存到CnfTyp 改数据不包括 begin和补齐位*/
    CNF_TYP CnfTyp; /*读取flash 临时存储,等待返回给电脑配置端*/
    read_inner_flash_buf(CNF_ADDRESS,(u8*)&CnfTyp,sizeof(CNF_TYP));
    if(memcmp((u8*)&CnfTyp,(u8*)gtp_CnfTyp ,sizeof(CNF_TYP))==0)
    {
      COM_print(COM1,"%s@%s:Write ok.#",CNF_WRITE_BEGIN,buf);
      return 1;
    }
    else
    {
      COM_print(COM1,"Error unknown cause write failure.#");
      return 0;
    }
}
/*各模块的读配置参数返回的数据,,在各模块读取配置里面调用*/
c8 CNF_ReadReturn(c8* buf1,c8 *buf2)
{
  COM_print(COM1,"%s@%s:%s#",CNF_READ_BEGIN,buf1,buf2);
  return 1;
}
/*****************************     TEST    ************************************/
int CNF_Test (int argc, char *argv[])
{
  if(argc==1)
  {
    if(strcmp(argv[0],CNF_TEST)==0)
    {
      COM_print(COM1,"Test ok.#"); /*数据识别码错误*/
      return 1;
    }
    else
    {
      COM_print(COM1,"Test no.#"); /*数据识别码错误*/
    }
  }
  return 0;
}
/*****************************    GPRS   **************************************/
/*GPRS写入设置*/
int CNF_WriteGprs(int argc, char *argv[])
{
  if(argc==6)
  {
    gtp_CnfTyp->GprsInfo.Choose= atoi(argv[1]);
    strcpy(gtp_CnfTyp->GprsInfo.Ip,argv[2]);
    strcpy(gtp_CnfTyp->GprsInfo.Realm,argv[3]);
    strcpy(gtp_CnfTyp->GprsInfo.Port,argv[4]);
    gtp_CnfTyp->GprsInfo.BeatTime= atoi(argv[5]);
    gtp_CnfTyp->GprsInfo.DataTime= atoi(argv[6]);
    CNF_WriteContrast(argv[0]);
  }
  else
  {
    COM_print(COM1,"Error data length.#");
    return 0;
  }
  return 0;
}
/*GPRS读取配置*/
int CNF_ReadGprs(int argc, char *argv[])
{
  c8 buf[100];
  sprintf(buf,"%d,%s,%s,%s,%d,%d",gtp_CnfTyp->GprsInfo.Choose,gtp_CnfTyp->GprsInfo.Ip,
                                  gtp_CnfTyp->GprsInfo.Realm,gtp_CnfTyp->GprsInfo.Port, 
                                  gtp_CnfTyp->GprsInfo.BeatTime,gtp_CnfTyp->GprsInfo.DataTime);
  CNF_ReadReturn(argv[0],buf);
  return 1;
}
/**************************      ANALOG_IN_INFO       *************************/
int CNF_WriteAnalogIn(int argc, char *argv[])
{
  u8 select=0;
  if(argc==4)
  {
    if(strcmp(argv[0],CNF_ANALOG_IN1)==0)       { select =0; }
    else if(strcmp(argv[0],CNF_ANALOG_IN2)==0)  { select =1; }
    else if(strcmp(argv[0],CNF_ANALOG_IN3)==0)  { select =2; }
    else                                        { select =3; }

    gtp_CnfTyp->AnalogInInfo[select].Flag = atoi(argv[1]);
    gtp_CnfTyp->AnalogInInfo[select].Compute=atoi(argv[2]);
    gtp_CnfTyp->AnalogInInfo[select].ComputeUpper = atol(argv[3]);
    gtp_CnfTyp->AnalogInInfo[select].ComputeLower = atol(argv[4]);
    CNF_WriteContrast(argv[0]);
  }
  else
  {
    COM_print(COM1,"Error data length.#");
    return 0;
  }
  return 0;
}
int CNF_ReadAnalogIn(int argc, char *argv[])
{
  u8 select=0;
    if(strcmp(argv[0],CNF_ANALOG_IN1)==0)       { select =0; }
    else if(strcmp(argv[0],CNF_ANALOG_IN2)==0)  { select =1; }
    else if(strcmp(argv[0],CNF_ANALOG_IN3)==0)  { select =2; }
    else                                        { select =3; }
      
  c8 buf[100];
  sprintf(buf,"%d,%d,%ld,%ld",gtp_CnfTyp->AnalogInInfo[select].Flag,
                            gtp_CnfTyp->AnalogInInfo[select].Compute,
                            gtp_CnfTyp->AnalogInInfo[select].ComputeUpper,
                            gtp_CnfTyp->AnalogInInfo[select].ComputeLower);
  CNF_ReadReturn(argv[0],buf);
  return 1;
}
/*********************     DIGITAL_IN_INFO      *******************************/
int CNF_WriteDigitalIn(int argc, char *argv[])
{
  u8 select=0;
  if(argc==1)
  { //COM_print(COM1,"%s\r\n",argv[0]);
    if(strcmp(argv[0],CNF_DIGITAL_IN1)==0)       { select =0;}
    else if(strcmp(argv[0],CNF_DIGITAL_IN2)==0)  { select =1; }
    else if(strcmp(argv[0],CNF_DIGITAL_IN3)==0)  { select =2; }
    else                                        { select =3; }
      
    gtp_CnfTyp->DigitalInInfo[select].Flag = atoi(argv[1]);
    CNF_WriteContrast(argv[0]);
  }
  else
  {
    COM_print(COM1,"Error data length.#");
    return 0;
  }
  return 0;
}
int CNF_ReadDigitalIn(int argc, char *argv[])
{
  u8 select=0;
    if(strcmp(argv[0],CNF_DIGITAL_IN1)==0)       { select =0; }
    else if(strcmp(argv[0],CNF_DIGITAL_IN2)==0)  { select =1; }
    else if(strcmp(argv[0],CNF_DIGITAL_IN3)==0)  { select =2; }
    else                                         { select =3; }
      
  c8 buf[100];
  sprintf(buf,"%d",gtp_CnfTyp->DigitalInInfo[select].Flag);
  CNF_ReadReturn(argv[0],buf);
  return 1;
}
/*************************    DIGITAL_OUT_INFO     ****************************/
int CNF_WriteDigitalOut(int argc, char *argv[])
{
  u8 select=0;
  if(argc==1)
  {
    if(strcmp(argv[0],CNF_DIGITAL_OUT1)==0)       { select =0; }
    else if(strcmp(argv[0],CNF_DIGITAL_OUT2)==0)  { select =1; }
    else if(strcmp(argv[0],CNF_DIGITAL_OUT3)==0)  { select =2; }
    else                                          { select =3; }
      
    gtp_CnfTyp->DigitalOutInfo[select].Flag = atoi(argv[1]);
    CNF_WriteContrast(argv[0]);
  }
  else
  {
    COM_print(COM1,"Error data length.#");
    return 0;
  }
  return 0;
}
int CNF_ReadDigitalOut(int argc, char *argv[])
{
  u8 select=0;
    if(strcmp(argv[0],CNF_DIGITAL_OUT1)==0)       { select =0; }
    else if(strcmp(argv[0],CNF_DIGITAL_OUT2)==0)  { select =1; }
    else if(strcmp(argv[0],CNF_DIGITAL_OUT3)==0)  { select =2; }
    else                                          { select =3; }

  c8 buf[100];
  sprintf(buf,"%d",gtp_CnfTyp->DigitalOutInfo[select].Flag);
  CNF_ReadReturn(argv[0],buf);
  return 1;
}
/**************************      MODBUS_INFO     ******************************/
int CNF_WriteModbus(int argc, char *argv[])
{
  u8 select=0;
  if(argc==5)
  {
    if(strcmp(argv[0],CNF_MODBUS1)==0)       { select =0;}
    else if(strcmp(argv[0],CNF_MODBUS2)==0)  { select =1;}
    else if(strcmp(argv[0],CNF_MODBUS3)==0)  { select =2;}
    else if(strcmp(argv[0],CNF_MODBUS4)==0)  { select =3;}
    else if(strcmp(argv[0],CNF_MODBUS5)==0)  { select =4;}
    else if(strcmp(argv[0],CNF_MODBUS6)==0)  { select =5;}
    else if(strcmp(argv[0],CNF_MODBUS7)==0)  { select =6;}
    else if(strcmp(argv[0],CNF_MODBUS8)==0)  { select =7;}
    else if(strcmp(argv[0],CNF_MODBUS9)==0)  { select =8;}
    else if(strcmp(argv[0],CNF_MODBUS10)==0)  { select =9;}
    else if(strcmp(argv[0],CNF_MODBUS11)==0)  { select =10;}
    else if(strcmp(argv[0],CNF_MODBUS12)==0)  { select =11;}
    else if(strcmp(argv[0],CNF_MODBUS13)==0)  { select =12;}
    else if(strcmp(argv[0],CNF_MODBUS14)==0)  { select =13;}
    else if(strcmp(argv[0],CNF_MODBUS15)==0)  { select =14;}
    else if(strcmp(argv[0],CNF_MODBUS16)==0)  { select =15;}
    else if(strcmp(argv[0],CNF_MODBUS17)==0)  { select =16;}
    else if(strcmp(argv[0],CNF_MODBUS18)==0)  { select =17;}
    else if(strcmp(argv[0],CNF_MODBUS19)==0)  { select =18;}
    else                                      { select =19;}
      
    gtp_CnfTyp->ModbusInfo[select].Flag = atoi(argv[1]);
    gtp_CnfTyp->ModbusInfo[select].Slave = atoi(argv[2]);
    gtp_CnfTyp->ModbusInfo[select].Function = atoi(argv[3]);
    gtp_CnfTyp->ModbusInfo[select].Register[0] = atoi(argv[4]);
    gtp_CnfTyp->ModbusInfo[select].Register[1] = atoi(argv[4])>>8;
    gtp_CnfTyp->ModbusInfo[select].Type = atoi(argv[5]);
    
    CNF_WriteContrast(argv[0]);
  }
  else
  {
    COM_print(COM1,"Error data length.#");
    return 0;
  }
  return 0;
}
int CNF_ReadModbus(int argc, char *argv[])
{
  u8 select=0;
    if(strcmp(argv[0],CNF_MODBUS1)==0)       { select =0;}
    else if(strcmp(argv[0],CNF_MODBUS2)==0)  { select =1;}
    else if(strcmp(argv[0],CNF_MODBUS3)==0)  { select =2;}
    else if(strcmp(argv[0],CNF_MODBUS4)==0)  { select =3;}
    else if(strcmp(argv[0],CNF_MODBUS5)==0)  { select =4;}
    else if(strcmp(argv[0],CNF_MODBUS6)==0)  { select =5;}
    else if(strcmp(argv[0],CNF_MODBUS7)==0)  { select =6;}
    else if(strcmp(argv[0],CNF_MODBUS8)==0)  { select =7;}
    else if(strcmp(argv[0],CNF_MODBUS9)==0)  { select =8;}
    else if(strcmp(argv[0],CNF_MODBUS10)==0)  { select =9;}
    else if(strcmp(argv[0],CNF_MODBUS11)==0)  { select =10;}
    else if(strcmp(argv[0],CNF_MODBUS12)==0)  { select =11;}
    else if(strcmp(argv[0],CNF_MODBUS13)==0)  { select =12;}
    else if(strcmp(argv[0],CNF_MODBUS14)==0)  { select =13;}
    else if(strcmp(argv[0],CNF_MODBUS15)==0)  { select =14;}
    else if(strcmp(argv[0],CNF_MODBUS16)==0)  { select =15;}
    else if(strcmp(argv[0],CNF_MODBUS17)==0)  { select =16;}
    else if(strcmp(argv[0],CNF_MODBUS18)==0)  { select =17;}
    else if(strcmp(argv[0],CNF_MODBUS19)==0)  { select =18;}
    else                                      { select =19;}

  c8 buf[100];
  sprintf(buf,"%d,%d,%d,%d,%d",gtp_CnfTyp->ModbusInfo[select].Flag,
                            gtp_CnfTyp->ModbusInfo[select].Slave,
                            gtp_CnfTyp->ModbusInfo[select].Function,
                            (gtp_CnfTyp->ModbusInfo[select].Register[0]+(gtp_CnfTyp->ModbusInfo[select].Register[1]<<8)),
                            gtp_CnfTyp->ModbusInfo[select].Type);
  CNF_ReadReturn(argv[0],buf);
  return 1;
}

/******************************************************************************/
/*返回 读写配置相应的函数指针,,根据 不同的指令返回不同模块的函数指针*/
CNF_fun CNF_SearchCallFun(c8* p,u8 select)
{
  switch (select)
  {
    /*写入*/
    case 0: 
      if(strcmp(p,CNF_TEST)==0)
            {return CNF_Test;}
      else if(strcmp(p,CNF_GPRS)==0) /*GPRS 的设置*/
            {return CNF_WriteGprs;}
      else if(strcmp(p,CNF_ANALOG_IN1)==0||strcmp(p,CNF_ANALOG_IN2)==0||
              strcmp(p,CNF_ANALOG_IN3)==0||strcmp(p,CNF_ANALOG_IN4)==0)
            {return CNF_WriteAnalogIn;}
						
      else if(strcmp(p,CNF_DIGITAL_IN1)==0||strcmp(p,CNF_DIGITAL_IN2)==0||
              strcmp(p,CNF_DIGITAL_IN3)==0||strcmp(p,CNF_DIGITAL_IN4)==0)
            {return CNF_WriteDigitalIn;}
						
      else if(strcmp(p,CNF_DIGITAL_OUT1)==0||strcmp(p,CNF_DIGITAL_OUT2)==0||
              strcmp(p,CNF_DIGITAL_OUT3)==0||strcmp(p,CNF_DIGITAL_OUT4)==0)
            {return CNF_WriteDigitalOut;}
						
      else if(strcmp(p,CNF_MODBUS1)==0||strcmp(p,CNF_MODBUS2)==0||
             strcmp(p,CNF_MODBUS3)==0||strcmp(p,CNF_MODBUS4)==0||
             strcmp(p,CNF_MODBUS5)==0||strcmp(p,CNF_MODBUS6)==0||
             strcmp(p,CNF_MODBUS7)==0||strcmp(p,CNF_MODBUS8)==0||
             strcmp(p,CNF_MODBUS9)==0||strcmp(p,CNF_MODBUS10)==0||
             strcmp(p,CNF_MODBUS11)==0||strcmp(p,CNF_MODBUS12)==0||
             strcmp(p,CNF_MODBUS13)==0||strcmp(p,CNF_MODBUS14)==0||
             strcmp(p,CNF_MODBUS15)==0||strcmp(p,CNF_MODBUS16)==0||
             strcmp(p,CNF_MODBUS17)==0||strcmp(p,CNF_MODBUS18)==0||
             strcmp(p,CNF_MODBUS19)==0||strcmp(p,CNF_MODBUS20)==0)
            {return CNF_WriteModbus;}
    break ;
    /*读取*/
    case 1: 
      if(strcmp(p,CNF_GPRS)==0) /*GPRS 的设置*/
            {return CNF_ReadGprs;}
      else if(strcmp(p,CNF_ANALOG_IN1)==0||strcmp(p,CNF_ANALOG_IN2)==0||
             strcmp(p,CNF_ANALOG_IN3)==0||strcmp(p,CNF_ANALOG_IN4)==0)
            {return CNF_ReadAnalogIn;}
						
      else if(strcmp(p,CNF_DIGITAL_IN1)==0||strcmp(p,CNF_DIGITAL_IN2)==0||
             strcmp(p,CNF_DIGITAL_IN3)==0||strcmp(p,CNF_DIGITAL_IN4)==0)
            {return CNF_ReadDigitalIn;}
						
      else if(strcmp(p,CNF_DIGITAL_OUT1)==0||strcmp(p,CNF_DIGITAL_OUT2)==0||
              strcmp(p,CNF_DIGITAL_OUT3)==0||strcmp(p,CNF_DIGITAL_OUT4)==0)
            {return CNF_ReadDigitalOut;}
						
      else if(strcmp(p,CNF_MODBUS1)==0||strcmp(p,CNF_MODBUS2)==0||
              strcmp(p,CNF_MODBUS3)==0||strcmp(p,CNF_MODBUS4)==0||
              strcmp(p,CNF_MODBUS5)==0||strcmp(p,CNF_MODBUS6)==0||
              strcmp(p,CNF_MODBUS7)==0||strcmp(p,CNF_MODBUS8)==0||
              strcmp(p,CNF_MODBUS9)==0||strcmp(p,CNF_MODBUS10)==0||
              strcmp(p,CNF_MODBUS11)==0||strcmp(p,CNF_MODBUS12)==0||
              strcmp(p,CNF_MODBUS13)==0||strcmp(p,CNF_MODBUS14)==0||
              strcmp(p,CNF_MODBUS15)==0||strcmp(p,CNF_MODBUS16)==0||
              strcmp(p,CNF_MODBUS17)==0||strcmp(p,CNF_MODBUS18)==0||
              strcmp(p,CNF_MODBUS19)==0||strcmp(p,CNF_MODBUS20)==0)
            {return CNF_ReadModbus;}
    default :
      break;
  }
  return NULL;
}
/******************************************************************************/
/*
输入格式  开头@功能码:数据1,数据2,数据3#  #代表结束
*/




void CNF_Data(COM_RxTypeDef *data ,char *dat)
{

  c8 buf[128];
  c8 *pBuf=NULL;
  u16 num=0;
  c8* argv[10];
  CNF_fun cnfFun=NULL;
  
  strcpy(buf,data->RxBuf); /*数据拷贝到buf里面*/
  //COM_print(COM1,"%d %d\r\n",strlen(buf),buf[strlen(buf)-1]);
  if(buf[strlen(buf)-1]!='#')  /*结尾不是#号*/
  {
    COM_print(COM1,"Error data  no '#' at the end.#"); /* 结尾不是# 错误*/
    return;
  }
  if(strstr(buf,CNF_MODE)!=0||strstr(buf,CNF_MODE_END)!=0)
  {
    COM_print(COM1,"模式切换"); /* 结尾不是# 错误*/
    return;
  }
  
  pBuf=strtok(buf,"@"); /*数据第一次分割,,把@前面的数据分出来*/
  //COM_print(COM1,"%d %d\r\n",strlen(buf),buf[strlen(buf)-1]);
  if(strcmp(pBuf,CNF_WRITE_BEGIN)==0||strcmp(pBuf,CNF_READ_BEGIN)==0)  /*对比 数据开头,如果不相同 数据错误*/
  {
    u8 select=0;
    if(strcmp(pBuf,CNF_WRITE_BEGIN)==0) select=0;
    else select=1;
      
    pBuf = strtok(NULL,":");  /*数据第二次分割 ,提取功能码\识别码*/
    cnfFun=CNF_SearchCallFun(pBuf,select); /*根据pBuf的字符串 把合适的函数返回,select 是读写的区分*/
    argv[num++]=pBuf;  /*  这个数据可能有用  在 模拟量输入 数字量输入 数字量输出 modbus里面都用用*/
    /*函数指针不为空*/
    if(cnfFun) 
    {
      while((pBuf=strtok(NULL,",#"))!=0)
      {
        argv[num++]=pBuf;
      }
      //COM_print(COM1,"%d\r\n",num);
      cnfFun(num-1,argv);
    }
    else  /*没有对应的处理函数*/
    {
      COM_print(COM1,"Error data module id code.#"); /*数据识别码错误*/
      return;
    }
  }

  
  else{COM_print(COM1,"Error data module name.#"); /*数据名字错误*/
    return;}
 } 
/******************************************************************************/
  //strcmp("HR@CNF:", dat);
  
//  if((data->RxBuf[0]=='H')&&(data->RxBuf[1]=='R')&&(data->RxBuf[2]=='@')&&(data->RxBuf[3]=='C')&&
//     (data->RxBuf[4]=='N')&&(data->RxBuf[5]=='F')&&(data->RxBuf[6]==':'))/*HR@CNF: 读取数据*/
//  {
//    
//    memcpy((c8*)gtp_CnfTyp,data->RxBuf,sizeof(CNF_TYP)-sizeof(CNF_INFO));
//    
//    read_inner_flash_buf(CNF_ADDRESS,(u8*)&gtp_CnfTyp->CnfInfo,sizeof(CNF_INFO));/*读取*/
//    COM_Write(COM1,(c8*)gtp_CnfTyp,sizeof(CNF_TYP));/*不用读取直接发送,在初始化的时候读取过*/
//  }
//  
// if(data->RxNum == sizeof(CNF_TYP))
// {
//    if((data->RxBuf[0]=='H')&&(data->RxBuf[1]=='R')&&(data->RxBuf[2]=='$')&&(dat[3]=='C')&&
//       (data->RxBuf[4]=='N')&&(data->RxBuf[5]=='F')&&(data->RxBuf[6]==':'))/*HR$CNF: 写入数据*/
//    {
//      memcpy((u8*)gtp_CnfTyp,data->RxBuf,sizeof(CNF_TYP));
//      //COM_Write(COM1,(c8*)dat,sizeof(CNF_TYP));

//      CNF_TYP CnfTyp; /*读取flash 临时存储,等待返回给电脑配置端*/
//      memcpy((u8*)&CnfTyp,data->RxBuf,sizeof(CNF_TYP));  /*先把该结构填充,,防止出现 补齐位出现问题*/
//      //memcpy(gtp_CnfTyp->CnfInfo,dat+7,sizeof(CNF_INFO)); /*把数据拷贝到gtp_CNF_Info用来存储*/
//      
//      write_inner_flash(CNF_ADDRESS,(u8*)&gtp_CnfTyp->CnfInfo,sizeof(CNF_INFO));/*写入flash*/
//      /*读取flash  保存到CnfTyp 改数据不包括 begin和补齐位*/
//      read_inner_flash_buf(CNF_ADDRESS,(u8*)&CnfTyp.CnfInfo,sizeof(CNF_INFO));
//      
//      //memcpy((c8*)CnfTyp.begin,dat,sizeof(gtp_CnfTyp->begin));
//      
//      COM_Write(COM1,(c8*)&CnfTyp,sizeof(CNF_TYP));
//      //if(memcmp((u8*)&CnfTyp.CnfInfo,(u8*)&gtp_CnfTyp->CnfInfo ,sizeof(CNF_INFO))==0)/*写入与读取对比*/
//      //{
//      //  COM_Write(COM1,"OK",2);
//      //}
//      
//    }
 // }
//}

//进入配置模式
_Bool CNF_mode(COM_RxTypeDef *data)
{
  c8 buf[128];
  static _Bool A=0;
  strcpy(buf,data->RxBuf); /*数据拷贝到buf里面*/
  //COM_print(COM1,"%s.#",buf); 
  if(strstr(buf,CNF_MODE)!=0)  //进入配置模式
  {
    COM_print(COM1,"%s.#",CNF_MODE); 
    A=1;
  }
  else if(strstr(buf,CNF_MODE_END)!=0)
  {
    COM_print(COM1,"%s.#",CNF_MODE_END);
    A=0;
  }
  return A;
}











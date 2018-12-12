/*******************************************************************************
文 件 名: config.c
功    能: 
创建日期: 2018-8-1
作    者:
邮    箱:
详    细: 配置
修改日期: 
修改原因:
作    者:
邮    箱: 
详    细: 
********************************************************************************
注意事项
*******************************************************************************/
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "uart.h"
#include "stm32_flash.h"

#define CONFIG_MALLOC     0
#if(CONFIG_MALLOC) /*动态内存方式写配置,由于需要大量的堆内存,而弃用*/
/*函数指针声明*/
//typedef uint8_t (*CNF_CALL) (uint8_t* );
#define CNF_CONTENT_NUM  200  /*配置的具体参数大小*/
#define CNF_TD_NUM       30   /*配置结构的数组大小*/
/*配置字符串和函数*/
typedef struct
{
  uint8_t *p_content;     /*配置的具体参数,指向一个固定数组存储数据*/ 
  uint8_t (*CNF_CALL) (uint8_t* );      /*回调函数指针*/
} Cnf_ConCall;
/*配置结构体*/
/*使用动态内存分配方式获取结构大小*/
typedef struct
{
  struct ConfigTapeDef *CNF_TD;
  uint8_t distinguish[16]; /*配置识别码 指向相应的配置名字*/
  Cnf_ConCall con_all;
}ConfigTapeDef;
extern ConfigTapeDef *gtv_CnfTd[CNF_TD_NUM];
extern void CNF_Init(ConfigTapeDef **ct);
#endif /**/

#define CNF_MODE    "CNFCNF"
#define CNF_MODE_END "CNFEND"
#define CNF_TEST    "TEST"
//#define CNF_TEST    "TEST"
#define CNF_WRITE_BEGIN  "QHHRW"  /*设置指令的开头*/
#define CNF_READ_BEGIN  "QHHRR"   /*读取指令的开头*/
#define CNF_GPRS  "GPRS"
#define CNF_ANALOG_IN1 "ANALOGIN1" /*模拟输入*/
#define CNF_ANALOG_IN2 "ANALOGIN2" /*模拟输入*/
#define CNF_ANALOG_IN3 "ANALOGIN3" /*模拟输入*/
#define CNF_ANALOG_IN4 "ANALOGIN4" /*模拟输入*/

#define CNF_DIGITAL_IN1 "DIGITALIN1" /*数字输入*/
#define CNF_DIGITAL_IN2 "DIGITALIN2" /*数字输入*/
#define CNF_DIGITAL_IN3 "DIGITALIN3" /*数字输入*/
#define CNF_DIGITAL_IN4 "DIGITALIN4" /*数字输入*/

#define CNF_DIGITAL_OUT1 "DIGITALOUT1" /*数字输出*/
#define CNF_DIGITAL_OUT2 "DIGITALOUT2" /*数字输出*/
#define CNF_DIGITAL_OUT3 "DIGITALOUT3" /*数字输出*/
#define CNF_DIGITAL_OUT4 "DIGITALOUT4" /*数字输出*/

#define CNF_MODBUS1  "MODBUS1"  /*数字输出*/
#define CNF_MODBUS2  "MODBUS2"  /*数字输出*/
#define CNF_MODBUS3  "MODBUS3"  /*数字输出*/
#define CNF_MODBUS4  "MODBUS4"  /*数字输出*/
#define CNF_MODBUS5  "MODBUS5"  /*数字输出*/
#define CNF_MODBUS6  "MODBUS6"  /*数字输出*/
#define CNF_MODBUS7  "MODBUS7"  /*数字输出*/
#define CNF_MODBUS8  "MODBUS8"  /*数字输出*/
#define CNF_MODBUS9  "MODBUS9"  /*数字输出*/
#define CNF_MODBUS10 "MODBUS10" /*数字输出*/
#define CNF_MODBUS11 "MODBUS11" /*数字输出*/
#define CNF_MODBUS12 "MODBUS12" /*数字输出*/
#define CNF_MODBUS13 "MODBUS13" /*数字输出*/
#define CNF_MODBUS14 "MODBUS14" /*数字输出*/
#define CNF_MODBUS15 "MODBUS15" /*数字输出*/
#define CNF_MODBUS16 "MODBUS16" /*数字输出*/
#define CNF_MODBUS17 "MODBUS17" /*数字输出*/
#define CNF_MODBUS18 "MODBUS18" /*数字输出*/
#define CNF_MODBUS19 "MODBUS19" /*数字输出*/
#define CNF_MODBUS20 "MODBUS20" /*数字输出*/



#define CNF_ADDRESS 0x807d800  /*存储开始地址*/
/*ID_SN*/
typedef  struct
{
    u32       ID;  //
    c8        SN[16]; /*唯一识别码,15个字节*/  
}ID_SN_INFO;
/*GPRS参数*/
typedef struct
{
  u8  Choose;      /*0 不管用,1 IP方式,2 域名方式*/
  u8  Reserve1[3];         /*预留,4字节补齐*/
  c8  Ip[16];      /*IP地址*/
  c8  Realm[32];   /*域名方式*/
  c8  Port[6];     /*端口号*/
  u8  Reserve0[2];         /*预留,4字节补齐*/
  u16  BeatTime;    /*心跳包时间间隔,单位S*/
  u16  DataTime;    /*数据包时间间隔,单位S*/
}GPRS_INFO;
/*模拟输入 参数*/
typedef struct
{

  u8 Flag ;    /*使能标志*/
  u8 Compute;  /*计算方式 0 是不计算一电流值输出,,1是计算,,需要提供 变换后的上下限*/
  u8  Reserve0; /*预留,4字节补齐*/
  u8  Reserve1; /*预留,4字节补齐*/
  s32 ComputeUpper;  /*上限+-数*/ 
  s32 ComputeLower ; /*下限+-数*/

}ANALOG_IN_INFO;
/*数字输入 参数*/
typedef struct
{
  u8 Flag ;    /*使能标志*/
  u8 Reserve0[3];/*预留,4字节补齐*/
}DIGITAL_IN_INFO;
/*数字输出参数*/
typedef struct
{
  u8 Flag;    /*使能标志,0不使用,1是常开,2是常闭*/
  //u8 State;   /*初始化状态 0是默认 1是打开,2是关闭*/
  u8 Reserve0[3];/*预留,4字节补齐*/
}DIGITAL_OUT_INFO;
/*485 参数*/
typedef struct
{
  u8 Flag;    /*使能标志 0 不使能 1 RS485 2 RS232*/
  u8 Slave;    /*从机地址*/
  u8 Function; /*功能码*/
  u8 Reserve0; /*站位预留,4字节补齐*/
  u8 Register[2];   /*从机寄存器*/
  u8 Type;     /*读数据的类型,和读到后的处理方法 0 两字节不做处理,1 量子界*/
  u8 Reserve1; /*站位预留,4字节补齐*/
}MODBUS_INFO;



/*配置结构,包括全部,以4字节对齐*/
typedef struct
{
  //u8 begin[8]; /*开头*/
  //u8 Reserve0;
  ID_SN_INFO IdSnInfo;
  GPRS_INFO GprsInfo;
  ANALOG_IN_INFO   AnalogInInfo[4];
  DIGITAL_IN_INFO  DigitalInInfo[4];
  DIGITAL_OUT_INFO DigitalOutInfo[4];
  MODBUS_INFO ModbusInfo[20];
}CNF_TYP;

//typedef struct
//{
    //u8 begin[7]; /*开头*/
    //u8 Reserve0;
//    CNF_INFO CnfInfo;
//}CNF_TYP;


extern CNF_TYP *gtp_CnfTyp; /*配置结构指针 在初始化中动态分配*/

void CNF_Init(void);

void CNF_Data(COM_RxTypeDef *data ,char *dat);
_Bool CNF_mode(COM_RxTypeDef *data);

//void CNF_Test (char * dat);

#endif

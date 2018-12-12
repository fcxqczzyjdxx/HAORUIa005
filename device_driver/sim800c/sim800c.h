#ifndef _SIM800C_H_
#define _SIM800C_H_


#include "stm32f10x_conf.h"

#include "stm32f10x.h"
#include "my_lib.h"
#include "uart.h"

#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志

extern state_type sim800cInterFlag;
extern COM_RxTypeDef sim800cBuf;;

state_type sim800c_WaitRecive(void);
unsigned char *sim800c_GetIPD(unsigned short timeOut);
state_type sim800c_Init(void);
state_type sim800c_SendData(c8 *data, unsigned short len);


state_type sim800c_SendCmd(char *cmd, char *res,uint32_t timeOut);
//#include "app_config.h"

//extern  uint8_t g_sim_get_buf[1024];
//extern  uint32_t g_sim_get_len;

uint8_t sim800c_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime);

uint8_t* sim800c_check_cmd(uint8_t flag,uint8_t *str);

uint8_t*  sim800c_realm_write(uint8_t * pBuf1,uint8_t * pBuf2,uint8_t * pBuf3);


#endif

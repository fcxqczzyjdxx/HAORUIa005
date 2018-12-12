/*
*********************************************************************************************************
*
*	模块名称 : 字符串操作\数值转换
*	文件名称 : bsp_user_lib.h
*	版    本 : V1.2
*	说    明 : 头文件
*
*********************************************************************************************************
*/

#ifndef __BSP_USER_LIB_H
#define __BSP_USER_LIB_H
#include "bsp.h"

//modbus crc 校验 
uint16 mbCRC16(uint8 *updata, uint32 len);
int str_len(char *_str);
void str_cpy(char *_tar, char *_src);
int str_cmp(char * s1, char * s2);
void mem_set(char *_tar, char _data, int _len);

void int_to_str(int _iNumber, char *_pBuf, unsigned char _len);
int str_to_int(char *_pStr);

uint16_t BEBufToUint16(uint8_t *_pBuf);
uint16_t LEBufToUint16(uint8_t *_pBuf);

uint32_t BEBufToUint32(uint8_t *_pBuf);
uint32_t LEBufToUint32(uint8_t *_pBuf);

uint16_t CRC16_Modbus(uint8_t *_pBuf, uint16_t _usLen) ;
int32_t  CaculTwoPoint(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x);

char BcdToChar(uint8_t _bcd);



void HexToAscll(uint8_t * _pHex, char *_pAscii, uint16_t _BinBytes);
uint32_t AsciiToUint32(char *pAscii);

float BEBufToFloat(uint8_t *p);


uint32 bufTOuint32(uint8 *pbuf); 
/*
pszInput:输入待分割字符串
pszDelimiters:分割标识符
uiAry_num:分割的份数
uiAry_size:每份的size
pszAry_out:分割的子串的输出参数
*/
int separate_string_to_array( char *pszInput,
                                char *pszDelimiters ,
                                  unsigned int Ary_num,
                                    unsigned int Ary_size,
                                      char *pszAry_out);
                                    
                                    
//pszInput:输入待分割字符串
//pszDelimiters:分割标识符
//uiAry_num:分割的份数
//uiAry_size:每份的size尺寸大小
//pszAry_out:分割的子串的输出参数
uint16 str_str(char * pszInput,char * pszDelimiters,
                                    unsigned int Ary_num,
                                    unsigned int Ary_size,
                                      char *pszAry_out);
                                    
//函数名 :separate_as_string_to_array
//功能   :  一个数组或字符串  把其中出现的某一个字符为界 分割成n份
//        例如 aaa\0\0\0bbb\0cc\0\0dddd\0  分割成aaa  bbb cc  dddd  
//pszInput:输入待分割字符串
//pszDelimiters:分割标识符
//uiAry_size:每份的size尺寸大小
//long_num  : 输入数组最大长度
//pszAry_out:分割的子串的输出参数
//返回份数
int str_as_str(uint8   *pszInput,
                                 char     *pszDelimiters,
                                 uint32   Ary_size,
                                 uint32   long_num, 
                                 uint8    *pszAry_out);
                                 
                                 
                                 
//16点法求平均值
 float average(float val);
#endif

/*****************************  *********************************/

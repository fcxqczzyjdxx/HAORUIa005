/*******************************************************************************
文 件 名: my_lib.h
功    能: 脱离底层函数
创建日期: 2018-8-1
作    者: 
邮    箱: 
详    细: 
修改日期: 
修改原因: 
作    者: 
邮    箱: 
详    细: 
********************************************************************************
注意事项
*******************************************************************************/
#ifndef __my_lib_h__
#define __my_lib_h__

#define ARM_C_LIB
//#define ARM_CPP_LIB

#ifdef  ARM_C_LIB
//#include <alloca.h>
//#include <arm_neon.h>
//#include <armdsp.h>
//#include <assert.h>
//#include <c55x.h>
//#include <complex.h>
//#include <ctype.h>
//#include <deque_spec.h>
//#include <dspfns.h>
//#include <errno.h>
//#include <fenv.h>
//#include <float.h>
//#include <inttypes.h>
//#include <iso646.h>
//#include <limits.h>
//#include <list_spec.h>
//#include <locale.h>
//#include <math.h>
//#include <rt_fp.h>
//#include <rt_heap.h>
//#include <rt_locale.h>
//#include <rt_misc.h>
//#include <rt_sys.h>
//#include <setjmp.h>
//#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>
#include <time.h>
//#include <vector_spec.h>
//#include <wchar.h>
//#include <wctype.h> 
#endif

#ifdef  ARM_CPP_LIB

#endif





//用于switch选择比较好
#define MK_INT(a,b,c,d)  (a + b * 256 + c * 256*256+d*256*256*256)

//modbus crc 校验 
uint16_t mbCRC16(uint8_t *updata, uint32_t len);
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


uint32_t bufTOuint32(uint8_t *pbuf); 
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
uint16_t str_str(char * pszInput,char * pszDelimiters,
                                    unsigned int Ary_num,
                                    unsigned int Ary_size,
                                      char *pszAry_out);
                                    
//函数名 :separate_as_string_to_array
//功能   :  一个数组或字符串  把其中出现的某一个字符为界 分割成n份并用 逗号 连接
//        例如 aaa\0\0\0bbb\0cc\0\0dddd\0  分割成aaa  bbb cc  dddd 连接aaa,bbb,cc,dddd, 
//pszInput:输入待分割字符串
//pszDelimiters:分割标识符
//uiAry_size:每份的size尺寸大小
//long_num  : 输入数组最大长度
//pszAry_out:分割的子串的输出参数
//返回份数
int str_as_str(uint8_t   *pszInput,
                                 char     *pszDelimiters,
                                 uint32_t   Ary_size,
                                 uint32_t   long_num, 
                                 uint8_t    *pszAry_out);
                                 
                                 
                                 
//16点法求平均值
 float average(float val);



#endif

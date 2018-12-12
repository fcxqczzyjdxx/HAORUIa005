/*
*********************************************************************************************************
*
*	模块名称 : 字符串操作\数值转换
*	文件名称 : bsp_user_lib.c
*	版    本 : V1.3a
*	说    明 : 提供一些常用的sting、mem操作函数以及Modbus CRC16函数
*   修改记录 :
*               V1.0  2013-12-05  首版
*               V1.1  2014-06-20  增加大小端整数转换函数
*				V1.2  2015-04-06  增加 BEBufToUint32()和 LEBufToUint32()
*				V1.3  2015-10-09  增加 BcdToChar(), HexToAscll()和 AsciiToUint32()
*				V1.3a 2015-10-09  解决 HexToAscll() 函数末尾补0的BUG
*
*	Copyright (C),
*
*********************************************************************************************************
*/

#include "bsp_user_lib.h"

/* CRC 高位字节值表 */
static const uint8_t s_CRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ;

/* CRC 低位字节值表 */
const uint8_t s_CRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

uint16   mbCRC16(uint8 *updata,uint32 len)  
{  
  uint8 uchCRCHi=0xff;  
  uint8 uchCRCLo=0xff;  
  uint8  uindex;  
  while(len--)  
  {  
  uindex=uchCRCHi^*updata++;  
  uchCRCHi=uchCRCLo^s_CRCHi[uindex];  
  uchCRCLo=s_CRCLo[uindex];  
  }  
  return (uchCRCHi<<8|uchCRCLo);  
}  


/*
*********************************************************************************************************
*	函 数 名: str_len
*	功能说明: 计算字符串长度
*	形    参: _str : 缓冲区，0是结束符
*	返 回 值: 无
*********************************************************************************************************
*/
int str_len(char *_str)
{
	int len = 0;

	while (*_str++) len++;
	return len;
}

/*
*********************************************************************************************************
*	函 数 名: str_cpy
*	功能说明: 复制字符串
*	形    参: _tar : 目标缓冲区
*			  _src : 源缓冲区
*	返 回 值: 无
*********************************************************************************************************
*/
void str_cpy(char *_tar, char *_src)
{
	do
	{
		*_tar++ = *_src;
	}
	while (*_src++);
}

/*
*********************************************************************************************************
*	函 数 名: str_cmp
*	功能说明: 字符串比较
*	形    参: s1 : 字符串1
*			  s2 : 字符串2
*	返 回 值: 0 表示相等 非0表示不等
*********************************************************************************************************
*/
int str_cmp(char * s1, char * s2)
{
	while ((*s1!=0) && (*s2!=0) && (*s1==*s2))
	{
		s1++;
		s2++;
	}
	return *s1 - *s2;
}

/*
*********************************************************************************************************
*	函 数 名: mem_set
*	功能说明: 填充缓冲区每个单元为固定值
*	形    参: _tar : 目标缓冲区
*			  _data : 数据字节
*			  _len : 数据长度
*	返 回 值: 无
*********************************************************************************************************
*/
void mem_set(char *_tar, char _data, int _len)
{
	while (_len--)
	{
		*_tar++ = _data;
	}
}

/*
*********************************************************************************************************
*	函 数 名: mem_cpy
*	功能说明: 复制缓冲区
*	形    参: _tar : 目标缓冲区
*			  _src : 源缓冲区
*			  _len : 字节个数
*	返 回 值: 无
*********************************************************************************************************
*/
void mem_cpy(char *_tar, char *_src, int _len)
{
	while (_len--)
	{
		*_tar++ = *_src++;
	}
}

/*
*********************************************************************************************************
*	函 数 名: int_to_ascii
*	功能说明: 将有符号整数转换为ASCII数组。转换结果是右对齐，左边补空格。
*	形    参: _iNumber : 有符号整数
*			  _pBuf : 目标缓冲区, 存放转换后的结果。以0结束的字符串。
*			  _len : ASCII字符个数, 字符串长度
*	返 回 值: 无
*********************************************************************************************************
*/
void int_to_str(int _iNumber, char *_pBuf, unsigned char _len)
{
	unsigned char i;
	int iTemp;

	if (_iNumber < 0)	/* 负数 */
	{
		iTemp = -_iNumber;	/* 转为正数 */
	}
	else
	{
		iTemp = _iNumber;
	}

	mem_set(_pBuf, ' ',_len);

	/* 将整数转换为ASCII字符串 */
	for (i = 0; i < _len; i++)
	{
		_pBuf[_len - 1 - i] = (iTemp % 10) + '0';
		iTemp = iTemp / 10;
		if (iTemp == 0)
		{
			break;
		}
	}
	_pBuf[_len] = 0;

	if (_iNumber < 0)	/* 负数 */
	{
		for (i = 0; i < _len; i++)
		{
			if ((_pBuf[i] == ' ') && (_pBuf[i + 1] != ' '))
			{
				_pBuf[i] = '-';
				break;
			}
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: str_to_int
*	功能说明: 将10进制形式的ASCII码字符串转换成整数。遇到小数点自动越过。"12.34"转换结果为1234
*	形    参: _pStr :待转换的ASCII码串，结束符为非'0'-'9'的字符,比如0,逗号,#等。
*	返 回 值: 有符号整数
*********************************************************************************************************
*/
int str_to_int(char *_pStr)
{
	unsigned char flag;
	char *p;
	int ulInt;
	unsigned char  i;
	unsigned char  ucTemp;

	p = _pStr;
	if (*p == '-')
	{
		flag = 1;	/* 负数 */
		p++;
	}
	else
	{
		flag = 0;
	}

	ulInt = 0;
	for (i = 0; i < 15; i++)
	{
		ucTemp = *p;
		if (ucTemp == '.')	/* 遇到小数点，自动跳过1个字节 */
		{
			p++;
			ucTemp = *p;
		}
		if ((ucTemp >= '0') && (ucTemp <= '9'))
		{
			ulInt = ulInt * 10 + (ucTemp - '0');
			p++;
		}
		else
		{
			break;
		}
	}

	if (flag == 1)
	{
		return -ulInt;
	}
	return ulInt;
}

/*
*********************************************************************************************************
*	函 数 名: BEBufToUint16
*	功能说明: 将2字节数组转换为16位无符号整数。数组是大端Big Endian次序，高字节在前。
*	形    参: _pBuf : 数组
*	返 回 值: 16位无符号整数
*********************************************************************************************************
*/
uint16_t BEBufToUint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[0] << 8) | _pBuf[1]);
}

/*
*********************************************************************************************************
*	函 数 名: LEBufToUint16
*	功能说明: 将2字节数组)转换为16位整数。数组是小端Little Endian次序，低字节在前。
*	形    参: _pBuf : 数组
*	返 回 值: 16位无符号整数
*********************************************************************************************************
*/
uint16_t LEBufToUint16(uint8_t *_pBuf)
{
    return (((uint16_t)_pBuf[1] << 8) | _pBuf[0]);
}

/*
*********************************************************************************************************
*	函 数 名: BEBufToUint32
*	功能说明: 将4字节数组转换为16位整数。数组是大端Big Endian次序，高字节在前。
*	形    参: _pBuf : 数组
*	返 回 值: 32位无符号整数
*********************************************************************************************************
*/
uint32_t BEBufToUint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[0] << 24) | ((uint32_t)_pBuf[1] << 16) | ((uint32_t)_pBuf[2] << 8) | _pBuf[3]);
}

/*
*********************************************************************************************************
*	函 数 名: LEBufToUint32
*	功能说明: 将4字节数组转换为16位整数。数组是小端Little Endian次序，低字节在前。
*	形    参: _pBuf : 数组
*	返 回 值: 32位无符号整数
*********************************************************************************************************
*/
uint32_t LEBufToUint32(uint8_t *_pBuf)
{
    return (((uint32_t)_pBuf[3] << 24) | ((uint32_t)_pBuf[2] << 16) | ((uint32_t)_pBuf[1] << 8) | _pBuf[0]);
}

/*
*********************************************************************************************************
*	函 数 名: CRC16_Modbus
*	功能说明: 计算CRC。 用于Modbus协议。
*	形    参: _pBuf : 参与校验的数据
*			  _usLen : 数据长度
*	返 回 值: 16位整数值。 对于Modbus ，此结果高字节先传送，低字节后传送。
*
*   所有可能的CRC值都被预装在两个数组当中，当计算报文内容时可以简单的索引即可；
*   一个数组包含有16位CRC域的所有256个可能的高位字节，另一个数组含有低位字节的值；
*   这种索引访问CRC的方式提供了比对报文缓冲区的每一个新字符都计算新的CRC更快的方法；
*
*	注意：此程序内部执行高/低CRC字节的交换。此函数返回的是已经经过交换的CRC值；也就是说，
*		该函数的返回值可以直接放置于报文用于发送；
*********************************************************************************************************
*/
uint16_t CRC16_Modbus(uint8_t *_pBuf, uint16_t _usLen)
{
	uint8_t ucCRCHi = 0xFF; /* 高CRC字节初始化 */
	uint8_t ucCRCLo = 0xFF; /* 低CRC 字节初始化 */
	uint16_t usIndex;  /* CRC循环中的索引 */

    while (_usLen--)
    {
		usIndex = ucCRCHi ^ *_pBuf++; /* 计算CRC */
		ucCRCHi = ucCRCLo ^ s_CRCHi[usIndex];
		ucCRCLo = s_CRCLo[usIndex];
    }
    return ((uint16_t)ucCRCHi << 8 | ucCRCLo);
}


/*
*********************************************************************************************************
*	函 数 名: CaculTwoPoint
*	功能说明: 根据2点直线方程，计算Y值
*	形    参:  2个点的坐标和x输入量
*	返 回 值: x对应的y值
*********************************************************************************************************
*/
int32_t  CaculTwoPoint(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x)
{
	return y1 + ((int64_t)(y2 - y1) * (x - x1)) / (x2 - x1);
}

/*
*********************************************************************************************************
*	函 数 名: BcdToChar
*	功能说明: 将BCD码转为ASCII字符。 比如 0x0A ==> 'A'
*	形    参: _bcd   ：输入的二进制数。必须小于16
*	返 回 值: 转换结果
*********************************************************************************************************
*/
char BcdToChar(uint8_t _bcd)
{
	if (_bcd < 10)
	{
		return _bcd + '0';
	}
	else if (_bcd < 16)
	{
		return _bcd + 'A';
	}
	else
	{
		return 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: HexToAscll
*	功能说明: 将二进制数组转换为16进制格式的ASCII字符串。每个2个ASCII字符后保留1个空格。
*			  0x12 0x34 转化为 0x31 0x32 0x20 0x33 0x34 0x00  即 "1234"
*	形    参: 	_pHex   ：输入的数据，二进制数组
*				_pAscii ：存放转换结果, ASCII字符串，0结束。1个二进制对应2个ASCII字符.
*	返 回 值: 转换得到的整数
*********************************************************************************************************
*/
void HexToAscll(uint8_t * _pHex, char *_pAscii, uint16_t _BinBytes)
{
	uint16_t i;
	
	if (_BinBytes == 0)
	{
		_pAscii[0] = 0;
	}
	else
	{
		for (i = 0; i < _BinBytes; i++)
		{
			_pAscii[3 * i] = BcdToChar(_pHex[i] >> 4);
			_pAscii[3 * i + 1] = BcdToChar(_pHex[i] & 0x0F);
			_pAscii[3 * i + 2] = ' ';
		}
		_pAscii[3 * (i - 1) + 2] = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: AsciiToUint32
*	功能说明: 变长的 ASCII 字符转换为32位整数  ASCII 字符以空格或者0结束 。 支持16进制和10进制输入
*	形    参: *pAscii ：要转换的ASCII码
*	返 回 值: 转换得到的整数
*********************************************************************************************************
*/
uint32_t AsciiToUint32(char *pAscii)
{
	char i;
	char bTemp;
	char bIsHex;
	char bLen;
	char bZeroLen;
	uint32_t lResult;
	uint32_t lBitValue;

	/* 判断是否是16进制数 */
	bIsHex = 0;
	if ((pAscii[0] == '0') && ((pAscii[1] == 'x') || (pAscii[1] == 'X')))
	{
		bIsHex=1;
	}

	lResult=0;
	// 最大数值为 4294967295, 10位+2字符"0x" //
	if (bIsHex == 0)
	{ // 十进制 //
		// 求长度 //
		lBitValue=1;

		/* 前导去0 */
		for (i = 0; i < 8; i++)
		{
			bTemp = pAscii[i];
			if (bTemp != '0')
				break;
		}
		bZeroLen = i;

		for (i = 0; i < 10; i++)
		{
			if ((pAscii[i] < '0') || (pAscii[i] > '9'))
				break;
			lBitValue = lBitValue * 10;
		}
		bLen = i;
		lBitValue = lBitValue / 10;
		if (lBitValue == 0)
			lBitValue=1;
		for (i = bZeroLen; i < bLen; i++)
		{
			lResult += (pAscii[i] - '0') * lBitValue;
			lBitValue /= 10;
		}
	}
	else
	{	/* 16进制 */
		/* 求长度 */
		lBitValue=1;

		/* 前导去0 */
		for (i = 0; i < 8; i++)
		{
			bTemp = pAscii[i + 2];
			if(bTemp!='0')
				break;
		}
		bZeroLen = i;
		for (; i < 8; i++)
		{
			bTemp=pAscii[i+2];
			if (((bTemp >= 'A') && (bTemp <= 'F')) ||
				((bTemp>='a')&&(bTemp<='f')) ||
				((bTemp>='0')&&(bTemp<='9')) )
			{
				lBitValue=lBitValue * 16;
			}
			else
			{
				break;
			}
		}
		lBitValue = lBitValue / 16;
		if (lBitValue == 0)
			lBitValue = 1;
		bLen = i;
		for (i = bZeroLen; i < bLen; i++)
		{
			bTemp = pAscii[i + 2];
			if ((bTemp >= 'A') && (bTemp <= 'F'))
			{
				bTemp -= 0x37;
			}
			else if ((bTemp >= 'a') && (bTemp <= 'f'))
			{
				bTemp -= 0x57;
			}
			else if ((bTemp >= '0') && (bTemp <= '9'))
			{
				bTemp -= '0';
			}
			lResult += bTemp*lBitValue;
			lBitValue /= 16;
		}
	}
	return lResult;
}




float BEBufToFloat(uint8_t *p)
{
  float a;
  uint8_t *r;
  
  r=(uint8_t * )&a;
  *r=p[1]; r++;
  *r=p[0]; r++;
  *r=p[3]; r++;
  *r=p[2];
  
//  *r=p[1]; r++;
//  *r=p[0]; r++;
//  *r=p[3]; r++;
//  *r=p[2];
  return (a);
}
//float LEBufToFloat(uint8_t *p)
//{
//  float a;
//  uint8_t *r;
//  
//  r=(uint8_t * )&a;
//  *r=p[1]; r++;
//  *r=p[0]; r++;
//  *r=p[3]; r++;
//  *r=p[2];
//  return (a);
//}



uint32 bufTOuint32(uint8 *pbuf)
{
  uint8 a=0;
  for(a=0;a<5;a++)
  {
    if(pbuf[a]=='\0')
      break ;
  }
  //printf("a=%d\r\n",a);
  switch(a)
  {
    case 1:
      return (pbuf[0]-'0');
    case 2:
      return (pbuf[0]-'0')*10 + (pbuf[1]-'0');
    case 3:
      return (pbuf[0]-'0')*100 + (pbuf[1]-'0')*100+(pbuf[1]-'0');
    case 4:   
      return (pbuf[0]-'0')*1000 + (pbuf[1]-'0')*100 + (pbuf[2]-'0')*10 + (pbuf[3]-'0') ;      
  }
  return 0;
  //return (pbuf[0]-'0')*10 + (pbuf[1]-'0');
  //return (pbuf[0]-'0')*1000 + (pbuf[1]-'0')*100 + (pbuf[2]-'0')*10 + (pbuf[3]-'0') ;
}



///*
//pszInput:输入待分割字符串
//pszDelimiters:分割标识符
//uiAry_num:分割的份数
//uiAry_size:每份的size尺寸大小
//pszAry_out:分割的子串的输出参数
//返回份数
//*/
int separate_string_to_array( char *pszInput,
                                char *pszDelimiters ,
                                  unsigned int Ary_num,
                                    unsigned int Ary_size,
                                      char *pszAry_out)
{
    //char *pszData = strdup(pszInput);
    char pszData[2048]={0};
    strcpy(pszData, pszInput);
    char *pszToken = NULL, *pszToken_saveptr;
    unsigned int Ary_cnt = 0;

    pszToken = strtok_r(pszData, pszDelimiters, &pszToken_saveptr);
    while( pszToken!=NULL)
    {
        //printf("pszToken=%s\n", pszToken);
        memcpy( pszAry_out + Ary_cnt*Ary_size, pszToken, Ary_size);
        if( ++Ary_cnt >= Ary_num)
            break;
        pszToken = strtok_r( NULL, pszDelimiters, &pszToken_saveptr);
    }
    //free(pszData);

    return Ary_cnt;
}

//pszInput:输入待分割字符串
//pszDelimiters:分割标识符
//uiAry_num:分割的份数
//uiAry_size:每份的size尺寸大小
//pszAry_out:分割的子串的输出参数,该数组最好是二维数组
uint16 str_str(char * pszInput,char * pszDelimiters,
                                    unsigned int Ary_num,
                                    unsigned int Ary_size,
                                      char *pszAry_out)
{
 uint16 i; //循环次数  就是分割份数
 char *subString;
 char * subStringNext;

  subString=pszInput;
  for (i = 0 ; i < Ary_num; i++)
  {
      if ((subStringNext = strstr(subString, pszDelimiters)) != NULL)
      {
        memcpy(pszAry_out+Ary_size*(i),  subString, subStringNext - subString);
        //printf("%d....%s\r\n",i,pszAry_out+Ary_size*(i));
      }
      subString=subStringNext+1;
  }      
  return i;
}

//函数名 :str_as_str
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
                                 uint8    *pszAry_out)
{
  int i=0;  //主循环次数
  int j=0;  //分割后二维数组的第几位,,相当于 xxx[][j]
  int a=0;  //确定发现'\0' 连续发生时 之作一次处理
  int b=0;  //跳转二维数组的维度  相当于 xxx[b][]
  if(*(pszInput+j) ==*pszDelimiters)
  {
    *pszAry_out=*pszDelimiters;
    return 0;
  }
  for(i=0;i<long_num ;i++)
  {
    if(*(pszInput+i) !=*pszDelimiters)
    {
      *(pszAry_out+Ary_size*b+j) = *(pszInput+i);  a=0;j++;
    }
    else
    {
      if(a==0)
        { 
          *(pszAry_out+Ary_size*b+j)=','; //每一份以","作结尾
          j=0;b++; 
        }
      a=1;
    }
    //printf("i=%d,,b=%d,,p=%d,,%s,,\r\n",i,b,(Ary_size*b+j-1),(pszAry_out+Ary_size*b+j-1));
  }
  if(b==1)
  {
    return b;
  }
  return b;
}


//16点法求平均值
float average(float val)
{
  static int i = 0;
  static float arr[16] = {0};
  static float sigma = 0.0;
  float ave = 0.0;
 
  sigma += val;
  sigma -= arr[i%16];
  arr[i%16] = val;
  i++;
  ave = i >= 16 ? (sigma / 16) : (sigma / i);
  return ave;
}




/***************************** *********************************/

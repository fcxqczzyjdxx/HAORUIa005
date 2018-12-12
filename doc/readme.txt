/*******************************************************************************
文 件 名: readme.txt
功    能: 描述文档
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


/*用于文件开头,文件的文字描述*/
/*******************************************************************************
文 件 名: 
功    能: 
创建日期: 
作    者: 
详    细: 
********************************************************************************
注意事项
*******************************************************************************/
/*******************************************************************************
文 件 名: 
功    能: 
创建日期: 
作    者: 
详    细: 
修改日期: 
修改原因: 
作    者: 
详    细: 
********************************************************************************
注意事项
*******************************************************************************/

/*用于函数的开头,函数的文字描述*/
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
/*******************************************************************************
函数名字: 
创建时间: 
作    者: 
函数功能:
依    赖: 
参    数: 
返    回: 
详细说明: 
修改时间: 
作    者: 
修改原因: 
依    赖: 
参    数: 
返    回: 
详细说明: 
*******************************************************************************/

/*文件结构*/
doc  文档
device_driver 外挂设备驱动
hardware  各种外设程序
libraries  官方库和一些自己写的通用函数
project   工程文件
system    延时和启动,,用的是正点原子的
user      main文件 中断文件等等

/*常用的一些函数*/
常用c标准函数
    memcpy(buf2, buf1, n);  拷贝 buf1->buf2
    memset(buf1,0,n);       填充 buf1填充0
    strlen(str);  字符串长度,遇见\0 结束,返回长度不包括\0
    extern char *strcat(char *dest, const char *src);  两个字符串链接  检查到\0

/*变量名字的规范*/
数据类型  在stm32f10x,h 数据类型中定义   
int liv_Width
其变量名解释如下：
l 局部变量（Local） （其它： g 全局变量（Global） ...）
i 数据类型（Interger）
v 变量（Variable） （其它： c 常量（Const） ...）
局部的
li8v_
li16v_
li32v_
全局的
gi8v_
gi16v_
gi32v_

global variable  全局变量
local variable   局部变量


/***********华丽的分割线**************/



/***********华丽的分割线**************/
/***********华丽的分割线**************/
/***********华丽的分割线**************/


/***********华丽的分割线**************/



/***********华丽的分割线**************/



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








........................END..........................
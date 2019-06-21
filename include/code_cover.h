#ifndef __CODE_COVER_H__
#define __CODE_COVER_H__
/*
 *@function name: 
    code_convert
 *@Author: yiweijiao
 *@Date: 2019-06-21 16:13:38
 *@describtion: 
    汉字字符编码转码
 *@parameter: 
    from_charset[in]:源数据编码
    to_charset[in]:目的数据编码
    inlen[in]:源数据长度
    outbuf[in]:目的数据大小

 *@return: 
    -1 转码器打开失败
    -2 转码失败
    0  成功
*/
int code_convert(char *from_charset, char *to_charset, char *inbuf, int inlen, char *outbuf, int outlen);
#endif
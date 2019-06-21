#include "iconv.h"
#include "code_cover.h"
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
int code_convert(char *from_charset, char *to_charset, char *inbuf, int inlen, char *outbuf, int outlen)
{
	iconv_t iconv_handle;
	char **pin = &inbuf;
	char **pout = &outbuf;
	iconv_handle = iconv_open(to_charset, from_charset);
	if (iconv_handle == 0) 
		return -1;
	if (iconv(iconv_handle, pin, (size_t *)&inlen, pout,(size_t *) &outlen) == -1) 
	{
		iconv_close(iconv_handle);
		return -2;
	}
	iconv_close(iconv_handle);
	return 0;
}
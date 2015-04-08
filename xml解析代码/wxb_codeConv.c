/********************************************************************
	created:	2007/11/15
	created:	15:11:2007   10:30
	filename:	wxb_codeConv.c
	author:		Wang xuebin 
	depend:		iconv.lib
	build:		����Ҫbuild��������������Դ������
	
	purpose:	�ṩ��UTF-8��GB2312������ת�����Լ������ת��
*********************************************************************/

#include "iconv.h"
#include <string.h>

//����ת��:��һ�ֱ���תΪ��һ�ֱ���   
int code_convert(char* from_charset, char* to_charset, char* inbuf,
				 int inlen, char* outbuf, int outlen)
{
	iconv_t cd;
	char** pin = &inbuf;   
	char** pout = &outbuf;

	cd = iconv_open(to_charset,from_charset);   
	if(cd == 0)
		return -1;
	memset(outbuf,0,outlen);   
	if(iconv(cd,(const char**)pin,(unsigned int *)&inlen,pout,(unsigned int*)&outlen)
		== -1)
		return -1;   
	iconv_close(cd);
	return 0;   
}

//UNICODE��תΪGB2312��   
//�ɹ��򷵻�һ����̬�����char*��������Ҫ��ʹ����Ϻ��ֶ�free��ʧ�ܷ���NULL
char* u2g(char *inbuf)   
{
	int nOutLen = 2 * strlen(inbuf) - 1;
	char* szOut = (char*)malloc(nOutLen);
	
	if (-1 == code_convert("utf-8","gb2312",inbuf,strlen(inbuf),szOut,nOutLen))
	{
		free(szOut);
		szOut = NULL;
	}
	return szOut;
}   

//GB2312��תΪUNICODE��   
//�ɹ��򷵻�һ����̬�����char*��������Ҫ��ʹ����Ϻ��ֶ�free��ʧ�ܷ���NULL
char* g2u(char *inbuf)   
{
	int nOutLen = 2 * strlen(inbuf) - 1;
	char* szOut = (char*)malloc(nOutLen);
	
	if (-1 == code_convert("gb2312","utf-8",inbuf,strlen(inbuf),szOut,nOutLen))
	{
		free(szOut);
		szOut = NULL;
	}
	return szOut;
}   

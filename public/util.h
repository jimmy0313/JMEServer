#ifndef util_h__
#define util_h__

#include <string>
#include <vector>

#ifdef WIN32
#include <Windows.h> // for	MultiByteToWideChar functions
#endif

#ifdef LINUX
#include <iconv.h>
#endif // LINUX

void split(const std::string& src_string, std::vector<std::string>& out_vec, const std::string& sign);


#ifdef LINUX		
int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen);
#endif

void UnicodeToUTF_8(char* pOut,wchar_t* pText);	
void UTF_8ToUnicode(wchar_t* pOut,char *pText);
void Gb2312ToUnicode(wchar_t* pOut,char *gbBuffer);
void UnicodeToGB2312(char* pOut,wchar_t uData);


void GB2312ToUTF_8(std::string& pOut,char *pText, int pLen);
void UTF_8ToGB2312(std::string &pOut, char *pText, int pLen);  

int stringLength(const char* str, int len); 
int stringLength(const std::string& src_string);

int stringLength(const char* str, int len);
#endif // util_h__

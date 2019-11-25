#include "stdafx.h"
#include "MTHelper.h"



std::string WideToMultiByte(const wchar_t* s){
	size_t num = WideCharToMultiByte(CP_ACP, 0, s, -1, NULL, 0, NULL, 0);

	char* pchar = new char[num];
	if (pchar == NULL)	return "";
	memset(pchar, 0, sizeof(*pchar)*num);
	int nLen = WideCharToMultiByte(CP_ACP, 0, s, -1, pchar, (int)num * sizeof(char), NULL, 0);
	if (nLen < num){
		delete[] pchar;
		pchar = NULL;
		return "";
	}
	std::string str = pchar;
	delete[] pchar;
	return str;
}


std::wstring MultiToWideByte(const char* s){
	size_t num = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);

	wchar_t* pchar = new wchar_t[num];
	if (pchar == NULL)	return NULL;
	memset(pchar, 0, sizeof(*pchar)*num);

	if (MultiByteToWideChar(CP_ACP, 0, s, -1, pchar, (int)num * sizeof(wchar_t)) < num){
		delete[] pchar;
		pchar = NULL;
		return NULL;
	}
	std::wstring str = pchar;
	delete[] pchar;
	return str;
}


std::string GB2Utf_8(const char* gb2312)
{ 
	int buffLen = 0;
	WCHAR wbuff[1024];
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wbuff, 1024);
	buffLen = WideCharToMultiByte(CP_UTF8, 0, wbuff, -1, NULL, 0, 0, 0);
	const char* utf8buf = new char[buffLen + 1];
	WideCharToMultiByte(CP_UTF8, 0, wbuff, -1, (LPSTR)utf8buf, buffLen, 0, 0);

	std::string tmp(utf8buf);
	delete[] utf8buf;

	return tmp;
}

std::string Utf_82GB(const char* utf)
{
	int buffLen = 0;
	WCHAR wbuff[1024];
	MultiByteToWideChar(CP_UTF8, 0, utf, -1, wbuff, 1024);
	buffLen = WideCharToMultiByte(CP_ACP, 0, wbuff, -1, NULL, 0, 0, 0);
	const char* gb2312 = new char[buffLen + 1];
	WideCharToMultiByte(CP_ACP, 0, wbuff, -1, (LPSTR)gb2312, buffLen, 0, 0);

	std::string tmp(gb2312);
	delete[] gb2312;
	return tmp;
}

std::string GetLastErrorToString(DWORD errorCode)
{
	//因为FORMAT_MESSAGE_ALLOCATE_BUFFER标志，这个函数帮你分配内存，所以需要LocalFree来释放

	TCHAR *text;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&text, 0, NULL);
	std::string str = WideToMultiByte(text);
//	CStringA result(text);    //结果
	LocalFree(text);
	return str;
}

int IsNum(wchar_t* pStr, int nLen){
	while (*pStr){
		if (*pStr >= '0' && *pStr <= '9' || *pStr == '.'){
			++pStr;
		}else{
			return -1;
		}
	}
	return 0;
}

int CharPtrToint(const unsigned char* pNum)
{
	if (sizeof(pNum) < 4)
		return -1;
	return (pNum[0] << 24) + (pNum[1] << 16) + (pNum[2] << 8) + pNum[3];
}


double CalPrecision(std::string sRes, std::string sTemplate){
	int nCount = 0;
	double p = 0;
	for (size_t i = 0; i < sTemplate.length(); i++){
		if (i > sRes.length())	break;
		if (sRes[i] == sTemplate[i]){
			nCount++;
		}
	}
	p = nCount*1.0 / sTemplate.length() * 100;
	return p;
}
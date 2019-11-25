#ifndef MT_HELPER_H_
#define MT_HELPER_H_

#include <stdlib.h>
#include <windows.h>
#include <string>
#include <shlwapi.h>

#pragma comment(lib,"shlwapi.lib")

#define GetPathExe(path,size) memset(path,0,size); \
	::GetModuleFileName(NULL, path, MAX_PATH); \
	PathRemoveFileSpec(path);

#define GetPathExeA(path,size) memset(path,0,size); \
	::GetModuleFileNameA(NULL, path, MAX_PATH); \
	PathRemoveFileSpecA(path);

std::string WideToMultiByte(const wchar_t* s);
std::wstring MultiToWideByte(const char* s);
std::string GB2Utf_8(const char* gb2312);
std::string Utf_82GB(const char* utf);

double CalPrecision(std::string sRes, std::string sTemplate);
std::string GetLastErrorToString(DWORD errorCode);

int IsNum(wchar_t* s, int nLen);

int CharPtrToint(const unsigned char* pNum);

#endif
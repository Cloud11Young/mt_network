#include "stdafx.h"
#include "SysHelper.h"
#include "GeneralHelper.h"

void SysGetSystemInfo(LPSYSTEM_INFO pInfo)
{
	ASSERT(pInfo != nullptr);
	::GetNativeSystemInfo(pInfo);
}

DWORD SysGetNumberOfProcessors()
{
	SYSTEM_INFO si;
	SysGetSystemInfo(&si);
	
	return si.dwNumberOfProcessors;
}

DWORD SysGetPageSize()
{
	SYSTEM_INFO si;
	SysGetSystemInfo(&si);

	return si.dwPageSize;
}

#if _MSC_VER < 1800

BOOL SysGetOSVersionInfo(LPOSVERSIONINFO pInfo, BOOL bInfoEx)
{
	ASSERT(pInfo != nullptr);

	pInfo->dwOSVersionInfoSize = bInfoEx ? sizeof(LPOSVERSIONINFOEX) : sizeof(LPOSVERSIONINFO);
	return ::GetVersionEx(pInfo);
}

DWORD SysGetOSVersion()
{
	OSVERSIONINFO vi;
	DWORD dwOSVersion = 0;

	if(SysGetOSVersionInfo(&vi))
		dwOSVersion = (vi.dwMajorVersion << 16) + vi.dwMinorVersion;

	return dwOSVersion;
}

#endif

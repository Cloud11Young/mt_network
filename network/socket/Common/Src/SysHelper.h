#pragma once

#include <windows.h>

// 获取系统信息
void SysGetSystemInfo(LPSYSTEM_INFO pInfo);
// 获取 CPU 核数
DWORD SysGetNumberOfProcessors();
// 获取页面大小
DWORD SysGetPageSize();

#if _MSC_VER < 1800

// 获取操作系统版本
BOOL SysGetOSVersionInfo(LPOSVERSIONINFO pInfo, BOOL bInfoEx = FALSE);
// 获取操作系统版本（高位双字节：主版本号；低位双字节：副版本号）
DWORD SysGetOSVersion();

#endif

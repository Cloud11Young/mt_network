#pragma once

#include <windows.h>

// ��ȡϵͳ��Ϣ
void SysGetSystemInfo(LPSYSTEM_INFO pInfo);
// ��ȡ CPU ����
DWORD SysGetNumberOfProcessors();
// ��ȡҳ���С
DWORD SysGetPageSize();

#if _MSC_VER < 1800

// ��ȡ����ϵͳ�汾
BOOL SysGetOSVersionInfo(LPOSVERSIONINFO pInfo, BOOL bInfoEx = FALSE);
// ��ȡ����ϵͳ�汾����λ˫�ֽڣ����汾�ţ���λ˫�ֽڣ����汾�ţ�
DWORD SysGetOSVersion();

#endif

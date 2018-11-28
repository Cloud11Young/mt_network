/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 2.3.21
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 75375912, 44636872
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#pragma once

#include <windows.h>

typedef void* HANDLE;

/* timeGetTime() ��װ���� */
unsigned long TimeGetTime();

/**********************************
����: ��ȡ��ǰʱ����ԭʼʱ���ʱ���
����: 
		dwOriginal	: ԭʼʱ�䣨���룩��ͨ���� timeGetTime() �� GetTickCount() ��ȡ

����ֵ:	�뵱ǰ timeGetTime() ֮���ʱ���
**********************************/
unsigned long GetTimeGap32(unsigned long dwOriginal);

#if _WIN32_WINNT >= _WIN32_WINNT_WS08
/**********************************
����: ��ȡ��ǰʱ����ԭʼʱ���ʱ���
����: 
		ullOriginal	: ԭʼʱ�䣨���룩��ͨ���� GetTickCount64() ��ȡ

����ֵ:	�뵱ǰ GetTickCount64() ֮���ʱ���
**********************************/
unsigned long long GetTimeGap64(unsigned long long ullOriginal);
#endif

/**********************************
����: ����Windows��Ϣ
����: 
			bDispatchQuitMsg	: �Ƿ�ת�� WM_QUIT ��Ϣ
									TRUE : ת����Ĭ�ϣ�
									FALSE: ��ת���������� FALSE

����ֵ:		TRUE  : ������Ϣ
			FALSE : bDispatchQuitMsg ����Ϊ FALSE ���յ� WM_QUIT ��Ϣ		
**********************************/
int PeekMessageLoop(int bDispatchQuitMsg = 1);

/**********************************
����: �ȴ�ָ��ʱ��, ͬʱ����Windows��Ϣ
����: (�ο�: MsgWaitForMultipleObjectsEx() )
		dwHandles		: ����Ԫ�ظ���
		szHandles		: ����������
		dwMilliseconds	: �ȴ�ʱ�� (����)
		dwWakeMask		: ��Ϣ���˱�ʶ
		dwFlags			: �ȴ�����

����ֵ: (0 ~ dwHandles - 1): �ȴ��ɹ�
		WAIT_TIMEOUT		: ��ʱ
		WAIT_FAILED			: ִ��ʧ��
**********************************/
unsigned long WaitForMultipleObjectsWithMessageLoop(unsigned long dwHandles, HANDLE szHandles[], unsigned long dwMilliseconds = INFINITE, unsigned long dwWakeMask = QS_ALLINPUT, unsigned long dwFlags = MWMO_INPUTAVAILABLE);

/**********************************
����: �ȴ�ָ��ʱ��, ͬʱ����Windows��Ϣ
����: (�ο�: MsgWaitForMultipleObjectsEx() )
		hHandle			: ������
		dwMilliseconds	: �ȴ�ʱ�� (����)
		dwWakeMask		: ��Ϣ���˱�ʶ
		dwFlags			: �ȴ�����

����ֵ: TRUE: �ȴ��ɹ���FALSE: ��ʱ		
**********************************/
int MsgWaitForSingleObject(HANDLE hHandle, unsigned long dwMilliseconds = INFINITE, unsigned long dwWakeMask = QS_ALLINPUT, unsigned long dwFlags = MWMO_INPUTAVAILABLE);

/**********************************
����: �ȴ�ָ��ʱ��, ͬʱ����Windows��Ϣ
����: (�ο�: MsgWaitForMultipleObjectsEx() )
		dwMilliseconds	: �ȴ�ʱ�� (����)
		dwWakeMask		: ��Ϣ���˱�ʶ
		dwFlags			: �ȴ�����

����ֵ: MsgWaitForMultipleObjectsEx() �����Ĳ������		
**********************************/
void WaitWithMessageLoop(unsigned long dwMilliseconds, unsigned long dwWakeMask = QS_ALLINPUT, unsigned long dwFlags = MWMO_INPUTAVAILABLE);

/**********************************
����: �ȴ���Sleep()�����ȴ�ĳ������С��ָ��ֵ
����: 
		plWorkingItemCount		: ���ӱ���
		lMaxWorkingItemCount	: ָ��ֵ
		dwCheckInterval			: ����� (����)

����ֵ: 		
**********************************/
void WaitForWorkingQueue(long* plWorkingItemCount, long lMaxWorkingItemCount, unsigned long dwCheckInterval);
/**********************************
����: �ȴ���Sleep()�����ȴ�ĳ��������С�� 0
����: 
		plWorkingItemCount		: ���ӱ���
		dwCheckInterval			: ����� (����)

����ֵ: 		
**********************************/
void WaitForComplete(long* plWorkingItemCount, unsigned long dwCheckInterval);

/**********************************
����: �ȴ���WaitWithMessageLoop()�����ȴ�ĳ������С��ָ��ֵ
����: 
		plWorkingItemCount		: ���ӱ���
		lMaxWorkingItemCount	: ָ��ֵ
		dwCheckInterval			: ����� (����)

����ֵ: 		
**********************************/
void MsgWaitForWorkingQueue(long* plWorkingItemCount, long lMaxWorkingItemCount, unsigned long dwCheckInterval = 10);
/**********************************
����: �ȴ���WaitWithMessageLoop()�����ȴ�ĳ��������С�� 0
����: 
		plWorkingItemCount		: ���ӱ���
		dwCheckInterval			: ����� (����)

����ֵ: 		
**********************************/
void MsgWaitForComplete(long* plWorkingItemCount, unsigned long dwCheckInterval = 10);

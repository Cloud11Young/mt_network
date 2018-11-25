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

//#include <winbase.h>

typedef void* HANDLE;

/* timeGetTime() 包装方法 */
unsigned long TimeGetTime();

/**********************************
描述: 获取当前时间与原始时间的时间差
参数: 
		dwOriginal	: 原始时间（毫秒），通常用 timeGetTime() 或 GetTickCount() 获取

返回值:	与当前 timeGetTime() 之间的时间差
**********************************/
unsigned long GetTimeGap32(unsigned long dwOriginal);

#if _WIN32_WINNT >= _WIN32_WINNT_WS08
/**********************************
描述: 获取当前时间与原始时间的时间差
参数: 
		ullOriginal	: 原始时间（毫秒），通常用 GetTickCount64() 获取

返回值:	与当前 GetTickCount64() 之间的时间差
**********************************/
unsigned long long GetTimeGap64(unsigned long long ullOriginal);
#endif

/**********************************
描述: 处理Windows消息
参数: 
			bDispatchQuitMsg	: 是否转发 WM_QUIT 消息
									TRUE : 转发（默认）
									FALSE: 不转发，并返回 FALSE

返回值:		TRUE  : 收完消息
			FALSE : bDispatchQuitMsg 参数为 FALSE 并收到 WM_QUIT 消息		
**********************************/
int PeekMessageLoop(int bDispatchQuitMsg = 1);

/**********************************
描述: 等待指定时间, 同时处理Windows消息
参数: (参考: MsgWaitForMultipleObjectsEx() )
		dwHandles		: 数组元素个数
		szHandles		: 对象句柄数组
		dwMilliseconds	: 等待时间 (毫秒)
		dwWakeMask		: 消息过滤标识
		dwFlags			: 等待类型

返回值: (0 ~ dwHandles - 1): 等待成功
		WAIT_TIMEOUT		: 超时
		WAIT_FAILED			: 执行失败
**********************************/
unsigned long WaitForMultipleObjectsWithMessageLoop(unsigned long dwHandles, HANDLE szHandles[], unsigned long dwMilliseconds = INFINITE, unsigned long dwWakeMask = QS_ALLINPUT, unsigned long dwFlags = MWMO_INPUTAVAILABLE);

/**********************************
描述: 等待指定时间, 同时处理Windows消息
参数: (参考: MsgWaitForMultipleObjectsEx() )
		hHandle			: 对象句柄
		dwMilliseconds	: 等待时间 (毫秒)
		dwWakeMask		: 消息过滤标识
		dwFlags			: 等待类型

返回值: TRUE: 等待成功，FALSE: 超时		
**********************************/
int MsgWaitForSingleObject(HANDLE hHandle, unsigned long dwMilliseconds = INFINITE, unsigned long dwWakeMask = QS_ALLINPUT, unsigned long dwFlags = MWMO_INPUTAVAILABLE);

/**********************************
描述: 等待指定时间, 同时处理Windows消息
参数: (参考: MsgWaitForMultipleObjectsEx() )
		dwMilliseconds	: 等待时间 (毫秒)
		dwWakeMask		: 消息过滤标识
		dwFlags			: 等待类型

返回值: MsgWaitForMultipleObjectsEx() 函数的操作结果		
**********************************/
void WaitWithMessageLoop(unsigned long dwMilliseconds, unsigned long dwWakeMask = QS_ALLINPUT, unsigned long dwFlags = MWMO_INPUTAVAILABLE);

/**********************************
描述: 等待用Sleep()函数等待某个变量小于指定值
参数: 
		plWorkingItemCount		: 监视变量
		lMaxWorkingItemCount	: 指定值
		dwCheckInterval			: 检查间隔 (毫秒)

返回值: 		
**********************************/
void WaitForWorkingQueue(long* plWorkingItemCount, long lMaxWorkingItemCount, unsigned long dwCheckInterval);
/**********************************
描述: 等待用Sleep()函数等待某个变量减小到 0
参数: 
		plWorkingItemCount		: 监视变量
		dwCheckInterval			: 检查间隔 (毫秒)

返回值: 		
**********************************/
void WaitForComplete(long* plWorkingItemCount, unsigned long dwCheckInterval);

/**********************************
描述: 等待用WaitWithMessageLoop()函数等待某个变量小于指定值
参数: 
		plWorkingItemCount		: 监视变量
		lMaxWorkingItemCount	: 指定值
		dwCheckInterval			: 检查间隔 (毫秒)

返回值: 		
**********************************/
void MsgWaitForWorkingQueue(long* plWorkingItemCount, long lMaxWorkingItemCount, unsigned long dwCheckInterval = 10);
/**********************************
描述: 等待用WaitWithMessageLoop()函数等待某个变量减小到 0
参数: 
		plWorkingItemCount		: 监视变量
		dwCheckInterval			: 检查间隔 (毫秒)

返回值: 		
**********************************/
void MsgWaitForComplete(long* plWorkingItemCount, unsigned long dwCheckInterval = 10);

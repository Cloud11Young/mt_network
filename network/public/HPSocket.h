
/******************************************************************************
Module:  HPSocket

Usage:
		方法一：
		--------------------------------------------------------------------------------------
		0. 应用程序包含 HPTypeDef.h / SocketInterface.h / HPSocket.h 头文件
		1. 调用 HP_Create_Xxx() 函数创建 HPSocket 对象
		2. 使用完毕后调用 HP_Destroy_Xxx() 函数销毁 HPSocket 对象

		方法二：
		--------------------------------------------------------------------------------------
		0. 应用程序包含 SocketInterface.h 和 HPSocket.h 头文件
		1. 创建 CXxxPtr 智能指针，通过智能指针使用 HPSocket 对象

Release:
		<-- 动态链接库 -->
		1. x86/HPSocket.dll				- (32位/MBCS/Release)
		2. x86/HPSocket_D.dll			- (32位/MBCS/DeBug)
		3. x86/HPSocket_U.dll			- (32位/UNICODE/Release)
		4. x86/HPSocket_UD.dll			- (32位/UNICODE/DeBug)
		5. x64/HPSocket.dll				- (64位/MBCS/Release)
		6. x64/HPSocket_D.dll			- (64位/MBCS/DeBug)
		7. x64/HPSocket_U.dll			- (64位/UNICODE/Release)
		8. x64/HPSocket_UD.dll			- (64位/UNICODE/DeBug)

		<-- 静态链接库 -->
		!!注意!!：使用 HPSocket 静态库时，需要在工程属性中定义预处理宏 -> HPSOCKET_STATIC_LIB
		1. x86/static/HPSocket.lib		- (32位/MBCS/Release)
		2. x86/static/HPSocket_D.lib	- (32位/MBCS/DeBug)
		3. x86/static/HPSocket_U.lib	- (32位/UNICODE/Release)
		4. x86/static/HPSocket_UD.lib	- (32位/UNICODE/DeBug)
		5. x64/static/HPSocket.lib		- (64位/MBCS/Release)
		6. x64/static/HPSocket_D.lib	- (64位/MBCS/DeBug)
		7. x64/static/HPSocket_U.lib	- (64位/UNICODE/Release)
		8. x64/static/HPSocket_UD.lib	- (64位/UNICODE/DeBug)

******************************************************************************/

#pragma once

/**************************************************/
/*********** imports / exports HPSocket ***********/

#ifdef SOCKET_STATIC_LIB
	#define SOCKET_API EXTERN_C
#else
	#ifdef SOCKET_EXPORTS
		#define SOCKET_API EXTERN_C __declspec(dllexport)
	#else
		#define SOCKET_API EXTERN_C __declspec(dllimport)
	#endif
#endif

#include "SocketInterface.h"

/*****************************************************************************************************************************************************/
/****************************************************************** TCP/UDP Exports ******************************************************************/
/*****************************************************************************************************************************************************/

/**************************************************/
/************** HPSocket 对象智能指针 **************/

template<class T, class _Listener, class _Creator> class CHPSocketPtr
{
public:
	CHPSocketPtr(_Listener* pListener)
	{
		m_pObj = _Creator::Create(pListener);
	}

	CHPSocketPtr() : m_pObj(nullptr)
	{

	}

	~CHPSocketPtr()
	{
		Reset();
	}

public:
	CHPSocketPtr& Reset(T* pObj = nullptr)
	{
		if(pObj != m_pObj)
		{
			if(m_pObj)
				_Creator::Destroy(m_pObj);

			m_pObj = pObj;
		}

		return *this;
	}

	CHPSocketPtr& Attach(T* pObj)
	{
		return Reset(pObj);
	}

	T* Detach()
	{
		T* pObj	= m_pObj;
		m_pObj	= nullptr;

		return pObj;
	}

	int IsValid	()	const	{return m_pObj != nullptr	;}
	T* Get			()	const	{return m_pObj				;}
	T* operator ->	()	const	{return m_pObj				;}
	operator T*		()	const	{return m_pObj				;}

	CHPSocketPtr& operator = (T* pObj)	{return Reset(pObj)	;}

private:
	CHPSocketPtr(const CHPSocketPtr&);
	CHPSocketPtr& operator = (const CHPSocketPtr&);

private:
	T* m_pObj;
};


// 创建 ITcpServer 对象
SOCKET_API ITcpServer* HP_Create_TcpServer(ITcpServerListener* pListener);
SOCKET_API ITcpClient* HP_Create_TcpClient(ITcpClientListener* pListener);
SOCKET_API ITcpPackServer* HP_Create_TcpPackServer(ITcpServerListener* pListener);
SOCKET_API ITcpPackClient* HP_Create_TcpPackClient(ITcpClientListener* pListener);

// 销毁 ITcpServer 对象
SOCKET_API void HP_Destroy_TcpServer(ITcpServer* pServer);
SOCKET_API void HP_Destroy_TcpClient(ITcpClient* pClient);
SOCKET_API void HP_Destroy_TcpPackServer(ITcpPackServer* pServer);
SOCKET_API void HP_Destroy_TcpPackClient(ITcpPackClient* pClient);

// ITcpServer 对象创建器
struct TcpServer_Creator
{
	static ITcpServer* Create(ITcpServerListener* pListener)
	{
		return HP_Create_TcpServer(pListener);
	}

	static void Destroy(ITcpServer* pServer)
	{
		HP_Destroy_TcpServer(pServer);
	}
};

// ITcpClient 对象创建器
struct TcpClient_Creator
{
	static ITcpClient* Create(ITcpClientListener* pListener)
	{
		return HP_Create_TcpClient(pListener);
	}

	static void Destroy(ITcpClient* pClient)
	{
		HP_Destroy_TcpClient(pClient);
	}
};

// ITcpPackServer 对象创建器
struct TcpPackServer_Creator
{
	static ITcpPackServer* Create(ITcpServerListener* pListener)
	{
		return HP_Create_TcpPackServer(pListener);
	}

	static void Destroy(ITcpPackServer* pServer)
	{
		HP_Destroy_TcpPackServer(pServer);
	}
};

// ITcpPackClient 对象创建器
struct TcpPackClient_Creator
{
	static ITcpPackClient* Create(ITcpClientListener* pListener)
	{
		return HP_Create_TcpPackClient(pListener);
	}

	static void Destroy(ITcpPackClient* pClient)
	{
		HP_Destroy_TcpPackClient(pClient);
	}
};

// ITcpServer 对象智能指针
typedef CHPSocketPtr<ITcpServer, ITcpServerListener, TcpServer_Creator>			CTcpServerPtr;
typedef CHPSocketPtr<ITcpClient, ITcpClientListener, TcpClient_Creator>			CTcpClientPtr;
typedef CHPSocketPtr<ITcpPackServer, ITcpServerListener, TcpPackServer_Creator>	CTcpPackServerPtr;
typedef CHPSocketPtr<ITcpPackClient, ITcpClientListener, TcpPackClient_Creator>	CTcpPackClientPtr;


/*****************************************************************************************************************************************************/
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

// 获取 HPSocket 版本号（4 个字节分别为：主版本号，子版本号，修正版本号，构建编号）
SOCKET_API DWORD HP_GetHPSocketVersion();

// 获取错误描述文本
SOCKET_API const char* HP_GetSocketErrorDesc(EnSocketError enCode);
// 调用系统的 GetLastError() 方法获取系统错误代码
SOCKET_API DWORD SYS_GetLastError();
// 调用系统的 WSAGetLastError() 方法获取系统错误代码
SOCKET_API int SYS_WSAGetLastError();
// 调用系统的 setsockopt()
SOCKET_API int SYS_SetSocketOption(SOCKET sock, int level, int name, LPVOID val, int len);
// 调用系统的 getsockopt()
SOCKET_API int SYS_GetSocketOption(SOCKET sock, int level, int name, LPVOID val, int* len);
// 调用系统的 ioctlsocket()
SOCKET_API int SYS_IoctlSocket(SOCKET sock, long cmd, u_long* arg);
// 调用系统的 WSAIoctl()
SOCKET_API int SYS_WSAIoctl(SOCKET sock, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer, LPDWORD lpcbBytesReturned);

// 设置 socket 选项：IPPROTO_TCP -> TCP_NODELAY
SOCKET_API int SYS_SSO_NoDelay(SOCKET sock, int bNoDelay);
// 设置 socket 选项：SOL_SOCKET -> SO_DONTLINGER
SOCKET_API int SYS_SSO_DontLinger(SOCKET sock, int bDont);
// 设置 socket 选项：SOL_SOCKET -> SO_LINGER
SOCKET_API int SYS_SSO_Linger(SOCKET sock, USHORT l_onoff, USHORT l_linger);
// 设置 socket 选项：SOL_SOCKET -> SO_RCVBUF
SOCKET_API int SYS_SSO_RecvBuffSize(SOCKET sock, int size);
// 设置 socket 选项：SOL_SOCKET -> SO_SNDBUF
SOCKET_API int SYS_SSO_SendBuffSize(SOCKET sock, int size);
// 设置 socket 选项：SOL_SOCKET -> SO_REUSEADDR
SOCKET_API int SYS_SSO_ReuseAddress(SOCKET sock, int bReuse);

// 获取 SOCKET 本地地址信息
SOCKET_API int SYS_GetSocketLocalAddress(SOCKET socket, char lpszAddress[], int& iAddressLen, USHORT& usPort);
// 获取 SOCKET 远程地址信息
SOCKET_API int SYS_GetSocketRemoteAddress(SOCKET socket, char lpszAddress[], int& iAddressLen, USHORT& usPort);

/* 枚举主机 IP 地址 */
SOCKET_API int SYS_EnumHostIPAddresses(const char* lpszHost, EnIPAddrType enType, LPTIPAddr** lpppIPAddr, int& iIPAddrCount);
/* 释放 LPTIPAddr* */
SOCKET_API int SYS_FreeHostIPAddresses(LPTIPAddr* lppIPAddr);
/* 检查字符串是否符合 IP 地址格式 */
SOCKET_API int SYS_IsIPAddress(const char* lpszAddress, EnIPAddrType* penType = nullptr);
/* 通过主机名获取 IP 地址 */
SOCKET_API int SYS_GetIPAddress(const char* lpszHost, char lpszIP[], int& iIPLenth, EnIPAddrType& enType);

/* 64 位网络字节序转主机字节序 */
SOCKET_API ULONGLONG SYS_NToH64(ULONGLONG value);
/* 64 位主机字节序转网络字节序 */
SOCKET_API ULONGLONG SYS_HToN64(ULONGLONG value);

// CP_XXX -> UNICODE
//SOCKET_API int SYS_CodePageToUnicode(int iCodePage, const char szSrc[], wchar_t szDest[], int& iDestLength);
// UNICODE -> CP_XXX
//SOCKET_API int SYS_UnicodeToCodePage(int iCodePage, const wchar_t szSrc[], char szDest[], int& iDestLength);
// GBK -> UNICODE
//SOCKET_API int SYS_GbkToUnicode(const char szSrc[], wchar_t szDest[], int& iDestLength);
// UNICODE -> GBK
//SOCKET_API int SYS_UnicodeToGbk(const wchar_t szSrc[], char szDest[], int& iDestLength);
// UTF8 -> UNICODE
//SOCKET_API int SYS_Utf8ToUnicode(const char szSrc[], wchar_t szDest[], int& iDestLength);
// UNICODE -> UTF8
//SOCKET_API int SYS_UnicodeToUtf8(const wchar_t szSrc[], char szDest[], int& iDestLength);
// GBK -> UTF8
//SOCKET_API int SYS_GbkToUtf8(const char szSrc[], char szDest[], int& iDestLength);
// UTF8 -> GBK
//SOCKET_API int SYS_Utf8ToGbk(const char szSrc[], char szDest[], int& iDestLength);

// 普通压缩（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
//SOCKET_API int SYS_Compress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// 高级压缩（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
//（默认参数：iLevel -> -1，iMethod -> 8，iWindowBits -> 15，iMemLevel -> 8，iStrategy -> 0）
//SOCKET_API int SYS_CompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iLevel = -1, int iMethod = 8, int iWindowBits = 15, int iMemLevel = 8, int iStrategy = 0);
// 普通解压（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
//SOCKET_API int SYS_Uncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// 高级解压（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
//（默认参数：iWindowBits -> 15）
//SOCKET_API int SYS_UncompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iWindowBits = 15);
// 推测压缩结果长度
//SOCKET_API DWORD SYS_GuessCompressBound(DWORD dwSrcLen, int bGZip = FALSE);

// Gzip 压缩（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
//SOCKET_API int SYS_GZipCompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// Gzip 解压（返回值：0 -> 成功，-3 -> 输入数据不正确，-5 -> 输出缓冲区不足）
//SOCKET_API int SYS_GZipUncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// 推测 Gzip 解压结果长度（如果返回 0 或不合理值则说明输入内容并非有效的 Gzip 格式）
//SOCKET_API DWORD SYS_GZipGuessUncompressBound(const BYTE* lpszSrc, DWORD dwSrcLen);

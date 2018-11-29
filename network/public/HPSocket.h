
/******************************************************************************
Module:  HPSocket

Usage:
		����һ��
		--------------------------------------------------------------------------------------
		0. Ӧ�ó������ HPTypeDef.h / SocketInterface.h / HPSocket.h ͷ�ļ�
		1. ���� HP_Create_Xxx() �������� HPSocket ����
		2. ʹ����Ϻ���� HP_Destroy_Xxx() �������� HPSocket ����

		��������
		--------------------------------------------------------------------------------------
		0. Ӧ�ó������ SocketInterface.h �� HPSocket.h ͷ�ļ�
		1. ���� CXxxPtr ����ָ�룬ͨ������ָ��ʹ�� HPSocket ����

Release:
		<-- ��̬���ӿ� -->
		1. x86/HPSocket.dll				- (32λ/MBCS/Release)
		2. x86/HPSocket_D.dll			- (32λ/MBCS/DeBug)
		3. x86/HPSocket_U.dll			- (32λ/UNICODE/Release)
		4. x86/HPSocket_UD.dll			- (32λ/UNICODE/DeBug)
		5. x64/HPSocket.dll				- (64λ/MBCS/Release)
		6. x64/HPSocket_D.dll			- (64λ/MBCS/DeBug)
		7. x64/HPSocket_U.dll			- (64λ/UNICODE/Release)
		8. x64/HPSocket_UD.dll			- (64λ/UNICODE/DeBug)

		<-- ��̬���ӿ� -->
		!!ע��!!��ʹ�� HPSocket ��̬��ʱ����Ҫ�ڹ��������ж���Ԥ����� -> HPSOCKET_STATIC_LIB
		1. x86/static/HPSocket.lib		- (32λ/MBCS/Release)
		2. x86/static/HPSocket_D.lib	- (32λ/MBCS/DeBug)
		3. x86/static/HPSocket_U.lib	- (32λ/UNICODE/Release)
		4. x86/static/HPSocket_UD.lib	- (32λ/UNICODE/DeBug)
		5. x64/static/HPSocket.lib		- (64λ/MBCS/Release)
		6. x64/static/HPSocket_D.lib	- (64λ/MBCS/DeBug)
		7. x64/static/HPSocket_U.lib	- (64λ/UNICODE/Release)
		8. x64/static/HPSocket_UD.lib	- (64λ/UNICODE/DeBug)

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
/************** HPSocket ��������ָ�� **************/

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


// ���� ITcpServer ����
SOCKET_API ITcpServer* HP_Create_TcpServer(ITcpServerListener* pListener);
SOCKET_API ITcpClient* HP_Create_TcpClient(ITcpClientListener* pListener);
SOCKET_API ITcpPackServer* HP_Create_TcpPackServer(ITcpServerListener* pListener);
SOCKET_API ITcpPackClient* HP_Create_TcpPackClient(ITcpClientListener* pListener);

// ���� ITcpServer ����
SOCKET_API void HP_Destroy_TcpServer(ITcpServer* pServer);
SOCKET_API void HP_Destroy_TcpClient(ITcpClient* pClient);
SOCKET_API void HP_Destroy_TcpPackServer(ITcpPackServer* pServer);
SOCKET_API void HP_Destroy_TcpPackClient(ITcpPackClient* pClient);

// ITcpServer ���󴴽���
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

// ITcpClient ���󴴽���
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

// ITcpPackServer ���󴴽���
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

// ITcpPackClient ���󴴽���
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

// ITcpServer ��������ָ��
typedef CHPSocketPtr<ITcpServer, ITcpServerListener, TcpServer_Creator>			CTcpServerPtr;
typedef CHPSocketPtr<ITcpClient, ITcpClientListener, TcpClient_Creator>			CTcpClientPtr;
typedef CHPSocketPtr<ITcpPackServer, ITcpServerListener, TcpPackServer_Creator>	CTcpPackServerPtr;
typedef CHPSocketPtr<ITcpPackClient, ITcpClientListener, TcpPackClient_Creator>	CTcpPackClientPtr;


/*****************************************************************************************************************************************************/
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

// ��ȡ HPSocket �汾�ţ�4 ���ֽڷֱ�Ϊ�����汾�ţ��Ӱ汾�ţ������汾�ţ�������ţ�
SOCKET_API DWORD HP_GetHPSocketVersion();

// ��ȡ���������ı�
SOCKET_API const char* HP_GetSocketErrorDesc(EnSocketError enCode);
// ����ϵͳ�� GetLastError() ������ȡϵͳ�������
SOCKET_API DWORD SYS_GetLastError();
// ����ϵͳ�� WSAGetLastError() ������ȡϵͳ�������
SOCKET_API int SYS_WSAGetLastError();
// ����ϵͳ�� setsockopt()
SOCKET_API int SYS_SetSocketOption(SOCKET sock, int level, int name, LPVOID val, int len);
// ����ϵͳ�� getsockopt()
SOCKET_API int SYS_GetSocketOption(SOCKET sock, int level, int name, LPVOID val, int* len);
// ����ϵͳ�� ioctlsocket()
SOCKET_API int SYS_IoctlSocket(SOCKET sock, long cmd, u_long* arg);
// ����ϵͳ�� WSAIoctl()
SOCKET_API int SYS_WSAIoctl(SOCKET sock, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer, LPDWORD lpcbBytesReturned);

// ���� socket ѡ�IPPROTO_TCP -> TCP_NODELAY
SOCKET_API int SYS_SSO_NoDelay(SOCKET sock, int bNoDelay);
// ���� socket ѡ�SOL_SOCKET -> SO_DONTLINGER
SOCKET_API int SYS_SSO_DontLinger(SOCKET sock, int bDont);
// ���� socket ѡ�SOL_SOCKET -> SO_LINGER
SOCKET_API int SYS_SSO_Linger(SOCKET sock, USHORT l_onoff, USHORT l_linger);
// ���� socket ѡ�SOL_SOCKET -> SO_RCVBUF
SOCKET_API int SYS_SSO_RecvBuffSize(SOCKET sock, int size);
// ���� socket ѡ�SOL_SOCKET -> SO_SNDBUF
SOCKET_API int SYS_SSO_SendBuffSize(SOCKET sock, int size);
// ���� socket ѡ�SOL_SOCKET -> SO_REUSEADDR
SOCKET_API int SYS_SSO_ReuseAddress(SOCKET sock, int bReuse);

// ��ȡ SOCKET ���ص�ַ��Ϣ
SOCKET_API int SYS_GetSocketLocalAddress(SOCKET socket, char lpszAddress[], int& iAddressLen, USHORT& usPort);
// ��ȡ SOCKET Զ�̵�ַ��Ϣ
SOCKET_API int SYS_GetSocketRemoteAddress(SOCKET socket, char lpszAddress[], int& iAddressLen, USHORT& usPort);

/* ö������ IP ��ַ */
SOCKET_API int SYS_EnumHostIPAddresses(const char* lpszHost, EnIPAddrType enType, LPTIPAddr** lpppIPAddr, int& iIPAddrCount);
/* �ͷ� LPTIPAddr* */
SOCKET_API int SYS_FreeHostIPAddresses(LPTIPAddr* lppIPAddr);
/* ����ַ����Ƿ���� IP ��ַ��ʽ */
SOCKET_API int SYS_IsIPAddress(const char* lpszAddress, EnIPAddrType* penType = nullptr);
/* ͨ����������ȡ IP ��ַ */
SOCKET_API int SYS_GetIPAddress(const char* lpszHost, char lpszIP[], int& iIPLenth, EnIPAddrType& enType);

/* 64 λ�����ֽ���ת�����ֽ��� */
SOCKET_API ULONGLONG SYS_NToH64(ULONGLONG value);
/* 64 λ�����ֽ���ת�����ֽ��� */
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

// ��ͨѹ��������ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
//SOCKET_API int SYS_Compress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// �߼�ѹ��������ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
//��Ĭ�ϲ�����iLevel -> -1��iMethod -> 8��iWindowBits -> 15��iMemLevel -> 8��iStrategy -> 0��
//SOCKET_API int SYS_CompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iLevel = -1, int iMethod = 8, int iWindowBits = 15, int iMemLevel = 8, int iStrategy = 0);
// ��ͨ��ѹ������ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
//SOCKET_API int SYS_Uncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// �߼���ѹ������ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
//��Ĭ�ϲ�����iWindowBits -> 15��
//SOCKET_API int SYS_UncompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iWindowBits = 15);
// �Ʋ�ѹ���������
//SOCKET_API DWORD SYS_GuessCompressBound(DWORD dwSrcLen, int bGZip = FALSE);

// Gzip ѹ��������ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
//SOCKET_API int SYS_GZipCompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// Gzip ��ѹ������ֵ��0 -> �ɹ���-3 -> �������ݲ���ȷ��-5 -> ������������㣩
//SOCKET_API int SYS_GZipUncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen);
// �Ʋ� Gzip ��ѹ������ȣ�������� 0 �򲻺���ֵ��˵���������ݲ�����Ч�� Gzip ��ʽ��
//SOCKET_API DWORD SYS_GZipGuessUncompressBound(const BYTE* lpszSrc, DWORD dwSrcLen);

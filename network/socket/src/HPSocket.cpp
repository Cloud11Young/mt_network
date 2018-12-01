#include "stdafx.h"
#include "HPSocket.h"
#include "TcpServer.h"
//#include "TcpAgent.h"
#include "TcpClient.h"
//#include "TcpPullServer.h"
// #include "TcpPullClient.h"
// #include "TcpPullAgent.h"
 #include "TcpPackServer.h"
 #include "TcpPackClient.h"
// #include "TcpPackAgent.h"
// #include "UdpServer.h"
// #include "UdpClient.h"
// #include "UdpCast.h"

// #include "HttpServer.h"
// #include "HttpAgent.h"
// #include "HttpClient.h"

//#include "TcpSrvListener.h"

/*****************************************************************************************************************************************************/
/****************************************************************** TCP/UDP Exports ******************************************************************/
/*****************************************************************************************************************************************************/

SOCKET_API ITcpServer* HP_Create_TcpServer(ITcpServerListener* pListener)
{
//	pListener = new ServerListener;
	return new CTcpServer(pListener);
}

// SOCKET_API ITcpAgent* HP_Create_TcpAgent(ITcpAgentListener* pListener)
// {
// 	return new CTcpAgent(pListener);
// }

SOCKET_API ITcpClient* HP_Create_TcpClient(ITcpClientListener* pListener)
{
	return new CTcpClient(pListener);
}

// SOCKET_API ITcpPullServer* HP_Create_TcpPullServer(ITcpServerListener* pListener)
// {
// 	return (ITcpPullServer*)(new CTcpPullServer(pListener));
// }

// SOCKET_API ITcpPullAgent* HP_Create_TcpPullAgent(ITcpAgentListener* pListener)
// {
// 	return (ITcpPullAgent*)(new CTcpPullAgent(pListener));
// }

// SOCKET_API ITcpPullClient* HP_Create_TcpPullClient(ITcpClientListener* pListener)
// {
// 	return (ITcpPullClient*)(new CTcpPullClient(pListener));
// }

SOCKET_API ITcpPackServer* HP_Create_TcpPackServer(ITcpServerListener* pListener)
{
	return (ITcpPackServer*)(new CTcpPackServer(pListener));
}

// SOCKET_API ITcpPackAgent* HP_Create_TcpPackAgent(ITcpAgentListener* pListener)
// {
// 	return (ITcpPackAgent*)(new CTcpPackAgent(pListener));
// }

SOCKET_API ITcpPackClient* HP_Create_TcpPackClient(ITcpClientListener* pListener)
{
	return (ITcpPackClient*)(new CTcpPackClient(pListener));
}

// SOCKET_API IUdpServer* HP_Create_UdpServer(IUdpServerListener* pListener)
// {
// 	return new CUdpServer(pListener);
// }

// SOCKET_API IUdpClient* HP_Create_UdpClient(IUdpClientListener* pListener)
// {
// 	return new CUdpClient(pListener);
// }

// SOCKET_API IUdpCast* HP_Create_UdpCast(IUdpCastListener* pListener)
// {
// 	return new CUdpCast(pListener);
// }

SOCKET_API void HP_Destroy_TcpServer(ITcpServer* pServer)
{
	delete pServer;
}

// SOCKET_API void HP_Destroy_TcpAgent(ITcpAgent* pAgent)
// {
// 	delete pAgent;
// }

SOCKET_API void HP_Destroy_TcpClient(ITcpClient* pClient)
{
	delete pClient;
}

// SOCKET_API void HP_Destroy_TcpPullServer(ITcpPullServer* pServer)
// {
// 	delete pServer;
// }

// SOCKET_API void HP_Destroy_TcpPullAgent(ITcpPullAgent* pAgent)
// {
// 	delete pAgent;
// }

// SOCKET_API void HP_Destroy_TcpPullClient(ITcpPullClient* pClient)
// {
// 	delete pClient;
// }

SOCKET_API void HP_Destroy_TcpPackServer(ITcpPackServer* pServer)
{
	delete pServer;
}

// SOCKET_API void HP_Destroy_TcpPackAgent(ITcpPackAgent* pAgent)
// {
// 	delete pAgent;
// }

SOCKET_API void HP_Destroy_TcpPackClient(ITcpPackClient* pClient)
{
	delete pClient;
}

// SOCKET_API void HP_Destroy_UdpServer(IUdpServer* pServer)
// {
// 	delete pServer;
// }

// SOCKET_API void HP_Destroy_UdpClient(IUdpClient* pClient)
// {
// 	delete pClient;
// }

// SOCKET_API void HP_Destroy_UdpCast(IUdpCast* pCast)
// {
// 	delete pCast;
// }

/*****************************************************************************************************************************************************/
/******************************************************************** HTTP Exports *******************************************************************/
/*****************************************************************************************************************************************************/

// SOCKET_API IHttpServer* HP_Create_HttpServer(IHttpServerListener* pListener)
// {
// 	return (IHttpServer*)(new CHttpServer(pListener));
// }

// SOCKET_API IHttpAgent* HP_Create_HttpAgent(IHttpAgentListener* pListener)
// {
// 	return (IHttpAgent*)(new CHttpAgent(pListener));
// }

// SOCKET_API IHttpClient* HP_Create_HttpClient(IHttpClientListener* pListener)
// {
// 	return (IHttpClient*)(new CHttpClient(pListener));
// }

// SOCKET_API IHttpSyncClient* HP_Create_HttpSyncClient(IHttpClientListener* pListener)
// {
// 	return (IHttpSyncClient*)(new CHttpSyncClient(pListener));
// }

// SOCKET_API void HP_Destroy_HttpServer(IHttpServer* pServer)
// {
// 	delete pServer;
// }

// SOCKET_API void HP_Destroy_HttpAgent(IHttpAgent* pAgent)
// {
// 	delete pAgent;
// }

// SOCKET_API void HP_Destroy_HttpClient(IHttpClient* pClient)
// {
// 	delete pClient;
// }

// SOCKET_API void HP_Destroy_HttpSyncClient(IHttpSyncClient* pClient)
// {
// 	delete pClient;
// }

/**************************************************************************/
/*************************** HTTP Cookie �������� **************************/

// SOCKET_API int HP_HttpCookie_MGR_LoadFromFile(LPCSTR lpszFile, int bKeepExists)
// {
// 	return g_CookieMgr.LoadFromFile(lpszFile, bKeepExists);
// }

// SOCKET_API int HP_HttpCookie_MGR_SaveToFile(LPCSTR lpszFile, int bKeepExists)
// {
// 	return g_CookieMgr.SaveToFile(lpszFile, bKeepExists);
// }

// SOCKET_API int HP_HttpCookie_MGR_ClearCookies(LPCSTR lpszDomain, LPCSTR lpszPath)
// {
// 	return g_CookieMgr.ClearCookies(lpszDomain, lpszPath);
// }

// SOCKET_API int HP_HttpCookie_MGR_RemoveExpiredCookies(LPCSTR lpszDomain, LPCSTR lpszPath)
// {
// 	return g_CookieMgr.RemoveExpiredCookies(lpszDomain, lpszPath);
// }

// SOCKET_API int HP_HttpCookie_MGR_SetCookie(LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge, int bHttpOnly, int bSecure, int enSameSite, int bOnlyUpdateValueIfExists)
// {
// 	return g_CookieMgr.SetCookie(lpszName, lpszValue, lpszDomain, lpszPath, iMaxAge, bHttpOnly, bSecure, (CCookie::EnSameSite)enSameSite, bOnlyUpdateValueIfExists);
// }

// SOCKET_API int HP_HttpCookie_MGR_DeleteCookie(LPCSTR lpszDomain, LPCSTR lpszPath, LPCSTR lpszName)
// {
// 	return g_CookieMgr.DeleteCookie(lpszDomain, lpszPath, lpszName);
// }

// SOCKET_API void HP_HttpCookie_MGR_SetEnableThirdPartyCookie(int bEnableThirdPartyCookie)
// {
// 	g_CookieMgr.SetEnableThirdPartyCookie(bEnableThirdPartyCookie);
// }

// SOCKET_API int HP_HttpCookie_MGR_IsEnableThirdPartyCookie()
// {
// 	return g_CookieMgr.IsEnableThirdPartyCookie();
// }

// SOCKET_API int HP_HttpCookie_HLP_ParseExpires(LPCSTR lpszExpires, __time64_t& tmExpires)
// {
// 	return CCookie::ParseExpires(lpszExpires, tmExpires);
// }

// SOCKET_API int HP_HttpCookie_HLP_MakeExpiresStr(char lpszBuff[], int& iBuffLen, __time64_t tmExpires)
// {
// 	return CCookie::MakeExpiresStr(lpszBuff, iBuffLen, tmExpires);
// }

// SOCKET_API int HP_HttpCookie_HLP_ToString(char lpszBuff[], int& iBuffLen, LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge, int bHttpOnly, int bSecure, int enSameSite)
// {
// 	return CCookie::ToString(lpszBuff, iBuffLen, lpszName, lpszValue, lpszDomain, lpszPath, iMaxAge, bHttpOnly, bSecure, (CCookie::EnSameSite)enSameSite);
// }

// SOCKET_API __time64_t HP_HttpCookie_HLP_CurrentUTCTime()
// {
// 	return CCookie::CurrentUTCTime();
// }

// SOCKET_API __time64_t HP_HttpCookie_HLP_MaxAgeToExpires(int iMaxAge)
// {
// 	return CCookie::MaxAgeToExpires(iMaxAge);
// }

// SOCKET_API int HP_HttpCookie_HLP_ExpiresToMaxAge(__time64_t tmExpires)
// {
// 	return CCookie::ExpiresToMaxAge(tmExpires);
// }

/*****************************************************************************************************************************************************/
/*************************************************************** Global Function Exports *************************************************************/
/*****************************************************************************************************************************************************/

SOCKET_API DWORD HP_GetHPSocketVersion()
{
	return ::GetHPSocketVersion();
}

SOCKET_API const char* HP_GetSocketErrorDesc(EnSocketError enCode)
{
	return ::GetSocketErrorDesc(enCode);
}

SOCKET_API DWORD SYS_GetLastError()
{
	return ::GetLastError();
}

SOCKET_API int SYS_WSAGetLastError()
{
	return ::WSAGetLastError();
}

SOCKET_API int SYS_SetSocketOption(SOCKET sock, int level, int name, LPVOID val, int len)
{
	return ::SSO_SetSocketOption(sock, level, name, val, len);
}

SOCKET_API int SYS_GetSocketOption(SOCKET sock, int level, int name, LPVOID val, int* len)
{
	return ::SSO_GetSocketOption(sock, level, name, val, len);
}

SOCKET_API int SYS_IoctlSocket(SOCKET sock, long cmd, u_long* arg)
{
	return ::SSO_IoctlSocket(sock, cmd, arg);
}

SOCKET_API int SYS_WSAIoctl(SOCKET sock, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer, LPDWORD lpcbBytesReturned)
{
	return ::SSO_WSAIoctl(sock, dwIoControlCode, lpvInBuffer, cbInBuffer, lpvOutBuffer, cbOutBuffer, lpcbBytesReturned);
}

SOCKET_API int SYS_SSO_NoDelay(SOCKET sock, int bNoDelay)
{
	return ::SSO_NoDelay(sock, bNoDelay);
}

SOCKET_API int SYS_SSO_DontLinger(SOCKET sock, int bDont)
{
	return ::SSO_DontLinger(sock, bDont);
}

SOCKET_API int SYS_SSO_Linger(SOCKET sock, USHORT l_onoff, USHORT l_linger)
{
	return ::SSO_Linger(sock, l_onoff, l_linger);
}

SOCKET_API int SYS_SSO_RecvBuffSize(SOCKET sock, int size)
{
	return ::SSO_RecvBuffSize(sock, size);
}

SOCKET_API int SYS_SSO_SendBuffSize(SOCKET sock, int size)
{
	return ::SSO_SendBuffSize(sock, size);
}

SOCKET_API int SYS_SSO_ReuseAddress(SOCKET sock, int bReuse)
{
	return ::SSO_ReuseAddress(sock, bReuse);
}

SOCKET_API int SYS_GetSocketLocalAddress(SOCKET socket, char lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	return ::GetSocketLocalAddress(socket, lpszAddress, iAddressLen, usPort);
}

SOCKET_API int SYS_GetSocketRemoteAddress(SOCKET socket, char lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	return ::GetSocketRemoteAddress(socket, lpszAddress, iAddressLen, usPort);
}

SOCKET_API int SYS_EnumHostIPAddresses(const char* lpszHost, EnIPAddrType enType, __out LPTIPAddr** lpppIPAddr, __out int& iIPAddrCount)
{
	return ::EnumHostIPAddresses(lpszHost, enType, lpppIPAddr, iIPAddrCount);
}

SOCKET_API int SYS_FreeHostIPAddresses(LPTIPAddr* lppIPAddr)
{
	return ::FreeHostIPAddresses(lppIPAddr);
}

SOCKET_API int SYS_IsIPAddress(const char* lpszAddress, EnIPAddrType* penType)
{
	return ::IsIPAddress(lpszAddress, penType);
}

SOCKET_API int SYS_GetIPAddress(const char* lpszHost, char lpszIP[], int& iIPLenth, EnIPAddrType& enType)
{
	return ::GetIPAddress(lpszHost, lpszIP, iIPLenth, enType);
}

SOCKET_API ULONGLONG SYS_NToH64(ULONGLONG value)
{
	return ::NToH64(value);
}

SOCKET_API ULONGLONG SYS_HToN64(ULONGLONG value)
{
	return ::HToN64(value);
}

// SOCKET_API int SYS_CodePageToUnicode(int iCodePage, const char szSrc[], wchar_t szDest[], int& iDestLength)
// {
// 	return ::CodePageToUnicode(iCodePage, szSrc, szDest, iDestLength);
// }

// SOCKET_API int SYS_UnicodeToCodePage(int iCodePage, const wchar_t szSrc[], char szDest[], int& iDestLength)
// {
// 	return ::UnicodeToCodePage(iCodePage, szSrc, szDest, iDestLength);
// }

// SOCKET_API int SYS_GbkToUnicode(const char szSrc[], wchar_t szDest[], int& iDestLength)
// {
// 	return ::GbkToUnicode(szSrc, szDest, iDestLength);
// }

// SOCKET_API int SYS_UnicodeToGbk(const wchar_t szSrc[], char szDest[], int& iDestLength)
// {
// 	return ::UnicodeToGbk(szSrc, szDest, iDestLength);
// }

// SOCKET_API int SYS_Utf8ToUnicode(const char szSrc[], wchar_t szDest[], int& iDestLength)
// {
// 	return ::Utf8ToUnicode(szSrc, szDest, iDestLength);
// }

// SOCKET_API int SYS_UnicodeToUtf8(const wchar_t szSrc[], char szDest[], int& iDestLength)
// {
// 	return ::UnicodeToUtf8(szSrc, szDest, iDestLength);
// }

// SOCKET_API int SYS_GbkToUtf8(const char szSrc[], char szDest[], int& iDestLength)
// {
// 	return ::GbkToUtf8(szSrc, szDest, iDestLength);
// }

// SOCKET_API int SYS_Utf8ToGbk(const char szSrc[], char szDest[], int& iDestLength)
// {
// 	return ::Utf8ToGbk(szSrc, szDest, iDestLength);
// }

// SOCKET_API int SYS_Compress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
// {
// 	return ::Compress(lpszSrc, dwSrcLen, lpszDest, dwDestLen);
// }

// SOCKET_API int SYS_CompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iLevel, int iMethod, int iWindowBits, int iMemLevel, int iStrategy)
// {
// 	return ::CompressEx(lpszSrc, dwSrcLen, lpszDest, dwDestLen, iLevel, iMethod, iWindowBits, iMemLevel, iStrategy);
// }

// SOCKET_API int SYS_Uncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
// {
// 	return ::Uncompress(lpszSrc, dwSrcLen, lpszDest, dwDestLen);
// }

// SOCKET_API int SYS_UncompressEx(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen, int iWindowBits)
// {
// 	return ::UncompressEx(lpszSrc, dwSrcLen, lpszDest, dwDestLen, iWindowBits);
// }

// SOCKET_API DWORD SYS_GuessCompressBound(DWORD dwSrcLen, int bGZip)
// {
// 	return ::GuessCompressBound(dwSrcLen, bGZip);
// }

// SOCKET_API int SYS_GZipCompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
// {
// 	return ::GZipCompress(lpszSrc, dwSrcLen, lpszDest, dwDestLen);
// }

// SOCKET_API int SYS_GZipUncompress(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
// {
// 	return ::GZipUncompress(lpszSrc, dwSrcLen, lpszDest, dwDestLen);
// }

// SOCKET_API DWORD SYS_GZipGuessUncompressBound(const BYTE* lpszSrc, DWORD dwSrcLen)
// {
// 	return ::GZipGuessUncompressBound(lpszSrc, dwSrcLen);
// }

// SOCKET_API DWORD SYS_GuessBase64EncodeBound(DWORD dwSrcLen)
// {
// 	return ::GuessBase64EncodeBound(dwSrcLen);
// }

// SOCKET_API DWORD SYS_GuessBase64DecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen)
// {
// 	return ::GuessBase64DecodeBound(lpszSrc, dwSrcLen);
// }

// SOCKET_API int SYS_Base64Encode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
// {
// 	return ::Base64Encode(lpszSrc, dwSrcLen, lpszDest, dwDestLen);
// }

// SOCKET_API int SYS_Base64Decode(const BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
// {
// 	return ::Base64Decode(lpszSrc, dwSrcLen, lpszDest, dwDestLen);
// }

// SOCKET_API DWORD SYS_GuessUrlEncodeBound(const BYTE* lpszSrc, DWORD dwSrcLen)
// {
// 	return ::GuessUrlEncodeBound(lpszSrc, dwSrcLen);
// }

// SOCKET_API DWORD SYS_GuessUrlDecodeBound(const BYTE* lpszSrc, DWORD dwSrcLen)
// {
// 	return ::GuessUrlDecodeBound(lpszSrc, dwSrcLen);
// }

// SOCKET_API int SYS_UrlEncode(BYTE* lpszSrc, DWORD dwSrcLen, BYTE* lpszDest, DWORD& dwDestLen)
// {
// 	return ::UrlEncode(lpszSrc, dwSrcLen, lpszDest, dwDestLen);
// }

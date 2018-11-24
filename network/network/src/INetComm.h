// INetComm.h: interface for the INetComm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INETCOMM_H__ED63B975_9DA9_4238_AACD_10F7EC3D58C9__INCLUDED_)
#define AFX_INETCOMM_H__ED63B975_9DA9_4238_AACD_10F7EC3D58C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <list>

#ifdef NET_EXPORT
#define NET_CLASS _declspec(dllexport)
#else
#define NET_CLASS _declspec(dllimport)
#endif

using namespace std;

#define SEND_SYN  0  // 同步
#define SEND_ASYN 1  // 异步


typedef void(__stdcall *LPCONNECT_CALLBACK)(PVOID pThis, wchar_t* strIP, DWORD dwPort, wchar_t* strPcName);
typedef void(__stdcall *LPDISCONNECT_CALLBACK)(PVOID pThis, wchar_t* strIP, DWORD dwPort);
typedef void(__stdcall *LPRECVMSG_CALLBACK)(PVOID pThis, PVOID pMsg, DWORD dwMsgLen, wchar_t* strIP, DWORD dwPort);
typedef void(__stdcall *LPPREAUTO_CONNECT_CALLBACK)(PVOID pThis, wchar_t* strIP, DWORD dwPort);
typedef void(__stdcall *LPPOSTAUTO_CONNECT_CALLBACK)(PVOID pThis, wchar_t* strIP, DWORD dwPort, BOOL bOK);
typedef void(__stdcall *LPERROR_CALLBACK)(PVOID pThis, wchar_t* strIP, DWORD dwPort, const wchar_t* msg);

typedef struct _USER_CB
{
	_USER_CB() :lpConnectCB(NULL), lpDisconnectCB(NULL), lpRecvMsgCB(NULL),
	lpPreAutoConnectCB(NULL), lpPostAutoConnectCB(NULL), lpCallBackData(NULL),
	lpErrorCB(NULL)
	{}
	LPCONNECT_CALLBACK		lpConnectCB;
	LPDISCONNECT_CALLBACK	lpDisconnectCB;
	LPRECVMSG_CALLBACK		lpRecvMsgCB;
	LPPREAUTO_CONNECT_CALLBACK lpPreAutoConnectCB;
	LPPOSTAUTO_CONNECT_CALLBACK lpPostAutoConnectCB;
	PVOID					lpCallBackData;
	LPERROR_CALLBACK        lpErrorCB;
}USER_CB, *PUSER_CB;

class NET_CLASS INetComm
{
public:
	static BOOL CreateInstance(INetComm **ppINetComm);
	virtual void Release() PURE;

	virtual BOOL Initialize(PVOID pThis, PUSER_CB callback, DWORD dwPort, wchar_t* strIp) PURE;//需要提供Server服务
	virtual BOOL Initialize(PVOID pThis, PUSER_CB callback) PURE;//不需要提供Server服务
	virtual BOOL GetStatus(BOOL &bIsServer, BOOL &bIsClient) PURE;
	virtual BOOL ConnectTo(wchar_t* pIP, DWORD uPort, BOOL bAutoReconnect = TRUE) PURE;
	virtual BOOL Disconnect(wchar_t* pIP, DWORD uPort) PURE;

	virtual BOOL SendMsg(LPVOID pMsg, DWORD dwMsgLen, wchar_t* pIP, DWORD uPort, DWORD dwWay = SEND_ASYN) PURE;
	virtual BOOL GetSocket(wchar_t* pIP, DWORD uPort, list<HANDLE> SocketList) PURE;

	virtual BOOL Uninitialize() PURE;
};

#endif // !defined(AFX_INETCOMM_H__ED63B975_9DA9_4238_AACD_10F7EC3D58C9__INCLUDED_)

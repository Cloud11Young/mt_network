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


typedef void(__stdcall *LPCONNECT_CALLBACK)(PVOID pThis, char* strIP, USHORT dwPort, char* strPcName);
typedef void(__stdcall *LPDISCONNECT_CALLBACK)(PVOID pThis, char* strIP, USHORT dwPort);
typedef void(__stdcall *LPRECVMSG_CALLBACK)(PVOID pThis, PVOID pMsg, DWORD dwMsgLen, char* strIP, USHORT dwPort);
typedef void(__stdcall *LPPREAUTO_CONNECT_CALLBACK)(PVOID pThis, char* strIP, USHORT dwPort);
typedef void(__stdcall *LPPOSTAUTO_CONNECT_CALLBACK)(PVOID pThis, char* strIP, USHORT dwPort, BOOL bOK);
typedef void(__stdcall *LPERROR_CALLBACK)(PVOID pThis, char* strIP, USHORT dwPort, const char* msg);

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
	virtual void Release() =0;

	virtual BOOL Initialize(PVOID pThis, PUSER_CB callback, USHORT dwPort, char* strIp) = 0;//需要提供Server服务
	virtual BOOL Initialize(PVOID pThis, PUSER_CB callback) = 0;//不需要提供Server服务
	virtual BOOL GetStatus(BOOL &bIsServer, BOOL &bIsClient) = 0;
	virtual BOOL ConnectTo(char* pIP, USHORT uPort, BOOL bAutoReconnect = TRUE) = 0;
	virtual BOOL Disconnect(char* pIP, USHORT uPort) = 0;

	virtual BOOL SendMsg(LPVOID pMsg, DWORD dwMsgLen, char* pIP, USHORT uPort, DWORD dwWay = SEND_ASYN) = 0;
	virtual BOOL GetSocket(char* pIP, USHORT uPort, list<HANDLE> SocketList) = 0;

	virtual BOOL Uninitialize() = 0;
};

#endif // !defined(AFX_INETCOMM_H__ED63B975_9DA9_4238_AACD_10F7EC3D58C9__INCLUDED_)

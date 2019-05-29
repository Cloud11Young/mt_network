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

//using namespace std;

#define SEND_SYN  0  // 同步
#define SEND_ASYN 1  // 异步

typedef unsigned short ushort;
typedef unsigned long  ulong;

typedef void(*LPCONNECT_CALLBACK)(void* pThis, const char* strIP, ushort dwPort, const char* strPcName);
typedef void(*LPDISCONNECT_CALLBACK)(void* pThis, const char* strIP, ushort dwPort);
typedef void(*LPRECVMSG_CALLBACK)(void* pThis, void* pMsg, ulong dwMsgLen, const char* strIP, ushort dwPort);
typedef void(*LPPREAUTO_CONNECT_CALLBACK)(void* pThis, const char* strIP, ushort dwPort);
typedef void(*LPPOSTAUTO_CONNECT_CALLBACK)(void* pThis, const char* strIP, ushort dwPort, int bOK);
typedef void(*LPERROR_CALLBACK)(void* pThis, const char* strIP, ushort dwPort, const char* msg);
typedef void(*LPLOG_CALLBACK)(int severity, const char* msg);

typedef struct _USER_CB
{
	_USER_CB() :lpConnectCB(NULL), lpDisconnectCB(NULL), lpRecvMsgCB(NULL),
	lpPreAutoConnectCB(NULL), lpPostAutoConnectCB(NULL), lpCallBackData(NULL),
	lpErrorCB(NULL), lpLogCB(NULL)
	{}
	LPCONNECT_CALLBACK		lpConnectCB;
	LPDISCONNECT_CALLBACK	lpDisconnectCB;
	LPRECVMSG_CALLBACK		lpRecvMsgCB;
	LPPREAUTO_CONNECT_CALLBACK lpPreAutoConnectCB;
	LPPOSTAUTO_CONNECT_CALLBACK lpPostAutoConnectCB;
	void*					lpCallBackData;
	LPERROR_CALLBACK        lpErrorCB;
	LPLOG_CALLBACK			lpLogCB;
}USER_CB, *PUSER_CB;

class NET_CLASS INetComm
{
public:
	static int CreateInstance(INetComm **ppINetComm);
	virtual void Release() =0;

	virtual int Initialize(void* pThis, PUSER_CB callback, ushort dwPort, const char* strIp) = 0;//需要提供Server服务
	virtual int Initialize(void* pThis, PUSER_CB callback) = 0;//不需要提供Server服务
	virtual int GetStatus(int &bIsServer, int &bIsClient) = 0;
	virtual int ConnectTo(const char* pIP, ushort uPort, int bAutoReconnect = 1) = 0;
	virtual int Disconnect(const char* pIP, ushort uPort) = 0;

	virtual int SendMsg(void* pMsg, ulong dwMsgLen, const char* pIP, ushort uPort, ulong dwWay = SEND_ASYN) = 0;
//	virtual int GetSocket(char* pIP, unsigned short uPort, std::list<HANDLE> SocketList) = 0;

	virtual int Uninitialize() = 0;
};

#endif // !defined(AFX_INETCOMM_H__ED63B975_9DA9_4238_AACD_10F7EC3D58C9__INCLUDED_)

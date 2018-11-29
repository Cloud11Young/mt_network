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

#define SEND_SYN  0  // ͬ��
#define SEND_ASYN 1  // �첽

typedef unsigned short ushort;
typedef unsigned long  ulong;

typedef void (*LPCONNECT_CALLBACK)(void* pThis, char* strIP, unsigned short dwPort, char* strPcName);
typedef void (*LPDISCONNECT_CALLBACK)(void* pThis, char* strIP, unsigned short dwPort);
typedef void (*LPRECVMSG_CALLBACK)(void* pThis, void* pMsg, unsigned long dwMsgLen, char* strIP, unsigned short dwPort);
typedef void(*LPPREAUTO_CONNECT_CALLBACK)(void* pThis, char* strIP, unsigned short dwPort);
typedef void (*LPPOSTAUTO_CONNECT_CALLBACK)(void* pThis, char* strIP, unsigned short dwPort, int bOK);
typedef void (*LPERROR_CALLBACK)(void* pThis, char* strIP, unsigned short dwPort, const char* msg);

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
	void*					lpCallBackData;
	LPERROR_CALLBACK        lpErrorCB;
}USER_CB, *PUSER_CB;

class NET_CLASS INetComm
{
public:
	static int CreateInstance(INetComm **ppINetComm);
	virtual void Release() =0;

	virtual int Initialize(void* pThis, PUSER_CB callback, unsigned short dwPort, char* strIp) = 0;//��Ҫ�ṩServer����
	virtual int Initialize(void* pThis, PUSER_CB callback) = 0;//����Ҫ�ṩServer����
	virtual int GetStatus(int &bIsServer, int &bIsClient) = 0;
	virtual int ConnectTo(char* pIP, unsigned short uPort, int bAutoReconnect = 1) = 0;
	virtual int Disconnect(char* pIP, unsigned short uPort) = 0;

	virtual int SendMsg(void* pMsg, unsigned long dwMsgLen, char* pIP, unsigned short uPort, unsigned long dwWay = SEND_ASYN) = 0;
//	virtual int GetSocket(char* pIP, unsigned short uPort, std::list<HANDLE> SocketList) = 0;

	virtual int Uninitialize() = 0;
};

#endif // !defined(AFX_INETCOMM_H__ED63B975_9DA9_4238_AACD_10F7EC3D58C9__INCLUDED_)

// INetComm.h: interface for the INetComm class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INETCOMM_H__ED63B975_9DA9_4238_AACD_10F7EC3D58C9__INCLUDED_)
#define AFX_INETCOMM_H__ED63B975_9DA9_4238_AACD_10F7EC3D58C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <list>

#include "NetworkDefine.h"
#include "IServer.h"
#include "IClient.h"

#ifdef NET_EXPORT
#define NetAPI _declspec(dllexport)
#else
#define NetAPI _declspec(dllimport)
#endif

//using namespace std;

#define SEND_SYN  0  // 同步
#define SEND_ASYN 1  // 异步

//typedef unsigned short ushort;
//typedef unsigned long  ulong;

class NetAPI INetComm
{
public:
	static int CreateInstance(INetComm **ppINetComm);
	virtual void Release() = 0;

	virtual int InitServer(void* pThis, ICallback* callback, uint16_t dwPort, const char* strIp) = 0;	//需要提供Server服务
	virtual int InitClient(void* pThis, ICallback* callback) = 0;										//不需要提供Server服务
	virtual int GetStatus(int &bIsServer, int &bIsClient) = 0;
	virtual int ConnectTo(const char* pIP, uint16_t uPort, int bAutoReconnect = 1) = 0;
	virtual int Disconnect(const char* pIP, uint16_t uPort) = 0;

	virtual int SendMsg(void* pMsg, size_t dwMsgLen, const char* pIP, uint16_t uPort) = 0;
//	virtual int GetSocket(char* pIP, unsigned short uPort, std::list<HANDLE> SocketList) = 0;

	virtual int Uninitialize() = 0;
};

namespace Network
{
	NetAPI IServer* CreateServer(ICallback*);
	NetAPI IClient* CreateClient(ICallback*);

	NetAPI void ReleaseServer(IServer* pServer);
	NetAPI void ReleaseClient(IClient* pClient);
}
#endif 

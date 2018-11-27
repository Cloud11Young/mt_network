#ifndef CNETCOMM_H_
#define CNETCOMM_H_

#include <afxstr.h>
#include "INetComm.h"

#include "../public/HPSocket.h"
#include "../public/HPTypeDef.h"
#include "../public/SocketInterface.h"
#include "../public/CriticalSection.h"
#include "Listener.h"
#include "DataType.h"

#define IP_LEN 32

class CNetComm :public INetComm{
public:
	CNetComm();
	virtual ~CNetComm();
	virtual void Release();

	virtual int Initialize(void* pThis, PUSER_CB callback, USHORT dwPort, char* strIp);//需要提供Server服务
	virtual int Initialize(void* pThis, PUSER_CB callback);//不需要提供Server服务
	virtual int GetStatus(int &bIsServer, int &bIsClient);
	virtual int ConnectTo(char* pIP, USHORT uPort, BOOL bAutoReconnect = TRUE);
	virtual int Disconnect(char* pIP, USHORT uPort);

	virtual int SendMsg(void* pMsg, DWORD dwMsgLen, char* pIP, USHORT uPort, DWORD dwWay = SEND_ASYN);
	virtual int GetSocket(char* pIP, USHORT uPort, list<HANDLE> SocketList);

	virtual int Uninitialize();
private:
	int StartConnectThread(const char* IP, USHORT port);
	static UINT WINAPI ConnectThread(LPVOID p);
private:
	ITcpServer* m_pServer;
	ITcpClient* m_pClient;
	PUSER_CB    m_pServerCtrl;
	PUSER_CB    m_pClientCtrl;
	int		m_bAutoReconnect;
	ServerListener* m_pSrvListen;
	ClientListener* m_pClientListen;
	CMTX     m_extractlock;
	char     m_conIP[IP_LEN];
	USHORT   m_conPort;
	int      m_bConStart;
};

#endif
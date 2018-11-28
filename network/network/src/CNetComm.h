#ifndef CNETCOMM_H_
#define CNETCOMM_H_

//#include <afxstr.h>
#include "INetComm.h"

#include "../public/HPSocket.h"
#include "../public/HPTypeDef.h"
#include "../public/SocketInterface.h"
//#include "../public/CriticalSection.h"
#include "Listener.h"
#include "DataType.h"

#define IP_LEN 32

class CNetComm :public INetComm{
public:
	CNetComm();
	virtual ~CNetComm();
	virtual void Release();

	virtual int Initialize(void* pThis, PUSER_CB callback, unsigned short dwPort, char* strIp);//需要提供Server服务
	virtual int Initialize(void* pThis, PUSER_CB callback);//不需要提供Server服务
	virtual int GetStatus(int &bIsServer, int &bIsClient);
	virtual int ConnectTo(char* pIP, unsigned short uPort, int bAutoReconnect = TRUE);
	virtual int Disconnect(char* pIP, unsigned short uPort);

	virtual int SendMsg(void* pMsg, unsigned long dwMsgLen, char* pIP, unsigned short uPort, unsigned long dwWay = SEND_ASYN);
//	virtual int GetSocket(char* pIP, unsigned short uPort, std::list<HANDLE> SocketList);

	virtual int Uninitialize();
private:
	int StartConnectThread(const char* IP, unsigned short port);
	static UINT WINAPI ConnectThread(LPVOID p);
private:
	ITcpPackServer* m_pServer;
	ITcpPackClient* m_pClient;
	PUSER_CB    m_pServerCtrl;
	PUSER_CB    m_pClientCtrl;
	int		m_bAutoReconnect;
	ServerListener* m_pSrvListen;
	ClientListener* m_pClientListen;
//	CMTX     m_extractlock;
	char			m_conIP[IP_LEN];
	unsigned short  m_conPort;
	int				m_bConStart;
};

#endif
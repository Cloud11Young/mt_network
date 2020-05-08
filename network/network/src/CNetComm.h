#ifndef CNETCOMM_H_
#define CNETCOMM_H_

//#include <afxstr.h>
#include "INetComm.h"
#include "NetHelper.h"
#include "event.h"
#include "Callback.h"

// #include "../public/HPSocket.h"
// #include "../public/HPTypeDef.h"
// #include "../public/SocketInterface.h"
//#include "../public/CriticalSection.h"
//#include "Listener.h"
//#include "DataType.h"

#define IP_LEN 32

#define NET_TYPE_CLIENT 0x01
#define NET_TYPE_SERVER 0x02

enum NetRet
{
	Ret_failed = 0,
	Ret_ok = 1
};


class CNetComm :public INetComm{
public:
	CNetComm();
	virtual ~CNetComm();
	virtual void Release();

	virtual int InitServer(void* pThis, ICallback* callback, uint16_t dwPort, const char* strIp);	//需要提供Server服务
	virtual int InitClient(void* pThis, ICallback* callback);										//不需要提供Server服务
	virtual int GetStatus(int &bIsServer, int &bIsClient);
	virtual int ConnectTo(const char* pIP, uint16_t uPort, int bAutoReconnect = 0);
	virtual int Disconnect(const char* pIP, uint16_t uPort);

	virtual int SendMsg(void* pMsg, size_t dwMsgLen, const char* pIP, uint16_t uPort);
//	virtual int GetSocket(char* pIP, unsigned short uPort, std::list<HANDLE> SocketList);
	virtual int Uninitialize();

	static void TimerCallback(evutil_socket_t, short, void *);

private:
	int StartConnectThread(const char* IP, unsigned short port);
	static unsigned int __stdcall ConnectThread(void* p);
	static unsigned int __stdcall LoopThread(void* p);
private:
	static const CInitWinSocket m_initSocket;
	static CNetComm* m_pNetComm;

	CServerCallback m_ServerCallback;
	CClientCallback m_ClientCallback;
	int		m_bAutoReconnect;
//	CMTX     m_extractlock;
	char            m_srvIP[IP_LEN];
	uint16_t        m_srvPort;
	char			m_conIP[IP_LEN];
	unsigned short  m_conPort;
	int				m_bConStart;

	struct event_base* m_srvBase;
	struct event_base* m_clientBase;
	struct evconnlistener* m_pListener;
	struct bufferevent* m_clientEventbuf;

	unsigned char m_NetType;
	HANDLE m_pThread[2];
};

#endif
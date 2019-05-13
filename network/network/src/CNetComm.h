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

typedef enum NetRet
{
	Ret_failed = 0,
	Ret_ok = 1
};


class CNetComm :public INetComm{
public:
	CNetComm();
	virtual ~CNetComm();
	virtual void Release();

	virtual int Initialize(void* pThis, PUSER_CB callback, ushort dwPort, const char* strIp);//需要提供Server服务
	virtual int Initialize(void* pThis, PUSER_CB callback);//不需要提供Server服务
	virtual int GetStatus(int &bIsServer, int &bIsClient);
	virtual int ConnectTo(const char* pIP, ushort uPort, int bAutoReconnect = 0);
	virtual int Disconnect(const char* pIP, ushort uPort);

	virtual int SendMsg(void* pMsg, ulong dwMsgLen, const char* pIP, ushort uPort, ulong dwWay = SEND_ASYN);
//	virtual int GetSocket(char* pIP, unsigned short uPort, std::list<HANDLE> SocketList);
	virtual int Uninitialize();

//	static void ListenerCallback(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* user_data);
//	static void EventReadCallback(bufferevent* bev, void* arg);
//	static void EventWriteCallback(bufferevent* bev, void* arg);
//	static void EventCallback(bufferevent* bev, short events, void* arg);
	static void TimerCallback(evutil_socket_t, short, void *);

private:
	int StartConnectThread(const char* IP, unsigned short port);
	static unsigned int __stdcall ConnectThread(void* p);
private:
	static const CInitWinSocket m_initSocket;
	static CNetComm* m_pNetComm;

	CServerCallback m_ServerCallback;
	CClientCallback m_ClientCallback;
//	ITcpPackServer* m_pServer;
//	ITcpPackClient* m_pClient;
//	PUSER_CB    m_pServerCtrl;
//	PUSER_CB    m_pClientCtrl;
	int		m_bAutoReconnect;
//	ServerListener* m_pSrvListen;
//	ClientListener* m_pClientListen;
//	CMTX     m_extractlock;
	char            m_srvIP[IP_LEN];
	ushort          m_srvPort;
	char			m_conIP[IP_LEN];
	unsigned short  m_conPort;
	int				m_bConStart;

	struct event_base* m_srvBase;
	struct event_base* m_clientBase;
	struct evconnlistener* m_pListener;
	struct bufferevent* m_clientEventbuf;

	unsigned char m_NetType;
};

#endif
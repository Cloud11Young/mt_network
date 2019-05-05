#include "stdafx.h"
#include "CNetComm.h"
//#include "BufferPool.h"
#include <process.h>
#include <windows.h>
#include <WinSock2.h>
#include "event2/thread.h"
#include "event2/listener.h"
/*#include "include/event.h"*/
// #include "mtHelper.h"
// #include "log4cpp/Category.hh"
// #include "log4cpp/RollingFileAppender.hh"
// #include "log4cpp/PatternLayout.hh"



static void timer_cb(evutil_socket_t, short, void *){
	printf("timer_cb callback\n");
}

static void socket_read_cb(bufferevent *bev, void *arg)
{
	char msg[4096];

	size_t len;
	// 这里一行一行的读取
	char* p = evbuffer_readln(bufferevent_get_input(bev), &len, EVBUFFER_EOL_ANY);
	if (p)
	{
		// 如果输入exit或者quit则退出程序
		// 可以使用event_base_loopexit或者event_base_loopbreak
		// 它们的区别是前者会把事件处理完才退出，后者是立即退出
		if (!strcmp(p, "exit"))
			event_base_loopexit(bufferevent_get_base(bev), NULL);
		else if (!strcmp(p, "quit"))
			event_base_loopbreak(bufferevent_get_base(bev));

		printf("recv data:%s\n", p);

		int n = sprintf_s(msg, "srv recv data:%s\n", p);
		//发送消息给客户端
		bufferevent_write(bev, msg, n);

		// 这里记得把分配的内存释放掉，不然会内存泄漏
		free(p);
	}
}

static void socket_write_cb(bufferevent* bev, void* arg)
{

}

static void socket_event_cb(bufferevent *bev, short events, void *arg)
{
	if (events & BEV_EVENT_EOF)
		printf("connection closed\n");
	else if (events & BEV_EVENT_ERROR)
		printf("some other error\n");

	//这将自动close套接字和free读写缓冲区  
	bufferevent_free(bev);
}

static void conn_read_cb(bufferevent* bev, void* arg)
{
	printf("%s:\n", __FUNCTION__);
}

static void conn_write_cb(bufferevent* bev, void* arg)
{
	printf("%s:\n", __FUNCTION__);
}

static void conn_event_cb(bufferevent* bev, short events, void* arg)
{
	printf("%s:\n", __FUNCTION__);
}

static void listener_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* user_data)
{
	char Buffer[256];
	sockaddr_in* addr = (sockaddr_in*)sock;
	evutil_inet_ntop(addr->sin_family, &addr->sin_addr, Buffer, sizeof(Buffer));
	printf("accept a client %d,IP:%s\n", fd, Buffer);

	event_base *base = (event_base*)user_data;

	//为这个客户端分配一个bufferevent  
	bufferevent *bev = bufferevent_socket_new(base, fd,
		BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);

	bufferevent_setcb(bev, socket_read_cb, NULL, socket_event_cb, NULL);
	bufferevent_enable(bev, EV_READ | EV_PERSIST);
}

const CInitWinSocket CNetComm::m_initSocket;

CNetComm::CNetComm() :m_srvBase(NULL), m_clientBase(NULL), m_pListener(NULL)
{
	m_bConStart = 0;
	memset(m_conIP, 0, sizeof(*m_conIP)*IP_LEN);
	memset(m_srvIP, 0, sizeof(*m_srvIP)*IP_LEN);

//	m_hExit = CreateEvent(NULL, TRUE, FALSE, NULL);
//	Initlog();
}

CNetComm::~CNetComm()
{
	Uninitialize();
// 	if (m_pServer)
// 		delete m_pServer;
// 	if (m_pClient)
// 		delete m_pClient;
// 	if (m_pSrvListen)
// 		delete m_pSrvListen;
// 	if (m_pClientListen)
// 		delete m_pClientListen;	
}

void CNetComm::Release(){

}

//需要提供Server服务
int CNetComm::Initialize(void* pThis, PUSER_CB callback, ushort dwPort, const char* strIp)
{
	struct sockaddr_in sin;

	m_pServerCtrl = callback;
// 	strcpy_s(m_srvIP, strIp);
 	m_srvPort = dwPort;

	evthread_use_windows_threads();
	struct event_config* cfg = event_config_new();
	event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);
	m_srvBase = event_base_new_with_config(cfg);
	if (m_srvBase == NULL)
	{
		if (m_pServerCtrl && m_pServerCtrl->lpErrorCB != NULL)
		{
			m_pServerCtrl->lpErrorCB(m_pServerCtrl->lpCallBackData, strIp, dwPort, "server create event_base failed");
			return Ret_failed;
		}
	}

	struct event* ev = event_new(m_srvBase, 1, 1, timer_cb, NULL);
	evtimer_set(ev, timer_cb, NULL);

	event_base_set(m_srvBase, ev);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(dwPort);

	m_pListener = evconnlistener_new_bind(m_srvBase, listener_cb, m_srvBase,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));

	event_base_dispatch(m_srvBase);
	

//	m_pServer = HP_Create_TcpPackServer(m_pSrvListen);
// 	if (!m_pServer)
// 	{
// 		if (m_pServerCtrl && m_pServerCtrl->lpErrorCB != NULL)
// 		{
// 			m_pServerCtrl->lpErrorCB(m_pServerCtrl->lpCallBackData, strIp, dwPort, "server Create object failed");
// 		}
// 		return 0;
// 	}	
//	m_pServer->SetMaxPackSize(0x3FFFFFF);

// 	if (!m_pServer->Start(strIp, dwPort))
// 	{
// 		const char* err = m_pServer->GetLastErrorDesc();
// //		m_pServer->GetLastError();
// 		if (m_pServerCtrl && m_pServerCtrl->lpErrorCB != NULL)
// 		{
// 			m_pServerCtrl->lpErrorCB(m_pServerCtrl->lpCallBackData, strIp, dwPort, err);
// 		}
// 		return 0;
// 	}

	int len = IP_LEN;
// 	m_pServer->GetListenAddress(m_srvIP, len, m_srvPort);
// 		
// 	if (!m_pServer->HasStarted())
// 		return 0;

	return 1;
}

//不需要提供Server服务
int CNetComm::Initialize(void* pThis, PUSER_CB callback)
{
	m_pClientCtrl = callback;

//	struct event_config* cfg = event_config_new();
//	event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);
//	m_clientBase = event_base_new_with_config(cfg);

	m_clientBase = event_base_new();

	if (m_clientBase == NULL)
	{
		if (m_pServerCtrl && m_pServerCtrl->lpErrorCB != NULL)
		{
			m_pServerCtrl->lpErrorCB(m_pServerCtrl->lpCallBackData, "", 0, "client create event_base failed");
			return Ret_failed;
		}
	}

	evthread_use_windows_threads();

	m_clientEventbuf = bufferevent_socket_new(m_clientBase, -1, BEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb(m_clientEventbuf, conn_read_cb, conn_write_cb, conn_event_cb, NULL);

	struct event* ev = event_new(m_clientBase, 1, 1, timer_cb, NULL);
	evtimer_set(ev, timer_cb, NULL);

	event_base_set(m_clientBase, ev);

// 	memset(&sin, 0, sizeof(sin));
// 	sin.sin_family = AF_INET;
// 	sin.sin_port = htons(dwPort);

// 	m_pListener = evconnlistener_new_bind(m_srvBase, listener_cb, m_srvBase,
// 		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1, (struct sockaddr*)&sin, sizeof(sin));

//	event_base_dispatch(m_clientBase);
// 	m_pClientListen->RegCallBack(callback);
// 
// 	m_pClient = HP_Create_TcpPackClient(m_pClientListen);	
// 	if (!m_pClient)
// 	{
// 		if (m_pClientCtrl && m_pClientCtrl->lpErrorCB != NULL)
// 		{
// 			m_pClientCtrl->lpErrorCB(m_pClientCtrl->lpCallBackData, NULL, 0, "create client object failed");
// 		}
// //		log4cpp::Category::getInstance("network").error("%s:%d] create client object failed", __FILE__, __LINE__);
// 		return 0;
// 	}	
// 
// 	m_pClient->SetMaxPackSize(0x3FFFFFF);

	return 1;
}

int CNetComm::GetStatus(int &bIsServer, int &bIsClient)
{
// 	if (m_pClient)
// 		bIsClient = TRUE;
// 	if (m_pServer)
// 		bIsServer = TRUE;
	return 1;
}

int CNetComm::ConnectTo(const char* pIP, unsigned short uPort, int bAutoReconnect/* = TRUE*/)
{
	struct sockaddr_in sin;
	sin.sin_addr.S_un.S_addr = inet_addr(pIP);
	sin.sin_port = htons(uPort);
	sin.sin_family = AF_INET;

	m_bAutoReconnect = bAutoReconnect;

	if (pIP == NULL || strlen(pIP) == 0)	return 0;
	strcpy_s(m_conIP, pIP);
	m_conPort = uPort;

	if (-1 == bufferevent_socket_connect(m_clientEventbuf, (struct sockaddr*)&sin, sizeof(sin)))
	{
		if (m_pClientCtrl && m_pClientCtrl->lpErrorCB != NULL)
		{
			m_pClientCtrl->lpErrorCB(m_pClientCtrl->lpCallBackData, pIP, uPort, "client connect to server failed");
		}
		return Ret_failed;
	}
	bufferevent_enable(m_clientEventbuf, EV_READ | EV_WRITE);

	return event_base_dispatch(m_clientBase);		
}

int CNetComm::Disconnect(const char* pIP, unsigned short uPort)
{
// 	if (m_pClient)
// 	{
// 		if (m_pClient->Stop() == 0)
// 		{
// 			const char* err = m_pClient->GetLastErrorDesc();
// 			if (m_pClientCtrl && m_pClientCtrl->lpErrorCB != NULL)
// 			{
// 				m_pClientCtrl->lpErrorCB(m_pClientCtrl->lpCallBackData, pIP, uPort, err);
// 			}
// 			return 0;
// 		}
// 		return 1;
// 	}
// 
// 	if (m_pServer)
// 	{
// 		DWORD nConnected = m_pServer->GetConnectionCount();
// 		if (nConnected == 0) return 1;
// 		CONNID* IDs = new CONNID[nConnected];
// 		CONNID destID;
// 		m_pServer->GetAllConnectionIDs(IDs, nConnected);
// 		
// 		for (int i = 0; i < nConnected; i++)
// 		{
// 			RemoteAddress remAddr;
// 			remAddr.iAddressLen = 50;
// 			m_pServer->GetRemoteAddress(IDs[i], remAddr.pAddress, remAddr.iAddressLen, remAddr.usPort);
// 			if (!strcmp(remAddr.pAddress, pIP) && remAddr.usPort == uPort)
// 			{
// 				destID = IDs[i];
// 				break;
// 			}
// 		}
// 
// 		delete[] IDs;
// 		return m_pServer->Disconnect(destID);
// 	}		
	return 0;
}

int CNetComm::SendMsg(void* pMsg, unsigned long dwMsgLen, const char* pIP, unsigned short uPort, unsigned long dwWay /*= SEND_ASYN*/){
//	int bSend = FALSE;
// 	if (m_pServer && m_pServer->HasStarted())
// 	{
// 		char tmpIP[IP_LEN] = { 0 };
// 		strcpy_s(tmpIP, pIP);
// 		CONNID id = m_pSrvListen->FindConnID(tmpIP, strlen(tmpIP), uPort);
// 		if (m_pServer->Send(id, (BYTE*)pMsg, dwMsgLen) == 0)
// 		{
// 			DWORD errCode = GetLastError();
// 			char tmp[512] = { 0 };
// 			sprintf_s(tmp, "server send msg error, error code = %d", errCode);
// 			if (m_pServerCtrl && m_pServerCtrl->lpErrorCB != NULL)
// 			{
// 				m_pServerCtrl->lpErrorCB(m_pServerCtrl->lpCallBackData, pIP, uPort, tmp);
// 			}
// 			return 0;
// 		}
// 	}
// 
// 	if (m_pClient && m_pClient->HasStarted())
// 	{
// 		if (m_pClient->Send((BYTE*)pMsg, dwMsgLen)==0)
// 		{
// 			DWORD errCode = GetLastError();
// 			char tmp[512] = { 0 };
// 			sprintf_s(tmp, "client send msg error, error code = %d", errCode);
// 			if (m_pClientCtrl && m_pClientCtrl->lpErrorCB != NULL)
// 			{
// 				m_pClientCtrl->lpErrorCB(m_pClientCtrl->lpCallBackData, pIP, uPort, tmp);
// 			}
// 			return 0;
// 		}
// 	}

	return 1;
}

int CNetComm::Uninitialize(){
	int isOK = 0;

	evconnlistener_free(m_pListener);
	event_base_free(m_srvBase);
	event_base_free(m_clientBase);

// 	if (m_pServer)
// 	{
// 		isOK = m_pServer->Stop();		
// 		if (isOK == 0)
// 		{
// 			DWORD errCode = GetLastError();
// 			char tmp[512] = { 0 };
// 			sprintf_s(tmp, "server stop error, error code = %d", errCode);
// 			if (m_pServerCtrl && m_pServerCtrl->lpErrorCB != NULL)
// 			{
// 				m_pServerCtrl->lpErrorCB(m_pServerCtrl->lpCallBackData, m_srvIP, m_srvPort, tmp);
// 			}
// 		}
// 	}
// 	if (m_pClient)
// 	{
// 		SetEvent(m_hExit);
// 		WaitForSingleObject(m_pthread, INFINITE);
// 		isOK = m_pClient->Stop();	
// 		if (isOK == 0)
// 		{
// 			DWORD errCode = GetLastError();
// 			char tmp[512] = { 0 };
// 			sprintf_s(tmp, "client stop error, error code = %d", errCode);
// 			if (m_pClientCtrl && m_pClientCtrl->lpErrorCB != NULL)
// 			{
// 				m_pClientCtrl->lpErrorCB(m_pClientCtrl->lpCallBackData, m_conIP, m_conPort, tmp);
// 			}
// 		}
// 	}
	return isOK;
}

int CNetComm::StartConnectThread(const char* IP, unsigned short port){

// 	ResetEvent(m_hExit);
// 	m_pthread = (HANDLE)_beginthreadex(nullptr, 0, ConnectThread, (LPVOID)this, 0, nullptr);
// 
// 	if (!m_pthread)
// 	{
// 		DWORD errCode = GetLastError();
// 		char tmp[512] = { 0 };
// 		sprintf_s(tmp, "client start reconnect thread error", errCode);
// 		if (m_pClientCtrl && m_pClientCtrl->lpErrorCB != NULL)
// 		{
// 			m_pClientCtrl->lpErrorCB(m_pClientCtrl->lpCallBackData, m_conIP, m_conPort, tmp);
// 		}
// 		return 0;
// 	}
//	CloseHandle(hDataThread);
	return 1;
}

unsigned int __stdcall CNetComm::ConnectThread(void* p)
{
	CNetComm* pNet = (CNetComm*)p;
// 	while (WaitForSingleObject(pNet->m_hExit, 50) != WAIT_OBJECT_0)
// 	{
// 		if (!pNet->m_pClient->HasStarted() && !pNet->m_pClient->Start(pNet->m_conIP, pNet->m_conPort, FALSE))
// 		{
// 			if (pNet->m_pClientCtrl != NULL && pNet->m_pClientCtrl->lpErrorCB)
// 			{
// 				pNet->m_pClientCtrl->lpErrorCB(pNet->m_pClientCtrl->lpCallBackData, pNet->m_conIP, pNet->m_conPort, pNet->m_pClient->GetLastErrorDesc());
// 			}
// 		}
// 		Sleep(5000);
// 	}
// 	ResetEvent(pNet->m_hExit);
// 	pNet->m_bConStart = FALSE;

	return 0;
}
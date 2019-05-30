#include "stdafx.h"

#include <iostream>
#include "Callback.h"
#include "event2/buffer.h"
#include "event2/event.h"
#include "CNetComm.h"

CServerCallback* CServerCallback::m_ServerCallback = NULL;
CServerCallback::CServerCallback()
{
	m_ServerCallback = this;
	m_pNetComm = NULL;
}

CServerCallback::~CServerCallback()
{

}

void CServerCallback::SetCallback(_USER_CB* pCallback, INetComm* pNetComm)
{
	m_pCallback = pCallback;
	m_pNetComm = pNetComm;
}

_USER_CB* CServerCallback::GetCBFunction()
{
	return m_pCallback;
}

bufferevent* CServerCallback::FindBufferevent(std::string IP, unsigned short port)
{
	std::string key = IP + ":" + std::to_string(port);
	if (m_mapClient.find(key) != m_mapClient.end())
	{
		return m_mapClient[key]->bufev;
	}
	return NULL;
}

static void Connection_close_cb(evutil_socket_t fd, short what, void *other_)
{
	if (what & EV_CLOSED)
	{
		std::cout << "connection closed!!!" << std::endl;
	}
}

void CServerCallback::ListenerCallback(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* user_data)
{
	char ClientIP[addr_size];
	
	sockaddr_in* addr = (sockaddr_in*)sock;
	evutil_inet_ntop(addr->sin_family, &addr->sin_addr, ClientIP, sizeof(ClientIP));
	int port = ntohs(addr->sin_port);
	printf("accept a client %d,IP:%s,PORT:%d\n", fd, ClientIP, port);

	event_base *base = (event_base*)user_data;

	//为这个客户端分配一个bufferevent  
	bufferevent *bev = bufferevent_socket_new(base, fd,	BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
	if (!bev)
	{
		printf("Create Client bufferevent failed.\n");
//		event_errx("");
		event_base_loopbreak(base);
		return;
	}

	bufferevent_setcb(bev, EventReadCallback, EventWriteCallback, EventCallback, NULL);
	bufferevent_enable(bev, EV_READ | EV_WRITE |EV_PERSIST);

	event closeEvent;
	event_assign(&closeEvent, base, fd, EV_CLOSED, Connection_close_cb, base);

	event_connection* evcon = new event_connection;
	evcon->fd = fd;
	evcon->bufev = bev;
	strcpy(evcon->address, ClientIP);
	evcon->port = port;
	std::string key = ClientIP + std::string(":") + std::to_string(port);
	m_ServerCallback->m_mapClient[key] = evcon;
}

void CServerCallback::EventReadCallback(bufferevent* bev, void* arg)
{
	evbuffer* evb = bufferevent_get_input(bev);
	size_t msglen = evbuffer_get_length(evb);
	char* msg = new char[msglen];
	memset(msg, 0, msglen);
	evbuffer_copyout(evb, msg, msglen);

//	event_connection* evcon = CONTAINING_RECORD(&bev, event_connection, bufev);
	evutil_socket_t fd = bufferevent_getfd(bev);
	sockaddr_in ClientAddr;
	int namelen = sizeof(ClientAddr);
	getpeername(fd, (sockaddr*)&ClientAddr, &namelen);
	char address[addr_size] = { 0 };
	int port = ntohs(ClientAddr.sin_port);
	evutil_inet_ntop(ClientAddr.sin_family, &ClientAddr.sin_addr, address, sizeof(address));

	if (m_ServerCallback->m_pCallback && m_ServerCallback->m_pCallback->lpRecvMsgCB)
	{
		m_ServerCallback->m_pCallback->lpRecvMsgCB(m_ServerCallback->m_pNetComm, msg, msglen, address, port);
	}

	delete[] msg;
}

void CServerCallback::EventWriteCallback(bufferevent* bev, void* arg)
{
	printf("%s: ", __FUNCTION__);
	evbuffer* output = bufferevent_get_output(bev);
	size_t buflen = evbuffer_get_length(output);
	char buf[512] = { 0 };
	evbuffer_copyout(output, buf, 512);
	printf("output buf: %s\n", buf);
}

void CServerCallback::EventCallback(bufferevent* bev, short events, void* arg)
{
	if (events & BEV_EVENT_CONNECTED)
	{
		std::cout << "client connected!!!" << std::endl;
	}
	else if (events & BEV_EVENT_EOF)
	{
		std::cout << "one connection closed!!!" << std::endl;
	}
	else if (events & BEV_EVENT_ERROR)
	{
		std::cout << "Got an error on the connection: " << strerror(errno) << std::endl;
	}
}

void CServerCallback::TimerCallback(evutil_socket_t, short, void *)
{
	printf("%s\n", __FUNCTION__);
}
#include "stdafx.h"

#include <iostream>
#include "Callback.h"
#include "event2/buffer.h"
#include "event2/event.h"

CServerCallback* CServerCallback::m_ServerCallback = NULL;
CServerCallback::CServerCallback()
{
	m_ServerCallback = this;
}

CServerCallback::~CServerCallback()
{

}

void CServerCallback::SetCallback(_USER_CB* pCallback)
{
	m_pCallback = pCallback;
}

bufferevent* CServerCallback::FindBufferevent(std::string IP, unsigned short port)
{
	std::string key = IP + ":" + std::to_string(port);
	if (m_mapClient.find(key) != m_mapClient.end())
	{
		return m_mapClient[key];
	}
	return NULL;
}

void CServerCallback::ListenerCallback(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* user_data)
{
	char ClientIP[256];
	int port;
	sockaddr_in* addr = (sockaddr_in*)sock;
	evutil_inet_ntop(addr->sin_family, &addr->sin_addr, ClientIP, sizeof(ClientIP));
	port = ntohs(addr->sin_port);
	printf("accept a client %d,IP:%s,PORT:%d\n", fd, ClientIP, port);

	event_base *base = (event_base*)user_data;

	//为这个客户端分配一个bufferevent  
	bufferevent *bev = bufferevent_socket_new(base, fd,	BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
	if (bev)
	{
		printf("Create Client bufferevent failed.\n");
		event_base_loopbreak(base);
		return;
	}

	bufferevent_setcb(bev, EventReadCallback, EventWriteCallback, EventCallback, NULL);
	bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);

	std::string key = ClientIP + std::string(":") + std::to_string(port);
	m_ServerCallback->m_mapClient[key] = bev;
}

void CServerCallback::EventReadCallback(bufferevent* bev, void* arg)
{
	evbuffer* evb = bufferevent_get_input(bev);
	size_t len = evbuffer_get_length(evb);
	char* msg = new char[len + 1];
	memset(msg, 0, len + 1);
	evbuffer_copyout(evb, msg, len);

	printf("recieve client %d msg: %s\n", bufferevent_getfd(bev),msg);
	delete[] msg;
}

void CServerCallback::EventWriteCallback(bufferevent* bev, void* arg)
{

}

void CServerCallback::EventCallback(bufferevent* bev, short events, void* arg)
{

}

void CServerCallback::TimerCallback(evutil_socket_t, short, void *)
{

}
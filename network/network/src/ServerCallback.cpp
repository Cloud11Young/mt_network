#include "stdafx.h"

#include "Callback.h"

CServerCallback::CServerCallback()
{

}

CServerCallback::~CServerCallback()
{

}

void CServerCallback::SetCallback(_USER_CB* pCallback)
{
	m_pCallback = pCallback;
}

void CServerCallback::ListenerCallback(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* user_data)
{

}

void CServerCallback::EventReadCallback(bufferevent* bev, void* arg)
{

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
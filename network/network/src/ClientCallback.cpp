#include "stdafx.h"

#include <iostream>
#include "Callback.h"
#include "event2/buffer.h"

bufferevent* CClientCallback::m_ClientEvent = NULL;
CClientCallback::CClientCallback()
{
	m_pNetComm = NULL;
}

CClientCallback::~CClientCallback()
{

}

void CClientCallback::SetCallback(ICallback* pCallback, INetComm* pNetComm)
{
	m_pCallback = pCallback;
	m_pNetComm = pNetComm;
}

ICallback* CClientCallback::GetCBFunction()
{
	return m_pCallback;
}


bufferevent* CClientCallback::GetBufferevent()
{
	return m_ClientEvent;
}

void CClientCallback::EventReadCallback(bufferevent* bev, void* arg)
{
	std::cout << __FUNCTION__ << ": ";
	evbuffer* evb = bufferevent_get_input(bev);
	size_t len = evbuffer_get_length(evb);
	char* data = new char[len + 1];
	memset(data, 0, len + 1);
	evbuffer_copyout(evb, data, len);
	printf("read msg %s", data);
	delete[] data;
}

void CClientCallback::EventWriteCallback(bufferevent* bev, void* arg)
{

}

void CClientCallback::EventCallback(bufferevent* bev, short events, void* arg)
{
	std::cout << __FUNCTION__ << ": ";
	if (events & BEV_EVENT_CONNECTED)
	{
		std::cout << "Connect server success" << std::endl;
		bufferevent_incref(bev);
		m_ClientEvent = bev;
	}
	else if (events & BEV_EVENT_ERROR)
	{
		std::cout << "Got an error on the connection: " << strerror(errno) << std::endl;
	}
	else if (events & BEV_EVENT_EOF)
	{
		std::cout << "Connection closed!!!" << std::endl;
	}
}
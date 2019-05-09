#ifndef NETWORK_CALLBACK_H_
#define NETWORK_CALLBACK_H_

#include "event2/bufferevent.h"

struct _USER_CB;

class CServerCallback
{
public:
	CServerCallback();
	~CServerCallback();

public:
	void SetCallback(_USER_CB* pCallback);
	static void ListenerCallback(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* user_data);
	static void EventReadCallback(bufferevent* bev, void* arg);
	static void EventWriteCallback(bufferevent* bev, void* arg);
	static void EventCallback(bufferevent* bev, short events, void* arg);
	static void TimerCallback(evutil_socket_t, short, void *);

private:
	_USER_CB* m_pCallback;
};

class CClientCallback
{
public:
	CClientCallback();
	~CClientCallback();

public:
	void SetCallback(_USER_CB* pCallback);
	static void EventReadCallback(bufferevent* bev, void* arg);
	static void EventWriteCallback(bufferevent* bev, void* arg);
	static void EventCallback(bufferevent* bev, short events, void* arg);

private:
	_USER_CB* m_pCallback;
};

#endif
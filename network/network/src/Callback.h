#ifndef NETWORK_CALLBACK_H_
#define NETWORK_CALLBACK_H_

#include "event2/bufferevent.h"
#include <map>
#include <string>

struct _USER_CB;
class INetComm;

class CServerCallback
{
public:
	CServerCallback();
	~CServerCallback();

	void SetCallback(_USER_CB* pCallback, INetComm* pNetComm);
	bufferevent* FindBufferevent(std::string IP, unsigned short port);
	static void ListenerCallback(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* user_data);
	static void EventReadCallback(bufferevent* bev, void* arg);
	static void EventWriteCallback(bufferevent* bev, void* arg);
	static void EventCallback(bufferevent* bev, short events, void* arg);
	static void TimerCallback(evutil_socket_t, short, void *);

private:
	_USER_CB* m_pCallback;
	INetComm* m_pNetComm;
	static CServerCallback* m_ServerCallback;
	std::map<std::string, bufferevent*> m_mapClient;
};

class CClientCallback
{
public:
	CClientCallback();
	~CClientCallback();

	bufferevent* GetBufferevent();
	void SetCallback(_USER_CB* pCallback, INetComm* pNetComm);
	static void EventReadCallback(bufferevent* bev, void* arg);
	static void EventWriteCallback(bufferevent* bev, void* arg);
	static void EventCallback(bufferevent* bev, short events, void* arg);

private:
	_USER_CB* m_pCallback;
	INetComm* m_pNetComm;
	static bufferevent* m_ClientEvent;
};

#endif
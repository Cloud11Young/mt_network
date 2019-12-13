#ifndef NETWORK_CALLBACK_H_
#define NETWORK_CALLBACK_H_

#include <map>
#include <string>
#include "event/event.h"
#include "event/evutil.h"

struct _USER_CB;
class INetComm;

#define addr_size 16
struct event_connection
{
	evutil_socket_t fd;
	bufferevent* bufev;

	char address[addr_size];
	ev_uint16_t port;
};

class CServerCallback
{
public:
	CServerCallback();
	~CServerCallback();

	void SetCallback(_USER_CB* pCallback, INetComm* pNetComm);
	_USER_CB* GetCBFunction();
	struct bufferevent* FindBufferevent(std::string IP, unsigned short port);
	static void ListenerCallback(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sock, int socklen, void* user_data);
	static void EventReadCallback(struct bufferevent* bev, void* arg);
	static void EventWriteCallback(struct bufferevent* bev, void* arg);
	static void EventCallback(struct bufferevent* bev, short events, void* arg);
	static void TimerCallback(evutil_socket_t, short, void *);

private:
	_USER_CB* m_pCallback;
	INetComm* m_pNetComm;
	static CServerCallback* m_ServerCallback;
	std::map<std::string, event_connection*> m_mapClient;
};

class CClientCallback
{
public:
	CClientCallback();
	~CClientCallback();

	bufferevent* GetBufferevent();
	void SetCallback(_USER_CB* pCallback, INetComm* pNetComm);
	_USER_CB* GetCBFunction();
	static void EventReadCallback(bufferevent* bev, void* arg);
	static void EventWriteCallback(bufferevent* bev, void* arg);
	static void EventCallback(bufferevent* bev, short events, void* arg);

private:
	_USER_CB* m_pCallback;
	INetComm* m_pNetComm;
	static bufferevent* m_ClientEvent;
};

#endif
#ifndef NETWORK_DEFINE_H_
#define NETWORK_DEFINE_H_

typedef void(*ConnectCallback)(void* pThis, const char* strIP, ushort dwPort, const char* strPcName);
typedef void(*DisconnectCallback)(void* pThis, const char* strIP, ushort dwPort);
typedef void(*ReceiveCallback)(void* pThis, void* pMsg, ulong dwMsgLen, const char* strIP, ushort dwPort);
typedef void(*LPPREAUTO_CONNECT_CALLBACK)(void* pThis, const char* strIP, ushort dwPort);
typedef void(*LPPOSTAUTO_CONNECT_CALLBACK)(void* pThis, const char* strIP, ushort dwPort, int bOK);
typedef void(*LPERROR_CALLBACK)(void* pThis, const char* strIP, ushort dwPort, const char* msg);
typedef void(*LogCallback)(int severity, const char* msg);

struct NetworkCallback
{
	ConnectCallback		_connectCB;
	DisconnectCallback	_disconnCB;
	ReceiveCallback		_receiveCB;
	LPPREAUTO_CONNECT_CALLBACK lpPreAutoConnectCB;
	LPPOSTAUTO_CONNECT_CALLBACK lpPostAutoConnectCB;
	LPERROR_CALLBACK        lpErrorCB;
	LogCallback			_logCB;
	void*				_callbackData;
};

#endif
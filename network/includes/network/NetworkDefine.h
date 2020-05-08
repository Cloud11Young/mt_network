#ifndef NETWORK_DEFINE_H_
#define NETWORK_DEFINE_H_

#include <stdint.h>


class ICallback
{
public:
	virtual ~ICallback() {}
	virtual void DoConnect(void* pThis, const char* strIP, uint16_t dwPort, const char* strPcName) = 0;
	virtual void DoDisconnect(void* pThis, const char* strIP, uint16_t dwPort) = 0;
	virtual void DoReceive(void* pThis, void* pMsg, size_t dwMsgLen, const char* strIP, uint16_t dwPort) = 0;
	virtual void LPPREAUTO_CONNECT_CALLBACK(void* pThis, const char* strIP, uint16_t dwPort) = 0;
	virtual void LPPOSTAUTO_CONNECT_CALLBACK(void* pThis, const char* strIP, uint16_t dwPort, int bOK) = 0;
//	virtual void LPERROR_CALLBACK(void* pThis, const char* strIP, uint16_t dwPort, const char* msg) = 0;
	virtual void DoLogger(int severity, const char* msg) = 0;
};


#endif
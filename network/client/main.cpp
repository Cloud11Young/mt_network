#include <stdio.h>
#include <windows.h>
#include "INetComm.h"
#include <process.h>

//#pragma comment(lib,"../lib/network.lib")

static void ConnectCB(void* pThis, const char* strIP, unsigned short dwPort, const char* strPcName);
static void DisconnectCB(void* pThis, const char* strIP, unsigned short dwPort);
static void RecvCB(void* pThis, void* pMsg, unsigned long dwMsgLen, const char* strIP, unsigned short dwPort);
//static void PREAUTO_CONNECT_CALLBACK(void* pThis, char* strIP, unsigned short dwPort);
//static void POSTAUTO_CONNECT_CALLBACK(void* pThis, char* strIP, unsigned short dwPort, int bOK);
static void ErrorCB(void* pThis, const char* strIP, unsigned short dwPort, const char* msg);

unsigned int __stdcall workthread(void* pVoid)
{
	INetComm* pNet = (INetComm*)pVoid;
	pNet->ConnectTo("192.168.101.11", 5001);
	char msg[256] = { 0 };
	char tmp[256] = { 0 };
//	printf("thread %d input send msg:", GetCurrentThreadId());
//	scanf("%s", &msg);

	while (true)
	{		
		strcpy_s(tmp, "start");
		strcat(tmp, msg);
		strcat(tmp, "end");
//		char* lmsg = new char[10 * 1024 * 1024];
//		memset(lmsg, 1, 10 * 1024 * 1024);

		pNet->SendMsg("hi, server!!!", 14, "192.168.101.13", 5001);
//		delete[] lmsg;
		Sleep(3000);
	}
}

class NetCallback : public ICallback
{
public:
	virtual void DoConnect(void* pThis, const char* strIP, uint16_t dwPort, const char* strPcName)
	{
		printf("client [%s:%d] connect to server success\n", strIP, dwPort);
	}

	virtual void DoDisconnect(void* pThis, const char* strIP, uint16_t dwPort)
	{
		printf("client [%s:%d] disconnected\n", strIP, dwPort);
	}

	virtual void DoReceive(void* pThis, void* pMsg, size_t dwMsgLen, const char* strIP, uint16_t dwPort)
	{
		printf("client [%s:%d] recv msg = \"%s\", length = %lld\n", strIP, dwPort, (char*)pMsg, dwMsgLen);
	}

	virtual void LPPREAUTO_CONNECT_CALLBACK(void* pThis, const char* strIP, uint16_t dwPort)
	{
	}

	virtual void LPPOSTAUTO_CONNECT_CALLBACK(void* pThis, const char* strIP, uint16_t dwPort, int bOK)
	{
	}

	virtual void DoLogger(int severity, const char* msg)
	{
		printf("severity = %d, msg = %s\n", severity, msg);
	}
};

int main(int argc, char** argv)
{
	INetComm* Nets[16] = { 0 };
	ICallback* Callbacks[16] = { 0 };

	for (int i = 0; i < 1; i++)
	{
		INetComm::CreateInstance(&Nets[i]);
		Callbacks[i] = new NetCallback();

		Nets[i]->InitClient(NULL, Callbacks[i]);
		_beginthreadex(NULL, 0, workthread, Nets[i], 0, NULL);
	}

	while (true)
	{
		Sleep(20);
	}

	return 0;
}

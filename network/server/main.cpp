#include <stdio.h>
#include <windows.h>
#include <process.h>
#include "INetComm.h"


INetComm* pNet = NULL;
static char ip[32] = { 0 };
static uint16_t port = 0;

unsigned int __stdcall SendThread(void* pvoid)
{
	char c[1];
	int len = 0;
	printf("input send msg :");
	scanf("%s", &c);

	len = GetCurrentThreadId();
	char* tmp = new char[5*1024*1024];
	memset(tmp, c[0], len);
	tmp[len] = '\0';
	while (true)
	{
		pNet->SendMsg(tmp, 5 * 1024 * 1024, ip, port);
		Sleep(500);
	}
}

class NetCallback : public ICallback
{
public:
	virtual void DoConnect(void* pThis, const char* strIP, uint16_t dwPort, const char* strPcName)
	{
		printf("client [%s:%d] connected\n", strIP, dwPort);
		port = dwPort;
		strcpy(ip, strIP);

		INetComm* pNet = (INetComm*)pThis;
		char srvMsg[] = "hi,client!!!\n";
		pNet->SendMsg(srvMsg, strlen(srvMsg) + 1, strIP, dwPort);
	}

	virtual void DoDisconnect(void* pThis, const char* strIP, uint16_t dwPort)
	{
		printf("client [%s:%d] disconnected\n", strIP, dwPort);
	}

	virtual void DoReceive(void* pThis, void* pMsg, uint64_t dwMsgLen, const char* strIP, uint16_t dwPort)
	{
		//	printf("client [%s:%d] recv msg = \"%s\", length = %d\n", strIP, dwPort,(char*)pMsg,dwMsgLen);
		char s[6] = { 0 };
		char e[4] = { 0 };
		memcpy(s, pMsg, 5);
		memcpy(e, (char*)pMsg + dwMsgLen - 4, 4);
		//	if (strcmp(s, "start") != 0 || strcmp(e, "end") != 0)
		//	{
		//		printf("recv wrong data, length = %d\n", dwMsgLen);
		//	}

		printf("recieve client IP:%s,PORT:%d, msg: %s\n", strIP, dwPort, (char*)pMsg);
		INetComm* pNet = (INetComm*)pThis;
		char srvMsg[] = "hi,client!!!\n";
		pNet->SendMsg(srvMsg, strlen(srvMsg) + 1, strIP, dwPort);		
	}

	virtual void LPPREAUTO_CONNECT_CALLBACK(void* pThis, const char* strIP, uint16_t dwPort)
	{
	}

	virtual void LPPOSTAUTO_CONNECT_CALLBACK(void* pThis, const char* strIP, uint16_t dwPort, int bOK)
	{
	}

//	virtual void LPERROR_CALLBACK(void* pThis, const char* strIP, uint16_t dwPort, const char* msg) = 0;

	virtual void DoLogger(int severity, const char* msg)
	{
		printf("severity = %d, msg = %s\n", severity, msg);
	}

};

int main(int argc, char** argv)
{	
	INetComm::CreateInstance(&pNet);
	if (pNet == NULL)
		return -1;

//	printf("please input server ip and port:");
//	scanf("%s%d", &tmp, &port);
//	sscanf(tmp, "%s%d", ip, &port);

	NetCallback* pCallback = new NetCallback();

	if (pNet->InitServer(NULL, pCallback, 5001, NULL) != 0)
	{
		printf("server start success\n");
	}
	
	while (true)
	{
		Sleep(20);
	}

	delete pCallback;

	return 0;
}
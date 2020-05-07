#include <stdio.h>
#include <windows.h>
#include <process.h>
#include "../network/src/INetComm.h"

static void ConnectCB(void* pThis, const char* strIP, unsigned short dwPort, const char* strPcName);
static void DisconnectCB(void* pThis, const char* strIP, unsigned short dwPort);
static void RecvCB(void* pThis, void* pMsg, unsigned long dwMsgLen, const char* strIP, unsigned short dwPort);
//static void PREAUTO_CONNECT_CALLBACK(void* pThis, char* strIP, unsigned short dwPort);
//static void POSTAUTO_CONNECT_CALLBACK(void* pThis, char* strIP, unsigned short dwPort, int bOK);
static void ErrorCB(void* pThis, const char* strIP, unsigned short dwPort, const char* msg);
static void LogCB(int severity, const char* msg);

INetComm* pNet = NULL;
static char ip[32] = { 0 };
static ushort port = 0;

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

int main(int argc, char** argv)
{	
	INetComm::CreateInstance(&pNet);
	if (pNet == NULL)
		return -1;

	NetworkCallback callback;
	callback._connectCB = ConnectCB;
	callback._disconnCB = DisconnectCB;
	callback._receiveCB = RecvCB;
	callback.lpErrorCB = ErrorCB;
	callback._callbackData = pNet;
	callback._logCB = LogCB;

	char tmp[64] = { 0 };
	char ip[32] = { 0 };
	unsigned short port = 5001;
//	printf("please input server ip and port:");
//	scanf("%s%d", &tmp,&port);
//	sscanf(tmp, "%s%d", ip, &port);

	if (pNet->InitServer(NULL, &callback, port, NULL) != 0)
	{
		printf("server start success\n");
	}
	
	while (true)
	{
		Sleep(20);
	}

	return 0;
}

void ConnectCB(void* pThis, const char* strIP, unsigned short dwPort, const char* strPcName)
{
	printf("client [%s:%d] connected\n", strIP, dwPort);
	port = dwPort;
	strcpy(ip, strIP);

	INetComm* pNet = (INetComm*)pThis;
	char srvMsg[] = "hi,client!!!\n";
	pNet->SendMsg(srvMsg, strlen(srvMsg) + 1, strIP, dwPort);
}

void DisconnectCB(void* pThis, const char* strIP, unsigned short dwPort)
{
	printf("client [%s:%d] disconnected\n", strIP, dwPort);

}

void RecvCB(void* pThis, void* pMsg, unsigned long dwMsgLen, const char* strIP, unsigned short dwPort)
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

void ErrorCB(void* pThis, const char* strIP, unsigned short dwPort, const char* msg)
{
	printf("client [%s:%d] error = \"%s\"\n", strIP, dwPort,msg);
}

void LogCB(int severity, const char* msg)
{
	printf("severity = %d, msg = %s\n", severity, msg);
}
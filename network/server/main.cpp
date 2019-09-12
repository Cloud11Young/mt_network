#include <stdio.h>
#include <windows.h>
#include <process.h>
#include "../network/src/INetComm.h"

#pragma comment(lib,"../lib/network.lib")

static void ConnectCB(void* pThis, const char* strIP, unsigned short dwPort, const char* strPcName);
static void DisconnectCB(void* pThis, const char* strIP, unsigned short dwPort);
static void RecvCB(void* pThis, void* pMsg, unsigned long dwMsgLen, const char* strIP, unsigned short dwPort);
//static void PREAUTO_CONNECT_CALLBACK(void* pThis, char* strIP, unsigned short dwPort);
//static void POSTAUTO_CONNECT_CALLBACK(void* pThis, char* strIP, unsigned short dwPort, int bOK);
static void ErrorCB(void* pThis, const char* strIP, unsigned short dwPort, const char* msg);

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

	USER_CB userCB;
	userCB.lpConnectCB = ConnectCB;
	userCB.lpDisconnectCB = DisconnectCB;
	userCB.lpRecvMsgCB = RecvCB;
	userCB.lpErrorCB = ErrorCB;
	userCB.lpCallBackData = pNet;

	char tmp[64] = { 0 };
	char ip[32] = { 0 };
	unsigned short port = 8800;
	printf("please input server ip and port:");
	scanf("%s%d", tmp, &port);
//	sscanf(tmp, "%s%d", ip, &port);

	if (pNet->Initialize(NULL, &userCB, port, NULL) != 0)
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

// 	for (int i = 0; i < 8; i++)
// 	{
// 		_beginthreadex(NULL, 0, SendThread, pNet, 0, NULL);
// 	}
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
	if (strcmp(s, "start") != 0 || strcmp(e, "end") != 0)
	{
		printf("recv wrong data, length = %d\n", dwMsgLen);
	}

	INetComm* pNet = (INetComm*)pThis;
	pNet->SendMsg(pMsg, dwMsgLen, strIP, dwPort);
}

void ErrorCB(void* pThis, const char* strIP, unsigned short dwPort, const char* msg)
{
	printf("client [%s:%d] error = \"%s\"\n", strIP, dwPort,msg);
}
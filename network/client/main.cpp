#include <stdio.h>
#include <windows.h>
#include "../network/src/INetComm.h"
#include <process.h>

#pragma comment(lib,"../lib/network.lib")

static void ConnectCB(void* pThis, const char* strIP, unsigned short dwPort, const char* strPcName);
static void DisconnectCB(void* pThis, const char* strIP, unsigned short dwPort);
static void RecvCB(void* pThis, void* pMsg, unsigned long dwMsgLen, const char* strIP, unsigned short dwPort);
//static void PREAUTO_CONNECT_CALLBACK(void* pThis, char* strIP, unsigned short dwPort);
//static void POSTAUTO_CONNECT_CALLBACK(void* pThis, char* strIP, unsigned short dwPort, int bOK);
static void ErrorCB(void* pThis, const char* strIP, unsigned short dwPort, const char* msg);

unsigned int __stdcall workthread(void* pVoid)
{
	INetComm* pNet = (INetComm*)pVoid;
	pNet->ConnectTo("127.0.0.1", 5001);
	char msg[256] = { 0 };
	char tmp[256] = { 0 };
	printf("thread %d input send msg:", GetCurrentThreadId());
	scanf("%s", &msg);

	while (true)
	{		
		strcpy_s(tmp, "start");
		strcat(tmp, msg);
		strcat(tmp, "end");
		char* lmsg = new char[10 * 1024 * 1024];
		memset(lmsg, 1, 10 * 1024 * 1024);

		pNet->SendMsg(lmsg, 10 * 1024 * 1024, "127.0.0.1", 8800);
		delete[] lmsg;
		Sleep(3000);
	}
}

int main(int argc, char** argv)
{
//	INetComm* pNet = NULL;
	
// 	if (pNet == NULL)
// 		return -1;

	USER_CB userCB;
	userCB.lpConnectCB = ConnectCB;
	userCB.lpDisconnectCB = DisconnectCB;
	userCB.lpRecvMsgCB = RecvCB;
	userCB.lpErrorCB = ErrorCB;

	INetComm* Nets[16] = { 0 };

	for (int i = 0; i < 1; i++)
	{
		INetComm::CreateInstance(&Nets[i]);
		Nets[i]->Initialize(NULL, &userCB);
		_beginthreadex(NULL, 0, workthread, Nets[i], 0, NULL);
	}

	while (true)
	{
//		pNet->SendMsg(msg, 11, "127.0.0.1", 8080);
		Sleep(20);
	}

	return 0;
}

void ConnectCB(void* pThis, const char* strIP, unsigned short dwPort, const char* strPcName)
{
	printf("client [%s:%d] connect to server success\n", strIP, dwPort);
}

void DisconnectCB(void* pThis, const char* strIP, unsigned short dwPort)
{
	printf("client [%s:%d] disconnected\n", strIP, dwPort);

}

void RecvCB(void* pThis, void* pMsg, unsigned long dwMsgLen, const char* strIP, unsigned short dwPort)
{
	printf("client [%s:%d] recv msg, length = %d\n", strIP, dwPort, dwMsgLen);

}

void ErrorCB(void* pThis, const char* strIP, unsigned short dwPort, const char* msg)
{
	printf("client [%s:%d] error = \"%s\"\n", strIP, dwPort, msg);
}
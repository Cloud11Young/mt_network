// EchoClient.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <atomic>
#include "mthelper.h"
#include "../network/src/INetComm.h"

static void ConnectCB(void* pThis, const char* strIP, unsigned short dwPort, const char* strPcName);
static void DisconnectCB(void* pThis, const char* strIP, unsigned short dwPort);
static void RecvCB(void* pThis, void* pMsg, unsigned long dwMsgLen, const char* strIP, unsigned short dwPort);
static void ErrorCB(void* pThis, const char* strIP, unsigned short dwPort, const char* msg);

const size_t maxCount = 10000000;
std::atomic<size_t> count = 0;

int main(int argc, char** argv)
{
	INetComm* pNet = NULL;
	INetComm::CreateInstance(&pNet);
	if (pNet == NULL)	return 0;

	{

		USER_CB userCB;
		userCB.lpConnectCB = ConnectCB;
		userCB.lpDisconnectCB = DisconnectCB;
		userCB.lpRecvMsgCB = RecvCB;
		userCB.lpErrorCB = ErrorCB;
		userCB.lpCallBackData = pNet;		

		pNet->Initialize(NULL, &userCB);
	}
	char ip[64] = { 0 };
	unsigned short port;
	std::cout << "please input connect IP and port: ";
	std::cin >> ip >> port;
	pNet->ConnectTo(ip, port);

	char quit;
	while (std::cin >> quit)
	{
		if (quit == 'q')
			break;
	}

	pNet->Uninitialize();
	std::cout << "Client quit !!!\n";
	system("pause");

	return 0;
}

void ConnectCB(void* pThis, const char* strIP, unsigned short dwPort, const char* strPcName)
{
	std::cout << "Connect to " << strIP << ":" << dwPort << "successed!!!" << std::endl;
}

void DisconnectCB(void* pThis, const char* strIP, unsigned short dwPort)
{
	std::cout << "Disconnect to server !!!\n";
}

void RecvCB(void* pThis, void* pMsg, unsigned long dwMsgLen, const char* strIP, unsigned short dwPort)
{
	INetComm* pNet = (INetComm*)pThis;

	if (count == maxCount)
	{
		char endMsg[] = "end";
		if (pNet->SendMsg(endMsg, strlen(endMsg) + 1, strIP, dwPort) > 0)
		{
			++count;
			std::cout << "Send messages end !!!\n";
			return;
		}
	}
	else if (count > maxCount)
		return;

	if (pNet->SendMsg(pMsg, dwMsgLen, strIP, dwPort) > 0)
		++count;
}

void ErrorCB(void* pThis, const char* strIP, unsigned short dwPort, const char* msg)
{
	std::cout << "there is a error msg: " << msg << std::endl;
}
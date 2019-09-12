// EchoServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <time.h>
#include <atomic>
#include "mthelper.h"
#include "../network/src/INetComm.h"
#include "log4cpp/Category.hh"
#include "log4cpp/RollingFileAppender.hh"
#include "log4cpp/PatternLayout.hh"
#include "log4cpp/OstreamAppender.hh"


static void ConnectCB(void* pThis, const char* strIP, unsigned short dwPort, const char* strPcName);
static void DisconnectCB(void* pThis, const char* strIP, unsigned short dwPort);
static void RecvCB(void* pThis, void* pMsg, unsigned long dwMsgLen, const char* strIP, unsigned short dwPort);
static void ErrorCB(void* pThis, const char* strIP, unsigned short dwPort, const char* msg);

static void SendMsg(INetComm* pNet, const char* ip, unsigned short dwPort);

struct msgbuffer
{
	size_t count;
	char buffer[65500];
};

//static char msgbuffer[65500] = { 0 };
std::atomic<size_t> msgCount = 0;
const size_t maxCount = 100000;

static void Initlog()
{
	char path[MAX_PATH];
	char cfgPath[MAX_PATH] = { 0 };
	char filePath[MAX_PATH] = { 0 };
	GetPathExeA(path, MAX_PATH);

	sprintf_s(filePath, "%s\\log", path);
	if (GetFileAttributesA(filePath) == INVALID_FILE_ATTRIBUTES)
	{
		CreateDirectoryA(filePath, NULL);
	}

	time_t t;
	time(&t);
	struct tm timeinfo;
	localtime_s(&timeinfo, &t);
	char stm[255] = { 0 };
	strftime(stm, sizeof(stm), "\\server_%y_%m_%d %H_%M_%S.txt", &timeinfo);
	sprintf_s(filePath, "%s\\log\\%s", path, stm);

	log4cpp::RollingFileAppender* RollAppender = new log4cpp::RollingFileAppender("RollFile", filePath);
	if (RollAppender == NULL)	return;

	RollAppender->setMaximumFileSize(20 * 1024 * 1024);
	RollAppender->setMaxBackupIndex(100);

	log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
	if (layout == NULL)	return;

	layout->setConversionPattern("[%d %p %t %m %n");
	RollAppender->setLayout(layout);

	log4cpp::OstreamAppender* StreamAppender = new log4cpp::OstreamAppender("console", &std::cout);

	log4cpp::Category& root = log4cpp::Category::getRoot();
//	root.addAppender(RollAppender);
	root.addAppender(StreamAppender);
	root.setRootPriority(log4cpp::Priority::INFO);


}


int main(int argc, char** argv)
{
	Initlog();
	INetComm* pNet = NULL;
	INetComm::CreateInstance(&pNet);
	if (pNet == NULL)	return -1;

	USER_CB userCB;
	userCB.lpConnectCB = ConnectCB;
	userCB.lpDisconnectCB = DisconnectCB;
	userCB.lpRecvMsgCB = RecvCB;
	userCB.lpErrorCB = ErrorCB;
	userCB.lpCallBackData = pNet;

	int port = 9999;
	std::cout << "please input server bind port: ";
	std::cin >> port;

	if (pNet->Initialize(NULL, &userCB, port, NULL) != 0)
		std::cout << "server start success\n";
	else
	{
		std::cout << "server start failed\n";
		goto end;
	}

	char quit;
	while (std::cin >> quit)
	{
		if (quit == 'q')
			break;
	}

	pNet->Release();
	std::cout << "server quit\n";
end:
	system("pause");

	return 0;
}

void ConnectCB(void* pThis, const char* strIP, unsigned short dwPort, const char* strPcName)
{
	std::cout << strIP << ":" << dwPort << "connected !!!\n";
	std::cout << "Server start send msg\n";
	SendMsg((INetComm*)pThis, strIP, dwPort);
}

void DisconnectCB(void* pThis, const char* strIP, unsigned short dwPort)
{
	std::cout << strIP << ":" << dwPort << " disconnected !!!\n";
}

void RecvCB(void* pThis, void* pMsg, unsigned long dwMsgLen, const char* strIP, unsigned short dwPort)
{
	msgbuffer* buf = (msgbuffer*)pMsg;
	clock_t startT;
	startT = (clock_t)atoi(buf->buffer);
	clock_t gapT = clock() - startT;
	char logbuf[256] = { 0 };
	sprintf_s(logbuf, "Come from %s:%d, bytes = %zd, time = %ldms", strIP, dwPort, sizeof(msgbuffer), gapT);
	log4cpp::Category::getRoot().info(logbuf);

	SendMsg((INetComm*)pThis, strIP, dwPort);
}

void ErrorCB(void* pThis, const char* strIP, unsigned short dwPort, const char* msg)
{
	std::cout << strIP << ":" << dwPort <<" "<< msg << std::endl;
}

void SendMsg(INetComm* pNet,const char* ip, unsigned short dwPort)
{
	if (msgCount + 1 > maxCount)
	{
		std::cout << "please input q to quit.\n";
		return;
	}
	
	clock_t t = clock();

	msgbuffer buf;
	buf.count = msgCount;
	memset(buf.buffer, 0, 65500);
	strcpy_s(buf.buffer, std::to_string(t).c_str());
	if (!pNet->SendMsg(&buf, sizeof(buf), ip, dwPort))
	{
		char logbuffer[256] = { 0 };
		sprintf_s(logbuffer, "Server send msg to %s:%d falied.", ip, dwPort);
		log4cpp::Category::getRoot().error(logbuffer);
	}
	msgCount++;
}
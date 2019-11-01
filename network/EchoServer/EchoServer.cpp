// EchoServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <time.h>
#include <atomic>
#include <mutex>
#include <vector>
#include <algorithm>
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

static void SendMsg(INetComm* pNet, const char* ip, unsigned short dwPort, size_t count = 1);

struct msgbuffer
{
	size_t count;
	char buffer[65500];
};

//std::atomic<clock_t> maxTime = 0;
typedef std::vector<clock_t> VecMaxTime;
typedef std::pair<std::string, VecMaxTime> ConnectMaxTime;
std::map<std::string, ConnectMaxTime> mapConnectMaxTime;
std::mutex vecLock;

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

	RollAppender->setMaximumFileSize(50 * 1024 * 1024);
	RollAppender->setMaxBackupIndex(1000);

	log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
	if (layout == NULL)	return;

	layout->setConversionPattern("[%d %p %t %m %n");
	RollAppender->setLayout(layout);

	log4cpp::OstreamAppender* StreamAppender = new log4cpp::OstreamAppender("console", &std::cout);

	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.addAppender(RollAppender);
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

	if (pNet->Initialize(NULL, &userCB, port, "0.0.0.0") != 0)
		log4cpp::Category::getRoot().info("server start success");
	else
	{
		log4cpp::Category::getRoot().error("server start failed");
		goto end;
	}

	char quit;
	while (std::cin >> quit)
	{
		if (quit == 'q')
			break;
	}

	pNet->Release();
	log4cpp::Category::getRoot().info("server quit");
end:
	system("pause");

	return 0;
}

void ConnectCB(void* pThis, const char* strIP, unsigned short dwPort, const char* strPcName)
{
	char path[MAX_PATH];
	char filePath[MAX_PATH] = { 0 };
	GetPathExeA(path, MAX_PATH);
	
	std::string logKey = strIP + std::string("_") + std::to_string(dwPort);
	if (mapConnectMaxTime.find(logKey) == mapConnectMaxTime.end())
	{
		size_t logSize = mapConnectMaxTime.size();
		std::string value = "connect_" + std::to_string(++logSize);
		mapConnectMaxTime.insert(std::make_pair(logKey, std::make_pair(value,VecMaxTime())));
		mapConnectMaxTime[logKey].second.reserve(50);
	}
	sprintf_s(filePath, "%s\\log\\%s.txt", path, mapConnectMaxTime[logKey].first.c_str());

	log4cpp::RollingFileAppender* RollAppender = new log4cpp::RollingFileAppender("RollFile" + mapConnectMaxTime[logKey].first, filePath);
	if (RollAppender == NULL)	return;

	RollAppender->setMaximumFileSize(50 * 1024 * 1024);
	RollAppender->setMaxBackupIndex(100);

	log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
	if (layout == NULL)	return;

	layout->setConversionPattern("[%d %p %t %m %n");
	RollAppender->setLayout(layout);

	log4cpp::Category& cate = log4cpp::Category::getInstance(mapConnectMaxTime[logKey].first);
	cate.addAppender(RollAppender);
	cate.setRootPriority(log4cpp::Priority::INFO);

	log4cpp::Category::getInstance(mapConnectMaxTime[logKey].first).info("%s:%d connected !!!", strIP, dwPort);
	log4cpp::Category::getInstance(mapConnectMaxTime[logKey].first).info("Server start send msg");
	SendMsg((INetComm*)pThis, strIP, dwPort);
}

void DisconnectCB(void* pThis, const char* strIP, unsigned short dwPort)
{
	std::string logKey = strIP + std::string("_") + std::to_string(dwPort);
	log4cpp::Category::getInstance(mapConnectMaxTime[logKey].first).info("%s:%d disconnected !!!", strIP, dwPort);
//	log4cpp::Category::getInstance(mapConnectMaxTime[logKey].first).shutdown();
}

void RecvCB(void* pThis, void* pMsg, unsigned long dwMsgLen, const char* strIP, unsigned short dwPort)
{
	std::string logKey = strIP + std::string("_") + std::to_string(dwPort);
	VecMaxTime& vMaxTime = mapConnectMaxTime[logKey].second;
	if (!strcmp("end", (const char*)pMsg))
	{
		log4cpp::Category::getInstance(mapConnectMaxTime[logKey].first).error("%s:%d msg end !!!", strIP, dwPort);

		std::string strTimes;
		for (size_t i = 0; i < vMaxTime.size(); i++)
		{
			strTimes += std::to_string(vMaxTime[i]);
		}

		log4cpp::Category::getInstance(mapConnectMaxTime[logKey].first).notice("%s:%d max elapsed time：%s", strIP, dwPort,strTimes.c_str());

		return;
	}

	msgbuffer* buf = (msgbuffer*)pMsg;
	clock_t startT;
	startT = (clock_t)atoi(buf->buffer);
	clock_t gapT = clock() - startT;
	
	if (vMaxTime.size() > 0 && gapT > vMaxTime[0])
	{
		if (vMaxTime.size() >= 50)
		{
			vMaxTime[0] = gapT;
		}
		else
			vMaxTime.emplace_back(gapT);
		std::sort(vMaxTime.begin(), vMaxTime.end());
	}
	else if (vMaxTime.size() == 0)
	{
		vMaxTime.emplace_back(gapT);
	}
		

	log4cpp::Category::getInstance(mapConnectMaxTime[logKey].first).info
	(
		"Come from %s:%d count = %zd, bytes = %zd, time = %ldms", strIP, dwPort, buf->count, sizeof(msgbuffer), gapT
	);

	SendMsg((INetComm*)pThis, strIP, dwPort, ++buf->count);
}

void ErrorCB(void* pThis, const char* strIP, unsigned short dwPort, const char* errmsg)
{
	std::string logKey = strIP + std::string("_") + std::to_string(dwPort);
	log4cpp::Category::getInstance(mapConnectMaxTime[logKey].first).error("%s:%d %s !!!", strIP, dwPort, errmsg);
}

void SendMsg(INetComm* pNet,const char* ip, unsigned short dwPort, size_t count)
{
	clock_t t = clock();

	msgbuffer buf;
	buf.count = count;
	memset(buf.buffer, 0, 65500);
	strcpy_s(buf.buffer, std::to_string(t).c_str());
	if (!pNet->SendMsg(&buf, sizeof(buf), ip, dwPort))
	{
		std::string logKey = ip + std::string("_") + std::to_string(dwPort);
		log4cpp::Category::getInstance(mapConnectMaxTime[logKey].first).error("Server send msg to %s:%d falied.", ip, dwPort);
	}
}
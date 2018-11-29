#include "stdafx.h"
#include "CNetComm.h"
#include "BufferPool.h"
#include <process.h>
// #include "mtHelper.h"
// #include "log4cpp/Category.hh"
// #include "log4cpp/RollingFileAppender.hh"
// #include "log4cpp/PatternLayout.hh"

// static int IsDirExist(const char* path){
// 	DWORD dwAttri = GetFileAttributesA(path);
// 	return INVALID_FILE_ATTRIBUTES != dwAttri && 0 != (dwAttri&FILE_ATTRIBUTE_DIRECTORY);
// }

// static void Initlog()
// {
// 	char path[MAX_PATH];
// 	char cfgPath[MAX_PATH] = { 0 };
// 	char filePath[MAX_PATH] = { 0 };
// 	GetPathExeA(path, MAX_PATH);
// 
// 	//	strcat(path, "\\log");
// 	sprintf_s(filePath, "%s\\log", path);
// 
// 	if (!IsDirExist(filePath))
// 	{
// 		CreateDirectoryA(filePath, NULL);
// 	}
// 
// 	time_t t;
// 	time(&t);
// 	struct tm* timeinfo;
// 	timeinfo = localtime(&t);
// 	char stm[255] = { 0 };
// 	strftime(stm, sizeof(stm), "\\net_%y_%m_%d %H_%M_%S.txt", timeinfo);
// 	sprintf_s(filePath, "%s\\log\\%s", path, stm);
// 	sprintf_s(cfgPath, "%s\\config\\log4cpp.property", path);
// 
// 	log4cpp::RollingFileAppender* RollAppender = new log4cpp::RollingFileAppender("default", filePath);
// 	if (RollAppender == NULL)	return;
// 
// 	RollAppender->setMaximumFileSize(100 * 1024 * 1024);
// 	RollAppender->setMaxBackupIndex(10);
// 
// 	log4cpp::PatternLayout* layout = new log4cpp::PatternLayout();
// 	if (layout == NULL)	return;
// 
// 	layout->setConversionPattern("[%d %p %t %m %n");
// 	RollAppender->setLayout(layout);
// 
// 	log4cpp::Category& root = log4cpp::Category::getRoot();
// 	root.addAppender(RollAppender);
// 
// 	log4cpp::Category& netlog = root.getInstance("network");
// 	root.setRootPriority(log4cpp::Priority::ERROR);
// 
// 	netlog.setPriority(log4cpp::Priority::INFO);
// //	mvslog.setPriority((log4cpp::Priority::Value)IAoiConfig()->GetLogLv());
// 
// }

CNetComm::CNetComm() :m_pServer(NULL), m_pClient(NULL), m_pthread(NULL)
{
	m_pSrvListen = new ServerListener;
	m_pClientListen = new ClientListener;
	m_bConStart = FALSE;
	memset(m_conIP, 0, sizeof(*m_conIP)*IP_LEN);

	m_hExit = CreateEvent(NULL, TRUE, FALSE, NULL);
//	Initlog();
}

CNetComm::~CNetComm()
{
	Uninitialize();
	if (m_pServer)
		delete m_pServer;
	if (m_pClient)
		delete m_pClient;
	if (m_pSrvListen)
		delete m_pSrvListen;
	if (m_pClientListen)
		delete m_pClientListen;	
}

void CNetComm::Release(){

}

//需要提供Server服务
int CNetComm::Initialize(void* pThis, PUSER_CB callback, unsigned short dwPort, char* strIp)
{
	m_pServerCtrl = callback;
	m_pSrvListen->RegCallBack(callback);

	m_pServer = HP_Create_TcpPackServer(m_pSrvListen);
	if (!m_pServer)
	{
//		log4cpp::Category::getInstance("network").error("%s:%d] create server object failed", __FILE__, __LINE__);
		return 0;
	}	

	if (!m_pServer->Start(strIp, dwPort))
	{
		const char* err = m_pServer->GetLastErrorDesc();
		m_pServer->GetLastError();
//		log4cpp::Category::getInstance("network").error("%s:%d] server start error = \"%s\", ip = %s, port = %d",
//			__FILE__, __LINE__, err, strIp, dwPort);
		return 0;
	}
		
	if (!m_pServer->HasStarted())
		return 0;

	return 1;
}

//不需要提供Server服务
int CNetComm::Initialize(void* pThis, PUSER_CB callback)
{
	m_pClientCtrl = callback;
	m_pClientListen->RegCallBack(callback);

	m_pClient = HP_Create_TcpPackClient(m_pClientListen);	
	if (!m_pClient)
	{
//		log4cpp::Category::getInstance("network").error("%s:%d] create client object failed", __FILE__, __LINE__);
		return 0;
	}	

	return 1;
}

int CNetComm::GetStatus(int &bIsServer, int &bIsClient)
{
	if (m_pClient)
		bIsClient = TRUE;
	if (m_pServer)
		bIsServer = TRUE;
	return TRUE;
}

int CNetComm::ConnectTo(char* pIP, unsigned short uPort, int bAutoReconnect/* = TRUE*/)
{
	m_bAutoReconnect = bAutoReconnect;
//	m_conIP = pIP;
	if (pIP == NULL || strlen(pIP) == 0)	return 0;
	strcpy_s(m_conIP, pIP);
	m_conPort = uPort;

	if (m_bAutoReconnect)
	{
		if (!m_pClient->HasStarted() && !m_bConStart)
			m_bConStart = StartConnectThread(pIP, uPort);
	}
	else if (!m_pClient || !m_pClient->Start(pIP, uPort, FALSE))
	{
		if (!m_pClient)
		{
			const char* err = m_pClient->GetLastErrorDesc();
			m_pClient->GetLastError();
//			log4cpp::Category::getInstance("network").error("%s:%d] client start error = \"%s\", ip = %s, port = %d",
//				__FILE__, __LINE__, err, pIP, uPort);
		}
		return 0;
	}		
	
	return 1;	
}

int CNetComm::Disconnect(char* pIP, unsigned short uPort)
{
	if (m_pClient)
		return m_pClient->Stop();

	if (m_pServer)
	{
		DWORD nConnected = m_pServer->GetConnectionCount();
		CONNID* IDs = new CONNID[nConnected];
		CONNID destID;
		m_pServer->GetAllConnectionIDs(IDs, nConnected);
		
		for (int i = 0; i < nConnected; i++)
		{
			RemoteAddress* pAddr = m_pSrvListen->FindRemoteAddr(IDs[i]);
			if (pAddr->pAddress == pIP && pAddr->usPort == uPort)
			{
				destID = pAddr->dwConnID;
				m_pSrvListen->DeleteRemoteAddr(pAddr->dwConnID);
				break;
			}
		}

		delete[] IDs;
		return m_pServer->Disconnect(destID);
	}		
	return FALSE;
}

int CNetComm::SendMsg(void* pMsg, unsigned long dwMsgLen, char* pIP, unsigned short uPort, unsigned long dwWay /*= SEND_ASYN*/){
	int bSend = FALSE;
	if (m_pServer && m_pServer->HasStarted()){
		CONNID id = m_pSrvListen->FindConnID(pIP, strlen(pIP), uPort);		
		bSend = m_pServer->Send(id, (BYTE*)pMsg, dwMsgLen);
	}

	if (m_pClient && m_pClient->HasStarted()){
		bSend = m_pClient->Send((BYTE*)pMsg, dwMsgLen);
	}

	return bSend;
}

int CNetComm::Uninitialize(){
	int isOK = FALSE;
	if (m_pServer)
	{
		isOK = m_pServer->Stop();		
	}
	if (m_pClient)
	{
		SetEvent(m_hExit);
		WaitForSingleObject(m_pthread, INFINITE);
		isOK = m_pClient->Stop();		
	}
	return isOK;
}

int CNetComm::StartConnectThread(const char* IP, unsigned short port){
	int isOK = TRUE;

	ResetEvent(m_hExit);
	m_pthread = (HANDLE)_beginthreadex(nullptr, 0, ConnectThread, (LPVOID)this, 0, nullptr);

	if (!m_pthread)
	{
		isOK = FALSE;
	}
//	CloseHandle(hDataThread);
	return isOK;
}

UINT WINAPI CNetComm::ConnectThread(LPVOID p)
{
	CNetComm* pNet = (CNetComm*)p;
	while (WaitForSingleObject(pNet->m_hExit, 50) != WAIT_OBJECT_0)
	{
		if (!pNet->m_pClient->HasStarted() && !pNet->m_pClient->Start(pNet->m_conIP, pNet->m_conPort, FALSE))
		{
			if (pNet->m_pClientCtrl != NULL && pNet->m_pClientCtrl->lpErrorCB)
			{
				pNet->m_pClientCtrl->lpErrorCB(pNet->m_pClientCtrl->lpCallBackData, pNet->m_conIP, pNet->m_conPort, pNet->m_pClient->GetLastErrorDesc());
			}
		}
		Sleep(100);
	}
	ResetEvent(pNet->m_hExit);
	pNet->m_bConStart = FALSE;

	return 0;
}
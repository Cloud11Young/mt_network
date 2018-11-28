#include "stdafx.h"
#include "CNetComm.h"
#include "BufferPool.h"
#include <process.h>

// #ifdef _DEBUG
// #pragma comment(lib,"../lib/HPSocket_D.lib")
// #else
// #pragma comment(lib,"../lib/HPSocket.lib")
// #endif

CNetComm::CNetComm():m_pServer(NULL),m_pClient(NULL){
	m_pSrvListen = new ServerListener;
	m_pClientListen = new ClientListener;
	m_bConStart = FALSE;
	memset(m_conIP, 0, sizeof(*m_conIP)*IP_LEN);
}

CNetComm::~CNetComm(){
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

BOOL CNetComm::Initialize(PVOID pThis, PUSER_CB callback, USHORT dwPort, char* strIp){//需要提供Server服务
	m_pSrvListen->RegCallBack(callback);

	m_pServer = HP_Create_TcpPackServer(m_pSrvListen);
	if (!m_pServer)
		return FALSE;

	m_pServerCtrl = callback;

	char cIP[64] = {0};
	/*memcpy(cIP, strIp.GetBuffer(), 64);*/
	strcpy_s(cIP, strIp);
	if (!m_pServer->Start(cIP, dwPort))
	{
		return FALSE;
// 		if (!m_pSrvListen->StartDataThread()){
// 			return FALSE;
// 		}
	}
		
	if (!m_pServer->HasStarted())
		return FALSE;
	return TRUE;
}

BOOL CNetComm::Initialize(PVOID pThis, PUSER_CB callback){//不需要提供Server服务
	m_pClient = HP_Create_TcpPackClient(m_pClientListen);
	m_pClientListen->RegCallBack(callback);
	if (!m_pClient)
		return FALSE;

	m_pClientCtrl = callback;
	return TRUE;
}

BOOL CNetComm::GetStatus(BOOL &bIsServer, BOOL &bIsClient){
	if (m_pClient)
		bIsClient = TRUE;
	if (m_pServer)
		bIsServer = TRUE;
	return TRUE;
}

BOOL CNetComm::ConnectTo(char* pIP, USHORT uPort, BOOL bAutoReconnect/* = TRUE*/){
	m_bAutoReconnect = bAutoReconnect;
//	m_conIP = pIP;
	if (pIP == NULL || strlen(pIP) == 0)	return FALSE;
	strcpy_s(m_conIP, pIP);
	m_conPort = uPort;

	if (m_bAutoReconnect){
		if (!m_pClient->HasStarted() && !m_bConStart)
			m_bConStart = StartConnectThread(pIP, uPort);
	}
	else if (!m_pClient || !m_pClient->Start(pIP, uPort, FALSE))
		return FALSE;
	
// 	if (!m_pClientListen->StartDataThread())
// 		return FALSE;	
	return TRUE;	
}

int CNetComm::Disconnect(char* pIP, USHORT uPort){
	if (m_pClient)
		return m_pClient->Stop();
	if (m_pServer){
		DWORD nConnected = m_pServer->GetConnectionCount();
		CONNID* IDs = new CONNID[nConnected];
		CONNID destID;
		m_pServer->GetAllConnectionIDs(IDs, nConnected);
		
		for (int i = 0; i < nConnected; i++){
			RemoteAddress* pAddr = m_pSrvListen->FindRemoteAddr(IDs[i]);
			if (pAddr->pAddress == pIP && pAddr->usPort == uPort){
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

int CNetComm::SendMsg(LPVOID pMsg, DWORD dwMsgLen, char* pIP, USHORT uPort, DWORD dwWay /*= SEND_ASYN*/){
// 	PacketHead head;
// 	head.DataSize = dwMsgLen;
// 	head.DataType = 1;
// 	BYTE* pTmp = new BYTE[sizeof(head) + dwMsgLen];
// 	memset(pTmp, 0, sizeof(head)+dwMsgLen);
//  	memcpy(pTmp, &head, sizeof(head));
// 	memcpy(pTmp + sizeof(head), pMsg, dwMsgLen);
	
	BOOL bSend = FALSE;
	if (m_pServer && m_pServer->HasStarted()){
		CONNID id = m_pSrvListen->FindConnID(pIP, strlen(pIP), uPort);		
		bSend = m_pServer->Send(id, (BYTE*)pMsg, dwMsgLen);
	}

	if (m_pClient && m_pClient->HasStarted()){
		bSend = m_pClient->Send((BYTE*)pMsg, dwMsgLen);
	}

//	delete[] pTmp;

	return bSend;
}

int CNetComm::GetSocket(char* pIP, USHORT uPort, list<HANDLE> SocketList){

	return FALSE;
}

int CNetComm::Uninitialize(){
	int isOK = FALSE;
	if (m_pServer){
		isOK = m_pServer->Stop();		
	}
	if (m_pClient){
		isOK = m_pClient->Stop();		
	}
	return isOK;
}

int CNetComm::StartConnectThread(const char* IP, USHORT port){
	BOOL isOK = TRUE;

	HANDLE hDataThread = (HANDLE)_beginthreadex(nullptr, 0, ConnectThread, (LPVOID)this, 0, nullptr);

	if (!hDataThread)
	{
		//SetLastError(SE_WORKER_THREAD_CREATE, __FUNCTION__, ::GetLastError());
		isOK = FALSE;
	}
	CloseHandle(hDataThread);
	return isOK;
}

UINT WINAPI CNetComm::ConnectThread(LPVOID p){
	CNetComm* pNet = (CNetComm*)p;
	while (TRUE){
		if (!pNet->m_pClient->HasStarted() && !pNet->m_pClient->Start(pNet->m_conIP, pNet->m_conPort, FALSE))
		{
			if (pNet->m_pClientCtrl != NULL && pNet->m_pClientCtrl->lpErrorCB)
			{
				pNet->m_pClientCtrl->lpErrorCB(pNet->m_pClientCtrl->lpCallBackData, pNet->m_conIP, pNet->m_conPort, pNet->m_pClient->GetLastErrorDesc());
			}
		}
		Sleep(100);
	}
	pNet->m_bConStart = FALSE;
	return 0;
}
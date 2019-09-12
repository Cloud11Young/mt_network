#include "stdafx.h"
#include "CNetComm.h"
#include "BufferPool.h"
#include <process.h>

CNetComm::CNetComm() :m_pServer(NULL), m_pClient(NULL), m_pthread(NULL)
{
	m_pSrvListen = new ServerListener;
	m_pClientListen = new ClientListener;
	m_bConStart = FALSE;
	memset(m_conIP, 0, sizeof(*m_conIP)*IP_LEN);
	memset(m_srvIP, 0, sizeof(*m_srvIP)*IP_LEN);

	m_hExit = CreateEvent(NULL, TRUE, FALSE, NULL);
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

void CNetComm::Release()
{

}

//需要提供Server服务
int CNetComm::Initialize(void* pThis, PUSER_CB callback, ushort dwPort, const char* strIp)
{
	if (m_pServer != NULL)		return 0;
	m_pServerCtrl = callback;
	m_pSrvListen->RegCallBack(callback);

	m_pServer = HP_Create_TcpPackServer(m_pSrvListen);
	if (!m_pServer)
	{
		if (m_pServerCtrl && m_pServerCtrl->lpErrorCB != NULL)
		{
			m_pServerCtrl->lpErrorCB(m_pServerCtrl->lpCallBackData, strIp, dwPort, "server Create object failed");
		}
		return 0;
	}	
	m_pServer->SetMaxPackSize(0x7FFFFFF);

	if (!m_pServer->Start(strIp, dwPort))
	{
		const char* err = m_pServer->GetLastErrorDesc();
//		m_pServer->GetLastError();
		if (m_pServerCtrl && m_pServerCtrl->lpErrorCB != NULL)
		{
			m_pServerCtrl->lpErrorCB(m_pServerCtrl->lpCallBackData, strIp, dwPort, err);
		}
		return 0;
	}

	int len = IP_LEN;
	m_pServer->GetListenAddress(m_srvIP, len, m_srvPort);
		
	if (!m_pServer->HasStarted())
		return 0;

	return 1;
}

//不需要提供Server服务
int CNetComm::Initialize(void* pThis, PUSER_CB callback)
{
	if (m_pClient != NULL)	return FALSE;
	m_pClientCtrl = callback;
	m_pClientListen->RegCallBack(callback);

	m_pClient = HP_Create_TcpPackClient(m_pClientListen);	
	if (!m_pClient)
	{
		if (m_pClientCtrl && m_pClientCtrl->lpErrorCB != NULL)
		{
			m_pClientCtrl->lpErrorCB(m_pClientCtrl->lpCallBackData, NULL, 0, "create client object failed");
		}
		return 0;
	}	

	m_pClient->SetMaxPackSize(0x7FFFFFF);

	return 1;
}

int CNetComm::GetStatus(int &bIsServer, int &bIsClient)
{
	if (m_pClient)
		bIsClient = 1;
	if (m_pServer)
		bIsServer = 1;
	return 1;
}

int CNetComm::ConnectTo(const char* pIP, unsigned short uPort, int bAutoReconnect/* = TRUE*/)
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
	else if (m_pClient)
	{
		if (!m_pClient->HasStarted())
		{
			if (!m_pClient->Start(pIP, uPort, FALSE))
			{
				const char* err = m_pClient->GetLastErrorDesc();
				m_pClient->GetLastError();

				if (m_pClientCtrl && m_pClientCtrl->lpErrorCB != NULL)
				{
					m_pClientCtrl->lpErrorCB(m_pClientCtrl->lpCallBackData, pIP, uPort, err);
				}
				return 0;
			}
		}
	}		
	
	return 1;	
}

int CNetComm::Disconnect(const char* pIP, unsigned short uPort)
{
	if (m_pClient)
	{
		if (m_pClient->Stop() == 0)
		{
			const char* err = m_pClient->GetLastErrorDesc();
			if (m_pClientCtrl && m_pClientCtrl->lpErrorCB != NULL)
			{
				m_pClientCtrl->lpErrorCB(m_pClientCtrl->lpCallBackData, pIP, uPort, err);
			}
			return 0;
		}
		return 1;
	}

	if (m_pServer)
	{
		DWORD nConnected = m_pServer->GetConnectionCount();
		if (nConnected == 0) return 1;
		CONNID* IDs = new CONNID[nConnected];
		CONNID destID;
		m_pServer->GetAllConnectionIDs(IDs, nConnected);
		
		for (DWORD i = 0; i < nConnected; i++)
		{
			RemoteAddress remAddr;
			remAddr.iAddressLen = 50;
			m_pServer->GetRemoteAddress(IDs[i], remAddr.pAddress, remAddr.iAddressLen, remAddr.usPort);
			if (!strcmp(remAddr.pAddress, pIP) && remAddr.usPort == uPort)
			{
				destID = IDs[i];
				break;
			}
		}

		delete[] IDs;
		return m_pServer->Disconnect(destID);
	}		
	return 0;
}

int CNetComm::SendMsg(void* pMsg, unsigned long dwMsgLen, const char* pIP, unsigned short uPort, unsigned long dwWay /*= SEND_ASYN*/){
//	int bSend = FALSE;
	if (m_pServer && m_pServer->HasStarted())
	{
		char tmpIP[IP_LEN] = { 0 };
		strcpy_s(tmpIP, pIP);
		CONNID id = m_pSrvListen->FindConnID(tmpIP, strlen(tmpIP), uPort);
		if (m_pServer->Send(id, (BYTE*)pMsg, dwMsgLen) == 0)
		{
			DWORD errCode = GetLastError();
			char tmp[512] = { 0 };
			sprintf_s(tmp, "server send msg error, error code = %d", errCode);
			if (m_pServerCtrl && m_pServerCtrl->lpErrorCB != NULL)
			{
				m_pServerCtrl->lpErrorCB(m_pServerCtrl->lpCallBackData, pIP, uPort, tmp);
			}
			return 0;
		}
	}

	if (m_pClient && m_pClient->HasStarted())
	{
		if (m_pClient->Send((BYTE*)pMsg, dwMsgLen)==0)
		{
			DWORD errCode = GetLastError();
			char tmp[512] = { 0 };
			sprintf_s(tmp, "client send msg error, error code = %d", errCode);
			if (m_pClientCtrl && m_pClientCtrl->lpErrorCB != NULL)
			{
				m_pClientCtrl->lpErrorCB(m_pClientCtrl->lpCallBackData, pIP, uPort, tmp);
			}
			return 0;
		}
	}

	return 1;
}

int CNetComm::Uninitialize(){
	int isOK = 0;
	if (m_pServer)
	{
		isOK = m_pServer->Stop();		
		if (isOK == 0)
		{
			DWORD errCode = GetLastError();
			char tmp[512] = { 0 };
			sprintf_s(tmp, "server stop error, error code = %d", errCode);
			if (m_pServerCtrl && m_pServerCtrl->lpErrorCB != NULL)
			{
				m_pServerCtrl->lpErrorCB(m_pServerCtrl->lpCallBackData, m_srvIP, m_srvPort, tmp);
			}
		}
		HP_Destroy_TcpPackServer(m_pServer);
		m_pServer = NULL;
	}
	if (m_pClient)
	{
		SetEvent(m_hExit);
		WaitForSingleObject(m_pthread, INFINITE);
		isOK = m_pClient->Stop();	
		if (isOK == 0)
		{
			DWORD errCode = GetLastError();
			char tmp[512] = { 0 };
			sprintf_s(tmp, "client stop error, error code = %d", errCode);
			if (m_pClientCtrl && m_pClientCtrl->lpErrorCB != NULL)
			{
				m_pClientCtrl->lpErrorCB(m_pClientCtrl->lpCallBackData, m_conIP, m_conPort, tmp);
			}
		}
		HP_Destroy_TcpPackClient(m_pClient);
		m_pClient = NULL;
	}
	return isOK;
}

int CNetComm::StartConnectThread(const char* IP, unsigned short port){

	ResetEvent(m_hExit);
	m_pthread = (HANDLE)_beginthreadex(nullptr, 0, ConnectThread, (LPVOID)this, 0, nullptr);

	if (!m_pthread)
	{
		DWORD errCode = GetLastError();
		char tmp[512] = { 0 };
		sprintf_s(tmp, "client start reconnect thread error %d", errCode);
		if (m_pClientCtrl && m_pClientCtrl->lpErrorCB != NULL)
		{
			m_pClientCtrl->lpErrorCB(m_pClientCtrl->lpCallBackData, m_conIP, m_conPort, tmp);
		}
		return 0;
	}
	return 1;
}

UINT WINAPI CNetComm::ConnectThread(LPVOID p)
{
	CNetComm* pNet = (CNetComm*)p;
	while (WaitForSingleObject(pNet->m_hExit, 50) != WAIT_OBJECT_0)
	{
		if (pNet->m_pClient && !pNet->m_pClient->HasStarted())
		{
			pNet->m_pClient->Stop();
			if (!pNet->m_pClient->Start(pNet->m_conIP, pNet->m_conPort, FALSE))
			{
				if (pNet->m_pClientCtrl != NULL && pNet->m_pClientCtrl->lpErrorCB)
				{
					pNet->m_pClientCtrl->lpErrorCB(pNet->m_pClientCtrl->lpCallBackData, pNet->m_conIP, pNet->m_conPort, pNet->m_pClient->GetLastErrorDesc());
				}
			}
		}
		Sleep(5000);
	}
	ResetEvent(pNet->m_hExit);
	pNet->m_bConStart = FALSE;

	return 0;
}
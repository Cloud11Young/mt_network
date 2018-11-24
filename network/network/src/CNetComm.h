#ifndef CNETCOMM_H_
#define CNETCOMM_H_

#include <afxstr.h>
#include "INetComm.h"

#include "../public/HPSocket.h"
#include "../public/HPTypeDef.h"
#include "../public/SocketInterface.h"
#include "../public/CriticalSection.h"
#include "Listener.h"
#include "DataType.h"


class CNetComm :public INetComm{
public:
	CNetComm();
	virtual ~CNetComm();
	virtual void Release();

	virtual BOOL Initialize(PVOID pThis, PUSER_CB callback, DWORD dwPort, wchar_t* strIp);//需要提供Server服务
	virtual BOOL Initialize(PVOID pThis, PUSER_CB callback);//不需要提供Server服务
	virtual BOOL GetStatus(BOOL &bIsServer, BOOL &bIsClient);
	virtual BOOL ConnectTo(wchar_t* pIP, DWORD uPort, BOOL bAutoReconnect = TRUE);
	virtual BOOL Disconnect(wchar_t* pIP, DWORD uPort);

	virtual BOOL SendMsg(LPVOID pMsg, DWORD dwMsgLen, wchar_t* pIP, DWORD uPort, DWORD dwWay = SEND_ASYN);
	virtual BOOL GetSocket(wchar_t* pIP, DWORD uPort, list<HANDLE> SocketList);

	virtual BOOL Uninitialize();
private:
	BOOL StartConnectThread(CString IP,DWORD port);
	static UINT WINAPI ConnectThread(LPVOID p);
private:
	ITcpServer* m_pServer;
	ITcpClient* m_pClient;
	PUSER_CB    m_pServerCtrl;
	PUSER_CB    m_pClientCtrl;
	BOOL		m_bAutoReconnect;
	ServerListener* m_pSrvListen;
	ClientListener* m_pClientListen;
	CMTX m_extractlock;
	CString m_conIP;
	DWORD   m_conPort;
	BOOL    m_bConStart;
};

#endif
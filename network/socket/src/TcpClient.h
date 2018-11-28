/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Version	: 5.0.2
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 75375912, 44636872
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#pragma once

#include "SocketHelper.h"
#include "../Common/Src/Event.h"
#include "../Common/Src/bufferptr.h"
#include "../Common/Src/bufferpool.h"
#include "../Common/Src/CriticalSection.h"

class CTcpClient : public ITcpClient
{
public:
	virtual int Start	(const char* lpszRemoteAddress, USHORT usPort, int bAsyncConnect = TRUE, const char* lpszBindAddress = nullptr);
	virtual int Stop	();
	virtual int Send	(const BYTE* pBuffer, int iLength, int iOffset = 0);
	virtual int SendSmallFile	(const char* lpszFileName, const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr);
	virtual int SendPackets	(const WSABUF pBuffers[], int iCount)	{return DoSendPackets(pBuffers, iCount);}
	virtual int			HasStarted			()	{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState	GetState			()	{return m_enState;}
	virtual CONNID			GetConnectionID		()	{return m_dwConnID;};
	virtual EnSocketError	GetLastError		()	{return m_enLastError;}
	virtual const char*		GetLastErrorDesc	()	{return ::GetSocketErrorDesc(m_enLastError);}

	virtual int GetLocalAddress		(char lpszAddress[], int& iAddressLen, USHORT& usPort);
	virtual int GetRemoteHost			(char lpszHost[], int& iHostLen, USHORT& usPort);
	virtual int GetPendingDataLength	(int& iPending) {iPending = m_iPending; return HasStarted();}

public:
	virtual int IsSecure				() {return FALSE;}

	virtual void SetSocketBufferSize	(DWORD dwSocketBufferSize)		{m_dwSocketBufferSize	= dwSocketBufferSize;}
	virtual void SetKeepAliveTime		(DWORD dwKeepAliveTime)			{m_dwKeepAliveTime		= dwKeepAliveTime;}
	virtual void SetKeepAliveInterval	(DWORD dwKeepAliveInterval)		{m_dwKeepAliveInterval	= dwKeepAliveInterval;}
	virtual void SetFreeBufferPoolSize	(DWORD dwFreeBufferPoolSize)	{m_dwFreeBufferPoolSize = dwFreeBufferPoolSize;}
	virtual void SetFreeBufferPoolHold	(DWORD dwFreeBufferPoolHold)	{m_dwFreeBufferPoolHold = dwFreeBufferPoolHold;}
	virtual void SetExtra				(void* pExtra)					{m_pExtra				= pExtra;}						

	virtual DWORD GetSocketBufferSize	()	{return m_dwSocketBufferSize;}
	virtual DWORD GetKeepAliveTime		()	{return m_dwKeepAliveTime;}
	virtual DWORD GetKeepAliveInterval	()	{return m_dwKeepAliveInterval;}
	virtual DWORD GetFreeBufferPoolSize	()	{return m_dwFreeBufferPoolSize;}
	virtual DWORD GetFreeBufferPoolHold	()	{return m_dwFreeBufferPoolHold;}
	virtual void* GetExtra				()	{return m_pExtra;}

protected:
	virtual EnHandleResult FirePrepareConnect(SOCKET socket)
		{return DoFirePrepareConnect(this, socket);}
	virtual EnHandleResult FireConnect()
		{
			EnHandleResult rs		= DoFireConnect(this);
			if(rs != HR_ERROR) rs	= FireHandShake();
			return rs;
		}
	virtual EnHandleResult FireHandShake()
		{return DoFireHandShake(this);}
	virtual EnHandleResult FireSend(const BYTE* pData, int iLength)
		{return DoFireSend(this, pData, iLength);}
	virtual EnHandleResult FireReceive(const BYTE* pData, int iLength)
		{return DoFireReceive(this, pData, iLength);}
	virtual EnHandleResult FireReceive(int iLength)
		{return DoFireReceive(this, iLength);}
	virtual EnHandleResult FireClose(EnSocketOperation enOperation, int iErrorCode)
		{return DoFireClose(this, enOperation, iErrorCode);}

	virtual EnHandleResult DoFirePrepareConnect(ITcpClient* pSender, SOCKET socket)
		{return m_pListener->OnPrepareConnect(pSender, pSender->GetConnectionID(), socket);}
	virtual EnHandleResult DoFireConnect(ITcpClient* pSender)
		{return m_pListener->OnConnect(pSender, pSender->GetConnectionID());}
	virtual EnHandleResult DoFireHandShake(ITcpClient* pSender)
		{return m_pListener->OnHandShake(pSender, pSender->GetConnectionID());}
	virtual EnHandleResult DoFireSend(ITcpClient* pSender, const BYTE* pData, int iLength)
		{return m_pListener->OnSend(pSender, pSender->GetConnectionID(), pData, iLength);}
	virtual EnHandleResult DoFireReceive(ITcpClient* pSender, const BYTE* pData, int iLength)
		{return m_pListener->OnReceive(pSender, pSender->GetConnectionID(), pData, iLength);}
	virtual EnHandleResult DoFireReceive(ITcpClient* pSender, int iLength)
		{return m_pListener->OnReceive(pSender, pSender->GetConnectionID(), iLength);}
	virtual EnHandleResult DoFireClose(ITcpClient* pSender, EnSocketOperation enOperation, int iErrorCode)
		{return m_pListener->OnClose(pSender, pSender->GetConnectionID(), enOperation, iErrorCode);}

	void SetLastError(EnSocketError code, LPCSTR func, int ec);
	virtual int CheckParams();
	virtual void PrepareStart();
	virtual void Reset();

	virtual void OnWorkerThreadEnd(DWORD dwThreadID) {}

	int DoSendPackets(const WSABUF pBuffers[], int iCount);

	static int DoSendPackets(CTcpClient* pClient, const WSABUF pBuffers[], int iCount)
		{return pClient->DoSendPackets(pBuffers, iCount);}

protected:
	void SetReserved	(void* pReserved)	{m_pReserved = pReserved;}						
	void* GetReserved	()					{return m_pReserved;}
	int GetRemoteHost	(char** lpszHost, USHORT* pusPort = nullptr);

private:
	void SetRemoteHost	(const char* lpszHost, USHORT usPort);

	int CheckStarting();
	int CheckStoping(DWORD dwCurrentThreadID);
	int CreateClientSocket(const char* lpszRemoteAddress, HP_SOCKADDR& addrRemote, USHORT usPort, const char* lpszBindAddress, HP_SOCKADDR& addrBind);
	int BindClientSocket(const HP_SOCKADDR& addrBind);
	int ConnectToServer(const HP_SOCKADDR& addrRemote);
	int CreateWorkerThread();
	int ProcessNetworkEvent();
	int ReadData();
	int SendData();
	int DoSendData(TItem* pItem);
	TItem* GetSendBuffer();
	int SendInternal(const WSABUF pBuffers[], int iCount);
	void WaitForWorkerThreadEnd(DWORD dwCurrentThreadID);

	int HandleError	(WSANETWORKEVENTS& events);
	int HandleRead		(WSANETWORKEVENTS& events);
	int HandleWrite	(WSANETWORKEVENTS& events);
	int HandleConnect	(WSANETWORKEVENTS& events);
	int HandleClose	(WSANETWORKEVENTS& events);

	static UINT WINAPI WorkerThreadProc(LPVOID pv);

public:
	CTcpClient(ITcpClientListener* pListener)
	: m_pListener			(pListener)
	, m_lsSend				(m_itPool)
	, m_soClient			(INVALID_SOCKET)
	, m_evSocket			(nullptr)
	, m_dwConnID			(0)
	, m_usPort				(0)
	, m_hWorker				(nullptr)
	, m_dwWorkerID			(0)
	, m_bAsyncConnect		(FALSE)
	, m_iPending			(0)
	, m_enState				(SS_STOPPED)
	, m_enLastError			(SE_OK)
	, m_pExtra				(nullptr)
	, m_pReserved			(nullptr)
	, m_dwSocketBufferSize	(DEFAULT_TCP_SOCKET_BUFFER_SIZE)
	, m_dwFreeBufferPoolSize(DEFAULT_CLIENT_FREE_BUFFER_POOL_SIZE)
	, m_dwFreeBufferPoolHold(DEFAULT_CLIENT_FREE_BUFFER_POOL_HOLD)
	, m_dwKeepAliveTime		(DEFALUT_TCP_KEEPALIVE_TIME)
	, m_dwKeepAliveInterval	(DEFALUT_TCP_KEEPALIVE_INTERVAL)
	{
		ASSERT(sm_wsSocket.IsValid());
		ASSERT(m_pListener);
	}

	virtual ~CTcpClient()
	{
		Stop();
	}

private:
	static const CInitSocket sm_wsSocket;

private:
	ITcpClientListener*	m_pListener;
	TClientCloseContext m_ccContext;

	int				m_bAsyncConnect;
	SOCKET				m_soClient;
	HANDLE				m_evSocket;
	CONNID				m_dwConnID;


	DWORD				m_dwSocketBufferSize;
	DWORD				m_dwFreeBufferPoolSize;
	DWORD				m_dwFreeBufferPoolHold;
	DWORD				m_dwKeepAliveTime;
	DWORD				m_dwKeepAliveInterval;

	HANDLE				m_hWorker;
	UINT				m_dwWorkerID;

	volatile EnServiceState	m_enState;
	EnSocketError		m_enLastError;

	void*				m_pExtra;
	void*				m_pReserved;

	CBufferPtr			m_rcBuffer;

protected:
	std::string			m_strHost;
	USHORT				m_usPort;

	CItemPool			m_itPool;

private:
	CSpinGuard			m_csState;

	CCriSec				m_csSend;
	TItemList			m_lsSend;

	CEvt				m_evBuffer;
	CEvt				m_evWorker;

	volatile int		m_iPending;
};

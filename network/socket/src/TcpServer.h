#pragma once

#include "SocketHelper.h"
#include "../Common/Src/Event.h"
#include "../Common/Src/STLHelper.h"
#include "../Common/Src/RingBuffer.h"
#include "../Common/Src/PrivateHeap.h"

class CTcpServer : public ITcpServer
{
public:
	virtual int Start	(const char* lpszBindAddress, unsigned short usPort);
	virtual int Stop	();
	virtual int Send	(CONNID dwConnID, const BYTE* pBuffer, int iLength, int iOffset = 0);
	virtual int SendSmallFile(CONNID dwConnID, const char* lpszFileName, const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr);
	virtual int SendPackets	(CONNID dwConnID, const WSABUF pBuffers[], int iCount)	{return DoSendPackets(dwConnID, pBuffers, iCount);}
	virtual int			HasStarted					()	{return m_enState == SS_STARTED || m_enState == SS_STARTING;}
	virtual EnServiceState	GetState					()	{return m_enState;}
	virtual int			Disconnect					(CONNID dwConnID, int bForce = TRUE);
	virtual int			DisconnectLongConnections	(unsigned long dwPeriod, int bForce = TRUE);
	virtual int			DisconnectSilenceConnections(unsigned long dwPeriod, int bForce = TRUE);
	virtual int			GetListenAddress			(char lpszAddress[], int& iAddressLen, USHORT& usPort);
	virtual int			GetLocalAddress				(CONNID dwConnID, char lpszAddress[], int& iAddressLen, USHORT& usPort);
	virtual int			GetRemoteAddress			(CONNID dwConnID, char lpszAddress[], int& iAddressLen, USHORT& usPort);
	
	virtual int GetPendingDataLength	(CONNID dwConnID, int& iPending);
	virtual unsigned long GetConnectionCount	();
	virtual int GetAllConnectionIDs	(CONNID pIDs[], unsigned long& dwCount);
	virtual int GetConnectPeriod		(CONNID dwConnID, unsigned long& dwPeriod);
	virtual int GetSilencePeriod		(CONNID dwConnID, unsigned long& dwPeriod);
	virtual EnSocketError GetLastError	()	{return m_enLastError;}
	virtual const char*	GetLastErrorDesc	()	{return ::GetSocketErrorDesc(m_enLastError);}

#ifdef _SSL_SUPPORT
	virtual int SetupSSLContext	(int iVerifyMode = SSL_VM_NONE, const char* lpszPemCertFile = nullptr, const char* lpszPemKeyFile = nullptr, const char* lpszKeyPasswod = nullptr, const char* lpszCAPemCertFileOrPath = nullptr, Fn_SNI_ServerNameCallback fnServerNameCallback = nullptr)	{return FALSE;}
	virtual int AddSSLContext		(int iVerifyMode = SSL_VM_NONE, const char* lpszPemCertFile = nullptr, const char* lpszPemKeyFile = nullptr, const char* lpszKeyPasswod = nullptr, const char* lpszCAPemCertFileOrPath = nullptr)															{return FALSE;}
	virtual void CleanupSSLContext	()																																																											{}
#endif

public:
	virtual int IsSecure				() {return FALSE;}

	virtual int SetConnectionExtra(CONNID dwConnID, void* pExtra);
	virtual int GetConnectionExtra(CONNID dwConnID, void** ppExtra);

	virtual void SetSendPolicy				(EnSendPolicy enSendPolicy)		{m_enSendPolicy				= enSendPolicy;}
	virtual void SetMaxConnectionCount		(unsigned long dwMaxConnectionCount)	{m_dwMaxConnectionCount		= dwMaxConnectionCount;}
	virtual void SetWorkerThreadCount		(unsigned long dwWorkerThreadCount)		{m_dwWorkerThreadCount		= dwWorkerThreadCount;}
	virtual void SetSocketListenQueue		(unsigned long dwSocketListenQueue)		{m_dwSocketListenQueue		= dwSocketListenQueue;}
	virtual void SetAcceptSocketCount		(unsigned long dwAcceptSocketCount)		{m_dwAcceptSocketCount		= dwAcceptSocketCount;}
	virtual void SetSocketBufferSize		(unsigned long dwSocketBufferSize)		{m_dwSocketBufferSize		= dwSocketBufferSize;}
	virtual void SetFreeSocketObjLockTime	(unsigned long dwFreeSocketObjLockTime)	{m_dwFreeSocketObjLockTime	= dwFreeSocketObjLockTime;}
	virtual void SetFreeSocketObjPool		(unsigned long dwFreeSocketObjPool)		{m_dwFreeSocketObjPool		= dwFreeSocketObjPool;}
	virtual void SetFreeBufferObjPool		(unsigned long dwFreeBufferObjPool)		{m_dwFreeBufferObjPool		= dwFreeBufferObjPool;}
	virtual void SetFreeSocketObjHold		(unsigned long dwFreeSocketObjHold)		{m_dwFreeSocketObjHold		= dwFreeSocketObjHold;}
	virtual void SetFreeBufferObjHold		(unsigned long dwFreeBufferObjHold)		{m_dwFreeBufferObjHold		= dwFreeBufferObjHold;}
	virtual void SetKeepAliveTime			(unsigned long dwKeepAliveTime)			{m_dwKeepAliveTime			= dwKeepAliveTime;}
	virtual void SetKeepAliveInterval		(unsigned long dwKeepAliveInterval)		{m_dwKeepAliveInterval		= dwKeepAliveInterval;}
	virtual void SetMarkSilence				(int bMarkSilence)				{m_bMarkSilence				= bMarkSilence;}

	virtual EnSendPolicy GetSendPolicy		()	{return m_enSendPolicy;}
	virtual unsigned long GetMaxConnectionCount		()	{return m_dwMaxConnectionCount;}
	virtual unsigned long GetWorkerThreadCount		()	{return m_dwWorkerThreadCount;}
	virtual unsigned long GetSocketListenQueue		()	{return m_dwSocketListenQueue;}
	virtual unsigned long GetAcceptSocketCount		()	{return m_dwAcceptSocketCount;}
	virtual unsigned long GetSocketBufferSize		()	{return m_dwSocketBufferSize;}
	virtual unsigned long GetFreeSocketObjLockTime	()	{return m_dwFreeSocketObjLockTime;}
	virtual unsigned long GetFreeSocketObjPool		()	{return m_dwFreeSocketObjPool;}
	virtual unsigned long GetFreeBufferObjPool		()	{return m_dwFreeBufferObjPool;}
	virtual unsigned long GetFreeSocketObjHold		()	{return m_dwFreeSocketObjHold;}
	virtual unsigned long GetFreeBufferObjHold		()	{return m_dwFreeBufferObjHold;}
	virtual unsigned long GetKeepAliveTime			()	{return m_dwKeepAliveTime;}
	virtual unsigned long GetKeepAliveInterval		()	{return m_dwKeepAliveInterval;}
	virtual int  IsMarkSilence				()	{return m_bMarkSilence;}

protected:
	virtual EnHandleResult FirePrepareListen(SOCKET soListen)
		{return DoFirePrepareListen(soListen);}
	virtual EnHandleResult FireAccept(TSocketObj* pSocketObj)
		{
			EnHandleResult rs		= DoFireAccept(pSocketObj);
			if(rs != HR_ERROR) rs	= FireHandShake(pSocketObj);
			if(rs != HR_ERROR) pSocketObj->ResetSndBuffSize(pSocketObj->socket);
			return rs;
		}
	virtual EnHandleResult FireHandShake(TSocketObj* pSocketObj)
		{return DoFireHandShake(pSocketObj);}
	virtual EnHandleResult FireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return DoFireReceive(pSocketObj, pData, iLength);}
	virtual EnHandleResult FireReceive(TSocketObj* pSocketObj, int iLength)
		{return DoFireReceive(pSocketObj, iLength);}
	virtual EnHandleResult FireSend(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return DoFireSend(pSocketObj, pData, iLength);}
	virtual EnHandleResult FireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
		{return DoFireClose(pSocketObj, enOperation, iErrorCode);}
	virtual EnHandleResult FireShutdown()
		{return DoFireShutdown();}

	virtual EnHandleResult DoFirePrepareListen(SOCKET soListen)
		{return m_pListener->OnPrepareListen(this, soListen);}
	virtual EnHandleResult DoFireAccept(TSocketObj* pSocketObj)
		{return m_pListener->OnAccept(this, pSocketObj->connID, pSocketObj->socket);}
	virtual EnHandleResult DoFireHandShake(TSocketObj* pSocketObj)
		{return m_pListener->OnHandShake(this, pSocketObj->connID);}
	virtual EnHandleResult DoFireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return m_pListener->OnReceive(this, pSocketObj->connID, pData, iLength);}
	virtual EnHandleResult DoFireReceive(TSocketObj* pSocketObj, int iLength)
		{return m_pListener->OnReceive(this, pSocketObj->connID, iLength);}
	virtual EnHandleResult DoFireSend(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return m_pListener->OnSend(this, pSocketObj->connID, pData, iLength);}
	virtual EnHandleResult DoFireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
		{return m_pListener->OnClose(this, pSocketObj->connID, enOperation, iErrorCode);}
	virtual EnHandleResult DoFireShutdown()
		{return m_pListener->OnShutdown(this);}

	void SetLastError(EnSocketError code, LPCSTR func, int ec);
	virtual int CheckParams();
	virtual void PrepareStart();
	virtual void Reset();

	virtual void OnWorkerThreadEnd(unsigned long dwThreadID) {}

	int DoSendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount);
	int DoSendPackets(TSocketObj* pSocketObj, const WSABUF pBuffers[], int iCount);
	TSocketObj* FindSocketObj(CONNID dwConnID);

private:
	EnHandleResult TriggerFireAccept(TSocketObj* pSocketObj);
	EnHandleResult TriggerFireReceive(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
	EnHandleResult TriggerFireSend(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
	EnHandleResult TriggerFireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode);

protected:
	int SetConnectionExtra(TSocketObj* pSocketObj, void* pExtra);
	int GetConnectionExtra(TSocketObj* pSocketObj, void** ppExtra);
	int SetConnectionReserved(CONNID dwConnID, void* pReserved);
	int GetConnectionReserved(CONNID dwConnID, void** ppReserved);
	int SetConnectionReserved(TSocketObj* pSocketObj, void* pReserved);
	int GetConnectionReserved(TSocketObj* pSocketObj, void** ppReserved);
	int SetConnectionReserved2(CONNID dwConnID, void* pReserved2);
	int GetConnectionReserved2(CONNID dwConnID, void** ppReserved2);
	int SetConnectionReserved2(TSocketObj* pSocketObj, void* pReserved2);
	int GetConnectionReserved2(TSocketObj* pSocketObj, void** ppReserved2);

private:
	int CheckStarting();
	int CheckStoping();
	int CreateListenSocket(const char* lpszBindAddress, unsigned short usPort);
	int CreateCompletePort();
	int CreateWorkerThreads();
	int StartAccept();

//	int StartDataThread();

	void CloseListenSocket();
	void WaitForAcceptSocketClose();
	void DisconnectClientSocket();
	void WaitForClientSocketClose();
	void ReleaseClientSocket();
	void ReleaseFreeSocket();
	void ReleaseFreeBuffer();
	void WaitForWorkerThreadEnd();
	void CloseCompletePort();

	TBufferObj*	GetFreeBufferObj(int iLen = -1);
	TSocketObj*	GetFreeSocketObj(CONNID dwConnID, SOCKET soClient);
	void		AddFreeBufferObj(TBufferObj* pBufferObj);
	void		AddFreeSocketObj(TSocketObj* pSocketObj, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0);
	TSocketObj*	CreateSocketObj();
	void		DeleteSocketObj(TSocketObj* pSocketObj);
	int		InvalidSocketObj(TSocketObj* pSocketObj);
	void		ReleaseGCSocketObj(int bForce = FALSE);

	void		AddClientSocketObj(CONNID dwConnID, TSocketObj* pSocketObj);
	void		CloseClientSocketObj(TSocketObj* pSocketObj, EnSocketCloseFlag enFlag = SCF_NONE, EnSocketOperation enOperation = SO_UNKNOWN, int iErrorCode = 0, int iShutdownFlag = SD_SEND);

private:
	static UINT WINAPI WorkerThreadProc(LPVOID pv);

	static UINT WINAPI DataThreadProc(LPVOID pv);	

	EnIocpAction CheckIocpCommand(OVERLAPPED* pOverlapped, unsigned long dwBytes, ULONG_PTR ulCompKey);

	void ForceDisconnect(CONNID dwConnID);
	void HandleIo		(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj, unsigned long dwBytes, unsigned long dwErrorCode);
	void HandleError	(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj, unsigned long dwErrorCode);
	void HandleAccept	(SOCKET soListen, TBufferObj* pBufferObj);
	void HandleSend		(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj);
	void HandleReceive	(CONNID dwConnID, TSocketObj* pSocketObj, TBufferObj* pBufferObj);

	int SendInternal(TSocketObj* pSocketObj, const WSABUF pBuffers[], int iCount);
	int SendPack	(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength);
	int SendSafe	(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength);
	int SendDirect	(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength);
	int CatAndPost	(TSocketObj* pSocketObj, const BYTE* pBuffer, int iLength);

	int DoAccept		();
	int ContinueReceive(TSocketObj* pSocketObj, TBufferObj* pBufferObj, EnHandleResult& hr);
	int DoReceive		(TSocketObj* pSocketObj, TBufferObj* pBufferObj);

	int DoSend		(CONNID dwConnID);
	int DoSend		(TSocketObj* pSocketObj);
	int DoSendPack	(TSocketObj* pSocketObj);
	int DoSendSafe	(TSocketObj* pSocketObj);
	int SendItem	(TSocketObj* pSocketObj);

	void CheckError	(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode);

public:
	CTcpServer(ITcpServerListener* pListener)
	: m_pListener				(pListener)
	, m_hCompletePort			(nullptr)
	, m_soListen				(INVALID_SOCKET)
	, m_iRemainAcceptSockets	(0)
	, m_pfnAcceptEx				(nullptr)
	, m_pfnGetAcceptExSockaddrs	(nullptr)
	, m_pfnDisconnectEx			(nullptr)
	, m_enLastError				(SE_OK)
	, m_enState					(SS_STOPPED)
	, m_usFamily				(AF_UNSPEC)
	, m_enSendPolicy			(SP_PACK)
	, m_dwMaxConnectionCount	(DEFAULT_MAX_CONNECTION_COUNT)
	, m_dwWorkerThreadCount		(DEFAULT_WORKER_THREAD_COUNT)
	, m_dwSocketListenQueue		(DEFAULT_TCP_SERVER_SOCKET_LISTEN_QUEUE)
	, m_dwAcceptSocketCount		(DEFAULT_TCP_SERVER_ACCEPT_SOCKET_COUNT)
	, m_dwSocketBufferSize		(DEFAULT_TCP_SOCKET_BUFFER_SIZE)
	, m_dwFreeSocketObjLockTime	(DEFAULT_FREE_SOCKETOBJ_LOCK_TIME)
	, m_dwFreeSocketObjPool		(DEFAULT_FREE_SOCKETOBJ_POOL)
	, m_dwFreeBufferObjPool		(DEFAULT_FREE_BUFFEROBJ_POOL)
	, m_dwFreeSocketObjHold		(DEFAULT_FREE_SOCKETOBJ_HOLD)
	, m_dwFreeBufferObjHold		(DEFAULT_FREE_BUFFEROBJ_HOLD)
	, m_dwKeepAliveTime			(DEFALUT_TCP_KEEPALIVE_TIME)
	, m_dwKeepAliveInterval		(DEFALUT_TCP_KEEPALIVE_INTERVAL)
	, m_bMarkSilence			(TRUE)
	{
		ASSERT(sm_wsSocket.IsValid());
		ASSERT(m_pListener);
	}

	virtual ~CTcpServer()
	{
		Stop();
	}

private:
	EnSendPolicy m_enSendPolicy;
	unsigned long m_dwMaxConnectionCount;
	unsigned long m_dwWorkerThreadCount;
	unsigned long m_dwSocketListenQueue;
	unsigned long m_dwAcceptSocketCount;
	unsigned long m_dwSocketBufferSize;
	unsigned long m_dwFreeSocketObjLockTime;
	unsigned long m_dwFreeSocketObjPool;
	unsigned long m_dwFreeBufferObjPool;
	unsigned long m_dwFreeSocketObjHold;
	unsigned long m_dwFreeBufferObjHold;
	unsigned long m_dwKeepAliveTime;
	unsigned long m_dwKeepAliveInterval;
	int  m_bMarkSilence;

private:
	static const CInitSocket	sm_wsSocket;

	LPFN_ACCEPTEX				m_pfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS	m_pfnGetAcceptExSockaddrs;
	LPFN_DISCONNECTEX			m_pfnDisconnectEx;

	ADDRESS_FAMILY				m_usFamily;

private:
	ITcpServerListener*	m_pListener;
	SOCKET				m_soListen;
	HANDLE				m_hCompletePort;
	EnServiceState		m_enState;
	EnSocketError		m_enLastError;

	vector<HANDLE>		m_vtWorkerThreads;

	CPrivateHeap		m_phSocket;
	CBufferObjPool		m_bfObjPool;

	CSpinGuard			m_csState;

	TSocketObjPtrPool	m_bfActiveSockets;

	TSocketObjPtrList	m_lsFreeSocket;
	TSocketObjPtrQueue	m_lsGCSocket;

	volatile long		m_iRemainAcceptSockets;
};

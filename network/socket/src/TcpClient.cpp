#include "stdafx.h"
#include "TcpClient.h"
//#include "../Common/Src/WaitFor.h"
#include <process.h>
// 
// #include "mtHelper.h"
// #include "log4cpp/Category.hh"
// #include "log4cpp/RollingFileAppender.hh"
// #include "log4cpp/PatternLayout.hh"

//#pragma comment(lib,"../lib/log4cppD.lib")

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
// 	strftime(stm, sizeof(stm), "\\client_%y_%m_%d %H_%M_%S.txt", timeinfo);
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
// 	log4cpp::Category& netlog = root.getInstance("client");
// 	root.setRootPriority(log4cpp::Priority::ERROR);
// 
// 	netlog.setPriority(log4cpp::Priority::INFO);
// }

const CInitSocket CTcpClient::sm_wsSocket;

int CTcpClient::Start(const char* lpszRemoteAddress, USHORT usPort, int bAsyncConnect, const char* lpszBindAddress)
{
//	Initlog();

	if(!CheckParams() || !CheckStarting())
		return FALSE;

	PrepareStart();
	m_ccContext.Reset();

	int isOK		= FALSE;
	m_bAsyncConnect	= bAsyncConnect;

	HP_SOCKADDR addrRemote, addrBind;

	if(CreateClientSocket(lpszRemoteAddress, addrRemote, usPort, lpszBindAddress, addrBind))
	{
		if(BindClientSocket(addrBind))
		{
			if(FirePrepareConnect(m_soClient) != HR_ERROR)
			{
				if(ConnectToServer(addrRemote))
				{
					if(CreateWorkerThread())
						isOK = TRUE;
					else
					{
						SetLastError(SE_WORKER_THREAD_CREATE, __FUNCTION__, ERROR_CREATE_FAILED);
//						log4cpp::Category::getInstance("client").error("%s:%d] create worker thread failed", __FILE__, __LINE__);
					}
						
				}
				else
				{
					SetLastError(SE_CONNECT_SERVER, __FUNCTION__, ::WSAGetLastError());
//					log4cpp::Category::getInstance("client").error("%s:%d] connect to server failed", __FILE__, __LINE__);
				}
					
			}
			else
			{
				SetLastError(SE_SOCKET_PREPARE, __FUNCTION__, ERROR_CANCELLED);
//				log4cpp::Category::getInstance("client").error("%s:%d] prepare connect failed", __FILE__, __LINE__);
			}
				
		}
		else
		{
			SetLastError(SE_SOCKET_BIND, __FUNCTION__, ::WSAGetLastError());
//			log4cpp::Category::getInstance("client").error("%s:%d] bind socket failed", __FILE__, __LINE__);
		}
			
	}
	else
	{
		SetLastError(SE_SOCKET_CREATE, __FUNCTION__, ::WSAGetLastError());
//		log4cpp::Category::getInstance("client").error("%s:%d] create socket failed", __FILE__, __LINE__);
	}	

	if(!isOK)
	{
		DWORD dwCode = ::GetLastError();

		m_ccContext.Reset(FALSE);
		Stop();

		::SetLastError(dwCode);
	}

	return isOK;
}

int CTcpClient::CheckParams()
{
	if	(((int)m_dwSocketBufferSize > 0)									&&
		((int)m_dwFreeBufferPoolSize >= 0)									&&
		((int)m_dwFreeBufferPoolHold >= 0)									&&
		((int)m_dwKeepAliveTime >= 1000 || m_dwKeepAliveTime == 0)			&&
		((int)m_dwKeepAliveInterval >= 1000 || m_dwKeepAliveInterval == 0)	)
		return TRUE;

	SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
	return FALSE;
}

void CTcpClient::PrepareStart()
{
	m_itPool.SetItemCapacity((int)m_dwSocketBufferSize);
	m_itPool.SetPoolSize((int)m_dwFreeBufferPoolSize);
	m_itPool.SetPoolHold((int)m_dwFreeBufferPoolHold);

	m_itPool.Prepare();
}

int CTcpClient::CheckStarting()
{
	CSpinLock locallock(m_csState);

	if(m_enState == SS_STOPPED)
		m_enState = SS_STARTING;
	else
	{
		SetLastError(SE_ILLEGAL_STATE, __FUNCTION__, ERROR_INVALID_OPERATION);
		return FALSE;
	}

	return TRUE;
}

int CTcpClient::CheckStoping(DWORD dwCurrentThreadID)
{
	if(m_enState == SS_STOPPED)
		return FALSE;

	CSpinLock locallock(m_csState);

	if(HasStarted())
	{
		m_enState = SS_STOPPING;
		return TRUE;
	}
	else if(m_enState == SS_STOPPING)
	{
		if(dwCurrentThreadID != m_dwWorkerID)
		{
			while(m_enState != SS_STOPPED)
				::Sleep(30);
		}

		SetLastError(SE_ILLEGAL_STATE, __FUNCTION__, ERROR_INVALID_OPERATION);
	}

	return FALSE;
}

int CTcpClient::CreateClientSocket(const char* lpszRemoteAddress, HP_SOCKADDR& addrRemote, unsigned short usPort, const char* lpszBindAddress, HP_SOCKADDR& addrBind)
{
	if(!::GetSockAddrByHostName(lpszRemoteAddress, usPort, addrRemote))
		return FALSE;

	if(lpszBindAddress && lpszBindAddress[0] != 0)
	{
		if(!::sockaddr_A_2_IN(lpszBindAddress, 0, addrBind))
			return FALSE;

		if(addrRemote.family != addrBind.family)
		{
			::WSASetLastError(WSAEAFNOSUPPORT);
			return FALSE;
		}
	}

	m_soClient = socket(addrRemote.family, SOCK_STREAM, IPPROTO_TCP);

	if(m_soClient == INVALID_SOCKET)
		return FALSE;

	int bOnOff	= (m_dwKeepAliveTime > 0 && m_dwKeepAliveInterval > 0);
	VERIFY(::SSO_KeepAliveVals(m_soClient, bOnOff, m_dwKeepAliveTime, m_dwKeepAliveInterval) == NO_ERROR);

	m_evSocket = ::WSACreateEvent();
	ASSERT(m_evSocket != WSA_INVALID_EVENT);

	SetRemoteHost(lpszRemoteAddress, usPort);

	return TRUE;
}

int CTcpClient::BindClientSocket(const HP_SOCKADDR& addrBind)
{
	if(addrBind.IsSpecified() && (::bind(m_soClient, addrBind.Addr(), addrBind.AddrSize()) == SOCKET_ERROR))
		return FALSE;

	m_dwConnID = ::GenerateConnectionID();

	return TRUE;
}

int CTcpClient::ConnectToServer(const HP_SOCKADDR& addrRemote)
{
	int isOK = FALSE;

	if(m_bAsyncConnect)
	{
		if(::WSAEventSelect(m_soClient, m_evSocket, FD_CONNECT | FD_CLOSE) != SOCKET_ERROR)
		{
			int rc = ::connect(m_soClient, addrRemote.Addr(), addrRemote.AddrSize());
			isOK = (rc == NO_ERROR || (rc == SOCKET_ERROR && ::WSAGetLastError() == WSAEWOULDBLOCK));
		}
	}
	else
	{
		if(::connect(m_soClient, addrRemote.Addr(), addrRemote.AddrSize()) != SOCKET_ERROR)
		{
			if(::WSAEventSelect(m_soClient, m_evSocket, FD_READ | FD_WRITE | FD_CLOSE) != SOCKET_ERROR)
			{
				if(FireConnect() != HR_ERROR)
				{
					m_enState	= SS_STARTED;
					isOK		= TRUE;
				}
			}
		}
	}

	return isOK;
}

int CTcpClient::CreateWorkerThread()
{
	m_hWorker = (HANDLE)_beginthreadex(nullptr, 0, WorkerThreadProc, (LPVOID)this, 0, &m_dwWorkerID);

	return m_hWorker != nullptr;
}

UINT WINAPI CTcpClient::WorkerThreadProc(LPVOID pv)
{
	TRACE("---------------> Client Worker Thread 0x%08X started <---------------\n", ::GetCurrentThreadId());
//	log4cpp::Category::getInstance("client").info("%s:%d] ---------------> Client Worker Thread 0x%08X started <---------------", 
//		__FILE__, __LINE__,	::GetCurrentThreadId());

	int bCallStop		= TRUE;
	CTcpClient* pClient	= (CTcpClient*)pv;
	HANDLE hEvents[]	= {pClient->m_evSocket, pClient->m_evBuffer, pClient->m_evWorker};

	pClient->m_rcBuffer.Malloc(pClient->m_dwSocketBufferSize);

	while(pClient->HasStarted())
	{
		DWORD retval = ::WSAWaitForMultipleEvents(3, hEvents, FALSE, WSA_INFINITE, FALSE);

		if(retval == WSA_WAIT_EVENT_0)
		{
			if(!pClient->ProcessNetworkEvent())
				break;
		}
		else if(retval == WSA_WAIT_EVENT_0 + 1)
		{
			if(!pClient->SendData())
				break;
		}
		else if(retval == WSA_WAIT_EVENT_0 + 2)
		{
			bCallStop = FALSE;
			break;
		}
		else if(retval == WSA_WAIT_FAILED)
		{
			pClient->m_ccContext.Reset(TRUE, SO_UNKNOWN, ::WSAGetLastError());
			break;
		}
		else
			VERIFY(FALSE);
	}

	pClient->OnWorkerThreadEnd(::GetCurrentThreadId());

	if(bCallStop && pClient->HasStarted())
		pClient->Stop();

	TRACE("---------------> Client Worker Thread 0x%08X stoped <---------------\n", ::GetCurrentThreadId());
//	log4cpp::Category::getInstance("client").info("%s:%d] ---------------> Client Worker Thread 0x%08X stoped <---------------",
//		__FILE__, __LINE__, ::GetCurrentThreadId());

	return 0;
}

int CTcpClient::ProcessNetworkEvent()
{
	int bContinue = TRUE;
	WSANETWORKEVENTS events;
	
	int rc = ::WSAEnumNetworkEvents(m_soClient, m_evSocket, &events);

	if(rc == SOCKET_ERROR)
		bContinue = HandleError(events);

	if(m_bAsyncConnect && bContinue && events.lNetworkEvents & FD_CONNECT)
		bContinue = HandleConnect(events);

	if(bContinue && events.lNetworkEvents & FD_READ)
		bContinue = HandleRead(events);

	if(bContinue && events.lNetworkEvents & FD_WRITE)
		bContinue = HandleWrite(events);

	if(bContinue && events.lNetworkEvents & FD_CLOSE)
		bContinue = HandleClose(events);

	return bContinue;
}

int CTcpClient::HandleError(WSANETWORKEVENTS& events)
{
	int iCode						= ::WSAGetLastError();
	EnSocketOperation enOperation	= SO_UNKNOWN;

	if(events.lNetworkEvents & FD_CONNECT)
		enOperation = SO_CONNECT;
	else if(events.lNetworkEvents & FD_CLOSE)
		enOperation = SO_CLOSE;
	else if(events.lNetworkEvents & FD_READ)
		enOperation = SO_RECEIVE;
	else if(events.lNetworkEvents & FD_WRITE)
		enOperation = SO_SEND;

	VERIFY(::WSAResetEvent(m_evSocket));
	m_ccContext.Reset(TRUE, enOperation, iCode);

	return FALSE;
}

int CTcpClient::HandleRead(WSANETWORKEVENTS& events)
{
	int bContinue	= TRUE;
	int iCode		= events.iErrorCode[FD_READ_BIT];

	if(iCode == 0)
		bContinue = ReadData();
	else
	{
		m_ccContext.Reset(TRUE, SO_RECEIVE, iCode);
		bContinue = FALSE;
	}

	return bContinue;
}

int CTcpClient::HandleWrite(WSANETWORKEVENTS& events)
{
	int bContinue	= TRUE;
	int iCode		= events.iErrorCode[FD_WRITE_BIT];

	if(iCode == 0)
		bContinue = SendData();
	else
	{
		m_ccContext.Reset(TRUE, SO_SEND, iCode);
		bContinue = FALSE;
	}

	return bContinue;
}

int CTcpClient::HandleConnect(WSANETWORKEVENTS& events)
{
	int bContinue	= TRUE;
	int iCode		= events.iErrorCode[FD_CONNECT_BIT];

	if(iCode == 0)
	{
		if(::WSAEventSelect(m_soClient, m_evSocket, FD_READ | FD_WRITE | FD_CLOSE) != SOCKET_ERROR)
		{
			if(FireConnect() != HR_ERROR)
				m_enState = SS_STARTED;
			else
				iCode = ERROR_CANCELLED;
		}
		else
			iCode = ::WSAGetLastError();
	}

	if(iCode != 0)
	{
		if(iCode != ERROR_CANCELLED)
			m_ccContext.Reset(TRUE, SO_CONNECT, iCode);
		else
			m_ccContext.Reset(FALSE);

		bContinue = FALSE;
	}

	return bContinue;
}

int CTcpClient::HandleClose(WSANETWORKEVENTS& events)
{
	int iCode = events.iErrorCode[FD_CLOSE_BIT];

	if(iCode == 0)
		m_ccContext.Reset(TRUE, SO_CLOSE, SE_OK);
	else
		m_ccContext.Reset(TRUE, SO_CLOSE, iCode);

	return FALSE;
}

int CTcpClient::ReadData()
{
	while(TRUE)
	{
		int rc = recv(m_soClient, (char*)(BYTE*)m_rcBuffer, m_dwSocketBufferSize, 0);

		if(rc > 0)
		{
			if(FireReceive(m_rcBuffer, rc) == HR_ERROR)
			{
				TRACE("<C-CNNID: %Iu> OnReceive() event return 'HR_ERROR', connection will be closed !\n", m_dwConnID);
//				log4cpp::Category::getInstance("client").error("%s:%d] <C-CNNID: %Iu> OnReceive() event return 'HR_ERROR', connection will be closed !",
//					__FILE__, __LINE__, m_dwConnID);

				m_ccContext.Reset(TRUE, SO_RECEIVE, ERROR_CANCELLED);
				return FALSE;
			}
		}
		else if(rc == SOCKET_ERROR)
		{
			int code = ::WSAGetLastError();

			if(code == WSAEWOULDBLOCK)
				break;
			else
			{
				m_ccContext.Reset(TRUE, SO_RECEIVE, code);
				return FALSE;
			}
		}
		else if(rc == 0)
		{
			m_ccContext.Reset(TRUE, SO_CLOSE, SE_OK);
			return FALSE;
		}
		else
			ASSERT(FALSE);
	}

	return TRUE;
}

int CTcpClient::SendData()
{
	int isOK = TRUE;

	while(TRUE)
	{
		TItemPtr itPtr(m_itPool, GetSendBuffer());

		if(itPtr.IsValid())
		{
			ASSERT(!itPtr->IsEmpty());

			isOK = DoSendData(itPtr);

			if(isOK)
			{
				if(!itPtr->IsEmpty())
				{
					CCriSecLock locallock(m_csSend);
					m_lsSend.PushFront(itPtr.Detach());
					
					break;
				}
			}
			else
				break;
		}
		else
			break;
	}

	return isOK;
}

TItem* CTcpClient::GetSendBuffer()
{
	TItem* pItem = nullptr;

	if(m_lsSend.Size() > 0)
	{
		CCriSecLock locallock(m_csSend);

		if(m_lsSend.Size() > 0)
			pItem = m_lsSend.PopFront();
	}

	return pItem;
}

int CTcpClient::DoSendData(TItem* pItem)
{
	while(!pItem->IsEmpty())
	{
		int rc = 0;

		{
			CCriSecLock locallock(m_csSend);

			rc = send(m_soClient, (char*)pItem->Ptr(), min(pItem->Size(), (int)m_dwSocketBufferSize), 0);
			if(rc > 0) m_iPending -= rc;
		}

		if(rc > 0)
		{
			if(FireSend(pItem->Ptr(), rc) == HR_ERROR)
			{
				TRACE("<C-CNNID: %Iu> OnSend() event should not return 'HR_ERROR' !!\n", m_dwConnID);
//				log4cpp::Category::getInstance("client").error("%s:%d] <C-CNNID: %Iu> OnSend() event should not return 'HR_ERROR' !!",
//					__FILE__, __LINE__, m_dwConnID);

				ASSERT(FALSE);
			}

			pItem->Reduce(rc);
		}
		else if(rc == SOCKET_ERROR)
		{
			int code = ::WSAGetLastError();

			if(code == WSAEWOULDBLOCK)
				break;
			else
			{
				m_ccContext.Reset(TRUE, SO_SEND, code);
				return FALSE;
			}
		}
		else
			ASSERT(FALSE);
	}

	return TRUE;
}

int CTcpClient::Stop()
{
	DWORD dwCurrentThreadID = ::GetCurrentThreadId();

	if(!CheckStoping(dwCurrentThreadID))
		return FALSE;

	WaitForWorkerThreadEnd(dwCurrentThreadID);

	if(m_ccContext.bFireOnClose)
		FireClose(m_ccContext.enOperation, m_ccContext.iErrorCode);

	if(m_evSocket != nullptr)
	{
		::WSACloseEvent(m_evSocket);
		m_evSocket	= nullptr;
	}

	if(m_soClient != INVALID_SOCKET)
	{
		shutdown(m_soClient, SD_SEND);
		closesocket(m_soClient);
		m_soClient	= INVALID_SOCKET;
	}

	Reset();

	return TRUE;
}

void CTcpClient::Reset()
{
	CCriSecLock locallock(m_csSend);

	m_rcBuffer.Free();
	m_evBuffer.Reset();
	m_evWorker.Reset();
	m_lsSend.Clear();
	m_itPool.Clear();

//	m_strHost.Empty();
	m_strHost.clear();

	m_usPort	= 0;
	m_iPending	= 0;
	m_enState	= SS_STOPPED;
}

void CTcpClient::WaitForWorkerThreadEnd(DWORD dwCurrentThreadID)
{
	if(m_hWorker != nullptr)
	{
		if(dwCurrentThreadID != m_dwWorkerID)
		{
			m_evWorker.Set();
			VERIFY(::WaitForSingleObject(m_hWorker, INFINITE) == WAIT_OBJECT_0);
		}

		::CloseHandle(m_hWorker);

		m_hWorker		= nullptr;
		m_dwWorkerID	= 0;
	}
}

int CTcpClient::Send(const BYTE* pBuffer, int iLength, int iOffset)
{
	ASSERT(pBuffer && iLength > 0);

	if(iOffset != 0) pBuffer += iOffset;

	WSABUF buffer;
	buffer.len = iLength;
	buffer.buf = (char*)pBuffer;

	return SendPackets(&buffer, 1);
}

int CTcpClient::DoSendPackets(const WSABUF pBuffers[], int iCount)
{
	ASSERT(pBuffers && iCount > 0);

	int result = NO_ERROR;

	if(pBuffers && iCount > 0)
	{
		if(HasStarted())
		{
			CCriSecLock locallock(m_csSend);

			if(HasStarted())
				result = SendInternal(pBuffers, iCount);
			else
				result = ERROR_INVALID_STATE;
		}
		else
			result = ERROR_INVALID_STATE;
	}
	else
		result = ERROR_INVALID_PARAMETER;

	if(result != NO_ERROR)
		::SetLastError(result);

	return (result == NO_ERROR);
}

int CTcpClient::SendInternal(const WSABUF pBuffers[], int iCount)
{
	int result = NO_ERROR;

	ASSERT(m_iPending >= 0);

	int iPending	= m_iPending;
	int isPending	= m_iPending > 0;

	for(int i = 0; i < iCount; i++)
	{
		int iBufLen = pBuffers[i].len;

		if(iBufLen > 0)
		{
			BYTE* pBuffer = (BYTE*)pBuffers[i].buf;
			ASSERT(pBuffer);

			m_lsSend.Cat(pBuffer, iBufLen);
			m_iPending += iBufLen;
		}
	}

	if(!isPending && m_iPending > iPending) m_evBuffer.Set();

	return result;
}

int CTcpClient::SendSmallFile(const char* lpszFileName, const LPWSABUF pHead, const LPWSABUF pTail)
{
	CAtlFile file;
	CAtlFileMapping<> fmap;
	WSABUF szBuf[3];

	HRESULT hr = ::MakeSmallFilePackage(lpszFileName, file, fmap, szBuf, pHead, pTail);

	if(FAILED(hr))
	{
		::SetLastError(HRESULT_CODE(hr));
		return FALSE;
	}

	return SendPackets(szBuf, 3);
}

void CTcpClient::SetLastError(EnSocketError code, LPCSTR func, int ec)
{
	TRACE("%s --> Error: %d, EC: %d\n", func, code, ec);
//	log4cpp::Category::getInstance("client").error("%s:%d] %s --> Error: %d, EC: %d",
//		__FILE__, __LINE__, func, code, ec);

	m_enLastError = code;
	::SetLastError(ec);
}

int CTcpClient::GetLocalAddress(char lpszAddress[], int& iAddressLen, USHORT& usPort)
{
	ASSERT(lpszAddress != nullptr && iAddressLen > 0);

	return ::GetSocketLocalAddress(m_soClient, lpszAddress, iAddressLen, usPort);
}

void CTcpClient::SetRemoteHost(const char* lpszHost, USHORT usPort)
{
	m_strHost = lpszHost;
	m_usPort  = usPort;
}

int CTcpClient::GetRemoteHost(char lpszHost[], int& iHostLen, USHORT& usPort)
{
	int isOK = FALSE;

	if(m_strHost.length() == 0)
		return isOK;

	int iLen = (int)m_strHost.length() + 1;

	if(iHostLen >= iLen)
	{
		memcpy(lpszHost, m_strHost.c_str(), iLen * sizeof(char));
		usPort = m_usPort;

		isOK = TRUE;
	}

	iHostLen = iLen;

	return isOK;
}


int CTcpClient::GetRemoteHost(char** lpszHost, USHORT* pusPort)
{
//	*lpszHost = m_strHost;
	strcpy_s((char*)*lpszHost, m_strHost.length() + 1, m_strHost.c_str());

	if(pusPort != nullptr)
		*pusPort = m_usPort;

//	return !m_strHost.IsEmpty();
	return m_strHost.length() != 0;
}

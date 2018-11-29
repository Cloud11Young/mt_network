#include "stdafx.h"
#include <ws2tcpip.h>
#include "Listener.h"
#include "BufferPool.h"
#include <winsock2.h>
#include "../public/HPSocket.h"
#include <process.h>
#include "log4cpp/Category.hh"

static char* GetLastErrorToString(DWORD errorCode)
{
	//因为FORMAT_MESSAGE_ALLOCATE_BUFFER标志，这个函数帮你分配内存，所以需要LocalFree来释放

	char *text=NULL;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&text, 0, NULL);
//	std::string str = WideToMultiByte(text);
	//	CStringA result(text);    //结果
	char* ptmp = new char[strlen(text) + 1];
	strcpy_s(ptmp, strlen(text) + 1,text);
	LocalFree(text);
	return ptmp;
}

static const char* GetMapOpertor(EnSocketOperation oper)
{
	switch (oper)
	{
	case SO_ACCEPT:
		return "so_accept";
	case SO_CLOSE:
		return "so_close";
	case SO_CONNECT:
		return "so_connect";
	case SO_RECEIVE:
		return "so_receive";
	case SO_SEND:
		return "so_send";
	case SO_UNKNOWN:
		return "so_unknown";
	}
	return "so_unknown";
}


ServerListener::ServerListener()/*:m_extractlock(FALSE,L"datalock")*/{
	
}

ServerListener::~ServerListener(){
	for (auto it = m_vAddr.begin(); it != m_vAddr.end(); it++){
		if (it->second){
			delete it->second;
			it->second = NULL;
		}
	}
	m_vAddr.clear();
}

EnHandleResult ServerListener::OnPrepareListen(ITcpServer* pSender, SOCKET soListen){
	return HR_IGNORE;
}

EnHandleResult ServerListener::OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient){
	RemoteAddress* remoteaddr=new RemoteAddress;
	remoteaddr->dwConnID = dwConnID;
	remoteaddr->iAddressLen = sizeof(remoteaddr->pAddress) / sizeof(char);
	BOOL bGet=pSender->GetRemoteAddress(dwConnID, remoteaddr->pAddress, remoteaddr->iAddressLen, remoteaddr->usPort);
	if (bGet == FALSE)
		return HR_ERROR;
	
	m_vAddr[dwConnID] = remoteaddr;	

	if (m_pCallBack){
		SOCKADDR_IN sa;
		int len = sizeof(sa);
		getpeername(soClient, (SOCKADDR*)&sa, &len);
		char name[50] = { 0 };
		getnameinfo((SOCKADDR*)&sa, len, name, 50, NULL, 0, NI_NAMEREQD);

// 		TCHAR hostname[50] = { 0 };
// 		int iLength = MultiByteToWideChar(CP_ACP, 0, name, (int)strlen(name) + 1, NULL, 0);
// 		MultiByteToWideChar(CP_ACP, 0, name, (int)strlen(name) + 1, hostname, iLength);
		m_pCallBack->lpConnectCB(m_pCallBack->lpCallBackData, remoteaddr->pAddress, remoteaddr->usPort, name);
	}
	return HR_OK;
}

EnHandleResult ServerListener::OnHandShake(ITcpServer* pSender, CONNID dwConnID){
	return HR_IGNORE;
}

EnHandleResult ServerListener::OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength){
	return HR_IGNORE;
}

EnHandleResult ServerListener::OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength){
// 	Buffer* pBuf = new Buffer;
// 	pBuf->copy(pData, iLength);
// 
// 	std::deque<Buffer*>* pQueue = BufferPool::Instance()->CreateBuffer(dwConnID);
// 	pQueue->push_back(pBuf);
// 
// 	ExtractBuffer(pQueue, dwConnID);

	RemoteAddress remoteaddr;
	remoteaddr.dwConnID = dwConnID;
	remoteaddr.iAddressLen = sizeof(remoteaddr.pAddress) / sizeof(char);
	BOOL bGet = pSender->GetRemoteAddress(dwConnID, remoteaddr.pAddress, remoteaddr.iAddressLen, remoteaddr.usPort);

	if (m_pCallBack->lpRecvMsgCB != NULL)
	{
		m_pCallBack->lpRecvMsgCB(m_pCallBack->lpCallBackData, (void*)pData, iLength,
			remoteaddr.pAddress, remoteaddr.usPort);
	}

	return HR_OK;
}

EnHandleResult ServerListener::OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength){
	return HR_IGNORE;
}

EnHandleResult ServerListener::OnShutdown(ITcpServer* pSender){
	
	return HR_IGNORE;
}

EnHandleResult ServerListener::OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode){
	RemoteAddress* pRemoteAddr = FindRemoteAddr(dwConnID);
	if (dwConnID != pRemoteAddr->dwConnID)	return HR_ERROR;
	DeleteRemoteAddr(dwConnID);
	BufferPool::Instance()->RemoveLeftBuffer(dwConnID);

	if (m_pCallBack)
	{
		char* sErr = GetLastErrorToString(iErrorCode);
		const char* sOper = GetMapOpertor(enOperation);
		char err[512] = { 0 };
		sprintf(err, "socket operator %s, error %s", sOper, sErr);
		log4cpp::Category::getInstance("network").error("%s:%d] server client close info = \"%s\"",
			__FILE__, __LINE__, err);
		delete[] sErr;

		if (m_pCallBack->lpErrorCB)
		{			
			m_pCallBack->lpErrorCB(m_pCallBack->lpCallBackData, pRemoteAddr->pAddress, pRemoteAddr->usPort, err);			
		}
	

		if (m_pCallBack->lpDisconnectCB)
			m_pCallBack->lpDisconnectCB(m_pCallBack->lpCallBackData, pRemoteAddr->pAddress, pRemoteAddr->usPort);
	}

	return HR_OK;
}

RemoteAddress* ServerListener::FindRemoteAddr(CONNID dwConnID){
	return m_vAddr[dwConnID];
}

void ServerListener::DeleteRemoteAddr(CONNID dwConnID){
	auto it = m_vAddr.find(dwConnID);
	if (it != m_vAddr.end())
		m_vAddr.erase(it);
}

CONNID ServerListener::FindConnID(char* sIP, int IPLen, USHORT usPort){
	for (auto it = m_vAddr.begin(); it != m_vAddr.end(); it++){
		RemoteAddress* pAddr = it->second;
		if (!memcmp(sIP, pAddr->pAddress, IPLen) && pAddr->usPort == usPort)
			return pAddr->dwConnID;
	}
	return 0;
}

// void ServerListener::ExtractData(Buffer* pBuf, std::deque<Buffer*>* pComplete, CONNID dwConnID){
// 	BYTE* pTmpData = pBuf->pData;
// 
// 	PacketHead* pHead = (PacketHead*)pBuf->pData;
// 	unsigned int Datasize = pHead->DataSize;
// 	int len = pBuf->used;
// 
// 	int left = 0;
// 	while ((left = len - sizeof(*pHead) - Datasize) >= 0){
// 		Buffer* pCombinData = new Buffer;
// 		pCombinData->pAddr = FindRemoteAddr(dwConnID);		
// 		pCombinData->copy(pTmpData + sizeof(*pHead), Datasize);	
// 		CCriSecLock lock(m_extractlock);
// 		pComplete->push_back(pCombinData);	
// 
// 		len = left;
// 		if (left < sizeof(*pHead))
// 			break;
// 
// 		pTmpData += sizeof(*pHead) + Datasize;
// 		pHead = (PacketHead*)(pTmpData);
// 		Datasize = pHead->DataSize;
// 		if (left < (sizeof(*pHead) + Datasize))
// 			break;
// 	}
// 
// 	pBuf->TrunFront(pBuf->used - left);
// }

// void ServerListener::ExtractBuffer(std::deque<Buffer*>* pBufs, CONNID dwConnID){
// 	std::deque<Buffer*>* pComplete = BufferPool::Instance()->GetCompleteBuffers();
// 	Buffer* leftbuf = BufferPool::Instance()->GetLeftBuffer(dwConnID);
// 
// 	while (pBufs->size() > 0){
// 		Buffer* pBuf = pBufs->front();
// 
// 		leftbuf->Append(pBuf, pBuf->used);
// 		if (leftbuf->used < sizeof(PacketHead)){
// 			goto del;
// 		}
//  
// 		PacketHead* head = (PacketHead*)leftbuf->pData;
// 		if (leftbuf->used < (sizeof(*head) + head->DataSize))
// 			goto del;
//  		ExtractData(leftbuf, pComplete, dwConnID);	
// 
// 	del:
// 		if (pBuf)
// 			delete pBuf;
// 		pBufs->pop_front();
// 	}
// }

void ServerListener::RegCallBack(PUSER_CB callback){
	m_pCallBack = callback;
}

// BOOL ServerListener::StartDataThread(){
// 	BOOL isOK = TRUE;
// 
// 	HANDLE hDataThread = (HANDLE)_beginthreadex(nullptr, 0, ReceiveThread, (LPVOID)this, 0, nullptr);
// 
// 	if (!hDataThread)
// 	{
// 		//SetLastError(SE_WORKER_THREAD_CREATE, __FUNCTION__, ::GetLastError());
// 		isOK = FALSE;
// 	}
// 	CloseHandle(hDataThread);
// 
// 	return isOK;
// }

// UINT WINAPI ServerListener::ReceiveThread(LPVOID void*)
// {
// 	ServerListener* pListen = (ServerListener*)void*;
// 	while (TRUE)
// 	{
// 		Buffer* pBuffer = NULL;
// 		{
// 			CCriSecLock lock(pListen->m_extractlock);
// 			std::deque<Buffer*>* pComplete = BufferPool::Instance()->GetCompleteBuffers();
// 			if (pComplete->size() > 0)
// 			{
// 				pBuffer = pComplete->front();
// 				pComplete->pop_front();
// 			}
// 		}
// 
// 		if (pBuffer != NULL)
// 		{
// 			pListen->m_pCallBack->lpRecvMsgCB(pListen->m_pCallBack->lpCallBackData, pBuffer->pData, pBuffer->used,
// 	 					pBuffer->pAddr->pAddress, pBuffer->pAddr->usPort);
// 			delete pBuffer;
// 			pBuffer = NULL;
// 		}
// 
// // 		while (pComplete && pComplete->size() > 0)
// // 		{			
// // 			CCriSecLock lock(pListen->m_extractlock);
// // 
// // 			Buffer* pBuffer = pComplete->front();
// // 			pListen->m_pCallBack->lpRecvMsgCB(pListen->m_pCallBack->lpCallBackData, pBuffer->pData, pBuffer->used,
// // 					pBuffer->pAddr->pAddress, pBuffer->pAddr->usPort);
// // 			
// // 			delete pBuffer;
// // 			pComplete->pop_front();
// // 		}
// 		Sleep(10);
// 	}
// 	return 0;
// }


//////////////////////////////////////////////////////////////////////////

ClientListener::ClientListener() /*:m_extractlock(FALSE, L"clientlock")*/{
	m_remoteaddr = new RemoteAddress;
}

ClientListener::~ClientListener(){
	for (auto it = m_vAddr.begin(); it != m_vAddr.end(); it++){
		if (it->second){
			delete it->second;
			it->second = NULL;
		}
	}
	m_vAddr.clear();
	if (m_remoteaddr)
		delete m_remoteaddr;
	m_remoteaddr = NULL;
}

EnHandleResult ClientListener::OnPrepareListen(ITcpClient* pSender, SOCKET soListen){
	return HR_IGNORE;
}

EnHandleResult ClientListener::OnAccept(ITcpClient* pSender, CONNID dwConnID, UINT_PTR soClient){
	return HR_IGNORE;
}

EnHandleResult ClientListener::OnHandShake(ITcpClient* pSender, CONNID dwConnID){
	m_remoteaddr->dwConnID = dwConnID;
	m_remoteaddr->iAddressLen = sizeof(m_remoteaddr->pAddress) / sizeof(TCHAR);
	BOOL bGet = pSender->GetRemoteHost(m_remoteaddr->pAddress, m_remoteaddr->iAddressLen, m_remoteaddr->usPort);
	if (bGet == FALSE)
		return HR_ERROR;

	char cIP[50] = { 0 };
	int clen = sizeof(cIP) / sizeof(char);
// 	int size = WideCharToMultiByte(CP_ACP, 0, m_remoteaddr->pAddress, -1, NULL, 0, NULL, NULL);
// 	WideCharToMultiByte(CP_ACP, 0, m_remoteaddr->pAddress, -1, cIP, size, NULL, NULL);
	
	if (m_pCallBack){
		SOCKADDR_IN sa;
		int len = sizeof(sa);
		sa.sin_family = AF_INET;
		sa.sin_addr.S_un.S_addr = inet_addr(cIP);
		sa.sin_port = htons(m_remoteaddr->usPort);
		
		char name[50] = { 0 };
		getnameinfo((SOCKADDR*)&sa, len, name, 50, NULL, 0, NI_NAMEREQD);

// 		TCHAR hostname[50] = { 0 };
// 		int iLength = MultiByteToWideChar(CP_ACP, 0, name, (int)strlen(name) + 1, NULL, 0);
// 		MultiByteToWideChar(CP_ACP, 0, name, (int)strlen(name) + 1, hostname, iLength);
		m_pCallBack->lpConnectCB(m_pCallBack->lpCallBackData, m_remoteaddr->pAddress, m_remoteaddr->usPort, name);
		m_vAddr[dwConnID] = m_remoteaddr;
	}
	return HR_OK;
}

EnHandleResult ClientListener::OnReceive(ITcpClient* pSender, CONNID dwConnID, int iLength){
	return HR_IGNORE;
}

EnHandleResult ClientListener::OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength){
// 	Buffer* pBuf = new Buffer;
// 	pBuf->copy(pData, iLength);
// 
// 	std::deque<Buffer*>* pQueue = BufferPool::Instance()->CreateBuffer(dwConnID);
// 	pQueue->push_back(pBuf);
// 
// 	ExtractBuffer(pQueue, dwConnID);
//	RemoteAddress remoteaddr;
//	BOOL bGet = pSender->GetRemoteHost(dwConnID, remoteaddr.pAddress, remoteaddr.iAddressLen, remoteaddr.usPort);

	if (m_pCallBack->lpRecvMsgCB != NULL)
	{
		m_pCallBack->lpRecvMsgCB(m_pCallBack->lpCallBackData, (void*)pData, iLength,
			m_remoteaddr->pAddress, m_remoteaddr->usPort);
	}

	return HR_OK;
}

EnHandleResult ClientListener::OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength){
	return HR_IGNORE;
}

EnHandleResult ClientListener::OnShutdown(ITcpClient* pSender){
	return HR_IGNORE;
}

EnHandleResult ClientListener::OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode){
// 	RemoteAddress* pRemoteAddr = FindRemoteAddr(dwConnID);
// 	if (dwConnID != pRemoteAddr->dwConnID)	return HR_ERROR;
	BufferPool::Instance()->RemoveLeftBuffer(dwConnID);

//	if (m_pCallBack)	m_pCallBack->lpDisconnectCB(m_pCallBack->lpCallBackData, m_remoteaddr->pAddress, m_remoteaddr->usPort);
	if (m_pCallBack)
	{
		char* sErr = GetLastErrorToString(iErrorCode);
		const char* sOper = GetMapOpertor(enOperation);
		char err[512] = { 0 };
		sprintf(err, "socket operator %s, error %s", sOper, sErr);
		log4cpp::Category::getInstance("network").error("%s:%d] client close info = \"%s\"",
			__FILE__, __LINE__, err);

		if (m_pCallBack->lpErrorCB)
		{			
			m_pCallBack->lpErrorCB(m_pCallBack->lpCallBackData, m_remoteaddr->pAddress, m_remoteaddr->usPort, err);			
		}
		delete[] sErr;

		if (m_pCallBack->lpDisconnectCB)
			m_pCallBack->lpDisconnectCB(m_pCallBack->lpCallBackData, m_remoteaddr->pAddress, m_remoteaddr->usPort);
	}
	return HR_OK;
}


RemoteAddress* ClientListener::FindRemoteAddr(CONNID dwConnID){
	return m_vAddr[dwConnID];
}

void ClientListener::DeleteRemoteAddr(CONNID dwConnID){
	auto it = m_vAddr.find(dwConnID);
	if (it != m_vAddr.end())
		m_vAddr.erase(it);
}

CONNID ClientListener::FindConnID(TCHAR* sIP, int IPLen, USHORT usPort){
	for (auto it = m_vAddr.begin(); it != m_vAddr.end(); it++){
		RemoteAddress* pAddr = it->second;
		if (!memcmp(sIP, pAddr->pAddress, IPLen) && pAddr->usPort == usPort)
			return pAddr->dwConnID;
	}
	return 0;
}

// void ClientListener::ExtractData(Buffer* pBuf, std::deque<Buffer*>* pComplete, CONNID dwConnID){
// 	BYTE* pTmpData = pBuf->pData;
// 
// 	PacketHead* pHead = (PacketHead*)pBuf->pData;
// 	unsigned int Datasize = pHead->DataSize;
// 	int len = pBuf->used;
// 
// 	int left = 0;
// 	while ((left = len - sizeof(*pHead) - Datasize) >= 0){
// 		CCriSecLock lock(m_extractlock);
// 		Buffer* pCombinData = new Buffer;
// 		pCombinData->pAddr = FindRemoteAddr(dwConnID);
// 		pCombinData->copy(pTmpData + sizeof(*pHead), Datasize);
// 		pComplete->push_back(pCombinData);
// 
// 		len = left;
// 		if (left < sizeof(*pHead))
// 			break;
// 
// 		pTmpData += sizeof(*pHead) + Datasize;
// 		pHead = (PacketHead*)(pTmpData);
// 		Datasize = pHead->DataSize;
// 		if (left < (sizeof(*pHead) + Datasize))
// 			break;
// 	}
// 
// 	pBuf->TrunFront(pBuf->used - left);
// }

// void ClientListener::ExtractBuffer(std::deque<Buffer*>* pBufs, CONNID dwConnID){
// 	std::deque<Buffer*>* pComplete = BufferPool::Instance()->GetCompleteBuffers();
// 	Buffer* leftbuf = BufferPool::Instance()->GetLeftBuffer(dwConnID);
// 
// 	while (pBufs->size() > 0){
// 		Buffer* pBuf = pBufs->front();
// 
// 		leftbuf->Append(pBuf, pBuf->used);
// 		if (leftbuf->used < sizeof(PacketHead)){
// 			goto del;
// 		}
// 
// 		PacketHead* head = (PacketHead*)leftbuf->pData;
// 		if (leftbuf->used < (sizeof(*head) + head->DataSize))
// 			goto del;
// 		ExtractData(leftbuf, pComplete, dwConnID);
// 
// 	del:
// 		if (pBuf)
// 			delete pBuf;
// 		pBufs->pop_front();
// 	}
// }

void ClientListener::RegCallBack(PUSER_CB callback){
	m_pCallBack = callback;
}

// BOOL ClientListener::StartDataThread(){
// 	BOOL isOK = TRUE;
// 
// 	HANDLE hDataThread = (HANDLE)_beginthreadex(nullptr, 0, ReceiveThread, (LPVOID)this, 0, nullptr);
// 
// 	if (!hDataThread)
// 	{
// 		//SetLastError(SE_WORKER_THREAD_CREATE, __FUNCTION__, ::GetLastError());
// 		isOK = FALSE;
// 	}
// 	CloseHandle(hDataThread);
// 
// 	return isOK;
// }


// UINT WINAPI ClientListener::ReceiveThread(LPVOID void*){
// 	ClientListener* pListen = (ClientListener*)void*;
// 
// 	while (TRUE)
// 	{
// 		Buffer* pBuffer = NULL;
// 		{
// 			CCriSecLock lock(pListen->m_extractlock);
// 			std::deque<Buffer*>* pComplete = BufferPool::Instance()->GetCompleteBuffers();
// 			if (pComplete->size() > 0)
// 			{
// 				pBuffer = pComplete->front();
// 				pComplete->pop_front();
// 			}
// 		}
// 
// 		if (pBuffer != NULL)
// 		{
// 			pListen->m_pCallBack->lpRecvMsgCB(pListen->m_pCallBack->lpCallBackData, pBuffer->pData, pBuffer->used,
// 				pBuffer->pAddr->pAddress, pBuffer->pAddr->usPort);
// 			delete pBuffer;
// 			pBuffer = NULL;
// 		}
// // 		std::deque<Buffer*>* pComplete = BufferPool::Instance()->GetCompleteBuffers();		
// // 		while (pComplete && pComplete->size() > 0){
// // 			CCriSecLock lock(pListen->m_extractlock);
// // 			Buffer* pBuffer = pComplete->front();
// // 
// // 			pListen->m_pCallBack->lpRecvMsgCB(pListen->m_pCallBack->lpCallBackData, pBuffer->pData, pBuffer->used,
// // 				pBuffer->pAddr->pAddress, pBuffer->pAddr->usPort);
// // 
// // 			delete pBuffer;
// // 			pComplete->pop_front();
// // 		}
// 		Sleep(10);
// 	}
// 	return 0;
// }

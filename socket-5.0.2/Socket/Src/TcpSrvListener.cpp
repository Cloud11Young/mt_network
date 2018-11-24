#include "stdafx.h"
#include "TcpSrvListener.h"




ServerListener::ServerListener(){//:m_pLeftBuf(NULL),m_extractlock(FALSE,"extractlock"){

}

ServerListener::~ServerListener(){
// 	if (m_pLeftBuf)
// 		delete m_pLeftBuf;
// 	m_pLeftBuf = NULL;
}

EnHandleResult ServerListener::OnPrepareListen(ITcpServer* pSender, SOCKET soListen){
	return HR_IGNORE;
}

EnHandleResult ServerListener::OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient){
	return HR_IGNORE;
}

EnHandleResult ServerListener::OnHandShake(ITcpServer* pSender, CONNID dwConnID){
	return HR_IGNORE;
}

EnHandleResult ServerListener::OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength){
// 	Buffer* pBuf = new Buffer;
// 	pBuf->copy(pData, iLength);
// 
// 	std::queue<Buffer*>* pQueue=BufferPool::Instance()->CreateBuffer(dwConnID);
// 	pQueue->push(pBuf);
// 	ExtractBuffer(pQueue, dwConnID);

	return HR_OK;
}

EnHandleResult ServerListener::OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength){
	return HR_IGNORE;

}

EnHandleResult ServerListener::OnShutdown(ITcpServer* pSender){
	return HR_IGNORE;
}


// void ServerListener::Combination(std::queue<Buffer*>& vBuf){
// 	while (vBuf.size() > 0){
// 		Buffer* pBuf = vBuf.front();		
// 
// 		if (pBuf->Length < sizeof(PacketHead)){
// 			if (!m_pLeftBuf){
// 				m_pLeftBuf = new Buffer;
// 				m_pLeftBuf->copy(pBuf->pData, pBuf->Length);
// 				goto del;
// 			}
// 		}
// 		
// 		if (m_pLeftBuf){
// 			if (m_pLeftBuf->Length < sizeof(PacketHead)){
// 				m_pLeftBuf->Append(pBuf, pBuf->Length);
// 				if (m_pLeftBuf->Length < sizeof(PacketHead)){
// 					goto del;
// 				}
// 			}
// 
// 			PacketHead* frontHead = (PacketHead*)m_pLeftBuf->pData;
// 			unsigned int Datasize = frontHead->DataSize;
// 
// 			if (m_pLeftBuf->Length > (Datasize+sizeof(*frontHead))){
// 				//m_pLeftBuf = ExtractData(m_pLeftBuf);
// 				goto del;
// 			}
// 
// 			if (Datasize > (m_pLeftBuf->Length - sizeof(*frontHead) + pBuf->Length)){
// 				m_pLeftBuf->Append(pBuf, pBuf->Length);
// 				goto del;
// 			}
// 
// 			m_pLeftBuf->Append(pBuf, Datasize - m_pLeftBuf->Length + sizeof(*frontHead));
// 			m_pLeftBuf->TrunFront(sizeof(*frontHead));			
// 			vCombinData.push(m_pLeftBuf);
// 			m_pLeftBuf = NULL;
// 		}
// 
// //		m_pLeftBuf = ExtractData(pBuf);
// 
// 	del:
// 		if (pBuf)
// 			delete pBuf;
// 		vBuf.pop();
// 	}
// }

// Buffer* ServerListener::ExtractData(Buffer* pBuf, std::queue<Buffer*>* pComplete){
// 	BYTE* pTmpData = pBuf->pData;
// 
// 	PacketHead* pHead = (PacketHead*)pBuf->pData;
// 	unsigned int Datasize = pHead->DataSize;
// 
// 	int left = 0;
// 	while ((left = pBuf->Length - sizeof(*pHead) - Datasize) >= 0){
// 		Buffer* pCombinData = new Buffer;
// 		pCombinData->copy(pTmpData + sizeof(*pHead), Datasize);
// 		CMutexLock lock(m_extractlock);
// 		pComplete->push(pCombinData);
// 
// 		pBuf->Length = left;
// 		if (left < sizeof(*pHead))
// 			break;
// 
// 		pTmpData += sizeof(*pHead) + Datasize;
// 		pHead = (PacketHead*)(pTmpData);
// 		Datasize = pHead->DataSize;
// 	}
// 
// 	Buffer* pLeftBuf = NULL;
// 	if (left != 0){
// 		pLeftBuf = new Buffer;
// 		pLeftBuf->copy(pTmpData, pBuf->Length);
// 	}
// 	return pLeftBuf;
// }

// void ServerListener::ExtractBuffer(std::queue<Buffer*>* pBufs, CONNID ConID){
// 	std::queue<Buffer*>* pComplete = BufferPool::Instance()->CreateCompleteBuffer(ConID);
// 
// 	while (pBufs->size() > 0){
// 		Buffer* pBuf = pBufs->front();
// 
// 		if (pBuf->Length < sizeof(PacketHead)){
// 			if (!m_pLeftBuf){
// 				m_pLeftBuf = new Buffer;
// 				m_pLeftBuf->copy(pBuf->pData, pBuf->Length);
// 				goto del;
// 			}
// 		}
// 
// 		if (m_pLeftBuf){
// 			if (m_pLeftBuf->Length < sizeof(PacketHead)){
// 				m_pLeftBuf->Append(pBuf, pBuf->Length);
// 				if (m_pLeftBuf->Length < sizeof(PacketHead)){
// 					goto del;
// 				}
// 			}
// 
// 			PacketHead* frontHead = (PacketHead*)m_pLeftBuf->pData;
// 			unsigned int Datasize = frontHead->DataSize;
// 
// 			if (m_pLeftBuf->Length >(Datasize + sizeof(*frontHead))){
// 				m_pLeftBuf = ExtractData(m_pLeftBuf, pComplete);
// 				goto del;
// 			}
// 
// 			if (Datasize > (m_pLeftBuf->Length - sizeof(*frontHead) + pBuf->Length)){
// 				m_pLeftBuf->Append(pBuf, pBuf->Length);
// 				goto del;
// 			}
// 
// 			m_pLeftBuf->Append(pBuf, Datasize - m_pLeftBuf->Length + sizeof(*frontHead));
// 			m_pLeftBuf->TrunFront(sizeof(*frontHead));
// 			pComplete->push(m_pLeftBuf);
// 			m_pLeftBuf = NULL;
// 		}
// 
// 		m_pLeftBuf = ExtractData(pBuf, pComplete);
// 
// 	del:
// 		if (pBuf)
// 			delete pBuf;
// 		pBufs->pop();
// 	}
// }


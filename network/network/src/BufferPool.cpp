#include "stdafx.h"
#include "BufferPool.h"

BufferPool* BufferPool::ptr = NULL;

BufferPool::BufferPool() :m_vCompletePool(NULL){

}

BufferPool::~BufferPool(){
	if (m_vCompletePool){
		while (m_vCompletePool->size()){
			Buffer* ptmp = m_vCompletePool->front();
			if (ptmp)
				delete ptmp;
			
			m_vCompletePool->pop_front();
		}
		delete m_vCompletePool;
		m_vCompletePool = NULL;
	}
}

std::deque<Buffer*>* BufferPool::CreateBuffer(CONNID dwConID){
	std::lock_guard<std::mutex> lck(m_mapLock);
	auto it = m_mapPool.find(dwConID);
	if (it != m_mapPool.end())
		return (it->second);
	std::deque<Buffer*>* pQueue = new std::deque<Buffer*>;
	m_mapPool[dwConID] = pQueue;
	return pQueue;
}

// std::queue<Buffer*>* BufferPool::CreateCompleteBuffer(){
// 	auto it = m_mapCompletePool.find(dwConID);
// 	if (it != m_mapCompletePool.end())
// 		return (it->second);
// 	std::queue<Buffer*>* pQueue = new std::queue<Buffer*>;
// 	m_mapCompletePool[dwConID] = pQueue;
// 	return pQueue;
// }

std::deque<Buffer*>* BufferPool::GetCompleteBuffers(){
	if (!m_vCompletePool)
		m_vCompletePool = new std::deque<Buffer*>;
	return m_vCompletePool;
}

Buffer* BufferPool::GetLeftBuffer(CONNID dwConnID){
	Buffer* leftbuf;
	if (m_mapLeft.find(dwConnID) == m_mapLeft.end()){
		leftbuf = new Buffer;
		m_mapLeft[dwConnID] = leftbuf;
	}
	return m_mapLeft[dwConnID];
}

BOOL BufferPool::RemoveLeftBuffer(CONNID dwConnID){
	auto it = m_mapLeft.find(dwConnID);
	if (it != m_mapLeft.end()){
		if (it->second)
			delete it->second;
		m_mapLeft.erase(it);
	}
	return TRUE;
}
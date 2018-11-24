#ifndef BUFFER_POOL_H_
#define BUFFER_POOL_H_
#include <queue>
#include <map>
#include "../public/HPTypeDef.h"
#include "DataType.h"

#include <mutex>

class BufferPool{
public:
	static BufferPool* Instance(){
		if (ptr == NULL)
		ptr = new BufferPool;
		return ptr;
	}
	std::deque<Buffer*>* CreateBuffer(CONNID dwConID);
	std::deque<Buffer*>* GetCompleteBuffers();
	Buffer* GetLeftBuffer(CONNID dwConnID);
	BOOL BufferPool::RemoveLeftBuffer(CONNID dwConnID);

private:
	static BufferPool* ptr;
	BufferPool();
	~BufferPool();	
	std::map<CONNID, std::deque<Buffer*>* > m_mapPool;
	std::deque<Buffer*>*  m_vCompletePool;
	std::map<CONNID, Buffer*> m_mapLeft;
	std::mutex  m_mapLock;
};

#endif
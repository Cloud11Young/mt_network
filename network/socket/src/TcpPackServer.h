#pragma once

#include "TcpServer.h"
#include "MiscHelper.h"
#include "../Common/Src/bufferpool.h"

template<class T> class CTcpPackServerT : public IPackSocket, public T
{
public:
	virtual int SendPackets(CONNID dwConnID, const WSABUF pBuffers[], int iCount)
	{
		int iNewCount = iCount + 1;
		unique_ptr<WSABUF[]> buffers(new WSABUF[iNewCount]);

		DWORD header;
		if(!::AddPackHeader(pBuffers, iCount, buffers, m_dwMaxPackSize, m_usHeaderFlag, header))
			return FALSE;

		return __super::SendPackets(dwConnID, buffers.get(), iNewCount);
	}

protected:
	virtual EnHandleResult DoFireHandShake(TSocketObj* pSocketObj)
	{
		EnHandleResult result = __super::DoFireHandShake(pSocketObj);

		if(result != HR_ERROR)
		{
			TBuffer* pBuffer = m_bfPool.PickFreeBuffer(pSocketObj->connID);
			VERIFY(SetConnectionReserved(pSocketObj, TBufferPackInfo::Construct(pBuffer)));
		}

		return result;
	}

	virtual EnHandleResult DoFireReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
	{
		TBufferPackInfo* pInfo = nullptr;
		GetConnectionReserved(pSocketObj, (void**)&pInfo);
		ASSERT(pInfo);

		TBuffer* pBuffer = (TBuffer*)pInfo->pBuffer;
		ASSERT(pBuffer && pBuffer->IsValid());

		return ParsePack(this, pInfo, pBuffer, pSocketObj, m_dwMaxPackSize, m_usHeaderFlag, pData, iLength);
	}

	virtual EnHandleResult DoFireClose(TSocketObj* pSocketObj, EnSocketOperation enOperation, int iErrorCode)
	{
		EnHandleResult result = __super::DoFireClose(pSocketObj, enOperation, iErrorCode);

		TBufferPackInfo* pInfo = nullptr;
		GetConnectionReserved(pSocketObj, (void**)&pInfo);
		ASSERT(pInfo);

		if(pInfo != nullptr)
		{
			m_bfPool.PutFreeBuffer(pInfo->pBuffer);
			TBufferPackInfo::Destruct(pInfo);
		}

		return result;
	}

	virtual EnHandleResult DoFireShutdown()
	{
		EnHandleResult result = __super::DoFireShutdown();

		m_bfPool.Clear();

		return result;
	}

	virtual int CheckParams()
	{
		if	((m_dwMaxPackSize > 0 && m_dwMaxPackSize <= TCP_PACK_MAX_SIZE_LIMIT)	&&
			(m_usHeaderFlag >= 0 && m_usHeaderFlag <= TCP_PACK_HEADER_FLAG_LIMIT)	)
			return __super::CheckParams();

		SetLastError(SE_INVALID_PARAM, __FUNCTION__, ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	virtual void PrepareStart()
	{
		__super::PrepareStart();

		m_bfPool.SetMaxCacheSize	(GetMaxConnectionCount());
		m_bfPool.SetItemCapacity	(GetSocketBufferSize());
		m_bfPool.SetItemPoolSize	(GetFreeBufferObjPool());
		m_bfPool.SetItemPoolHold	(GetFreeBufferObjHold());
		m_bfPool.SetBufferLockTime	(GetFreeSocketObjLockTime());
		m_bfPool.SetBufferPoolSize	(GetFreeSocketObjPool());
		m_bfPool.SetBufferPoolHold	(GetFreeSocketObjHold());

		m_bfPool.Prepare();
	}

public:
	virtual void SetMaxPackSize		(DWORD dwMaxPackSize)		{m_dwMaxPackSize = dwMaxPackSize;}
	virtual void SetPackHeaderFlag	(USHORT usPackHeaderFlag)	{m_usHeaderFlag  = usPackHeaderFlag;}
	virtual DWORD GetMaxPackSize	()							{return m_dwMaxPackSize;}
	virtual USHORT GetPackHeaderFlag()							{return m_usHeaderFlag;}

private:
	EnHandleResult DoFireSuperReceive(TSocketObj* pSocketObj, const BYTE* pData, int iLength)
		{return __super::DoFireReceive(pSocketObj, pData, iLength);}

	friend EnHandleResult ParsePack<>	(CTcpPackServerT* pThis, TBufferPackInfo* pInfo, TBuffer* pBuffer, TSocketObj* pSocket,
										DWORD dwMaxPackSize, USHORT usPackHeaderFlag, const BYTE* pData, int iLength);

public:
	CTcpPackServerT(ITcpServerListener* pListener)
	: T					(pListener)
	, m_dwMaxPackSize	(TCP_PACK_DEFAULT_MAX_SIZE)
	, m_usHeaderFlag	(TCP_PACK_DEFAULT_HEADER_FLAG)
	{

	}

	virtual ~CTcpPackServerT()
	{
		Stop();
	}

private:
	DWORD	m_dwMaxPackSize;
	USHORT	m_usHeaderFlag;

	CBufferPool m_bfPool;
};

typedef CTcpPackServerT<CTcpServer> CTcpPackServer;

#ifdef _SSL_SUPPORT

#include "SSLServer.h"
typedef CTcpPackServerT<CSSLServer> CSSLPackServer;

#endif

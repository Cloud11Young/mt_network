#pragma once

#include <ws2tcpip.h>
#include <mswsock.h>
#include <malloc.h>

#include <atlfile.h>

#include "SocketInterface.h"
#include "../Common/Src/WaitFor.h"
#include "../Common/Src/bufferpool.h"
#include "../Common/Src/RingBuffer.h"

/************************************************************************
���ƣ�ȫ�ֳ���
��������������Ĺ���ȫ�ֳ���
************************************************************************/

/* HP-Socket �汾�� */
#define HP_VERSION_MAJOR						5
#define HP_VERSION_MINOR						0
#define HP_VERSION_REVISE						2
#define HP_VERSION_BUILD						1

/* IOCP ������߳��� */
#define MAX_WORKER_THREAD_COUNT					500
/* IOCP Socket ��������Сֵ */
#define MIN_SOCKET_BUFFER_SIZE					88
/* С�ļ�����ֽ��� */
#define MAX_SMALL_FILE_SIZE						0x3FFFFF
/* �������ʱ�� */
#define MAX_CONNECTION_PERIOD					(MAXLONG / 2)
/* IOCP ��������¼�ʱ�������ȡ���� */
#define MAX_IOCP_CONTINUE_RECEIVE				25

/* Server/Agent Ĭ����������� */
#define DEFAULT_MAX_CONNECTION_COUNT			10000
/* Server/Agent Ĭ�� IOCP �����߳��� */
#define DEFAULT_WORKER_THREAD_COUNT				GetDefaultWorkerThreadCount()
/* Server/Agent Ĭ�� Socket �����������ʱ�� */
#define DEFAULT_FREE_SOCKETOBJ_LOCK_TIME		(10 * 1000)
/* Server/Agent Ĭ�� Socket ����ش�С */
#define DEFAULT_FREE_SOCKETOBJ_POOL				150
/* Server/Agent Ĭ�� Socket ����ػ��շ�ֵ */
#define DEFAULT_FREE_SOCKETOBJ_HOLD				600
/* Server/Agent Ĭ���ڴ�黺��ش�С */
#define DEFAULT_FREE_BUFFEROBJ_POOL				300
/* Server/Agent Ĭ���ڴ�黺��ػ��շ�ֵ */
#define DEFAULT_FREE_BUFFEROBJ_HOLD				1200
/* Client Ĭ���ڴ�黺��ش�С */
#define DEFAULT_CLIENT_FREE_BUFFER_POOL_SIZE	10
/* Client Ĭ���ڴ�黺��ػ��շ�ֵ */
#define DEFAULT_CLIENT_FREE_BUFFER_POOL_HOLD	40
/* IPv4 Ĭ�ϰ󶨵�ַ */
#define  DEFAULT_IPV4_BIND_ADDRESS				"0.0.0.0"
/* IPv6 Ĭ�ϰ󶨵�ַ */
#define  DEFAULT_IPV6_BIND_ADDRESS				"::"

/* TCP Ĭ��ͨ�����ݻ�������С */
#define DEFAULT_TCP_SOCKET_BUFFER_SIZE			GetDefaultTcpSocketBufferSize()
/* TCP Ĭ����������� */
#define DEFALUT_TCP_KEEPALIVE_TIME				(30 * 1000)
/* TCP Ĭ������ȷ�ϰ������ */
#define DEFALUT_TCP_KEEPALIVE_INTERVAL			(10 * 1000)
/* TCP Server Ĭ�� Listen ���д�С */
#define DEFAULT_TCP_SERVER_SOCKET_LISTEN_QUEUE	SOMAXCONN
/* TCP Server Ĭ��ԤͶ�� Accept ���� */
#define DEFAULT_TCP_SERVER_ACCEPT_SOCKET_COUNT	300

/* UDP Ĭ�����ݱ�����󳤶� */
#define DEFAULT_UDP_MAX_DATAGRAM_SIZE			1472
/* UDP Ĭ�� Receive ԤͶ������ */
#define DEFAULT_UDP_POST_RECEIVE_COUNT			300
/* UDP Ĭ�ϼ������Դ��� */
#define DEFAULT_UDP_DETECT_ATTEMPTS				3
/* UDP Ĭ�ϼ������ͼ�� */
#define DEFAULT_UDP_DETECT_INTERVAL				20

/* TCP Pack ������λ�� */
#define TCP_PACK_LENGTH_BITS					22
/* TCP Pack ���������� */
#define TCP_PACK_LENGTH_MASK					0x3FFFFF
/* TCP Pack ����󳤶�Ӳ���� */
#define TCP_PACK_MAX_SIZE_LIMIT					0x3FFFFF
/* TCP Pack ��Ĭ����󳤶� */
#define TCP_PACK_DEFAULT_MAX_SIZE				0x040000
/* TCP Pack ��ͷ��ʶֵӲ���� */
#define TCP_PACK_HEADER_FLAG_LIMIT				0x0003FF
/* TCP Pack ��ͷĬ�ϱ�ʶֵ */
#define TCP_PACK_DEFAULT_HEADER_FLAG			0x000000

#define PORT_SEPARATOR_CHAR						':'
#define IPV6_ADDR_BEGIN_CHAR					'['
#define IPV6_ADDR_END_CHAR						']'
#define IPV4_ADDR_SEPARATOR_CHAR				'.'
#define IPV6_ADDR_SEPARATOR_CHAR				':'
#define IPV6_ZONE_INDEX_CHAR					'%'

unsigned long GetDefaultWorkerThreadCount();
unsigned long GetDefaultTcpSocketBufferSize();

/************************************************************************
���ƣ�Windows Socket �����ʼ����
�������Զ����غ�ж�� Windows Socket ���
************************************************************************/
class CInitSocket
{
public:
	CInitSocket(LPWSADATA lpWSAData = nullptr, BYTE minorVersion = 2, BYTE majorVersion = 2)
	{
		LPWSADATA lpTemp = lpWSAData;
		if(!lpTemp)
			lpTemp	= (LPWSADATA)_alloca(sizeof(WSADATA));

		m_iResult	= ::WSAStartup(MAKEWORD(majorVersion, minorVersion), lpTemp);
	}

	~CInitSocket()
	{
		if(IsValid())
			::WSACleanup();
	}

	int	 GetResult() const {return m_iResult;}
	int IsValid()	 const {return m_iResult == 0;}

private:
	int m_iResult;
};

typedef struct hp_addr
{
	ADDRESS_FAMILY family;

	union
	{
		ULONG_PTR	addr;
		IN_ADDR		addr4;
		IN6_ADDR	addr6;
	};

	static const hp_addr any_addr4;
	static const hp_addr any_addr6;

	inline int AddrSize() const
	{
		return AddrSize(family);
	}

	inline static int AddrSize(ADDRESS_FAMILY f)
	{
		if(f == AF_INET)
			return sizeof(IN_ADDR);

		return sizeof(IN6_ADDR);
	}

	inline static const hp_addr& AnyAddr(ADDRESS_FAMILY f)
	{
		static const hp_addr s_any_addr4(AF_INET, TRUE);
		static const hp_addr s_any_addr6(AF_INET6, TRUE);

		if(f == AF_INET)
			return s_any_addr4;

		return s_any_addr6;
	}

	inline const ULONG_PTR* Addr()	const	{return &addr;}
	inline ULONG_PTR* Addr()				{return &addr;}

	inline int IsIPv4()			const	{return family == AF_INET;}
	inline int IsIPv6()			const	{return family == AF_INET6;}
	inline int IsSpecified()		const	{return IsIPv4() || IsIPv6();}
	inline void ZeroAddr()					{::ZeroMemory(&addr6, sizeof(addr6));}
	inline void Reset()						{::ZeroMemory(this, sizeof(*this));}

	inline hp_addr& Copy(hp_addr& other) const
	{
		if(this != &other)
			memcpy(&other, this, offsetof(hp_addr, addr) + AddrSize());

		return other;
	}

	hp_addr(ADDRESS_FAMILY f = AF_UNSPEC, int bZeroAddr = FALSE)
	{
		family = f;

		if(bZeroAddr) ZeroAddr();
	}

} HP_ADDR, *HP_PADDR;

typedef struct hp_sockaddr
{
	union
	{
		ADDRESS_FAMILY	family;
		SOCKADDR		addr;
		SOCKADDR_IN		addr4;
		SOCKADDR_IN6	addr6;
	};

	inline int AddrSize() const
	{
		return AddrSize(family);
	}

	inline static int AddrSize(ADDRESS_FAMILY f)
	{
		if(f == AF_INET)
			return sizeof(SOCKADDR_IN);

		return sizeof(SOCKADDR_IN6);
	}

	inline static const hp_sockaddr& AnyAddr(ADDRESS_FAMILY f)
	{
		static const hp_sockaddr s_any_addr4(AF_INET, TRUE);
		static const hp_sockaddr s_any_addr6(AF_INET6, TRUE);

		if(f == AF_INET)
			return s_any_addr4;

		return s_any_addr6;
	}

	inline static int AddrMinStrLength(ADDRESS_FAMILY f)
	{
		if(f == AF_INET)
			return 16;

		return 46;
	}

	inline int IsIPv4()			const	{return family == AF_INET;}
	inline int IsIPv6()			const	{return family == AF_INET6;}
	inline int IsSpecified()		const	{return IsIPv4() || IsIPv6();}
	inline USHORT Port()			const	{return ntohs(addr4.sin_port);}
	inline void SetPort(USHORT usPort)		{addr4.sin_port = htons(usPort);}
	inline void* SinAddr()			const	{return IsIPv4() ? (void*)&addr4.sin_addr : (void*)&addr6.sin6_addr;}
	inline void* SinAddr()					{return IsIPv4() ? (void*)&addr4.sin_addr : (void*)&addr6.sin6_addr;}

	inline const SOCKADDR* Addr()	const	{return &addr;}
	inline SOCKADDR* Addr()					{return &addr;}
	inline void ZeroAddr()					{::ZeroMemory(((char*)this) + sizeof(family), sizeof(*this) - sizeof(family));}
	inline void Reset()						{::ZeroMemory(this, sizeof(*this));}

	inline hp_sockaddr& Copy(hp_sockaddr& other) const
	{
		if(this != &other)
			memcpy(&other, this, AddrSize());

		return other;
	}

	size_t Hash() const
	{
		ASSERT(IsSpecified());

		if(IsIPv4())
			return ((addr4.sin_family << 16) | addr4.sin_port) ^ addr4.sin_addr.s_addr;
		else
		{
			ULONG* p = (ULONG*)(((char*)this) + offsetof(SOCKADDR_IN6, sin6_addr));
			return ((addr6.sin6_family << 16) | addr6.sin6_port) ^ addr6.sin6_flowinfo ^ p[0] ^ p[1] ^ p[2] ^ p[3] ^ p[4];
		}
	}

	int EqualTo(const hp_sockaddr& other) const
	{
		ASSERT(IsSpecified() && other.IsSpecified());

		if(IsIPv4())
			return memcmp(this, &other, offsetof(SOCKADDR_IN, sin_zero)) == 0;
		else
			return memcmp(this, &other, sizeof(addr6)) == 0;
	}

	hp_sockaddr(ADDRESS_FAMILY f = AF_UNSPEC, int bZeroAddr = FALSE)
	{
		family = f;

		if(bZeroAddr) ZeroAddr();
	}

} HP_SOCKADDR, *HP_PSOCKADDR;

/* Server ����� Agent ����ڲ�ʹ�õ��¼����������� */

// �����ѹر�
#define HR_CLOSED	0xFF

/* �ر����ӱ�ʶ */
enum EnSocketCloseFlag
{
	SCF_NONE	= 0,	// �������¼�
	SCF_CLOSE	= 1,	// ���� �����ر� OnClose �¼�
	SCF_ERROR	= 2		// ���� �쳣�ر� OnClose �¼�
};

/* ���ݻ����������ṹ */
template<class T> struct TBufferObjBase
{
	WSAOVERLAPPED		ov;
	EnSocketOperation	operation;
	WSABUF				buff;

	int					capacity;
	CPrivateHeap&		heap;

	T* next;
	T* last;

	static T* Construct(CPrivateHeap& heap, unsigned long dwCapacity)
	{
		T* pBufferObj = (T*)heap.Alloc(sizeof(T) + dwCapacity);
		ASSERT(pBufferObj);

		ZeroMemory(pBufferObj, sizeof(T));
		
		pBufferObj->TBufferObjBase::TBufferObjBase(heap, dwCapacity);
		pBufferObj->buff.buf = ((char*)pBufferObj) + sizeof(T);

		return pBufferObj;
	}

	static void Destruct(T* pBufferObj)
	{
		ASSERT(pBufferObj);
		pBufferObj->heap.Free(pBufferObj);
	}

	TBufferObjBase(CPrivateHeap& hp, unsigned long dwCapacity)
	: heap(hp)
	, capacity((int)dwCapacity)
	{
		ASSERT(capacity > 0);
	}

	int Cat(const BYTE* pData, int length)
	{
		ASSERT(pData != nullptr && length > 0);

		int cat = min(Remain(), length);

		if(cat > 0)
		{
			memcpy(buff.buf + buff.len, pData, cat);
			buff.len += cat;
		}

		return cat;
	}

	void Reset()	{buff.len = 0;}
	int Remain()	{return capacity - buff.len;}
	int IsFull()	{return Remain() == 0;}
};

/* ���ݻ������ṹ */
struct TBufferObj : public TBufferObjBase<TBufferObj>
{
	SOCKET client;
};

/* UDP ���ݻ������ṹ */
struct TUdpBufferObj : public TBufferObjBase<TUdpBufferObj>
{
	HP_SOCKADDR	remoteAddr;
	int			addrLen;
};

/* ���ݻ���������ģ�� */
template<class T> struct TBufferObjListT : public TSimpleList<T>
{
public:
	int Cat(const BYTE* pData, int length)
	{
		ASSERT(pData != nullptr && length > 0);

		int remain = length;

		while(remain > 0)
		{
			T* pItem = Back();

			if(pItem == nullptr || pItem->IsFull())
				pItem = PushBack(bfPool.PickFreeItem());

			int cat  = pItem->Cat(pData, remain);

			pData	+= cat;
			remain	-= cat;
		}

		return length;
	}

	T* PushTail(const BYTE* pData, int length)
	{
		ASSERT(pData != nullptr && length > 0 && length <= (int)bfPool.GetItemCapacity());

		T* pItem = PushBack(bfPool.PickFreeItem());
		pItem->Cat(pData, length);

		return pItem;
	}

	void Release()
	{
		bfPool.PutFreeItem(*this);
	}

public:
	TBufferObjListT(CNodePoolT<T>& pool) : bfPool(pool)
	{
	}

private:
	CNodePoolT<T>& bfPool;
};

/* ���ݻ���������� */
typedef CNodePoolT<TBufferObj>			CBufferObjPool;
/* UDP ���ݻ���������� */
typedef CNodePoolT<TUdpBufferObj>		CUdpBufferObjPool;
/* ���ݻ���������ģ�� */
typedef TBufferObjListT<TBufferObj>		TBufferObjList;
/* UDP ���ݻ���������ģ�� */
typedef TBufferObjListT<TUdpBufferObj>	TUdpBufferObjList;

/* ���ݻ������ṹ���� */
typedef CRingPool<TBufferObj>		TBufferObjPtrList;

/* Udp ���ݻ������ṹ���� */
typedef CRingPool<TUdpBufferObj>	TUdpBufferObjPtrList;

/* Socket �����������ṹ */
struct TSocketObjBase
{
	static const long DEF_SNDBUFF_SIZE = 8192;

	CONNID		connID;
	HP_SOCKADDR	remoteAddr;
	void*		extra;
	void*		reserved;
	void*		reserved2;
	int		valid;

	union
	{
		unsigned long	freeTime;
		unsigned long	connTime;
	};

	unsigned long		activeTime;

	CCriSec		csSend;

	long			sndBuffSize;
	volatile int	smooth;
	volatile long	pending;
	volatile long	sndCount;

	CReentrantSpinGuard csRecv;

	static int IsExist(TSocketObjBase* pSocketObj)
		{return pSocketObj != nullptr;}

	static int IsValid(TSocketObjBase* pSocketObj)
		{return pSocketObj != nullptr && pSocketObj->valid;}

	static void Invalid(TSocketObjBase* pSocketObj)
		{ASSERT(IsExist(pSocketObj)); pSocketObj->valid = FALSE;}

	static void Release(TSocketObjBase* pSocketObj)
		{ASSERT(IsExist(pSocketObj)); pSocketObj->freeTime = ::TimeGetTime();}

	long Pending()		{return pending;}
	int IsPending()	{return pending > 0;}
	int IsCanSend()	{return sndCount <= sndBuffSize;}
	int IsSmooth()		{return smooth;}
	void TurnOnSmooth()	{smooth = TRUE;}

	int TurnOffSmooth()
		{return ::InterlockedCompareExchange((volatile long*)&smooth, FALSE, TRUE) == TRUE;}
	
	int ResetSndBuffSize(SOCKET socket)
	{
		int len = (int)(sizeof(sndBuffSize));
		return getsockopt(socket, SOL_SOCKET, SO_SNDBUF, (CHAR*)&sndBuffSize, &len) != 0;
	}

	void Reset(CONNID dwConnID)
	{
		connID		= dwConnID;
		valid		= TRUE;
		smooth		= TRUE;
		pending		= 0;
		sndCount	= 0;
		sndBuffSize	= DEF_SNDBUFF_SIZE;
		extra		= nullptr;
		reserved	= nullptr;
		reserved2	= nullptr;
	}
};

/* ���ݻ������ṹ */
struct TSocketObj : public TSocketObjBase
{
	SOCKET			socket;
	CStringA		host;
	TBufferObjList	sndBuff;
	
	TSocketObj(CBufferObjPool& bfPool)
	: sndBuff(bfPool)
	{

	}

	static void Release(TSocketObj* pSocketObj)
	{
		__super::Release(pSocketObj);

		pSocketObj->sndBuff.Release();
	}

	void Reset(CONNID dwConnID, SOCKET soClient)
	{
		__super::Reset(dwConnID);
		
		host.Empty();

		socket = soClient;
	}

	int GetRemoteHost(LPCSTR* lpszHost, USHORT* pusPort = nullptr)
	{
		*lpszHost = host;

		if(pusPort)
			*pusPort = remoteAddr.Port();

		return (*lpszHost != nullptr && (*lpszHost)[0] != 0);
	}
};

/* UDP ���ݻ������ṹ */
struct TUdpSocketObj : public TSocketObjBase
{
	TUdpBufferObjList	sndBuff;
	volatile unsigned long		detectFails;

	TUdpSocketObj(CUdpBufferObjPool& bfPool)
	: sndBuff(bfPool)
	{

	}

	static void Release(TUdpSocketObj* pSocketObj)
	{
		__super::Release(pSocketObj);

		pSocketObj->sndBuff.Release();
	}

	void Reset(CONNID dwConnID)
	{
		__super::Reset(dwConnID);
		detectFails = 0;
	}
};

/* ��Ч TSocketObj ���� */
typedef CRingCache2<TSocketObj, CONNID, true>		TSocketObjPtrPool;
/* ʧЧ TSocketObj ���� */
typedef CRingPool<TSocketObj>						TSocketObjPtrList;
/* ʧЧ TSocketObj �������սṹ���� */
typedef CCASQueue<TSocketObj>						TSocketObjPtrQueue;

/* ��Ч TUdpSocketObj ���� */
typedef CRingCache2<TUdpSocketObj, CONNID, true>	TUdpSocketObjPtrPool;
/* ʧЧ TUdpSocketObj ���� */
typedef CRingPool<TUdpSocketObj>					TUdpSocketObjPtrList;
/* ʧЧ TUdpSocketObj �������սṹ���� */
typedef CCASQueue<TUdpSocketObj>					TUdpSocketObjPtrQueue;

/* HP_SOCKADDR �Ƚ��� */
struct hp_sockaddr_func
{
	struct hash
	{
		size_t operator() (const HP_SOCKADDR* pA) const
		{
			return pA->Hash();
		}
	};

	struct equal_to
	{
		int operator () (const HP_SOCKADDR* pA, const HP_SOCKADDR* pB) const
		{
			return pA->EqualTo(*pB);
		}
	};

};

/* ��ַ-���� ID ��ϣ�� */
typedef unordered_map<const HP_SOCKADDR*, CONNID, hp_sockaddr_func::hash, hp_sockaddr_func::equal_to>
										TSockAddrMap;
/* ��ַ-���� ID ��ϣ������� */
typedef TSockAddrMap::iterator			TSockAddrMapI;
/* ��ַ-���� ID ��ϣ�� const ������ */
typedef TSockAddrMap::const_iterator	TSockAddrMapCI;

/* IClient ����ر������� */
struct TClientCloseContext
{
	int bFireOnClose;
	EnSocketOperation enOperation;
	int iErrorCode;

	TClientCloseContext(int bFire = TRUE, EnSocketOperation enOp = SO_CLOSE, int iCode = SE_OK)
	{
		Reset(bFire, enOp, iCode);
	}

	void Reset(int bFire = TRUE, EnSocketOperation enOp = SO_CLOSE, int iCode = SE_OK)
	{
		bFireOnClose = bFire;
		enOperation	 = enOp;
		iErrorCode	 = iCode;
	}

};

/*****************************************************************************************************/
/******************************************** ������������ ********************************************/
/*****************************************************************************************************/

// ��ȡ HPSocket �汾�ţ�4 ���ֽڷֱ�Ϊ�����汾�ţ��Ӱ汾�ţ������汾�ţ�������ţ�
unsigned long GetHPSocketVersion();

/* ��ȡ���������ı� */
const char* GetSocketErrorDesc(EnSocketError enCode);

/* ȷ����ַ�� */
ADDRESS_FAMILY DetermineAddrFamily(const char* lpszAddress);

/* ��ַ�ַ�����ַת��Ϊ HP_ADDR */
int GetInAddr(const char* lpszAddress, __out HP_ADDR& addr);

/* ��ַ�ַ�����ַת��Ϊ HP_SOCKADDR */
int GetSockAddr(const char* lpszAddress, USHORT usPort, __inout HP_SOCKADDR& addr);

/* ����ַ����Ƿ���� IP ��ַ��ʽ */
int IsIPAddress(const char* lpszAddress, __out EnIPAddrType* penType = nullptr);

/* ͨ����������ȡ IP ��ַ */
int GetIPAddress(const char* lpszHost, __out char* lpszIP, __inout int& iIPLenth, __out EnIPAddrType& enType);

/* ͨ����������ȡ HP_SOCKADDR */
int GetSockAddrByHostName(const char* lpszHost, USHORT usPort, __out HP_SOCKADDR& addr);

/* ͨ����������ȡ HP_SOCKADDR */
int GetSockAddrByHostNameDirectly(const char* lpszHost, USHORT usPort, HP_SOCKADDR &addr);

/* ö������ IP ��ַ */
int EnumHostIPAddresses(const char* lpszHost, EnIPAddrType enType, __out LPTIPAddr** lpppIPAddr, __out int& iIPAddrCount);

/* ��� LPTIPAddr* */
int RetrieveSockAddrIPAddresses(const vector<HP_PSOCKADDR>& vt, __out LPTIPAddr** lpppIPAddr, __out int& iIPAddrCount);

/* �ͷ� LPTIPAddr* */
int FreeHostIPAddresses(LPTIPAddr* lppIPAddr);

/* �� HP_SOCKADDR �ṹת��Ϊ��ַ�ַ��� */
int sockaddr_IN_2_A(const HP_SOCKADDR& addr, __out ADDRESS_FAMILY& usFamily, __out char* lpszAddress, __inout int& iAddressLen, __out USHORT& usPort);

/* �ѵ�ַ�ַ���ת��Ϊ HP_SOCKADDR �ṹ */
int sockaddr_A_2_IN(const char* lpszAddress, unsigned short usPort, __out HP_SOCKADDR& addr);

/* ��ȡ Socket �ı��ػ�Զ�̵�ַ��Ϣ */
int GetSocketAddress(SOCKET socket, __out char* lpszAddress, __inout int& iAddressLen, __out USHORT& usPort, int bLocal = TRUE);

/* ��ȡ Socket �ı��ص�ַ��Ϣ */
int GetSocketLocalAddress(SOCKET socket, __out char* lpszAddress, __inout int& iAddressLen, __out USHORT& usPort);
/* ��ȡ Socket ��Զ�̵�ַ��Ϣ */
int GetSocketRemoteAddress(SOCKET socket, __out char* lpszAddress, __inout int& iAddressLen, __out USHORT& usPort);

/* 64 λ�����ֽ���ת�����ֽ��� */
ULONGLONG NToH64(ULONGLONG value);
/* 64 λ�����ֽ���ת�����ֽ��� */
ULONGLONG HToN64(ULONGLONG value);

/* ��ȡ Socket ��ĳ����չ������ָ�� */
void* GetExtensionFuncPtr					(SOCKET sock, GUID guid);
/* ��ȡ AcceptEx ��չ����ָ�� */
LPFN_ACCEPTEX Get_AcceptEx_FuncPtr			(SOCKET sock);
/* ��ȡ GetAcceptExSockaddrs ��չ����ָ�� */
LPFN_GETACCEPTEXSOCKADDRS Get_GetAcceptExSockaddrs_FuncPtr(SOCKET sock);
/* ��ȡ ConnectEx ��չ����ָ�� */
LPFN_CONNECTEX Get_ConnectEx_FuncPtr		(SOCKET sock);
/* ��ȡ TransmitFile ��չ����ָ�� */
LPFN_TRANSMITFILE Get_TransmitFile_FuncPtr	(SOCKET sock);
/* ��ȡ DisconnectEx ��չ����ָ�� */
LPFN_DISCONNECTEX Get_DisconnectEx_FuncPtr	(SOCKET sock);

HRESULT ReadSmallFile(const char* lpszFileName, CAtlFile& file, CAtlFileMapping<>& fmap, unsigned long dwMaxFileSize = MAX_SMALL_FILE_SIZE);
HRESULT MakeSmallFilePackage(const char* lpszFileName, CAtlFile& file, CAtlFileMapping<>& fmap, WSABUF szBuf[3], const LPWSABUF pHead = nullptr, const LPWSABUF pTail = nullptr);

/************************************************************************
���ƣ�IOCP ָ��Ͷ�ݰ�������
�������� IOCP ָ��Ͷ��
************************************************************************/

/* IOCP ���� */
enum EnIocpCommand
{
	IOCP_CMD_EXIT		= 0x00000000,	// �˳�����
	IOCP_CMD_ACCEPT		= 0xFFFFFFF1,	// ��������
	IOCP_CMD_DISCONNECT	= 0xFFFFFFF2,	// �Ͽ�����
	IOCP_CMD_SEND		= 0xFFFFFFF3	// ��������
};

/* IOCP ������� */
enum EnIocpAction
{
	IOCP_ACT_GOON		= 0,	// ����ִ��
	IOCP_ACT_CONTINUE	= 1,	// ����ִ��
	IOCP_ACT_BREAK		= 2		// �ж�ִ��
};

int PostIocpCommand(HANDLE hIOCP, EnIocpCommand enCmd, ULONG_PTR ulParam);
int PostIocpExit(HANDLE hIOCP);
int PostIocpAccept(HANDLE hIOCP);
int PostIocpDisconnect(HANDLE hIOCP, CONNID dwConnID);
int PostIocpSend(HANDLE hIOCP, CONNID dwConnID);
int PostIocpClose(HANDLE hIOCP, CONNID dwConnID, int iErrorCode);

/************************************************************************
���ƣ�setsockopt() ��������
�������򻯳��õ� setsockopt() ����
************************************************************************/

int SSO_SetSocketOption		(SOCKET sock, int level, int name, LPVOID val, int len);
int SSO_GetSocketOption		(SOCKET sock, int level, int name, LPVOID val, int* len);
int SSO_IoctlSocket			(SOCKET sock, long cmd, u_long* arg);
int SSO_WSAIoctl			(SOCKET sock, unsigned long dwIoControlCode, LPVOID lpvInBuffer, unsigned long cbInBuffer, LPVOID lpvOutBuffer, unsigned long cbOutBuffer, LPDWORD lpcbBytesReturned);

int SSO_UpdateAcceptContext	(SOCKET soClient, SOCKET soBind);
int SSO_UpdateConnectContext(SOCKET soClient, int iOption);
int SSO_NoBlock				(SOCKET sock, int bNoBlock = TRUE);
int SSO_NoDelay				(SOCKET sock, int bNoDelay = TRUE);
int SSO_DontLinger			(SOCKET sock, int bDont = TRUE);
int SSO_Linger				(SOCKET sock, USHORT l_onoff, USHORT l_linger);
int SSO_KeepAlive			(SOCKET sock, int bKeepAlive = TRUE);
int SSO_KeepAliveVals		(SOCKET sock, u_long onoff, u_long time, u_long interval);
int SSO_RecvBuffSize		(SOCKET sock, int size);
int SSO_SendBuffSize		(SOCKET sock, int size);
int SSO_ReuseAddress		(SOCKET sock, int bReuse = TRUE);
int SSO_UDP_ConnReset		(SOCKET sock, int bNewBehavior = TRUE);

/************************************************************************
���ƣ�Socket ��������
������Socket ������װ����
************************************************************************/

/* ��� IOCP ��������ֵ��NO_ERROR �򷵻� TRUE */
#define IOCP_NO_ERROR(result)	(result == NO_ERROR)
/* ��� IOCP ��������ֵ��WSA_IO_PENDING �򷵻� TRUE */
#define IOCP_PENDING(result)	(result == WSA_IO_PENDING)
/* ��� IOCP ��������ֵ��NO_ERROR �� WSA_IO_PENDING �򷵻� TRUE */
#define IOCP_SUCCESS(result)	(IOCP_NO_ERROR(result) || IOCP_PENDING(result))

/* ���� Connection ID */
CONNID GenerateConnectionID	();
/* �ر� Socket */
int ManualCloseSocket		(SOCKET sock, int iShutdownFlag = 0xFF, int bGraceful = TRUE, int bReuseAddress = FALSE);
/* Ͷ�� AccceptEx()������ WSA_IO_PENDING ת��Ϊ NO_ERROR */
int PostAccept				(LPFN_ACCEPTEX pfnAcceptEx, SOCKET soListen, SOCKET soClient, TBufferObj* pBufferObj, ADDRESS_FAMILY usFamily);
/* Ͷ�� AccceptEx() */
int PostAcceptNotCheck		(LPFN_ACCEPTEX pfnAcceptEx, SOCKET soListen, SOCKET soClient, TBufferObj* pBufferObj, ADDRESS_FAMILY usFamily);
/* Ͷ�� ConnectEx()������ WSA_IO_PENDING ת��Ϊ NO_ERROR */
int PostConnect				(LPFN_CONNECTEX pfnConnectEx, SOCKET soClient, const HP_SOCKADDR& sockAddr, TBufferObj* pBufferObj);
/* Ͷ�� ConnectEx() */
int PostConnectNotCheck		(LPFN_CONNECTEX pfnConnectEx, SOCKET soClient, const HP_SOCKADDR& sockAddr, TBufferObj* pBufferObj);
/* Ͷ�� WSASend()������ WSA_IO_PENDING ת��Ϊ NO_ERROR */
int PostSend				(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* Ͷ�� WSASend() */
int PostSendNotCheck		(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* Ͷ�� WSARecv()������ WSA_IO_PENDING ת��Ϊ NO_ERROR */
int PostReceive				(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* Ͷ�� WSARecv() */
int PostReceiveNotCheck		(TSocketObj* pSocketObj, TBufferObj* pBufferObj);
/* Ͷ�� WSASendTo()������ WSA_IO_PENDING ת��Ϊ NO_ERROR */
int PostSendTo				(SOCKET sock, TUdpBufferObj* pBufferObj);
/* Ͷ�� WSASendTo() */
int PostSendToNotCheck		(SOCKET sock, TUdpBufferObj* pBufferObj);
/* Ͷ�� WSARecvFrom()������ WSA_IO_PENDING ת��Ϊ NO_ERROR */
int PostReceiveFrom			(SOCKET sock, TUdpBufferObj* pBufferObj);
/* Ͷ�� WSARecvFrom() */
int PostReceiveFromNotCheck	(SOCKET sock, TUdpBufferObj* pBufferObj);
/* ִ�з����� WSARecv()������ WSAEWOULDBLOCK ת��Ϊ NO_ERROR */
int NoBlockReceive(TBufferObj* pBufferObj);
/* ִ�з����� WSARecv() */
int NoBlockReceiveNotCheck(TBufferObj* pBufferObj);

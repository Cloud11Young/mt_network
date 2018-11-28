#ifndef LISTENER_H_
#define LISTENER_H_

#include "../public/SocketInterface.h"
//#include "../public/CriticalSection.h"
#include "DataType.h"
#include "INetComm.h"
#include <deque>
#include <map>

class ServerListener :public CTcpServerListener{
public:
	ServerListener();
	virtual ~ServerListener();
	virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen);							
	virtual EnHandleResult OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient);				
	virtual EnHandleResult OnHandShake(ITcpServer* pSender, CONNID dwConnID);								
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength);
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);	
	virtual EnHandleResult OnShutdown(ITcpServer* pSender);	
	virtual EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
public:
	RemoteAddress* FindRemoteAddr(CONNID dwConnID);
	CONNID FindConnID(char* sIP, int IPLen, USHORT usPort);
	void DeleteRemoteAddr(CONNID dwConnID);	
	void RegCallBack(PUSER_CB callback);
//	BOOL StartDataThread();
private:
	void ExtractData(Buffer* pBuf, std::deque<Buffer*>* pComplete, CONNID dwConnID);
	void ExtractBuffer(std::deque<Buffer*>* pBufs, CONNID dwConnID);
//	static UINT WINAPI ReceiveThread(LPVOID pVoid);
private:
	std::map<CONNID, RemoteAddress*> m_vAddr;
	PUSER_CB m_pCallBack;
//	CInterCriSec m_extractlock;
};

class ClientListener :public CTcpClientListener{
public:
	ClientListener();
	virtual ~ClientListener();
	virtual EnHandleResult OnPrepareListen(ITcpClient* pSender, SOCKET soListen);							
	virtual EnHandleResult OnAccept(ITcpClient* pSender, CONNID dwConnID, UINT_PTR soClient);				
	virtual EnHandleResult OnHandShake(ITcpClient* pSender, CONNID dwConnID);								
	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, int iLength);
	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength);	
	virtual EnHandleResult OnShutdown(ITcpClient* pSender);	
	virtual EnHandleResult OnClose(ITcpClient* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode);
public:
	RemoteAddress* FindRemoteAddr(CONNID dwConnID);
	CONNID FindConnID(TCHAR* sIP, int IPLen, USHORT usPort);
	void DeleteRemoteAddr(CONNID dwConnID);	
	void RegCallBack(PUSER_CB callback);
//	BOOL StartDataThread();

private:
	void ExtractBuffer(std::deque<Buffer*>* pBufs, CONNID dwConnID);
	void ExtractData(Buffer* pBuf, std::deque<Buffer*>* pComplete, CONNID dwConnID);
//	static UINT WINAPI ReceiveThread(LPVOID pVoid);
private:
	std::map<CONNID, RemoteAddress*> m_vAddr;
	PUSER_CB m_pCallBack;
//	CInterCriSec m_extractlock;
	RemoteAddress* m_remoteaddr;
};

#endif
#include "SocketInterface.h"
#include "../../common/src/CriticalSection.h"


class ServerListener : public CTcpServerListener{
public:
	ServerListener();
	virtual ~ServerListener();
	virtual EnHandleResult OnPrepareListen(ITcpServer* pSender, SOCKET soListen);
	virtual EnHandleResult OnAccept(ITcpServer* pSender, CONNID dwConnID, UINT_PTR soClient);
	virtual EnHandleResult OnHandShake(ITcpServer* pSender, CONNID dwConnID);
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnSend(ITcpServer* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnShutdown(ITcpServer* pSender);
	virtual EnHandleResult OnClose(ITcpServer* pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode){ return HR_IGNORE; }
	virtual EnHandleResult OnReceive(ITcpServer* pSender, CONNID dwConnID, int iLength){ return HR_IGNORE; }
private:
// 	void Combination(std::queue<Buffer*>& vBuf);
// 	Buffer* ExtractData(Buffer* pBuf, std::queue<Buffer*>* pComplete);
// 	std::queue<Buffer*>* CreateBufferPool(CONNID dwConID);
// 	void ExtractBuffer(std::queue<Buffer*>* pBufs, CONNID ConID);
private:
// 	Buffer* m_pLeftBuf;
// 	std::queue<Buffer*> vTmpData;
// 	std::queue<Buffer*> vCombinData;
// 	CMTX m_extractlock;
};


class ClientListener : public CTcpClientListener{
public:
	virtual EnHandleResult OnPrepareListen(ITcpClient* pSender, SOCKET soListen);
	virtual EnHandleResult OnAccept(ITcpClient* pSender, CONNID dwConnID, UINT_PTR soClient);
	virtual EnHandleResult OnHandShake(ITcpClient* pSender, CONNID dwConnID);
	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, int iLength);
	virtual EnHandleResult OnSend(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength);
	virtual EnHandleResult OnShutdown(ITcpClient* pSender);
	virtual EnHandleResult OnReceive(ITcpClient* pSender, CONNID dwConnID, const BYTE* pData, int iLength){ return HR_IGNORE; }
};


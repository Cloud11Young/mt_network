#ifndef NET_HELPER_H_
#define NET_HELPER_H_

#include <winsock2.h>

class CInitWinSocket
{
public:
	CInitWinSocket(WSADATA* lpWSAData=NULL,short minorVersion=2,short majorVersion=2)
	{
		WSADATA* lpTemp = lpWSAData;
		if (lpTemp == NULL)
			lpTemp = new WSADATA;
		m_iResult = ::WSAStartup(MAKEWORD(majorVersion, minorVersion), lpTemp);
	}
	~CInitWinSocket()
	{
		if (m_iResult == 0)
			::WSACleanup();
	}
private:
	int m_iResult;
};

#endif
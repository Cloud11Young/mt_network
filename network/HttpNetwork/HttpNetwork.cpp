// HttpNetwork.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "HttpNetwork.h"
#include "CHttpClient.h"
#include "CHttpServer.h"

HTTP_API IHttpServer* MT::CreateHttpServer()
{
	return new CHttpServer();
}

HTTP_API IHttpClient* MT::CreateHttpClient()
{
	return new CHttpClient();
}

HTTP_API void MT::DestroyHttpServer(IHttpServer* pServer)
{
	delete pServer;
}

HTTP_API void MT::DestroyHttpClient(IHttpClient* pClient)
{
	delete pClient;
}

// test_network_client.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Ctest_network_clientApp: 
// �йش����ʵ�֣������ test_network_client.cpp
//

class Ctest_network_clientApp : public CWinApp
{
public:
	Ctest_network_clientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Ctest_network_clientApp theApp;
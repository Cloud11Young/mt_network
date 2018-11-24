
// test_network_srvDlg.h : 头文件
//

#pragma once

#include "INetComm.h"

// Ctest_network_srvDlg 对话框
class Ctest_network_srvDlg : public CDialogEx
{
// 构造
public:
	Ctest_network_srvDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TEST_NETWORK_SRV_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void ShowMessage(CString strIP, DWORD port, CString strPcName);
	void Disconnect(CString strIP, DWORD port);
	void RecvMsg(PVOID sMsg, DWORD dwMsgLen, CString strIP, DWORD port);
public:
	CListBox m_listCtrl;
private:
	INetComm* m_pNetwork;
	PUSER_CB pCallback;
};



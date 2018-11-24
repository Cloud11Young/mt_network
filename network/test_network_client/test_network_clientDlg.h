
// test_network_clientDlg.h : ͷ�ļ�
//

#pragma once

#include "../network/src/INetComm.h"
#include "afxwin.h"
// Ctest_network_clientDlg �Ի���
class Ctest_network_clientDlg : public CDialogEx
{
// ����
public:
	Ctest_network_clientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TEST_NETWORK_CLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	PUSER_CB m_callback;
	INetComm* m_pNetWork;

	CListBox m_listbox;
	CEdit m_conIP;
	CEdit m_conPort;
	CEdit m_SendData;
	CButton m_check;
	CButton m_chkloop;
	CButton m_btnCon;
	CButton m_btnStop;
	CButton m_btnSend;
public:
	DWORD GetPortIP(CString& strIP);
	void RecvMsg(LPVOID pMsg, DWORD dwMsgLen, CString strIP, DWORD dwPort);
	void ConnectMsg(CString strIP, DWORD port, CString strPcName);
	void DisConnectMsg(CString strIP, DWORD port);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
};

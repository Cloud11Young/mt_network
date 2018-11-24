
// test_network_srvDlg.h : ͷ�ļ�
//

#pragma once

#include "INetComm.h"

// Ctest_network_srvDlg �Ի���
class Ctest_network_srvDlg : public CDialogEx
{
// ����
public:
	Ctest_network_srvDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TEST_NETWORK_SRV_DIALOG };

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



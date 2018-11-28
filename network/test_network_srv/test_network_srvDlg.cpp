
// test_network_srvDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "test_network_srv.h"
#include "test_network_srvDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#pragma comment(lib,"../lib/network.lib")

static void CONNECT_CALLBACK(PVOID pThis, char* strIP, USHORT dwPort, char* strPcName);
static void DISCONNECT_CALLBACK(PVOID pThis, char* strIP, USHORT dwPort);
static void RECVMSG_CALLBACK(PVOID pThis, PVOID pMsg, DWORD dwMsgLen, char* strIP, USHORT dwPort);
static void PREAUTO_CONNECT_CALLBACK(PVOID pThis, char* strIP, USHORT dwPort);
static void POSTAUTO_CONNECT_CALLBACK(PVOID pThis, char* strIP, USHORT dwPort, BOOL bOK);

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Ctest_network_srvDlg �Ի���



Ctest_network_srvDlg::Ctest_network_srvDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Ctest_network_srvDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ctest_network_srvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
}

BEGIN_MESSAGE_MAP(Ctest_network_srvDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// Ctest_network_srvDlg ��Ϣ�������

BOOL Ctest_network_srvDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	pCallback = new USER_CB;
	pCallback->lpConnectCB = CONNECT_CALLBACK;
	
	pCallback->lpDisconnectCB = DISCONNECT_CALLBACK;
	pCallback->lpRecvMsgCB = RECVMSG_CALLBACK;
	pCallback->lpCallBackData = this;
// 	pCallback->lpPreAutoConnectCB;
// 	pCallback->lpPostAutoConnectCB;

	INetComm::CreateInstance(&m_pNetwork);
	BOOL bInit = m_pNetwork->Initialize(NULL, pCallback, 9900, "192.168.101.13");
	if (!bInit)
		MessageBox(L"network Initialize failed");
	else{
		m_listCtrl.InsertString(0, L"server start success");
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void Ctest_network_srvDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Ctest_network_srvDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Ctest_network_srvDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ctest_network_srvDlg::ShowMessage(CString strIP, DWORD port, CString strPcName){
	CString strMsg;
	strMsg.Format(L"server: IP: %s  port: %d from '%s' has connected", strIP, port, strPcName);
	int num = m_listCtrl.GetCount();
	m_listCtrl.InsertString(num, strMsg);
}

void Ctest_network_srvDlg::Disconnect(CString strIP, DWORD port){
	CString strMsg;
	strMsg.Format(L"server: IP: %s  port: %d disconnected", strIP, port);
	int num = m_listCtrl.GetCount();
	m_listCtrl.InsertString(num, strMsg);
}

void Ctest_network_srvDlg::RecvMsg(PVOID sMsg, DWORD dwMsgLen, CString strIP, DWORD port){
	CString strMsg;
	strMsg.Format(L"server:port:%d Msg:%s", port, sMsg);
	int num = m_listCtrl.GetCount();
	m_listCtrl.InsertString(num, strMsg);
	m_pNetwork->SendMsg(sMsg, dwMsgLen, CStringA(strIP).GetBuffer(), port);
}

void CONNECT_CALLBACK(PVOID pThis, char* strIP, USHORT dwPort, char* strPcName){
	Ctest_network_srvDlg* pDlg = (Ctest_network_srvDlg*)pThis;
	pDlg->ShowMessage(CString(strIP), dwPort, CString(strPcName));
}

void DISCONNECT_CALLBACK(PVOID pThis, char* strIP, USHORT dwPort){
	Ctest_network_srvDlg* pDlg = (Ctest_network_srvDlg*)pThis;
	pDlg->Disconnect(CString(strIP), dwPort);
}

void RECVMSG_CALLBACK(PVOID pThis, PVOID pMsg, DWORD dwMsgLen, char* strIP, USHORT dwPort){
	Ctest_network_srvDlg* pDlg = (Ctest_network_srvDlg*)pThis;
// 	TCHAR* pBuf = new TCHAR[dwMsgLen/sizeof(TCHAR)];
// 	memcpy(pBuf, pMsg, dwMsgLen);
	pDlg->RecvMsg(pMsg, dwMsgLen, CString(strIP), dwPort);
}

void PREAUTO_CONNECT_CALLBACK(PVOID pThis, char* strIP, USHORT dwPort){

}

void POSTAUTO_CONNECT_CALLBACK(PVOID pThis, char* strIP, USHORT dwPort, BOOL bOK){

}
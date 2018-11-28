
// test_network_clientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "test_network_client.h"
#include "test_network_clientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#pragma comment(lib,"../lib/network_d.lib")

static void CONNECT_CALLBACK(void* pThis, char* strIP, USHORT dwPort, char* strPcName);
static void DISCONNECT_CALLBACK(void* pThis, char* strIP, USHORT dwPort);
static void RECVMSG_CALLBACK(void* pThis, void* pMsg, DWORD dwMsgLen, char* strIP, USHORT dwPort);
static void PREAUTO_CONNECT_CALLBACK(void* pThis, char* strIP, USHORT dwPort);
static void POSTAUTO_CONNECT_CALLBACK(void* pThis, char* strIP, USHORT dwPort, BOOL bOK);

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


// Ctest_network_clientDlg �Ի���



Ctest_network_clientDlg::Ctest_network_clientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Ctest_network_clientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ctest_network_clientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listbox);
	DDX_Control(pDX, IDC_EDIT1, m_conIP);
	DDX_Control(pDX, IDC_EDIT2, m_conPort);
	DDX_Control(pDX, IDC_EDIT3, m_SendData);
	DDX_Control(pDX, IDC_BUTTON1, m_btnCon);
	DDX_Control(pDX, IDC_BUTTON2, m_btnStop);
	DDX_Control(pDX, IDC_BUTTON3, m_btnSend);
	DDX_Control(pDX, IDC_CHECK1, m_check);
	DDX_Control(pDX, IDC_CHECK2, m_chkloop);
}

BEGIN_MESSAGE_MAP(Ctest_network_clientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &Ctest_network_clientDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &Ctest_network_clientDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &Ctest_network_clientDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


// Ctest_network_clientDlg ��Ϣ�������

BOOL Ctest_network_clientDlg::OnInitDialog()
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
	m_callback = new USER_CB;
	m_callback->lpConnectCB = CONNECT_CALLBACK;
	m_callback->lpDisconnectCB = DISCONNECT_CALLBACK;
	m_callback->lpRecvMsgCB = RECVMSG_CALLBACK;
	m_callback->lpCallBackData = this;

	INetComm::CreateInstance(&m_pNetWork);
	BOOL bInit = m_pNetWork->Initialize(NULL, m_callback);
	if (bInit)
		m_listbox.InsertString(0, L"client start success");
	else
		m_listbox.InsertString(0, L"client start failed");
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void Ctest_network_clientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Ctest_network_clientDlg::OnPaint()
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
HCURSOR Ctest_network_clientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CONNECT_CALLBACK(void* pThis, char* strIP, USHORT dwPort, char* strPcName){
	Ctest_network_clientDlg* pDlg = (Ctest_network_clientDlg*)pThis;
	pDlg->ConnectMsg(CString(strIP), dwPort, CString(strPcName));
}

void DISCONNECT_CALLBACK(void* pThis, char* strIP, USHORT dwPort){
	Ctest_network_clientDlg* pDlg = (Ctest_network_clientDlg*)pThis;
	pDlg->DisConnectMsg(CString(strIP), dwPort);
}

void RECVMSG_CALLBACK(void* pThis, void* pMsg, DWORD dwMsgLen, char* strIP, USHORT dwPort){
	Ctest_network_clientDlg* pDlg = (Ctest_network_clientDlg*)pThis;
	pDlg->RecvMsg(pMsg, dwMsgLen, CString(strIP), dwPort);
}

void PREAUTO_CONNECT_CALLBACK(void* pThis, char* strIP, USHORT dwPort){

}

void POSTAUTO_CONNECT_CALLBACK(void* pThis, char* strIP, USHORT dwPort, BOOL bOK){

}

void Ctest_network_clientDlg::OnBnClickedButton1()
{
	CString strIP;	
	DWORD port = GetPortIP(strIP);

	BOOL bCon = m_pNetWork->ConnectTo(CStringA(strIP).GetBuffer(), port);
	
// 	int num = m_listbox.GetCount();
// 	if (bCon)		
// 		m_listbox.InsertString(num, L"client: connect to server success");
// 	else
// 		m_listbox.InsertString(num, L"client: connect to server failed");
}


void Ctest_network_clientDlg::OnBnClickedButton2()
{
	m_pNetWork->Uninitialize();
}


void Ctest_network_clientDlg::OnBnClickedButton3()
{
	BOOL bSend=FALSE;
	if (m_check.GetCheck()){
		CString strIP;
		DWORD port = GetPortIP(strIP);

		CString filename;
		m_SendData.GetWindowText(filename);

		CStdioFile file;
		file.Open(filename, CFile::modeRead);
		CString txt;
		while (file.ReadString(txt)){
			bSend = m_pNetWork->SendMsg(txt.GetBuffer(), (txt.GetLength()+1)*sizeof(TCHAR), CStringA(strIP).GetBuffer(), port);
		}
		file.Close();
// 		CStringA filenameA(filename);
// 		FILE* pFile = fopen(filenameA.GetBuffer(), "r");
		
// 		int n = 0;
// 		char buf[1024] = { 0 };
// 		while (fgets(buf,1024,pFile) != NULL){
// 			m_pNetWork->SendMsg(buf, n, strIP, port);
// 			memset(buf, 0, n);
// 		}
		
	}
	else if (m_chkloop.GetCheck()){
		CString txt;
		m_SendData.GetWindowText(txt);
		for (int i = 0; i < 3000000; i++){
			CString sNo;
			sNo.Format(L"%d: %s", i, txt);
			bSend = m_pNetWork->SendMsg(sNo.GetBuffer(), (sNo.GetLength() + 1)*sizeof(TCHAR), "", 0);
		}
	}
	else{
		CString strData,strIP;
		m_SendData.GetWindowText(strData);
		DWORD port = GetPortIP(strIP);
		bSend=m_pNetWork->SendMsg(strData.GetBuffer(), sizeof(TCHAR) * (strData.GetLength()+1), CStringA(strIP).GetBuffer(), port);
	}
	if (!bSend){
		m_listbox.InsertString(m_listbox.GetCount(), L"send failed");
	}
}


DWORD Ctest_network_clientDlg::GetPortIP(CString& strIP){
	CString strPort;
	m_conIP.GetWindowText(strIP);
	m_conPort.GetWindowText(strPort);
	DWORD port = _ttoi(strPort.GetBuffer());
	return port;
}

void Ctest_network_clientDlg::RecvMsg(LPVOID pMsg, DWORD dwMsgLen, CString strIP, USHORT dwPort){
	TCHAR* pBuf = new TCHAR[dwMsgLen / sizeof(TCHAR)];
	memcpy(pBuf, pMsg, dwMsgLen);
	CString msg;
	msg.Format(L"recv from %s : %s", strIP, pBuf);
	m_listbox.InsertString(m_listbox.GetCount(), msg);
}

void Ctest_network_clientDlg::ConnectMsg(CString strIP, USHORT port, CString strPcName){
	CString msg;
	msg.Format(L"connected %s:%d name=%s", strIP, port, strPcName);
	m_listbox.InsertString(m_listbox.GetCount(), msg);
}

void Ctest_network_clientDlg::DisConnectMsg(CString strIP, USHORT port){
	CString msg;
	msg.Format(L"disconnected %s:%d", strIP, port);
	m_listbox.InsertString(m_listbox.GetCount(), msg);
}
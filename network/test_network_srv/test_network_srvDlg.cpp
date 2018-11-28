
// test_network_srvDlg.cpp : 实现文件
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

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// Ctest_network_srvDlg 对话框



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


// Ctest_network_srvDlg 消息处理程序

BOOL Ctest_network_srvDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Ctest_network_srvDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
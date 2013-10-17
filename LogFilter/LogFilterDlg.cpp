
// LogFilterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LogFilter.h"
#include "LogFilterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CLogFilterDlg 对话框



CLogFilterDlg::CLogFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLogFilterDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLogFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_CTRL, m_ctrlTab);
}

BEGIN_MESSAGE_MAP(CLogFilterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CTRL, &CLogFilterDlg::OnTcnSelchangeTabCtrl)
END_MESSAGE_MAP()


// CLogFilterDlg 消息处理程序

BOOL CLogFilterDlg::OnInitDialog()
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//ShowWindow(SW_MAXIMIZE);

	// TODO: 在此添加额外的初始化代码
	m_ctrlTab.InsertItem(0,_T("日志分析"));  //添加“日志分析”选项卡    
	m_ctrlTab.InsertItem(1,_T("日志搜索"));  //添加“日志搜索”选项卡

	m_dlgLogFilter.Create(IDD_DLG_ANALYZE,GetDlgItem(IDC_TAB_CTRL));
	m_dlgLogSearch.Create(IDD_DLG_LOGSEARCH,GetDlgItem(IDC_TAB_CTRL));
	//获得IDC_TABTEST客户区大小    
	CRect rs;    
	m_ctrlTab.GetClientRect(&rs);    
	//调整子对话框在父窗口中的位置    
	rs.top+=23;     
 	rs.bottom-=5;     
 	rs.left+=1;     
 	rs.right-=5;     

	//设置子对话框尺寸并移动到指定位置    
	m_dlgLogFilter.MoveWindow(&rs);
	m_dlgLogSearch.MoveWindow(&rs);

	//分别设置隐藏和显示    
	m_dlgLogFilter.ShowWindow(true);
	m_dlgLogSearch.ShowWindow(false);

	//设置默认的选项卡    
	m_ctrlTab.SetCurSel(0);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CLogFilterDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLogFilterDlg::OnPaint()
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
HCURSOR CLogFilterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CLogFilterDlg::OnTcnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	int nCurSel = m_ctrlTab.GetCurSel();
	switch (nCurSel)
	{
	case 0:
		m_dlgLogFilter.ShowWindow(true);
		m_dlgLogSearch.ShowWindow(false);
		break;
	case 1:
		m_dlgLogFilter.ShowWindow(false);
		m_dlgLogSearch.ShowWindow(true);
		break;
	default:
		break;
	}

	*pResult = 0;
}


BOOL CLogFilterDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_RETURN||pMsg->wParam==VK_ESCAPE)
			return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

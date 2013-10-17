
// LogFilterDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LogFilter.h"
#include "LogFilterDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CLogFilterDlg �Ի���



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


// CLogFilterDlg ��Ϣ�������

BOOL CLogFilterDlg::OnInitDialog()
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//ShowWindow(SW_MAXIMIZE);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_ctrlTab.InsertItem(0,_T("��־����"));  //��ӡ���־������ѡ�    
	m_ctrlTab.InsertItem(1,_T("��־����"));  //��ӡ���־������ѡ�

	m_dlgLogFilter.Create(IDD_DLG_ANALYZE,GetDlgItem(IDC_TAB_CTRL));
	m_dlgLogSearch.Create(IDD_DLG_LOGSEARCH,GetDlgItem(IDC_TAB_CTRL));
	//���IDC_TABTEST�ͻ�����С    
	CRect rs;    
	m_ctrlTab.GetClientRect(&rs);    
	//�����ӶԻ����ڸ������е�λ��    
	rs.top+=23;     
 	rs.bottom-=5;     
 	rs.left+=1;     
 	rs.right-=5;     

	//�����ӶԻ���ߴ粢�ƶ���ָ��λ��    
	m_dlgLogFilter.MoveWindow(&rs);
	m_dlgLogSearch.MoveWindow(&rs);

	//�ֱ��������غ���ʾ    
	m_dlgLogFilter.ShowWindow(true);
	m_dlgLogSearch.ShowWindow(false);

	//����Ĭ�ϵ�ѡ�    
	m_ctrlTab.SetCurSel(0);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLogFilterDlg::OnPaint()
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
HCURSOR CLogFilterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CLogFilterDlg::OnTcnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_RETURN||pMsg->wParam==VK_ESCAPE)
			return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

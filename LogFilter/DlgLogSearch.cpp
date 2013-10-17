// DlgLogSearch.cpp : 实现文件
//

#include "stdafx.h"
#include "LogFilter.h"
#include "DlgLogSearch.h"
#include "afxdialogex.h"


// CDlgLogSearch 对话框

IMPLEMENT_DYNAMIC(CDlgLogSearch, CDialogEx)

CDlgLogSearch::CDlgLogSearch(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgLogSearch::IDD, pParent)
{

}

CDlgLogSearch::~CDlgLogSearch()
{
}

void CDlgLogSearch::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgLogSearch, CDialogEx)
END_MESSAGE_MAP()


// CDlgLogSearch 消息处理程序

// DlgLogSearch.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LogFilter.h"
#include "DlgLogSearch.h"
#include "afxdialogex.h"


// CDlgLogSearch �Ի���

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


// CDlgLogSearch ��Ϣ�������

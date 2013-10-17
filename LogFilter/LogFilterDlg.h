
// LogFilterDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "DlgLogFilter.h"
#include "DlgLogSearch.h"


// CLogFilterDlg �Ի���
class CLogFilterDlg : public CDialogEx
{
// ����
public:
	CLogFilterDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_LOGFILTER_DIALOG };

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
	CTabCtrl m_ctrlTab;
	CDlgLogFilter m_dlgLogFilter;
	CDlgLogSearch m_dlgLogSearch;
	afx_msg void OnTcnSelchangeTabCtrl(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

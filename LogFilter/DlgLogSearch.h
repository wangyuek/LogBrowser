#pragma once


// CDlgLogSearch �Ի���

class CDlgLogSearch : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLogSearch)

public:
	CDlgLogSearch(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLogSearch();

// �Ի�������
	enum { IDD = IDD_DLG_LOGSEARCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};

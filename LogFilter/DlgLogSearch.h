#pragma once


// CDlgLogSearch 对话框

class CDlgLogSearch : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLogSearch)

public:
	CDlgLogSearch(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLogSearch();

// 对话框数据
	enum { IDD = IDD_DLG_LOGSEARCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};

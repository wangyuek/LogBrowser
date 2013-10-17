#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "../myHead.h"

// CDlgLogFilter 对话框

class CDlgLogFilter : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLogFilter)

public:
	CDlgLogFilter(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgLogFilter();

// 对话框数据
	enum { IDD = IDD_DLG_ANALYZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	LogFile	m_objLogFile;	//LogFile结构体，存放打开的日志文件名称、数量和路径
	LogPattern m_objLogPattern;	//LogPattern结构体，存放配置文件中关于log文件的特征值
	LogField m_objLogField;		//LogField结构体，存放所有field信息
	CString m_strScriptPath;	//配置脚本路径
	CString m_strLogDirPath;	//日志目录路径
	CString m_strUsePrefix;	//是否使用前缀
	CString m_strFixName;	//前缀或者后缀名称
	CString m_strDelimiter;	//日志字段分隔符
	CString m_strRecSep;	//日志record分隔符
	CComboBox m_ctrlCondition;	//过滤条件
	CString m_strConditionVal;	//过滤条件值
	CListCtrl m_ctrlLogList;	//日志内容list
	CListCtrl m_ctrlFilterList;	//过滤条件list
	FilterCondition m_objFilterCondition;	//过滤条件structure，记录所有的过滤条件
	//////////////////////////////////////////////////////////////////////////
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnDir();
	afx_msg void OnBnClickedBtnAdd();
	void UpdateLogContentListControl(CString *pstrRec,DWORD dwRecNum);
	CString m_strConditionName;
	afx_msg void OnBnClickedBtnExecute();
	afx_msg void OnBnClickedBtnDelcon();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnDelteall();
	afx_msg void OnBnClickedBtnScript();
	void InitControl(void);
};

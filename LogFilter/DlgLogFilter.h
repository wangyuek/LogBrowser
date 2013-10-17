#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "../myHead.h"

// CDlgLogFilter �Ի���

class CDlgLogFilter : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLogFilter)

public:
	CDlgLogFilter(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgLogFilter();

// �Ի�������
	enum { IDD = IDD_DLG_ANALYZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	LogFile	m_objLogFile;	//LogFile�ṹ�壬��Ŵ򿪵���־�ļ����ơ�������·��
	LogPattern m_objLogPattern;	//LogPattern�ṹ�壬��������ļ��й���log�ļ�������ֵ
	LogField m_objLogField;		//LogField�ṹ�壬�������field��Ϣ
	CString m_strScriptPath;	//���ýű�·��
	CString m_strLogDirPath;	//��־Ŀ¼·��
	CString m_strUsePrefix;	//�Ƿ�ʹ��ǰ׺
	CString m_strFixName;	//ǰ׺���ߺ�׺����
	CString m_strDelimiter;	//��־�ֶηָ���
	CString m_strRecSep;	//��־record�ָ���
	CComboBox m_ctrlCondition;	//��������
	CString m_strConditionVal;	//��������ֵ
	CListCtrl m_ctrlLogList;	//��־����list
	CListCtrl m_ctrlFilterList;	//��������list
	FilterCondition m_objFilterCondition;	//��������structure����¼���еĹ�������
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

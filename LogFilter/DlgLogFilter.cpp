// DlgLogFilter.cpp : 实现文件
//

#include "stdafx.h"
#include "LogFilter.h"
#include "DlgLogFilter.h"
#include "afxdialogex.h"

extern CRITICAL_SECTION g_cs;
// CDlgLogFilter 对话框

IMPLEMENT_DYNAMIC(CDlgLogFilter, CDialogEx)

CDlgLogFilter::CDlgLogFilter(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlgLogFilter::IDD, pParent)
	, m_strScriptPath(_T(""))
	, m_strLogDirPath(_T(""))
	, m_strUsePrefix(_T(""))
	, m_strFixName(_T(""))
	, m_strDelimiter(_T(""))
	, m_strConditionVal(_T(""))
	, m_strConditionName(_T(""))
{
	//初始化m_objLogFile成员变量
	m_objLogFile.m_nFileNum = 0;
	m_objLogFile.m_dwTotalBufSize = 0;
	m_objLogFile.m_dwRecNum = 0;
	m_objLogFile.m_strLogDirPath.Empty();
	m_objLogFile.m_pdwFileSizeArray=NULL;
	m_objLogFile.m_pHandleArray=NULL;
	m_objLogFile.m_pstrLogName=NULL;
	m_objLogFile.m_pTotalBuf=NULL;
	m_objLogFile.m_pstrRec=NULL;
	//初始化m_objFilterCondition成员变量
	m_objFilterCondition.m_nConditionNum =0;
	m_objFilterCondition.m_nArrayLen = 0;
	m_objFilterCondition.m_pstrConditionName = NULL;
	m_objFilterCondition.m_pstrConditionValue = NULL;
	//////////////////////////////////////////////////////////////////////////
	InitializeCriticalSection(&g_cs);
}

CDlgLogFilter::~CDlgLogFilter()
{
}

void CDlgLogFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SCRIPT, m_strScriptPath);
	DDX_Text(pDX, IDC_EDIT_DIR, m_strLogDirPath);
	DDX_Text(pDX, IDC_EDIT_FIX, m_strUsePrefix);
	DDX_Text(pDX, IDC_EDIT_FIXVALUE, m_strFixName);
	DDX_Text(pDX, IDC_EDIT_DELIMITER, m_strDelimiter);
	DDX_Control(pDX, IDC_COMBO_CONDITION, m_ctrlCondition);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_strConditionVal);
	DDX_Control(pDX, IDC_LIST_LOG, m_ctrlLogList);
	DDX_Control(pDX, IDC_LIST_CONDITION, m_ctrlFilterList);
	DDX_CBString(pDX, IDC_COMBO_CONDITION, m_strConditionName);
}


BEGIN_MESSAGE_MAP(CDlgLogFilter, CDialogEx)

	ON_BN_CLICKED(IDC_BTN_DIR, &CDlgLogFilter::OnBnClickedBtnDir)
	ON_BN_CLICKED(IDC_BTN_ADD, &CDlgLogFilter::OnBnClickedBtnAdd)
	ON_BN_CLICKED(IDC_BTN_EXECUTE, &CDlgLogFilter::OnBnClickedBtnExecute)
	ON_BN_CLICKED(IDC_BTN_DELCON, &CDlgLogFilter::OnBnClickedBtnDelcon)
	ON_BN_CLICKED(IDC_BTN_DELTEALL, &CDlgLogFilter::OnBnClickedBtnDelteall)
	ON_BN_CLICKED(IDC_BTN_SCRIPT, &CDlgLogFilter::OnBnClickedBtnScript)
END_MESSAGE_MAP()


// CDlgLogFilter 消息处理程序


BOOL CDlgLogFilter::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//读取配置文件，根据配置文件内容初始化界面控件
	TCHAR tRetStr[200];//读取的字符串
	const DWORD dwLen=200;//读取的字符串长度
	if(GetPrivateProfileString(_T("CONFIG"),_T("script_file"),NULL,tRetStr,dwLen,_T("./config.ini")))
	{
		m_strScriptPath = tRetStr;

		GetPrivateProfileString(_T("CONFIG"),_T("log_dir"),NULL,tRetStr,dwLen,_T("./config.ini"));
		m_strLogDirPath = tRetStr;

		//[LOGFILE]
		GetPrivateProfileString(_T("LOGFILE"),_T("use_prefix"),NULL,tRetStr,dwLen,_T("./config.ini"));
		m_strUsePrefix = tRetStr;
		m_objLogPattern.m_strUsePrefix = tRetStr;
		if (!m_strUsePrefix.Compare(_T("1")))
		{
			m_strUsePrefix = _T("前缀");
		} 
		else
		{
			m_strUsePrefix = _T("后缀");
		}

		GetPrivateProfileString(_T("LOGFILE"),_T("prefix"),NULL,tRetStr,dwLen,_T("./config.ini"));
		m_strFixName = tRetStr;
		m_objLogPattern.m_strPrefix = tRetStr;
		if (m_strFixName.IsEmpty())
		{
			GetPrivateProfileString(_T("LOGFILE"),_T("postfix"),NULL,tRetStr,dwLen,_T("./config.ini"));
			m_strFixName = tRetStr;
			m_objLogPattern.m_strPostfix = tRetStr;
		} 

		GetPrivateProfileString(_T("LOGFILE"),_T("delimiter"),NULL,tRetStr,dwLen,_T("./config.ini"));
		m_strDelimiter = tRetStr;
		m_objLogPattern.m_strDelimiter = tRetStr;

		GetPrivateProfileString(_T("LOGFILE"),_T("rec_sep"),NULL,tRetStr,dwLen,_T("./config.ini"));
		m_strRecSep = tRetStr;
		if (!m_strRecSep.CompareNoCase(_T("crlf")))
		{
			m_strRecSep = _T("\r\n");
		}
		else if (!m_strRecSep.CompareNoCase(_T("lf")))
		{
			m_strRecSep = _T("\n");
		}
		m_objLogPattern.m_strRecSep = m_strRecSep;

		//[FIELD]
		m_objLogField.m_nTotal = GetPrivateProfileInt(_T("FIELD"),_T("amount"),NULL,_T("./config.ini"));
		m_objLogField.m_pstrFieldItem = new CString[m_objLogField.m_nTotal];
		CString strTempKeyName;//模板KeyName
		for (int i=1;i<=m_objLogField.m_nTotal;i++)
		{
			strTempKeyName.Format(_T("field%d"),i);
			GetPrivateProfileString(_T("FIELD"),strTempKeyName,NULL,tRetStr,dwLen,_T("./config.ini"));
			m_objLogField.m_pstrFieldItem[i-1] = tRetStr;
		}

		//初始化“条件”ComboBox和“日志内容”ListControl控件
		m_ctrlLogList.InsertColumn(0,_T("ID"),LVCFMT_CENTER,30);
		CString strTemp;
		for (int i=1;i<=m_objLogField.m_nTotal;i++)
		{
			strTemp.Format(_T("%d."),i);
			strTemp += m_objLogField.m_pstrFieldItem[i-1];
			m_ctrlCondition.AddString(strTemp);
			m_ctrlLogList.InsertColumn(i,m_objLogField.m_pstrFieldItem[i-1],LVCFMT_CENTER,85);
		}
		DWORD dwStyle = m_ctrlLogList.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
		dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
		m_ctrlLogList.SetExtendedStyle(dwStyle); //设置扩展风格

		//初始化m_ctrlFilterList ListControl控件
		m_ctrlFilterList.InsertColumn(0,_T("ID"),LVCFMT_CENTER,30);
		m_ctrlFilterList.InsertColumn(1,_T("条件"),LVCFMT_CENTER,130);
		m_ctrlFilterList.InsertColumn(2,_T("条件值"),LVCFMT_CENTER,220);

		dwStyle = m_ctrlFilterList.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
		dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
		m_ctrlFilterList.SetExtendedStyle(dwStyle); //设置扩展风格
		//////////////////////////////////////////////////////////////////////////
		UpdateData(false);
	}
	else
	{
		AfxMessageBox(_T("当前目录下找不到默认配置文件！"));
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//导入日志文件
void CDlgLogFilter::OnBnClickedBtnDir()
{
	// TODO: 在此添加控件通知处理程序代码
	//重置m_objLogFile变量，释放之前开辟的内存
	if(m_objLogFile.m_pdwFileSizeArray)
	{
		delete []m_objLogFile.m_pdwFileSizeArray;
		m_objLogFile.m_pdwFileSizeArray=NULL;
	}
	if (m_objLogFile.m_pHandleArray)
	{
		//关闭所有打开的文件句柄
		for (int i=0;i<m_objLogFile.m_nFileNum;i++)
			::CloseHandle(m_objLogFile.m_pHandleArray[i]);
		
		delete []m_objLogFile.m_pHandleArray;
		m_objLogFile.m_pHandleArray=NULL;
	}
	if (m_objLogFile.m_pstrLogName)
	{
		delete []m_objLogFile.m_pstrLogName;
		m_objLogFile.m_pstrLogName=NULL;
	}
	if (m_objLogFile.m_pTotalBuf)
	{
		delete []m_objLogFile.m_pTotalBuf;
		m_objLogFile.m_pTotalBuf=NULL;
	}
	if (m_objLogFile.m_pstrRec)
	{
		delete []m_objLogFile.m_pstrRec;
		m_objLogFile.m_pstrRec=NULL;
	}
	m_objLogFile.m_nFileNum = 0;
	m_objLogFile.m_dwTotalBufSize = 0;
	m_objLogFile.m_dwRecNum = 0;
	m_objLogFile.m_strLogDirPath.Empty();
	//////////////////////////////////////////////////////////////////////////
	TCHAR szPath[MAX_PATH]=_T("");     //存放选择的目录路径 

	BROWSEINFO bi;   
	bi.hwndOwner = m_hWnd;   
	bi.pidlRoot = NULL;   
	bi.pszDisplayName = szPath;   
	bi.lpszTitle = _T("请选择日志目录：");   
	bi.ulFlags = 0;   
	bi.lpfn = NULL;   
	bi.lParam = 0;   
	bi.iImage = 0;   
	//弹出选择目录对话框
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);   

	if(lp && SHGetPathFromIDList(lp, szPath))
	{
		m_strLogDirPath = szPath;
		//如果取到的路径最后一个字符不为'\'，添加一个'\'
		if (m_strLogDirPath.Right(1).Compare(_T("\\")))
		{
			m_strLogDirPath += _T("\\");
		}
		m_objLogFile.m_strLogDirPath = m_strLogDirPath;
		//清空日志内容listcontrol中的内容
		m_ctrlLogList.DeleteAllItems();
		UpdateData(FALSE);
		//查找指定目录下的所有匹配文件，并将文件内容读到m_objLogFile.m_pdwFileSizeArray开辟的buffer中
		FindAndRead(&m_objLogFile,m_objLogPattern.m_strUsePrefix,m_strFixName);
		//生成record的二维数组，并将buffer内容添加到二维数组中
		FillRecordArray(&m_objLogFile,&m_objLogField,&m_objLogPattern);
		//填充到“日志内容”List Control控件中
		this->UpdateLogContentListControl(m_objLogFile.m_pstrRec,m_objLogFile.m_dwRecNum);
	}
	else   
		AfxMessageBox(_T("无效的目录，请重新选择"));   
}

//添加过滤条件
void CDlgLogFilter::OnBnClickedBtnAdd()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ctrlFilterList.DeleteAllItems();
	UpdateData();
	//如果过滤条件>=当前设置的条件，增大动态数组
	if (m_objFilterCondition.m_nConditionNum >= m_objFilterCondition.m_nArrayLen)
	{
		//如果第一次添加过滤条件，则创建CString数组，粒度为5
		if (!(m_objFilterCondition.m_pstrConditionName || m_objFilterCondition.m_pstrConditionValue))
		{
			m_objFilterCondition.m_pstrConditionName = new CString[5];
			m_objFilterCondition.m_pstrConditionValue = new CString[5];
			m_objFilterCondition.m_nArrayLen = 5;
		}
		else
		{
			//将动态数组增大，粒度为5，将原有条件放入新的数组，删除原有开辟的内存空间
			m_objFilterCondition.m_nArrayLen += 5;
			CString *pstrTemp1 = new CString[m_objFilterCondition.m_nArrayLen];
			CString *pstrTemp2 = new CString[m_objFilterCondition.m_nArrayLen];
			for (int i=0;i<m_objFilterCondition.m_nArrayLen-5;i++)
			{
				pstrTemp1[i] = m_objFilterCondition.m_pstrConditionName[i];
				pstrTemp2[i] = m_objFilterCondition.m_pstrConditionValue[i];
			}
			delete []m_objFilterCondition.m_pstrConditionName;
			delete []m_objFilterCondition.m_pstrConditionValue;

			m_objFilterCondition.m_pstrConditionName = pstrTemp1;
			m_objFilterCondition.m_pstrConditionValue = pstrTemp2;
		}
	}
	
	//将新增的条件放入到动态数组中
	m_objFilterCondition.m_pstrConditionName[m_objFilterCondition.m_nConditionNum]=m_strConditionName;
	m_objFilterCondition.m_pstrConditionValue[m_objFilterCondition.m_nConditionNum]=m_strConditionVal;
	m_objFilterCondition.m_nConditionNum++;

	//在“条件”List Control控件中显示所有条件
	CString strRowNo;
	for (int i=0;i<m_objFilterCondition.m_nConditionNum;i++)
	{
		strRowNo.Format(_T("%d"),i);
		m_ctrlFilterList.InsertItem(i,strRowNo);
		m_ctrlFilterList.SetItemText(i,1,m_objFilterCondition.m_pstrConditionName[i]);
		m_ctrlFilterList.SetItemText(i,2,m_objFilterCondition.m_pstrConditionValue[i]);
	}
}

//更新“日志内容”List Control控件显示的内容
void CDlgLogFilter::UpdateLogContentListControl(CString *pstrRec,DWORD dwRecNum)
{
	CString strRowNo;
	for (int nRow=0;nRow<(int)dwRecNum;nRow++)
	{
		strRowNo.Format(_T("%d"),nRow);
		m_ctrlLogList.InsertItem(nRow,strRowNo);
		for (int nColumn=1;nColumn<=m_objLogField.m_nTotal;nColumn++)
		{
			m_ctrlLogList.SetItemText(nRow,nColumn,pstrRec[nRow*m_objLogField.m_nTotal+nColumn-1]);
		}
	}
}

//根据设定的查询条件对结果进行查询
void CDlgLogFilter::OnBnClickedBtnExecute()
{
	// TODO: 在此添加控件通知处理程序代码
	//遍历所有字段，根据所有查询条件，将符合条件的record插入“日志内容”List Control控件显示内容
	if (m_objFilterCondition.m_nConditionNum == 0)
	{
		//如果没有设置过滤条件，直接返回
		return;
	}
	m_ctrlLogList.DeleteAllItems();
	CString *pstrRowNo=NULL;
 	//已经添加的过滤条件在record中的下标
	int nColumn = 0;
	CString strColumn;
	//符合条件的record数量
	int nMeetNum=0;
	for (int nRow=0;nRow<(int)(m_objLogFile.m_dwRecNum);nRow++)
	{
		pstrRowNo = m_objLogFile.m_pstrRec + nRow*m_objLogField.m_nTotal;
		for (int i=0;i<m_objFilterCondition.m_nConditionNum;i++)
		{
			strColumn = m_objFilterCondition.m_pstrConditionName[i].Left(m_objFilterCondition.m_pstrConditionName[i].Find(_T(".")));
			nColumn = atoi(strColumn);
			if (-1 == pstrRowNo[nColumn - 1].Find(m_objFilterCondition.m_pstrConditionValue[i]))
			{
				//如果此列不符合过滤条件
				break;
			}
			else if ( i == m_objFilterCondition.m_nConditionNum - 1 )
			{
				CString strRowNo;
				strRowNo.Format(_T("%d"),nMeetNum);
				m_ctrlLogList.InsertItem(nMeetNum,strRowNo);
				for (int j=0;j<m_objLogField.m_nTotal;j++)
				{
					m_ctrlLogList.SetItemText(nMeetNum,j+1,pstrRowNo[j]);
				}
				nMeetNum++;
			}
		}
	}
}


void CDlgLogFilter::OnBnClickedBtnDelcon()
{
	// TODO: 在此添加控件通知处理程序代码
	for(int i=0; i<m_ctrlFilterList.GetItemCount(); i++)
	{
		if( m_ctrlFilterList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED )
		{
			//选中的第i行之后的"条件"前移,如果只有一个过滤条件，直接将m_objFilterCondition.m_nConditionNum--
			if (1 == m_objFilterCondition.m_nConditionNum)
			{
				m_objFilterCondition.m_nConditionNum--;
			}
			else
			{
				for (int j=0;j<m_objFilterCondition.m_nConditionNum-(i+1);j++)
				{
					m_objFilterCondition.m_pstrConditionName[i+j]=m_objFilterCondition.m_pstrConditionName[i+j+1];
					m_objFilterCondition.m_pstrConditionValue[i+j]=m_objFilterCondition.m_pstrConditionValue[i+j+1];
				}
				m_objFilterCondition.m_nConditionNum--;
				if (i>0)
				{
					i--;
				}
				
				m_ctrlFilterList.DeleteItem(i);
			}
		}
	}

	m_ctrlFilterList.DeleteAllItems();
	//在“条件”List Control控件中显示所有条件
	CString strRowNo;
	for (int i=0;i<m_objFilterCondition.m_nConditionNum;i++)
	{
		strRowNo.Format(_T("%d"),i);
		m_ctrlFilterList.InsertItem(i,strRowNo);
		m_ctrlFilterList.SetItemText(i,1,m_objFilterCondition.m_pstrConditionName[i]);
		m_ctrlFilterList.SetItemText(i,2,m_objFilterCondition.m_pstrConditionValue[i]);
	}
}


BOOL CDlgLogFilter::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_RETURN||pMsg->wParam==VK_ESCAPE)
			return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDlgLogFilter::OnBnClickedBtnDelteall()
{
	// TODO: 在此添加控件通知处理程序代码
	m_objFilterCondition.m_nConditionNum = 0;
	m_ctrlFilterList.DeleteAllItems();
}


void CDlgLogFilter::OnBnClickedBtnScript()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE);
	if (IDOK == dlg.DoModal())
	{
		m_strScriptPath = dlg.GetPathName();
		WritePrivateProfileString(_T("CONFIG"),_T("script_file"),m_strScriptPath.GetBuffer(m_strScriptPath.GetLength()),_T("./config.ini"));
		InitControl();
	}
	else
	{

	}
}


void CDlgLogFilter::InitControl(void)
{
	TCHAR tRetStr[200];//读取的字符串
	const DWORD dwLen=200;//读取的字符串长度
	if(GetPrivateProfileString(_T("CONFIG"),_T("script_file"),NULL,tRetStr,dwLen,m_strScriptPath))
	{
		GetPrivateProfileString(_T("CONFIG"),_T("log_dir"),NULL,tRetStr,dwLen,m_strScriptPath);
		m_strLogDirPath = tRetStr;

		//[LOGFILE]
		GetPrivateProfileString(_T("LOGFILE"),_T("use_prefix"),NULL,tRetStr,dwLen,m_strScriptPath);
		m_strUsePrefix = tRetStr;
		m_objLogPattern.m_strUsePrefix = tRetStr;
		if (!m_strUsePrefix.Compare(_T("1")))
		{
			m_strUsePrefix = _T("前缀");
		} 
		else
		{
			m_strUsePrefix = _T("后缀");
		}

		GetPrivateProfileString(_T("LOGFILE"),_T("prefix"),NULL,tRetStr,dwLen,m_strScriptPath);
		m_strFixName = tRetStr;
		m_objLogPattern.m_strPrefix = tRetStr;
		if (m_strFixName.IsEmpty())
		{
			GetPrivateProfileString(_T("LOGFILE"),_T("postfix"),NULL,tRetStr,dwLen,m_strScriptPath);
			m_strFixName = tRetStr;
			m_objLogPattern.m_strPostfix = tRetStr;
		} 

		GetPrivateProfileString(_T("LOGFILE"),_T("delimiter"),NULL,tRetStr,dwLen,m_strScriptPath);
		m_strDelimiter = tRetStr;
		m_objLogPattern.m_strDelimiter = tRetStr;

		GetPrivateProfileString(_T("LOGFILE"),_T("rec_sep"),NULL,tRetStr,dwLen,m_strScriptPath);
		m_strRecSep = tRetStr;
		if (!m_strRecSep.CompareNoCase(_T("crlf")))
		{
			m_strRecSep = _T("\r\n");
		}
		else if (!m_strRecSep.CompareNoCase(_T("lf")))
		{
			m_strRecSep = _T("\n");
		}
		m_objLogPattern.m_strRecSep = m_strRecSep;

		//[FIELD]
		m_objLogField.m_nTotal = GetPrivateProfileInt(_T("FIELD"),_T("amount"),NULL,m_strScriptPath);
		m_objLogField.m_pstrFieldItem = new CString[m_objLogField.m_nTotal];
		CString strTempKeyName;//模板KeyName
		for (int i=1;i<=m_objLogField.m_nTotal;i++)
		{
			strTempKeyName.Format(_T("field%d"),i);
			GetPrivateProfileString(_T("FIELD"),strTempKeyName,NULL,tRetStr,dwLen,m_strScriptPath);
			m_objLogField.m_pstrFieldItem[i-1] = tRetStr;
		}

		//初始化“条件”ComboBox和“日志内容”ListControl控件
		m_ctrlLogList.InsertColumn(0,_T("ID"),LVCFMT_CENTER,30);
		CString strTemp;
		for (int i=1;i<=m_objLogField.m_nTotal;i++)
		{
			strTemp.Format(_T("%d."),i);
			strTemp += m_objLogField.m_pstrFieldItem[i-1];
			m_ctrlCondition.AddString(strTemp);
			m_ctrlLogList.InsertColumn(i,m_objLogField.m_pstrFieldItem[i-1],LVCFMT_CENTER,85);
		}
		DWORD dwStyle = m_ctrlLogList.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
		dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
		m_ctrlLogList.SetExtendedStyle(dwStyle); //设置扩展风格

		//初始化m_ctrlFilterList ListControl控件
		m_ctrlFilterList.InsertColumn(0,_T("ID"),LVCFMT_CENTER,30);
		m_ctrlFilterList.InsertColumn(1,_T("条件"),LVCFMT_CENTER,130);
		m_ctrlFilterList.InsertColumn(2,_T("条件值"),LVCFMT_CENTER,220);

		dwStyle = m_ctrlFilterList.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
		dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
		m_ctrlFilterList.SetExtendedStyle(dwStyle); //设置扩展风格
		//////////////////////////////////////////////////////////////////////////
		UpdateData(false);
	}
}

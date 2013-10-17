// DlgLogFilter.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LogFilter.h"
#include "DlgLogFilter.h"
#include "afxdialogex.h"

extern CRITICAL_SECTION g_cs;
// CDlgLogFilter �Ի���

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
	//��ʼ��m_objLogFile��Ա����
	m_objLogFile.m_nFileNum = 0;
	m_objLogFile.m_dwTotalBufSize = 0;
	m_objLogFile.m_dwRecNum = 0;
	m_objLogFile.m_strLogDirPath.Empty();
	m_objLogFile.m_pdwFileSizeArray=NULL;
	m_objLogFile.m_pHandleArray=NULL;
	m_objLogFile.m_pstrLogName=NULL;
	m_objLogFile.m_pTotalBuf=NULL;
	m_objLogFile.m_pstrRec=NULL;
	//��ʼ��m_objFilterCondition��Ա����
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


// CDlgLogFilter ��Ϣ�������


BOOL CDlgLogFilter::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//��ȡ�����ļ������������ļ����ݳ�ʼ������ؼ�
	TCHAR tRetStr[200];//��ȡ���ַ���
	const DWORD dwLen=200;//��ȡ���ַ�������
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
			m_strUsePrefix = _T("ǰ׺");
		} 
		else
		{
			m_strUsePrefix = _T("��׺");
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
		CString strTempKeyName;//ģ��KeyName
		for (int i=1;i<=m_objLogField.m_nTotal;i++)
		{
			strTempKeyName.Format(_T("field%d"),i);
			GetPrivateProfileString(_T("FIELD"),strTempKeyName,NULL,tRetStr,dwLen,_T("./config.ini"));
			m_objLogField.m_pstrFieldItem[i-1] = tRetStr;
		}

		//��ʼ����������ComboBox�͡���־���ݡ�ListControl�ؼ�
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
		dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
		dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
		m_ctrlLogList.SetExtendedStyle(dwStyle); //������չ���

		//��ʼ��m_ctrlFilterList ListControl�ؼ�
		m_ctrlFilterList.InsertColumn(0,_T("ID"),LVCFMT_CENTER,30);
		m_ctrlFilterList.InsertColumn(1,_T("����"),LVCFMT_CENTER,130);
		m_ctrlFilterList.InsertColumn(2,_T("����ֵ"),LVCFMT_CENTER,220);

		dwStyle = m_ctrlFilterList.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
		dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
		m_ctrlFilterList.SetExtendedStyle(dwStyle); //������չ���
		//////////////////////////////////////////////////////////////////////////
		UpdateData(false);
	}
	else
	{
		AfxMessageBox(_T("��ǰĿ¼���Ҳ���Ĭ�������ļ���"));
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

//������־�ļ�
void CDlgLogFilter::OnBnClickedBtnDir()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//����m_objLogFile�������ͷ�֮ǰ���ٵ��ڴ�
	if(m_objLogFile.m_pdwFileSizeArray)
	{
		delete []m_objLogFile.m_pdwFileSizeArray;
		m_objLogFile.m_pdwFileSizeArray=NULL;
	}
	if (m_objLogFile.m_pHandleArray)
	{
		//�ر����д򿪵��ļ����
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
	TCHAR szPath[MAX_PATH]=_T("");     //���ѡ���Ŀ¼·�� 

	BROWSEINFO bi;   
	bi.hwndOwner = m_hWnd;   
	bi.pidlRoot = NULL;   
	bi.pszDisplayName = szPath;   
	bi.lpszTitle = _T("��ѡ����־Ŀ¼��");   
	bi.ulFlags = 0;   
	bi.lpfn = NULL;   
	bi.lParam = 0;   
	bi.iImage = 0;   
	//����ѡ��Ŀ¼�Ի���
	LPITEMIDLIST lp = SHBrowseForFolder(&bi);   

	if(lp && SHGetPathFromIDList(lp, szPath))
	{
		m_strLogDirPath = szPath;
		//���ȡ����·�����һ���ַ���Ϊ'\'�����һ��'\'
		if (m_strLogDirPath.Right(1).Compare(_T("\\")))
		{
			m_strLogDirPath += _T("\\");
		}
		m_objLogFile.m_strLogDirPath = m_strLogDirPath;
		//�����־����listcontrol�е�����
		m_ctrlLogList.DeleteAllItems();
		UpdateData(FALSE);
		//����ָ��Ŀ¼�µ�����ƥ���ļ��������ļ����ݶ���m_objLogFile.m_pdwFileSizeArray���ٵ�buffer��
		FindAndRead(&m_objLogFile,m_objLogPattern.m_strUsePrefix,m_strFixName);
		//����record�Ķ�ά���飬����buffer������ӵ���ά������
		FillRecordArray(&m_objLogFile,&m_objLogField,&m_objLogPattern);
		//��䵽����־���ݡ�List Control�ؼ���
		this->UpdateLogContentListControl(m_objLogFile.m_pstrRec,m_objLogFile.m_dwRecNum);
	}
	else   
		AfxMessageBox(_T("��Ч��Ŀ¼��������ѡ��"));   
}

//��ӹ�������
void CDlgLogFilter::OnBnClickedBtnAdd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_ctrlFilterList.DeleteAllItems();
	UpdateData();
	//�����������>=��ǰ���õ�����������̬����
	if (m_objFilterCondition.m_nConditionNum >= m_objFilterCondition.m_nArrayLen)
	{
		//�����һ����ӹ����������򴴽�CString���飬����Ϊ5
		if (!(m_objFilterCondition.m_pstrConditionName || m_objFilterCondition.m_pstrConditionValue))
		{
			m_objFilterCondition.m_pstrConditionName = new CString[5];
			m_objFilterCondition.m_pstrConditionValue = new CString[5];
			m_objFilterCondition.m_nArrayLen = 5;
		}
		else
		{
			//����̬������������Ϊ5����ԭ�����������µ����飬ɾ��ԭ�п��ٵ��ڴ�ռ�
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
	
	//���������������뵽��̬������
	m_objFilterCondition.m_pstrConditionName[m_objFilterCondition.m_nConditionNum]=m_strConditionName;
	m_objFilterCondition.m_pstrConditionValue[m_objFilterCondition.m_nConditionNum]=m_strConditionVal;
	m_objFilterCondition.m_nConditionNum++;

	//�ڡ�������List Control�ؼ�����ʾ��������
	CString strRowNo;
	for (int i=0;i<m_objFilterCondition.m_nConditionNum;i++)
	{
		strRowNo.Format(_T("%d"),i);
		m_ctrlFilterList.InsertItem(i,strRowNo);
		m_ctrlFilterList.SetItemText(i,1,m_objFilterCondition.m_pstrConditionName[i]);
		m_ctrlFilterList.SetItemText(i,2,m_objFilterCondition.m_pstrConditionValue[i]);
	}
}

//���¡���־���ݡ�List Control�ؼ���ʾ������
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

//�����趨�Ĳ�ѯ�����Խ�����в�ѯ
void CDlgLogFilter::OnBnClickedBtnExecute()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���������ֶΣ��������в�ѯ������������������record���롰��־���ݡ�List Control�ؼ���ʾ����
	if (m_objFilterCondition.m_nConditionNum == 0)
	{
		//���û�����ù���������ֱ�ӷ���
		return;
	}
	m_ctrlLogList.DeleteAllItems();
	CString *pstrRowNo=NULL;
 	//�Ѿ���ӵĹ���������record�е��±�
	int nColumn = 0;
	CString strColumn;
	//����������record����
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
				//������в����Ϲ�������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	for(int i=0; i<m_ctrlFilterList.GetItemCount(); i++)
	{
		if( m_ctrlFilterList.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED )
		{
			//ѡ�еĵ�i��֮���"����"ǰ��,���ֻ��һ������������ֱ�ӽ�m_objFilterCondition.m_nConditionNum--
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
	//�ڡ�������List Control�ؼ�����ʾ��������
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
	// TODO: �ڴ����ר�ô����/����û���
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_RETURN||pMsg->wParam==VK_ESCAPE)
			return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDlgLogFilter::OnBnClickedBtnDelteall()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_objFilterCondition.m_nConditionNum = 0;
	m_ctrlFilterList.DeleteAllItems();
}


void CDlgLogFilter::OnBnClickedBtnScript()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	TCHAR tRetStr[200];//��ȡ���ַ���
	const DWORD dwLen=200;//��ȡ���ַ�������
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
			m_strUsePrefix = _T("ǰ׺");
		} 
		else
		{
			m_strUsePrefix = _T("��׺");
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
		CString strTempKeyName;//ģ��KeyName
		for (int i=1;i<=m_objLogField.m_nTotal;i++)
		{
			strTempKeyName.Format(_T("field%d"),i);
			GetPrivateProfileString(_T("FIELD"),strTempKeyName,NULL,tRetStr,dwLen,m_strScriptPath);
			m_objLogField.m_pstrFieldItem[i-1] = tRetStr;
		}

		//��ʼ����������ComboBox�͡���־���ݡ�ListControl�ؼ�
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
		dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
		dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
		m_ctrlLogList.SetExtendedStyle(dwStyle); //������չ���

		//��ʼ��m_ctrlFilterList ListControl�ؼ�
		m_ctrlFilterList.InsertColumn(0,_T("ID"),LVCFMT_CENTER,30);
		m_ctrlFilterList.InsertColumn(1,_T("����"),LVCFMT_CENTER,130);
		m_ctrlFilterList.InsertColumn(2,_T("����ֵ"),LVCFMT_CENTER,220);

		dwStyle = m_ctrlFilterList.GetExtendedStyle();
		dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
		dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
		m_ctrlFilterList.SetExtendedStyle(dwStyle); //������չ���
		//////////////////////////////////////////////////////////////////////////
		UpdateData(false);
	}
}

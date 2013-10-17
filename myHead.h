#ifndef _MY_HEADER_
#define _MY_HEADER_

typedef struct _LogField_
{
	int m_nTotal;//field������
	CString *m_pstrFieldItem;//��CString��̬���������������
}LogField;

typedef struct _LogPattern_
{
	CString m_strUsePrefix;	//�Ƿ�ʹ��ǰ׺
	CString m_strPrefix;		//ǰ׺����
	CString m_strPostfix;		//��׺����
	CString m_strDelimiter;		//field�ָ���
	CString m_strRecSep;		//record�ָ���
}LogPattern;

typedef struct _LogFile_
{
	int m_nFileNum;	//��־�ļ�����
	CString m_strLogDirPath;	//��־�ļ�Ŀ¼
	CString *m_pstrLogName;	//��־�ļ�������
	HANDLE *m_pHandleArray;	//��־�ļ��������
	BYTE *m_pTotalBuf;	//����ѡ�����־�ļ�����
	DWORD *m_pdwFileSizeArray;	//�ļ���С����
	DWORD m_dwTotalBufSize;	//������־�ļ������С
	CString *m_pstrRec;	//����record��¼
	DWORD m_dwRecNum;	//����record������
}LogFile;

typedef struct _FilterCondition_
{
	int m_nConditionNum;	//��������������
	int m_nArrayLen;	//��̬����ĳ���
	CString *m_pstrConditionName;	//��������������
	CString *m_pstrConditionValue;	//����������ֵ
}FilterCondition;

//CalculateRecordNum�߳�ʹ�õĽṹ�壬Ϊ�˶��������
typedef struct _CalRecNumThread_
{
	LogFile *m_pLogFile;	//LogFile�ṹ�壬�����־�ļ���Ϣ
	LogPattern *m_pLogPattern;	//LogPattern�ṹ��
	int m_nIndex;			//��Ҫ���ʵ���־�ļ�����
}CalRecNumThread;

//////////////////////////////////////////////////////////////////////////
//����ָ��Ŀ¼�������ļ�,�����ļ�����buffer��
void FindAndRead(LogFile *pObjLogFiles,CString strUsePrefix,CString strFixName);
//����record�Ķ�ά���飬����buffer������ӵ���ά������
void FillRecordArray(LogFile *pObjLogFiles,LogField *pObjLogField,LogPattern *pObjLogPattern);
//��ָ���ִ���ȡ����һ���ִ���, ���������н����Ӵ�ɾ��
CString GetField(CString &strRecord, CString strFieldSep, CString strRecSep);
//�̺߳���������buffer��record������
void CalculateRecordNum(void * p);
#endif
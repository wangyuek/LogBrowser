#include "stdafx.h"
#include "../myHead.h"

/*Mutex mutex1;*/
//�ٽ���������Դ
CRITICAL_SECTION g_cs;

//���߳�ͬ��
HANDLE *g_pHandle = NULL;
DWORD g_dwBeginTime;

///////////////////////////////////////////////////////////////////////////////
//  Func:       
//  Describe:   ��ָ���ִ���ȡ����һ���ִ���, ���������н����Ӵ�ɾ��
//  Input:      
//              strRecord:      ����
//              strRecSep:      �ֶηָ��
//              strFieldSep:    ��¼�ָ��
//  Output:     
//              strRecord:      �޳��Ӵ��������
//  Return:     �Ӵ�,�����ǿմ�
//  Note:       
///////////////////////////////////////////////////////////////////////////////
CString GetField(CString &strRecord, CString strFieldSep, CString strRecSep)  
{
	int iFieldSepPos;		// ��һ���ֶηָ����λ������
	int iRecSepPos;			// ��һ����¼�ָ����λ������
	int iStartPos;			// ����λ���������ǰ��һ��λ������
	CString strSep;			// �ָ��
	CString strField;

	//  ASSERT(strRecSep.GetLength());
	//	ASSERT(strFieldSep.GetLength());

	iFieldSepPos = strRecord.Find(strFieldSep);
	iRecSepPos   = strRecord.Find(strRecSep);

	// û���ҵ�ָ�����ֶηָ���ͼ�¼�ָ��, ���ִ�����Ч��, ��Ҫ���, 
	// �����ؿմ�
	if (iFieldSepPos < 0 && iRecSepPos < 0)
	{
		//		strField = "";
		strField = strRecord;
		strRecord.Empty();
		return strField;
	}

	//  ȡ����λ���������ǰ��һ��
	if (iFieldSepPos >= 0)
	{
		if (iFieldSepPos < iRecSepPos || iRecSepPos < 0 )
		{
			iStartPos = iFieldSepPos;
			strSep = strFieldSep;
		}
		else
		{
			iStartPos = iRecSepPos;
			strSep = strRecSep;
		}
	}
	else
	{
		iStartPos = iRecSepPos;
		strSep = strRecSep;
	}

	// ȡ����һ���ֶ�, ���ǲ������ָ��, Ȼ�󽫸��ֶ��Լ��ָ����
	// �ַ�����ȥ��
	strField = strRecord.Left(iStartPos);
	strRecord.Delete(0, iStartPos + strSep.GetLength());

	return strField;
}

void FindAndRead(LogFile *pObjLogFiles,CString strUsePrefix,CString strFixName)
{
	//��ʼ��pObjLogFiles����
	pObjLogFiles->m_nFileNum = NULL;
	pObjLogFiles->m_pstrLogName = NULL;

	TCHAR szFind[MAX_PATH]=_T("");
	lstrcpy(szFind,pObjLogFiles->m_strLogDirPath); //windows API ��lstrcpy������strcpy

	//����ʹ��ǰ׺ƥ�仹�Ǻ�׺ƥ�䣬�趨ģ������
	if (!strUsePrefix.CompareNoCase(_T("1")))
	{
		strFixName += _T(".*");
		lstrcat(szFind,strFixName);
	}
	else
	{
		strFixName.Insert(0,_T("*."));
		lstrcat(szFind,strFixName);
	}

	WIN32_FIND_DATA wfd;
	HANDLE hFind=FindFirstFile(szFind,& wfd);
	if(hFind==INVALID_HANDLE_VALUE) // ���û���ҵ������ʧ��
		return;
	do
	{
		if(wfd.cFileName[0]=='.')
			continue; //����������Ŀ¼
		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
// 			TCHAR szFile[MAX_PATH];
// 			if(IsRoot(lpszPath))
// 				wsprintf(szFile,"%s%s",lpszPath,wfd.cFileName);
// 			else
// 				wsprintf(szFile,"%s//%s",lpszPath,wfd.cFileName);
// 			FindInAll(szFile); //����ҵ�����Ŀ¼��������Ŀ¼���еݹ�
		}
		else
		{
			//ͳ�����з���Ҫ����ļ�����
			pObjLogFiles->m_nFileNum++;
		}
	}while(FindNextFile(hFind,&wfd));
	FindClose(hFind); //�رղ��Ҿ��

	//������ҵ����ļ���������0������洢������ļ�����
	if (pObjLogFiles->m_nFileNum)
	{
		int nIndex = 0;//�ļ��������������±�
		pObjLogFiles->m_pstrLogName = new CString[pObjLogFiles->m_nFileNum];
		HANDLE hFind=FindFirstFile(szFind,& wfd);
		if(hFind==INVALID_HANDLE_VALUE) // ���û���ҵ������ʧ��
			return;
		do
		{
			if(wfd.cFileName[0]=='.')
				continue; //����������Ŀ¼
			if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			}
			else
			{
				//���ļ���������
				pObjLogFiles->m_pstrLogName[nIndex] = pObjLogFiles->m_strLogDirPath + wfd.cFileName;
				nIndex++;
			}
		}while(FindNextFile(hFind,&wfd));
		FindClose(hFind); //�رղ��Ҿ��
	}

	//�������ҵ���log files
	if (pObjLogFiles->m_nFileNum)
	{
		DWORD dwTotalFileSize = 0;//�����ļ��Ĵ�С����λbyte
		pObjLogFiles->m_pHandleArray = new HANDLE[pObjLogFiles->m_nFileNum];
		pObjLogFiles->m_pdwFileSizeArray = new DWORD[pObjLogFiles->m_nFileNum];
		//��ȡ�����ļ������ͳ�������ļ���С����λbyte
		for (int i=0;i<pObjLogFiles->m_nFileNum;i++)
		{
			pObjLogFiles->m_pHandleArray[i] =  
				CreateFile(pObjLogFiles->m_pstrLogName[i],GENERIC_READ,FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (pObjLogFiles->m_pHandleArray[i])
			{
				pObjLogFiles->m_pdwFileSizeArray[i] = GetFileSize(pObjLogFiles->m_pHandleArray[i],NULL);
				dwTotalFileSize += pObjLogFiles->m_pdwFileSizeArray[i];
			}
		}
		//Ϊ�����ļ������ڴ�ռ�
		pObjLogFiles->m_dwTotalBufSize = dwTotalFileSize;
		pObjLogFiles->m_pTotalBuf = new BYTE[dwTotalFileSize];
		//�������ļ����뿪�ٵ��ڴ�
		DWORD dwOffset=0;//��buffer�д�����ݵ�ƫ��
		for (int i=0;i<pObjLogFiles->m_nFileNum;i++)
		{
			::ReadFile(pObjLogFiles->m_pHandleArray[i],dwOffset+pObjLogFiles->m_pTotalBuf,
				pObjLogFiles->m_pdwFileSizeArray[i],&pObjLogFiles->m_pdwFileSizeArray[i],NULL);
			dwOffset += pObjLogFiles->m_pdwFileSizeArray[i];
		}
	}
}

void FillRecordArray(LogFile *pObjLogFiles,LogField *pObjLogField,LogPattern *pObjLogPattern)
{
	//////////////////////////////////////////////////////////////////////////
	//2013-10-11 �������ļ����޴�ʱ������record����ʱ�ٶ�̫��������
	//ͨ���������̵߳ķ���������������߳����������ļ���������
	g_pHandle=new HANDLE[pObjLogFiles->m_nFileNum];
	for (int i=0;i<pObjLogFiles->m_nFileNum;i++)
	{
		g_pHandle[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	CalRecNumThread *pobjTemp = new CalRecNumThread[pObjLogFiles->m_nFileNum];
#ifdef _DEBUG
	g_dwBeginTime = GetCurrentTime();
#endif
	for (int i=0;i<pObjLogFiles->m_nFileNum;i++)
	{
		pobjTemp[i].m_pLogFile = pObjLogFiles;
		pobjTemp[i].m_pLogPattern = pObjLogPattern;
		pobjTemp[i].m_nIndex = i;
		_beginthread(CalculateRecordNum,NULL,(void *)(pobjTemp+i));
	}
	for (int j=0;j<pObjLogFiles->m_nFileNum;j++)
	{
		WaitForSingleObject(g_pHandle[j],INFINITE);
	}
	if (pObjLogFiles->m_dwRecNum)
	{
		pObjLogFiles->m_pstrRec = new CString[pObjLogFiles->m_dwRecNum*pObjLogField->m_nTotal];
		CString strBuf = (char *)pObjLogFiles->m_pTotalBuf;
		for (DWORD i=0;i<pObjLogFiles->m_dwRecNum*pObjLogField->m_nTotal;i++)
		{
			pObjLogFiles->m_pstrRec[i] =  GetField(strBuf,pObjLogPattern->m_strDelimiter,pObjLogPattern->m_strRecSep);
		}
	}
}

//�̺߳���������buffer��record������
void CalculateRecordNum(void * p)
{
	//WaitForSingleObject(g_pHandle[((CalRecNumThread *)p)->m_nIndex],INFINITE);
	LogFile *pObjLogFiles = ((CalRecNumThread *)p)->m_pLogFile;
	LogPattern *pObjLogPattern = ((CalRecNumThread *)p)->m_pLogPattern;
	int nIndex = ((CalRecNumThread *)p)->m_nIndex;

	//����buffer�е�offset
	DWORD dwOffset = 0;
	for (int i=0;i<nIndex;i++)
	{
		dwOffset += pObjLogFiles->m_pdwFileSizeArray[i];
	}
	//���ɶ�Ӧ�ļ���С��CString��Ȼ������log file��record������
	CString strBuf((TCHAR *)(pObjLogFiles->m_pTotalBuf+dwOffset),pObjLogFiles->m_pdwFileSizeArray[nIndex]);
	int nRecNum = 0;
	int nStart = 0;
	while (-1 != (nStart = strBuf.Find(pObjLogPattern->m_strRecSep,nStart)))
	{
		nStart++;
		nRecNum++;
	}
	DWORD dwCurrentTime = GetCurrentTime();
	dwCurrentTime = dwCurrentTime - g_dwBeginTime;
	TRACE(_T("CalculateRecordNum() thread no : %d��dwOffset=%d��nRecNum=%d��RunTime=%d\n"),
		nIndex,dwOffset,nRecNum,dwCurrentTime/1000);
	
	//synchronized(mutex1)
	EnterCriticalSection(&g_cs);
	pObjLogFiles->m_dwRecNum += nRecNum;
	LeaveCriticalSection(&g_cs);

	SetEvent(g_pHandle[nIndex]);
	return;
}
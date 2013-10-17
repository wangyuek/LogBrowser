#include "stdafx.h"
#include "../myHead.h"

/*Mutex mutex1;*/
//临界区互斥资源
CRITICAL_SECTION g_cs;

//多线程同步
HANDLE *g_pHandle = NULL;
DWORD g_dwBeginTime;

///////////////////////////////////////////////////////////////////////////////
//  Func:       
//  Describe:   从指定字串中取出第一个字串串, 并从主串中将该子串删除
//  Input:      
//              strRecord:      主串
//              strRecSep:      字段分割符
//              strFieldSep:    记录分割符
//  Output:     
//              strRecord:      剔除子串后的主串
//  Return:     子串,可能是空串
//  Note:       
///////////////////////////////////////////////////////////////////////////////
CString GetField(CString &strRecord, CString strFieldSep, CString strRecSep)  
{
	int iFieldSepPos;		// 第一个字段分割符的位置索引
	int iRecSepPos;			// 第一个记录分割符的位置索引
	int iStartPos;			// 两个位置索引中最靠前的一个位置索引
	CString strSep;			// 分割符
	CString strField;

	//  ASSERT(strRecSep.GetLength());
	//	ASSERT(strFieldSep.GetLength());

	iFieldSepPos = strRecord.Find(strFieldSep);
	iRecSepPos   = strRecord.Find(strRecSep);

	// 没有找到指定的字段分割符和记录分割符, 该字串是无效的, 需要清空, 
	// 并返回空串
	if (iFieldSepPos < 0 && iRecSepPos < 0)
	{
		//		strField = "";
		strField = strRecord;
		strRecord.Empty();
		return strField;
	}

	//  取两个位置索引中最靠前的一个
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

	// 取出第一个字段, 但是不包含分割符, 然后将该字段以及分割符从
	// 字符串中去掉
	strField = strRecord.Left(iStartPos);
	strRecord.Delete(0, iStartPos + strSep.GetLength());

	return strField;
}

void FindAndRead(LogFile *pObjLogFiles,CString strUsePrefix,CString strFixName)
{
	//初始化pObjLogFiles变量
	pObjLogFiles->m_nFileNum = NULL;
	pObjLogFiles->m_pstrLogName = NULL;

	TCHAR szFind[MAX_PATH]=_T("");
	lstrcpy(szFind,pObjLogFiles->m_strLogDirPath); //windows API 用lstrcpy，不是strcpy

	//根据使用前缀匹配还是后缀匹配，设定模糊搜索
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
	if(hFind==INVALID_HANDLE_VALUE) // 如果没有找到或查找失败
		return;
	do
	{
		if(wfd.cFileName[0]=='.')
			continue; //过滤这两个目录
		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
// 			TCHAR szFile[MAX_PATH];
// 			if(IsRoot(lpszPath))
// 				wsprintf(szFile,"%s%s",lpszPath,wfd.cFileName);
// 			else
// 				wsprintf(szFile,"%s//%s",lpszPath,wfd.cFileName);
// 			FindInAll(szFile); //如果找到的是目录，则进入此目录进行递归
		}
		else
		{
			//统计所有符合要求的文件数量
			pObjLogFiles->m_nFileNum++;
		}
	}while(FindNextFile(hFind,&wfd));
	FindClose(hFind); //关闭查找句柄

	//如果查找到的文件数量大于0，分配存储，存放文件名称
	if (pObjLogFiles->m_nFileNum)
	{
		int nIndex = 0;//文件名称数组索引下标
		pObjLogFiles->m_pstrLogName = new CString[pObjLogFiles->m_nFileNum];
		HANDLE hFind=FindFirstFile(szFind,& wfd);
		if(hFind==INVALID_HANDLE_VALUE) // 如果没有找到或查找失败
			return;
		do
		{
			if(wfd.cFileName[0]=='.')
				continue; //过滤这两个目录
			if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			}
			else
			{
				//将文件放入数组
				pObjLogFiles->m_pstrLogName[nIndex] = pObjLogFiles->m_strLogDirPath + wfd.cFileName;
				nIndex++;
			}
		}while(FindNextFile(hFind,&wfd));
		FindClose(hFind); //关闭查找句柄
	}

	//打开所有找到的log files
	if (pObjLogFiles->m_nFileNum)
	{
		DWORD dwTotalFileSize = 0;//所有文件的大小，单位byte
		pObjLogFiles->m_pHandleArray = new HANDLE[pObjLogFiles->m_nFileNum];
		pObjLogFiles->m_pdwFileSizeArray = new DWORD[pObjLogFiles->m_nFileNum];
		//获取所有文件句柄，统计所有文件大小，单位byte
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
		//为所有文件开辟内存空间
		pObjLogFiles->m_dwTotalBufSize = dwTotalFileSize;
		pObjLogFiles->m_pTotalBuf = new BYTE[dwTotalFileSize];
		//将所有文件读入开辟的内存
		DWORD dwOffset=0;//在buffer中存放数据的偏移
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
	//2013-10-11 由于在文件量巨大时，计算record数量时速度太慢，所以
	//通过创建多线程的方法来解决，创建线程数量根据文件数量决定
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

//线程函数，计算buffer中record的数量
void CalculateRecordNum(void * p)
{
	//WaitForSingleObject(g_pHandle[((CalRecNumThread *)p)->m_nIndex],INFINITE);
	LogFile *pObjLogFiles = ((CalRecNumThread *)p)->m_pLogFile;
	LogPattern *pObjLogPattern = ((CalRecNumThread *)p)->m_pLogPattern;
	int nIndex = ((CalRecNumThread *)p)->m_nIndex;

	//计算buffer中的offset
	DWORD dwOffset = 0;
	for (int i=0;i<nIndex;i++)
	{
		dwOffset += pObjLogFiles->m_pdwFileSizeArray[i];
	}
	//生成对应文件大小的CString，然后计算此log file中record的数量
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
	TRACE(_T("CalculateRecordNum() thread no : %d，dwOffset=%d，nRecNum=%d，RunTime=%d\n"),
		nIndex,dwOffset,nRecNum,dwCurrentTime/1000);
	
	//synchronized(mutex1)
	EnterCriticalSection(&g_cs);
	pObjLogFiles->m_dwRecNum += nRecNum;
	LeaveCriticalSection(&g_cs);

	SetEvent(g_pHandle[nIndex]);
	return;
}
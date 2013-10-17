#ifndef _MY_HEADER_
#define _MY_HEADER_

typedef struct _LogField_
{
	int m_nTotal;//field的数量
	CString *m_pstrFieldItem;//用CString动态数组来存放所有列
}LogField;

typedef struct _LogPattern_
{
	CString m_strUsePrefix;	//是否使用前缀
	CString m_strPrefix;		//前缀名称
	CString m_strPostfix;		//后缀名称
	CString m_strDelimiter;		//field分隔符
	CString m_strRecSep;		//record分隔符
}LogPattern;

typedef struct _LogFile_
{
	int m_nFileNum;	//日志文件数量
	CString m_strLogDirPath;	//日志文件目录
	CString *m_pstrLogName;	//日志文件名数组
	HANDLE *m_pHandleArray;	//日志文件句柄数组
	BYTE *m_pTotalBuf;	//所有选择的日志文件缓存
	DWORD *m_pdwFileSizeArray;	//文件大小数组
	DWORD m_dwTotalBufSize;	//所有日志文件缓存大小
	CString *m_pstrRec;	//所有record记录
	DWORD m_dwRecNum;	//所有record的数量
}LogFile;

typedef struct _FilterCondition_
{
	int m_nConditionNum;	//过滤条件的数量
	int m_nArrayLen;	//动态数组的长度
	CString *m_pstrConditionName;	//过滤条件的列名
	CString *m_pstrConditionValue;	//过滤条件的值
}FilterCondition;

//CalculateRecordNum线程使用的结构体，为了多参数传递
typedef struct _CalRecNumThread_
{
	LogFile *m_pLogFile;	//LogFile结构体，存放日志文件信息
	LogPattern *m_pLogPattern;	//LogPattern结构体
	int m_nIndex;			//需要访问的日志文件索引
}CalRecNumThread;

//////////////////////////////////////////////////////////////////////////
//查找指定目录下所有文件,并将文件读到buffer中
void FindAndRead(LogFile *pObjLogFiles,CString strUsePrefix,CString strFixName);
//生成record的二维数组，并将buffer内容添加到二维数组中
void FillRecordArray(LogFile *pObjLogFiles,LogField *pObjLogField,LogPattern *pObjLogPattern);
//从指定字串中取出第一个字串串, 并从主串中将该子串删除
CString GetField(CString &strRecord, CString strFieldSep, CString strRecSep);
//线程函数，计算buffer中record的数量
void CalculateRecordNum(void * p);
#endif

// LogFilter.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLogFilterApp:
// �йش����ʵ�֣������ LogFilter.cpp
//

class CLogFilterApp : public CWinApp
{
public:
	CLogFilterApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CLogFilterApp theApp;
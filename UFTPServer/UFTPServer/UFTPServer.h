
// UFTPServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUFTPServerApp: 
// �йش����ʵ�֣������ UFTPServer.cpp
//

class CUFTPServerApp : public CWinApp
{
public:
	CUFTPServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CUFTPServerApp theApp;
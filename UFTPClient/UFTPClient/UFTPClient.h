
// UFTPClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CUFTPClientApp: 
// �йش����ʵ�֣������ UFTPClient.cpp
//

class CUFTPClientApp : public CWinApp
{
public:
	CUFTPClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CUFTPClientApp theApp;
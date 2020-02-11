// ServerSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "UFTPServer.h"
#include "ServerSocket.h"
#include "UFTPServerDlg.h"


// CServerSocket

CServerSocket::CServerSocket()
{
}

CServerSocket::~CServerSocket()
{
	if (m_hSocket != INVALID_SOCKET)
	{
		Close();
	}
}


// CServerSocket 成员函数


void CServerSocket::OnSend(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	CUFTPServerDlg *pDlg = (CUFTPServerDlg*)AfxGetApp()->GetMainWnd();
	pDlg->sendPkt();
	CAsyncSocket::OnSend(nErrorCode);
}


void CServerSocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	CUFTPServerDlg *pDlg = (CUFTPServerDlg*)AfxGetApp()->GetMainWnd();
	pDlg->recvPkt();
	CAsyncSocket::OnReceive(nErrorCode);
}

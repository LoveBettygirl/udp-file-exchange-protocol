// ClientSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "UFTPClient.h"
#include "ClientSocket.h"
#include "UFTPClientDlg.h"


// CClientSocket

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
	if (m_hSocket != INVALID_SOCKET)
	{
		Close();
	}
}


// CClientSocket 成员函数


void CClientSocket::OnSend(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	CUFTPClientDlg *pDlg = (CUFTPClientDlg*)AfxGetApp()->GetMainWnd();
	pDlg->sendPkt();
	CAsyncSocket::OnSend(nErrorCode);
}


void CClientSocket::OnReceive(int nErrorCode)
{
	// TODO: 在此添加专用代码和/或调用基类
	CUFTPClientDlg *pDlg = (CUFTPClientDlg*)AfxGetApp()->GetMainWnd();
	pDlg->recvPkt();
	CAsyncSocket::OnReceive(nErrorCode);
}

// ServerSocket.cpp : ʵ���ļ�
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


// CServerSocket ��Ա����


void CServerSocket::OnSend(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	CUFTPServerDlg *pDlg = (CUFTPServerDlg*)AfxGetApp()->GetMainWnd();
	pDlg->sendPkt();
	CAsyncSocket::OnSend(nErrorCode);
}


void CServerSocket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	CUFTPServerDlg *pDlg = (CUFTPServerDlg*)AfxGetApp()->GetMainWnd();
	pDlg->recvPkt();
	CAsyncSocket::OnReceive(nErrorCode);
}

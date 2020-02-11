// ClientSocket.cpp : ʵ���ļ�
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


// CClientSocket ��Ա����


void CClientSocket::OnSend(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	CUFTPClientDlg *pDlg = (CUFTPClientDlg*)AfxGetApp()->GetMainWnd();
	pDlg->sendPkt();
	CAsyncSocket::OnSend(nErrorCode);
}


void CClientSocket::OnReceive(int nErrorCode)
{
	// TODO: �ڴ����ר�ô����/����û���
	CUFTPClientDlg *pDlg = (CUFTPClientDlg*)AfxGetApp()->GetMainWnd();
	pDlg->recvPkt();
	CAsyncSocket::OnReceive(nErrorCode);
}

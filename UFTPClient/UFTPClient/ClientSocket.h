#pragma once

// CClientSocket ����Ŀ��

class CClientSocket : public CAsyncSocket
{
public:
	CClientSocket();
	virtual ~CClientSocket();
	virtual void OnSend(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
};



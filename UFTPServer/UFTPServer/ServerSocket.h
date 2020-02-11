#pragma once

// CServerSocket ÃüÁîÄ¿±ê

class CServerSocket : public CAsyncSocket
{
public:
	CServerSocket();
	virtual ~CServerSocket();
	virtual void OnSend(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
};



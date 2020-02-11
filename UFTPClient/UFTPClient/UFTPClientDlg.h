
// UFTPClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "MyListBox.h"
#include "ClientSocket.h"
#include "FileDlg.h"
#include <vector>
using namespace std;

struct Fileblock
{
	u_char buf[1024];
	int len;
};


// CUFTPClientDlg �Ի���
class CUFTPClientDlg : public CDialogEx
{
// ����
public:
	CUFTPClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UFTPCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	~CUFTPClientDlg();
	CIPAddressCtrl m_servip;
	CEdit m_servport;
	CEdit m_uploadfn;
	CEdit m_downloadfn;
	CButton m_localdir;
	CButton m_servdir;
	CMyListBox m_loglist;
	CString ip;
	int port;
	CString servip;
	int servport;
	bool isconnect;
	bool uploaddata;
	CClientSocket *m_pSocket;
	CButton m_conn;
	CButton m_disconn;
	CButton m_upload;
	CButton m_download;
	CString filepath;
	CString refilename;
	CFile currsendfile;
	CFile currrecvfile;
	vector<Fileblock*> sendFileblock;
	vector<Fileblock*> recvFileblock;
	afx_msg void OnBnClickedConnect();
	void recvPkt();
	afx_msg void OnBnClickedDisconn();
	afx_msg void OnBnClickedLocaldir();
	afx_msg void OnBnClickedUpload();
	void sendPkt();
	bool downloaddata;
	bool endsenddata;
	CString sefilename;
	bool endrecvdata;

	//sender
	int maxsendseq;
	//int maxsendwind;
	int cursendseq;
	int cursendack;
	bool *sendack;
	int currsendpos; //��ǰ�����ļ��Ľ���
	int lastsendpkt;
	void SenderInit();
	//bool sendSeqIsAvailable();
	bool sendackHandler(int index);
	void timeoutHandler();

	//receiver
	int waitseq;
	int recvseq;
	int currecvpos;
	int lastrecvpkt;
	void ReceiverInit();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedDownload();
	afx_msg void OnBnClickedServdir();
	afx_msg void OnClose();
};

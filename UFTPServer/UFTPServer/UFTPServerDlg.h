
// UFTPServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "MyListBox.h"
#include "ServerSocket.h"
#include "DotEdit.h"
#include "xsleep.h"
#include <vector>
using namespace std;


struct Fileblock
{
	u_char buf[1024];
	int len;
};

struct Client
{
	CString ip;
	int port;
	bool uploaddata;
	bool downloaddata;
	bool endsenddata;
	CString sefilename;
	CString refilename;
	bool endrecvdata;
	CFile currsendfile;
	CFile currrecvfile;
	vector<Fileblock*> recvFileblock;
	vector<Fileblock*> sendFileblock;
	CWinThread *thread;

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
};

// CUFTPServerDlg �Ի���
class CUFTPServerDlg : public CDialogEx
{
// ����
public:
	CUFTPServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UFTPSERVER_DIALOG };
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
	CButton m_mode;
	CMyListBox m_loglist;
	CServerSocket *m_pSocket;
	CString ip;
	int port;
	vector<CString> allFile;
	void findAllFile();
	vector<Client*> clients; //�����ӵ��û���Ϣ�б�
	void sendPkt();
	void sendPkt(CString curip, int curport);
	void recvPkt();
	int findClient(CString ip, int port);
	afx_msg void OnClose();
	~CUFTPServerDlg();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedNormal();
	int udtHandler(int c);
	CEdit m_setdelay;
	CDotEdit m_setdrop;
	CToolTipCtrl m_Mytip;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

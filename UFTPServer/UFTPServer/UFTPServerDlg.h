
// UFTPServerDlg.h : 头文件
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
	int currsendpos; //当前发送文件的进度
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

// CUFTPServerDlg 对话框
class CUFTPServerDlg : public CDialogEx
{
// 构造
public:
	CUFTPServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UFTPSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	vector<Client*> clients; //已连接的用户信息列表
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

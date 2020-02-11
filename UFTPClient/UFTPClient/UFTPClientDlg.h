
// UFTPClientDlg.h : 头文件
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


// CUFTPClientDlg 对话框
class CUFTPClientDlg : public CDialogEx
{
// 构造
public:
	CUFTPClientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UFTPCLIENT_DIALOG };
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

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedDownload();
	afx_msg void OnBnClickedServdir();
	afx_msg void OnClose();
};

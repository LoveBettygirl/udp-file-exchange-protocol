
// UFTPClientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UFTPClient.h"
#include "UFTPClientDlg.h"
#include "afxdialogex.h"
#include "pkt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString char2CString(char *str)
{
	//计算char *数组大小，以字节为单位，一个汉字占两个字节
	int charLen = strlen(str);
	//计算多字节字符的大小，按字符计算。
	int len = MultiByteToWideChar(CP_ACP, 0, str, charLen, NULL, 0);
	//为宽字节字符数组申请空间，数组大小为按字节计算的多字节字符大小
	TCHAR *buf = new TCHAR[len + 1];
	//多字节编码转换成宽字节编码
	MultiByteToWideChar(CP_ACP, 0, str, charLen, buf, len);
	buf[len] = '\0';  //添加字符串结尾，注意不是len+1
	//将TCHAR数组转换为CString
	CString pWideChar;
	pWideChar.Append(buf);
	//删除缓冲区
	delete[]buf;
	return pWideChar;
}

char *CString2char(CString str)
{
	//注意：以下n和len的值大小不同,n是按字符计算的，len是按字节计算的
	int n = str.GetLength();
	//获取宽字节字符的大小，大小是按字节计算的
	int len = WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), NULL, 0, NULL, NULL);
	//为多字节字符数组申请空间，数组大小为按字节计算的宽字节字节大小
	char * p = new char[len + 1];  //以字节为单位
	//宽字节编码转换成多字节编码
	WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), p, len, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, str, str.GetLength() + 1, p, len + 1, NULL, NULL);
	p[len + 1] = '/0';  //多字节字符以'/0'结束
	return p;
}

Pkt *makeSynFinPkt(BYTE opcode, BYTE ack)
{
	Pkt *pkt = new Pkt;
	memset(pkt, 0, sizeof(Pkt));
	pkt->opcode = opcode;
	pkt->ack = ack;
	return pkt;
}

FileListPkt *makeFileListReq()
{
	FileListPkt *pkt = new FileListPkt;
	memset(pkt, 0, sizeof(FileListPkt));
	pkt->opcode = 3;
	pkt->ack = 0;
	return pkt;
}

FileListPkt *makeFileListReply(char *filelist)
{
	int len = strlen(filelist);
	FileListPkt *pkt = (FileListPkt*)malloc(sizeof(FileListPkt) + len - 1);
	memset(pkt, 0, sizeof(FileListPkt) + len - 1);
	pkt->opcode = 3;
	pkt->ack = 1;
	pkt->len = len;
	memcpy((void*)(pkt->filelistdata), filelist, len);
	return pkt;
}

LoadFilePkt *makeLoadFileReq(BYTE opcode, char *filename)
{
	int len = strlen(filename);
	LoadFilePkt *pkt = (LoadFilePkt*)malloc(sizeof(LoadFilePkt) + len - 1);
	memset(pkt, 0, sizeof(FileListPkt) + len - 1);
	pkt->opcode = opcode;
	pkt->ack = 0;
	pkt->filenamelen = len;
	memcpy((void*)(pkt->filename), filename, len);
	return pkt;
}

LoadFilePkt *makeLoadFileReply(BYTE opcode)
{
	LoadFilePkt *pkt = new LoadFilePkt;
	memset(pkt, 0, sizeof(LoadFilePkt));
	pkt->opcode = opcode;
	pkt->ack = 1;
	return pkt;
}

TransDataPkt *makeTransDataPkt(BYTE opcode, const u_char *data, int datalen, int block)
{
	TransDataPkt *pkt = (TransDataPkt*)malloc(sizeof(TransDataPkt) + datalen - 1);
	memset(pkt, 0, sizeof(TransDataPkt) + datalen - 1);
	pkt->opcode = opcode;
	pkt->block = block;
	pkt->datalen = datalen;
	memcpy((void*)(pkt->data), data, datalen);
	return pkt;
}

TransDataPkt *makeTransDataEnd(BYTE opcode, int block)
{
	TransDataPkt *pkt = new TransDataPkt;
	memset(pkt, 0, sizeof(TransDataPkt));
	pkt->opcode = opcode;
	pkt->block = block;
	pkt->datalen = 0;
	return pkt;
}

TransDataPkt *makeTransDataAck(BYTE opcode, int block)
{
	TransDataPkt *pkt = new TransDataPkt;
	memset(pkt, 0, sizeof(TransDataPkt));
	pkt->opcode = opcode;
	pkt->ack = 1;
	pkt->block = block;
	return pkt;
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CUFTPClientDlg 对话框



CUFTPClientDlg::CUFTPClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_UFTPCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_SMILE);
	ip = L"127.0.0.1";
	port = 0;
	isconnect = false;
	servport = 0;
	uploaddata = false;
	downloaddata = false;
	currsendpos = 0;
	endsenddata = false;
	m_pSocket = new CClientSocket;
}

CUFTPClientDlg::~CUFTPClientDlg()
{
	delete m_pSocket;
}

void CUFTPClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVIP, m_servip);
	DDX_Control(pDX, IDC_SERVPORT, m_servport);
	DDX_Control(pDX, IDC_UPLOADFILENAME, m_uploadfn);
	DDX_Control(pDX, IDC_DOWNLOADFN, m_downloadfn);
	DDX_Control(pDX, IDC_LOCALDIR, m_localdir);
	DDX_Control(pDX, IDC_SERVDIR, m_servdir);
	DDX_Control(pDX, IDC_LOGLIST, m_loglist);
	DDX_Control(pDX, IDC_CONNECT, m_conn);
	DDX_Control(pDX, IDC_DISCONN, m_disconn);
	DDX_Control(pDX, IDC_UPLOAD, m_upload);
	DDX_Control(pDX, IDC_DOWNLOAD, m_download);
}

BEGIN_MESSAGE_MAP(CUFTPClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT, &CUFTPClientDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_DISCONN, &CUFTPClientDlg::OnBnClickedDisconn)
	ON_BN_CLICKED(IDC_LOCALDIR, &CUFTPClientDlg::OnBnClickedLocaldir)
	ON_BN_CLICKED(IDC_UPLOAD, &CUFTPClientDlg::OnBnClickedUpload)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_DOWNLOAD, &CUFTPClientDlg::OnBnClickedDownload)
	ON_BN_CLICKED(IDC_SERVDIR, &CUFTPClientDlg::OnBnClickedServdir)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CUFTPClientDlg 消息处理程序

BOOL CUFTPClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	srand((unsigned)time(NULL));
	do
	{
		port = rand() % 65536 + 10000;
	} while (!m_pSocket->Create(port, SOCK_DGRAM, FD_READ, ip));
	CString temp;
	temp.Format(L"当前客户端IP：%s  端口号：%d", ip, port);
	((CStatic*)GetDlgItem(IDC_CLIENTINFO))->SetWindowTextW(temp);
	m_servport.SetWindowTextW(L"");
	m_conn.EnableWindow(TRUE);
	m_disconn.EnableWindow(FALSE);
	m_servdir.EnableWindow(FALSE);
	m_servip.EnableWindow(TRUE);
	m_servport.EnableWindow(TRUE);
	m_upload.EnableWindow(FALSE);
	m_download.EnableWindow(FALSE);
	m_loglist.InsertString(m_loglist.GetCount(), L"UFTPClient is ready!");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUFTPClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CUFTPClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CUFTPClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUFTPClientDlg::recvPkt()
{
	u_char receiveBuf[4096];
	CString servip1;
	UINT servport1;
	int ret = m_pSocket->ReceiveFrom(receiveBuf, sizeof(receiveBuf) - 1, servip1, servport1);
	if (ret == SOCKET_ERROR)
	{
		//WSAEWOULDBLOCK：非阻塞模式下，请求的操作被阻塞，需等待再次调用
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			//WSAECONNRESET：表示连接被reset，UDP是不需要建立连接的，这里直接忽略这个错误
			//事实上这是Windows socket的一个bug，当UDP套接字在某次发送后收到一个不可到达的ICMP包时
			//这个错误将在下一次调用OnReceive()后返回，会造成假读的现象，网络中不可达的主机报这个错
			if (GetLastError() == WSAECONNRESET)
			{
				return;
			}
			CString error;
			int errorcode = GetLastError();
			error.Format(L"Socket failed to receive: %d", errorcode);
			LPVOID lpMsgBuf;
			//错误详细信息
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				errorcode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //Default language
				(LPTSTR)&lpMsgBuf,
				0,
				NULL
				);
			error = error + L"\n" + (LPCTSTR)lpMsgBuf;
			AfxMessageBox(error);
			m_pSocket->Close();
		}
	}
	Pkt *pkt = (Pkt*)receiveBuf;
	//complete
	if (pkt->opcode == 1 && pkt->ack == 1)	//收到服务器建立连接的响应
	{
		if (isconnect)
			return;
		isconnect = true;
		KillTimer(pkt->opcode);
		m_loglist.InsertString(m_loglist.GetCount(), L"Connection to server succeeded!");
		((CStatic*)GetDlgItem(IDC_CONNSTATE))->SetWindowTextW(L"与服务器连接情况：已连接");
		m_conn.EnableWindow(FALSE);
		m_disconn.EnableWindow(TRUE);
		m_servdir.EnableWindow(TRUE);
		m_servip.EnableWindow(FALSE);
		m_servport.EnableWindow(FALSE);
		m_upload.EnableWindow(TRUE);
		m_download.EnableWindow(TRUE);
	}
	//complete
	else if (pkt->opcode == 2 && pkt->ack == 1)	 //收到服务器关闭连接的响应
	{
		if (!isconnect)
			return;
		isconnect = false;
		KillTimer(pkt->opcode);
		m_loglist.InsertString(m_loglist.GetCount(), L"Connection to server is closed!");
		((CStatic*)GetDlgItem(IDC_CONNSTATE))->SetWindowTextW(L"与服务器连接情况：未连接");
		m_conn.EnableWindow(TRUE);
		m_disconn.EnableWindow(FALSE);
		m_servdir.EnableWindow(FALSE);
		m_servip.EnableWindow(TRUE);
		m_servport.EnableWindow(TRUE);
		m_upload.EnableWindow(FALSE);
		m_download.EnableWindow(FALSE);
	}
	else if (pkt->opcode == 2 && pkt->ack == 0)	 //收到服务器关闭连接的响应
	{
		if (!isconnect)
			return;
		isconnect = false;
		m_loglist.InsertString(m_loglist.GetCount(), L"Server request to close connection!");
		((CStatic*)GetDlgItem(IDC_CONNSTATE))->SetWindowTextW(L"与服务器连接情况：未连接");
		KillTimer(1);
		KillTimer(2);
		KillTimer(3);
		KillTimer(4);
		KillTimer(5);
		KillTimer(6);
		m_conn.EnableWindow(TRUE);
		m_disconn.EnableWindow(FALSE);
		m_servdir.EnableWindow(FALSE);
		m_servip.EnableWindow(TRUE);
		m_servport.EnableWindow(TRUE);
		m_upload.EnableWindow(FALSE);
		m_download.EnableWindow(FALSE);
	}
	else if (pkt->opcode == 3 && pkt->ack == 1)	 //收到服务器返回的文件列表
	{
		if (!isconnect)
			return;
		KillTimer(pkt->opcode);
		m_loglist.InsertString(m_loglist.GetCount(), L"Request for file list of server succeeded!");
		FileListPkt *list = (FileListPkt *)pkt;
		CFileDlg dlg;
		dlg.filelist = list->filelistdata;
		if (dlg.DoModal() == IDOK)
		{
			sefilename = dlg.curselstr;
			m_downloadfn.SetWindowTextW(sefilename);
		}
		else
		{
			sefilename = dlg.curselstr;
			m_downloadfn.SetWindowTextW(L"");
		}
		m_download.EnableWindow(TRUE);
		m_upload.EnableWindow(TRUE);
		m_servdir.EnableWindow(TRUE);
	}
	else if (pkt->opcode == 4 && pkt->ack == 1)	 //收到服务器允许上传文件的响应，接下来就要发文件了
	{
		if (!isconnect)
			return;
		KillTimer(pkt->opcode);
		if (!currsendfile.Open(filepath, CFile::modeRead | CFile::typeBinary)) //文件读取失败
		{
			m_loglist.InsertString(m_loglist.GetCount(), L"Open file "+refilename+L" failed! No such file or directory!");
			m_upload.EnableWindow(TRUE);
		}
		else
		{
			uploaddata = true;
			endsenddata = false;
			downloaddata = false;
			endrecvdata = false;
			int dwRead = 0;
			sendFileblock.clear();
			sendFileblock.push_back(NULL);
			//将文件分割成很多1024字节的小块，最后一块可能小于1024字节
			do
			{
				Fileblock *block = new Fileblock;
				dwRead = currsendfile.Read(block->buf, 1024);
				if (dwRead <= 0)
					break;
				block->len = dwRead;
				sendFileblock.push_back(block);
			}
			while (dwRead > 0);
			sendFileblock.push_back(NULL);	//告知对方文件已经发完的块
			SenderInit();
			//m_pSocket->AsyncSelect(FD_WRITE); //发送文件块
			SetTimer(6, 5000, NULL);
			//m_pSocket->AsyncSelect(FD_WRITE); //发送文件块
			sendPkt(); //发送文件块
		}
	}
	else if (pkt->opcode == 5 && pkt->ack == 1)	 //收到服务器允许下载文件的响应，接下来就要准备接收文件了
	{
		if (!isconnect)
			return;
		KillTimer(pkt->opcode);
		if (!currrecvfile.Open(L"shared\\" + sefilename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) //文件读取失败
		{
			m_loglist.InsertString(m_loglist.GetCount(), L"Open file " + sefilename + L" failed! Create file error!");
			m_download.EnableWindow(TRUE);
		}
		else
		{
			downloaddata = true;
			endrecvdata = false;
			uploaddata = false;
			endsenddata = false;
			recvFileblock.clear();
			recvFileblock.push_back(NULL);
			ReceiverInit();
			m_pSocket->AsyncSelect(FD_READ);
		}
	}
	else if (pkt->opcode == 6 && pkt->ack == 1)	 //收到服务器对发送的某个文件块的确认
	{
		if (!isconnect)
			return;
		if (!(uploaddata&&!endsenddata))
			return;
		TransDataPkt *ack = (TransDataPkt*)pkt;
		if (!sendackHandler((int)(ack->block) - 1))
			return;
		//sendackHandler((int)(ack->block) - 1);
		KillTimer(6);
		if (sendFileblock[ack->block]==NULL && ack->block != 0) //收到了对最后一个包的确认，则是传输完成
		{
			endsenddata = true;
			uploaddata = false;
			m_download.EnableWindow(TRUE);
			m_upload.EnableWindow(TRUE);
			m_servdir.EnableWindow(TRUE);
			currsendfile.Close();
			m_loglist.InsertString(m_loglist.GetCount(), L"Upload file "+refilename+" succeeded!");
			//m_pSocket->AsyncSelect(FD_READ);
			return;
		}
		//SetTimer(6, 5000, NULL);
		//m_pSocket->AsyncSelect(FD_WRITE); //继续发送文件
		sendPkt(); //继续发送文件
		SetTimer(6, 5000, NULL);
	}
	else if (pkt->opcode == 7 && pkt->ack == 0)	 //收到服务器传来的文件
	{
		if (!isconnect)
			return;
		/*if (!(downloaddata&&!endrecvdata))
			return;	 */
		TransDataPkt *dat = (TransDataPkt*)pkt;
		if (!(downloaddata&&!endrecvdata))
		{
			sendPkt();
			return;
		}
		if (dat->block == currecvpos + 1)
		{
			if (dat->datalen == 0)
			{
				lastrecvpkt = dat->block;
			}
			waitseq++;
			currecvpos++;
			recvseq = waitseq - 1;
			currrecvfile.Write(dat->data, dat->datalen);
			Fileblock *block = new Fileblock;
			block->len = dat->datalen;
			memcpy((void*)(block->buf), dat->data, dat->datalen);
			recvFileblock.push_back(block);
			//m_pSocket->AsyncSelect(FD_WRITE);
			sendPkt();
		}
		else
		{
			//m_pSocket->AsyncSelect(FD_WRITE);
			sendPkt();
		} 
	}
}

void CUFTPClientDlg::SenderInit()
{
	maxsendseq = sendFileblock.size() - 1;
	cursendseq = 0;
	//maxsendwind = maxsendseq / 2;
	cursendack = 0;
	currsendpos = 0;
	lastsendpkt = 0;
	sendack = new bool[maxsendseq];
	for (int i = 0; i < maxsendseq; i++)
	{
		sendack[i] = true;
	}
}

/*bool CUFTPClientDlg::sendSeqIsAvailable()
{
	int step;  
	step = cursendseq - cursendack;  
	step = step >= 0 ? step : step + maxsendseq;  //序列号是否在当前发送窗口之内  
	if(step >= maxsendwind)
	{   
		return false;  
	}  
	if(sendack[cursendseq])
	{   
		return true;  
	}  
	return false;
} */

bool CUFTPClientDlg::sendackHandler(int index)
{
	if (index == cursendack)
	{
		sendack[index] = true;
		cursendack = (index + 1) % maxsendseq;
		return true;
	}
	return false;
	/*if (cursendack <= index)
	{
		for (int i = cursendack; i <= index; i++)
		{
			sendack[i] = true;
		}
		cursendack = (index + 1) % maxsendseq;
	}
	else
	{
		//ack 超过了最大值，回到了 cursendack 的左边   
		for (int i = cursendack; i < maxsendseq; i++)
		{
			sendack[i] = true;
		}
		for (int i = 0; i <= index; i++)
		{
			sendack[i] = true;
		}
		cursendack = index + 1;
	} */
}

void CUFTPClientDlg::timeoutHandler()
{
	int index;  
	index = cursendack % maxsendseq;
	/*for (int i = 0; i< maxsendwind; i++) 
	{
		index = (i + cursendack) % maxsendseq;
		sendack[index] = true;
	}  */
	sendack[index] = true; 
	/*currsendpos -= maxsendwind;
	if (currsendpos < 0)
		currsendpos = 0;*/  
	currsendpos = cursendack;
	cursendseq = cursendack;
}

void CUFTPClientDlg::ReceiverInit()
{
	recvseq = 0;
	waitseq = 0;
	currecvpos = 0;
	lastrecvpkt = 0;
}

void CUFTPClientDlg::sendPkt()
{
	if (uploaddata)
	{
		if (!isconnect)
			return;
		if (endsenddata)
			return;
		if (currsendpos+1<sendFileblock.size())
		{
			sendack[cursendseq] = false;  //sendack只有在发送但未确认的时候是false，其余情况都是true

			//send
			if (sendFileblock[currsendpos + 1] == NULL&&currsendpos + 1 != 0)
			{
				lastsendpkt = currsendpos + 1;
				TransDataPkt *pkt = makeTransDataEnd(6, currsendpos + 1);
				if (m_pSocket->SendTo(pkt, sizeof(TransDataPkt), servport, servip) == SOCKET_ERROR)
				{
					//WSAEWOULDBLOCK：非阻塞模式下，请求的操作被阻塞，需等待再次调用
					if (GetLastError() != WSAEWOULDBLOCK)
					{
						CString error;
						int errorcode = GetLastError(); //获取错误码
						error.Format(L"Socket failed to send: %d", errorcode);
						LPVOID lpMsgBuf;
						//错误详细信息
						FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER |
							FORMAT_MESSAGE_FROM_SYSTEM |
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL,
							errorcode,
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //Default language
							(LPTSTR)&lpMsgBuf,
							0,
							NULL
							);
						error = error + L"\n" + (LPCTSTR)lpMsgBuf;
						AfxMessageBox(error); //错误提示框
						m_pSocket->Close();  //遇到错误就关闭套接字
					}
				}
				else
				{
					CString temp;
					temp.Format(L"Send end block #%d", currsendpos + 1);
					m_loglist.InsertString(m_loglist.GetCount(), temp);
				}
			}
			else
			{
				TransDataPkt *pkt = makeTransDataPkt(6, sendFileblock[currsendpos + 1]->buf, sendFileblock[currsendpos + 1]->len, currsendpos + 1);
				if (m_pSocket->SendTo(pkt, sizeof(TransDataPkt) + pkt->datalen - 1, servport, servip) == SOCKET_ERROR)
				{
					//WSAEWOULDBLOCK：非阻塞模式下，请求的操作被阻塞，需等待再次调用
					if (GetLastError() != WSAEWOULDBLOCK)
					{
						CString error;
						int errorcode = GetLastError(); //获取错误码
						error.Format(L"Socket failed to send: %d", errorcode);
						LPVOID lpMsgBuf;
						//错误详细信息
						FormatMessage(
							FORMAT_MESSAGE_ALLOCATE_BUFFER |
							FORMAT_MESSAGE_FROM_SYSTEM |
							FORMAT_MESSAGE_IGNORE_INSERTS,
							NULL,
							errorcode,
							MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //Default language
							(LPTSTR)&lpMsgBuf,
							0,
							NULL
							);
						error = error + L"\n" + (LPCTSTR)lpMsgBuf;
						AfxMessageBox(error); //错误提示框
						m_pSocket->Close();  //遇到错误就关闭套接字
					}
				}
				else
				{
					CString temp;
					temp.Format(L"Send block #%d", currsendpos + 1);
					m_loglist.InsertString(m_loglist.GetCount(), temp);
				}
			}
			

			cursendseq++;
			currsendpos++;
			cursendseq %= maxsendseq;
			//m_pSocket->AsyncSelect(FD_WRITE);
		}
		//Sleep(500);
		m_pSocket->AsyncSelect(FD_READ); //准备接收ack
		//m_pSocket->AsyncSelect(FD_WRITE);
	}
	else if (downloaddata)
	{
		if (!isconnect)
			return;
		if (endrecvdata)
			return;
		//send
		TransDataPkt *pkt = makeTransDataAck(7, currecvpos);
		if (m_pSocket->SendTo(pkt, sizeof(TransDataPkt), servport, servip) == SOCKET_ERROR)
		{
			//WSAEWOULDBLOCK：非阻塞模式下，请求的操作被阻塞，需等待再次调用
			if (GetLastError() != WSAEWOULDBLOCK)
			{
				CString error;
				int errorcode = GetLastError(); //获取错误码
				error.Format(L"Socket failed to send: %d", errorcode);
				LPVOID lpMsgBuf;
				//错误详细信息
				FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					errorcode,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //Default language
					(LPTSTR)&lpMsgBuf,
					0,
					NULL
					);
				error = error + L"\n" + (LPCTSTR)lpMsgBuf;
				AfxMessageBox(error); //错误提示框
				m_pSocket->Close();  //遇到错误就关闭套接字
			}
		}
		else
		{
			CString temp;
			temp.Format(L"Block #%d has been successfully received!", currecvpos);
			m_loglist.InsertString(m_loglist.GetCount(), temp);
			if (currecvpos == lastrecvpkt)
			{
				endrecvdata = true;
				downloaddata = false;
				m_download.EnableWindow(TRUE);
				m_upload.EnableWindow(TRUE);
				m_servdir.EnableWindow(TRUE);
				m_loglist.InsertString(m_loglist.GetCount(), L"Download file " + sefilename + " succeeded!");
				currrecvfile.Close();
			}
			m_pSocket->AsyncSelect(FD_READ);
		}
	}
}


void CUFTPClientDlg::OnBnClickedConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	if (isconnect)
	{
		return;
	}
	if (m_servip.IsBlank())
	{
		AfxMessageBox(L"服务器IP地址不能为空！");
		return;
	}
	CString sport;
	BYTE nf1, nf2, nf3, nf4;
	m_servip.GetAddress(nf1, nf2, nf3, nf4);
	servip.Format(L"%d.%d.%d.%d", nf1, nf2, nf3, nf4);
	m_servport.GetWindowTextW(sport);
	if (sport.GetLength() == 0)
	{
		AfxMessageBox(L"服务器端口号不能为空！");
		return;
	}
	servport = _ttoi(sport);
	Pkt *pkt = makeSynFinPkt(1, 0);
	//向服务器发送请求
	if (m_pSocket->SendTo(pkt, sizeof(Pkt), servport, servip) == SOCKET_ERROR)
	{
		//WSAEWOULDBLOCK：非阻塞模式下，请求的操作被阻塞，需等待再次调用
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			CString error;
			int errorcode = GetLastError(); //获取错误码
			error.Format(L"Socket failed to send: %d", errorcode);
			LPVOID lpMsgBuf;
			//错误详细信息
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				errorcode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //Default language
				(LPTSTR)&lpMsgBuf,
				0,
				NULL
				);
			error = error + L"\n" + (LPCTSTR)lpMsgBuf;
			AfxMessageBox(error); //错误提示框
			m_pSocket->Close();  //遇到错误就关闭套接字
		}
	}
	else
	{
		m_loglist.InsertString(m_loglist.GetCount(), L"Connecting to server......");
		m_conn.EnableWindow(FALSE);
		SetTimer(pkt->opcode, 5000, NULL);  //发送成功就设置定时器，定时5s
		m_pSocket->AsyncSelect(FD_READ);
	}
	//m_pSocket->Close(); //这里还不能关闭套接字，因为要等待服务器响应
}


void CUFTPClientDlg::OnBnClickedDisconn()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!isconnect)
	{
		return;
	}
	m_disconn.EnableWindow(FALSE);
	Pkt *pkt = makeSynFinPkt(2, 0);
	//向服务器发送请求
	if (m_pSocket->SendTo(pkt, sizeof(Pkt), servport, servip) == SOCKET_ERROR)
	{
		//WSAEWOULDBLOCK：非阻塞模式下，请求的操作被阻塞，需等待再次调用
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			CString error;
			int errorcode = GetLastError(); //获取错误码
			error.Format(L"Socket failed to send: %d", errorcode);
			LPVOID lpMsgBuf;
			//错误详细信息
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				errorcode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //Default language
				(LPTSTR)&lpMsgBuf,
				0,
				NULL
				);
			error = error + L"\n" + (LPCTSTR)lpMsgBuf;
			AfxMessageBox(error); //错误提示框
			m_pSocket->Close();  //遇到错误就关闭套接字
		}
	}
	else
	{
		m_loglist.InsertString(m_loglist.GetCount(), L"Closing the connection to server......");
		SetTimer(pkt->opcode, 5000, NULL);  //发送成功就设置定时器，定时5s
		m_pSocket->AsyncSelect(FD_READ);
	}
}


void CUFTPClientDlg::OnBnClickedLocaldir()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL isOpen = TRUE;
	CString defaultDir = L"C:"; //默认路径
	CString fileName = L""; //默认文件名
	CString filter = L"所有文件 (*.*)|*.*||"; //所有文件
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL); //打开文件
	openFileDlg.GetOFN().lpstrInitialDir = L"";

	int ret = openFileDlg.DoModal();//打开文件窗口
	if (ret == IDOK)
	{
		filepath = openFileDlg.GetPathName();
		refilename = openFileDlg.GetFileName();
		m_uploadfn.SetWindowTextW(filepath);
	}
	else
	{
		filepath = L"";
		refilename = L"";
		m_uploadfn.SetWindowTextW(L"");
	}
}


void CUFTPClientDlg::OnBnClickedUpload()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!isconnect)
	{
		return;
	}
	if (refilename.GetLength() == 0)
	{
		AfxMessageBox(L"上传文件不能为空！");
		return;
	}
	m_download.EnableWindow(FALSE);
	m_upload.EnableWindow(FALSE);
	m_servdir.EnableWindow(FALSE);
	LoadFilePkt *pkt = makeLoadFileReq(4, CString2char(refilename));
	//向服务器发送请求
	if (m_pSocket->SendTo(pkt, sizeof(LoadFilePkt)+pkt->filenamelen-1, servport, servip) == SOCKET_ERROR)
	{
		//WSAEWOULDBLOCK：非阻塞模式下，请求的操作被阻塞，需等待再次调用
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			CString error;
			int errorcode = GetLastError(); //获取错误码
			error.Format(L"Socket failed to send: %d", errorcode);
			LPVOID lpMsgBuf;
			//错误详细信息
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				errorcode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //Default language
				(LPTSTR)&lpMsgBuf,
				0,
				NULL
				);
			error = error + L"\n" + (LPCTSTR)lpMsgBuf;
			AfxMessageBox(error); //错误提示框
			m_pSocket->Close();  //遇到错误就关闭套接字
		}
	}
	else
	{
		m_loglist.InsertString(m_loglist.GetCount(), L"Requesting for uploading file " + refilename + L"......");
		SetTimer(pkt->opcode, 5000, NULL);  //发送成功就设置定时器，定时5s
		m_pSocket->AsyncSelect(FD_READ);
	}
}


void CUFTPClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
	{
		KillTimer(1);
		m_loglist.InsertString(m_loglist.GetCount(), L"Request for connection timed out!");
		m_conn.EnableWindow(TRUE);
	}
	else if (nIDEvent == 2)
	{
		KillTimer(2);
		m_loglist.InsertString(m_loglist.GetCount(), L"Request for disconnection timed out!");
		m_disconn.EnableWindow(TRUE);
	}
	else if (nIDEvent == 3)
	{
		KillTimer(3);
		m_loglist.InsertString(m_loglist.GetCount(), L"Request for file list of server timed out!");
		m_download.EnableWindow(TRUE);
		m_upload.EnableWindow(TRUE);
		m_servdir.EnableWindow(TRUE);
	}
	else if (nIDEvent == 4)
	{
		KillTimer(4);
		m_loglist.InsertString(m_loglist.GetCount(), L"Request for uploading file timed out!");
		m_download.EnableWindow(TRUE);
		m_upload.EnableWindow(TRUE);
		m_servdir.EnableWindow(TRUE);
	}
	else if(nIDEvent == 5)
	{
		KillTimer(5);
		m_loglist.InsertString(m_loglist.GetCount(), L"Request for downloading file timed out!");
		m_download.EnableWindow(TRUE);
		m_upload.EnableWindow(TRUE);
		m_servdir.EnableWindow(TRUE);
	}
	else if (nIDEvent == 6)
	{
		timeoutHandler();
		m_loglist.InsertString(m_loglist.GetCount(), L"Timed out! Waiting for retransmission......");
		KillTimer(6);
		SetTimer(6, 5000, NULL);
		sendPkt();
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CUFTPClientDlg::OnBnClickedDownload()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!isconnect)
	{
		return;
	}
	if (sefilename.GetLength() == 0)
	{
		AfxMessageBox(L"下载文件不能为空！");
		return;
	}
	m_download.EnableWindow(FALSE);
	m_upload.EnableWindow(FALSE);
	m_servdir.EnableWindow(FALSE);
	LoadFilePkt *pkt = makeLoadFileReq(5, CString2char(sefilename));
	//向服务器发送请求
	if (m_pSocket->SendTo(pkt, sizeof(LoadFilePkt) + pkt->filenamelen-1, servport, servip) == SOCKET_ERROR)
	{
		//WSAEWOULDBLOCK：非阻塞模式下，请求的操作被阻塞，需等待再次调用
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			CString error;
			int errorcode = GetLastError(); //获取错误码
			error.Format(L"Socket failed to send: %d", errorcode);
			LPVOID lpMsgBuf;
			//错误详细信息
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				errorcode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //Default language
				(LPTSTR)&lpMsgBuf,
				0,
				NULL
				);
			error = error + L"\n" + (LPCTSTR)lpMsgBuf;
			AfxMessageBox(error); //错误提示框
			m_pSocket->Close();  //遇到错误就关闭套接字
		}
	}
	else
	{
		m_loglist.InsertString(m_loglist.GetCount(), L"Requesting for downloading file " + sefilename + L"......");
		SetTimer(pkt->opcode, 5000, NULL);  //发送成功就设置定时器，定时5s
		m_pSocket->AsyncSelect(FD_READ);
	}
}


void CUFTPClientDlg::OnBnClickedServdir()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!isconnect)
	{
		return;
	}
	m_download.EnableWindow(FALSE);
	m_upload.EnableWindow(FALSE);
	m_servdir.EnableWindow(FALSE);
	FileListPkt *pkt = makeFileListReq();
	//向服务器发送请求
	if (m_pSocket->SendTo(pkt, sizeof(FileListPkt), servport, servip) == SOCKET_ERROR)
	{
		//WSAEWOULDBLOCK：非阻塞模式下，请求的操作被阻塞，需等待再次调用
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			CString error;
			int errorcode = GetLastError(); //获取错误码
			error.Format(L"Socket failed to send: %d", errorcode);
			LPVOID lpMsgBuf;
			//错误详细信息
			FormatMessage(
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				errorcode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //Default language
				(LPTSTR)&lpMsgBuf,
				0,
				NULL
				);
			error = error + L"\n" + (LPCTSTR)lpMsgBuf;
			AfxMessageBox(error); //错误提示框
			m_pSocket->Close();  //遇到错误就关闭套接字
		}
	}
	else
	{
		m_loglist.InsertString(m_loglist.GetCount(), L"Requesting for file list of server......");
		SetTimer(pkt->opcode, 5000, NULL);  //发送成功就设置定时器，定时5s
		m_pSocket->AsyncSelect(FD_READ);
	}
}


void CUFTPClientDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OnBnClickedDisconn();
	m_pSocket->Close();
	CDialogEx::OnClose();
}

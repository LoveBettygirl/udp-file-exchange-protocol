
// UFTPServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "UFTPServer.h"
#include "UFTPServerDlg.h"
#include "afxdialogex.h"
#include "pkt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void Client::SenderInit()
{
	maxsendseq = sendFileblock.size() - 1;
	//maxsendwind = maxsendseq / 2;
	cursendseq = 0;
	cursendack = 0;
	currsendpos = 0;
	lastsendpkt = 0;
	sendack = new bool[maxsendseq];
	for (int i = 0; i < maxsendseq; i++)
	{
		sendack[i] = true;
	}
}

/*bool Client::sendSeqIsAvailable()
{
	int step;
	step = cursendseq - cursendack;
	step = step >= 0 ? step : step + maxsendseq;  //序列号是否在当前发送窗口之内  
	if (step >= maxsendwind)
	{
		return false;
	}
	if (sendack[cursendseq])
	{
		return true;
	}
	return false;
} */

bool Client::sendackHandler(int index)
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
		for (int i = cursendack; i < maxsendseq; ++i)
		{
			sendack[i] = true;
		}
		for (int i = 0; i <= index; ++i)
		{
			sendack[i] = true;
		}
		cursendack = index + 1;
	}  */
}

void Client::timeoutHandler()
{
	int index;
	index = cursendack % maxsendseq;
	/*for (int i = 0; i< maxsendwind; i++)
	{
		index = (i + cursendack) % maxsendseq;
		sendack[index] = true;
	} */
	sendack[index] = true;
	currsendpos = cursendack;
	cursendseq = cursendack;
}

void Client::ReceiverInit()
{
	recvseq = 0;
	waitseq = 0;
	currecvpos = 0;
	lastrecvpkt = 0;
}

Client *newClient(CString ip, int port)
{
	Client *client = new Client;
	client->ip = ip;
	client->port = port;
	client->sefilename = L"";
	client->refilename = L"";
	client->downloaddata = false;
	client->endrecvdata = false;
	client->uploaddata = false;
	client->endsenddata = false;
	return client;
}

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


// CUFTPServerDlg 对话框



CUFTPServerDlg::CUFTPServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_UFTPSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_SMILE);
	m_pSocket = new CServerSocket;
	ip = L"127.0.0.1";
	port = 2000;
}

CUFTPServerDlg::~CUFTPServerDlg()
{
	delete m_pSocket;
}

void CUFTPServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NORMAL, m_mode);
	DDX_Control(pDX, IDC_LOGLIST, m_loglist);
	DDX_Control(pDX, IDC_SETDELAY, m_setdelay);
	DDX_Control(pDX, IDC_SETDROP, m_setdrop);
}

BEGIN_MESSAGE_MAP(CUFTPServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_NORMAL, &CUFTPServerDlg::OnBnClickedNormal)
	ON_BN_CLICKED(IDC_DROP, &CUFTPServerDlg::OnBnClickedNormal)
	ON_BN_CLICKED(IDC_DELAY, &CUFTPServerDlg::OnBnClickedNormal)
END_MESSAGE_MAP()


// CUFTPServerDlg 消息处理程序

BOOL CUFTPServerDlg::OnInitDialog()
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
	/*m_Mytip.Create(this);
	m_Mytip.AddTool(GetDlgItem(IDC_DELAY), L"会阻塞主线程，谨慎使用！");
	m_Mytip.SetDelayTime(0); //设置延迟
	m_Mytip.Activate(TRUE); //设置是否启用提示	*/
	if (!m_pSocket->Create(port, SOCK_DGRAM, FD_READ, ip))
	{
		MessageBox(L"创建套接字错误！", L"UDPServer", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	m_pSocket->Bind(port, ip);
	CString temp;
	temp.Format(L"服务器IP：%s  端口号：%d", ip, port);
	((CStatic*)GetDlgItem(IDC_SERVINFO))->SetWindowTextW(temp);
	m_setdelay.SetWindowTextW(L"");
	((CButton *)GetDlgItem(IDC_NORMAL))->SetCheck(TRUE);
	m_loglist.InsertString(m_loglist.GetCount(), L"UFTPServer is ready!");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CUFTPServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CUFTPServerDlg::OnPaint()
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
HCURSOR CUFTPServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CUFTPServerDlg::findAllFile()
{
	allFile.clear();
	CFileFind finder;
	bool finding = finder.FindFile(L"shared\\*.*");
	while (finding)//文件路径存在
	{
		finding = finder.FindNextFile();
		if (finder.IsDirectory())
			continue;
		if (finder.IsDots())
			continue;
		allFile.push_back(finder.GetFileName());//得到该路径下文件的文件名
		//由于只要当前文件夹的文件，不需要子文件夹，故不需递归查找
	}
}

void CUFTPServerDlg::sendPkt()
{
	for (int i = 0; i < clients.size(); i++)
	{
		if (clients[i]->downloaddata)
		{
			if (clients[i]->endsenddata)
			{
				continue;
			}
			
			else if (clients[i]->currsendpos + 1<clients[i]->sendFileblock.size())
			{
				clients[i]->sendack[clients[i]->cursendseq] = false;  //sendack只有在发送但未确认的时候是false，其余情况都是true

				//send
				if (clients[i]->sendFileblock[clients[i]->currsendpos + 1] == NULL&&clients[i]->currsendpos + 1 != 0)
				{
					clients[i]->lastsendpkt = clients[i]->currsendpos + 1;

					TransDataPkt *pkt = makeTransDataEnd(7, clients[i]->currsendpos + 1);
					if (m_pSocket->SendTo(pkt, sizeof(TransDataPkt) + pkt->datalen - 1, clients[i]->port, clients[i]->ip) == SOCKET_ERROR)
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
						temp.Format(L"Client ip: %s port: %d Send end block #%d", clients[i]->ip, clients[i]->port, clients[i]->cursendseq + 1);
						m_loglist.InsertString(m_loglist.GetCount(), temp);
					}
				}
				else
				{
					TransDataPkt *pkt = makeTransDataPkt(7, clients[i]->sendFileblock[clients[i]->currsendpos + 1]->buf, clients[i]->sendFileblock[clients[i]->currsendpos + 1]->len, clients[i]->currsendpos + 1);
					if (m_pSocket->SendTo(pkt, sizeof(TransDataPkt) + pkt->datalen - 1, clients[i]->port, clients[i]->ip) == SOCKET_ERROR)
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
						temp.Format(L"Client ip: %s port: %d Send block #%d", clients[i]->ip, clients[i]->port, clients[i]->cursendseq + 1);
						m_loglist.InsertString(m_loglist.GetCount(), temp);
					}
				}

				clients[i]->cursendseq++;
				clients[i]->currsendpos++;
				clients[i]->cursendseq %= clients[i]->maxsendseq;
			}
			m_pSocket->AsyncSelect(FD_READ); //准备接收ack
			//m_pSocket->AsyncSelect(FD_WRITE);
		}
		else if (clients[i]->uploaddata)
		{
			if (clients[i]->endrecvdata)
			{
				continue;
			}
			//send
			TransDataPkt *pkt = makeTransDataAck(6, clients[i]->currecvpos);
			if (clients[i]->currecvpos == clients[i]->lastrecvpkt)
			{
				clients[i]->endrecvdata = true;
				clients[i]->currrecvfile.Close();
				//m_pSocket->AsyncSelect(FD_READ);
			}	
			if (m_pSocket->SendTo(pkt, sizeof(TransDataPkt), clients[i]->port, clients[i]->ip) == SOCKET_ERROR)
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
				temp.Format(L"Client ip: %s port: %d Block #%d has been successfully received!", clients[i]->ip, clients[i]->port, clients[i]->currecvpos);
				m_loglist.InsertString(m_loglist.GetCount(), temp);
				m_pSocket->AsyncSelect(FD_READ);
			}
		}
	}
}

void CUFTPServerDlg::sendPkt(CString curip, int curport)
{
	int i = findClient(curip, curport);
	if (i < 0)
		return;
	if (clients[i]->downloaddata)
	{
		if (clients[i]->endsenddata)
		{
			return;
		}
		else if (clients[i]->currsendpos + 1<clients[i]->sendFileblock.size())
		{
			clients[i]->sendack[clients[i]->cursendseq] = false;  //sendack只有在发送但未确认的时候是false，其余情况都是true

			//send
			if (clients[i]->sendFileblock[clients[i]->currsendpos + 1] == NULL&&clients[i]->currsendpos + 1 != 0)
			{
				clients[i]->lastsendpkt = clients[i]->currsendpos + 1;

				TransDataPkt *pkt = makeTransDataEnd(7, clients[i]->currsendpos + 1);
				if (m_pSocket->SendTo(pkt, sizeof(TransDataPkt) + pkt->datalen - 1, clients[i]->port, clients[i]->ip) == SOCKET_ERROR)
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
					temp.Format(L"Client ip: %s port: %d Send end block #%d", clients[i]->ip, clients[i]->port, clients[i]->cursendseq + 1);
					m_loglist.InsertString(m_loglist.GetCount(), temp);
				}
			}
			else
			{
				TransDataPkt *pkt = makeTransDataPkt(7, clients[i]->sendFileblock[clients[i]->currsendpos + 1]->buf, clients[i]->sendFileblock[clients[i]->currsendpos + 1]->len, clients[i]->currsendpos + 1);
				if (m_pSocket->SendTo(pkt, sizeof(TransDataPkt) + pkt->datalen - 1, clients[i]->port, clients[i]->ip) == SOCKET_ERROR)
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
					temp.Format(L"Client ip: %s port: %d Send block #%d", clients[i]->ip, clients[i]->port, clients[i]->cursendseq + 1);
					m_loglist.InsertString(m_loglist.GetCount(), temp);
				}
			}

			clients[i]->cursendseq++;
			clients[i]->currsendpos++;
			clients[i]->cursendseq %= clients[i]->maxsendseq;
		}
		m_pSocket->AsyncSelect(FD_READ); //准备接收ack
										 //m_pSocket->AsyncSelect(FD_WRITE);
	}
	else if (clients[i]->uploaddata)
	{
		if (clients[i]->endrecvdata)
		{
			return;
		}
		//send
		TransDataPkt *pkt = makeTransDataAck(6, clients[i]->currecvpos);
		if (m_pSocket->SendTo(pkt, sizeof(TransDataPkt), clients[i]->port, clients[i]->ip) == SOCKET_ERROR)
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
			temp.Format(L"Client ip: %s port: %d Block #%d has been successfully received!", clients[i]->ip, clients[i]->port, clients[i]->currecvpos);
			m_loglist.InsertString(m_loglist.GetCount(), temp);
			if (clients[i]->currecvpos == clients[i]->lastrecvpkt)
			{
				clients[i]->endrecvdata = true;
				clients[i]->currrecvfile.Close();
				CString temp;
				temp.Format(L"Client ip: %s port: %d Upload file %s succeeded!", clients[i]->ip, clients[i]->port, clients[i]->sefilename);
				m_loglist.InsertString(m_loglist.GetCount(), temp);
				//m_pSocket->AsyncSelect(FD_READ);
			}
			m_pSocket->AsyncSelect(FD_READ);
		}
	}
}

int CUFTPServerDlg::findClient(CString ip, int port)
{
	for (int i = 0; i < clients.size(); i++)
	{
		if (clients[i]->ip == ip&&clients[i]->port == port)
			return i;
	}
	return -1;
}

void CUFTPServerDlg::recvPkt()
{
	u_char receiveBuf[4096];
	CString clientip1;
	UINT clientport1;
	int ret = m_pSocket->ReceiveFrom(receiveBuf, sizeof(receiveBuf) - 1, clientip1, clientport1);
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
	if (pkt->opcode == 1 && pkt->ack == 0) //收到客户端连接请求
	{
		if (findClient(clientip1, clientport1) < 0)  //不存在此用户，就新建一个用户
		{
			Client *client = newClient(clientip1, clientport1);
			clients.push_back(client);
		}
		else
			return;
		Pkt *pkt1 = makeSynFinPkt(1, 1);
		//发送连接响应
		if (m_pSocket->SendTo(pkt1, sizeof(Pkt), clientport1, clientip1) == SOCKET_ERROR)
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
			CString temp1, temp2;
			temp1.Format(L"Client ip: %s port: %d request for connection", clientip1, clientport1);
			m_loglist.InsertString(m_loglist.GetCount(), temp1);
			temp2.Format(L"已连接的用户数：%d", clients.size());
			((CStatic*)GetDlgItem(IDC_USERINFO))->SetWindowTextW(temp2);
		}
	}
	else if (pkt->opcode == 2 && pkt->ack == 0) //收到客户端关闭连接请求
	{
		int c;
		if ((c = findClient(clientip1, clientport1)) >= 0)  //删除此用户
		{
			clients.erase(clients.begin() + c);
		}
		else
			return;
		Pkt *pkt1 = makeSynFinPkt(2, 1);
		//发送关闭连接响应
		if (m_pSocket->SendTo(pkt1, sizeof(Pkt), clientport1, clientip1) == SOCKET_ERROR)
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
			CString temp1, temp2;
			temp1.Format(L"Client ip: %s port: %d request for disconnection", clientip1, clientport1);
			m_loglist.InsertString(m_loglist.GetCount(), temp1);
			temp2.Format(L"已连接的用户数：%d", clients.size());
			((CStatic*)GetDlgItem(IDC_USERINFO))->SetWindowTextW(temp2);
		}
	}
	else if (pkt->opcode == 3 && pkt->ack == 0)	  //向客户端返回文件列表
	{
		int c = findClient(clientip1, clientport1);
		if (c < 0)
			return;
		findAllFile();
		//制作服务器端文件列表
		CString filelist;
		for (int i = 0; i < allFile.size(); i++)
		{
			filelist += (allFile[i] + L"\n");
		}
		FileListPkt *filelistpkt = makeFileListReply(CString2char(filelist));

		if (m_pSocket->SendTo(filelistpkt, sizeof(FileListPkt)+filelistpkt->len-1, clientport1, clientip1) == SOCKET_ERROR)
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
			CString temp1;
			temp1.Format(L"Client ip: %s port: %d request for file list", clientip1, clientport1);
			m_loglist.InsertString(m_loglist.GetCount(), temp1);
		}
	}
	else if (pkt->opcode == 4 && pkt->ack == 0)	 //上传文件请求
	{
		int cc = findClient(clientip1, clientport1);
		if (cc < 0)
			return;
		LoadFilePkt *loadfilepkt = makeLoadFileReply(pkt->opcode);

		if (m_pSocket->SendTo(loadfilepkt, sizeof(LoadFilePkt), clientport1, clientip1) == SOCKET_ERROR)
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
			int c = findClient(clientip1, clientport1);
			LoadFilePkt *loadfilepkt1 = (LoadFilePkt*)pkt;
			char *t = new char[loadfilepkt1->filenamelen + 1];
			memcpy(t, loadfilepkt1->filename, loadfilepkt1->filenamelen);
			t[loadfilepkt1->filenamelen] = '\0';
			clients[c]->sefilename = char2CString(t);
			if (!clients[c]->currrecvfile.Open(L"shared\\"+clients[c]->sefilename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) //文件读取失败
			{
				m_loglist.InsertString(m_loglist.GetCount(), L"Open file " + clients[c]->sefilename + L" failed! Create file error!");
			}
			else
			{
				clients[c]->uploaddata = true;
				clients[c]->endrecvdata = false;
				clients[c]->downloaddata = false;
				clients[c]->endsenddata = false;
				clients[c]->recvFileblock.clear();
				clients[c]->recvFileblock.push_back(NULL);
				clients[c]->ReceiverInit();
				m_pSocket->AsyncSelect(FD_READ);
			}
		}
	}
	else if (pkt->opcode == 5 && pkt->ack == 0)	//下载文件请求
	{
		int cc = findClient(clientip1, clientport1);
		if (cc < 0)
			return;
		LoadFilePkt *loadfilepkt = makeLoadFileReply(pkt->opcode);

		if (m_pSocket->SendTo(loadfilepkt, sizeof(LoadFilePkt), clientport1, clientip1) == SOCKET_ERROR)
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
			int c = findClient(clientip1, clientport1);
			LoadFilePkt *loadfilepkt1 = (LoadFilePkt*)pkt;
			char *t = new char[loadfilepkt1->filenamelen + 1];
			memcpy(t, loadfilepkt1->filename, loadfilepkt1->filenamelen);
			t[loadfilepkt1->filenamelen] = '\0';
			clients[c]->refilename = char2CString(t);
			if (!clients[c]->currsendfile.Open(L"shared\\" + clients[c]->refilename, CFile::modeRead | CFile::typeBinary)) //文件读取失败
			{
				m_loglist.InsertString(m_loglist.GetCount(), L"Open file " + clients[c]->refilename + L" failed! No such file or directory!");
			}
			else
			{
				clients[c]->downloaddata = true;
				clients[c]->endsenddata = false;
				clients[c]->uploaddata = false;
				clients[c]->endrecvdata = false;
				int dwRead = 0;
				clients[c]->sendFileblock.clear();
				clients[c]->sendFileblock.push_back(NULL);
				//将文件分割成很多1024字节的小块，最后一块可能小于1024字节
				do
				{
					Fileblock *block = new Fileblock;
					dwRead = clients[c]->currsendfile.Read(block->buf, 1024);
					if (dwRead <= 0)
						break;
					block->len = dwRead;
					clients[c]->sendFileblock.push_back(block);
				} while (dwRead > 0);
				clients[c]->sendFileblock.push_back(NULL);	//告知对方文件已经发完的块
				clients[c]->SenderInit();
				SetTimer(10000 + c, 5000, NULL);
				sendPkt(clients[c]->ip, clients[c]->port); //发送文件块
				//m_pSocket->AsyncSelect(FD_WRITE); 
			}
		}
	}
	else if (pkt->opcode == 6 && pkt->ack == 0)	 //收到客户端发来的文件块
	{
		TransDataPkt *dat = (TransDataPkt*)pkt;
		int c = findClient(clientip1, clientport1);
		if (c < 0)
			return;
		/*if (!(clients[c]->uploaddata&&!clients[c]->endrecvdata))
			return;	*/
		int udt = udtHandler(c);
		if (udt == 2)
		{
			CString temp;
			temp.Format(L"Client ip: %s port: %d The block #%d is dropped!", clientip1, clientport1, dat->block);
			m_loglist.InsertString(m_loglist.GetCount(), temp);
			return;
		}
		/*else if (udt == 3)
		{
			CString temp;
			temp.Format(L"Client ip: %s port: %d Delay event happened!", clientip1, clientport1);
			m_loglist.InsertString(m_loglist.GetCount(), temp);	 
		} */
		//m_loglist.InsertString(m_loglist.GetCount(), L"111");
		if (!(clients[c]->uploaddata&&!clients[c]->endrecvdata))
		{
			sendPkt(clients[c]->ip, clients[c]->port);
			return;
		}
		if (dat->block == clients[c]->currecvpos + 1)
		{
			if (dat->datalen == 0)
			{
				clients[c]->lastrecvpkt = dat->block;
			}
			clients[c]->waitseq++;
			clients[c]->currecvpos++;
			clients[c]->recvseq = clients[c]->waitseq - 1;
			clients[c]->currrecvfile.Write(dat->data, dat->datalen);
			Fileblock *block = new Fileblock;
			block->len = dat->datalen;
			memcpy((void*)(block->buf), dat->data, dat->datalen);
			clients[c]->recvFileblock.push_back(block);
			sendPkt(clients[c]->ip, clients[c]->port);
			//m_pSocket->AsyncSelect(FD_WRITE);
		}
		else
		{
			sendPkt(clients[c]->ip, clients[c]->port);
			//m_pSocket->AsyncSelect(FD_WRITE);
		} 
		//Sleep(2000);
		//m_pSocket->AsyncSelect(FD_WRITE);
	}
	else if (pkt->opcode == 7 && pkt->ack == 1)	 //下载文件块确认
	{
		TransDataPkt *ack = (TransDataPkt*)pkt;
		int c = findClient(clientip1, clientport1);
		if (c < 0)
			return;
		if (!(clients[c]->downloaddata&&!clients[c]->endsenddata))
			return;
		int udt = udtHandler(c);
		if (udt == 2)
		{
			CString temp;
			temp.Format(L"Client ip: %s port: %d The ack of block #%d is dropped!", clientip1, clientport1, ack->block);
			m_loglist.InsertString(m_loglist.GetCount(), temp);
			//m_pSocket->AsyncSelect(FD_WRITE); //继续发送文件
			//SetTimer(10000 + u, 5000, NULL);
			return;
		}
		/*else if (udt == 3)
		{
			CString temp;
			temp.Format(L"Client ip: %s port: %d Delay event happened!", clientip1, clientport1);
			m_loglist.InsertString(m_loglist.GetCount(), temp);
		}  */
		if (!clients[c]->sendackHandler((int)(ack->block) - 1))
			return;
		KillTimer(10000 + c);
		if (clients[c]->sendFileblock[ack->block] == NULL && ack->block != 0) //收到了对最后一个包的确认，则是传输完成
		{
			clients[c]->endsenddata = true;
			clients[c]->currsendfile.Close();
			KillTimer(10000 + c);
			//m_pSocket->AsyncSelect(FD_READ);
			CString temp;
			temp.Format(L"Client ip: %s port: %d Download file %s succeeded!", clientip1, clientport1, clients[c]->refilename);
			m_loglist.InsertString(m_loglist.GetCount(), temp);
			return;
		}  
		sendPkt(clients[c]->ip, clients[c]->port); //继续发送文件
		//m_pSocket->AsyncSelect(FD_WRITE); //继续发送文件
		SetTimer(10000 + c, 5000, NULL);
	}
}

void CUFTPServerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	for (int i = 0; i < clients.size(); i++)
	{
		Pkt *pkt = makeSynFinPkt(2, 0);
		//发送关闭连接响应
		if (m_pSocket->SendTo(pkt, sizeof(Pkt), clients[i]->port, clients[i]->ip) == SOCKET_ERROR)
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
	}
	clients.clear();
	m_pSocket->Close();
	CDialogEx::OnClose();
}


void CUFTPServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent >= 10000)
	{
		int c = nIDEvent - 10000;
		clients[c]->timeoutHandler();
		CString temp;
		temp.Format(L"Client ip: %s port: %d Timed out! Waiting for retransmission......", clients[c]->ip, clients[c]->port);
		m_loglist.InsertString(m_loglist.GetCount(), temp);
		KillTimer(nIDEvent);
		SetTimer(nIDEvent, 5000, NULL);
		sendPkt(clients[c]->ip, clients[c]->port);
		//m_pSocket->AsyncSelect(FD_WRITE);
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CUFTPServerDlg::OnBnClickedNormal()
{
	// TODO: 在此添加控件通知处理程序代码
	/*if (((CButton *)GetDlgItem(IDC_NORMAL))->GetCheck() == TRUE)
	{

	}
	else if (((CButton *)GetDlgItem(IDC_DROP))->GetCheck() == TRUE)
	{

	}  
	else if (((CButton *)GetDlgItem(IDC_DELAY))->GetCheck() == TRUE)
	{
		Sleep(4000);
	}*/
}

int CUFTPServerDlg::udtHandler(int c)
{
	srand((unsigned)time(NULL) * 5);
	CString drop, delay;
	double dropnum;
	int delaynum;
	m_setdrop.GetWindowTextW(drop);
	m_setdelay.GetWindowTextW(delay);
	if (drop.GetLength() == 0)
	{
		dropnum = 0.5;
	}
	else
	{
		dropnum = _ttof(drop);
	}
	if (delay.GetLength() == 0)
	{
		delaynum = 4000;
	}
	else
	{
		delaynum = _ttoi(delay);
	}
	int bound = (int)(dropnum * 100);
	int r = rand() % 101;
	/*CString temp;
	temp.Format(L"%d", r);
	m_loglist.InsertString(m_loglist.GetCount(), temp);	*/
	if (r <= bound)
	{
		if (((CButton *)GetDlgItem(IDC_DROP))->GetCheck() == TRUE)
		{
			return 2;
		}
		else if (((CButton *)GetDlgItem(IDC_DELAY))->GetCheck() == TRUE)
		{
			CString temp;
			temp.Format(L"Client ip: %s port: %d A delay event happened!", clients[c]->ip, clients[c]->port);
			m_loglist.InsertString(m_loglist.GetCount(), temp);
			XSleep(delaynum); //实现延迟并不阻塞主线程
			/*clock_t start, finish;
			start = clock();
			do
			{
				finish = clock();
			} while (finish - start < delaynum); */
			/*AfxBeginThread(delayy, (LPVOID)delaynum, THREAD_PRIORITY_NORMAL);
			while (!d);
			d = false;	*/
			return 3;
		}
	}
	return 1;
}

BOOL CUFTPServerDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	/*if (pMsg->message == WM_MOUSEMOVE)
		m_Mytip.RelayEvent(pMsg); */
	return CDialogEx::PreTranslateMessage(pMsg);
}

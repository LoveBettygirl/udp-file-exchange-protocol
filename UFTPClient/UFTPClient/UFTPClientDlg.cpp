
// UFTPClientDlg.cpp : ʵ���ļ�
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
	//����char *�����С�����ֽ�Ϊ��λ��һ������ռ�����ֽ�
	int charLen = strlen(str);
	//������ֽ��ַ��Ĵ�С�����ַ����㡣
	int len = MultiByteToWideChar(CP_ACP, 0, str, charLen, NULL, 0);
	//Ϊ���ֽ��ַ���������ռ䣬�����СΪ���ֽڼ���Ķ��ֽ��ַ���С
	TCHAR *buf = new TCHAR[len + 1];
	//���ֽڱ���ת���ɿ��ֽڱ���
	MultiByteToWideChar(CP_ACP, 0, str, charLen, buf, len);
	buf[len] = '\0';  //����ַ�����β��ע�ⲻ��len+1
	//��TCHAR����ת��ΪCString
	CString pWideChar;
	pWideChar.Append(buf);
	//ɾ��������
	delete[]buf;
	return pWideChar;
}

char *CString2char(CString str)
{
	//ע�⣺����n��len��ֵ��С��ͬ,n�ǰ��ַ�����ģ�len�ǰ��ֽڼ����
	int n = str.GetLength();
	//��ȡ���ֽ��ַ��Ĵ�С����С�ǰ��ֽڼ����
	int len = WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), NULL, 0, NULL, NULL);
	//Ϊ���ֽ��ַ���������ռ䣬�����СΪ���ֽڼ���Ŀ��ֽ��ֽڴ�С
	char * p = new char[len + 1];  //���ֽ�Ϊ��λ
	//���ֽڱ���ת���ɶ��ֽڱ���
	WideCharToMultiByte(CP_ACP, 0, str, str.GetLength(), p, len, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, str, str.GetLength() + 1, p, len + 1, NULL, NULL);
	p[len + 1] = '/0';  //���ֽ��ַ���'/0'����
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CUFTPClientDlg �Ի���



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


// CUFTPClientDlg ��Ϣ�������

BOOL CUFTPClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	srand((unsigned)time(NULL));
	do
	{
		port = rand() % 65536 + 10000;
	} while (!m_pSocket->Create(port, SOCK_DGRAM, FD_READ, ip));
	CString temp;
	temp.Format(L"��ǰ�ͻ���IP��%s  �˿ںţ�%d", ip, port);
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

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CUFTPClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
		//WSAEWOULDBLOCK��������ģʽ�£�����Ĳ�������������ȴ��ٴε���
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			//WSAECONNRESET����ʾ���ӱ�reset��UDP�ǲ���Ҫ�������ӵģ�����ֱ�Ӻ����������
			//��ʵ������Windows socket��һ��bug����UDP�׽�����ĳ�η��ͺ��յ�һ�����ɵ����ICMP��ʱ
			//�����������һ�ε���OnReceive()�󷵻أ�����ɼٶ������������в��ɴ�������������
			if (GetLastError() == WSAECONNRESET)
			{
				return;
			}
			CString error;
			int errorcode = GetLastError();
			error.Format(L"Socket failed to receive: %d", errorcode);
			LPVOID lpMsgBuf;
			//������ϸ��Ϣ
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
	if (pkt->opcode == 1 && pkt->ack == 1)	//�յ��������������ӵ���Ӧ
	{
		if (isconnect)
			return;
		isconnect = true;
		KillTimer(pkt->opcode);
		m_loglist.InsertString(m_loglist.GetCount(), L"Connection to server succeeded!");
		((CStatic*)GetDlgItem(IDC_CONNSTATE))->SetWindowTextW(L"����������������������");
		m_conn.EnableWindow(FALSE);
		m_disconn.EnableWindow(TRUE);
		m_servdir.EnableWindow(TRUE);
		m_servip.EnableWindow(FALSE);
		m_servport.EnableWindow(FALSE);
		m_upload.EnableWindow(TRUE);
		m_download.EnableWindow(TRUE);
	}
	//complete
	else if (pkt->opcode == 2 && pkt->ack == 1)	 //�յ��������ر����ӵ���Ӧ
	{
		if (!isconnect)
			return;
		isconnect = false;
		KillTimer(pkt->opcode);
		m_loglist.InsertString(m_loglist.GetCount(), L"Connection to server is closed!");
		((CStatic*)GetDlgItem(IDC_CONNSTATE))->SetWindowTextW(L"����������������δ����");
		m_conn.EnableWindow(TRUE);
		m_disconn.EnableWindow(FALSE);
		m_servdir.EnableWindow(FALSE);
		m_servip.EnableWindow(TRUE);
		m_servport.EnableWindow(TRUE);
		m_upload.EnableWindow(FALSE);
		m_download.EnableWindow(FALSE);
	}
	else if (pkt->opcode == 2 && pkt->ack == 0)	 //�յ��������ر����ӵ���Ӧ
	{
		if (!isconnect)
			return;
		isconnect = false;
		m_loglist.InsertString(m_loglist.GetCount(), L"Server request to close connection!");
		((CStatic*)GetDlgItem(IDC_CONNSTATE))->SetWindowTextW(L"����������������δ����");
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
	else if (pkt->opcode == 3 && pkt->ack == 1)	 //�յ����������ص��ļ��б�
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
	else if (pkt->opcode == 4 && pkt->ack == 1)	 //�յ������������ϴ��ļ�����Ӧ����������Ҫ���ļ���
	{
		if (!isconnect)
			return;
		KillTimer(pkt->opcode);
		if (!currsendfile.Open(filepath, CFile::modeRead | CFile::typeBinary)) //�ļ���ȡʧ��
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
			//���ļ��ָ�ɺܶ�1024�ֽڵ�С�飬���һ�����С��1024�ֽ�
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
			sendFileblock.push_back(NULL);	//��֪�Է��ļ��Ѿ�����Ŀ�
			SenderInit();
			//m_pSocket->AsyncSelect(FD_WRITE); //�����ļ���
			SetTimer(6, 5000, NULL);
			//m_pSocket->AsyncSelect(FD_WRITE); //�����ļ���
			sendPkt(); //�����ļ���
		}
	}
	else if (pkt->opcode == 5 && pkt->ack == 1)	 //�յ����������������ļ�����Ӧ����������Ҫ׼�������ļ���
	{
		if (!isconnect)
			return;
		KillTimer(pkt->opcode);
		if (!currrecvfile.Open(L"shared\\" + sefilename, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) //�ļ���ȡʧ��
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
	else if (pkt->opcode == 6 && pkt->ack == 1)	 //�յ��������Է��͵�ĳ���ļ����ȷ��
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
		if (sendFileblock[ack->block]==NULL && ack->block != 0) //�յ��˶����һ������ȷ�ϣ����Ǵ������
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
		//m_pSocket->AsyncSelect(FD_WRITE); //���������ļ�
		sendPkt(); //���������ļ�
		SetTimer(6, 5000, NULL);
	}
	else if (pkt->opcode == 7 && pkt->ack == 0)	 //�յ��������������ļ�
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
	step = step >= 0 ? step : step + maxsendseq;  //���к��Ƿ��ڵ�ǰ���ʹ���֮��  
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
		//ack ���������ֵ���ص��� cursendack �����   
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
			sendack[cursendseq] = false;  //sendackֻ���ڷ��͵�δȷ�ϵ�ʱ����false�������������true

			//send
			if (sendFileblock[currsendpos + 1] == NULL&&currsendpos + 1 != 0)
			{
				lastsendpkt = currsendpos + 1;
				TransDataPkt *pkt = makeTransDataEnd(6, currsendpos + 1);
				if (m_pSocket->SendTo(pkt, sizeof(TransDataPkt), servport, servip) == SOCKET_ERROR)
				{
					//WSAEWOULDBLOCK��������ģʽ�£�����Ĳ�������������ȴ��ٴε���
					if (GetLastError() != WSAEWOULDBLOCK)
					{
						CString error;
						int errorcode = GetLastError(); //��ȡ������
						error.Format(L"Socket failed to send: %d", errorcode);
						LPVOID lpMsgBuf;
						//������ϸ��Ϣ
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
						AfxMessageBox(error); //������ʾ��
						m_pSocket->Close();  //��������͹ر��׽���
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
					//WSAEWOULDBLOCK��������ģʽ�£�����Ĳ�������������ȴ��ٴε���
					if (GetLastError() != WSAEWOULDBLOCK)
					{
						CString error;
						int errorcode = GetLastError(); //��ȡ������
						error.Format(L"Socket failed to send: %d", errorcode);
						LPVOID lpMsgBuf;
						//������ϸ��Ϣ
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
						AfxMessageBox(error); //������ʾ��
						m_pSocket->Close();  //��������͹ر��׽���
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
		m_pSocket->AsyncSelect(FD_READ); //׼������ack
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
			//WSAEWOULDBLOCK��������ģʽ�£�����Ĳ�������������ȴ��ٴε���
			if (GetLastError() != WSAEWOULDBLOCK)
			{
				CString error;
				int errorcode = GetLastError(); //��ȡ������
				error.Format(L"Socket failed to send: %d", errorcode);
				LPVOID lpMsgBuf;
				//������ϸ��Ϣ
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
				AfxMessageBox(error); //������ʾ��
				m_pSocket->Close();  //��������͹ر��׽���
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (isconnect)
	{
		return;
	}
	if (m_servip.IsBlank())
	{
		AfxMessageBox(L"������IP��ַ����Ϊ�գ�");
		return;
	}
	CString sport;
	BYTE nf1, nf2, nf3, nf4;
	m_servip.GetAddress(nf1, nf2, nf3, nf4);
	servip.Format(L"%d.%d.%d.%d", nf1, nf2, nf3, nf4);
	m_servport.GetWindowTextW(sport);
	if (sport.GetLength() == 0)
	{
		AfxMessageBox(L"�������˿ںŲ���Ϊ�գ�");
		return;
	}
	servport = _ttoi(sport);
	Pkt *pkt = makeSynFinPkt(1, 0);
	//���������������
	if (m_pSocket->SendTo(pkt, sizeof(Pkt), servport, servip) == SOCKET_ERROR)
	{
		//WSAEWOULDBLOCK��������ģʽ�£�����Ĳ�������������ȴ��ٴε���
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			CString error;
			int errorcode = GetLastError(); //��ȡ������
			error.Format(L"Socket failed to send: %d", errorcode);
			LPVOID lpMsgBuf;
			//������ϸ��Ϣ
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
			AfxMessageBox(error); //������ʾ��
			m_pSocket->Close();  //��������͹ر��׽���
		}
	}
	else
	{
		m_loglist.InsertString(m_loglist.GetCount(), L"Connecting to server......");
		m_conn.EnableWindow(FALSE);
		SetTimer(pkt->opcode, 5000, NULL);  //���ͳɹ������ö�ʱ������ʱ5s
		m_pSocket->AsyncSelect(FD_READ);
	}
	//m_pSocket->Close(); //���ﻹ���ܹر��׽��֣���ΪҪ�ȴ���������Ӧ
}


void CUFTPClientDlg::OnBnClickedDisconn()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!isconnect)
	{
		return;
	}
	m_disconn.EnableWindow(FALSE);
	Pkt *pkt = makeSynFinPkt(2, 0);
	//���������������
	if (m_pSocket->SendTo(pkt, sizeof(Pkt), servport, servip) == SOCKET_ERROR)
	{
		//WSAEWOULDBLOCK��������ģʽ�£�����Ĳ�������������ȴ��ٴε���
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			CString error;
			int errorcode = GetLastError(); //��ȡ������
			error.Format(L"Socket failed to send: %d", errorcode);
			LPVOID lpMsgBuf;
			//������ϸ��Ϣ
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
			AfxMessageBox(error); //������ʾ��
			m_pSocket->Close();  //��������͹ر��׽���
		}
	}
	else
	{
		m_loglist.InsertString(m_loglist.GetCount(), L"Closing the connection to server......");
		SetTimer(pkt->opcode, 5000, NULL);  //���ͳɹ������ö�ʱ������ʱ5s
		m_pSocket->AsyncSelect(FD_READ);
	}
}


void CUFTPClientDlg::OnBnClickedLocaldir()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL isOpen = TRUE;
	CString defaultDir = L"C:"; //Ĭ��·��
	CString fileName = L""; //Ĭ���ļ���
	CString filter = L"�����ļ� (*.*)|*.*||"; //�����ļ�
	CFileDialog openFileDlg(isOpen, defaultDir, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL); //���ļ�
	openFileDlg.GetOFN().lpstrInitialDir = L"";

	int ret = openFileDlg.DoModal();//���ļ�����
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!isconnect)
	{
		return;
	}
	if (refilename.GetLength() == 0)
	{
		AfxMessageBox(L"�ϴ��ļ�����Ϊ�գ�");
		return;
	}
	m_download.EnableWindow(FALSE);
	m_upload.EnableWindow(FALSE);
	m_servdir.EnableWindow(FALSE);
	LoadFilePkt *pkt = makeLoadFileReq(4, CString2char(refilename));
	//���������������
	if (m_pSocket->SendTo(pkt, sizeof(LoadFilePkt)+pkt->filenamelen-1, servport, servip) == SOCKET_ERROR)
	{
		//WSAEWOULDBLOCK��������ģʽ�£�����Ĳ�������������ȴ��ٴε���
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			CString error;
			int errorcode = GetLastError(); //��ȡ������
			error.Format(L"Socket failed to send: %d", errorcode);
			LPVOID lpMsgBuf;
			//������ϸ��Ϣ
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
			AfxMessageBox(error); //������ʾ��
			m_pSocket->Close();  //��������͹ر��׽���
		}
	}
	else
	{
		m_loglist.InsertString(m_loglist.GetCount(), L"Requesting for uploading file " + refilename + L"......");
		SetTimer(pkt->opcode, 5000, NULL);  //���ͳɹ������ö�ʱ������ʱ5s
		m_pSocket->AsyncSelect(FD_READ);
	}
}


void CUFTPClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!isconnect)
	{
		return;
	}
	if (sefilename.GetLength() == 0)
	{
		AfxMessageBox(L"�����ļ�����Ϊ�գ�");
		return;
	}
	m_download.EnableWindow(FALSE);
	m_upload.EnableWindow(FALSE);
	m_servdir.EnableWindow(FALSE);
	LoadFilePkt *pkt = makeLoadFileReq(5, CString2char(sefilename));
	//���������������
	if (m_pSocket->SendTo(pkt, sizeof(LoadFilePkt) + pkt->filenamelen-1, servport, servip) == SOCKET_ERROR)
	{
		//WSAEWOULDBLOCK��������ģʽ�£�����Ĳ�������������ȴ��ٴε���
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			CString error;
			int errorcode = GetLastError(); //��ȡ������
			error.Format(L"Socket failed to send: %d", errorcode);
			LPVOID lpMsgBuf;
			//������ϸ��Ϣ
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
			AfxMessageBox(error); //������ʾ��
			m_pSocket->Close();  //��������͹ر��׽���
		}
	}
	else
	{
		m_loglist.InsertString(m_loglist.GetCount(), L"Requesting for downloading file " + sefilename + L"......");
		SetTimer(pkt->opcode, 5000, NULL);  //���ͳɹ������ö�ʱ������ʱ5s
		m_pSocket->AsyncSelect(FD_READ);
	}
}


void CUFTPClientDlg::OnBnClickedServdir()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!isconnect)
	{
		return;
	}
	m_download.EnableWindow(FALSE);
	m_upload.EnableWindow(FALSE);
	m_servdir.EnableWindow(FALSE);
	FileListPkt *pkt = makeFileListReq();
	//���������������
	if (m_pSocket->SendTo(pkt, sizeof(FileListPkt), servport, servip) == SOCKET_ERROR)
	{
		//WSAEWOULDBLOCK��������ģʽ�£�����Ĳ�������������ȴ��ٴε���
		if (GetLastError() != WSAEWOULDBLOCK)
		{
			CString error;
			int errorcode = GetLastError(); //��ȡ������
			error.Format(L"Socket failed to send: %d", errorcode);
			LPVOID lpMsgBuf;
			//������ϸ��Ϣ
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
			AfxMessageBox(error); //������ʾ��
			m_pSocket->Close();  //��������͹ر��׽���
		}
	}
	else
	{
		m_loglist.InsertString(m_loglist.GetCount(), L"Requesting for file list of server......");
		SetTimer(pkt->opcode, 5000, NULL);  //���ͳɹ������ö�ʱ������ʱ5s
		m_pSocket->AsyncSelect(FD_READ);
	}
}


void CUFTPClientDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	OnBnClickedDisconn();
	m_pSocket->Close();
	CDialogEx::OnClose();
}

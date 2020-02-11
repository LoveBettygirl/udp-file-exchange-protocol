// FileDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UFTPClient.h"
#include "FileDlg.h"
#include "afxdialogex.h"


void findall(CString src, CString dst, vector<CString> &index)
{
	int lastoccur = 0, nextoccur = 0;
	int len = dst.GetLength();
	while ((nextoccur = src.Find(dst, lastoccur)) >= 0)
	{
		CString temp;
		temp = src.Mid(lastoccur, nextoccur - lastoccur);
		index.push_back(temp);
		lastoccur = nextoccur + len;
	}
}

extern CString char2CString(char *str);
extern char *CString2char(CString str);

// CFileDlg �Ի���

IMPLEMENT_DYNAMIC(CFileDlg, CDialogEx)

CFileDlg::CFileDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_FILEDIALOG, pParent)
{
	cursel = -1;
}

CFileDlg::~CFileDlg()
{
}

void CFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILELIST, m_filelist);
}


BEGIN_MESSAGE_MAP(CFileDlg, CDialogEx)
	ON_LBN_SELCHANGE(IDC_FILELIST, &CFileDlg::OnLbnSelchangeFilelist)
	ON_BN_CLICKED(IDOK, &CFileDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CFileDlg ��Ϣ�������


void CFileDlg::OnLbnSelchangeFilelist()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	cursel = m_filelist.GetCurSel();
	m_filelist.GetText(cursel, curselstr);
}


void CFileDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (cursel < 0)
	{
		AfxMessageBox(L"��ѡ��һ�������ļ���");
		return;
	}
	CDialogEx::OnOK();
}


BOOL CFileDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	vector<CString> files;
	findall(char2CString((char *)filelist), L"\n", files);
	for (int i = 0; i < files.size(); i++)
	{
		m_filelist.InsertString(m_filelist.GetCount(), files[i]);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

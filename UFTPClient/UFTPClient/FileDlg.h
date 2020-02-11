#pragma once
#include "MyListBox.h"
#include <vector>
using namespace std;


// CFileDlg �Ի���

class CFileDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFileDlg)

public:
	CFileDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFileDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILEDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	int cursel;
	CString curselstr;
	BYTE *filelist;
	CMyListBox m_filelist;
	afx_msg void OnLbnSelchangeFilelist();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};

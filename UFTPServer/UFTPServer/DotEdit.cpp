#include "stdafx.h"
#include "DotEdit.h"


CDotEdit::CDotEdit()
{
}


CDotEdit::~CDotEdit()
{
}
BEGIN_MESSAGE_MAP(CDotEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()


void CDotEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	// ��֤С�������ֻ�ܳ���һ��
	if (nChar == '.')
	{
		CString str;
		// ��ȡԭ���༭���е��ַ���
		GetWindowText(str);
		//��ԭ�����ַ������Ѿ���һ��С����,�򲻽�С��������,��֤�����ֻ������һ��С����
		if (str.Find('.') != -1)
		{

		}
		// ������������С����
		else
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
		}
	}
	// ��������,Backspace,Delete
	else if ((nChar >= '0' && nChar <= '9') || (nChar == 0x08) || (nChar == 0x10))
	{
		CString str;
		// ��ȡԭ���༭���е��ַ���
		GetWindowText(str);
		if ((nChar >= '0' && nChar <= '9') && (str.Find('.')!=-1)&&(str.Find('.') == str.GetLength() - 3))
		{
			return;
		}
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
	// �����ļ�,������Ӧ
	else
	{
	}
	//CEdit::OnChar(nChar, nRepCnt, nFlags);
}

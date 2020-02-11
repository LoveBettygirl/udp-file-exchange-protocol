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
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// 保证小数点最多只能出现一次
	if (nChar == '.')
	{
		CString str;
		// 获取原来编辑框中的字符串
		GetWindowText(str);
		//若原来的字符串中已经有一个小数点,则不将小数点输入,保证了最多只能输入一个小数点
		if (str.Find('.') != -1)
		{

		}
		// 否则就输入这个小数点
		else
		{
			CEdit::OnChar(nChar, nRepCnt, nFlags);
		}
	}
	// 输入数字,Backspace,Delete
	else if ((nChar >= '0' && nChar <= '9') || (nChar == 0x08) || (nChar == 0x10))
	{
		CString str;
		// 获取原来编辑框中的字符串
		GetWindowText(str);
		if ((nChar >= '0' && nChar <= '9') && (str.Find('.')!=-1)&&(str.Find('.') == str.GetLength() - 3))
		{
			return;
		}
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
	// 其它的键,都不响应
	else
	{
	}
	//CEdit::OnChar(nChar, nRepCnt, nFlags);
}

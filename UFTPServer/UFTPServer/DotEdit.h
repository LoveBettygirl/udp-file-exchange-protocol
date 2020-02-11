#pragma once
#include "afxwin.h"
class CDotEdit :
	public CEdit
{
public:
	CDotEdit();
	~CDotEdit();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};


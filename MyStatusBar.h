#pragma once
#include "afxribbonstatusbar.h"

class CMyStatusBar :
	public CMFCRibbonStatusBar
{
protected:
	bool IAmBeingShowed;
public:
	CMyStatusBar(void);
	~CMyStatusBar(void);
	//virtual void ShowPane(BOOL bShow,BOOL bDelay,BOOL bActivate);
	bool getbShow(void);
	void setbShow(bool val);

	//afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//DECLARE_MESSAGE_MAP()
	
};

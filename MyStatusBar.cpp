#include "StdAfx.h"
#include "MyStatusBar.h"
#include "MainFrm.h"

/*
BEGIN_MESSAGE_MAP(CMyStatusBar, CMFCRibbonStatusBar)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()
*/

CMyStatusBar::CMyStatusBar(void)
{
	IAmBeingShowed = true;
}

CMyStatusBar::~CMyStatusBar(void)
{
}

bool CMyStatusBar::getbShow(void)
{
	return IAmBeingShowed;
}

void CMyStatusBar::setbShow(bool val)
{
	IAmBeingShowed = val;
}

/*
void CMyStatusBar::ShowPane(BOOL bShow,BOOL bDelay,BOOL bActivate)
{
	CMFCRibbonStatusBar::ShowPane(bShow,bDelay,bActivate);
	setbShow(bShow);
	((CMainFrame *)AfxGetMainWnd())->RecalcLayout();
}
*/

/*
void CMyStatusBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CMFCRibbonStatusBar::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	setbShow(bShow);
	((CMainFrame *)AfxGetMainWnd())->RecalcLayout();
}
*/

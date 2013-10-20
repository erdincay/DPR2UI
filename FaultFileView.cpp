// FaultFileView.cpp : CFaultFileView 类的实现
//

#include "stdafx.h"
#include "DPR2UI.h"
#include "FaultFileView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFaultFileView

CFaultFileView::CFaultFileView()
{
}

CFaultFileView::~CFaultFileView()
{
}


BEGIN_MESSAGE_MAP(CFaultFileView, CWnd)
	ON_WM_PAINT()
	//ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CFaultFileView 消息处理程序

BOOL CFaultFileView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CFaultFileView::OnPaint() 
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	
	// TODO: 在此处添加消息处理程序代码
	
	// 不要为绘制消息而调用 CWnd::OnPaint()
}

/*
void CFaultFileView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CWnd::OnLButtonDown(nFlags, point);
}
*/

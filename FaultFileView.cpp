// FaultFileView.cpp : CFaultFileView ���ʵ��
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



// CFaultFileView ��Ϣ�������

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
	CPaintDC dc(this); // ���ڻ��Ƶ��豸������
	
	// TODO: �ڴ˴������Ϣ����������
	
	// ��ҪΪ������Ϣ������ CWnd::OnPaint()
}

/*
void CFaultFileView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CWnd::OnLButtonDown(nFlags, point);
}
*/

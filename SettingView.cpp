// SettingView.cpp : CSettingView ���ʵ��
//

#include "stdafx.h"
#include "DPR2UI.h"
#include "SettingView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSettingView

CSettingView::CSettingView()
{
}

CSettingView::~CSettingView()
{
}


BEGIN_MESSAGE_MAP(CSettingView, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CSettingView ��Ϣ�������

BOOL CSettingView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CSettingView::OnPaint() 
{
	CPaintDC dc(this); // ���ڻ��Ƶ��豸������
	
	// TODO: �ڴ˴������Ϣ����������
	
	// ��ҪΪ������Ϣ������ CWnd::OnPaint()
}
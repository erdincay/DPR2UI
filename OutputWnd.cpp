#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd()
{
	IAmBeingShowed = true;
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_Font.CreateStockObject(DEFAULT_GUI_FONT);

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建选项卡窗口:
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("未能创建输出选项卡窗口\n");
		return -1;      // 未能创建
	}

	// 创建输出窗格:
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndOutputWarn.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
		!m_wndOutputReturn.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
		!m_wndOutputFind.Create(dwStyle, rectDummy, &m_wndTabs, 4))
	{
		TRACE0("未能创建输出窗口\n");
		return -1;      // 未能创建
	}

	m_wndOutputWarn.SetFont(&m_Font);
	m_wndOutputReturn.SetFont(&m_Font);
	m_wndOutputFind.SetFont(&m_Font);

	CString strTabName;
	BOOL bNameValid;

	// 将列表窗口附加到选项卡:
	int count = 0;
	bNameValid = strTabName.LoadString(IDS_RETURN_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputReturn, strTabName, (UINT)0);
	m_wndOutputReturn.setIndexOfTab(count++);

	bNameValid = strTabName.LoadString(IDS_WARN_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputWarn, strTabName, (UINT)1);
	m_wndOutputWarn.setIndexOfTab(count++);

	bNameValid = strTabName.LoadString(IDS_FIND_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputFind, strTabName, (UINT)2);
	m_wndOutputFind.setIndexOfTab(count++);

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// 选项卡控件应覆盖整个工作区:
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

	((CMainFrame *)AfxGetMainWnd())->RecalcLayout();
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&m_Font);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::FillWarnWindow(CString strVal)
{
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString strTM;
	strTM.Format(_T("%.4d-%.2d-%.2d %.2d:%.2d:%.2d "),tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond);

	m_wndOutputWarn.AddString(strTM + strVal);
	m_wndTabs.SetActiveTab(m_wndOutputWarn.getIndexOfTab());
}

void COutputWnd::FillReturnWindow(CString strVal)
{
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString strTM;
	strTM.Format(_T("%.4d-%.2d-%.2d %.2d:%.2d:%.2d "),tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond);

	m_wndOutputReturn.AddString(strTM + strVal);
	m_wndTabs.SetActiveTab(m_wndOutputReturn.getIndexOfTab());
}

void COutputWnd::FillFindWindow(CString strVal)
{
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString strTM;
	strTM.Format(_T("%.4d-%.2d-%.2d %.2d:%.2d:%.2d "),tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond);

	m_wndOutputFind.AddString(strTM + strVal);
	m_wndTabs.SetActiveTab(m_wndOutputFind.getIndexOfTab());
}

bool COutputWnd::getbShow(void)
{
	return IAmBeingShowed;
}

void COutputWnd::setbShow(bool val)
{
	IAmBeingShowed = val;
}

///*
void COutputWnd::ShowPane(BOOL bShow,BOOL bDelay,BOOL bActivate)
{
	CDockablePane::ShowPane(bShow,bDelay,bActivate);
	setbShow(bShow);
	((CMainFrame *)AfxGetMainWnd())->RecalcLayout();
}
//*/

void COutputWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDockablePane::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	setbShow(bShow);
	((CMainFrame *)AfxGetMainWnd())->RecalcLayout();
}

/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList()
{
	
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY,OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR,OnUpdateEditClear)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_MBUTTONDBLCLK()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList 消息处理程序

void COutputList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::OnEditCopy()
{
	int index = GetCurSel();
	if (index == LB_ERR)
	{
		AfxMessageBox(_T("未选择拷贝内容"));
		return;
	}

	 CString strTmp;
	 GetText(index,strTmp);

	 if( OpenClipboard() )
	 {
		 HGLOBAL clipbuffer;
		 WCHAR * buffer;
		 EmptyClipboard();
		 clipbuffer = GlobalAlloc(GHND | GMEM_SHARE, (strTmp.GetLength()+1)*sizeof(WCHAR));
		 buffer = (WCHAR*)GlobalLock(clipbuffer);
		 lstrcpy(buffer,strTmp);
		 GlobalUnlock(clipbuffer);
#ifdef _UNICODE
		 SetClipboardData(CF_UNICODETEXT,clipbuffer);
#else
		 SetClipboardData(CF_TEXT,clipbuffer);
#endif
		 CloseClipboard();
	 }
}

void COutputList::OnEditClear()
{
	ResetContent();
}

void COutputList::OnViewOutput()
{
	CDockablePane* pParentBar = DYNAMIC_DOWNCAST(CDockablePane, GetOwner()->GetOwner());
	CMDIFrameWndEx* pMainFrame = DYNAMIC_DOWNCAST(CMDIFrameWndEx, GetTopLevelFrame());

	if (pMainFrame != NULL && pParentBar != NULL)
	{
		pMainFrame->SetFocus();
		pMainFrame->ShowPane(pParentBar, FALSE, FALSE, FALSE);
		pMainFrame->RecalcLayout();
		((COutputWnd *)pParentBar)->setbShow(false);

	}
}

void COutputList::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	bool bSelected = false;
	for (int i=0;i<GetCount();i++)
	{
		if (GetSel(i) > 0)
		{
			bSelected = true;
			break;
		}
	}
	if(bSelected)
	{
		pCmdUI->Enable(true);
	}
	else
	{
		pCmdUI->Enable(false);
	}
}

void COutputList::OnUpdateEditClear(CCmdUI* pCmdUI)
{
	if (GetCount() > 0)
	{
		pCmdUI->Enable(true);
	} 
	else
	{
		pCmdUI->Enable(false);
	}
}

int COutputList::getIndexOfTab()
{
	return m_iIndexOfTab;
}

void COutputList::setIndexOfTab(int val)
{
	m_iIndexOfTab = val;
}

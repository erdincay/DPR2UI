
#include "stdafx.h"
#include "mainfrm.h"
#include "FileView.h"
#include "Resource.h"
#include "DPR2UI.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

CFileView::CFileView()
{
	IAmBeingShowed = true;
	strRoot.clear();

}

CFileView::~CFileView()
{

}

BEGIN_MESSAGE_MAP(CFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_NEW_DIR, OnAddNewDir)
	ON_COMMAND(ID_OPEN, OnItemOpen)
	ON_COMMAND(ID_OPEN_WITH, OnItemOpenWith)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_COMMAND(ID_REFRESH,OnRefresh)
	ON_UPDATE_COMMAND_UI(ID_REFRESH,OnUpdateRefresh)
	ON_COMMAND(ID_EDIT_CLEAR_ALL, OnEditClearAll)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OPEN,ID_OPEN_WITH,OnUpdatePopFile)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_CLEAR,ID_EDIT_CUT,OnUpdatePopFile)
	//ON_NOTIFY_REFLECT(WM_LBUTTONDBLCLK, &CFileView::OnNMDblclk) 
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDBLCLK()
	//ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 消息处理程序

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("未能创建文件视图\n");
		return -1;      // 未能创建
	}

	// 加载视图图像:
	m_FileViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* 已锁定*/);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// 填入一些静态树视图数据(此处只需填入虚拟代码，而不是复杂的数据)
	FillFileView();
	AdjustLayout();

	return 0;
}

void CFileView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();

	((CMainFrame *)AfxGetMainWnd())->RecalcLayout();
}

void CFileView::FillFileView()
{
	TraversingAllFile(theApp.g_sDataBases.getRootCatalog());	
}

void CFileView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CFileView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFileView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CFileView::OnAddNewDir()
{
	CString resPath;
	/*
	CString curPath;
	GetCurrentDirectory(MAX_PATH,curPath.GetBufferSetLength(MAX_PATH + 1));
	curPath.ReleaseBuffer();
	*/
	
	if(!SelDir(resPath,"//",_bstr_t("选取文件目录")))
	{
		TraversingAllFile(resPath);
	}


}

void CFileView::OnItemOpen()
{
	// TODO: 在此处添加命令处理程序代码
	HTREEITEM curItem = m_wndFileView.GetSelectedItem();
	if (curItem == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	CString pathName = m_wndFileView.GetItemText(curItem);

	HTREEITEM curParent = NULL;
	HTREEITEM tmpItem = curItem;
	
	while(curParent = m_wndFileView.GetParentItem(tmpItem))
	{
		tmpItem = curParent;
		pathName = m_wndFileView.GetItemText(tmpItem) + _T("\\") + pathName;
	}

	int ItemType = m_wndFileView.getCurItemInfo();

	switch (ItemType)
	{
	case ROOT_ITEM_TYPE:
		OpenMenu(pathName);
		break;

	case FILE_ITEM_TYPE:
		OpenFaultFile(pathName);
		break;

	case MENU_ITEM_TYPE:
		OpenMenu(pathName);
		break;

	case XML_ITEM_TYPE:
		OpenXmlFile(pathName);
		break;

	default:
		break;
	}

}

void CFileView::OnItemOpenWith()
{
	// TODO: 在此处添加命令处理程序代码
}

void CFileView::OnDummyCompile()
{
	// TODO: 在此处添加命令处理程序代码
}

void CFileView::OnEditCut()
{
	// TODO: 在此处添加命令处理程序代码
}

void CFileView::OnEditCopy()
{
	// TODO: 在此处添加命令处理程序代码
}

void CFileView::OnEditClear()
{
	int ItemType = m_wndFileView.getCurItemInfo();
	if (ItemType != ROOT_ITEM_TYPE)
	{
		CString str = _T("请选择根目录再进行移除操作");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	HTREEITEM curItem = m_wndFileView.GetSelectedItem();
	CString rootName = m_wndFileView.GetItemText(curItem);

	for (int i=0;i<strRoot.size();i++)
	{
		if (rootName.Compare(strRoot[i]) == 0)
		{
			strRoot.erase(strRoot.begin() + i);
			break;
		}
	}

	RefreshFileView();
}

void CFileView::OnEditClearAll()
{
	strRoot.clear();
	RefreshFileView();
}

void CFileView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CFileView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndFileView.SetFocus();
}

void CFileView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* 锁定 */);

	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("无法加载位图: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
}

int CFileView::SelDir(CString& strPath,char * strRootFolder,LPCTSTR cDlgName)
{
	WCHAR   path[MAX_PATH];  
	path[0]='\0'; 
	//LPWSTR path;
	BROWSEINFO    bi;        
	if(strRootFolder==NULL)        
		bi.pidlRoot=NULL;        
	else    
	{        
		LPITEMIDLIST     pIdl    =    NULL;        
		IShellFolder*    pDesktopFolder;        
		char             szPath[MAX_PATH];        
		OLECHAR          olePath[MAX_PATH];        
		ULONG            chEaten;        
		ULONG            dwAttributes;        
		strcpy_s(szPath, strRootFolder);        
		if(SUCCEEDED(SHGetDesktopFolder(&pDesktopFolder)))        
		{        
			MultiByteToWideChar(CP_ACP,    MB_PRECOMPOSED,    szPath,    -1,    olePath,    MAX_PATH);        
			pDesktopFolder->ParseDisplayName(NULL,    NULL,    olePath,    &chEaten,    &pIdl,    &dwAttributes);        
			pDesktopFolder->Release();        
		}        
		bi.pidlRoot=pIdl;        
	}      
	bi.hwndOwner=GetSafeHwnd();          
	bi.pszDisplayName= path;
	bi.lpszTitle=cDlgName;        
	bi.ulFlags=0;        
	bi.lpfn=NULL;        
	bi.lParam=NULL;        
	bi.iImage=NULL;        
	       
	if(SHGetPathFromIDList(SHBrowseForFolder(&bi),path))     
	{     
		strPath.Format(_T("%s"),path);     
		return 0;     
	}     
	else   
	{
		return -1;     
	}
}
void CFileView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int ItemType = m_wndFileView.getCurItemInfo();
	switch (ItemType)
	{
	case ROOT_ITEM_TYPE:
		break;

	case FILE_ITEM_TYPE:
		OnItemOpen();
		break;

	case MENU_ITEM_TYPE:
		break;

	case XML_ITEM_TYPE:
		OnItemOpen();
		break;

	default:
		break;
	}

	CDockablePane::OnLButtonDblClk(nFlags, point);
}


bool CFileView::getbShow(void)
{
	return IAmBeingShowed;
}

void CFileView::setbShow(bool val)
{
	IAmBeingShowed = val;
}

/*
void CFileView::ShowPane(BOOL bShow,BOOL bDelay,BOOL bActivate)
{
	CDockablePane::ShowPane(bShow,bDelay,bActivate);
	setbShow(bShow);
	((CMainFrame *)AfxGetMainWnd())->RecalcLayout();
}
*/

void CFileView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDockablePane::OnShowWindow(bShow, nStatus);
	// TODO: 在此处添加消息处理程序代码
	setbShow(bShow);
	((CMainFrame *)AfxGetMainWnd())->RecalcLayout();
}

int CFileView::BrowerDir(HTREEITEM parentItem,CString parentPath)
{
	CFileFind ff;

	CString file = parentPath + _T("\\*");
	bool bRes = ff.FindFile(file);
	while (bRes)
	{
		bRes = ff.FindNextFile();
		if (ff.IsDirectory() && !ff.IsDots())
		{
			CString strPath = ff.GetFilePath();
			CString strTitle = ff.GetFileTitle();

			HTREEITEM hSubItem = m_wndFileView.InsertItem( strTitle, 0, 0, parentItem);
			m_wndFileView.SetItemData(hSubItem,MENU_ITEM_TYPE);

			BrowerDir(hSubItem,strPath);
		}
		else if (!ff.IsDirectory() && !ff.IsDots())
		{
			CString name = ff.GetFileName();
			CString strSuffix;
			if(FindFileSuffixName(name,strSuffix))
			{
				if (strSuffix.CompareNoCase(_T(".inf")) == 0)
				{
					HTREEITEM hSubItem = m_wndFileView.InsertItem(name, 2, 2, parentItem);
					m_wndFileView.SetItemData(hSubItem,FILE_ITEM_TYPE);
				}
				else if (strSuffix.CompareNoCase(_T(".cfg")) == 0)
				{
					HTREEITEM hSubItem = m_wndFileView.InsertItem(name, 2, 2, parentItem);
					m_wndFileView.SetItemData(hSubItem,FILE_ITEM_TYPE);
				}
				else if (strSuffix.CompareNoCase(_T(".dat")) == 0)
				{
					HTREEITEM hSubItem = m_wndFileView.InsertItem(name, 2, 2, parentItem);
					m_wndFileView.SetItemData(hSubItem,FILE_ITEM_TYPE);
				}
				else if (strSuffix.CompareNoCase(_T(".xml")) == 0)
				{
					HTREEITEM hSubItem = m_wndFileView.InsertItem(name, 1, 1, parentItem);
					m_wndFileView.SetItemData(hSubItem,XML_ITEM_TYPE);
				}
				else if (strSuffix.CompareNoCase(_T(".win")) == 0)
				{
					HTREEITEM hSubItem = m_wndFileView.InsertItem(name, 1, 1, parentItem);
					m_wndFileView.SetItemData(hSubItem,WIN_ITEM_TYPE);
				}
			}
		}
	}

	return 0;
}

void CFileView::TraversingAllFile(CString path)
{
	if (!FolderExist(path))
	{
		return;
	}

	HTREEITEM hRoot;
	hRoot = m_wndFileView.InsertItem(path, 0, 0);
	m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
	m_wndFileView.SetItemData(hRoot,ROOT_ITEM_TYPE);
	strRoot.push_back(path);

	BrowerDir(hRoot,path);

	m_wndFileView.Expand(hRoot, TVE_EXPAND);
}
void CFileView::RefreshFileView(void)
{
	/*
	vector<CString> strTmps;
	HTREEITEM hSibling = m_wndFileView.GetRootItem();
	CString name = m_wndFileView.GetItemText(hSibling);
	strTmps.push_back(name);

	
	while(hSibling != NULL)
	{
		hSibling = m_wndFileView.GetNextSiblingItem(hSibling);
		name = m_wndFileView.GetItemText(hSibling);
		strTmps.push_back(name);
	}
	*/

	vector<CString> strTmps;
	strTmps.clear();

	for (int i=0;i<strRoot.size();i++)
	{
		strTmps.push_back(strRoot[i]);
	}

	strRoot.clear();
	m_wndFileView.DeleteAllItems();

	for (int i=0;i<strTmps.size();i++)
	{
		TraversingAllFile(strTmps[i]);
	}
	
}

void CFileView::OpenFaultFile(CString fileName)
{
	CFileFind ff;

	CString strSuffix;
	CString strPrefix;
	if(FindFileSuffixName(fileName,strSuffix))
	{
		if( 
				strSuffix.CompareNoCase(_T(".cfg")) == 0
			||	strSuffix.CompareNoCase(_T(".dat")) == 0
			||	strSuffix.CompareNoCase(_T(".inf")) == 0
		   )
		{
			strPrefix = fileName.Left(fileName.GetLength() - strSuffix.GetLength());
			if (!(ff.FindFile(strPrefix + _T(".cfg"))))
			{
				CString cfgErr;
				cfgErr.Format(_T("故障文件%s不存在，无法打开"),strPrefix + _T(".cfg"));
				AfxMessageBox(cfgErr);
				((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(cfgErr);
				return;
			}

			if (!(ff.FindFile(strPrefix + _T(".dat"))))
			{
				CString cfgErr;
				cfgErr.Format(_T("故障文件%s不存在，无法打开"),strPrefix + _T(".dat"));
				AfxMessageBox(cfgErr);
				((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(cfgErr);

				return;
			}

		}
		else
		{
			CString str = _T("非法故障文件类型");
			AfxMessageBox(str);
			((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
			return;
		}
	}
	else
	{
		CString str = _T("非法故障文件类型");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		return;
	}

	ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), _T("open"),CString(theApp.g_sDataBases.getOriginalCatalog()) + _T("\\WaveAnalyze.exe"),strPrefix + _T(".cfg"), 0, SW_SHOWMAXIMIZED);

	/*
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = _T(".\\WaveAnalyze.exe");
	ShExecInfo.lpParameters = strPrefix + _T(".cfg");
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	//WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	//*/

	/*
	CString cmd = _T(".\\WaveAnalyze.exe ") + strPrefix + _T(".cfg");
	LPCSTR lpcs = WCHARtoChar(cmd.GetBuffer());
	cmd.ReleaseBuffer();
	WinExec(lpcs,SW_SHOW);
	delete lpcs;	
	*/
}

void CFileView::OpenMenu(CString menuName)
{
	ShellExecute(NULL, _T("open"), menuName,0, 0, SW_SHOWNORMAL);
}

void CFileView::OpenXmlFile(CString pathName)
{
	ShellExecute(NULL, _T("open"), pathName,0, 0, SW_SHOWNORMAL);
}

void CFileView::OnUpdatePopFile(CCmdUI* pCmdUI)
{
	bool bHaveRoot = strRoot.size() > 0;
	bool bSelRoot = (m_wndFileView.getCurItemInfo() == ROOT_ITEM_TYPE);
	
	switch (pCmdUI->m_nID)
	{
	case ID_OPEN:
		pCmdUI->Enable(bHaveRoot);
		break;

	case ID_EDIT_CLEAR:
		pCmdUI->Enable(bSelRoot);
		break;

	case ID_EDIT_CLEAR_ALL:
		pCmdUI->Enable(bHaveRoot);
		break;

	default:
		break;
	}
}

void CFileView::OnRefresh()
{
	RefreshFileView();
}
void CFileView::OnUpdateRefresh(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(true);
}

BOOL CFileView::PreTranslateMessage(MSG* pMsg)
{
	///*
	if (pMsg->message == WM_LBUTTONDBLCLK && pMsg->hwnd == m_wndFileView.GetSafeHwnd())
	{   
		OnLButtonDblClk(pMsg->wParam,pMsg->pt);

		::TranslateMessage(pMsg);   
		::DispatchMessage(pMsg);   

		return   TRUE;//   TRUE,   msg   is   processed   

	}
	//*/
	//   FALSE,   for   further   process   
	return CDockablePane::PreTranslateMessage(pMsg);
}
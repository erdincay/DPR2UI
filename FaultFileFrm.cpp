// FaultFileFrm.cpp : CFaultFileFrame 类的实现
//

#include "stdafx.h"
#include "DPR2UI.h"
#include "MainFrm.h"
#include "FaultFileFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFaultFileFrame

IMPLEMENT_DYNCREATE(CFaultFileFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CFaultFileFrame, CMDIChildWndEx)
	ON_COMMAND(ID_FILE_CLOSE, &CFaultFileFrame::OnFileClose)
	ON_COMMAND_RANGE(ID_FAULTFILE_MENU, ID_FAULTFILE_ANALYSIS, OnPopFaultFile)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FAULTFILE_MENU,ID_FAULTFILE_ANALYSIS,OnUpdatePopFaultFile)
	ON_COMMAND(ID_REFRESH,OnRefresh)
	ON_UPDATE_COMMAND_UI(ID_REFRESH,OnUpdateRefresh)
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

// CFaultFileFrame 构造/析构

CFaultFileFrame::CFaultFileFrame()
{
	// TODO: 在此添加成员初始化代码
	m_iStationIndex = NO_STATION_SEl;
	m_iDevIndex = NO_DEV_SEL;
}

CFaultFileFrame::~CFaultFileFrame()
{
	((CMainFrame *)AfxGetMainWnd())->ClearFaultFileFrmPtr();
}


BOOL CFaultFileFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CFaultFileFrame 诊断

#ifdef _DEBUG
void CFaultFileFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CFaultFileFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CFaultFileFrame 消息处理程序
void CFaultFileFrame::OnFileClose() 
{
	// 若要关闭框架，只需发送 WM_CLOSE，
	// 这相当于从系统菜单中选择关闭。
	SendMessage(WM_CLOSE);
}

int CFaultFileFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	//GetClientRect(rectDummy);
	rectDummy.SetRectEmpty();
	
	// 创建一个视图以占用框架的工作区
	///*
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,rectDummy, this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("未能创建视图窗口\n");
		return -1;
	}
	//*/

	///*
	if (!m_wndShowList.Create(WS_VISIBLE | WS_CHILD | LVS_REPORT, rectDummy, this, 2))
	{
		TRACE0("未能创建故障列表\n");
		return -1;      // 未能创建
	}
	
	m_wndShowList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_wndShowList.ModifyStyleEx(0,WS_EX_CLIENTEDGE);
	m_wndShowList.EnableMarkSortedColumn();
	m_wndShowList.EnableMultipleSort();
	//*/

	
	//AdjustLayout();
	//InitFaultFileFrmList();

	return 0;
}

void CFaultFileFrame::InitFaultFileFrmList(int stationIndex,int devIndex)
{
	//m_wndShowList.DeleteAllItems();

	int count = (m_wndShowList.GetHeaderCtrl()).GetItemCount();
	for (int i=0;i<count;i++)
	{
		m_wndShowList.DeleteColumn(0);
	}

	CRect rectClient;
	m_wndShowList.GetClientRect(&rectClient);
	

	LPTSTR lpszCols[] = {_T("记录时间"),_T("启动原因"),_T("远方文件路径名"),_T("已下载"),_T("备注"),_T("")};
	LV_COLUMN   lvColumn;
	//initialize the columns
	
	
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	int narryWeight[] = {rectClient.Width() / 6,rectClient.Width() / 5,rectClient.Width() / 3,rectClient.Width() / 9,(rectClient.Width() - rectClient.Width() / 6 - rectClient.Width() / 5 -rectClient.Width() / 3 - rectClient.Width() / 9)};

	for(int i = 0; wcslen(lpszCols[i])!=0; i++)
	{
		//make the secondary columns smaller
		lvColumn.cx = narryWeight[i];
		lvColumn.pszText = lpszCols[i];
		m_wndShowList.InsertColumn(i,&lvColumn);
	}

	RefreshFaultFileList(stationIndex,devIndex);

}

/*
void CFaultFileFrame::InitFaultFileFrmList()
{
	//m_wndShowList.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_wndShowList.DeleteAllItems();

	int count = m_wndShowList.GetHeaderCtrl().GetItemCount();
	for (int i=0;i<count;i++)
	{
		m_wndShowList.DeleteColumn(0);
	}

	CRect rectClient;
	m_wndShowList.GetClientRect(&rectClient);


	LPTSTR lpszCols[] = {_T("记录时间"),_T("启动原因"),_T("远方文件路径名"),_T("已下载"),_T("备注"),_T("")};
	LV_COLUMN   lvColumn;
	//initialize the columns
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	int narryWeight[] = {rectClient.Width() / 4,rectClient.Width() / 4,rectClient.Width() / 4,rectClient.Width() / 9,(rectClient.Width() - rectClient.Width() / 4 - rectClient.Width() / 4 -rectClient.Width() / 4 - rectClient.Width() / 9)};

	for(int i = 0; wcslen(lpszCols[i])!=0; i++)
	{
		//make the secondary columns smaller
		lvColumn.cx = narryWeight[i];
		lvColumn.pszText = lpszCols[i];
		m_wndShowList.InsertColumn(i,&lvColumn);
	}

	RefreshFaultFileList(iStationIndex,iDevIndex);

}
*/

void CFaultFileFrame::RecalcLayout(void)
{
	CRect rectClient;
	m_wndShowList.GetClientRect(&rectClient);

	LPTSTR lpszCols[] = {_T("记录时间"),_T("启动原因"),_T("远方文件路径名"),_T("已下载"),_T("备注"),_T("")};
	int narryWeight[] = {rectClient.Width() / 6,rectClient.Width() / 5,rectClient.Width() / 3,rectClient.Width() / 9,(rectClient.Width() - rectClient.Width() / 6 - rectClient.Width() / 5 -rectClient.Width() / 3 - rectClient.Width() / 9)};

	for(int i = 0; wcslen(lpszCols[i])!=0; i++)
	{
		m_wndShowList.SetColumnWidth(i,narryWeight[i]);
	}
}

void CFaultFileFrame::RefreshFaultFileList(int stationIndex,int devIndex)
{
	if (stationIndex < 0 || stationIndex >= theApp.g_sDataBases.size())
	{
		return;
	}

	if (devIndex < 0 || devIndex >= theApp.g_sDataBases[stationIndex].vDevs.size())
	{
		return;
	}

	m_iStationIndex = stationIndex;
	m_iDevIndex = devIndex;

	m_wndShowList.DeleteAllItems();

	if (theApp.g_sDataBases[stationIndex].vDevs[devIndex].faultfiles.size() == 0)
	{
		theApp.g_sDataBases[stationIndex].vDevs[devIndex].faultfiles.LoadFaultMenu(stationIndex,devIndex);
	}

	for (int j=0;j<theApp.g_sDataBases[stationIndex].vDevs[devIndex].faultfiles.size();j++)
	{
		SYSTEMTIME tm = theApp.g_sDataBases[stationIndex].vDevs[devIndex].faultfiles[j].getFileTime();
		CString strTM;
		//strTM.Format(_T("%.4d-%.2d-%.2d %.2d:%.2d:%.2d.%.3d"),tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond,tm.wMilliseconds);
		strTM.Format(_T("%.4d-%.2d-%.2d %.2d:%.2d:%.2d"),tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond);
		m_wndShowList.InsertItem(j,strTM);
		m_wndShowList.SetItemText(j,1,theApp.g_sDataBases[stationIndex].vDevs[devIndex].faultfiles[j].getTrigReason());
		m_wndShowList.SetItemText(j,2,theApp.g_sDataBases[stationIndex].vDevs[devIndex].faultfiles[j].getFaultFileName());

		CString strFileName = theApp.g_sDataBases.getRootCatalog();
		strFileName += theApp.g_sDataBases[stationIndex].getStationCatalog();
		strFileName += theApp.g_sDataBases[stationIndex].vDevs.getDevsCatalog();
		strFileName += theApp.g_sDataBases[stationIndex].vDevs[devIndex].getDevCatalog();
		strFileName += theApp.g_sDataBases[stationIndex].vDevs[devIndex].faultfiles.getFaultFilesCatalog();
		strFileName += _T("\\");
		strFileName += theApp.g_sDataBases[stationIndex].vDevs[devIndex].faultfiles[j].getLocalFileName();

		if (FileExist(strFileName + _T(".cfg")) && FileExist(strFileName + _T(".dat")))
		{
			theApp.g_sDataBases[stationIndex].vDevs[devIndex].faultfiles[j].setFileBeExisted(true);
			m_wndShowList.SetItemText(j,3,_T("YES"));
		}
		else
		{
			theApp.g_sDataBases[stationIndex].vDevs[devIndex].faultfiles[j].setFileBeExisted(false);
			m_wndShowList.SetItemText(j,3,_T("NO"));
		}
	}
}

void CFaultFileFrame::AdjustLayout(void)
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect frameClient;
	CRect viewClient;
	GetClientRect(frameClient);
	m_wndView.GetClientRect(viewClient);
	m_wndShowList.SetWindowPos(NULL, frameClient.left, frameClient.top, frameClient.Width(), frameClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);	
}

void CFaultFileFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWndEx::OnSetFocus(pOldWnd);

	m_wndView.SetFocus();
}

BOOL CFaultFileFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// 让视图第一次尝试该命令
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	{
		return TRUE;
	}
	
	// 否则，执行默认处理
	return CMDIChildWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
int CFaultFileFrame::getSelectedFileIndex(void)
{
	if (m_wndShowList.GetItemCount() > 0)
	{
		POSITION pos = m_wndShowList.GetFirstSelectedItemPosition();
		while (pos)
		{
				int nItem = m_wndShowList.GetNextSelectedItem(pos);
				return nItem;
		}
	}

	return -1;
}

void CFaultFileFrame::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	CListCtrl* pWndList = (CListCtrl*)&m_wndShowList;
	ASSERT_VALID(pWndList);

	if (pWnd != pWndList)
	{
		CMDIChildWndEx::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptList = point;
		pWndList->ScreenToClient(&ptList);

		UINT flags = 0;
		int index = pWndList->HitTest(ptList, &flags);
		if (index > 0)
		{
			pWndList->SetItemState(index,LVIS_SELECTED,LVIS_SELECTED);
		}
	}

	pWndList->SetFocus();

	CMenu menu;

	menu.LoadMenu(IDR_POPUP_FAULTFRM);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
		{
			return;
		}

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}
}

void CFaultFileFrame::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(m_iStationIndex < 0 || m_iStationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站序号错误，重新选择子站");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	if (m_iDevIndex < 0 || m_iDevIndex >= theApp.g_sDataBases[m_iStationIndex].vDevs.size())
	{
		CString str = _T("装置序号错误，重新选择装置");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int fileIndex = getSelectedFileIndex();
	if (fileIndex < 0 || fileIndex >= theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].faultfiles.size())
	{
		CString str = _T("故障文件选择错误,重新选择文件");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		RefreshFaultFileList(m_iStationIndex,m_iDevIndex);

		return;
	}

	if (theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].faultfiles[fileIndex].getFileBeExisted())
	{
		OnAnalysisFaultFile();
	}
	else
	{
		OnGetFaultFile();
	}

	CMDIChildWndEx::OnLButtonDblClk(nFlags, point);
}

void CFaultFileFrame::OnPopFaultFile(UINT id)
{
	switch (id)
	{
	case ID_FAULTFILE_MENU:
		OnOpenLocalMenu();
		break;

	case ID_FAULTFILE_GET:
		OnGetFaultFile();
		break;

	case ID_FAULTFILE_ANALYSIS:
		OnAnalysisFaultFile();
		break;

	default:
		break;
	}
}

void CFaultFileFrame::OnUpdatePopFaultFile(CCmdUI* pCmdUI)
{
	int stationIndex = m_iStationIndex;
	int devIndex = m_iDevIndex;
	int fileIndex = getSelectedFileIndex();

	bool bFileSel      = (fileIndex >= 0);
	bool bStationLegal = (theApp.g_sDataBases.size() > 0) && (stationIndex >= 0) && (stationIndex < theApp.g_sDataBases.size());
	bool bCommActive   = bStationLegal && (theApp.g_sDataBases[stationIndex].getCommActive());
	bool bDevLegal	   = bStationLegal && (theApp.g_sDataBases[stationIndex].vDevs.size() > 0) && (devIndex >= 0) && (devIndex < theApp.g_sDataBases[stationIndex].vDevs.size());

	switch (pCmdUI->m_nID)
	{
	case ID_FAULTFILE_MENU:
		pCmdUI->Enable(bDevLegal);
		break;

	case ID_FAULTFILE_GET:
		pCmdUI->Enable(bCommActive && bDevLegal && bFileSel);
		break;

	case ID_FAULTFILE_ANALYSIS:
		pCmdUI->Enable(bDevLegal && bFileSel);
		break;

	default:
		break;
	}
}

void CFaultFileFrame::OnRefresh()
{
	AdjustLayout();
	InitFaultFileFrmList(m_iStationIndex,m_iDevIndex);
}

void CFaultFileFrame::OnUpdateRefresh(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(true);
}
void CFaultFileFrame::OnOpenLocalMenu(void)
{
	if(m_iStationIndex < 0 || m_iStationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站序号错误，重新选择子站");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		return;
	}

	if (m_iDevIndex < 0 || m_iDevIndex >= theApp.g_sDataBases[m_iStationIndex].vDevs.size())
	{
		CString str = _T("装置序号错误，重新选择装置");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		return;
	}

	CString path;
	path += theApp.g_sDataBases.getRootCatalog();
	path += theApp.g_sDataBases[m_iStationIndex].getStationCatalog();
	path += theApp.g_sDataBases[m_iStationIndex].vDevs.getDevsCatalog();
	path += theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].getDevCatalog();
	path += theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].faultfiles.getFaultFilesCatalog();

	int fileIndex = getSelectedFileIndex();
	if (fileIndex >= 0 && fileIndex < theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].faultfiles.size())
	{
		path += CString(_T("\\")) + theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].faultfiles[fileIndex].getLocalFileName();

		int index = path.ReverseFind(_T('\\'));
		if (index >= 0)
		{
			path = path.Left(index);
		}
	}

	if (!FolderExist(path))
	{
		BuildDirectory(path);
	}

	ShellExecute(NULL, _T("open"), path,0, 0, SW_SHOWNORMAL);
}

void CFaultFileFrame::OnGetFaultFile(void)
{
	if(m_iStationIndex < 0 || m_iStationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站序号错误，重新选择子站");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		return;
	}

	if (!theApp.g_sDataBases[m_iStationIndex].getCommActive())
	{
		CString str = _T("通讯错误, 重新建立通讯");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		return;
	}

	if (m_iDevIndex < 0 || m_iDevIndex >= theApp.g_sDataBases[m_iStationIndex].vDevs.size())
	{
		CString str = _T("装置序号错误，重新选择装置");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		return;
	}

	int fileIndex = getSelectedFileIndex();
	if (fileIndex < 0 || fileIndex >= theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].faultfiles.size())
	{
		CString str = _T("故障文件选择错误,重新选择文件");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		RefreshFaultFileList(m_iStationIndex,m_iDevIndex);

		return;
	}

	theApp.g_sDataBases[m_iStationIndex].CommCallFaultFile(m_iStationIndex,m_iDevIndex,fileIndex);
}

void CFaultFileFrame::OnAnalysisFaultFile(void)
{
	if(m_iStationIndex < 0 || m_iStationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站序号错误，重新选择子站");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		return;
	}

	if (m_iDevIndex < 0 || m_iDevIndex >= theApp.g_sDataBases[m_iStationIndex].vDevs.size())
	{
		CString str = _T("装置序号错误，重新选择装置");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		return;
	}

	int fileIndex = getSelectedFileIndex();
	if (fileIndex < 0 || fileIndex >= theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].faultfiles.size())
	{
		CString str = _T("故障文件选择错误,重新选择文件");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		RefreshFaultFileList(m_iStationIndex,m_iDevIndex);

		return;
	}

	if (!theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].faultfiles[fileIndex].getFileBeExisted())
	{
		CString str = _T("文件不存在，重新下载");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		return;
	}

	CString pathName = theApp.g_sDataBases.getRootCatalog();
	pathName += theApp.g_sDataBases[m_iStationIndex].getStationCatalog();
	pathName += theApp.g_sDataBases[m_iStationIndex].vDevs.getDevsCatalog();
	pathName += theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].getDevCatalog();
	pathName += theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].faultfiles.getFaultFilesCatalog();
	pathName += CString(_T("\\")) + theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].faultfiles[fileIndex].getLocalFileName();

	if (!(FileExist(pathName + _T(".cfg")) && FileExist(pathName + _T(".dat"))))
	{
		CString str = _T("文件不存在，重新下载");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		return;
	}

	HINSTANCE hins = ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), _T("open"), CString(theApp.g_sDataBases.getOriginalCatalog()) + _T("\\WaveAnalyze.exe"),pathName + _T(".cfg"), 0, SW_SHOWMAXIMIZED);
}

BOOL CFaultFileFrame::PreTranslateMessage(MSG* pMsg)
{
	///*
	if (pMsg->message == WM_LBUTTONDBLCLK && pMsg->hwnd == m_wndShowList.GetSafeHwnd())
	{   
		OnLButtonDblClk(pMsg->wParam,pMsg->pt);

		::TranslateMessage(pMsg);   
		::DispatchMessage(pMsg);   

		return   TRUE;//   TRUE,   msg   is   processed   

	}
	//*/
	//   FALSE,   for   further   process   
	return CMDIChildWndEx::PreTranslateMessage(pMsg);
}

HWND CFaultFileFrame::getMyListWnd(void)
{
	return m_wndShowList.GetSafeHwnd();
}

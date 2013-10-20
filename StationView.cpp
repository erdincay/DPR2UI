
#include "stdafx.h"
#include "MainFrm.h"
#include "StationView.h"
#include "Resource.h"
#include "DPR2UI.h"
#include "Markup.h"

class CStationViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CStationView;

	DECLARE_SERIAL(CStationViewMenuButton)

public:
	CStationViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}


	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CStationViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// 构造/析构
//////////////////////////////////////////////////////////////////////

CStationView::CStationView()
{
	m_nCurrSort = ID_RW_SAVE;
	IAmBeingShowed = true;
}

CStationView::~CStationView()
{
	IAmBeingShowed = false;
	DeleteAllItems();
}	

BEGIN_MESSAGE_MAP(CStationView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_COMMAND(ID_NEW_FOLDER, OnNewStation)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_REFRESH,OnRefresh)
	ON_UPDATE_COMMAND_UI(ID_REFRESH,OnUpdateRefresh)
	ON_COMMAND_RANGE(ID_RW_SAVE, ID_RW_OPEN, OnRWIO)
	ON_UPDATE_COMMAND_UI_RANGE(ID_RW_SAVE, ID_RW_OPEN, OnUpdateRWIO)
	ON_COMMAND_RANGE(ID_STATION_ADD, ID_STATION_PROPERTY, OnPopStation)
	ON_UPDATE_COMMAND_UI_RANGE(ID_STATION_ADD,ID_STATION_PROPERTY,OnUpdatePopStation)
	ON_COMMAND_RANGE(ID_STATION_DIAL, ID_STATION_HANG, OnPopStation)
	ON_UPDATE_COMMAND_UI_RANGE(ID_STATION_DIAL, ID_STATION_HANG,OnUpdatePopStation)
	ON_COMMAND_RANGE(ID_FAULTFILE_MENU, ID_FAULTFILE_ANALYSIS, OnPopFaultFile)
	ON_UPDATE_COMMAND_UI_RANGE(ID_FAULTFILE_MENU,ID_FAULTFILE_ANALYSIS,OnUpdatePopFaultFile)
	ON_COMMAND_RANGE(ID_SETTING_VERITY, ID_SETTING_LOAD, OnPopSetting)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SETTING_VERITY,ID_SETTING_LOAD,OnUpdatePopSetting)
	ON_COMMAND_RANGE(ID_OP_TRIGRECORD, ID_OP_SETTIME, OnPopOperating)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OP_TRIGRECORD,ID_OP_SETTIME,OnUpdatePopOperating)
	
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStationView 消息处理程序

int CStationView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndStationView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("未能创建子站视图\n");
		return -1;      // 未能创建
	}

	// 加载图像:
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* 已锁定*/);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CStationViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CStationViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CStationViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	// 填入一些静态树视图数据(此处只需填入虚拟代码，而不是复杂的数据)
	CString xmlName = theApp.g_sDataBases.getRootCatalog();
	xmlName += _T("\\mAincFg.xml");
	if (FileExist(xmlName))
	{
		LoadCFG(xmlName);
	}
	
	FillStationsView();
	AdjustLayout();

	return 0;
}

void CStationView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();

	((CMainFrame *)AfxGetMainWnd())->RecalcLayout();
}

void CStationView::FillStationsView()
{
	CString strTmp;
	DeleteAllItems();

	for (UINT i=0; i<theApp.g_sDataBases.size();i++)
	{
		//子站树根
		HTREEITEM  hRoot;
		if (!theApp.g_sDataBases[i].getCommActive())
		{
			hRoot = m_wndStationView.InsertItem(theApp.g_sDataBases[i].getStationName(), 2, 2);
		}
		else
		{
			hRoot = m_wndStationView.InsertItem(theApp.g_sDataBases[i].getStationName(), 1, 1);
		}
		m_wndStationView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
		stTreeItemInfo * rootPtr = new stTreeItemInfo(i,NO_DEV_SEL,ROOT_ITEM_TYPE);
		m_wndStationView.SetItemData(hRoot,(DWORD_PTR)rootPtr);
		
		//子站ID枝
		strTmp.Format( _T("%.4d : %.3d"),theApp.g_sDataBases[i].getStationID(),theApp.g_sDataBases[i].getStationAddr());
		HTREEITEM hID = m_wndStationView.InsertItem(strTmp,4,4,hRoot);
		stTreeItemInfo * idPtr = new stTreeItemInfo(i,NO_DEV_SEL,ID_ITEM_TYPE);
		m_wndStationView.SetItemData(hID,(DWORD_PTR)idPtr);

		//子站通讯枝
		HTREEITEM hComm;
		if (theApp.g_sDataBases[i].getCommType() == COMM_TYPE_ETH)
		{
			strTmp = theApp.g_sDataBases[i].getStationIPAddr();
			strTmp += _T(":");
			strTmp.Format( strTmp + _T("%.4d"),theApp.g_sDataBases[i].getStationIPPort());
			hComm = m_wndStationView.InsertItem(strTmp,5,5,hRoot);
		}
		else if (theApp.g_sDataBases[i].getCommType() == COMM_TYPE_PHO)
		{
			hComm = m_wndStationView.InsertItem(theApp.g_sDataBases[i].getStationPhoneNO(),5,5,hRoot);
		}
		stTreeItemInfo * commPtr = new stTreeItemInfo(i,NO_DEV_SEL,COMM_ITEM_TYPE);
		m_wndStationView.SetItemData(hComm,(DWORD_PTR)commPtr);


		for (int j=0;j<theApp.g_sDataBases[i].vDevs.size();j++)
		{
			//装置ID枝
			strTmp.Format(_T("%.4d : %.3d"),theApp.g_sDataBases[i].vDevs[j].getDevID(),theApp.g_sDataBases[i].vDevs[j].getDevAddr());
			HTREEITEM hDev = m_wndStationView.InsertItem(strTmp,0,0,hRoot);
			stTreeItemInfo * devPtr = new stTreeItemInfo(i,j,DEV_ITEM_TYPE);
			m_wndStationView.SetItemData(hDev,(DWORD_PTR)devPtr);

			//装置故障文件枝
			HTREEITEM hFile = m_wndStationView.InsertItem(_T("故障文件"),6,6,hDev);
			stTreeItemInfo * filePtr = new stTreeItemInfo(i,j,FAULT_ITEM_TYPE);
			m_wndStationView.SetItemData(hFile,(DWORD_PTR)filePtr);
			

			//装置定值枝
			HTREEITEM hSet  = m_wndStationView.InsertItem(_T("装置定值"),3,3,hDev);
			stTreeItemInfo * setPtr = new stTreeItemInfo(i,j,SETTING_ITEM_TYPE);
			m_wndStationView.SetItemData(hSet,(DWORD_PTR)setPtr);


			//装置操作枝
			HTREEITEM hOP = m_wndStationView.InsertItem(_T("远方操作"),4,4,hDev);
			stTreeItemInfo * OpPtr = new stTreeItemInfo(i,j,OP_ITEM_TYPE);
			m_wndStationView.SetItemData(hOP,(DWORD_PTR)OpPtr);


			m_wndStationView.Expand(hDev, TVE_EXPAND);
			
		}

		m_wndStationView.Expand(hRoot, TVE_EXPAND);
	}

}

void CStationView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndStationView;
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

	CMenu menu;

	if (m_wndStationView.getCurItemInfo() == NULL)
	{
		menu.LoadMenu(IDR_POPUP_STATION);
	}
	else
	{
		int iStationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
		//int iDevIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
		int iItemType = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iItemType;
		bool bDialUp = false;
		if (iStationIndex >= 0 && iStationIndex < theApp.g_sDataBases.size())
		{
			if (theApp.g_sDataBases[iStationIndex].getCommType() == COMM_TYPE_PHO)
			{
				bDialUp = true;
			}
		}
		
		switch (iItemType)
		{
		case FAULT_ITEM_TYPE:
			menu.LoadMenu(IDR_POPUP_FAULTFILE);
			break;

		case SETTING_ITEM_TYPE:
			menu.LoadMenu(IDR_POPUP_SETTING);
			break;

		case OP_ITEM_TYPE:
			menu.LoadMenu(IDR_POPUP_OPERATING);
			break;

		default:
			if (bDialUp)
			{
				menu.LoadMenu(IDR_POPUP_STATION_DIAL);
			}
			else
			{
				menu.LoadMenu(IDR_POPUP_STATION);
			}
			break;
		}
	}
	
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

void CStationView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndStationView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CStationView::PreTranslateMessage(MSG* pMsg)
{
	///*
	if (pMsg->message == WM_LBUTTONDBLCLK && pMsg->hwnd == m_wndStationView.GetSafeHwnd())
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

void CStationView::OnRWIO(UINT id)
{
	m_nCurrSort = id;

	CStationViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CStationViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}

	switch (id)
	{
	case ID_RW_SAVE:
		OnSaveCFG();
		break;

	case ID_RW_LOAD:
		OnLoadCFG();
		break;

	case ID_RW_TEXT:
		OnTextCFG();
		break;

	case ID_RW_OPEN:
		OnOpenCFG();
		break;

	default:
		break;
	}
}

void CStationView::OnUpdateRWIO(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CStationView::OnPopFaultFile(UINT id)
{
	switch (id)
	{
	case ID_FAULTFILE_MENU:
		OnGetFaultFileMenu();
		break;

	case ID_FAULTFILE_SHOWMENU:
		OnShowFaultFileMenu();
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

void CStationView::OnUpdatePopFaultFile(CCmdUI* pCmdUI)
{
	int stationIndex = NO_STATION_SEl;
	int devIndex = NO_DEV_SEL;
	int fileIndex = ((CMainFrame *)AfxGetMainWnd())->getSeletedFaultFile();

	if (m_wndStationView.getCurItemInfo() != NULL)
	{
		stationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
		devIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	}

	bool bFileSel      = (fileIndex >= 0);
	bool bStationLegal = (theApp.g_sDataBases.size() > 0) && (stationIndex >= 0) && (stationIndex < theApp.g_sDataBases.size());
	bool bCommActive   = bStationLegal && (theApp.g_sDataBases[stationIndex].getCommActive());
	bool bDevLegal	   = bStationLegal && (theApp.g_sDataBases[stationIndex].vDevs.size() > 0) && (devIndex >= 0) && (devIndex < theApp.g_sDataBases[stationIndex].vDevs.size());

	switch (pCmdUI->m_nID)
	{
	case ID_FAULTFILE_MENU:
		pCmdUI->Enable(bCommActive && bDevLegal);
		break;

	case ID_FAULTFILE_SHOWMENU:
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

void CStationView::OnPopSetting(UINT id)
{
	switch(id)
	{
	case ID_SETTING_VERITY:
		OnVeritySetting();
		break;

	case ID_SETTING_SHOW:
		OnShowSetting();
		break;

	case ID_SETTING_GET:
		OnGetSetting();
		break;

	case ID_SETTING_PUT:
		OnPutSetting();
		break;

	case ID_SETTING_SAVE:
		OnSaveSetting();
		break;

	case ID_SETTING_LOAD:
		OnLoadSetting();
		break;

	default:
		break;
	}
}

void CStationView::OnUpdatePopSetting(CCmdUI* pCmdUI)
{
	int stationIndex = NO_STATION_SEl;
	int devIndex = NO_DEV_SEL;

	if (m_wndStationView.getCurItemInfo() != NULL)
	{
		stationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
		devIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	}

	bool bSettingOpen = (((CMainFrame *)AfxGetMainWnd())->m_wndSetting != NULL);
	bool bStationLegal = (theApp.g_sDataBases.size() > 0) && (stationIndex >= 0) && (stationIndex < theApp.g_sDataBases.size());
	bool bCommActive   = bStationLegal && (theApp.g_sDataBases[stationIndex].getCommActive());
	bool bDevLegal	   = bStationLegal && (theApp.g_sDataBases[stationIndex].vDevs.size() > 0) && (devIndex >= 0) && (devIndex < theApp.g_sDataBases[stationIndex].vDevs.size());

	switch(pCmdUI->m_nID)
	{
	case ID_SETTING_VERITY:
		pCmdUI->Enable(bSettingOpen);
		break;

	case ID_SETTING_SHOW:
		pCmdUI->Enable(bStationLegal && bDevLegal);
		break;

	case ID_SETTING_GET:
		pCmdUI->Enable(bCommActive && bDevLegal);
		break;

	case ID_SETTING_PUT:
		pCmdUI->Enable(bCommActive && bDevLegal && bSettingOpen);
		break;

	case ID_SETTING_SAVE:
		pCmdUI->Enable(bSettingOpen);
		break;

	case ID_SETTING_LOAD:
		pCmdUI->Enable(true);
		break;

	default:
		break;
	}
}

void CStationView::OnPopStation(UINT id)
{
	switch (id)
	{
	case ID_STATION_ADD:
		OnNewStation();
		break;

	case ID_STATION_DEL_ALL:
		OnDelAllStation();
		break;

	case ID_STATION_DEL:
		OnDelStation();
		break;

	case ID_STASION_CONNECT:
		OnConnectStation();
		break;

	case ID_STASION_DISCONNECT:
		OnDisConnectStation();
		break;

	case ID_DEV_ADD:
		OnNewDev();
		break;

	case ID_DEV_DEL:
		OnDelDev();
		break;

	case ID_DEV_DEL_ALL:
		OnDelAllDev();
		break;

	case ID_STATION_PROPERTY:
		OnProperty();
		break;

	case ID_OPEN_LOCAL_MENU:
		OnOpenLocalMenu();
		break;

	case ID_STATION_DIAL:
		OnDialUp();
		break;

	case ID_STATION_HANG:
		OnHangOff();
		break;

	default:
		break;
	}



}

void CStationView::OnUpdatePopStation(CCmdUI* pCmdUI)
{
	int stationIndex = NO_STATION_SEl;
	int devIndex = NO_DEV_SEL;

	if (m_wndStationView.getCurItemInfo() != NULL)
	{
		stationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
		devIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	}
	
	bool bStationLegal = (theApp.g_sDataBases.size() > 0) && (stationIndex >= 0) && (stationIndex < theApp.g_sDataBases.size());
	bool bCommActive   = bStationLegal && (theApp.g_sDataBases[stationIndex].getCommActive());
	bool bDevLegal	   = bStationLegal && (theApp.g_sDataBases[stationIndex].vDevs.size() > 0) && (devIndex >= 0) && (devIndex < theApp.g_sDataBases[stationIndex].vDevs.size());

	switch (pCmdUI->m_nID)
	{
	case ID_STATION_ADD:
		pCmdUI->Enable(theApp.g_sDataBases.size() < MAX_STATION_SUM);
		break;

	case ID_STATION_DEL_ALL:
		pCmdUI->Enable(theApp.g_sDataBases.size() > 0);
		break;

	case ID_STATION_DEL:
		pCmdUI->Enable(bStationLegal);
		break;

	case ID_STASION_CONNECT:
		pCmdUI->Enable((!bCommActive) && bStationLegal);
		break;

	case ID_STASION_DISCONNECT:
		pCmdUI->Enable(bCommActive);
		break;

	case ID_STATION_PROPERTY:
		pCmdUI->Enable(bStationLegal);
		break;

	case ID_DEV_ADD:
		pCmdUI->Enable(bStationLegal);
		break;

	case ID_DEV_DEL:
		pCmdUI->Enable(bDevLegal);
		break;

	case ID_DEV_DEL_ALL:
		pCmdUI->Enable(bStationLegal);
		break;

	case ID_OPEN_LOCAL_MENU:
		pCmdUI->Enable(bStationLegal);
		break;

	case ID_STATION_DIAL:
		pCmdUI->Enable(bStationLegal);
		break;

	case ID_STATION_HANG:
		pCmdUI->Enable(bStationLegal);
		break;

	default:
		break;
	}
}

void CStationView::OnPopOperating(UINT id)
{
	switch (id)
	{
	case ID_OP_TRIGRECORD:
		OnTrigRecord();
		break;

	case ID_OP_RECVTIME:
		OnRecvTime();
		break;

	case ID_OP_SETTIME:
		OnSetTime();
		break;

	default:
		break;
	}
}

void CStationView::OnUpdatePopOperating(CCmdUI* pCmdUI)
{
	int stationIndex = NO_STATION_SEl;
	int devIndex = NO_DEV_SEL;

	if (m_wndStationView.getCurItemInfo() != NULL)
	{
		stationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
		devIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	}

	bool bStationLegal = (theApp.g_sDataBases.size() > 0) && (stationIndex >= 0) && (stationIndex < theApp.g_sDataBases.size());
	bool bCommActive   = bStationLegal && (theApp.g_sDataBases[stationIndex].getCommActive());
	bool bDevLegal	   = bStationLegal && (theApp.g_sDataBases[stationIndex].vDevs.size() > 0) && (devIndex >= 0) && (devIndex < theApp.g_sDataBases[stationIndex].vDevs.size());

	switch (pCmdUI->m_nID)
	{
	case ID_OP_TRIGRECORD:
		pCmdUI->Enable(bCommActive && bDevLegal);
		break;

	case ID_OP_RECVTIME:
		pCmdUI->Enable(bCommActive && bDevLegal);
		break;

	case ID_OP_SETTIME:
		pCmdUI->Enable(bCommActive && bDevLegal);
		break;

	default:
		break;
	}
}

void CStationView::OnClassAddMemberFunction()
{
	
}

void CStationView::OnClassAddMemberVariable()
{
	// TODO: 在此处添加命令处理程序代码
}

void CStationView::OnClassDefinition()
{
	// TODO: 在此处添加命令处理程序代码
}

void CStationView::OnClassProperties()
{
	// TODO: 在此处添加命令处理程序代码
}

void CStationView::OnNewStation()
{
	bool bConnect = false;
	for (int i = 0;i<theApp.g_sDataBases.size();i++)
	{
		if (theApp.g_sDataBases[i].getCommActive())
		{
			bConnect = true;
		}
	}

	if (bConnect)
	{
		AfxMessageBox(_T("请先断开各个子站连接再添加子站"));
		return;
	}

	UINT index;
	if (theApp.g_sDataBases.size() <= 0)
	{
		index = 0;
	}
	else
	{
		index = theApp.g_sDataBases.size();
	}
	CDPRStation m_station(index);
	theApp.g_sDataBases.push_back(m_station);

	FillStationsView();	
}

void CStationView::OnDelAllStation()
{
	bool bConnect = false;
	for (int i = 0;i<theApp.g_sDataBases.size();i++)
	{
		if (theApp.g_sDataBases[i].getCommActive())
		{
			bConnect = true;
		}
	}

	if (bConnect)
	{
		AfxMessageBox(_T("请先断开各个子站连接再删除子站"));
		return;
	}

	theApp.g_sDataBases.clear();

	FillStationsView();

	((CMainFrame *)AfxGetMainWnd())->CloseFaultFileFrm();
	((CMainFrame *)AfxGetMainWnd())->CloseSettingFrm();
}
void CStationView::OnDelStation()
{
	if (theApp.g_sDataBases.size() <= 0)
	{
		CString str = _T("子站数量为0，无法删除");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}
	
	HTREEITEM curItem = m_wndStationView.GetSelectedItem();
	if (curItem == NULL)
	{
		CString str = _T("未选择子站，无法删除");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	bool bConnect = false;
	for (int i = 0;i<theApp.g_sDataBases.size();i++)
	{
		if (theApp.g_sDataBases[i].getCommActive())
		{
			bConnect = true;
		}
	}

	if (bConnect)
	{
		AfxMessageBox(_T("请先断开各个子站连接再删除子站"));
		return;
	}

	int index = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (index < 0 || index >= theApp.g_sDataBases.size())
	{
		CString str = _T("未选择子站，无法删除");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	
	theApp.g_sDataBases.erase(theApp.g_sDataBases.begin() + index);

	FillStationsView();

	((CMainFrame *)AfxGetMainWnd())->CloseFaultFileFrm();
	((CMainFrame *)AfxGetMainWnd())->CloseSettingFrm();

}

/*
const stTreeItemInfo * CStationView::getCurItemInfo(void)
{
	HTREEITEM curItem = m_wndStationView.GetSelectedItem();
	if (curItem == NULL)
	{
		return NULL;
	}

	stTreeItemInfo * curData = (stTreeItemInfo *)m_wndStationView.GetItemData(curItem);
	
	return curData;
}
*/

/*
int CStationView::setCurInfoToDataBase(void)
{
	if (m_wndStationView.getCurItemInfo() == NULL)
	{
		return -1;
	}

	int ret = theApp.g_sDataBases.setCurStationIndex(((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex);
	if (ret == 0 && ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex > 0)
	{
		theApp.g_sDataBases[((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex].setCurDevIndex(((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex);
	}

	return 0;
}
*/

void CStationView::OnProperty()
{
	if (m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int stationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;

	if (stationIndex < 0 || stationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("选择装置序号错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int devIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	int fileIndex = ((CMainFrame *)AfxGetMainWnd())->getSeletedFaultFile();

	((CMainFrame *)AfxGetMainWnd())->RefreshPropertyView(stationIndex,devIndex,fileIndex);
	
}

void CStationView::OnOpenLocalMenu()
{
	if (m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int stationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;

	if (stationIndex < 0 || stationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("选择装置序号错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int devIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;

	int ItemType = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iItemType;

	CString path = _T("");

	switch (ItemType)
	{
	case ROOT_ITEM_TYPE:
		path = theApp.g_sDataBases.getRootCatalog();
		path += theApp.g_sDataBases[stationIndex].getStationCatalog();
		break;

	case ID_ITEM_TYPE:
		path = theApp.g_sDataBases.getRootCatalog();
		path += theApp.g_sDataBases[stationIndex].getStationCatalog();
		break;

	case COMM_ITEM_TYPE:
		path = theApp.g_sDataBases.getRootCatalog();
		path += theApp.g_sDataBases[stationIndex].getStationCatalog();
		break;

	case DEV_ITEM_TYPE:
		path = theApp.g_sDataBases.getRootCatalog();
		path += theApp.g_sDataBases[stationIndex].getStationCatalog();
		path += theApp.g_sDataBases[stationIndex].vDevs.getDevsCatalog();
		if (devIndex >= 0 && devIndex < theApp.g_sDataBases[stationIndex].vDevs.size())
		{
			path += theApp.g_sDataBases[stationIndex].vDevs[devIndex].getDevCatalog();
		}
		break;

	case FAULT_ITEM_TYPE:
		path = theApp.g_sDataBases.getRootCatalog();
		path += theApp.g_sDataBases[stationIndex].getStationCatalog();
		path += theApp.g_sDataBases[stationIndex].vDevs.getDevsCatalog();
		if (devIndex >= 0 && devIndex < theApp.g_sDataBases[stationIndex].vDevs.size())
		{
			path += theApp.g_sDataBases[stationIndex].vDevs[devIndex].getDevCatalog();
			path += theApp.g_sDataBases[stationIndex].vDevs[devIndex].faultfiles.getFaultFilesCatalog();
		}
		break;

	case SETTING_ITEM_TYPE:
		path = theApp.g_sDataBases.getRootCatalog();
		path += theApp.g_sDataBases[stationIndex].getStationCatalog();
		path += theApp.g_sDataBases[stationIndex].vDevs.getDevsCatalog();
		if (devIndex >= 0 && devIndex < theApp.g_sDataBases[stationIndex].vDevs.size())
		{
			path += theApp.g_sDataBases[stationIndex].vDevs[devIndex].getDevCatalog();
			path += theApp.g_sDataBases[stationIndex].vDevs[devIndex].setting.getSettingCatalog();
		}
		break;

	default:
		break;
	}

	if (!FolderExist(path))
	{
		BuildDirectory(path);
	}

	ShellExecute(NULL, _T("open"), path,0, 0, SW_SHOWNORMAL);
}

void CStationView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndStationView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CStationView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndStationView.SetFocus();
}

void CStationView::OnChangeVisualStyle()
{
	m_StationViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

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

	m_StationViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_StationViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndStationView.SetImageList(&m_StationViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* 锁定*/);
}

bool CStationView::getbShow(void)
{
	return IAmBeingShowed;
}

void CStationView::setbShow(bool val)
{
	IAmBeingShowed = val;
}
void CStationView::RefreshStationView(void)
{
	FillStationsView();
}

void CStationView::OnConnectStation(void)
{
	if (m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择子站，无法连接");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}
	int stationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (stationIndex < 0 || stationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("未选择子站，无法连接");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	theApp.g_sDataBases[stationIndex].OpenConnect();
	
}

void CStationView::OnDisConnectStation(void)
{
	if (m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择子站，无法断开连接");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}
	int stationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (stationIndex < 0 || stationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("未选择子站，无法断开连接");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}
		
	theApp.g_sDataBases[stationIndex].CloseConnect();
}

void CStationView::RefreshStationImage(void)
{
	HTREEITEM lef = m_wndStationView.GetRootItem();

	while (lef != NULL)
	{
		int stationIndex = ((stTreeItemInfo *)m_wndStationView.GetItemData(lef))->iStationIndex;
		if (stationIndex >= 0 && stationIndex < theApp.g_sDataBases.size())
		{
			if(!theApp.g_sDataBases[stationIndex].getCommActive())
			{
				m_wndStationView.SetItemImage(lef,2,2);
			}
			else
			{
				m_wndStationView.SetItemImage(lef,1,1);
			}
		}

		lef = m_wndStationView.GetNextSiblingItem(lef);
	}
}

void CStationView::OnGetFaultFileMenu(void)
{
	if(m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int StationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (StationIndex < 0 || StationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	if (!theApp.g_sDataBases[StationIndex].getCommActive())
	{
		CString str = _T("通讯错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int DevIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	if ( DevIndex < 0 || DevIndex >= theApp.g_sDataBases[StationIndex].vDevs.size())
	{
		CString str = _T("装置选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	theApp.g_sDataBases[StationIndex].CommCallFaultFileMenu(StationIndex,DevIndex);
}

void CStationView::OnGetFaultFile(void)
{
	if(m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int StationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (StationIndex < 0 || StationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	if (!theApp.g_sDataBases[StationIndex].getCommActive())
	{
		CString str = _T("通讯错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int DevIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	if (DevIndex < 0 || DevIndex >= theApp.g_sDataBases[StationIndex].vDevs.size())
	{
		CString str = _T("装置选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int fileIndex = ((CMainFrame *)AfxGetMainWnd())->getSeletedFaultFile();
	if (fileIndex < 0 || fileIndex >= theApp.g_sDataBases[StationIndex].vDevs[DevIndex].faultfiles.size())
	{
		CString str = _T("故障文件选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		((CMainFrame *)AfxGetMainWnd)->RefreshFaultFileFrm(StationIndex,DevIndex);

		return;
	}

	theApp.g_sDataBases[StationIndex].CommCallFaultFile(StationIndex,DevIndex,fileIndex);
}

void CStationView::OnAnalysisFaultFile(void)
{
	if(m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int StationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (StationIndex < 0 || StationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int DevIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	if (DevIndex < 0 || DevIndex >= theApp.g_sDataBases[StationIndex].vDevs.size())
	{
		CString str = _T("装置选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int fileIndex = ((CMainFrame *)AfxGetMainWnd())->getSeletedFaultFile();
	if (fileIndex < 0 || fileIndex >= theApp.g_sDataBases[StationIndex].vDevs[DevIndex].faultfiles.size())
	{
		CString str = _T("故障文件选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		((CMainFrame *)AfxGetMainWnd)->RefreshFaultFileFrm(StationIndex,DevIndex);

		return;
	}

	if (!theApp.g_sDataBases[StationIndex].vDevs[DevIndex].faultfiles[fileIndex].getFileBeExisted())
	{
		CString str = _T("文件不存在，重新下载");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	CString pathName = theApp.g_sDataBases.getRootCatalog();
	pathName += theApp.g_sDataBases[StationIndex].getStationCatalog();
	pathName += theApp.g_sDataBases[StationIndex].vDevs.getDevsCatalog();
	pathName += theApp.g_sDataBases[StationIndex].vDevs[DevIndex].getDevCatalog();
	pathName += theApp.g_sDataBases[StationIndex].vDevs[DevIndex].faultfiles.getFaultFilesCatalog();
	pathName += CString(_T("\\")) + theApp.g_sDataBases[StationIndex].vDevs[DevIndex].faultfiles[fileIndex].getLocalFileName();

	if (!(FileExist(pathName + _T(".cfg")) && FileExist(pathName + _T(".dat"))))
	{
		CString str = _T("文件不存在，重新下载");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), _T("open"), CString(theApp.g_sDataBases.getOriginalCatalog()) + _T("\\WaveAnalyze.exe"),pathName + _T(".cfg"), 0, SW_SHOWMAXIMIZED);

	/*
	SHELLEXECUTEINFO ShExecInfo = {0};
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = curPath + _T("\\WaveAnalyze.exe");
	ShExecInfo.lpParameters = pathName + _T(".cfg");
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	//WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
	*/

	/*
	CString cmd = _T(".\\WaveAnalyze.exe ") + pathName + _T(".cfg");
	LPCSTR lpcs = WCHARtoChar(cmd.GetBuffer());
	cmd.ReleaseBuffer();
	WinExec(lpcs,SW_SHOW);
	delete lpcs;	
	*/
}

void CStationView::OnVeritySetting(void)
{
	if(((CMainFrame *)AfxGetMainWnd())->m_wndSetting == NULL)
	{
		return;
	}

	((CMainFrame *)AfxGetMainWnd())->m_wndSetting->SetFocus();
}

void CStationView::OnGetSetting(void)
{
	if(m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int StationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (StationIndex < 0 || StationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	if (!theApp.g_sDataBases[StationIndex].getCommActive())
	{
		CString str = _T("通讯错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int DevIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	if ( DevIndex < 0 || DevIndex >= theApp.g_sDataBases[StationIndex].vDevs.size())
	{
		CString str = _T("装置选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	theApp.g_sDataBases[StationIndex].CommGetSettingCFG(StationIndex,DevIndex);
}

void CStationView::OnPutSetting(void)
{
	if(m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int StationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (StationIndex < 0 || StationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	if (!theApp.g_sDataBases[StationIndex].getCommActive())
	{
		CString str = _T("通讯错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int DevIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	if ( DevIndex < 0 || DevIndex >= theApp.g_sDataBases[StationIndex].vDevs.size())
	{
		CString str = _T("装置选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	OnSaveSetting();
	theApp.g_sDataBases[StationIndex].CommPutSettingCFG(StationIndex,DevIndex);
}

void CStationView::OnSaveSetting(void)
{
	if(m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int StationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (StationIndex < 0 || StationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int DevIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	if (DevIndex < 0 || DevIndex >= theApp.g_sDataBases[StationIndex].vDevs.size())
	{
		CString str = _T("装置选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int ret = ((CMainFrame *)AfxGetMainWnd())->SaveSettingVal(StationIndex,DevIndex);
	if (ret)
	{
		return;
	}

	CString fileMenu = theApp.g_sDataBases.getRootCatalog();
	fileMenu += theApp.g_sDataBases[StationIndex].getStationCatalog();
	fileMenu += theApp.g_sDataBases[StationIndex].vDevs.getDevsCatalog();
	fileMenu += theApp.g_sDataBases[StationIndex].vDevs[DevIndex].getDevCatalog();
	fileMenu += theApp.g_sDataBases[StationIndex].vDevs[DevIndex].setting.getSettingCatalog();

	if (!FolderExist(fileMenu))
	{
		BuildDirectory(fileMenu);
	}

	fileMenu += _T("\\");
	fileMenu += _T(STR_SETCFG_NAME);

	theApp.g_sDataBases[StationIndex].vDevs[DevIndex].setting.SaveSettingCFG(fileMenu);

	((CMainFrame *)AfxGetMainWnd())->RefreshFileView();
}

void CStationView::OnLoadSetting(void)
{
	if(m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int StationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (StationIndex < 0 || StationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int DevIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	if (DevIndex < 0 || DevIndex >= theApp.g_sDataBases[StationIndex].vDevs.size())
	{
		CString str = _T("装置选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	CString fileMenu = theApp.g_sDataBases.getRootCatalog();
	fileMenu += theApp.g_sDataBases[StationIndex].getStationCatalog();
	fileMenu += theApp.g_sDataBases[StationIndex].vDevs.getDevsCatalog();
	fileMenu += theApp.g_sDataBases[StationIndex].vDevs[DevIndex].getDevCatalog();
	fileMenu += theApp.g_sDataBases[StationIndex].vDevs[DevIndex].setting.getSettingCatalog();

	CFileDialog fdlg(true);
	fdlg.m_ofn.lpstrFilter = _T("(*.win)\0*.win\0\0");
	fdlg.m_ofn.hInstance = AfxGetInstanceHandle();
	fdlg.m_ofn.lpstrInitialDir = fileMenu;

	if (fdlg.DoModal() == IDCANCEL)
	{
		return;
	}

	//CString fileName = fdlg.GetFileName();
	CString filePath = fdlg.GetPathName();

	if (FileExist(filePath))
	{
		theApp.g_sDataBases[StationIndex].vDevs[DevIndex].setting.LoadSettingCFG(filePath);
		((CMainFrame *)AfxGetMainWnd())->RefreshSettingFrm(StationIndex,DevIndex);
	}
}

void CStationView::OnShowFaultFileMenu(void)
{
	((CMainFrame *)AfxGetMainWnd())->RefreshFaultFileFrm(((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex,((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex);
}

void CStationView::OnShowSetting(void)
{
	((CMainFrame *)AfxGetMainWnd())->RefreshSettingFrm(((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex,((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex);
}

/*
void CStationView::ShowPane(BOOL bShow,BOOL bDelay,BOOL bActivate)
{
	CDockablePane::ShowPane(bShow,bDelay,bActivate);
	setbShow(bShow);
	((CMainFrame *)AfxGetMainWnd())->RecalcLayout();
}
*/

void CStationView::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDockablePane::OnShowWindow(bShow, nStatus);

	// TODO: 在此处添加消息处理程序代码
	setbShow(bShow);
	((CMainFrame *)AfxGetMainWnd())->RecalcLayout();
}
void CStationView::LoadCFG(CString fileName)
{
	if (!FileExist(fileName))
	{
		return;
	}

	CMarkup xml;

	if(!xml.Load(fileName))
	{
		return;
	}

	theApp.g_sDataBases.clear();

	xml.ResetMainPos();
	xml.FindElem(); //root
	xml.IntoElem();

	CString strTmp;
	CDPRStation dprTmp;
	CDPRDev devTmp;
	while(xml.FindElem(_T("sTatIOn")))
	{
		dprTmp.vDevs.clear();

		if (xml.FindChildElem(_T("Name")))
		{
			strTmp = xml.GetChildData();
			dprTmp.setStationName(strTmp.GetBuffer());
			strTmp.ReleaseBuffer();
		}

		if (xml.FindChildElem(_T("ID")))
		{
			strTmp = xml.GetChildData();
			int iVal = 0;
			if(CString2Int(strTmp,iVal))
			{
				dprTmp.setStationID(abs(iVal));
			}
		}

		if (xml.FindChildElem(_T("Addr")))
		{
			strTmp = xml.GetChildData();
			int iVal = 0;
			if(CString2Int(strTmp,iVal))
			{
				dprTmp.setStationAddr(abs(iVal));
			}
		}

		if (xml.FindChildElem(_T("CommType")))
		{
			strTmp = xml.GetChildData();
			int iVal = 0;
			if (CString2Int(strTmp,iVal))
			{
				dprTmp.setCommType(iVal);
			}
		}

		if (xml.FindChildElem(_T("IP")))
		{
			strTmp = xml.GetChildData();
			dprTmp.setStationIPAddr(strTmp.GetBuffer());
			strTmp.ReleaseBuffer();
		}

		if (xml.FindChildElem(_T("Port")))
		{
			strTmp = xml.GetChildData();
			int iVal = 0;
			if(CString2Int(strTmp,iVal))
			{
				dprTmp.setStationIPPort(abs(iVal));
			}
		}

		if (xml.FindChildElem(_T("Modem")))
		{
			strTmp = xml.GetChildData();
			int iVal = 0;
			if(CString2Int(strTmp,iVal))
			{
				dprTmp.setModemIndex(abs(iVal));
			}
		}

		if (xml.FindChildElem(_T("Phone")))
		{
			strTmp = xml.GetChildData();
			dprTmp.setStationPhoneNO(strTmp.GetBuffer());
			strTmp.ReleaseBuffer();
		}

		if (xml.FindChildElem(_T("Usr")))
		{
			strTmp = xml.GetChildData();
			dprTmp.setPhoneUsrName(strTmp.GetBuffer());
			strTmp.ReleaseBuffer();
		}

		if (xml.FindChildElem(_T("Pas")))
		{
			strTmp = xml.GetChildData();
			dprTmp.setPhonePassWord(strTmp.GetBuffer());
			strTmp.ReleaseBuffer();
		}

		while (xml.FindChildElem(_T("dEv")))
		{
			xml.IntoElem();
			if (xml.FindChildElem(_T("ID")))
			{
				strTmp = xml.GetChildData();
				int iVal = 0;
				if(CString2Int(strTmp,iVal))
				{
					devTmp.setDevID(abs(iVal));
				}
			}

			if (xml.FindChildElem(_T("Addr")))
			{
				strTmp = xml.GetChildData();
				int iVal = 0;
				if(CString2Int(strTmp,iVal))
				{
					devTmp.setDevAddr(abs(iVal));
				}
			}
			xml.OutOfElem();
			dprTmp.vDevs.push_back(devTmp);
		}

		theApp.g_sDataBases.push_back(dprTmp);
	}
}


void CStationView::OnSaveCFG(void)
{
	CMarkup xml;
	xml.SetDoc(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"));

	xml.AddElem(_T("mAincFg"));
	xml.IntoElem();
	for (int i=0;i<theApp.g_sDataBases.size();i++)
	{
		xml.AddElem(_T("sTatIOn"));
		xml.AddChildElem(_T("Name"),theApp.g_sDataBases[i].getStationName());
		xml.AddChildElem(_T("ID"),theApp.g_sDataBases[i].getStationID());
		xml.AddChildElem(_T("Addr"),theApp.g_sDataBases[i].getStationAddr());
		xml.AddChildElem(_T("CommType"),theApp.g_sDataBases[i].getCommType());
		xml.AddChildElem(_T("IP"),theApp.g_sDataBases[i].getStationIPAddr());
		xml.AddChildElem(_T("Port"),theApp.g_sDataBases[i].getStationIPPort());
		xml.AddChildElem(_T("Modem"),theApp.g_sDataBases[i].getModemIndex());
		xml.AddChildElem(_T("Phone"),theApp.g_sDataBases[i].getStationPhoneNO());
		xml.AddChildElem(_T("Usr"),theApp.g_sDataBases[i].getPhoneUsrName());
		xml.AddChildElem(_T("Pas"),theApp.g_sDataBases[i].getPhonePassWord());
		xml.IntoElem();
		for (int j=0;j<theApp.g_sDataBases[i].vDevs.size();j++)
		{
			xml.AddElem(_T("dEv"));
			xml.AddChildElem(_T("ID"),theApp.g_sDataBases[i].vDevs[j].getDevID());
			xml.AddChildElem(_T("Addr"),theApp.g_sDataBases[i].vDevs[j].getDevAddr());
		}
		xml.OutOfElem();
	}
	xml.OutOfElem();

	//结束 存档
	CString pathName = CString(theApp.g_sDataBases.getRootCatalog());
	if (!FolderExist(pathName))
	{
		BuildDirectory(pathName);
	}

	pathName += _T("\\mAincFg.xml");
	xml.Save(pathName);

	((CMainFrame *)AfxGetMainWnd())->RefreshFileView();
}

void CStationView::OnLoadCFG(void)
{
	CFileDialog fdlg(true);

	fdlg.m_ofn.lpstrFilter = _T("(*.xml)\0*.xml\0\0");
	fdlg.m_ofn.hInstance = AfxGetInstanceHandle();
	fdlg.m_ofn.lpstrInitialDir = theApp.g_sDataBases.getRootCatalog();

	if (fdlg.DoModal() == IDCANCEL)
	{
		return;
	}

	//CString fileName = fdlg.GetFileName();
	CString filePath = fdlg.GetPathName();

	LoadCFG(filePath);

	FillStationsView();
}

void CStationView::OnOpenCFG(void)
{
	CString pathName = CString(theApp.g_sDataBases.getRootCatalog());
	pathName += _T("\\mAincFg.xml");
	if (FileExist(pathName))
	{
		ShellExecute(NULL, _T("open"), pathName,0, 0, SW_SHOWNORMAL);
	}
}

void CStationView::OnTextCFG(void)
{
}

void CStationView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	stTreeItemInfo * item = (stTreeItemInfo *)m_wndStationView.getCurItemInfo();

	if (item == NULL)
	{
		return;
	}

	switch (item->iItemType)
	{
	case ROOT_ITEM_TYPE:
		OnProperty();
		break;

	case ID_ITEM_TYPE:
		OnProperty();
		break;

	case COMM_ITEM_TYPE:
		OnProperty();
		break;

	case DEV_ITEM_TYPE:
		OnProperty();
		break;

	case FAULT_ITEM_TYPE:
		OnShowFaultFileMenu();
		break;

	case SETTING_ITEM_TYPE:
		OnShowSetting();
		break;

	case OP_ITEM_TYPE:
		break;

	default:
		break;
	}

	CDockablePane::OnLButtonDblClk(nFlags, point);
}

void CStationView::OnRefresh()
{
	RefreshStationView();
}
void CStationView::OnUpdateRefresh(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(true);
}
void CStationView::OnNewDev(void)
{
	if(m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int StationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (StationIndex < 0 || StationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	if (theApp.g_sDataBases[StationIndex].vDevs.size() > MAX_DEV_SUM)
	{
		CString str;
		str.Format(_T("装置数量超过限值%d"),MAX_DEV_SUM);
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	CDPRDev dev(&theApp.g_sDataBases[StationIndex].vDevs);
	theApp.g_sDataBases[StationIndex].vDevs.push_back(dev);

	FillStationsView();
}

void CStationView::OnDelDev(void)
{
	HTREEITEM curItem = m_wndStationView.GetSelectedItem();
	if (curItem == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int StationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (StationIndex < 0 || StationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int DevIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	if (DevIndex < 0 || DevIndex >= theApp.g_sDataBases[StationIndex].vDevs.size())
	{
		CString str = _T("装置选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	if (theApp.g_sDataBases[StationIndex].vDevs.size() > 0)
	{
		theApp.g_sDataBases[StationIndex].vDevs.erase(theApp.g_sDataBases[StationIndex].vDevs.begin() + DevIndex);
	}

	FillStationsView();
}

void CStationView::OnDelAllDev(void)
{
	if(m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int StationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (StationIndex < 0 || StationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	if (theApp.g_sDataBases[StationIndex].vDevs.size() > 0)
	{
		theApp.g_sDataBases[StationIndex].vDevs.clear();
	}

	FillStationsView();
}

void CStationView::OnTrigRecord()
{
	if(m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int StationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (StationIndex < 0 || StationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	if (!theApp.g_sDataBases[StationIndex].getCommActive())
	{
		CString str = _T("通讯错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int DevIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	if ( DevIndex < 0 || DevIndex >= theApp.g_sDataBases[StationIndex].vDevs.size())
	{
		CString str = _T("装置选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	theApp.g_sDataBases[StationIndex].CommSendTrigRecord(StationIndex,DevIndex);
}

void CStationView::OnRecvTime()
{
	if(m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int StationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (StationIndex < 0 || StationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	if (!theApp.g_sDataBases[StationIndex].getCommActive())
	{
		CString str = _T("通讯错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int DevIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	if ( DevIndex < 0 || DevIndex >= theApp.g_sDataBases[StationIndex].vDevs.size())
	{
		CString str = _T("装置选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	theApp.g_sDataBases[StationIndex].CommCallDevTime(StationIndex,DevIndex);
}

void CStationView::OnSetTime()
{
	if(m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择项目");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int StationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (StationIndex < 0 || StationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	if (!theApp.g_sDataBases[StationIndex].getCommActive())
	{
		CString str = _T("通讯错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	int DevIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iDevIndex;
	if ( DevIndex < 0 || DevIndex >= theApp.g_sDataBases[StationIndex].vDevs.size())
	{
		CString str = _T("装置选择错误");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	SYSTEMTIME tm;
	GetLocalTime(&tm);

	theApp.g_sDataBases[StationIndex].vDevs[DevIndex].setDevTime(tm);
	theApp.g_sDataBases[StationIndex].CommSetDevTime(StationIndex,DevIndex);
}

BOOL CStationView::DeleteAllItems()
{
	HTREEITEM curItem = m_wndStationView.GetRootItem();
	while (curItem != NULL)
	{
		DWORD_PTR curData = m_wndStationView.GetItemData(curItem);
		if (curData != NULL)
		{
			delete (stTreeItemInfo *)curData;
		}

		curItem = m_wndStationView.GetNextVisibleItem(curItem);
	}

	return m_wndStationView.DeleteAllItems();
}

void CStationView::OnDialUp()
{
	if (m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择子站，无法拨号");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}
	int stationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (stationIndex < 0 || stationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("未选择子站，无法拨号");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	theApp.g_sDataBases[stationIndex].DialUp();
}

void CStationView::OnHangOff()
{
	if (m_wndStationView.getCurItemInfo() == NULL)
	{
		CString str = _T("未选择子站，无法断开");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}
	int stationIndex = ((stTreeItemInfo *)m_wndStationView.getCurItemInfo())->iStationIndex;
	if (stationIndex < 0 || stationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("未选择子站，无法断开");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	theApp.g_sDataBases[stationIndex].HangOff();
}

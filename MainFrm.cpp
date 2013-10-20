#include "stdafx.h"
#include "DPR2UI.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int PROGRESS_CTRL_TIMEOUT_ID = 1;
const int PROGRESS_CTRL_TIMEOUT_VAL = 300000; //单位 毫秒

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND(ID_WINDOW_MANAGER, &CMainFrame::OnWindowManager)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_OFF_2007_AQUA, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND_RANGE(ID_VIEW_TOOLBAR, ID_VIEW_STATUS_BAR, &CMainFrame::OnViewChoose)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_TOOLBAR, ID_VIEW_STATUS_BAR, &CMainFrame::OnUpdateViewChoose)
	ON_COMMAND_RANGE(ID_VIEW_FILEVIEW, ID_VIEW_STATIONVIEW, &CMainFrame::OnViewChoose)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_FILEVIEW, ID_VIEW_STATIONVIEW, &CMainFrame::OnUpdateViewChoose)
	ON_COMMAND_RANGE(ID_VIEW_OUTPUTWND, ID_VIEW_PROPERTIESWND, &CMainFrame::OnViewChoose)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_OUTPUTWND, ID_VIEW_PROPERTIESWND, &CMainFrame::OnUpdateViewChoose)
	ON_UPDATE_COMMAND_UI_RANGE(ID_WINDOW_NEW, ID_WINDOW_NEW, &CMainFrame::OnUpdateViewChoose)
	//ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_NEW, ID_FILE_NEW, &CMainFrame::OnUpdateViewChoose)
	ON_MESSAGE(FAULT_MENU_MSG,OnFaultMenuMessage)
	ON_MESSAGE(FAULT_FILE_MSG,OnFaultFileMessage)
	ON_MESSAGE(SETTING_FILE_MSG,OnSettingFileMessage)
	ON_MESSAGE(WM_PROGESS_START, OnProgessStart) 
	ON_MESSAGE(WM_PROGESS_STEP, OnProgessStep) 
	ON_MESSAGE(WM_PROGESS_END, OnProgessEnd)
	ON_MESSAGE(TRIG_RECORD_ACTIVE,OnTrigRecordActiveMessage)
	ON_MESSAGE(TRIG_RECORD_NEGATIVE,OnTrigRecordNegativeMessage)
	ON_MESSAGE(CALL_TIME_ACTIVE,OnCallTimeActiveMessage)
	ON_MESSAGE(CALL_TIME_NEGATINE,OnCallTimeNegativeMessage)
	ON_MESSAGE(SET_TIME_ACTIVE,OnSetTimeActiveMessage)
	ON_MESSAGE(SET_TIME_NEGATIVE,OnSetTimeNegativeMessage)
	ON_MESSAGE(REFRESH_STATION_VIEW,OnRefreshStationViewMessage)
	ON_MESSAGE(REFRESH_PROPERTY_VIEW,OnRefreshPropertyViewMessage)
	ON_MESSAGE(SET_SETTING_ACTIVE,OnSetSettingActiveMessage)
	ON_MESSAGE(SET_SETTING_NEGATIVE,OnSetSettingNegativeMessage)
	//ON_MESSAGE(WM_RASDIALEVENT, OnRasDialMessage)
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_SILVER);
	
	//初始化视图指针
	ClearProgressCtrlPtr();
	ClearFaultFileFrmPtr();
	ClearSettingFrmPtr();
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	CMDITabInfo mdiTabParams;
	mdiTabParams.m_style = CMFCTabCtrl::STYLE_3D_ONENOTE; // 其他可用样式...
	mdiTabParams.m_bActiveTabCloseButton = TRUE;      // 设置为 FALSE 会将关闭按钮放置在选项卡区域的右侧
	mdiTabParams.m_bTabIcons = FALSE;    // 设置为 TRUE 将在 MDI 选项卡上启用文档图标
	mdiTabParams.m_bAutoColor = TRUE;    // 设置为 FALSE 将禁用 MDI 选项卡的自动着色
	mdiTabParams.m_bDocumentMenu = TRUE; // 在选项卡区域的右边缘启用文档菜单
	EnableMDITabbedGroups(TRUE, mdiTabParams);

	m_wndRibbonBar.Create(this);
	InitializeRibbon();

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);

	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 加载菜单项图像(不在任何标准工具栏上):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// 创建停靠窗口
	///*
	if (!CreateDockingWindows())
	{
		TRACE0("未能创建停靠窗口\n");
		return -1;
	}
	

	m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndStationView.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndFileView);
	CDockablePane* pTabbedBar = NULL;
	m_wndStationView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndOutput);
	m_wndProperties.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndProperties);
	//*/

	// 启用增强的窗口管理对话框
	EnableWindowsDialog(ID_WINDOW_MANAGER, IDS_WINDOWS_MANAGER, TRUE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	return TRUE;
}

void CMainFrame::InitializeRibbon()
{
	BOOL bNameValid;

	CString strTemp;
	bNameValid = strTemp.LoadString(IDS_RIBBON_FILE);
	ASSERT(bNameValid);

	// 加载面板图像:
	m_PanelImages.SetImageSize(CSize(16, 16));
	m_PanelImages.Load(IDB_BUTTONS);

	// 初始主按钮:
	m_MainButton.SetImage(IDB_MAIN);
	m_MainButton.SetText(_T("\nf"));
	m_MainButton.SetToolTipText(strTemp);

	m_wndRibbonBar.SetApplicationButton(&m_MainButton, CSize (45, 45));
	CMFCRibbonMainPanel* pMainPanel = m_wndRibbonBar.AddMainCategory(strTemp, IDB_FILESMALL, IDB_FILELARGE);

	bNameValid = strTemp.LoadString(IDS_RIBBON_NEW);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_NEW, strTemp, 0, 0));
	bNameValid = strTemp.LoadString(IDS_RIBBON_OPEN);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_OPEN, strTemp, 1, 1));
	bNameValid = strTemp.LoadString(IDS_RIBBON_SAVE);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE, strTemp, 2, 2));
	bNameValid = strTemp.LoadString(IDS_RIBBON_SAVEAS);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE_AS, strTemp, 3, 3));

	bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnPrint = new CMFCRibbonButton(ID_FILE_PRINT, strTemp, 6, 6);
	pBtnPrint->SetKeys(_T("p"), _T("w"));
	bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_LABEL);
	ASSERT(bNameValid);
	pBtnPrint->AddSubItem(new CMFCRibbonLabel(strTemp));
	bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_QUICK);
	ASSERT(bNameValid);
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_DIRECT, strTemp, 7, 7, TRUE));
	bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_PREVIEW);
	ASSERT(bNameValid);
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_PREVIEW, strTemp, 8, 8, TRUE));
	bNameValid = strTemp.LoadString(IDS_RIBBON_PRINT_SETUP);
	ASSERT(bNameValid);
	pBtnPrint->AddSubItem(new CMFCRibbonButton(ID_FILE_PRINT_SETUP, strTemp, 11, 11, TRUE));
	pMainPanel->Add(pBtnPrint);
	pMainPanel->Add(new CMFCRibbonSeparator(TRUE));

	bNameValid = strTemp.LoadString(IDS_RIBBON_CLOSE);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_CLOSE, strTemp, 9, 9));

	bNameValid = strTemp.LoadString(IDS_RIBBON_RECENT_DOCS);
	ASSERT(bNameValid);
	pMainPanel->AddRecentFilesList(strTemp);

	bNameValid = strTemp.LoadString(IDS_RIBBON_EXIT);
	ASSERT(bNameValid);
	pMainPanel->AddToBottom(new CMFCRibbonMainPanelButton(ID_APP_EXIT, strTemp, 15));

	// 为“剪贴板”面板添加“主”类别:
	bNameValid = strTemp.LoadString(IDS_RIBBON_HOME);
	ASSERT(bNameValid);
	CMFCRibbonCategory* pCategoryHome = m_wndRibbonBar.AddCategory(strTemp, IDB_WRITESMALL, IDB_WRITELARGE);

	// 创建“剪贴板”面板:
	bNameValid = strTemp.LoadString(IDS_RIBBON_CLIPBOARD);
	ASSERT(bNameValid);
	CMFCRibbonPanel* pPanelClipboard = pCategoryHome->AddPanel(strTemp, m_PanelImages.ExtractIcon(27));

	bNameValid = strTemp.LoadString(IDS_RIBBON_PASTE);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnPaste = new CMFCRibbonButton(ID_EDIT_PASTE, strTemp, 0, 0);
	pPanelClipboard->Add(pBtnPaste);

	bNameValid = strTemp.LoadString(IDS_RIBBON_CUT);
	ASSERT(bNameValid);
	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_CUT, strTemp, 1));
	bNameValid = strTemp.LoadString(IDS_RIBBON_COPY);
	ASSERT(bNameValid);
	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_COPY, strTemp, 2));
	bNameValid = strTemp.LoadString(IDS_RIBBON_SELECTALL);
	ASSERT(bNameValid);
	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_SELECT_ALL, strTemp, -1));

	// 创建和添加“视图”面板:
	bNameValid = strTemp.LoadString(IDS_RIBBON_VIEW);
	ASSERT(bNameValid);
	CMFCRibbonPanel* pPanelView = pCategoryHome->AddPanel(strTemp, m_PanelImages.ExtractIcon (7));

	bNameValid = strTemp.LoadString(IDS_RIBBON_STATUSBAR);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnStatusBar = new CMFCRibbonCheckBox(ID_VIEW_STATUS_BAR, strTemp);
	pPanelView->Add(pBtnStatusBar);

	bNameValid = strTemp.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnFileView = new CMFCRibbonCheckBox(ID_VIEW_FILEVIEW, strTemp);
	pPanelView->Add(pBtnFileView);

	bNameValid = strTemp.LoadString(IDS_STATION_VIEW);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnStationView = new CMFCRibbonCheckBox(ID_VIEW_STATIONVIEW, strTemp);
	pPanelView->Add(pBtnStationView);

	bNameValid = strTemp.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnOutputView = new CMFCRibbonCheckBox(ID_VIEW_OUTPUTWND, strTemp);
	pPanelView->Add(pBtnOutputView);

	bNameValid = strTemp.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnPropertyView = new CMFCRibbonCheckBox(ID_VIEW_PROPERTIESWND, strTemp);
	pPanelView->Add(pBtnPropertyView);

	// 创建和添加“窗口”面板:
	bNameValid = strTemp.LoadString(IDS_RIBBON_WINDOW);
	ASSERT(bNameValid);
	CMFCRibbonPanel* pPanelWindow = pCategoryHome->AddPanel(strTemp, m_PanelImages.ExtractIcon (7));

	bNameValid = strTemp.LoadString(IDS_RIBBON_WINDOWS);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnWindows = new CMFCRibbonButton(ID_WINDOW_MANAGER, strTemp, -1, 1);
	pBtnWindows->SetMenu(IDR_WINDOWS_MENU, TRUE);
	pPanelWindow->Add(pBtnWindows);

	// 将元素添加到选项卡右侧:
	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE);
	ASSERT(bNameValid);
	CMFCRibbonButton* pVisualStyleButton = new CMFCRibbonButton(-1, strTemp, -1, -1);

	pVisualStyleButton->SetMenu(IDR_THEME_MENU, FALSE /* 无默认命令*/, TRUE /* 右对齐*/);

	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE_TIP);
	ASSERT(bNameValid);
	pVisualStyleButton->SetToolTipText(strTemp);
	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE_DESC);
	ASSERT(bNameValid);
	pVisualStyleButton->SetDescription(strTemp);
	m_wndRibbonBar.AddToTabs(pVisualStyleButton);

	// 添加快速访问工具栏命令:
	CList<UINT, UINT> lstQATCmds;

	lstQATCmds.AddTail(ID_FILE_NEW);
	lstQATCmds.AddTail(ID_FILE_OPEN);
	lstQATCmds.AddTail(ID_FILE_SAVE);
	lstQATCmds.AddTail(ID_FILE_PRINT_DIRECT);

	m_wndRibbonBar.SetQuickAccessCommands(lstQATCmds);

	m_wndRibbonBar.AddToTabs(new CMFCRibbonButton(ID_APP_ABOUT, _T("\na"), m_PanelImages.ExtractIcon (0)));
}

BOOL CMainFrame::CreateDockingWindows()
{
	BOOL bNameValid;

	// 创建子站视图
	CString strClassView;
	bNameValid = strClassView.LoadString(IDS_STATION_VIEW);
	ASSERT(bNameValid);
	if (!m_wndStationView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_STATIONVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“子站视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建文件视图
	CString strFileView;
	bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
	ASSERT(bNameValid);
	if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“文件视图”窗口\n");
		return FALSE; // 未能创建
	}

	// 创建输出窗口
	///*
	CString strOutputWnd;
	bNameValid = strOutputWnd.LoadString(IDS_OUTPUT_WND);
	ASSERT(bNameValid);
	if (!m_wndOutput.Create(strOutputWnd, this, CRect(0, 0, 100, 100), TRUE, ID_VIEW_OUTPUTWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建输出窗口\n");
		return FALSE; // 未能创建
	}
	//*/

	// 创建属性窗口
	///*
	CString strPropertiesWnd;
	bNameValid = strPropertiesWnd.LoadString(IDS_PROPERTIES_WND);
	ASSERT(bNameValid);
	if (!m_wndProperties.Create(strPropertiesWnd, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_PROPERTIESWND, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("未能创建“属性”窗口\n");
		return FALSE; // 未能创建
	}
	//*/

	SetDockingWindowIcons(theApp.m_bHiColorIcons);
	return TRUE;
}

void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndFileView.SetIcon(hFileViewIcon, FALSE);

	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndStationView.SetIcon(hClassViewIcon, FALSE);

	HICON hOutputBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_OUTPUT_WND_HC : IDI_OUTPUT_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndOutput.SetIcon(hOutputBarIcon, FALSE);

	HICON hPropertiesBarIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_PROPERTIES_WND_HC : IDI_PROPERTIES_WND), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndProperties.SetIcon(hPropertiesBarIcon, FALSE);

	UpdateMDITabbedBarsIcons();
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

/*
int CMainFrame::getSelectedStation(void)
{
	if (m_wndStationView.GetSafeHwnd() == NULL)
	{
		return NO_STATION_SEl;
	}

	return m_wndStationView.getCurStation();
}
*/

void CMainFrame::RefreshPropertyView(int station,int dev,int file)
{
	if (m_wndProperties.GetSafeHwnd() == NULL)
	{
		return;
	}

	m_wndProperties.RefreshPropertyView(station,dev,file);
}

void CMainFrame::OnViewChoose(UINT id)
{
	bool bAutoHide = false;
	switch (id)
	{
	case ID_VIEW_STATUS_BAR:
		if (m_wndStatusBar.GetSafeHwnd() == NULL)
		{
			break;
		}
		m_wndStatusBar.setbShow(!m_wndStatusBar.getbShow());
		m_wndStatusBar.ShowPane(m_wndStatusBar.getbShow(),false,m_wndStatusBar.getbShow());
		RecalcLayout();
		break;

	case ID_VIEW_FILEVIEW:
		if (m_wndFileView.GetSafeHwnd() == NULL)
		{
			break;
		}
		m_wndFileView.setbShow(!m_wndFileView.getbShow());
		m_wndFileView.ShowPane(m_wndFileView.getbShow(),false,m_wndFileView.getbShow());
		//RecalcLayout();
		break;

	case ID_VIEW_STATIONVIEW:
		if (m_wndStationView.GetSafeHwnd() == NULL)
		{
			break;
		}
		m_wndStationView.setbShow(!m_wndStationView.getbShow());
		m_wndStationView.ShowPane(m_wndStationView.getbShow(),false,m_wndStationView.getbShow());
		//RecalcLayout();
		break;

	case ID_VIEW_OUTPUTWND:
		if (m_wndOutput.GetSafeHwnd() == NULL)
		{
			break;
		}
		m_wndOutput.setbShow(!m_wndOutput.getbShow());
		m_wndOutput.ShowPane(m_wndOutput.getbShow(),false,m_wndOutput.getbShow());
		//RecalcLayout();
		break;

	case ID_VIEW_PROPERTIESWND:
		if (m_wndProperties.GetSafeHwnd() == NULL)
		{
			break;
		}
		m_wndProperties.setbShow(!m_wndProperties.getbShow());
		m_wndProperties.ShowPane(m_wndProperties.getbShow(),false,m_wndProperties.getbShow());
		//RecalcLayout();
		break;

	default:
		break;
	}
}

void CMainFrame::OnUpdateViewChoose(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	switch (pCmdUI->m_nID)
	{
	case ID_VIEW_STATUS_BAR:
		pCmdUI->SetCheck(m_wndStatusBar.getbShow());
		break;

	case ID_VIEW_FILEVIEW:
		pCmdUI->SetCheck(m_wndFileView.getbShow());
		break;

	case ID_VIEW_STATIONVIEW:
		pCmdUI->SetCheck(m_wndStationView.getbShow());
		break;

	case ID_VIEW_OUTPUTWND:
		pCmdUI->SetCheck(m_wndOutput.getbShow());
		break;

	case ID_VIEW_PROPERTIESWND:
		pCmdUI->SetCheck(m_wndProperties.getbShow());
		break;

	case ID_WINDOW_NEW:
		pCmdUI->Enable(false);
		break;

	default:
		break;
	}
}

void CMainFrame::RefreshStationView(void)
{
	if (m_wndStationView.GetSafeHwnd() == NULL)
	{
		return;
	}

	m_wndStationView.RefreshStationView();
}

void CMainFrame::RefreshStationImage(void)
{
	if (m_wndStationView.GetSafeHwnd() == NULL)
	{
		return;
	}

	m_wndStationView.RefreshStationImage();
}

int CMainFrame::getSeletedFaultFile(void)
{
	if (m_wndFaultFile == NULL)
	{
		return -1;
	}
	
	return m_wndFaultFile->getSelectedFileIndex();
}

LRESULT CMainFrame::OnFaultMenuMessage(WPARAM wparam,LPARAM lparam)
{
	CString str;
	str.Format(_T("子站%d装置%d 获得故障文件目录"),wparam,lparam);
	AddOutputReturn(str);

	RefreshFileView();
	return RefreshFaultFileFrm(wparam,lparam);
}

LRESULT CMainFrame::OnFaultFileMessage(WPARAM wparam,LPARAM lparam)
{
	CString str;
	str.Format(_T("子站%d装置%d 获得故障文件"),wparam,lparam);
	AddOutputReturn(str);

	RefreshFileView();
	return RefreshFaultFileFrm(wparam,lparam);
}

LRESULT CMainFrame::OnSettingFileMessage(WPARAM wparam,LPARAM lparam)
{
	CString str;
	str.Format(_T("子站%d装置%d 获得定值"),wparam,lparam);
	AddOutputReturn(str);

	RefreshFileView();
	return RefreshSettingFrm(wparam,lparam);
}

void CMainFrame::ClearSettingFrmPtr(void)
{
	m_wndSetting = NULL;
}

void CMainFrame::ClearFaultFileFrmPtr(void)
{
	m_wndFaultFile = NULL;
}

int CMainFrame::RefreshFaultFileFrm(int stationIndex,int devIndex)
{
	if (m_wndFaultFile == NULL)
	{
		HINSTANCE hInst = AfxGetResourceHandle();
		HMENU m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_FAULTFILETYPE));
		HACCEL m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_FAULTFILETYPE));

		m_wndFaultFile = (CFaultFileFrame *)CreateNewChild(RUNTIME_CLASS(CFaultFileFrame), IDR_FAULTFILETYPE, m_hMDIMenu, m_hMDIAccel);

		m_wndFaultFile->AdjustLayout();
		m_wndFaultFile->InitFaultFileFrmList(stationIndex,devIndex);
		m_wndFaultFile->SetFocus();
	}
	else if (m_wndFaultFile->GetSafeHwnd())
	{
		//m_wndFaultFile->AdjustLayout();
		m_wndFaultFile->RefreshFaultFileList(stationIndex,devIndex);
		m_wndFaultFile->SetFocus();
	}

	return 0;
}

int CMainFrame::RefreshSettingFrm(int stationIndex, int devIndex)
{
	if (m_wndSetting == NULL)
	{
		HINSTANCE hInst = AfxGetResourceHandle();
		HMENU m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_SETTINGTYPE));
		HACCEL m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_SETTINGTYPE));

		m_wndSetting = (CSettingFrame *)CreateNewChild(RUNTIME_CLASS(CSettingFrame), IDR_SETTINGTYPE, m_hMDIMenu, m_hMDIAccel);

		m_wndSetting->AdjustLayout();
		m_wndSetting->InitSettingFrmList(stationIndex,devIndex);
		m_wndSetting->SetFocus();
	}
	else if (m_wndSetting->GetSafeHwnd())
	{
		//m_wndSetting->AdjustLayout();
		m_wndSetting->RefreshSettingList(stationIndex,devIndex);
		m_wndSetting->SetFocus();
	}

	

	return 0;
}

void CMainFrame::RecalcLayout()
{
	CMDIFrameWndEx::RecalcLayout();

	if (m_wndFaultFile != NULL)
	{
		m_wndFaultFile->AdjustLayout();
		m_wndFaultFile->RecalcLayout();
	}

	if (m_wndSetting != NULL)
	{
		m_wndSetting->AdjustLayout();
		m_wndSetting->RecalcLayout();
	}	
}

void CMainFrame::RefreshFileView(void)
{
	if (m_wndFileView.GetSafeHwnd() == NULL)
	{
		return;
	}

	m_wndFileView.RefreshFileView();
}

void CMainFrame::CloseFaultFileFrm(void)
{
	if (m_wndFaultFile != NULL)
	{
		m_wndFaultFile->SendMessage(WM_CLOSE);
	}
}

void CMainFrame::CloseSettingFrm(void)
{
	if (m_wndSetting != NULL)
	{
		m_wndSetting->SendMessage(WM_CLOSE);
	}
}

LRESULT CMainFrame::OnProgessStart(WPARAM wParam, LPARAM lParam)
{
	CRect rect;
	m_wndStatusBar.GetClientRect(&rect);
	if (m_pProgressCtrl != NULL)
	{
		m_pProgressCtrl->CloseWindow();
		delete m_pProgressCtrl;
		ClearProgressCtrlPtr();
	}
	m_pProgressCtrl = new CTextProgressCtrl();
	m_pProgressCtrl->Create(WS_CHILD|WS_VISIBLE|PBS_SMOOTH, CRect(0,0,rect.right,rect.bottom), &m_wndStatusBar,1);
	m_pProgressCtrl->SetRange(wParam,lParam);

	SetTimer(PROGRESS_CTRL_TIMEOUT_ID,PROGRESS_CTRL_TIMEOUT_VAL,NULL);
	return 0;
}

LRESULT CMainFrame::OnProgessStep(WPARAM wParam, LPARAM lParam)
{
	int lastPos = (int)wParam;
	int step = (int)lParam;
	if(step < 0)
	{
		return -1;
	}

	if (m_pProgressCtrl != NULL)
	{
		m_pProgressCtrl->OffsetPos(step);
	}
	
	return 0;
}

LRESULT CMainFrame::OnProgessEnd(WPARAM wParam, LPARAM lParam)
{
	KillTimer(PROGRESS_CTRL_TIMEOUT_ID);

	if (m_pProgressCtrl != NULL)
	{
		m_pProgressCtrl->CloseWindow();
		delete m_pProgressCtrl;
		ClearProgressCtrlPtr();
	}
	
	return 0;
}

LRESULT CMainFrame::OnTrigRecordActiveMessage(WPARAM wParam, LPARAM lParam)
{
	int iStartionIndex = wParam;
	int iDevIndex = lParam;

	CString str;
	str.Format(_T("子站%d装置%d 启动录波成功"),iStartionIndex,iDevIndex);
	AfxMessageBox(str);
	AddOutputReturn(str);

	theApp.g_sDataBases[iStartionIndex].CommCallFaultFileMenu(iStartionIndex,iDevIndex);

	return 0;
}

LRESULT CMainFrame::OnTrigRecordNegativeMessage(WPARAM wParam, LPARAM lParam)
{
	CString str;
	str.Format(_T("子站%d装置%d 启动录波失败"),wParam,lParam);
	AfxMessageBox(str);
	AddOutputReturn(str);

	return 0;
}

LRESULT CMainFrame::OnCallTimeActiveMessage(WPARAM wParam, LPARAM lParam)
{
	int iStationIndex = wParam;
	int iDevIndex = lParam;
	SYSTEMTIME tm = theApp.g_sDataBases[iStationIndex].vDevs[iDevIndex].getDevTime();
	CString time;
	time.Format(_T("获得子站%d装置%d的时间： %.4d.%.2d.%.2d %.2d:%.2d:%.2d"),iStationIndex,iDevIndex,tm.wYear,tm.wMonth,tm.wDay,tm.wHour,tm.wMinute,tm.wSecond);
	AfxMessageBox(time);
	AddOutputReturn(time);

	return 0;
}

LRESULT CMainFrame::OnCallTimeNegativeMessage(WPARAM wParam, LPARAM lParam)
{
	CString str;
	str.Format(_T("子站%d装置%d 收取时间失败"),wParam,lParam);
	AfxMessageBox(str);
	AddOutputReturn(str);

	return 0L;
}

LRESULT CMainFrame::OnSetTimeActiveMessage(WPARAM wParam, LPARAM lParam)
{
	CString str;
	str.Format(_T("子站%d装置%d 对时成功"),wParam,lParam);
	AfxMessageBox(str);
	AddOutputReturn(str);

	return 0L;
}

LRESULT CMainFrame::OnSetTimeNegativeMessage(WPARAM wParam, LPARAM lParam)
{
	CString str;
	str.Format(_T("子站%d装置%d 对时失败"),wParam,lParam);
	AfxMessageBox(str);
	AddOutputReturn(str);

	return 0L;
}

LRESULT CMainFrame::OnRefreshStationViewMessage(WPARAM wparam,LPARAM lparam)
{
	RefreshStationView();

	return 0L;
}

LRESULT CMainFrame::OnRefreshPropertyViewMessage(WPARAM wparam,LPARAM lparam)
{
	RefreshPropertyView((int)wparam,NO_DEV_SEL,NO_FAULTFILE_SEL);

	return 0L;
}

void CMainFrame::ClearProgressCtrlPtr(void)
{
	m_pProgressCtrl = NULL;
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case PROGRESS_CTRL_TIMEOUT_ID:
		KillTimer(PROGRESS_CTRL_TIMEOUT_ID);
		OnProgessEnd(NULL,NULL);
		break;

	default:
		break;
	}
	CMDIFrameWndEx::OnTimer(nIDEvent);
}

void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	for (int i=0;i<theApp.g_sDataBases.size();i++)
	{
		theApp.g_sDataBases[i].ClearMainFrmPtr();
	}
	CMDIFrameWndEx::OnClose();
}

HWND CMainFrame::getFaultFileListWnd(void)
{
	return m_wndFaultFile->getMyListWnd();
}

int CMainFrame::SaveSettingVal(int StationIndex, int DevIndex)
{
	if (m_wndSetting != NULL)
	{
		return m_wndSetting->SaveSettingVal(StationIndex,DevIndex);
	}

	CString str = _T("定值视图未打开,不能保存");
	AfxMessageBox(str);
	AddOutputWarn(str);

	return -1;
}

void CMainFrame::AddOutputFind(CString strVal)
{
	m_wndOutput.FillFindWindow(strVal);
}

void CMainFrame::AddOutputReturn(CString strVal)
{
	m_wndOutput.FillReturnWindow(strVal);
}

void CMainFrame::AddOutputWarn(CString strVal)
{
	m_wndOutput.FillWarnWindow(strVal);
}

LRESULT CMainFrame::OnSetSettingActiveMessage(WPARAM wparam,LPARAM lparam)
{
	int iStationIndex = (int)wparam;
	int iDevIndex = (int)lparam;

	CString str;
	str.Format(_T("子站%d装置%d 设置定值成功"),iStationIndex,iDevIndex);
	AfxMessageBox(str);
	AddOutputReturn(str);

	return 0L;
}

LRESULT CMainFrame::OnSetSettingNegativeMessage(WPARAM wparam, LPARAM lparam)
{
	int iStationIndex = (int)wparam;
	int iDevIndex = (int)lparam;

	CString str;
	str.Format(_T("子站%d装置%d 设置定值失败"),iStationIndex,iDevIndex);
	AfxMessageBox(str);
	AddOutputReturn(str);

	return 0L;
}

/*
LRESULT CMainFrame::OnRasDialMessage(WPARAM wparam,LPARAM lparam)
{
	return 0L;
}
*/
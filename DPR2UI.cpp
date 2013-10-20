#include "stdafx.h"
#include "afxwinappex.h"
#include "DPR2UI.h"
#include "MainFrm.h"

#include "FaultFileFrm.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDPR2UIApp

BEGIN_MESSAGE_MAP(CDPR2UIApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CDPR2UIApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CDPR2UIApp::OnFileNew)
END_MESSAGE_MAP()


// CDPR2UIApp 构造

CDPR2UIApp::CDPR2UIApp()
{

	m_bHiColorIcons = TRUE;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的一个 CDPR2UIApp 对象

CDPR2UIApp theApp;


// CDPR2UIApp 初始化

BOOL CDPR2UIApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("NARI DPR"));

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 若要创建主窗口，此代码将创建新的框架窗口
	// 对象，然后将其设置为应用程序的主窗口对象
	CMDIFrameWnd* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// 创建主 MDI 框架窗口
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	// 试图加载共享 MDI 菜单和快捷键表
	//TODO: 添加附加成员变量，并加载对
	//	应用程序可能需要的附加菜单类型的调用
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_DPR2UITYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_DPR2UITYPE));

	// 主窗口已初始化，因此显示它并对其进行更新
	pFrame->ShowWindow(SW_SHOWMAXIMIZED);
	
	pFrame->UpdateWindow();

	return TRUE;
}


// CDPR2UIApp 消息处理程序

int CDPR2UIApp::ExitInstance() 
{
	//TODO: 处理可能已添加的附加资源
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	return CWinAppEx::ExitInstance();
}

void CDPR2UIApp::OnFileNew() 
{
	//CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	// 创建新的 MDI 子窗口
	//pFrame->m_wndFaultFile = (CFaultFileFrame *)pFrame->CreateNewChild(RUNTIME_CLASS(CFaultFileFrame), IDR_DPR2UITYPE, m_hMDIMenu, m_hMDIAccel);
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CDPR2UIApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CDPR2UIApp 自定义加载/保存方法

void CDPR2UIApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CDPR2UIApp::LoadCustomState()
{
}

void CDPR2UIApp::SaveCustomState()
{
}

// CDPR2UIApp 消息处理程序




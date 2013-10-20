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


// CDPR2UIApp ����

CDPR2UIApp::CDPR2UIApp()
{

	m_bHiColorIcons = TRUE;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CDPR2UIApp ����

CDPR2UIApp theApp;


// CDPR2UIApp ��ʼ��

BOOL CDPR2UIApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("NARI DPR"));

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// ��Ҫ���������ڣ��˴��뽫�����µĿ�ܴ���
	// ����Ȼ��������ΪӦ�ó���������ڶ���
	CMDIFrameWnd* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// ������ MDI ��ܴ���
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	// ��ͼ���ع��� MDI �˵��Ϳ�ݼ���
	//TODO: ��Ӹ��ӳ�Ա�����������ض�
	//	Ӧ�ó��������Ҫ�ĸ��Ӳ˵����͵ĵ���
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_DPR2UITYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_DPR2UITYPE));

	// �������ѳ�ʼ���������ʾ����������и���
	pFrame->ShowWindow(SW_SHOWMAXIMIZED);
	
	pFrame->UpdateWindow();

	return TRUE;
}


// CDPR2UIApp ��Ϣ�������

int CDPR2UIApp::ExitInstance() 
{
	//TODO: �����������ӵĸ�����Դ
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	return CWinAppEx::ExitInstance();
}

void CDPR2UIApp::OnFileNew() 
{
	//CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	// �����µ� MDI �Ӵ���
	//pFrame->m_wndFaultFile = (CFaultFileFrame *)pFrame->CreateNewChild(RUNTIME_CLASS(CFaultFileFrame), IDR_DPR2UITYPE, m_hMDIMenu, m_hMDIAccel);
}


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// �������жԻ����Ӧ�ó�������
void CDPR2UIApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CDPR2UIApp �Զ������/���淽��

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

// CDPR2UIApp ��Ϣ�������




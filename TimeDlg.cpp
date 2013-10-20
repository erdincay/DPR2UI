// TimeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DPR2UI.h"
#include "TimeDlg.h"
#include "log.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTimeDlg dialog
extern CDprApp theApp;

CTimeDlg::CTimeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTimeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimeDlg)
	m_hour = _T("");
	m_minute = _T("");
	m_second = _T("");
	m_waitInfo = _T("");
	//}}AFX_DATA_INIT
}


void CTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimeDlg)
	DDX_Control(pDX, IDC_AUTO, m_auto);
	DDX_Control(pDX, IDC_COMBO, m_selDpr);
	DDX_Control(pDX, IDC_MONTHCALENDAR, m_calendar);
	DDX_Text(pDX, IDC_HOUR, m_hour);
	DDV_MaxChars(pDX, m_hour, 2);
	DDX_Text(pDX, IDC_MINUTE, m_minute);
	DDV_MaxChars(pDX, m_minute, 2);
	DDX_Text(pDX, IDC_SECOND, m_second);
	DDV_MaxChars(pDX, m_second, 2);
	DDX_Text(pDX, IDC_WAITINFO, m_waitInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTimeDlg, CDialog)
	//{{AFX_MSG_MAP(CTimeDlg)
	ON_WM_PAINT()
	ON_MESSAGE(WM_REQTIME_END,OnReqTime)
	ON_CBN_SELCHANGE(IDC_COMBO, OnSelchangeCombo)
	ON_BN_CLICKED(IDC_AUTO, OnAuto)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeDlg message handlers

void CTimeDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect rect;
	this->GetWindowRect(&rect);
	int nWidth=rect.right-rect.left;
	int nHeight=rect.bottom-rect.top;
	dc.FillSolidRect(0,0,nWidth,nHeight,RGB(0,128,255));

	dc.SetBkMode(TRANSPARENT);
	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
	lf.lfHeight = 24;                      // request a 12-pixel-height font
	lf.lfWidth = 10;
	lf.lfEscapement =300;
	strcpy(lf.lfFaceName, "仿宋_GB2312");        // request a face name "Arial"
	VERIFY(font.CreateFontIndirect(&lf));  // create the font
	// Do something with the font just created...


	
	for(int i=0;i<6;i++)
	{
		for(int j=0;j<6;j++)
		{
			rect.left=nWidth*i/6;
			rect.right=nWidth*(i+1)/6;
			rect.top=nHeight*j/6;
			rect.bottom=nHeight*(j+1)/6;
	
			CFont * OldFont = dc.SelectObject(&font);

			// draw text in DC
		//	COLORREF OldColor = dc.SetTextColor( ::GetSysColor( COLOR_3DHILIGHT));
			COLORREF OldColor = dc.SetTextColor(RGB(0,211,255) );

			dc.DrawText( "DPR", rect + CPoint(1,1), DT_SINGLELINE | DT_LEFT | DT_VCENTER);
		//	dc.SetTextColor( ::GetSysColor( COLOR_3DSHADOW));
			dc.SetTextColor( RGB(0,121,255) );
			dc.DrawText( "DPR", rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);

			// restore old text color
			dc.SetTextColor( OldColor);
			// restore old font
			dc.SelectObject(OldFont);

			}
	
	}
	font.DeleteObject();
	// Do not call CDialog::OnPaint() for painting messages
}

BOOL CTimeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	this->m_calendar.ShowWindow(SW_HIDE);

	GetDlgItem(IDC_HOUR)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_MINUTE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SECOND)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);

	GetDlgItem(IDOK)->EnableWindow(FALSE);

	char tempChar[50];

	for(int i=0;i<theApp.DEVICE_NUMBER;i++)
	{
		memset(tempChar,0,sizeof(tempChar));
		sprintf(tempChar,"装置(站号:%04d)",theApp.dis[i].deviceID);
		m_selDpr.AddString(tempChar);
	}
	m_selDpr.SetCurSel(0);

	m_selDpr.ShowWindow(SW_HIDE);

	bAuto = TRUE;
	m_auto.SetCheck(1);
	
	this->m_waitInfo="正在收取装置时钟请等候... ...";
	UpdateData(FALSE);

	PostThreadMessage(theApp.threadID,WM_REQUEST_TIME,0,theApp.dis[0].deviceID);                                           

	
	SetTimer(0,180000,NULL);
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CTimeDlg::OnReqTime(WPARAM wParam, LPARAM lParam)
{
	//show date and time in interface
	GetDlgItem(IDC_WAITINFO)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_HOUR)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_MINUTE)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_SECOND)->ShowWindow(SW_SHOW);

	GetDlgItem(IDC_STATIC1)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC2)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC3)->ShowWindow(SW_SHOW);

	GetDlgItem(IDC_MONTHCALENDAR)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_COMBO)->ShowWindow(SW_SHOW);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	
	SYSTEMTIME st;
	memcpy(&st,(BYTE*)wParam,sizeof(st));
	m_calendar.SetCurSel(&st);

	char temp[10];
	memset(temp,0,sizeof(temp));
	sprintf(temp,"%d",st.wHour);
	this->m_hour=CString(temp);

	memset(temp,0,sizeof(temp));
	sprintf(temp,"%d",st.wMinute);
	this->m_minute=CString(temp);

	memset(temp,0,sizeof(temp));
	sprintf(temp,"%d",st.wSecond);
	this->m_second=CString(temp);

	if(bAuto)
	{
		int devID = this->m_selDpr.GetCurSel();
		PostThreadMessage(theApp.threadID,WM_REQUEST_TIME,0,theApp.dis[devID].deviceID);     
	}
	UpdateData(FALSE);

	return 0;
}

void CTimeDlg::OnOK() 
{
	CLog log;
	log.WriteLog("操作记录：当地核对时钟",22);
	// TODO: Add extra validation here
	UpdateData(TRUE);
	SYSTEMTIME st;
	this->m_calendar.GetCurSel(&st);
	st.wHour=atoi(this->m_hour);
	st.wMinute=atoi(this->m_minute);
	st.wSecond=atoi(this->m_second);
	if(st.wHour>23||st.wMinute>59||st.wSecond>59)
	{
		MessageBox("时钟输入有误",NULL,MB_OK);
		CDialog::OnOK();
	}
	memcpy(&theApp.sendtime,&st,sizeof(st));

	int cnt=m_selDpr.GetCurSel();

	PostThreadMessage(theApp.threadID,WM_SEND_TIME,(WPARAM)&theApp.sendtime,theApp.dis[cnt].deviceID);                                           
	
	CDialog::OnOK();
}

void CTimeDlg::OnSelchangeCombo() 
{
	// TODO: Add your control notification handler code here
	this->m_calendar.ShowWindow(SW_HIDE);

	GetDlgItem(IDC_HOUR)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_MINUTE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_SECOND)->ShowWindow(SW_HIDE);

	GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);

	GetDlgItem(IDOK)->EnableWindow(FALSE);

//	char tempChar[50];


	int cnt=m_selDpr.GetCurSel();

	m_selDpr.ShowWindow(SW_HIDE);

	this->m_waitInfo="正在收取装置时钟请等候... ...";
	UpdateData(FALSE);

	PostThreadMessage(theApp.threadID,WM_REQUEST_TIME,0,theApp.dis[cnt].deviceID);                                           
}

void CTimeDlg::OnAuto() 
{
	// TODO: Add your control notification handler code here
	bAuto = !bAuto;	
	if(bAuto)
	{
		int devID = this->m_selDpr.GetCurSel();
		PostThreadMessage(theApp.threadID,WM_REQUEST_TIME,0,theApp.dis[devID].deviceID);     
	}
}

void CTimeDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnTimer(nIDEvent);
	CDialog::OnCancel();
}

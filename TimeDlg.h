#if !defined(AFX_TIMEDLG_H__D8C8FC96_185A_4D11_ABA4_03F25D47BA3F__INCLUDED_)
#define AFX_TIMEDLG_H__D8C8FC96_185A_4D11_ABA4_03F25D47BA3F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimeDlg dialog

class CTimeDlg : public CDialog
{
// Construction
public:
	CTimeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTimeDlg)
	enum { IDD = IDD_TIMEDLG };
	CButton	m_auto;
	CComboBox	m_selDpr;
	CMonthCalCtrl	m_calendar;
	CString	m_hour;
	CString	m_minute;
	CString	m_second;
	CString	m_waitInfo;
	//}}AFX_DATA

private:
	BOOL bAuto;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimeDlg)
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnReqTime(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	afx_msg void OnSelchangeCombo();
	afx_msg void OnAuto();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMEDLG_H__D8C8FC96_185A_4D11_ABA4_03F25D47BA3F__INCLUDED_)

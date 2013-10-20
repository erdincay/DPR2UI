#pragma once

/////////////////////////////////////////////////////////////////////////////
// COutputList 窗口

class COutputList : public CListBox
{
protected:
	int m_iIndexOfTab;
// 构造
public:
	COutputList();

// 实现
public:
	virtual ~COutputList();

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnViewOutput();

public:
	int getIndexOfTab();
	void setIndexOfTab(int val);

	DECLARE_MESSAGE_MAP()
};

class COutputWnd : public CDockablePane
{
// 构造
public:
	COutputWnd();

// 属性
protected:
	CFont m_Font;
	bool IAmBeingShowed;
	CMFCTabCtrl	m_wndTabs;

	COutputList m_wndOutputWarn;
	COutputList m_wndOutputReturn;
	COutputList m_wndOutputFind;

public:
	void FillWarnWindow(CString strVal);
	void FillReturnWindow(CString strVal);
	void FillFindWindow(CString strVal);

protected:
	void AdjustHorzScroll(CListBox& wndListBox);

// 实现
public:
	virtual ~COutputWnd();
	virtual void ShowPane(BOOL bShow,BOOL bDelay,BOOL bActivate);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	DECLARE_MESSAGE_MAP()
public:
	bool getbShow(void);
	void setbShow(bool val);
};


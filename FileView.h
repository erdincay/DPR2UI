
#pragma once

#include "ViewTree.h"
#include <vector>
using namespace std;

class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CFileView : public CDockablePane
{
// 构造
public:
	CFileView();

	void AdjustLayout();
	void OnChangeVisualStyle();

// 属性
protected:
	bool IAmBeingShowed;
	CViewTree m_wndFileView;
	CImageList m_FileViewImages;
	CFileViewToolBar m_wndToolBar;
	vector<CString> strRoot;

protected:
	void FillFileView();

// 实现
public:
	virtual ~CFileView();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//virtual void ShowPane(BOOL bShow,BOOL bDelay,BOOL bActivate);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnAddNewDir();
	afx_msg void OnItemOpen();
	afx_msg void OnItemOpenWith();
	afx_msg void OnDummyCompile();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnEditClearAll();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);	
	//afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	//afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnUpdatePopFile(CCmdUI * pCmdUI);
	afx_msg void OnRefresh();
	afx_msg void OnUpdateRefresh(CCmdUI * pCmdUI);
	DECLARE_MESSAGE_MAP()

public:
	void TraversingAllFile(CString path);
	int SelDir(CString& strPath,char * strRootFolder,LPCTSTR cDlgName);	
	bool getbShow(void);
	void setbShow(bool val);
	int BrowerDir(HTREEITEM parentItem,CString parentPath);
	void RefreshFileView(void);
	void OpenFaultFile(CString fileName);
	void OpenMenu(CString menuName);
	void OpenXmlFile(CString pathName);
};


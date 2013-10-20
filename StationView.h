
#pragma once

#include "ViewTree.h"

class CStationToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CStationView : public CDockablePane
{
public:
	CStationView();
	virtual ~CStationView();

	void AdjustLayout();
	void OnChangeVisualStyle();

protected:
	CStationToolBar m_wndToolBar;
	CViewTree m_wndStationView;
	CImageList m_StationViewImages;
	UINT m_nCurrSort;
	//UINT m_nCurrStation;
	bool IAmBeingShowed;

	void FillStationsView();

// ÖØÐ´
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//virtual void ShowPane(BOOL bShow,BOOL bDelay,BOOL bActivate);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClassAddMemberFunction();
	afx_msg void OnClassAddMemberVariable();
	afx_msg void OnClassDefinition();
	afx_msg void OnClassProperties();
	afx_msg void OnNewStation();
	afx_msg void OnDelStation();
	afx_msg void OnDelAllStation();
	afx_msg void OnProperty();
	afx_msg void OnOpenLocalMenu();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);
	afx_msg void OnRWIO(UINT id);
	afx_msg void OnUpdateRWIO(CCmdUI* pCmdUI);
	afx_msg void OnPopStation(UINT id);
	afx_msg void OnUpdatePopStation(CCmdUI* pCmdUI);
	afx_msg void OnPopFaultFile(UINT id);
	afx_msg void OnUpdatePopFaultFile(CCmdUI * pCmdUI);
	afx_msg void OnPopSetting(UINT id);
	afx_msg void OnUpdatePopSetting(CCmdUI * pCmdUI);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRefresh();
	afx_msg void OnUpdateRefresh(CCmdUI * pCmdUI);
	afx_msg void OnPopOperating(UINT id);
	afx_msg void OnUpdatePopOperating(CCmdUI* pCmdUI);


	DECLARE_MESSAGE_MAP()
public:
	//const stTreeItemInfo * getCurItemInfo(void);
	bool getbShow(void);
	void setbShow(bool val);
	void RefreshStationView(void);
	void OnConnectStation(void);
	void OnDisConnectStation(void);
	void RefreshStationImage(void);
	//int setCurInfoToDataBase(void);
	void OnGetFaultFileMenu(void);
	void OnGetFaultFile(void);
	void OnAnalysisFaultFile(void);
	void OnVeritySetting(void);
	void OnGetSetting(void);
	void OnPutSetting(void);
	void OnSaveSetting(void);
	void OnLoadSetting(void);
	void OnShowFaultFileMenu(void);
	void OnShowSetting(void);
	void OnSaveCFG(void);
	void OnLoadCFG(void);
	void OnOpenCFG(void);
	void OnTextCFG(void);
	
	void OnNewDev(void);
	void OnDelDev(void);
	void OnDelAllDev(void);

	void OnDialUp();
	void OnHangOff();

	void OnTrigRecord();
	void OnRecvTime();
	void OnSetTime();
	void LoadCFG(CString fileName);
	BOOL DeleteAllItems();
};


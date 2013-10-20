

#pragma once

class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CPropertiesWnd : public CDockablePane
{
// 构造
public:
	CPropertiesWnd();

	void AdjustLayout();

// 属性
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

protected:
	int m_iStationIndex;
	int m_iDevIndex;
	int m_iFaultFileIndex;

	bool IAmBeingShowed;
	bool bExpandAll;
	CFont m_fntPropList;
	CComboBox m_wndObjectCombo;
	CPropertiesToolBar m_wndToolBar;
	CMFCPropertyGridCtrl m_wndPropList;
	CMFCPropertyGridProperty * m_gridPropName;
	CMFCPropertyGridProperty * m_gridPropID;
	CMFCPropertyGridProperty * m_gridPropAddr;
	CMFCPropertyGridProperty * m_gridPropIP;
	CMFCPropertyGridProperty * m_gridPropPort;
	CMFCPropertyGridProperty * m_gridPropPhone;
	CMFCPropertyGridProperty * m_gridPropModem;
	CMFCPropertyGridProperty * m_gridPropUserName;
	CMFCPropertyGridProperty * m_gridPropPassWord;

	CMFCPropertyGridProperty * m_gridPropDev[MAX_DEV_SUM];

// 实现
public:
	virtual ~CPropertiesWnd();
	virtual void ShowPane(BOOL bShow,BOOL bDelay,BOOL bActivate);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnSaveProperties();
	afx_msg void OnUpdateSaveProperties(CCmdUI* pCmdUI);
	afx_msg void OnRefreshProperties();
	afx_msg void OnUpdateRefreshProperties(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnComboSelChange();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg LRESULT OnPropertyChanged (WPARAM,LPARAM);

	DECLARE_MESSAGE_MAP()

	void InitPropList();
	void SetPropListFont();
public:
//	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	int InitCommGroup(CMFCPropertyGridProperty * pGroupComm,UINT index);
	int InitDevGroup(CMFCPropertyGridProperty * pGroupDev, UINT index);
	int InitStationGroup(CMFCPropertyGridProperty * pGroupStation, UINT index);
	void RefreshPropertyView(int station,int dev,int file);
	bool getbShow(void);
	void setbShow(bool val);	
};


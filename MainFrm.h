// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "FileView.h"
#include "StationView.h"
#include "OutputWnd.h"
#include "PropertiesWnd.h"
#include "FaultFileFrm.h"
#include "SettingFrm.h"
#include "MyStatusBar.h"
#include "TextProgressCtrl.h"

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// 属性
public:

// 操作

public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMyStatusBar  m_wndStatusBar;
	CFileView         m_wndFileView;
	CStationView        m_wndStationView;
	COutputWnd        m_wndOutput;
	CPropertiesWnd    m_wndProperties;
	
public:
	CTextProgressCtrl	*m_pProgressCtrl;
	CFaultFileFrame * m_wndFaultFile;
	CSettingFrame * m_wndSetting;
// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewChoose(UINT id);
	afx_msg void OnUpdateViewChoose(CCmdUI* pCmdUI);
	afx_msg LRESULT OnFaultMenuMessage(WPARAM wparam,LPARAM lparam);
	afx_msg LRESULT OnFaultFileMessage(WPARAM wparam,LPARAM lparam);
	afx_msg LRESULT OnSettingFileMessage(WPARAM wparam,LPARAM lparam);
	afx_msg LRESULT OnProgessStart(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProgessStep(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnProgessEnd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTrigRecordActiveMessage(WPARAM wparam,LPARAM lparam);
	afx_msg LRESULT OnTrigRecordNegativeMessage(WPARAM wparam,LPARAM lparam);
	afx_msg LRESULT OnCallTimeActiveMessage(WPARAM wparam,LPARAM lparam);
	afx_msg LRESULT OnCallTimeNegativeMessage(WPARAM wparam,LPARAM lparam);
	afx_msg LRESULT OnSetTimeActiveMessage(WPARAM wparam,LPARAM lparam);
	afx_msg LRESULT OnSetTimeNegativeMessage(WPARAM wparam,LPARAM lparam);
	afx_msg LRESULT OnRefreshStationViewMessage(WPARAM wparam,LPARAM lparam);
	afx_msg LRESULT OnRefreshPropertyViewMessage(WPARAM wparam,LPARAM lparam);
	afx_msg LRESULT OnSetSettingActiveMessage(WPARAM wparam,LPARAM lparam);
	afx_msg LRESULT OnSetSettingNegativeMessage(WPARAM wparam,LPARAM lparam);
	//afx_msg LRESULT OnRasDialMessage(WPARAM wparam,LPARAM lparam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
public:
//重载
	virtual void RecalcLayout();

protected:
	void InitializeRibbon();
	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
public:
	//int getSelectedStation(void);
	void RefreshPropertyView(int station,int dev,int file);
//	afx_msg void OnClose();
	void RefreshStationView(void);
	void RefreshStationImage(void);
	int getSeletedFaultFile(void);
	void ClearFaultFileFrmPtr(void);
	void ClearSettingFrmPtr(void);
	int RefreshFaultFileFrm(int stationIndex,int devIndex);
	int RefreshSettingFrm(int stationIndex, int devIndex);
	void RefreshFileView(void);
	void CloseFaultFileFrm(void);
	void CloseSettingFrm(void);
	void ClearProgressCtrlPtr(void);
	HWND getFaultFileListWnd(void);
	int SaveSettingVal(int StationIndex, int DevIndex);
	void AddOutputFind(CString strVal);
	void AddOutputReturn(CString strVal);
	void AddOutputWarn(CString strVal);
};



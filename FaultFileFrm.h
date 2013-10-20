// FaultFileFrm.h : CFaultFileFrame 类的接口
//

#pragma once
#include "FaultFileView.h"

class CFaultFileFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CFaultFileFrame)
public:
	CFaultFileFrame();

// 属性
	protected:
	int m_iStationIndex;
	int m_iDevIndex;
// 操作
public:

// 重写
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 实现
public:
	// 框架工作区的视图。
	CFaultFileView m_wndView;
	CMFCListCtrl m_wndShowList;
	virtual ~CFaultFileFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 生成的消息映射函数
protected:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnFileClose();
	afx_msg void OnPopFaultFile(UINT id);
	afx_msg void OnUpdatePopFaultFile(CCmdUI * pCmdUI);
	afx_msg void OnRefresh();
	afx_msg void OnUpdateRefresh(CCmdUI * pCmdUI);
	DECLARE_MESSAGE_MAP()
public:
	void InitFaultFileFrmList(int stationIndex,int devIndex);
	//void InitFaultFileFrmList();
	void RefreshFaultFileList(int stationIndex,int devIndex);
	void AdjustLayout(void);
	int getSelectedFileIndex(void);
	void RecalcLayout(void);
	
	void OnOpenLocalMenu(void);
	void OnGetFaultFile(void);
	void OnAnalysisFaultFile(void);
	HWND getMyListWnd(void);
};

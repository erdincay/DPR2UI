// SettingFrm.h : CSettingFrame 类的接口
//

#pragma once
#include "SettingView.h"
#include "Mysupergrid.h"
#include "DPR2UI.h"

struct stCopyInfo
{
	bool bCopying;
	int ItemType;
	CStringList strVal;
};

class CSettingFrame : public CMDIChildWndEx
{
	DECLARE_DYNCREATE(CSettingFrame)
public:
	CSettingFrame();

// 属性
protected:
	int m_iStationIndex;
	int m_iDevIndex;
	stCopyInfo copyInfo;

	CSuperGridCtrl::CTreeItem* pRoot;
	//CSuperGridCtrl::CTreeItem* pAISetting;
	//CSuperGridCtrl::CTreeItem* pDISetting;
	//CSuperGridCtrl::CTreeItem* pFreSetting;
	//CSuperGridCtrl::CTreeItem* pSeqSetting;

	CStringList m_strListChannelType;
	CStringList m_strListDIChoose;
	//CStringList m_strListDIOperate;
// 操作
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// 实现
public:
	// 框架工作区的视图。
	CSettingView m_wndView;
	CMySuperGrid m_wndShowList;

	virtual ~CSettingFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 生成的消息映射函数
protected:
	afx_msg void OnFileClose();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnPopSetting(UINT id);
	afx_msg void OnUpdatePopSetting(CCmdUI * pCmdUI);
	afx_msg void OnOpenLocalMenu();
	afx_msg void OnUpdateLocalMenu(CCmdUI * pCmdUI);
	afx_msg void OnRefresh();
	afx_msg void OnUpdateRefresh(CCmdUI * pCmdUI);
	afx_msg LRESULT OnPatchProcessMsg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	void InitSettingFrmList(int stationIndex,int devIndex);
	void RefreshSettingList(int StationIndex,int DevIndex);
	int LoadSettingCFG(int StationIndex, int DevIndex);
	int SaveSettingCFG(int StationIndex, int DevIndex);
	void AdjustLayout(void);
	int getSelectedFileIndex(void);
	int InitAISettingList(CSuperGridCtrl::CTreeItem* pParent);
	int InitDISettingList(CSuperGridCtrl::CTreeItem* pParent);
	int InitFreSettingList(CSuperGridCtrl::CTreeItem* pParent);
	int InitSeqSettingList(CSuperGridCtrl::CTreeItem* pParent);
	void RecalcLayout();
	void OnCopySettingVal();
	void OnPasteSettingVal();
	void OnExpandAll();
	void OnCollapseAll();
	int SaveSettingVal(int StationIndex,int DevIndex);
	int SaveAISettingVal(CSuperGridCtrl::CTreeItem* pParent);
	int SaveDISettingVal(CSuperGridCtrl::CTreeItem* pParent);
	int SaveFreSettingVal(CSuperGridCtrl::CTreeItem* pParent);
	int SaveSeqSettingVal(CSuperGridCtrl::CTreeItem* pParent);
};

// 用于批处理定值的 CPatchDlg 对话框
class CPatchDlg : public CDialog
{
public:
	CPatchDlg();
	CPatchDlg(int type);

	// 对话框数据
	enum { IDD = IDD_DIALOG_PATCH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	friend class CSettingFrame;
	UINT startIndex;
	UINT endIndex;

	CMySuperGrid m_wndShowList;
	CStringList m_strListChannelType;
	CStringList m_strListDIChoose;
	CStringList strVal;
	int ListType;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedOk();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

public:
	int InitShowList(int type);
	int InitAIShowList();
	int InitDIShowList();
	int InitFREShowList();
	int InitSEQShowList();
	void AdjustLayout();
	afx_msg void OnBnClickedCancel();
};

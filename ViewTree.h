#pragma once

/////////////////////////////////////////////////////////////////////////////
// CViewTree 窗口
struct stTreeItemInfo
{
	int iStationIndex;
	int iDevIndex;
	int iItemType;
	stTreeItemInfo()
	{
		iStationIndex = -1;
		iDevIndex = -1;
		iItemType = -1;
	}

	stTreeItemInfo(int station,int dev,int type)
	{
		iStationIndex = station;
		iDevIndex = dev;
		iItemType = type;
	}
};

class CViewTree : public CTreeCtrl
{
// 构造
public:
	CViewTree();

// 重写
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// 实现
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	const DWORD_PTR getCurItemInfo(void);
};

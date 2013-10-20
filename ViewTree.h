#pragma once

/////////////////////////////////////////////////////////////////////////////
// CViewTree ����
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
// ����
public:
	CViewTree();

// ��д
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// ʵ��
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	const DWORD_PTR getCurItemInfo(void);
};

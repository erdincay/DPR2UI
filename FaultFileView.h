// FaultFileView.h : CFaultFileView ��Ľӿ�
//


#pragma once


// CFaultFileView ����

class CFaultFileView : public CWnd
{
// ����
public:
	CFaultFileView();

// ����
public:
	//CMFCPropertyGridCtrl m_wndShowList;
// ����

// ��д
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//virtual void OnInitialUpdate();
// ʵ��
public:
	virtual ~CFaultFileView();

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


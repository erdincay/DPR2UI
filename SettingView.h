// SettingView.h : CSettingView ��Ľӿ�
//


#pragma once


// CSettingView ����

class CSettingView : public CWnd
{
// ����
public:
	CSettingView();

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
	virtual ~CSettingView();

// ���ɵ���Ϣӳ�亯��
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};


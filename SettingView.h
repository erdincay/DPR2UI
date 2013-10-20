// SettingView.h : CSettingView 类的接口
//


#pragma once


// CSettingView 窗口

class CSettingView : public CWnd
{
// 构造
public:
	CSettingView();

// 属性
public:
	//CMFCPropertyGridCtrl m_wndShowList;
// 操作

// 重写
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//virtual void OnInitialUpdate();
// 实现
public:
	virtual ~CSettingView();

// 生成的消息映射函数
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};


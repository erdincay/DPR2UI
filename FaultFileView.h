// FaultFileView.h : CFaultFileView 类的接口
//


#pragma once


// CFaultFileView 窗口

class CFaultFileView : public CWnd
{
// 构造
public:
	CFaultFileView();

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
	virtual ~CFaultFileView();

// 生成的消息映射函数
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号
#include "StationDataBases.h"
//#include "RAS.h"
#include "RasClient.h"

// CDPR2UIApp:
// 有关此类的实现，请参阅 DPR2UI.cpp
//

class CDPR2UIApp : public CWinAppEx
{
public:
	CDPR2UIApp();


// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
protected:
	HMENU  m_hMDIMenu;
	HACCEL m_hMDIAccel;

public:
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	CDPRStations g_sDataBases;
	//CRAS ras;
	CRasClient rasClient;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	DECLARE_MESSAGE_MAP()
};

extern CDPR2UIApp theApp;

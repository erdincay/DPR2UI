#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "StationDataBases.h"
//#include "RAS.h"
#include "RasClient.h"

// CDPR2UIApp:
// �йش����ʵ�֣������ DPR2UI.cpp
//

class CDPR2UIApp : public CWinAppEx
{
public:
	CDPR2UIApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
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

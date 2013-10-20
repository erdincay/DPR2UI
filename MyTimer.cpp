#include "stdafx.h"
#include "MyTimer.h"
#include "DPR2UI.h"

extern CDPR2UIApp theApp;
//必须要在外部定义一下静态数据成员
CTimerMap CMyTimer::m_sTimeMap;
UINT CMyTimer::index = 1;

CMyTimer::CMyTimer()
{
	m_nTimerID = index++;
}

CMyTimer::CMyTimer(UINT myID)
{
	m_nTimerID = myID;
}

CMyTimer::~CMyTimer()
{
	//KillMyTimer();
	RemoveItemKey();
}

void CALLBACK CMyTimer::MyTimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime)
{
	CString sz;
	CMyTimer::m_sTimeMap[idEvent]->KillMyTimer();
	//m_sTimeMap.RemoveKey(m_nTimerID);

}

/*
void CMyTimer::SetMyTimer(UINT nElapse,CString sz)
{
	szContent = sz;
	m_nTimerID = SetTimer(theApp.GetMainWnd()->m_hWnd,m_nTimerID,nElapse,(TIMERPROC)MyTimerProc);
	m_sTimeMap[m_nTimerID] = this;
}
*/

UINT CMyTimer::SetMyTimer(UINT nElapse,TIMERPROC timeProc,void * obj)
{
	//szContent = sz;
	if (timeProc != NULL)
	{
		m_nTimerID = SetTimer(theApp.GetMainWnd()->m_hWnd,m_nTimerID,nElapse,(TIMERPROC)timeProc);
	}
	else
	{
		m_nTimerID = SetTimer(theApp.GetMainWnd()->m_hWnd,m_nTimerID,nElapse,(TIMERPROC)MyTimerProc);
	}
	
	m_sTimeMap[m_nTimerID] = this;
	TargetObj = obj;

	return m_nTimerID;
}

void CMyTimer::KillMyTimer()
{
	KillTimer(theApp.GetMainWnd()->GetSafeHwnd(),m_nTimerID);
	//m_sTimeMap.RemoveKey(m_nTimerID);
}

void CMyTimer::RemoveItemKey()
{
	m_sTimeMap.RemoveKey(m_nTimerID);
}

void CMyTimer::setMyID(UINT id)
{
	m_nTimerID = id;
}

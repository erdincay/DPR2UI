// RAS.h: interface for the CRAS class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RAS_H__3DFAD314_4B46_11D3_A73C_000000000000__INCLUDED_)
#define AFX_RAS_H__3DFAD314_4B46_11D3_A73C_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <ras.h>
#include <rasdlg.h>

#define RAS_DLL "rasapi32.dll"

typedef DWORD (WINAPI *CRASEnumEntries)(LPTSTR reserved, LPTSTR lpszPhonebook, LPRASENTRYNAME lprasentryname, LPDWORD lpcb, LPDWORD lpcEntries);
typedef DWORD (WINAPI *CRASDial)(LPRASDIALEXTENSIONS lpRasDialExtensions, LPTSTR lpszPhonebook, LPRASDIALPARAMS lpRasDialParams, DWORD dwNotifierType, LPVOID lpvNotifier, LPHRASCONN lphRasConn);
typedef DWORD (WINAPI *CRASHangUp)(HRASCONN hrasconn);
typedef VOID  (WINAPI *CRASCallback)(UINT unMsg, RASCONNSTATE rasconnstate, DWORD dwError);
typedef DWORD (WINAPI *CRASGetErrorString)(UINT uErrorValue, LPTSTR lpszErrorString, DWORD cBufSize);
typedef DWORD (WINAPI *CRASEnumConnections)(LPRASCONN lprasconn, LPDWORD lpcb, LPDWORD lpcConnections);
typedef DWORD (WINAPI *CRASGetConnectStatus)(HRASCONN hrasconn, LPRASCONNSTATUS lprasconnstatus);

class CRAS  
{
public:
	BOOL IsEntryConnected(CString sEntry);
	BOOL IsConnected();
	ULONG GetEntryCount();
	CString GetStringError(DWORD dwError);
	CString GetStringState(RASCONNSTATE rasconnstate);
	BOOL HangUp();
	BOOL Dial(CString sEntry, CString sUserName, CString sPassword, CRASCallback pfnCallback = NULL, CString sDomain = _T(""),CString sPhoneNo=_T(""));
	CString GetEntry(ULONG ulIndex);
	BOOL Finalize();
	BOOL IsInitialized();
	void Reset();
	BOOL Initialize();
	CRAS();
	virtual ~CRAS();

public:
	HRASCONN m_hConnection;
	ULONG m_ulEntryCount;
	RASENTRYNAME m_Entries[20];
	BOOL LoadPhonebook();
	HINSTANCE m_hRASDll;
};

#endif // !defined(AFX_RAS_H__3DFAD314_4B46_11D3_A73C_000000000000__INCLUDED_)

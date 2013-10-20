// RAS.cpp: implementation of the CRAS class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RAS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRAS::CRAS()
{
	Reset();
	Initialize();
}

CRAS::~CRAS()
{
	Finalize();
}

BOOL CRAS::Initialize()
{
	m_hRASDll = LoadLibrary(_T("RASAPI32.DLL"));
	LoadPhonebook();
	return IsInitialized();
}

void CRAS::Reset()
{
	m_hRASDll      = NULL;
	m_ulEntryCount = 0;
	m_hConnection  = NULL;
}

BOOL CRAS::IsInitialized()
{
	return (m_hRASDll != NULL);
}

BOOL CRAS::Finalize()
{
	if (IsInitialized())
		return FreeLibrary(m_hRASDll);
	else
		return TRUE;
}

BOOL CRAS::LoadPhonebook()
{
	if (!IsInitialized())
		return FALSE;

	unsigned long ulEntryCount = sizeof(RASENTRYNAME) * 20;
	ZeroMemory(m_Entries, sizeof(RASENTRYNAME) * 20);
	m_Entries[0].dwSize = sizeof(RASENTRYNAME);
	//CRASEnumEntries pRASEnumEntries = (CRASEnumEntries)GetProcAddress(m_hRASDll, "RasEnumEntriesA");
	CRASEnumEntries pRASEnumEntries = (CRASEnumEntries)GetProcAddress(m_hRASDll, "RasEnumEntries");
	if (pRASEnumEntries)
	{
		pRASEnumEntries(NULL, NULL, m_Entries, &ulEntryCount, &m_ulEntryCount);
		return TRUE;
	}
	else
		return FALSE;
}

CString CRAS::GetEntry(ULONG ulIndex)
{
	if ((ulIndex >=0) && (ulIndex < m_ulEntryCount))
		return m_Entries[ulIndex].szEntryName;
	else
		return _T("");
}

BOOL CRAS::Dial(CString sEntry, CString sUserName, CString sPassword, CRASCallback pfnCallback, CString sDomain,CString sPhoneNo)
{
	if (!IsInitialized())
		return FALSE;

	RASDIALEXTENSIONS extensions;
	ZeroMemory(&extensions, sizeof(RASDIALEXTENSIONS));
	extensions.dwSize = sizeof(RASDIALEXTENSIONS);
	extensions.dwfOptions = RDEOPT_UsePrefixSuffix;

	RASDIALPARAMS params;
	ZeroMemory(&params, sizeof(params));
	params.dwSize = sizeof(params);
	lstrcpy(params.szEntryName, sEntry); 

	ZeroMemory(params.szPhoneNumber, RAS_MaxPhoneNumber + 1);
	ZeroMemory(params.szCallbackNumber, RAS_MaxCallbackNumber + 1);
	ZeroMemory(params.szUserName, UNLEN + 1);
	ZeroMemory(params.szPassword, PWLEN + 1);
	ZeroMemory(params.szDomain, DNLEN + 1);

#if (WINVER >= 0x401)   
	params.dwSubEntry = 0;
	params.dwCallbackId = 0;
#endif

	lstrcpy(params.szPhoneNumber,sPhoneNo);
	lstrcpy(params.szUserName, sUserName);
	lstrcpy(params.szPassword, sPassword);
	lstrcpy(params.szDomain,   sDomain);

	//CRASDial pRASDial = (CRASDial)GetProcAddress(m_hRASDll, "RasDialA");
	CRASDial pRASDial = (CRASDial)GetProcAddress(m_hRASDll, "RasDial");
	if (pRASDial)
	{
		return !pRASDial(&extensions, NULL, &params, 0, pfnCallback, &m_hConnection);
	}
	else
	{
		return FALSE;
	}
}

BOOL CRAS::HangUp()
{
	if (!IsInitialized())
		return FALSE;

	//CRASHangUp pRASHangUp = (CRASHangUp)GetProcAddress(m_hRASDll, "RasHangUpA");
	CRASHangUp pRASHangUp = (CRASHangUp)GetProcAddress(m_hRASDll, "RasHangUp");
	BOOL result = !pRASHangUp(m_hConnection);
//	if (result)
		m_hConnection = NULL;
	return result;
}

CString CRAS::GetStringState(RASCONNSTATE rasconnstate)
{
	if (
		(rasconnstate == RASCS_OpenPort) ||
		(rasconnstate == RASCS_PortOpened) ||
		(rasconnstate == RASCS_ConnectDevice) ||
		(rasconnstate == RASCS_SubEntryConnected)
	   )
	return _T("Dialing remote server");

	if (
		(rasconnstate == RASCS_DeviceConnected) ||
		(rasconnstate == RASCS_AllDevicesConnected)
	   )
	return _T("Remote server connected");

	if (
		(rasconnstate == RASCS_Authenticate) ||
		(rasconnstate == RASCS_AuthNotify) ||
		(rasconnstate == RASCS_AuthRetry) ||
		(rasconnstate == RASCS_AuthCallback) ||
		(rasconnstate == RASCS_AuthProject) ||
		(rasconnstate == RASCS_AuthLinkSpeed) ||
		(rasconnstate == RASCS_AuthAck) ||
		(rasconnstate == RASCS_ReAuthenticate) ||
		(rasconnstate == RASCS_Authenticated) ||
		(rasconnstate == RASCS_StartAuthentication) ||
		(rasconnstate == RASCS_RetryAuthentication) 
	   )
	return _T("Authentificating at remote server");

	if (rasconnstate == RASCS_AuthChangePassword)  
	return _T("You must change password");

	if (rasconnstate == RASCS_PasswordExpired)
	return _T("Password expired");

	if (
		(rasconnstate == RASCS_PrepareForCallback) ||
		(rasconnstate == RASCS_WaitForModemReset) ||
		(rasconnstate == RASCS_WaitForCallback) ||
		(rasconnstate == RASCS_CallbackComplete)
	   )
	return _T("Processing modem callback");

	if (
		(rasconnstate == RASCS_LogonNetwork) ||
		(rasconnstate == RASCS_Connected)
	   )
	return _T("Logging on network");

	if (
		(rasconnstate == RASCS_SubEntryDisconnected) ||
		(rasconnstate == RASCS_Disconnected)
	   )
	return _T("Disconnected from remote server");

	return _T("Unrecognized RAS state");
}

CString CRAS::GetStringError(DWORD dwError)
{
	//CRASGetErrorString pRASGetErrorString = (CRASGetErrorString)GetProcAddress(m_hRASDll, "RasGetErrorStringA");
	CRASGetErrorString pRASGetErrorString = (CRASGetErrorString)GetProcAddress(m_hRASDll, "RasGetErrorString");

	if (pRASGetErrorString)
	{
		WCHAR *pszErrorString = new WCHAR[1024];
		CString sResult;
		ZeroMemory(pszErrorString, 1024);
		if (!pRASGetErrorString(dwError, pszErrorString, 1024))
			sResult = pszErrorString;
		else
			sResult = _T("Unable to retrieve RAS error message text");
		delete[] pszErrorString;
		return sResult;
	}
	return _T("Unable to retrieve RAS error message text");
}

ULONG CRAS::GetEntryCount()
{
	return m_ulEntryCount;
}

BOOL CRAS::IsConnected()
{
	return (m_hConnection != NULL);
}

BOOL CRAS::IsEntryConnected(CString sEntry)
{
	if (!IsInitialized())
		return FALSE;

	//CRASEnumConnections  pRASEnumConnections  = (CRASEnumConnections)GetProcAddress(m_hRASDll, "RasEnumConnectionsA");
	CRASEnumConnections  pRASEnumConnections  = (CRASEnumConnections)GetProcAddress(m_hRASDll, "RasEnumConnections");
	//CRASGetConnectStatus pRASGetConnectStatus = (CRASGetConnectStatus)GetProcAddress(m_hRASDll, "RasGetConnectStatusA");
	CRASGetConnectStatus pRASGetConnectStatus = (CRASGetConnectStatus)GetProcAddress(m_hRASDll, "RasGetConnectStatus");
	if ((!pRASEnumConnections) || (!pRASGetConnectStatus))
		return FALSE;

	DWORD dwSize = sizeof(RASCONN) * 20, dwCount = 20;
	HRASCONN hConnection = NULL;
	RASCONN  connections[20];
	connections[0].dwSize = sizeof(RASCONN);
	if (!pRASEnumConnections(connections, &dwSize, &dwCount))
	{
		for (DWORD i=0; i<dwCount; i++)
			if (sEntry == connections[i].szEntryName)
			{
				hConnection = connections[i].hrasconn;
				break;
			}
	}
	else
		return FALSE;

	if (!hConnection)
		return FALSE;

	RASCONNSTATUS status;
	status.dwSize = sizeof(RASCONNSTATUS);
	if (!pRASGetConnectStatus(hConnection, &status))
		return (status.rasconnstate == RASCS_Connected);
	else
		return FALSE;
}

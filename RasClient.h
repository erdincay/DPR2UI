// RasClient.h
#pragma message("library is linking with \"rasapi32.lib\"")
#pragma comment(lib, "rasapi32.lib")

#include <ras.h>
#include <raserror.h>

#define ERROR_ENTRY_NO_FOUND	(RASBASEEND + 2)
#define ERROR_NO_RASSTATUS		(RASBASEEND + 3)

#define IPADDRESS				1	
#define IPADDRESS_DNS			2
#define IPADDRESS_DNS_ALT		3
#define IPADDRESS_WINS			4
#define IPADDRESS_WINS_ALT		5


class CRasClient
{
protected:
	ULONG m_ulEntryCount;
	RASENTRYNAME m_Entries[MAX_MODEM_CONN];
	HRASCONN m_ConnectionHandle;
	RASCONNSTATUS m_rasStatus;

public:
	CRasClient();
	~CRasClient();

	long GetDeviceCount();
	BOOL GetDeviceNameType(int nIndex, CString& strName, CString& strType);
	BOOL GetModemName(CString* strModemNameArray);
	long GetModemCount();
	DWORD ChangeEntryName(CString strOldName, CString strNewName);
	DWORD CreateNewEntry(CString strEntry, CString strDeviceType, 
		CString strDeviceName, 
		DWORD dwfNetProtocols, DWORD dwFrameProtocal, 
		CString strLocalPhone, CString strPhoneBook = _T(""));
	DWORD SetEntryOption(CString strEntry, DWORD dwfOptions, 
		BOOL bSet = TRUE, CString strPhoneBook = _T(""));
	DWORD SetEntryServerProtocal(CString strEntry, 
		DWORD dwfNetProtocols, CString strPhoneBook = _T(""));
	DWORD SetEntryIPAddress(CString strEntry, int nIPType,
		BYTE a, BYTE b, BYTE c,	BYTE d, CString strPhoneBook = _T(""));
	DWORD GetEntryProperties(CString strEntry, RASENTRY* lpRasEntry
		, LPTSTR lpszPhoneBook = NULL);
	DWORD RasDialSyn(CString strEntry, CString strPhoneNumber = _T(""),
		CString strUserName = _T(""), CString strPassword = _T(""), CString strDomain = _T(""), 
		CString strPhoneBook = _T(""));
	DWORD RasDialAsyn(CString strEntry, LPVOID lpvNotifer, CString strPhoneNumber = _T(""),
		CString strUserName = _T(""), CString strPassword = _T(""), CString strDomain = _T(""), 
		CString strPhoneBook = _T(""));
	DWORD RasDialAsynCallback1(CString strEntry, LPVOID lpvNotifer, CString strPhoneNumber = _T(""),
		CString strUserName = _T(""), CString strPassword = _T(""), CString strDomain = _T(""), 
		CString strPhoneBook = _T(""));
	DWORD RasDialAsynCallback2(CString strEntry, LPVOID lpvNotifer, CString strPhoneNumber = _T(""),
		CString strUserName = _T(""), CString strPassword = _T(""), CString strDomain = _T(""), 
		CString strPhoneBook = _T(""));
	BOOL GetRasStatusString(CString &strStatus, BOOL& bConnected);
	DWORD GetRasErrorString(CString &strError);
	DWORD GetRasErrorStringByErrorCode(DWORD dwError,
				DWORD dwExtendError, CString &strError);
	BOOL HangUpConnection(CString strEntry);
	BOOL GetRasConnection(CString strEntry, HRASCONN &hrasConn);
	BOOL LoadPhonebook();
	int getEntryCount(void);
	HRASCONN getConnectionHandle(void);
	CString getEntryName(UINT ulIndex);
	void setConnectionHandle(HRASCONN hConn);
};

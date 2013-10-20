#pragma once

#include "FaultFiles.h"
#include "SettingVal.h"
//#include <vector>
//using namespace std;

class CDPRDevs;
//class CFaultFiles;
//class CSettingVal;

class CDPRDev
{
protected:
	WORD wAddr;
	UINT DEV_ID;
	//UINT curFaultFile;
	WCHAR DevCatalog[MAX_NAME_LENGTH];
	SYSTEMTIME devTM;
	CDPRDevs * parentDEVs;

public:
	CFaultFiles faultfiles;
	CSettingVal setting;
	
protected:
	void setParentDEVs(CDPRDevs * ptr);
public:
	CDPRDev(void);
	CDPRDev(CDPRDevs * ptr);
	CDPRDev(const CDPRDev & rhs);
	~CDPRDev(void);
	UINT getDevID(void);
	void setDevID(UINT id);
	WCHAR * getDevCatalog(void);
	int setDevCatalog(WCHAR * log);
	UINT getFaultFileIndex(void);
	void setFaultFileIndex(UINT index);
	void InitDevDataBase(void);
	int getDevAddr(void);
	int setDevAddr(WORD val);
	const CDPRDevs * getParentDEVs();
	void CopyDevDataBase(CDPRDev & rhs);
	SYSTEMTIME getDevTime(void);
	void setDevTime(SYSTEMTIME tm);
};

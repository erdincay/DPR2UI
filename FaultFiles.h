#pragma once

#include "FaultFile.h"
//#include "DEV.h"
#include <vector>
using namespace std;

class CDPRDev;
//class CFaultFile;

class CFaultFiles :
	public vector<CFaultFile>
{
protected:
	CDPRDev * parentDev;
	WCHAR FFsCatalog[MAX_NAME_LENGTH];
public:
	CFaultFiles(void);
	CFaultFiles(CDPRDev * ptr);
	~CFaultFiles(void);
	void setParentDev(CDPRDev * ptr);
	const CDPRDev * getParentDev();
	WCHAR * getFaultFilesCatalog(void);
	int setFaultFilesCatalog(WCHAR * log);
	void SaveFaultMenu(int stationIndex,int devIndex);
	void LoadFaultMenu(int stationIndex,int devIndex);
};

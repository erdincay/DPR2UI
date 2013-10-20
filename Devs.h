#pragma once

#include <vector>
#include "dev.h"
using namespace std;

class CDPRStation;

class CDPRDevs :
	public vector<CDPRDev>
{
protected:
	CDPRStation * parentStation;
	WCHAR DevsCatalog[MAX_NAME_LENGTH];
public:
	CDPRDevs(void);
	CDPRDevs(CDPRStation * ptr);
	~CDPRDevs(void);
	void push_back(const CDPRDev & _Val);
	WCHAR * getDevsCatalog(void);
	int setDevsCatalog(WCHAR * log);
	const CDPRStation * getParentStationPtr();
	void setParentStationPtr(CDPRStation * ptr);
	int getDevIndexByAddr(WORD val);
};

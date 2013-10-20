#pragma once

#include <vector>
#include "StationDataBase.h"
using namespace std;

struct stTest
{
	byte byte1;
	byte byte2;
	byte byte3;
	long long1;
};

class CDPRStations :
	public vector<CDPRStation>
{
protected:
	//UINT curStation;
	WCHAR wOriginalCatalog[MAX_PATH];
	WCHAR wRootCatalog[MAX_PATH];
public:
	//CWnd * MainFrame;
	//HWND MainFrame_hWnd;

	byte fileBuf[MAX_FILEBUF_LENGTH];
	DWORD fileLength;
	CDPRStations(void);
	~CDPRStations(void);
	void push_back(const CDPRStation & _Val);
	//int getCurStationIndex(void);
	//int setCurStationIndex(int index);
	int SaveBufToFile(int iStationIndex,int iDevIndex,int iFileIndex);
	bool ConsummateFileName(WCHAR * srcName,CString & dstName,byte type);
	WCHAR * getRootCatalog(void);
	int setRootCatalog(WCHAR * log);
	WCHAR * getOriginalCatalog();
	DWORD getFileLength(void);
	void setFileLength(DWORD val);
	void ClearFileBuf(void);
};

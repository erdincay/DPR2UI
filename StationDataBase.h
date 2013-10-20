#pragma once

#include "DEVs.h"
//#include <vector>
//using namespace std;

class CProtocol;

struct stNET
{
	WCHAR IP_Addr[16];
	UINT Port;
};

struct stPHONE
{
	WCHAR UsrName[MAX_NAME_LENGTH];
	WCHAR PassWord[MAX_NAME_LENGTH];
	WCHAR phone_no[24];
	UINT ModemIndex;
};

struct stComm 
{
	WORD Protocol;
	byte commType;
	
	stNET NetInterface;
	stPHONE PhonelInterface;
};

struct stFaultFileInfo
{
	UINT stationIndex;
	UINT devIndex;
	UINT fileIndex;

	stFaultFileInfo()
	{
		stationIndex = 0;
		devIndex = 0;
		fileIndex = 0;
	}

	stFaultFileInfo(UINT station)
	{
		stationIndex = station;
		devIndex = 0;
		fileIndex = 0;
	}

	stFaultFileInfo(UINT station,UINT dev)
	{
		stationIndex = station;
		devIndex = dev;
		fileIndex = 0;
	}

	stFaultFileInfo(UINT station,UINT dev,UINT file)
	{
		stationIndex = station;
		devIndex = dev;
		fileIndex = file;
	}
};

class CDPRStation
{
protected:
	bool bCommActive;
	UINT StationID;
	//UINT curDev;
	UINT iMyIndexOfStations;
	WCHAR StationName[MAX_NAME_LENGTH];
	WCHAR StationCatalog[MAX_NAME_LENGTH];
	stComm comm;

	WORD wAddr;
	byte FCB;
public:
	CWnd * MainFrame;
	HWND MainFrame_hWnd;
	CProtocol * myProtocalObj;
	CDPRDevs vDevs;

protected:
	int InitStationDataBase(void);
	int setStationCatalog(WCHAR * log);

public:
	CDPRStation(void);
	CDPRStation(UINT index);
	CDPRStation(const CDPRStation& rhs);
	~CDPRStation(void);
	WCHAR * getStationName(void);
	int setStationName(WCHAR * name);
	WCHAR * getPhoneUsrName(void);
	int setPhoneUsrName(WCHAR * name);
	WCHAR * getPhonePassWord(void);
	int setPhonePassWord(WCHAR * name);
	byte getCommType(void);
	void setCommType(byte commVal);
	WCHAR * getStationIPAddr(void);
	int setStationIPAddr(WCHAR * addr);
	UINT getStationIPPort(void);
	int setStationIPPort(UINT port);
	WCHAR * getStationPhoneNO(void);
	int setStationPhoneNO(WCHAR * no);
	UINT getStationID(void);
	int setStationID(UINT id);
	int getDevNums(void);
	const WCHAR * getStationCatalog(void);
	UINT getModemIndex(void);
	int setModemIndex(UINT index);
	bool getCommActive(void);
	void setCommActive(bool val);
	int OpenConnect(void);
	int CloseConnect(void);
	WORD getProtocol(void);
	void setProtocol(WORD val);
	//int getCurDevIndex(void);
	//int setCurDevIndex(int index);
	int getFCB(void);
	int setFCB(byte val);
	int ChangeFCB(void);

	int getStationAddr();
	int setStationAddr(WORD val);

	UINT getMyIndexOfStations(void);
	void CopyStationDataBase(CDPRStation& rhs);
	void ClearMainFrmPtr(void);

	int CommGetSettingCFG(int station,int dev);
	int CommPutSettingCFG(int station,int dev);
	int CommCallFaultFileMenu(int station,int dev);
	int CommSendTrigRecord(int station,int dev);
	int CommCallDevTime(int station,int dev);
	int CommSetDevTime(int station,int dev);
	int CommCallFaultFile(int station,int dev,int file);

	int DialUp();
	int HangOff();

public:
	static DWORD CALLBACK RasDialCallBack2( __in  DWORD dwCallbackId, __in  DWORD dwSubEntry,__in  HRASCONN hrasconn,__in  UINT unMsg,	__in  RASCONNSTATE rascs,__in  DWORD dwError,__in  DWORD dwExtendedError);

};

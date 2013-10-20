#include "StdAfx.h"
#include "StationDataBase.h"
#include "DPR2UI.h"
#include "Protocol.h"
#include "H103.h"
#include "MainFrm.h"

CDPRStation::CDPRStation(void)
{
	FCB = 0;

	//本实例在vector组合中的序号
	iMyIndexOfStations = 0;

	//初始化界面相关参数
	MainFrame      = AfxGetMainWnd();
	MainFrame_hWnd = AfxGetMainWnd()->GetSafeHwnd();
	
	//初始化通讯相关参数
	myProtocalObj = NULL;
	bCommActive = false;

	//初始化子站存储结构
	InitStationDataBase();
                                                                                                          
	vDevs.setParentStationPtr(this);
}

CDPRStation::CDPRStation(UINT index)
{
	FCB = 0;

	//本实例在vector组合中的序号
	iMyIndexOfStations = index;

	//初始化界面相关参数
	MainFrame      = AfxGetMainWnd();
	MainFrame_hWnd = AfxGetMainWnd()->GetSafeHwnd();

	//初始化通讯相关参数
	myProtocalObj = NULL;
	bCommActive = false;

	//初始化子站存储结构
	InitStationDataBase();

	vDevs.setParentStationPtr(this);
}

CDPRStation::CDPRStation(const CDPRStation& rhs)
{
	FCB = 0;

	//本实例在vector组合中的序号
	iMyIndexOfStations = rhs.iMyIndexOfStations;

	//初始化界面相关参数
	MainFrame      = rhs.MainFrame;
	MainFrame_hWnd = rhs.MainFrame_hWnd;

	//初始化通讯相关参数
	myProtocalObj = rhs.myProtocalObj;
	bCommActive = rhs.bCommActive;

	//初始化子站存储结构
	CopyStationDataBase((CDPRStation &)rhs);

	vDevs = rhs.vDevs;
	vDevs.setParentStationPtr(this);
}

CDPRStation::~CDPRStation(void)
{
	if (myProtocalObj != NULL)
	{
		SetEvent(myProtocalObj->gethProExitEvent());
		myProtocalObj->ClearRtuPtr();
	}
	
}

WCHAR * CDPRStation::getStationName(void)
{
	return StationName;
}

int CDPRStation::setStationName(WCHAR * name)
{
	CString oldName = getStationName();
	if (oldName.Compare(name) != 0)
	{
		lstrcpy((LPWSTR)StationName,name);

		CString strTmp = _T("\\");
		strTmp += getStationName();
		setStationCatalog(strTmp.GetBuffer());
		strTmp.ReleaseBuffer();
	}

	return 0;
	
}

WCHAR * CDPRStation::getPhoneUsrName(void)
{
	return comm.PhonelInterface.UsrName;
}

int CDPRStation::setPhoneUsrName(WCHAR * name)
{
	lstrcpy(comm.PhonelInterface.UsrName,name);

	return 0;
}

WCHAR * CDPRStation::getPhonePassWord(void)
{
	return comm.PhonelInterface.PassWord;
}

int CDPRStation::setPhonePassWord(WCHAR * name)
{
	lstrcpy(comm.PhonelInterface.PassWord,name);

	return 0;
}

byte CDPRStation::getCommType(void)
{
	return comm.commType;
}

void CDPRStation::setCommType(byte commVal)
{
	comm.commType = commVal;
}

WCHAR * CDPRStation::getStationIPAddr(void)
{
	return comm.NetInterface.IP_Addr;
}

int CDPRStation::setStationIPAddr(WCHAR * addr)
{
	if(!AssertIPFormat(addr))
	{
		return -1;
	}

	lstrcpy(comm.NetInterface.IP_Addr,addr);

	return 0;
}

UINT CDPRStation::getStationIPPort(void)
{
	return comm.NetInterface.Port;
}

int CDPRStation::setStationIPPort(UINT port)
{
	comm.NetInterface.Port = port;

	return 0;
}

WCHAR * CDPRStation::getStationPhoneNO(void)
{
	return comm.PhonelInterface.phone_no;
}

int CDPRStation::setStationPhoneNO(WCHAR * no)
{
	lstrcpy(comm.PhonelInterface.phone_no,no);

	return 0;
}

int CDPRStation::InitStationDataBase(void)
{
	wAddr = 0;

	int sum = theApp.g_sDataBases.size();
	if (sum > MAX_STATION_SUM || sum < 0)
	{
		sum = 0;
	}

	CString tmp;

	tmp.Format(_T("新建子站%03d"),(sum + 1));
	setStationName(tmp.GetBuffer());
	tmp.ReleaseBuffer();

	setStationAddr(sum + 1);
	setStationID((sum + 1) * 100);
	
	if ((sum % 2) == 0)
	{
		setCommType(COMM_TYPE_ETH);
	}
	else
	{
		setCommType(COMM_TYPE_PHO);
	}
	
	setProtocol(H103_PROTOCOL);

	tmp.Format(_T("127.0.0.%d"),(sum + 1));
	setStationIPAddr(tmp.GetBuffer());
	tmp.ReleaseBuffer();

	setStationIPPort(DEFAULT_PORT);

	tmp.Format(_T("0258309754%d"),(sum + 1));
	setStationPhoneNO(tmp.GetBuffer());
	tmp.ReleaseBuffer();

	tmp = _T("");
	setPhoneUsrName(tmp.GetBuffer());
	setPhonePassWord(tmp.GetBuffer());
	tmp.ReleaseBuffer();

	setModemIndex(0);

	return 0;
}

void CDPRStation::CopyStationDataBase(CDPRStation& rhs)
{
	setStationName(rhs.getStationName());
	setStationID(rhs.getStationID());
	setCommType(rhs.getCommType());
	setProtocol(rhs.getProtocol());
	setStationIPAddr(rhs.getStationIPAddr());
	setStationIPPort(rhs.getStationIPPort());
	setStationPhoneNO(rhs.getStationPhoneNO());
	setModemIndex(rhs.getModemIndex());
	setStationAddr(rhs.getStationAddr());
	setPhoneUsrName(rhs.getPhoneUsrName());
	setPhonePassWord(rhs.getPhonePassWord());
}

UINT CDPRStation::getStationID(void)
{
	return StationID;
}

int CDPRStation::setStationID(UINT id)
{
	StationID = id;
	return 0;
}

int CDPRStation::getDevNums(void)
{
	return vDevs.size();
}

const WCHAR * CDPRStation::getStationCatalog(void)
{
	return StationCatalog;
}

int CDPRStation::setStationCatalog(WCHAR * log)
{
	CString strTmp = log;
	while ((strTmp.Right(1)).Compare(_T(" ")) == 0)
	{
		strTmp = strTmp.Left(strTmp.GetLength() - 1);
	}

	CString oldCatalog = getStationCatalog();
	if (oldCatalog.Compare(strTmp) != 0)
	{
		lstrcpy(StationCatalog,strTmp);

		CString oldPath = theApp.g_sDataBases.getRootCatalog() + oldCatalog;
		CString curPath = CString(theApp.g_sDataBases.getRootCatalog()) + getStationCatalog();

		if (FolderExist(oldPath))
		{
			MoveFile(oldPath,curPath);
		}
		else
		{
			BuildDirectory(curPath);
		}
	}
	
	return 0;
}

UINT CDPRStation::getModemIndex(void)
{
	return comm.PhonelInterface.ModemIndex;
}

int CDPRStation::setModemIndex(UINT index)
{
	comm.PhonelInterface.ModemIndex = index;

	return 0;
}

bool CDPRStation::getCommActive(void)
{
	return bCommActive;
}

void CDPRStation::setCommActive(bool val)
{
	bCommActive = val;
	
	if (MainFrame_hWnd != NULL)
	{
		((CMainFrame *)MainFrame)->RefreshStationImage();
	}
}

int CDPRStation::OpenConnect(void)
{
	int ret = 0;

	HANDLE ProtocalHan = 0;
	unsigned long ProtocalID;

	if (getCommType() == COMM_TYPE_PHO)
	{
		CString strRasStatus;
		BOOL bRasConnected;
		if (theApp.rasClient.getConnectionHandle() == NULL)
		{
			HRASCONN hConn;
			if(theApp.rasClient.GetRasConnection(theApp.rasClient.getEntryName(getModemIndex()),hConn))
			{
				theApp.rasClient.setConnectionHandle(hConn);
			}
		}
		if(!theApp.rasClient.GetRasStatusString(strRasStatus,bRasConnected))
		{
			CString str;
			str.Format(_T("拨号连接未建立，请先建立连接"));
			AfxMessageBox(str);
			((CMainFrame *)MainFrame)->AddOutputWarn(str);

			return -1;
		}

		if (!bRasConnected)
		{
			CString str;
			str.Format(_T("拨号连接未建立，请先建立连接"));
			AfxMessageBox(str);
			((CMainFrame *)MainFrame)->AddOutputWarn(str);

			return -1;
		}
		
		DWORD ll;
		RASPPPIP raspppip;
		raspppip.dwSize = sizeof(raspppip);
		DWORD dwRet = RasGetProjectionInfo(theApp.rasClient.getConnectionHandle(),RASP_PppIp,&raspppip,&ll);
		if (dwRet != ERROR_SUCCESS)
		{
			CString str;
			str.Format(_T("获取拨号连接的IP地址失败"));
			AfxMessageBox(str);
			((CMainFrame *)MainFrame)->AddOutputWarn(str);

			return -1;
		}

		setStationIPAddr(raspppip.szServerIpAddress);
		setStationIPPort(DEFAULT_PORT);
	}

	if (myProtocalObj != NULL)
	{
		if(getCommActive() && myProtocalObj->getProtocolActive())
		{
			CString str = _T("请先关闭连接");
			AfxMessageBox(str);
			((CMainFrame *)MainFrame)->AddOutputWarn(str);

			return -1;
		}
		else
		{
			SetEvent(myProtocalObj->gethProExitEvent());
		}
	}

	switch (getProtocol())
	{
	case H103_PROTOCOL:
		myProtocalObj = new CH103(this);
		myProtocalObj->setProtocolActive(true);
		if((ProtocalHan=CreateThread(NULL,0,H103ProtocalProc,myProtocalObj,0,&ProtocalID))==NULL)
		//if(AfxBeginThread(H103ProtocalProc,myProtocalObj) == NULL)
		{
			myProtocalObj->setProtocolActive(false);
			int Error=GetLastError();

			CString str;
			str.Format(_T("子站%d 创建通讯线程失败 错误码:%d"),getMyIndexOfStations(),Error);
			((CMainFrame *)MainFrame)->AddOutputWarn(str);

			return Error;
		}
		//SetThreadPriority(ProtocalHan,THREAD_PRIORITY_HIGHEST);
		CloseHandle(ProtocalHan);
		break;

	default:
		break;
	}
	
	CString str;
	str.Format(_T("子站%d 尝试打开连接..."),getMyIndexOfStations());
	((CMainFrame *)MainFrame)->AddOutputReturn(str);

	return ret;
}

int CDPRStation::CloseConnect(void)
{
	int ret = 0;

	if (myProtocalObj == NULL)
	{
		CString str;
		str.Format(_T("子站规约线程未启动"),getMyIndexOfStations());
		((CMainFrame *)MainFrame)->AddOutputWarn(str);
	}
	else
	{
		SetEvent(myProtocalObj->gethProExitEvent());
		CString str;
		str.Format(_T("子站%d 尝试关闭连接..."),getMyIndexOfStations());
		((CMainFrame *)MainFrame)->AddOutputReturn(str);
	}

	return ret;
}

WORD CDPRStation::getProtocol(void)
{
	return comm.Protocol;
}

void CDPRStation::setProtocol(WORD val)
{
	comm.Protocol = val;
}

/*
int CDPRStation::getCurDevIndex(void)
{
	return curDev;
}

int CDPRStation::setCurDevIndex(int index)
{
	if(index >= vDevs.size())
	{
		return -1;
	}

	curDev = index;

	return 0;
}
*/

int CDPRStation::CommGetSettingCFG(int station,int dev)
{
	if (!getCommActive())
	{
		return PROTOCOL_COMM_ERR;
	}

	if (myProtocalObj == NULL)
	{
		return DATABASE_NO_PROTOCOL_ERR;
	}

	if (station < 0 || station >= theApp.g_sDataBases.size())
	{
		return STATION_INDEX_ERR;
	}

	if (dev < 0 || dev >= vDevs.size())
	{
		return DEV_INDEX_ERR;
	}

	theApp.g_sDataBases.ClearFileBuf();
	
	stFaultFileInfo * ffiPtr = new stFaultFileInfo(station,dev);
	myProtocalObj->AddSendCmd(CALL_DEV_SETTING,1,(DWORD_PTR)ffiPtr,this);

	CString str;
	str.Format(_T("子站%d装置%d 召唤定值"),station,dev);
	((CMainFrame *)MainFrame)->AddOutputReturn(str);

	return 0;
}

int CDPRStation::CommPutSettingCFG(int station,int dev)
{
	if (!getCommActive())
	{
		return PROTOCOL_COMM_ERR;
	}

	if (myProtocalObj == NULL)
	{
		return DATABASE_NO_PROTOCOL_ERR;
	}

	if (station < 0 || station >= theApp.g_sDataBases.size())
	{
		return STATION_INDEX_ERR;
	}

	if (dev < 0 || dev >= vDevs.size())
	{
		return DEV_INDEX_ERR;
	}

	if(AfxMessageBox(_T("确认下发定值？"),MB_YESNO) != IDYES)
	{
		return -1;
	}

	CString setCfgName;
	setCfgName += theApp.g_sDataBases.getRootCatalog();
	setCfgName += theApp.g_sDataBases[station].getStationCatalog();
	setCfgName += theApp.g_sDataBases[station].vDevs.getDevsCatalog();
	setCfgName += theApp.g_sDataBases[station].vDevs[dev].getDevCatalog();
	setCfgName += theApp.g_sDataBases[station].vDevs[dev].setting.getSettingCatalog();
	setCfgName += _T("\\");
	setCfgName += _T(STR_SETCFG_NAME);

	CFile settingFile;
	CFileException ex;
	CString err;

	if(!settingFile.Open(setCfgName,CFile::modeRead))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		err.Format(_T("Couldn't open setting CFG file: %1024s"), szError);
		AfxMessageBox(err);
		((CMainFrame *)MainFrame)->AddOutputWarn(err);

		return -1;
	}
	theApp.g_sDataBases.ClearFileBuf();
	settingFile.SeekToBegin();

	int count = settingFile.Read(theApp.g_sDataBases.fileBuf,settingFile.GetLength());
	if (count > 0)
	{
		theApp.g_sDataBases.setFileLength(count);
	}
	else
	{
		AfxMessageBox(err);
		((CMainFrame *)MainFrame)->AddOutputWarn(err);

		return -1;
	}

	stFaultFileInfo * ffiPtr = new stFaultFileInfo(station,dev);
	myProtocalObj->AddSendCmd(START_DEV_SETTING,1,(DWORD_PTR)ffiPtr,this);

	settingFile.Close();

	CString str;
	str.Format(_T("子站%d装置%d 下发定值"),station,dev);
	((CMainFrame *)MainFrame)->AddOutputReturn(str);

	return 0;
}

int CDPRStation::CommCallFaultFileMenu(int station, int dev)
{
	if (!getCommActive())
	{
		return PROTOCOL_COMM_ERR;
	}

	if (myProtocalObj == NULL)
	{
		return DATABASE_NO_PROTOCOL_ERR;
	}

	if (station < 0 || station >= theApp.g_sDataBases.size())
	{
		return STATION_INDEX_ERR;
	}

	if (dev < 0 || dev >= vDevs.size())
	{
		return DEV_INDEX_ERR;
	}

	vDevs[dev].faultfiles.clear();
	stFaultFileInfo * ffiPtr = new stFaultFileInfo(station,dev);
	myProtocalObj->AddSendCmd(CALL_FFI_MENU,1,(DWORD_PTR)ffiPtr,this);

	CString str;
	str.Format(_T("子站%d装置%d 召唤故障文件目录"),station,dev);
	((CMainFrame *)MainFrame)->AddOutputReturn(str);

	return 0;
}

int CDPRStation::CommCallFaultFile(int station, int dev,int file)
{
	if (!getCommActive())
	{
		return PROTOCOL_COMM_ERR;
	}

	if (myProtocalObj == NULL)
	{
		return DATABASE_NO_PROTOCOL_ERR;
	}

	if (station < 0 || station >= theApp.g_sDataBases.size())
	{
		return STATION_INDEX_ERR;
	}

	if (dev < 0 || dev >= vDevs.size())
	{
		return DEV_INDEX_ERR;
	}

	if (file < 0 || file >= vDevs[dev].faultfiles.size())
	{
		return FAULTFILE_INDEX_ERR;
	}

	theApp.g_sDataBases.ClearFileBuf();

	stFaultFileInfo * ffiPtr = new stFaultFileInfo(station,dev,file);
	myProtocalObj->AddSendCmd(CALL_FFI_FILE,1,(DWORD_PTR)ffiPtr,this);

	CString str;
	str.Format(_T("子站%d装置%d 召唤故障文件"),station,dev);
	((CMainFrame *)MainFrame)->AddOutputReturn(str);

	return 0;
}

int CDPRStation::CommSendTrigRecord(int station, int dev)
{
	if (!getCommActive())
	{
		return PROTOCOL_COMM_ERR;
	}

	if (myProtocalObj == NULL)
	{
		return DATABASE_NO_PROTOCOL_ERR;
	}

	if (station < 0 || station >= theApp.g_sDataBases.size())
	{
		return STATION_INDEX_ERR;
	}

	if (dev < 0 || dev >= vDevs.size())
	{
		return DEV_INDEX_ERR;
	}

	stFaultFileInfo * ffiPtr = new stFaultFileInfo(station,dev);
	myProtocalObj->AddSendCmd(TRIG_DEV_RECORD,1,(DWORD_PTR)ffiPtr,this);

	CString str;
	str.Format(_T("子站%d装置%d 启动录波"),station,dev);
	((CMainFrame *)MainFrame)->AddOutputReturn(str);

	return 0;
}

int CDPRStation::CommCallDevTime(int station, int dev)
{
	if (!getCommActive())
	{
		return PROTOCOL_COMM_ERR;
	}

	if (myProtocalObj == NULL)
	{
		return DATABASE_NO_PROTOCOL_ERR;
	}

	if (station < 0 || station >= theApp.g_sDataBases.size())
	{
		return STATION_INDEX_ERR;
	}

	if (dev < 0 || dev >= vDevs.size())
	{
		return DEV_INDEX_ERR;
	}

	vDevs[dev].faultfiles.clear();
	stFaultFileInfo * ffiPtr = new stFaultFileInfo(station,dev);
	myProtocalObj->AddSendCmd(CALL_TIME,1,(DWORD_PTR)ffiPtr,this);

	CString str;
	str.Format(_T("子站%d装置%d 召唤时间"),station,dev);
	((CMainFrame *)MainFrame)->AddOutputReturn(str);

	return 0;
}

int CDPRStation::CommSetDevTime(int station, int dev)
{
	if (!getCommActive())
	{
		return PROTOCOL_COMM_ERR;
	}

	if (myProtocalObj == NULL)
	{
		return DATABASE_NO_PROTOCOL_ERR;
	}

	if (station < 0 || station >= theApp.g_sDataBases.size())
	{
		return STATION_INDEX_ERR;
	}

	if (dev < 0 || dev >= vDevs.size())
	{
		return DEV_INDEX_ERR;
	}

	if(AfxMessageBox(_T("确认下发时钟？"),MB_YESNO) != IDYES)
	{
		return -1;
	}

	vDevs[dev].faultfiles.clear();
	stFaultFileInfo * ffiPtr = new stFaultFileInfo(station,dev);
	myProtocalObj->AddSendCmd(SET_TIME,1,(DWORD_PTR)ffiPtr,this);

	CString str;
	str.Format(_T("子站%d装置%d 下发时间"),station,dev);
	((CMainFrame *)MainFrame)->AddOutputReturn(str);

	return 0;
}

int CDPRStation::getFCB(void)
{
	return FCB;
}

int CDPRStation::setFCB(byte val)
{
	FCB = val;
	return 0;
}

int CDPRStation::ChangeFCB(void)
{
	if (FCB == 0x00)
	{
		FCB = 0x20;
	} 
	else
	{
		FCB = 0x00;
	}

	return 0;
}

int CDPRStation::getStationAddr()
{
	return wAddr;
}

int CDPRStation::setStationAddr(WORD val)
{
	wAddr = val;
	return 0;
}

UINT CDPRStation::getMyIndexOfStations(void)
{
	return iMyIndexOfStations;
}

void CDPRStation::ClearMainFrmPtr(void)
{
	MainFrame = NULL;
	MainFrame_hWnd = NULL;
}

int CDPRStation::DialUp()
{
	if (getCommType() == COMM_TYPE_PHO)
	{
		DWORD ll;
		RASPPPIP raspppip;

		CString strRasStats = _T("");
		BOOL bRasConnected = FALSE;

		if (theApp.rasClient.getConnectionHandle() == NULL)
		{
			HRASCONN hConn;
			if(theApp.rasClient.GetRasConnection(theApp.rasClient.getEntryName(getModemIndex()),hConn))
			{
				theApp.rasClient.setConnectionHandle(hConn);
			}
		}
		theApp.rasClient.GetRasStatusString(strRasStats,bRasConnected);
		if(bRasConnected)
		{
			CString str = _T("Modem已经拨号，可以打开连接");
			((CMainFrame *)MainFrame)->AddOutputReturn(str);
			RasGetProjectionInfo(theApp.rasClient.getConnectionHandle(),RASP_PppIpx,&raspppip,&ll);
			setStationIPAddr(raspppip.szServerIpAddress);
			setStationIPPort(DEFAULT_PORT);

		}
		else
		{
			CString str = _T("尝试拨号...");
			((CMainFrame *)MainFrame)->AddOutputReturn(str);
			theApp.rasClient.RasDialAsynCallback2(theApp.rasClient.getEntryName(getModemIndex()),RasDialCallBack2,getStationPhoneNO(),getPhoneUsrName(),getPhonePassWord());
		}
	}

	return 0;
}

int CDPRStation::HangOff()
{
	if (getCommType() == COMM_TYPE_PHO)
	{
		if(theApp.rasClient.HangUpConnection(theApp.rasClient.getEntryName(getModemIndex())))
		{
			((CMainFrame *)MainFrame)->AddOutputReturn(_T("挂断拨号成功"));
		}
		else
		{
			((CMainFrame *)MainFrame)->AddOutputReturn(_T("挂断拨号失败"));
		}
		
	}

	return 0;
}

DWORD  CALLBACK CDPRStation::RasDialCallBack2( __in  DWORD dwCallbackId, __in  DWORD dwSubEntry, __in  HRASCONN hrasconn,__in  UINT unMsg,	__in  RASCONNSTATE rascs,__in  DWORD dwError,__in  DWORD dwExtendedError)
{
	int ret = -1;
	CString strTmp;

	switch (rascs)
	{
	case RASCS_DeviceConnected:
		ret = 0;
		if (theApp.g_sDataBases.size() > 0)
		{
			((CMainFrame *)theApp.g_sDataBases[0].MainFrame)->AddOutputReturn(_T("拨号成功"));
		}
		break;

	case RASCS_AllDevicesConnected:
		if (theApp.g_sDataBases.size() > 0)
		{
			((CMainFrame *)theApp.g_sDataBases[0].MainFrame)->AddOutputReturn(_T("拨号断开"));
		}
		ret = 0;
		break;

	default:
		ret = 1;
		if (dwError != 0)
		{
			strTmp.Format(_T("拨号失败 错误码:%d"),dwError);
			AfxMessageBox(strTmp);
			if (theApp.g_sDataBases.size() > 0)
			{
				((CMainFrame *)theApp.g_sDataBases[0].MainFrame)->AddOutputWarn(strTmp);
			}
		}
		break;
		

	}

	return ret;
}
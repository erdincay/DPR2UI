#include "StdAfx.h"
#include "DEV.h"
#include "DPR2UI.h"

//class CDPRStation;
//class CDPRDevs;

CDPRDev::CDPRDev(void)
{
	setParentDEVs(NULL);

	InitDevDataBase();

	faultfiles.setParentDev(this);
	setting.setParentDev(this);
}

CDPRDev::CDPRDev(CDPRDevs * ptr)
{
	setParentDEVs(ptr);

	InitDevDataBase();

	faultfiles.setParentDev(this);
	setting.setParentDev(this);
}

CDPRDev::CDPRDev(const CDPRDev & rhs)
{
	parentDEVs = rhs.parentDEVs;

	CopyDevDataBase((CDPRDev &)rhs);

	faultfiles = rhs.faultfiles;
	setting = rhs.setting;

	faultfiles.setParentDev(this);
	setting.setParentDev(this);
}

CDPRDev::~CDPRDev(void)
{
}

UINT CDPRDev::getDevID(void)
{
	return DEV_ID;
}

void CDPRDev::setDevID(UINT id)
{
	if (id != DEV_ID)
	{
		DEV_ID = id;
		CString strId;
		strId.Format(_T("\\%.4d"),getDevID());
		setDevCatalog(strId.GetBuffer());
		strId.ReleaseBuffer();
	}
	
}

WCHAR * CDPRDev::getDevCatalog(void)
{
	return DevCatalog;
}

int CDPRDev::setDevCatalog(WCHAR * log)
{
	CString oldCatalog = getDevCatalog();
	if (oldCatalog.Compare(log) != 0)
	{
		lstrcpy(DevCatalog,log);

		CDPRDevs * devsPtr = (CDPRDevs *)getParentDEVs();
		if (devsPtr != NULL)
		{
			CDPRStation * stationPtr = (CDPRStation *)(devsPtr->getParentStationPtr());
			if (stationPtr != NULL)
			{
				CString oldPath = CString(theApp.g_sDataBases.getRootCatalog()) + CString(stationPtr->getStationCatalog()) + CString(devsPtr->getDevsCatalog()) + CString(oldCatalog);
				CString curPath = CString(theApp.g_sDataBases.getRootCatalog()) + CString(stationPtr->getStationCatalog()) + CString(devsPtr->getDevsCatalog()) + CString(getDevCatalog());

				if (FolderExist(oldPath))
				{
					MoveFile(oldPath,curPath);
				}
				else
				{
					BuildDirectory(curPath);
				}
			}
			else
			{
				return -1;
			}
		}
		else
		{
			return -1;
		}

	}

	return 0;
}

/*
UINT CDPRDev::getFaultFileIndex(void)
{
	return curFaultFile;
}

void CDPRDev::setFaultFileIndex(UINT index)
{
	curFaultFile = index;
}
*/

void CDPRDev::InitDevDataBase(void)
{
	setDevAddr(1);
	setDevID(getDevAddr() * 100);
	GetLocalTime(&devTM);
}

void CDPRDev::CopyDevDataBase(CDPRDev & rhs)
{
	setDevAddr(rhs.getDevAddr());
	setDevID(rhs.getDevID());
	setDevTime(rhs.getDevTime());
}

int CDPRDev::getDevAddr(void)
{
	return wAddr;
}

int CDPRDev::setDevAddr(WORD val)
{
	wAddr = val;
	return 0;
}

void CDPRDev::setParentDEVs(CDPRDevs * ptr)
{
	parentDEVs = ptr;
}

const CDPRDevs * CDPRDev::getParentDEVs()
{
	return parentDEVs;
}

SYSTEMTIME CDPRDev::getDevTime(void)
{
	return devTM;
}

void CDPRDev::setDevTime(SYSTEMTIME tm)
{
	devTM = tm;
}

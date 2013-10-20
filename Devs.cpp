#include "StdAfx.h"
#include "Devs.h"
#include "DPR2UI.h"

CDPRDevs::CDPRDevs(void)
{
	setParentStationPtr(NULL);
	setDevsCatalog(_T("\\DEVs"));

	/*
	for (int i=0; i<2; i++)
	{
		CDPRDev dev(this);
		push_back(dev);
	}
	*/
}

CDPRDevs::CDPRDevs(CDPRStation * ptr)
{
	setParentStationPtr(ptr);
	setDevsCatalog(_T("\\DEVs"));

	/*
	for (int i=0; i<2; i++)
	{
		CDPRDev dev(this);
		push_back(dev);
	}
	*/
}

CDPRDevs::~CDPRDevs(void)
{
}

void CDPRDevs::push_back(const CDPRDev & _Val)
{
	if (size() >= MAX_STATION_SUM)
	{
		CString strTmp;
		strTmp.Format(_T("装置数量超过限值%d"),MAX_DEV_SUM);
		AfxMessageBox(strTmp);
	}
	else
	{
		std::vector<CDPRDev>::push_back(_Val);
	}

}

WCHAR * CDPRDevs::getDevsCatalog(void)
{
	return DevsCatalog;
}

int CDPRDevs::setDevsCatalog(WCHAR * log)
{
	CString oldCatalog = getDevsCatalog();
	if (oldCatalog.Compare(log) != 0)
	{
		lstrcpy(DevsCatalog,log);

		if (getParentStationPtr() != NULL)
		{
			CString oldPath = CString(theApp.g_sDataBases.getRootCatalog()) + CString(((CDPRStation *)getParentStationPtr())->getStationCatalog()) + oldCatalog;
			CString curPath = CString(theApp.g_sDataBases.getRootCatalog()) + CString(((CDPRStation *)getParentStationPtr())->getStationCatalog()) + getDevsCatalog();

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

	return 0;
}

void CDPRDevs::setParentStationPtr(CDPRStation * ptr)
{
	parentStation = ptr;
}

const CDPRStation * CDPRDevs::getParentStationPtr()
{
	return parentStation;
}

int CDPRDevs::getDevIndexByAddr(WORD val)
{
	for (int i=0;i<size();i++)
	{
		if (val == at(i).getDevAddr())
		{
			return i;
		}
	}

	return -1;
}
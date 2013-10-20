#include "StdAfx.h"
#include "FaultFiles.h"
#include "DPR2UI.h"
#include "Markup.h"

CFaultFiles::CFaultFiles(void)
{
	setParentDev(NULL);
	setFaultFilesCatalog(_T("\\FaultFiles"));
}

CFaultFiles::CFaultFiles(CDPRDev * ptr)
{
	setParentDev(ptr);
	setFaultFilesCatalog(_T("\\FaultFiles"));
}

CFaultFiles::~CFaultFiles(void)
{
}

void CFaultFiles::setParentDev(CDPRDev * ptr)
{
	parentDev = ptr;
}

const CDPRDev * CFaultFiles::getParentDev()
{
	return parentDev;
}

WCHAR * CFaultFiles::getFaultFilesCatalog(void)
{
	return FFsCatalog;
}

int CFaultFiles::setFaultFilesCatalog(WCHAR * log)
{
	CString oldCatalog = getFaultFilesCatalog();
	if (oldCatalog.Compare(log) != 0)
	{
		lstrcpy(FFsCatalog,log);

		CDPRDev * DEVPtr = (CDPRDev *)getParentDev();
		if (DEVPtr != NULL)
		{
			CDPRDevs * DEVsPtr = (CDPRDevs *)DEVPtr->getParentDEVs();
			if (DEVsPtr != NULL)
			{
				CDPRStation * StationPtr = (CDPRStation *)DEVsPtr->getParentStationPtr();
				if (StationPtr != NULL)
				{
					CString oldPath = CString(theApp.g_sDataBases.getRootCatalog()) + CString(StationPtr->getStationCatalog()) + CString(DEVsPtr->getDevsCatalog()) + CString(DEVPtr->getDevCatalog()) + oldCatalog;
					CString curPath = CString(theApp.g_sDataBases.getRootCatalog()) + CString(StationPtr->getStationCatalog()) + CString(DEVsPtr->getDevsCatalog()) + CString(DEVPtr->getDevCatalog()) + getFaultFilesCatalog();
					
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
		else
		{
			return -1;
		}
	}

	return 0;
}

void CFaultFiles::SaveFaultMenu(int stationIndex,int devIndex)
{
	if (stationIndex < 0 || stationIndex >= theApp.g_sDataBases.size())
	{
		return;
	}

	if (devIndex < 0 || devIndex >= theApp.g_sDataBases[stationIndex].vDevs.size())
	{
		return;
	}

	if (size() == 0)
	{
		return;
	}

	CMarkup xml;
	xml.SetDoc(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"));

	xml.AddElem(_T("meNu"));

	xml.IntoElem();
	for (int i=0;i<size();i++)
	{
		xml.AddElem(_T("fAUltFIle"));
		xml.AddChildElem(_T("fUllnAme"),at(i).getFaultFileName());
		xml.AddChildElem(_T("loCalnAme"),at(i).getLocalFileName());
		xml.AddChildElem(_T("trIgrEasOn"),at(i).getTrigReason());
		SYSTEMTIME tm = at(i).getFileTime();
		CTime timeTmp(tm);
		xml.AddChildElem(_T("trIgtIme"),timeTmp.GetTime());
	}
	xml.OutOfElem();

	CString	pathName = theApp.g_sDataBases.getRootCatalog();
	pathName += theApp.g_sDataBases[stationIndex].getStationCatalog();
	pathName += theApp.g_sDataBases[stationIndex].vDevs.getDevsCatalog();
	pathName += theApp.g_sDataBases[stationIndex].vDevs[devIndex].getDevCatalog();
	pathName += getFaultFilesCatalog();

	if (!FolderExist(pathName))
	{
		BuildDirectory(pathName);
	}

	pathName += _T("\\menu.xml");

	xml.Save(pathName);
}

void CFaultFiles::LoadFaultMenu(int stationIndex,int devIndex)
{
	if (stationIndex < 0 || stationIndex >= theApp.g_sDataBases.size())
	{
		return;
	}

	if (devIndex < 0 || devIndex >= theApp.g_sDataBases[stationIndex].vDevs.size())
	{
		return;
	}

	CString	pathName = theApp.g_sDataBases.getRootCatalog();
	pathName += theApp.g_sDataBases[stationIndex].getStationCatalog();
	pathName += theApp.g_sDataBases[stationIndex].vDevs.getDevsCatalog();
	pathName += theApp.g_sDataBases[stationIndex].vDevs[devIndex].getDevCatalog();
	pathName += getFaultFilesCatalog();
	pathName += _T("\\menu.xml");

	if (!FileExist(pathName))
	{
		return;
	}

	CMarkup xml;

	if(!xml.Load(pathName))
	{
		return;
	}

	clear();

	xml.ResetMainPos();
	xml.FindElem(); //root
	xml.IntoElem();

	CFaultFile ffTmp;
	CString strTmp;
	while(xml.FindElem(_T("fAUltFIle")))
	{
		if (xml.FindChildElem(_T("fUllnAme")))
		{
			strTmp = xml.GetChildData();
			ffTmp.setFaultFileName(strTmp.GetBuffer());
			strTmp.ReleaseBuffer();
		}

		if (xml.FindChildElem(_T("loCalnAme")))
		{
			strTmp = xml.GetChildData();
			ffTmp.setLocalFileName(strTmp.GetBuffer());
			strTmp.ReleaseBuffer();
		}

		if (xml.FindChildElem(_T("trIgrEasOn")))
		{
			strTmp = xml.GetChildData();
			ffTmp.setTrigReason(strTmp.GetBuffer());
			strTmp.ReleaseBuffer();
		}

		if (xml.FindChildElem(_T("trIgtIme")))
		{
			strTmp = xml.GetChildData();
			int tm;
			if (CString2Int(strTmp,tm))
			{
				CTime timeTmp((time_t)tm);
				SYSTEMTIME time;
				timeTmp.GetAsSystemTime(time);
				ffTmp.setFileTime(time);
			}
		}

		push_back(ffTmp);
	}
}

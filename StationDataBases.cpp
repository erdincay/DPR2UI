#include "StdAfx.h"
#include "MainFrm.h"
#include "StationDataBases.h"

CDPRStations::CDPRStations(void)
{
	//初始化界面相关参数
	//MainFrame      = AfxGetMainWnd();
	//MainFrame_hWnd = AfxGetMainWnd()->GetSafeHwnd();

	fileLength = 0;
	memset(fileBuf,0,sizeof(fileBuf));

	GetCurrentDirectory(MAX_PATH,wOriginalCatalog);

	CString curPath;
	GetCurrentDirectory(MAX_PATH,curPath.GetBufferSetLength(MAX_PATH + 1));
	curPath.ReleaseBuffer();

	curPath += _T("\\DPR2UI");
	setRootCatalog(curPath.GetBuffer());
	curPath.ReleaseBuffer();
}

CDPRStations::~CDPRStations(void)
{
}

void CDPRStations::push_back(const CDPRStation & _Val)
{
	if (size() >= MAX_STATION_SUM)
	{
		CString strTmp;
		strTmp.Format(_T("子站数量超过限值%d"),MAX_STATION_SUM);
		AfxMessageBox(strTmp);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(strTmp);
	}
	else
	{
		std::vector<CDPRStation>::push_back(_Val);
	}
	
}

/*
int CDPRStations::getCurStationIndex(void)
{
	return curStation;
}

int CDPRStations::setCurStationIndex(int index)
{
	if (index >= size())
	{
		return -1;
	}

	curStation = index;

	return 0;
}
*/

int CDPRStations::SaveBufToFile(int iStationIndex,int iDevIndex,int iFileIndex)
{
	if(iStationIndex < 0 || iStationIndex >= size())
	{
		return -1;
	}

	if (iDevIndex < 0 || iDevIndex >= at(iStationIndex).vDevs.size())
	{
		return -1;
	}

	/*
	if (iFileIndex < 0 || iFileIndex >= at(iStationIndex).vDevs[iDevIndex].faultfiles.size())
	{
		return -1;
	}
	*/

	byte * filePtr = fileBuf;
	
	int namelength = strlen((char *)filePtr);
	if (namelength <= 0)
	{
		return -1;
	}
	WCHAR * name = ChartoWCHAR((char *)filePtr);

	filePtr += MAX_NAME_LENGTH;
	
	while((getFileLength() - (filePtr - fileBuf)) >= 5)
	{
		byte fileType = filePtr[0];

		CString fileName;
		CString pathName;
		if(!ConsummateFileName(name,fileName,fileType))
		{
			return -1;
		}

		if (fileType == SETTING_FILE_TYPE)
		{
			pathName = getRootCatalog();
			pathName += at(iStationIndex).getStationCatalog();
			pathName += at(iStationIndex).vDevs.getDevsCatalog();
			pathName += at(iStationIndex).vDevs[iDevIndex].getDevCatalog();
			pathName += at(iStationIndex).vDevs[iDevIndex].setting.getSettingCatalog();
			pathName += _T("\\");
			pathName += _T(STR_SETCFG_NAME);
		}
		else
		{
			pathName = getRootCatalog();
			pathName += at(iStationIndex).getStationCatalog();
			pathName += at(iStationIndex).vDevs.getDevsCatalog();
			pathName += at(iStationIndex).vDevs[iDevIndex].getDevCatalog();
			pathName += at(iStationIndex).vDevs[iDevIndex].faultfiles.getFaultFilesCatalog();
			pathName += _T("\\") + fileName;
		}

		int index = pathName.ReverseFind(_T('\\'));
		CString strleft = pathName;
		if (index >= 0)
		{
			strleft = pathName.Left(index);
		}

		if (!FolderExist(strleft))
		{
			BuildDirectory(strleft);
		}

		DWORD fileLength = filePtr[1] + filePtr[2] * 0x100 + filePtr[3] * 0x10000 + filePtr[4] * 0x1000000;
		filePtr += 5;

		if ((getFileLength() - (filePtr - fileBuf)) >= fileLength)
		{
			if (fileType == SETTING_FILE_TYPE)
			{
				SET_STRUCT setTmp;
				if (fileLength == sizeof(setTmp))
				{
					memcpy(&setTmp,filePtr,fileLength);
					WORD recvCheckSum = setTmp.checksum;
					WORD recvAntiCheckSum = setTmp.anti_checksum;
					setTmp.checksum = 0;
					setTmp.anti_checksum = 0;
					WORD CheckSum = CalcCheckSumByte((const byte *)&setTmp,sizeof(setTmp));
					WORD AntiCheckSum = CalcAntiCheckSumWord(CheckSum);

					if (!((CheckSum == recvCheckSum) && (AntiCheckSum == recvAntiCheckSum)))
					{
						filePtr += fileLength;
						CString str = _T("定值文件校验错误");
						AfxMessageBox(str);
						if (size() > 0)
						{
							((CMainFrame *)at(0).MainFrame)->AddOutputWarn(str);
						}
						break;
					}
				}
				else
				{
					filePtr += fileLength;
					CString str = _T("定值文件校验错误");
					AfxMessageBox(str);
					if (size() > 0)
					{
						((CMainFrame *)at(0).MainFrame)->AddOutputWarn(str);
					}
					break;
				}
				
			}

			CFile faultfile;
			CFileException ex;
			if(!faultfile.Open(pathName,CFile::modeCreate | CFile::modeWrite,&ex))
			{
				TCHAR szError[1024];
				ex.GetErrorMessage(szError, 1024);
				_tprintf_s(_T("Couldn't open source file: %1024s"), szError);

				return -1;
			}
			faultfile.Write(filePtr,fileLength);

			faultfile.Close();

			filePtr += fileLength;

			switch (fileType)
			{
			case INF_FILE_TYPE:
				//PostMessage(at(iStationIndex).MainFrame_hWnd,FAULT_FILE_MSG,iStationIndex,iDevIndex);
				break;

			case CFG_FILE_TYPE:
				//PostMessage(at(iStationIndex).MainFrame_hWnd,FAULT_FILE_MSG,iStationIndex,iDevIndex);
				break;

			case DAT_FILE_TYPE:
				PostMessage(at(iStationIndex).MainFrame_hWnd,FAULT_FILE_MSG,iStationIndex,iDevIndex);
				break;

			case SETTING_FILE_TYPE:
				at(iStationIndex).vDevs[iDevIndex].setting.LoadSettingCFG(pathName);
				PostMessage(at(iStationIndex).MainFrame_hWnd,SETTING_FILE_MSG,iStationIndex,iDevIndex);
				break;

			default:
				break;
			}
		}
		else
		{
			return -1;
		}

	}

	ClearFileBuf();

	delete name;
	
	return 0;
}

bool CDPRStations::ConsummateFileName(WCHAR * srcName,CString &dstName,byte type)
{
	switch (type)
	{
	case INF_FILE_TYPE:
		dstName = CString(srcName) + _T(".inf");
		break;

	case CFG_FILE_TYPE:
		dstName = CString(srcName) + _T(".cfg");
		break;

	case DAT_FILE_TYPE:
		dstName = CString(srcName) + _T(".dat");
		break;

	case SETTING_FILE_TYPE:
		dstName = srcName;
		break;

	default:
		return false;
	}

	int index = dstName.ReverseFind(_T('//'));
	if (index >= 0)
	{
		CString strleft = dstName.Left(index);
		CString strRight = dstName.Right(dstName.GetLength() - index - 1);
		
		index = strleft.ReverseFind(_T('//'));
		if (index >= 0)
		{
			CString menu = strleft.Right(strleft.GetLength() - index - 1);

			if (type == SETTING_FILE_TYPE)
			{
				dstName = strRight;
			}
			else
			{
				dstName = menu + _T("\\") + strRight;
			}
		}
	}

	return true;
}

WCHAR * CDPRStations::getRootCatalog(void)
{
	return wRootCatalog;
}

int CDPRStations::setRootCatalog(WCHAR * log)
{
	CString oldCatalog = getRootCatalog();
	if (oldCatalog.Compare(log) != 0)
	{
		lstrcpy(wRootCatalog,log);

		if (FolderExist(oldCatalog))
		{
			MoveFile(oldCatalog,getRootCatalog());
		}
		else
		{
			BuildDirectory(getRootCatalog());
		}
	}

	return 0;
}

WCHAR * CDPRStations::getOriginalCatalog()
{
	return wOriginalCatalog;
}

DWORD CDPRStations::getFileLength(void)
{
	return fileLength;
}

void CDPRStations::setFileLength(DWORD val)
{
	fileLength = val;
}

void CDPRStations::ClearFileBuf(void)
{
	memset(fileBuf,0,sizeof(fileBuf));
	setFileLength(0);
}

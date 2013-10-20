#include "StdAfx.h"
#include "SettingVal.h"
#include "DPR2UI.h"


CSettingVal::CSettingVal(void)
{
	setbInitStruct(false);
	memset(&settingVal,0,sizeof(settingVal));
	setParentDev(NULL);
	setSettingCatalog(_T("\\Setting"));
}

CSettingVal::CSettingVal(CDPRDev * ptr)
{
	setbInitStruct(false);
	memset(&settingVal,0,sizeof(settingVal));
	setParentDev(ptr);
	setSettingCatalog(_T("\\Setting"));
}

CSettingVal::~CSettingVal(void)
{
}

void CSettingVal::setParentDev(CDPRDev * ptr)
{
	parentDev = ptr;
}

const CDPRDev * CSettingVal::getParentDev()
{
	return parentDev;
}

WCHAR * CSettingVal::getSettingCatalog(void)
{
	return SettingCatalog;
}

int CSettingVal::setSettingCatalog(WCHAR * log)
{
	CString oldCatalog = getSettingCatalog();
	if (oldCatalog.Compare(log) != 0)
	{
		lstrcpy(SettingCatalog,log);

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
					CString curPath = CString(theApp.g_sDataBases.getRootCatalog()) + CString(StationPtr->getStationCatalog()) + CString(DEVsPtr->getDevsCatalog()) + CString(DEVPtr->getDevCatalog()) + getSettingCatalog();

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
int CSettingVal::LoadSettingCFG(CString fileName)
{
	if (!FileExist(fileName))
	{
		return -1;
	}

	CFile settingFile;
	CFileException ex;

	if(!settingFile.Open(fileName,CFile::modeRead))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);

		return -1;
	}

	if (settingFile.GetLength() == sizeof(settingVal))
	{
		settingFile.Read(&settingVal,sizeof(settingVal));
		setbInitStruct(true);

		settingFile.Close();
		return 0;
	}

	settingFile.Close();
	return -1;
}

int CSettingVal::SaveSettingCFG(CString fileName)
{
	CFile settingFile;
	CFileException ex;

	if(!settingFile.Open(fileName,CFile::modeCreate | CFile::modeWrite,&ex))
	{
		TCHAR szError[1024];
		ex.GetErrorMessage(szError, 1024);
		_tprintf_s(_T("Couldn't open source file: %1024s"), szError);

		return -1;
	}

	setCheckSum(0);
	setAnti_CheckSum(0);
	WORD checkSum = CalcCheckSumByte((const byte *)&settingVal,sizeof(settingVal));
	setCheckSum(checkSum);
	WORD anti_checkSum = CalcAntiCheckSumWord(checkSum);
	setAnti_CheckSum(anti_checkSum);

	settingFile.Write((const byte *)&settingVal,sizeof(settingVal));

	return 0;
}

int CSettingVal::getAIChannelSum()
{
	if (settingVal.ai_channels > MAX_AI_SUM)
	{
		return MAX_AI_SUM;
	}
	else
	{
		return settingVal.ai_channels;
	}
}

int CSettingVal::setAIChannelSum(int val)
{
	if (val > MAX_AI_SUM || val < 0)
	{
		return -1;
	}

	settingVal.ai_channels = val;

	return 0;
}

const char * CSettingVal::getAIChannelName(int index)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return NULL;
	}

	return settingVal.AI_channel_id[index];
}

int CSettingVal::setAIChannelName(int index,const char * name)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return -1;
	}

	lstrcpyA(settingVal.AI_channel_id[index],name);

	return 0;
}

int CSettingVal::getChannelType(int index)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.channel_type[index];
}

int CSettingVal::setChannelType(int index,byte channelType)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return -1;
	}

	settingVal.channel_type[index] = channelType;

	return 0;
}

float CSettingVal::getChannelPtCt(int index)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.pt_ct_ratio[index];
}

int CSettingVal::setChannelPtCt(INT index,float val)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return -1;
	}

	settingVal.pt_ct_ratio[index] = val;

	return 0;
}

float CSettingVal::getChannelUpperLimit(int index)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.amplitude_limit[index].high;
}

float CSettingVal::getChannelLowerLimit(int index)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.amplitude_limit[index].low;
}

int CSettingVal::setChannelUpperLimit(int index,float val)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return -1;
	}

	settingVal.amplitude_limit[index].high = val;

	return 0;
}

int CSettingVal::setChannelLowerLimit(int index,float val)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return -1;
	}

	settingVal.amplitude_limit[index].low = val;

	return 0;
}

float CSettingVal::getChannelAmplitudeChange(int index)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.amplitude_change[index];
}

int CSettingVal::setChannelAmplitudeChange(int index,float val)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return -1;
	}

	settingVal.amplitude_change[index] = val;

	return 0;
}
float CSettingVal::getChannelSlowAmplitudeChange(int index)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.slow_amplitude_change[index];
}

int CSettingVal::setChannelSlowAmplitudeChange(int index,float val)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return -1;
	}

	settingVal.slow_amplitude_change[index] = val;

	return 0;
}

float CSettingVal::getChannelAmplitudeNull(int index)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.amplitude_null[index];
}

int CSettingVal::setChannelAmplitudeNull(int index,float val)
{
	if (index >= MAX_AI_SUM || index < 0)
	{
		return -1;
	}
	
	settingVal.amplitude_null[index] = val;

	return 0;

}
float CSettingVal::getFreqUpperLimit(int index)
{
	if (index >= MAX_FRE_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.freq_limit.high;
}

int CSettingVal::setFreqUpperLimit( int index /*= 0*/,float val /*= 0*/ )
{
	if (index >= MAX_FRE_SUM || index < 0)
	{
		return -1;
	}

	settingVal.freq_limit.high = val;

	return 0;
}

float CSettingVal::getFreqLowerLimit(int index)
{
	if (index >= MAX_FRE_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.freq_limit.low;
}

int CSettingVal::setFreqLowerLimit( int index /*= 0*/,float val /*= 0*/ )
{
	if (index >= MAX_FRE_SUM || index < 0)
	{
		return -1;
	}

	settingVal.freq_limit.low = val;
	
	return 0;
}

float CSettingVal::getFreqUpperDisLimit(int index)
{
	if (index >= MAX_FRE_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.freq_dis_limit.high;
}

int CSettingVal::setFreqUpperDisLimit( int index /*= 0*/, float val /*= 0*/ )
{
	if (index >= MAX_FRE_SUM || index < 0)
	{
		return -1;
	}

	settingVal.freq_dis_limit.high = val;

	return 0;
}

float CSettingVal::getFreqLowerDisLimit(int index)
{
	if (index >= MAX_FRE_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.freq_dis_limit.low;
}

int CSettingVal::setFreqLowerDisLimit( int index /*= 0*/, float val /*= 0*/ )
{
	if (index >= MAX_FRE_SUM || index < 0)
	{
		return -1;
	}

	settingVal.freq_dis_limit.low = val;

	return 0;
}

float CSettingVal::getFreqChangeRate(int index)
{
	if (index >= MAX_FRE_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.freq_change_rate;
}

int CSettingVal::setFreqChangeRate( int index /*= 0*/,float val /*= 0*/ )
{
	if (index >= MAX_FRE_SUM || index < 0)
	{
		return -1;
	}

	settingVal.freq_change_rate = val;

	return 0;
}

int CSettingVal::getDIChannelSum(void)
{
	if(settingVal.di_words * 32 > MAX_DI_SUM)
	{
		return MAX_DI_SUM;
	}
	else
	{
		return settingVal.di_words * 32;
	}
}

int CSettingVal::setDIChannelSum(int val)
{
	if (val < 0 || val > MAX_DI_SUM)
	{
		return -1;
	}

	settingVal.di_words = val / 32;
	if ((val % 32) > 0)
	{
		settingVal.di_words++;
	}

	return 0;
}

const char * CSettingVal::getDIChannelName(int index)
{
	if (index >= MAX_DI_SUM || index < 0)
	{
		return NULL;
	}

	return settingVal.DI_channel_id[index];
}

int CSettingVal::setDIChannelName(int index,const char * name)
{
	if (index >= MAX_DI_SUM || index < 0)
	{
		return -1;
	}

	lstrcpyA(settingVal.DI_channel_id[index],name);

	return 0;
}

int CSettingVal::getDINegativeChangeVal(int index)
{
	if (index >= MAX_DI_SUM || index < 0)
	{
		return -1;
	}

	int Dword_Val = index / 32;
	int Bit_Val = index % 32;

	if ((settingVal.di[Dword_Val].negative_di_change & Dword_mask[Bit_Val]) > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int CSettingVal::setDINegativeChangeVal(int index,bool val)
{
	if (index >= MAX_DI_SUM || index < 0)
	{
		return -1;
	}

	int Dword_Val = index / 32;
	int Bit_Val = index % 32;

	if (val)
	{
		settingVal.di[Dword_Val].negative_di_change = (settingVal.di[Dword_Val].negative_di_change) | (Dword_mask[Bit_Val]);
	}
	else
	{
		settingVal.di[Dword_Val].negative_di_change = (settingVal.di[Dword_Val].negative_di_change) & (~Dword_mask[Bit_Val]);
	}

	return 0;
}

int CSettingVal::getDIPositiveChangeVal(int index)
{
	if (index >= MAX_DI_SUM || index < 0)
	{
		return -1;
	}

	int Dword_Val = index / 32;
	int Bit_Val = index % 32;

	if ((settingVal.di[Dword_Val].positive_di_change & Dword_mask[Bit_Val]) > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int CSettingVal::setDIPositiveChangeVal(int index,bool val)
{
	if (index >= MAX_DI_SUM || index < 0)
	{
		return -1;
	}

	int Dword_Val = index / 32;
	int Bit_Val = index % 32;

	if (val)
	{
		settingVal.di[Dword_Val].positive_di_change = (settingVal.di[Dword_Val].positive_di_change) | (Dword_mask[Bit_Val]);
	}
	else
	{
		settingVal.di[Dword_Val].positive_di_change = (settingVal.di[Dword_Val].positive_di_change) & (~Dword_mask[Bit_Val]);
	}

	return 0;
}

int CSettingVal::getGroupSum(void)
{
	if (settingVal.total_groups > MAX_GROUP_SUM)
	{
		return MAX_GROUP_SUM;
	}
	else
	{
		return settingVal.total_groups;
	}
}

int CSettingVal::setGroupSum(int val)
{
	if (val < 0 || val > MAX_GROUP_SUM)
	{
		return -1;
	}

	settingVal.total_groups = val;

	return 0;
}

float CSettingVal::getPosiNull(int index)
{
	if (index >= MAX_GROUP_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.pos_null[index];
}

int CSettingVal::setPosiNull(int index, float val)
{
	if (index >= MAX_GROUP_SUM || index < 0)
	{
		return -1;
	}

	settingVal.pos_null[index] = val;

	return 0;
}

float CSettingVal::getPosiUpperLimit(int index)
{
	if (index >= MAX_GROUP_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.posi_limit[index].high;
}

int CSettingVal::setPosiUpperLimit(int index, float val)
{
	if (index >= MAX_GROUP_SUM || index < 0)
	{
		return -1;
	}
	settingVal.posi_limit[index].high = val;

	return 0;
}

float CSettingVal::getPosiLowerLimit(int index)
{
	if (index >= MAX_GROUP_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.posi_limit[index].low;
}

int CSettingVal::setPosiLowerLimit(int index, float val)
{
	if (index >= MAX_GROUP_SUM || index < 0)
	{
		return -1;
	}

	settingVal.posi_limit[index].low = val;

	return 0;
}

float CSettingVal::getNegaUpperLimit(int index)
{
	if (index >= MAX_GROUP_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.neg_limit[index];
}

int CSettingVal::setNegaUpperLimit(int index, float val)
{
	if (index >= MAX_GROUP_SUM || index < 0)
	{
		return -1;
	}

	settingVal.neg_limit[index] = val;

	return 0;
}

float CSettingVal::getZeroUpperLimit(int index)
{
	if (index >= MAX_GROUP_SUM || index < 0)
	{
		return -1;
	}

	return settingVal.zero_limit[index];
}

int CSettingVal::setZeroUpperLimit(int index, float val)
{
	if (index >= MAX_GROUP_SUM || index < 0)
	{
		return -1;
	}

	settingVal.zero_limit[index] = val;

	return 0;
}

const unsigned char * CSettingVal::getGroupIDs(int index)
{
	if (index >= MAX_GROUP_SUM || index < 0)
	{
		return NULL;
	}

	return settingVal.group[index];
}

int CSettingVal::setGroupIDs(int index, const unsigned char * val)
{
	if (index >= MAX_GROUP_SUM || index < 0)
	{
		return -1;
	}

	for (int i=0;i<MAX_GROUP_MEMBERS;i++)
	{
		settingVal.group[index][i] = val[i];
	}

	return 0;
}

int CSettingVal::setGroupID(int index,int secondaryIndex,byte val)
{
	if (index >= MAX_GROUP_SUM || index < 0)
	{
		return -1;
	}

	if (secondaryIndex >= MAX_GROUP_MEMBERS || secondaryIndex < 0)
	{
		return -1;
	}

	settingVal.group[index][secondaryIndex] = val;

	return 0;
}

const char * CSettingVal::getGroupName(int index)
{
	if (index >= MAX_GROUP_SUM || index < 0)
	{
		return NULL;
	}

	return settingVal.AI_group_id[index];
}

int CSettingVal::setGroupName(int index, const char * name)
{
	if (index >= MAX_GROUP_SUM || index < 0)
	{
		return -1;
	}

	lstrcpyA(settingVal.AI_group_id[index],name);

	return 0;
}

bool CSettingVal::getbInitStruct(void)
{
	return bInitSetting;
}

void CSettingVal::setbInitStruct(bool val)
{
	bInitSetting = val;
}

int CSettingVal::setCheckSum(WORD val)
{
	settingVal.checksum = val;

	return 0;
}

int CSettingVal::getCheckSum()
{
	return settingVal.checksum;
}

int CSettingVal::setAnti_CheckSum(WORD val)
{
	settingVal.anti_checksum = val;

	return 0;
}

int CSettingVal::getAnti_CheckSum()
{
	return settingVal.anti_checksum;
}

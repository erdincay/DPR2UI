#include "StdAfx.h"
#include "H103.h"
#include "MainFrm.h"

extern CDPR2UIApp theApp;

CH103::CH103(void)
{
	Syn_Char_Num = 4;
	Excepted_Rece_Bytes = 0;
	MaxFrameDataBytes = 4096;

	FrameTypeLength = 1;
	InfoNumLength = 1;
	TransReasonLength = 1;
	AsduAddrLength = 1;
	InfoAddrLength = 2;

	SetFrameLocation(5 + AsduAddrLength);

	/*
	SYX_START_ADDR = 0x0001;
	DYX_START_ADDR = 0x0001;
	YC_START_ADDR = 0x4001;
	YK_START_ADDR = 0x6001;
	YM_START_ADDR = 0x6401;
	*/

	/*
	ANY_FRAME_RECV_TIMEOUT       = 300000;
	HEART_FRAME_SEND_TIMEOUT     = 10000;
	IGRAME_FRAME_RECV_TIMEOUT    = 30000;
	CALL_ALL_DATA_SEND_TIMEOUT   = 600000;
	CALL_ALL_DD_SEND_TIMEOUT     = 900000;
	SET_TIME_FRAME_SEND_TIMEOUT  = 1800000;
	YK_SEL_FRAME_SEND_TIMEOUT    = 25000;
	YK_EXE_FRAME_SEND_TIMEOUT    = 25000;
	YK_CANCLE_FRAME_SEND_TIMEOUT = 25000;
	CALL_PARA_SEND_TIMEOUT       = 25000;
	SET_PARA_SEND_TIMEOUT        = 25000;
	REQUIRE_LINK_SEND_TIMEOUT    = 10000;
	*/

	H103NetClient.setTargetObjectPtr((void *)this);
	H103Log.InitLog(_T("H103.log"));
}

CH103::CH103(CDPRStation  * myDataBase)
{
	Syn_Char_Num = 4;
	Excepted_Rece_Bytes = 0;
	MaxFrameDataBytes = 4096;

	FrameTypeLength = 1;
	InfoNumLength = 1;
	TransReasonLength = 1;
	AsduAddrLength = 1;
	InfoAddrLength = 2;

	SetFrameLocation(5 + AsduAddrLength);

	/*
	SYX_START_ADDR = 0x0001;
	DYX_START_ADDR = 0x0001;
	YC_START_ADDR = 0x4001;
	YK_START_ADDR = 0x6001;
	YM_START_ADDR = 0x6401;
	*/

	/*
	ANY_FRAME_RECV_TIMEOUT       = 300000;
	HEART_FRAME_SEND_TIMEOUT     = 10000;
	IGRAME_FRAME_RECV_TIMEOUT    = 30000;
	CALL_ALL_DATA_SEND_TIMEOUT   = 600000;
	CALL_ALL_DD_SEND_TIMEOUT     = 900000;
	SET_TIME_FRAME_SEND_TIMEOUT  = 1800000;
	YK_SEL_FRAME_SEND_TIMEOUT    = 25000;
	YK_EXE_FRAME_SEND_TIMEOUT    = 25000;
	YK_CANCLE_FRAME_SEND_TIMEOUT = 25000;
	CALL_PARA_SEND_TIMEOUT       = 25000;
	SET_PARA_SEND_TIMEOUT        = 25000;
	REQUIRE_LINK_SEND_TIMEOUT    = 10000;
	*/
		
	H103NetClient.setTargetObjectPtr((void *)this);
	H103Log.InitLog(_T("H103.log"));

	curDPR = myDataBase;
	//lastRTU = curDPR;
	
	lstrcpy(curNet.IP_Addr , curDPR->getStationIPAddr());
	curNet.Port = curDPR->getStationIPPort();
	
	RTUs.push_back(curDPR);
}

CH103::~CH103(void)
{
	
	H103NetClient.setTargetObjectPtr(NULL);
	for (size_t i=0;i<RTUs.size();i++)
	{
		RTUs.at(i)->myProtocalObj = NULL;
	}
}

int CH103::InitH103(void)
{
	memset(&recvBuf,0,sizeof(recvBuf));
	memset(&Sendbuf,0,sizeof(Sendbuf));

	ClearSendCommandQue();

	mytimer.tAnyFrameRecv.KillMyTimer();

	if (curDPR != NULL)
	{
		curDPR->setCommActive(true);
	}

	AddSendCmd(CALL_STATION_CFG,2,-1,curDPR);

	return 0;
}

void CH103::UnInitH103(void)
{
	memset(&recvBuf,0,sizeof(recvBuf));
	memset(&Sendbuf,0,sizeof(byte));

	ClearSendCommandQue();

	mytimer.tAnyFrameRecv.KillMyTimer();

	if (curDPR != NULL)
	{
		curDPR->setCommActive(false);
	}
}

int CH103::OpenConnect()
{
	int ret = 0;
	
	ret = H103NetClient.Init(ProcessRecvData,(LPCTSTR)curNet.IP_Addr,curNet.Port);
	if (!ret)
	{
		H103Log.Add("socket连接成功返回\n");
		((CMainFrame *)(curDPR->MainFrame))->AddOutputReturn(_T("socket连接成功返回"));

		InitH103();
	}
	
	return ret;
}

int CH103::CloseConnect(void)
{
	SetEvent(hProSendEvent);

	UnInitH103();

	H103NetClient.UnInit();

	SetEvent(hProSendEvent);

	return 0;
}

DWORD _stdcall H103ProtocalProc(LPVOID pParam)
//UINT H103ProtocalProc(LPVOID pParam)
{
	CH103 * myH103 = (CH103 *)pParam;
	HANDLE event[3];
	event[0]=myH103->hProExitEvent;
	event[1]=myH103->hProSendEvent;
	event[2]=myH103->hProRecvEvent;

	int countConnectTimes = 0;

	int ret = myH103->OpenConnect();
	while (myH103->getProtocolActive())
	{
		//Sleep(1);
		if (!myH103->curDPR->getCommActive())
		{
			ret = myH103->OpenConnect();
			while(ret)
			{
				ResetEvent(myH103->hProSendEvent);
				ResetEvent(myH103->hProRecvEvent);
				TRACE("\nTheH103Proc Is Connecting....");
				DWORD dwRet = WaitForMultipleObjects(3,event,false,3000);
				if((dwRet-WAIT_OBJECT_0) == 0 || countConnectTimes > 3)
				{
					myH103->H103Log.Add("打开socket连接失败\n");
					if (myH103->curDPR != NULL)
					{
						((CMainFrame *)(myH103->curDPR->MainFrame))->AddOutputReturn(_T("打开socket连接失败"));
					}
					
					myH103->curDPR->myProtocalObj = NULL;
					myH103->curDPR = NULL;
					myH103->setProtocolActive(false);
					delete myH103;

					return 0L;
				}
				ret = myH103->OpenConnect();
				countConnectTimes++;

			}
		}
		else
		{
			::EnterCriticalSection(&myH103->SendCMDQueSection);
			if (myH103->SendCommandQue.empty())
			{
				::LeaveCriticalSection(&myH103->SendCMDQueSection);
				ResetEvent(myH103->hProSendEvent);
				ResetEvent(myH103->hProRecvEvent);
				TRACE("\nTheH103Proc Is Waiting....");
				DWORD dwRet = WaitForMultipleObjects(3,event,false,INFINITE);
				if((dwRet-WAIT_OBJECT_0) == 0)
				{
					myH103->H103Log.Add( "关闭socket，系统消息命令释放socket线程\n" );

					myH103->CloseConnect();

					if (myH103->curDPR != NULL)
					{
						((CMainFrame *)(myH103->curDPR->MainFrame))->AddOutputReturn(_T("关闭socket，系统消息命令释放socket线程"));
						myH103->curDPR->myProtocalObj = NULL;
						myH103->curDPR = NULL;
					}
					
					myH103->setProtocolActive(false);
					delete myH103;
					return 0L;
				}
				else if ((dwRet-WAIT_OBJECT_0) == 1)
				{
					continue;
				}
			}
			else
			{
				::LeaveCriticalSection(&myH103->SendCMDQueSection);
				stSendCmd node = myH103->getMaxPriopriyCmd();
				ret = myH103->SendProcess(node);
				if (ret)
				{
					myH103->CloseConnect();

					switch (ret)
					{
					case PROTOCOL_COMM_ERR:
						//CLog::GetObj().tmWrite( "关闭socket，发送线程发现通道标志为false\n" );
						myH103->H103Log.Add( "关闭socket，发送线程发现通道标志为false\n" );
						if (myH103->curDPR != NULL)
						{
							((CMainFrame *)(myH103->curDPR->MainFrame))->AddOutputReturn(_T("关闭socket，发送线程发现通道标志为false"));
						}
						break;

					default:
						//CLog::GetObj().tmWrite( "关闭socket，发送线程未知错误\n" );
						myH103->H103Log.Add( "关闭socket，发送线程未知错误\n" );
						if (myH103->curDPR != NULL)
						{
							((CMainFrame *)(myH103->curDPR->MainFrame))->AddOutputReturn(_T("关闭socket，发送线程未知错误"));
						}
						break;
					}
				}
			}
		}

	}

	return 0L;
}

void CALLBACK CH103::ProcessRecvData(void * pH103, char * pData, unsigned long DataLength)
{
	CH103 * myThis = (CH103 *)pH103;
	if (pData == NULL && DataLength == 0) //socket 断开
	{
		myThis->CloseConnect();
		myThis->H103Log.Add( "关闭socket，对端关闭socket\n" );
		if (myThis->curDPR != NULL)
		{
			((CMainFrame *)(myThis->curDPR->MainFrame))->AddOutputReturn(_T("关闭socket，对端关闭socket"));
		}
	}
	else //socket接收数据
	{
		CLoopBuf::put_buf((byte *)pData,&(myThis->recvBuf),DataLength);
		myThis->ShowFrame(RECV_FRAME_MSG,(byte *)pData,DataLength);
		myThis->RecvProcess();
	}
	
}

//校验报文头
int CH103::CheckFrameHead(void)
{
	byte tempChar;
	byte temp_buf[10];

	int char_left = CLoopBuf::char_num(&recvBuf);
	while (char_left >= Syn_Char_Num)
	{
		CLoopBuf::copyfrom_buf(temp_buf,&recvBuf,Syn_Char_Num);
		if ((temp_buf[0] == 0x68)&&(temp_buf[3]== 0x68))
		{
			Excepted_Rece_Bytes = temp_buf[1] + (temp_buf[2] & 0x0f) * 0x100 + 6;
			return 0;
		}
		else if(temp_buf[0]==0x10)
		{
			Excepted_Rece_Bytes = 5;
			return 0;
		}
		else
		{
			CLoopBuf::pop_char(&tempChar,&recvBuf);
		}
		char_left = CLoopBuf::char_num(&recvBuf);
		
	}
	return NO_SYN_HEAD;
}

int CH103::CheckFrameTail( byte * buf)
{
	int sum = 0;

	if ((Excepted_Rece_Bytes == 5) && (buf[0] == 0x10))
	{
		sum = CalcCheckSum8Bit(&buf[1],Excepted_Rece_Bytes - 3);
	}
	else if ((Excepted_Rece_Bytes > 5) && (buf[0] == 0x68) && (buf[3] == 0x68))
	{
		sum = CalcCheckSum8Bit(&buf[4],Excepted_Rece_Bytes - 6);
	}

	if ((buf[Excepted_Rece_Bytes - 1] == 0x16) && (buf[Excepted_Rece_Bytes - 2] == sum))
	{
		return 0;
	}

	return -1;
}

// 解析报文体
int CH103::ParseFrameBody(byte * buf)
{
	int ret = 0;
	byte funcType = buf[0];

	/*
	int addr = getAddrByRecvFrame(buf);
	if (addr < 0)
	{
		return addr;
	}
	*/

	/*
	curRTU = getRTUByAddr(addr);
	if (curRTU == NULL)
	{
		return -1;
	}
	*/

	/*
	if (bRepeatLastFrame && lastRTU!=NULL)
	{
		if (lastRTU->getRTUAddr() == addr)
		{
			ClearLastFrameInfo();
			SetEvent(hProRecvEvent);
		}
	}
	*/

	//curRTU->setLostConnectTimes(0);
	//curRTU->setCommActive(true);

	switch (funcType)
	{
	case 0x10:
		ret = ParseShortFrame(buf);
		if (ret)
		{
			return ret;
		}
		break;

	case 0x68:
		ret = ParseLongFrame(buf);
		if (ret)
		{
			return ret;
		}
		break;

	default:
		break;
	}
	return ret;
}

int CH103::ParseLongFrame(byte * buf)
{
	byte ACD = buf[4] & 0x20;
	if (ACD == 0x20)
	{
		AddSendCmd(CALL_PRIMARY_DATA,3,-1,curDPR);
	}

	UINT type_id = BufToVal(&buf[FrameTypeLocation],FrameTypeLength);
	UINT TransReason = BufToVal(&buf[TransReasonLocation],TransReasonLength);

	try
	{
		switch (type_id)
		{
		case 0x0f:
			ParseFaultFileMenu(buf);
			break;

		case 0x0d:
			ParseDataFile(buf);
			break;

		case 0x14:
			ParseTrigRecordCon(buf);
			break;

		case 0x05:
			ParseCallTimeCon(buf);
			break;

		case 0x06:
			ParseSetTimeCon(buf);
			break;

		case 0xcc:
			ParseStationInfo(buf);
			break;

		case 0xE6:
			ParseStartSettingCon(buf);
			break;

		case 0xE7:
			ParseSendSettingCon(buf);
			break;

		case 0xE8:
			ParseEndSettingCon(buf);
			break;

		default:
			throw FRAME_TYPE_ERR;
			break;
		}
	}

	catch(int err)
	{
		return err;
	}

	return 0;
}

int CH103::ParseShortFrame(byte * buf)
{
	byte ACD = buf[1] & 0x20;
	if (ACD == 0x20)
	{
		AddSendCmd(CALL_PRIMARY_DATA,3,-1,curDPR);
	}

	return 0;
}

/*
int CH103::ParseTimeCon(byte * buf)
{
	//int info_num = buf[7] & 0x7f;
	//int info_ISQ = buf[7] & 0x80;
	int info_num = BufToVal(&buf[InfoNumLocation],InfoNumLength) & (~(0x80<<(InfoNumLength - 1) * 8));
	int info_ISQ = (BufToVal(&buf[InfoNumLocation],InfoNumLength) >> ((InfoNumLength - 1) * 8)) & 0x80;

	if (info_num == 0)
	{
		return INFO_NUM_ERR;
	}

	SYSTEMTIME recvTime;
	GetLocalTime(&recvTime);
	WORD millisecond = BufToVal(&buf[DataLocation],2);
	recvTime.wMilliseconds = millisecond % 1000;
	recvTime.wSecond = millisecond / 1000;
	recvTime.wMinute = buf[DataLocation + 2] & 0x3f;
	recvTime.wHour = buf[DataLocation + 3] & 0x1f;
	recvTime.wDay = buf[DataLocation + 4] & 0x1f;
	recvTime.wMonth = buf[DataLocation + 5] & 0x0f;
	recvTime.wYear = buf[DataLocation + 6] & 0x7f;

	return 0;
}
*/

int CH103::ParseStationInfo(byte * buf)
{
	//int info_num = BufToVal(&buf[InfoNumLocation],InfoNumLength) & (~(0x80<<(InfoNumLength - 1) * 8));
	//int info_ISQ = (BufToVal(&buf[InfoNumLocation],InfoNumLength) >> ((InfoNumLength - 1) * 8)) & 0x80;

	byte devNum = buf[DataLocation + 1];

	if (devNum > 0 && devNum < MAX_DEV_SUM)
	{
		curDPR->vDevs.clear();

		for (int i=1;i<=devNum;i++)
		{
			CDPRDev dev;
			dev.setDevAddr(i);
			dev.setDevID(i * 1000);

			curDPR->vDevs.push_back(dev);
		}
	}

	PostMessage(curDPR->MainFrame_hWnd,REFRESH_STATION_VIEW,NULL,NULL);
	PostMessage(curDPR->MainFrame_hWnd,REFRESH_PROPERTY_VIEW,curDPR->getMyIndexOfStations(),NULL);

	return 0;
}

int CH103::ParseFaultFileMenu(byte * buf)
{
	byte ACD = buf[4] & 0x20;

	int devIndex = 0;
	//int info_num = BufToVal(&buf[InfoNumLocation],InfoNumLength) & (~(0x80<<(InfoNumLength - 1) * 8));
	//int info_ISQ = (BufToVal(&buf[InfoNumLocation],InfoNumLength) >> ((InfoNumLength - 1) * 8)) & 0x80;

	int info_addr = BufToVal(&buf[InfoAddrLocation],InfoAddrLength);
	int fileSum = info_addr;

	SYSTEMTIME startTime;
	GetLocalTime(&startTime);
	WORD millisecond = BufToVal(&buf[DataLocation],2);
	startTime.wMilliseconds = millisecond % 1000;
	startTime.wSecond = millisecond / 1000;
	startTime.wMinute = buf[DataLocation + 2] & 0x3f;
	startTime.wHour = buf[DataLocation + 3] & 0x1f;
	startTime.wDay = buf[DataLocation + 4] & 0x1f;
	startTime.wMonth = buf[DataLocation + 5] & 0x0f;
	startTime.wYear = buf[DataLocation + 6] & 0x7f;

	SYSTEMTIME endTime;
	GetLocalTime(&endTime);
	millisecond = BufToVal(&buf[DataLocation + 7],2);
	endTime.wMilliseconds = millisecond % 1000;
	endTime.wSecond = millisecond / 1000;
	endTime.wMinute = buf[DataLocation + 9] & 0x3f;
	endTime.wHour = buf[DataLocation + 10] & 0x1f;
	endTime.wDay = buf[DataLocation + 11] & 0x1f;
	endTime.wMonth = buf[DataLocation + 12] & 0x0f;
	endTime.wYear = buf[DataLocation + 13] & 0x7f;

	int devAddrlength = 1;
	int fileTimeLength = 7;
	int fileNameLength = 60;
	int TrigReasonLength = 50;
	int InfoDataLength = devAddrlength + fileTimeLength + fileNameLength + TrigReasonLength;

	char chName[MAX_NAME_LENGTH];

	for (int i=0;i<fileSum;i++)
	{
		WORD addr = buf[DataLocation + 14 + i*InfoDataLength];
		devIndex = (curDPR->vDevs).getDevIndexByAddr(addr);
		if (devIndex >= 0 && devIndex < curDPR->vDevs.size())
		{
			CFaultFile file(&(curDPR->vDevs[devIndex].faultfiles));

			//故障文件名
			//buf[DataLocation + 15 + 1 + i*InfoDataLength] = '/0';
			//fileNameLength = strlen((const char *)buf[DataLocation + 15 + i*InfoDataLength]);
			memcpy(chName,&buf[DataLocation + 15 + i*InfoDataLength],MAX_NAME_LENGTH);
			CString name(chName);
			file.setFaultFileName(name.GetBuffer());
			name.ReleaseBuffer();

			//故障文件时间
			SYSTEMTIME fileTime;
			GetLocalTime(&fileTime);
			millisecond = BufToVal(&buf[DataLocation + 75 + i*InfoDataLength],2);
			fileTime.wMilliseconds = millisecond % 1000;
			fileTime.wSecond = millisecond / 1000;
			fileTime.wMinute = buf[DataLocation + 77 + i*InfoDataLength] & 0x3f;
			fileTime.wHour = buf[DataLocation + 78 + i*InfoDataLength] & 0x1f;
			fileTime.wDay = buf[DataLocation + 79 + i*InfoDataLength] & 0x1f;
			fileTime.wMonth = buf[DataLocation + 80 + i*InfoDataLength] & 0x0f;
			fileTime.wYear = (buf[DataLocation + 81 + i*InfoDataLength] & 0x7f) + 2000;
			file.setFileTime(fileTime);

			//故障启动原因
			//TrigReasonLength = strlen((char *)buf[DataLocation + 72 + i*InfoDataLength]);
			//WCHAR * reason = ChartoWCHAR((char *)buf[DataLocation + 72 + i*InfoDataLength]);
			memcpy(chName,&buf[DataLocation + 82 + i*InfoDataLength],MAX_NAME_LENGTH);
			CString reason(chName);
			file.setTrigReason(reason.GetBuffer());
			reason.ReleaseBuffer();

			curDPR->vDevs[devIndex].faultfiles.push_back(file);
		}
	}

	if (ACD == 0 && devIndex >= 0 && devIndex < curDPR->vDevs.size())
	{
		curDPR->vDevs[devIndex].faultfiles.SaveFaultMenu(curDPR->getMyIndexOfStations(),devIndex);
		PostMessage(curDPR->MainFrame_hWnd,FAULT_MENU_MSG,curDPR->getMyIndexOfStations(),devIndex);
	}

	return 0;
}

int CH103::ParseDataFile(byte * buf)
{
	byte ACD = buf[4] & 0x20;

	int devIndex = 0;
	WORD addr = BufToVal(&buf[AsduAddrLocation],AsduAddrLength);
	devIndex = (curDPR->vDevs).getDevIndexByAddr(addr);
	if (devIndex < 0 || devIndex >= curDPR->vDevs.size())
	{
		return -1;
	}

	int frame_Length = buf[1] + (buf[2] & 0x0f) * 0x100;
	//int data_Length = frame_Length - 6;
	int file_Length = frame_Length - 10;

	int info_num = BufToVal(&buf[InfoNumLocation],InfoNumLength) & (~(0x80<<(InfoNumLength - 1) * 8));
	int info_ISQ = (BufToVal(&buf[InfoNumLocation],InfoNumLength) >> ((InfoNumLength - 1) * 8)) & 0x80;

	UINT info_addr = BufToVal(&buf[InfoAddrLocation],4);

	if (file_Length < 0)
	{
		return -1;
	}

	if (theApp.g_sDataBases.getFileLength() == 0)
	{
		byte * ptr = &buf[InfoAddrLocation + 4 + MAX_NAME_LENGTH + 1];
		DWORD sum = ptr[0] + ptr[1] * 0x100 + ptr[2] * 0x10000 + ptr[3] * 0x1000000;/* + 2000*/
		PostMessage(curDPR->MainFrame_hWnd,WM_PROGESS_START,0,sum);
	}

	memcpy(theApp.g_sDataBases.fileBuf + info_addr,&buf[InfoAddrLocation + 4],file_Length);
	PostMessage(curDPR->MainFrame_hWnd,WM_PROGESS_STEP,theApp.g_sDataBases.getFileLength(),file_Length);
	theApp.g_sDataBases.setFileLength(theApp.g_sDataBases.getFileLength() + file_Length);

	if (ACD == 0)
	{
		theApp.g_sDataBases.SaveBufToFile(curDPR->getMyIndexOfStations(),devIndex,0);
		PostMessage(curDPR->MainFrame_hWnd,WM_PROGESS_END,NULL,NULL);
	}
	
	return 0;
}

/*
int CH103::ParseEndInit(byte * buf)
{
	return 0;
}
*/

int CH103::ParseTrigRecordCon(byte * buf)
{
	int devIndex = 0;
	WORD addr = BufToVal(&buf[AsduAddrLocation],AsduAddrLength);
	devIndex = (curDPR->vDevs).getDevIndexByAddr(addr);
	if (devIndex < 0 || devIndex >= curDPR->vDevs.size())
	{
		return -1;
	}

	byte val = BufToVal(&buf[DataLocation],1);

	if (val == 2)
	{
		PostMessage(curDPR->MainFrame_hWnd,TRIG_RECORD_ACTIVE,curDPR->getMyIndexOfStations(),devIndex);
	}
	else
	{
		PostMessage(curDPR->MainFrame_hWnd,TRIG_RECORD_NEGATIVE,curDPR->getMyIndexOfStations(),devIndex);
	}

	return 0;
}

int CH103::ParseSetTimeCon(byte * buf)
{
	int devIndex = 0;
	WORD addr = BufToVal(&buf[AsduAddrLocation],AsduAddrLength);
	devIndex = (curDPR->vDevs).getDevIndexByAddr(addr);
	if (devIndex < 0 || devIndex >= curDPR->vDevs.size())
	{
		return -1;
	}

	byte val = BufToVal(&buf[DataLocation],1);

	if (val == 2)
	{
		PostMessage(curDPR->MainFrame_hWnd,SET_TIME_ACTIVE,curDPR->getMyIndexOfStations(),devIndex);
	}
	else
	{
		PostMessage(curDPR->MainFrame_hWnd,SET_TIME_NEGATIVE,curDPR->getMyIndexOfStations(),devIndex);
	}

	return 0;
}

int CH103::ParseCallTimeCon(byte * buf)
{
	int devIndex = 0;
	WORD addr = BufToVal(&buf[AsduAddrLocation],AsduAddrLength);
	devIndex = (curDPR->vDevs).getDevIndexByAddr(addr);
	if (devIndex < 0 || devIndex >= curDPR->vDevs.size())
	{
		return -1;
	}

	byte val = BufToVal(&buf[DataLocation],1);

	if (val == 2)
	{
		SYSTEMTIME recvTime;
		GetLocalTime(&recvTime);
		WORD millisecond = BufToVal(&buf[DataLocation + 1],2);
		recvTime.wMilliseconds = millisecond % 1000;
		recvTime.wSecond = millisecond / 1000;
		recvTime.wMinute = buf[DataLocation + 3] & 0x3f;
		recvTime.wHour = buf[DataLocation + 4] & 0x1f;
		recvTime.wDay = buf[DataLocation + 5] & 0x1f;
		recvTime.wMonth = buf[DataLocation + 6] & 0x0f;
		recvTime.wYear = (buf[DataLocation + 7] & 0x7f) + 2000;

		curDPR->vDevs[devIndex].setDevTime(recvTime);

		PostMessage(curDPR->MainFrame_hWnd,CALL_TIME_ACTIVE,curDPR->getMyIndexOfStations(),devIndex);
	}
	else
	{
		PostMessage(curDPR->MainFrame_hWnd,CALL_TIME_NEGATINE,curDPR->getMyIndexOfStations(),devIndex);
	}

	return 0;
}

int CH103::ParseStartSettingCon(byte * buf)
{
	WORD devAddr = BufToVal(&buf[AsduAddrLocation],AsduAddrLength);
	int devIndex = (curDPR->vDevs).getDevIndexByAddr(devAddr);
	if(devIndex < 0 || devIndex >= curDPR->vDevs.size() || devIndex != fileTransInfo.transDevIndex)
	{
		throw DEV_INDEX_ERR;
	}

	byte RII = buf[DataLocation];
	byte fileType = buf[DataLocation + 1];
	UINT fileID = BufToVal(&buf[DataLocation + 2],4);
	UINT filePos = BufToVal(&buf[DataLocation + 6],4);
	byte RepCode = buf[DataLocation + 10];

	if ((RII == fileTransInfo.RII) && (fileType == fileTransInfo.fileType) && (fileID == fileTransInfo.fileID) && (filePos == fileTransInfo.filePos))
	{
		if (RepCode == 0)
		{
			fileTransInfo.RII++;
			//fileTransInfo.fileID++;
			fileTransInfo.filePos = 0;
			
			stFaultFileInfo * ffiPtr = new stFaultFileInfo(curDPR->getMyIndexOfStations(),devIndex);
			AddSendCmd(SEND_DEV_SETTING,1,(DWORD_PTR)ffiPtr,curDPR);

			PostMessage(curDPR->MainFrame_hWnd,WM_PROGESS_START,0,theApp.g_sDataBases.getFileLength());
		}
	}
	
	return 0;
}

int CH103::ParseSendSettingCon(byte * buf)
{
	WORD devAddr = BufToVal(&buf[AsduAddrLocation],AsduAddrLength);
	int devIndex = (curDPR->vDevs).getDevIndexByAddr(devAddr);
	if(devIndex < 0 || devIndex >= curDPR->vDevs.size() || devIndex != fileTransInfo.transDevIndex)
	{
		throw DEV_INDEX_ERR;
	}

	byte RII = buf[DataLocation];
	byte fileType = buf[DataLocation + 1];
	UINT fileID = BufToVal(&buf[DataLocation + 2],4);
	UINT filePos = BufToVal(&buf[DataLocation + 6],4);
	byte RepCode = buf[DataLocation + 10];

	if ((RII == fileTransInfo.RII) && (fileType == fileTransInfo.fileType) && (fileID == fileTransInfo.fileID) && (filePos == fileTransInfo.filePos))
	{
		if (RepCode == 0)
		{
			fileTransInfo.RII++;
			//fileTransInfo.fileID++;

			stFaultFileInfo * ffiPtr = new stFaultFileInfo(curDPR->getMyIndexOfStations(),devIndex);

			if (theApp.g_sDataBases.getFileLength() - fileTransInfo.filePos > 0)
			{
				AddSendCmd(SEND_DEV_SETTING,1,(DWORD_PTR)ffiPtr,curDPR);
			}
			else
			{
				AddSendCmd(END_DEV_SETTING,1,(DWORD_PTR)ffiPtr,curDPR);
			}
			
		}
		else
		{
			PostMessage(curDPR->MainFrame_hWnd,WM_PROGESS_END,NULL,NULL);
		}
	}
	else
	{
		PostMessage(curDPR->MainFrame_hWnd,WM_PROGESS_END,NULL,NULL);
	}

	return 0;
}

int CH103::ParseEndSettingCon(byte * buf)
{
	WORD devAddr = BufToVal(&buf[AsduAddrLocation],AsduAddrLength);
	int devIndex = (curDPR->vDevs).getDevIndexByAddr(devAddr);
	if(devIndex < 0 || devIndex >= curDPR->vDevs.size() || devIndex != fileTransInfo.transDevIndex)
	{
		throw DEV_INDEX_ERR;
	}

	byte RII = buf[DataLocation];
	byte fileType = buf[DataLocation + 1];
	UINT fileID = BufToVal(&buf[DataLocation + 2],4);
	UINT filePos = BufToVal(&buf[DataLocation + 6],4);
	byte RepCode = buf[DataLocation + 10];

	PostMessage(curDPR->MainFrame_hWnd,WM_PROGESS_END,NULL,NULL);

	if ((RII == fileTransInfo.RII) && (fileType == fileTransInfo.fileType) && (fileID == fileTransInfo.fileID) /*&& (filePos == fileTransInfo.filePos)*/)
	{
		if (RepCode == 1)
		{
			PostMessage(curDPR->MainFrame_hWnd,SET_SETTING_ACTIVE,curDPR->getMyIndexOfStations(),devIndex);
		}
		else
		{
			PostMessage(curDPR->MainFrame_hWnd,SET_SETTING_NEGATIVE,curDPR->getMyIndexOfStations(),devIndex);
		}
	}

	return 0;
}

int CH103::AssemblePrimaryData(byte * buf)
{
	byte count = 0;

	buf[count++] = 0x10;
	buf[count++] = (0x5a + curDPR->getFCB()) | DIR_PRM;
	//buf[count++] = (byte)thisRTU->getRTUAddr();
	count += ValToBuf(&buf[count],curDPR->getStationAddr(),AsduAddrLength);
	buf[count++] = CalcCheckSum8Bit(&buf[1],count - 1);
	buf[count++] = 0x16;

	return count;
}

/*
int CH103::AssembleSetTime(byte * buf,SYSTEMTIME time)
{
	int count = 0;

	buf[count++] = 0x68;
	buf[count++] = 0;
	buf[count++] = 0;
	buf[count++] = 0x68;
	count += ValToBuf(&buf[count],0x67,FrameTypeLength);
	count += ValToBuf(&buf[count],0x01,InfoNumLength);
	count += ValToBuf(&buf[count],0x06,TransReasonLength);
	count += ValToBuf(&buf[count],curDPR->getStationID(),AsduAddrLength);
	count += ValToBuf(&buf[count],0x00,InfoAddrLength);
	buf[count++] = (time.wMilliseconds + time.wSecond * 1000) % 0x100;
	buf[count++] = (time.wMilliseconds + time.wSecond * 1000) / 0x100;
	buf[count++] = time.wMinute & 0x3f;
	buf[count++] = time.wHour & 0x1f;
	buf[count++] = time.wDay & 0x1f;
	buf[count++] = time.wMonth & 0x0f;
	buf[count++] = time.wYear % 100;

	buf[1] = (count - 6) % 0x100;
	buf[2] = ((count - 6) / 0x100) & 0x0f;

	return count;
}
*/

int CH103::AssembleCallStationInfo(byte * buf,UINT stationIndex)
{
	int count = 0;

	buf[count++] = 0x68;
	buf[count++] = 0;
	buf[count++] = 0;
	buf[count++] = 0x68;
	buf[count++] = 0x53 + curDPR->getFCB() | DIR_PRM;
	count += ValToBuf(&buf[count],curDPR->getStationAddr(),AsduAddrLength);
	count += ValToBuf(&buf[count],0xcc,FrameTypeLength);
	count += ValToBuf(&buf[count],0x81,InfoNumLength);
	count += ValToBuf(&buf[count],0x09,TransReasonLength);
	count += ValToBuf(&buf[count],0,AsduAddrLength);
	buf[count++] = 0xff;
	buf[count++] = 0;
	buf[count++] = CalcCheckSum8Bit(&buf[4],count - 4);
	buf[count++] = 0x16;

	buf[1] = (count - 6) % 0x100;
	buf[2] = ((count - 6) / 0x100) & 0x0f;

	return count;
}

int CH103::AssembleCallFaultFileMenu(byte * buf,UINT stationIndex,UINT devIndex)
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	int count = 0;

	buf[count++] = 0x68;
	buf[count++] = 0;
	buf[count++] = 0;
	buf[count++] = 0x68;
	buf[count++] = 0x53 + curDPR->getFCB() | DIR_PRM;
	count += ValToBuf(&buf[count],curDPR->getStationAddr(),AsduAddrLength);
	count += ValToBuf(&buf[count],0x0f,FrameTypeLength);
	count += ValToBuf(&buf[count],0x81,InfoNumLength);
	count += ValToBuf(&buf[count],0,TransReasonLength);
	count += ValToBuf(&buf[count],curDPR->vDevs[devIndex].getDevAddr(),AsduAddrLength);
	buf[count++] = 0xff;
	buf[count++] = devIndex;
	buf[count++] = (time.wMilliseconds + time.wSecond * 1000) % 0x100;
	buf[count++] = (time.wMilliseconds + time.wSecond * 1000) / 0x100;
	buf[count++] = time.wMinute & 0x3f;
	buf[count++] = time.wHour & 0x1f;
	buf[count++] = time.wDay & 0x1f;
	buf[count++] = time.wMonth & 0x0f;
	buf[count++] = time.wYear % 100;
	buf[count++] = (time.wMilliseconds + time.wSecond * 1000) % 0x100;
	buf[count++] = (time.wMilliseconds + time.wSecond * 1000) / 0x100;
	buf[count++] = time.wMinute & 0x3f;
	buf[count++] = time.wHour & 0x1f;
	buf[count++] = time.wDay & 0x1f;
	buf[count++] = time.wMonth & 0x0f;
	buf[count++] = time.wYear % 100;
	buf[count++] = CalcCheckSum8Bit(&buf[4],count - 4);
	buf[count++] = 0x16;

	buf[1] = (count - 6) % 0x100;
	buf[2] = ((count - 6) / 0x100) & 0x0f;

	return count;
}

int CH103::AssembleCallFaultFile(byte * buf,UINT stationIndex,UINT devIndex, UINT fileIndex)
{
	int count = 0;

	buf[count++]=0x68;
	buf[count++]=0;
	buf[count++]=0;	
	buf[count++]=0x68;
	buf[count++]=0x53 + curDPR->getFCB() | DIR_PRM;
	count += ValToBuf(&buf[count],curDPR->getStationAddr(),AsduAddrLength);
	count += ValToBuf(&buf[count],0x0d,FrameTypeLength);
	count += ValToBuf(&buf[count],0x81,InfoNumLength);
	count += ValToBuf(&buf[count],0,TransReasonLength);
	count += ValToBuf(&buf[count],curDPR->vDevs[devIndex].getDevAddr(),AsduAddrLength);
	buf[count++]=0x03;
	buf[count++]=fileIndex;

	char * fileName = WCHARtoChar(theApp.g_sDataBases[stationIndex].vDevs[devIndex].faultfiles[fileIndex].getFaultFileName());
	int length = strlen(fileName);
	memset(&buf[count],0,MAX_NAME_LENGTH);
	memcpy(&buf[count],fileName,length);
	delete fileName;
	count += MAX_NAME_LENGTH;

	buf[count++]=0x00;
	buf[count++]=0x00;
	buf[count++]=0x00;
	buf[count++]=0x00;

	buf[count++] = CalcCheckSum8Bit(&buf[4],count - 4);
	buf[count++]=0x16;

	buf[1] = (count - 6) % 0x100;
	buf[2] = ((count - 6) / 0x100) & 0x0f;

	return count;
}

int CH103::AssembleCallDevSetting(byte * buf,UINT stationIndex,UINT devIndex)
{
	int count = 0;

	buf[count++]=0x68;
	buf[count++]=0;
	buf[count++]=0;	
	buf[count++]=0x68;
	buf[count++]=0x53 + curDPR->getFCB() | DIR_PRM;
	count += ValToBuf(&buf[count],curDPR->getStationAddr(),AsduAddrLength);
	count += ValToBuf(&buf[count],0x0d,FrameTypeLength);
	count += ValToBuf(&buf[count],0x81,InfoNumLength);
	count += ValToBuf(&buf[count],0,TransReasonLength);
	count += ValToBuf(&buf[count],curDPR->vDevs[devIndex].getDevAddr(),AsduAddrLength);
	buf[count++]=0x01;
	buf[count++]=devIndex;

	string settingName = STR_SETCFG_NAME;
	memset(&buf[count],0,MAX_NAME_LENGTH);
	memcpy(&buf[count],settingName.c_str(),settingName.length());
	count += MAX_NAME_LENGTH;

	buf[count++]=0;
	buf[count++]=0;
	buf[count++]=0;
	buf[count++]=0;

	buf[count++] = CalcCheckSum8Bit(&buf[4],count - 4);
	buf[count++]=0x16;

	buf[1] = (count - 6) % 0x100;
	buf[2] = ((count - 6) / 0x100) & 0x0f;

	return count;
}

int CH103::AssembleStartDevSetting(byte * buf,UINT stationIndex,UINT devIndex)
{
	int count = 0;

	buf[count++]=0x68;
	buf[count++]=0;
	buf[count++]=0;	
	buf[count++]=0x68;
	buf[count++]=0x53 + curDPR->getFCB() | DIR_PRM;
	count += ValToBuf(&buf[count],curDPR->getStationAddr(),AsduAddrLength);
	count += ValToBuf(&buf[count],0xE6,FrameTypeLength);
	count += ValToBuf(&buf[count],0x81,InfoNumLength);
	count += ValToBuf(&buf[count],0x00,TransReasonLength);
	count += ValToBuf(&buf[count],curDPR->vDevs[devIndex].getDevAddr(),AsduAddrLength);
	buf[count++]=0xff;
	buf[count++]=devIndex;
	buf[count++]=fileTransInfo.RII;
	buf[count++]=fileTransInfo.fileType;
	count += ValToBuf(&buf[count],fileTransInfo.fileID,4);
	count += ValToBuf(&buf[count],theApp.g_sDataBases.getFileLength(),4);
	
	memset(&buf[count],0,88);
	count += 88;

	string settingName = STR_SETCFG_NAME;
	memset(&buf[count],0,MAX_NAME_LENGTH);
	memcpy(&buf[count],settingName.c_str(),settingName.length());
	count += MAX_NAME_LENGTH;

	buf[count++] = 0;

	buf[count++] = CalcCheckSum8Bit(&buf[4],count - 4);
	buf[count++]=0x16;

	buf[1] = (count - 6) % 0x100;
	buf[2] = ((count - 6) / 0x100) & 0x0f;

	return count;
}

int CH103::AssembleSendDevSetting(byte * buf,UINT stationIndex,UINT devIndex)
{
	int count = 0;
	int DataLength = 0;
	if (theApp.g_sDataBases.getFileLength() - fileTransInfo.filePos > 1000)
	{
		DataLength = 1000;
	}
	else
	{
		DataLength = theApp.g_sDataBases.getFileLength() - fileTransInfo.filePos;
	}

	PostMessage(curDPR->MainFrame_hWnd,WM_PROGESS_STEP,fileTransInfo.filePos,DataLength);

	buf[count++]=0x68;
	buf[count++]=0;
	buf[count++]=0;	
	buf[count++]=0x68;
	buf[count++]=0x53 + curDPR->getFCB() | DIR_PRM;
	count += ValToBuf(&buf[count],curDPR->getStationAddr(),AsduAddrLength);
	count += ValToBuf(&buf[count],0xE7,FrameTypeLength);
	count += ValToBuf(&buf[count],0x81,InfoNumLength);
	count += ValToBuf(&buf[count],0x00,TransReasonLength);
	count += ValToBuf(&buf[count],curDPR->vDevs[devIndex].getDevAddr(),AsduAddrLength);
	buf[count++]=0xff;
	buf[count++]=devIndex;
	buf[count++]=fileTransInfo.RII;
	buf[count++]=0;
	count += ValToBuf(&buf[count],fileTransInfo.fileID,4);
	count += ValToBuf(&buf[count],fileTransInfo.filePos,4);
	count += ValToBuf(&buf[count],DataLength,2);
	memcpy(&buf[count],&theApp.g_sDataBases.fileBuf[fileTransInfo.filePos],DataLength);
	fileTransInfo.filePos += DataLength;
	count += DataLength;

	buf[count++] = CalcCheckSum8Bit(&buf[4],count - 4);
	buf[count++]=0x16;

	buf[1] = (count - 6) % 0x100;
	buf[2] = ((count - 6) / 0x100) & 0x0f;

	return count;
}

int CH103::AssembleEndDevSetting(byte * buf,UINT stationIndex,UINT devIndex)
{
	int count = 0;

	buf[count++]=0x68;
	buf[count++]=0;
	buf[count++]=0;	
	buf[count++]=0x68;
	buf[count++]=0x53 + curDPR->getFCB() | DIR_PRM;
	count += ValToBuf(&buf[count],curDPR->getStationAddr(),AsduAddrLength);
	count += ValToBuf(&buf[count],0xE8,FrameTypeLength);
	count += ValToBuf(&buf[count],0x81,InfoNumLength);
	count += ValToBuf(&buf[count],0x00,TransReasonLength);
	count += ValToBuf(&buf[count],curDPR->vDevs[devIndex].getDevAddr(),AsduAddrLength);
	buf[count++]=0xff;
	buf[count++]=devIndex;
	buf[count++]=fileTransInfo.RII;
	buf[count++]=0;
	count += ValToBuf(&buf[count],fileTransInfo.fileID,4);
	count += ValToBuf(&buf[count],0,4);
	count += ValToBuf(&buf[count],3,2);

	buf[count++] = CalcCheckSum8Bit(&buf[4],count - 4);
	buf[count++]=0x16;

	buf[1] = (count - 6) % 0x100;
	buf[2] = ((count - 6) / 0x100) & 0x0f;

	return count;
}


int CH103::AssembleSendTrigRecord(byte * buf,UINT stationIndex,UINT devIndex)
{
	int count = 0;

	buf[count++]=0x68;
	buf[count++]=0;
	buf[count++]=0;	
	buf[count++]=0x68;
	buf[count++]=0x53 + curDPR->getFCB() | DIR_PRM;
	count += ValToBuf(&buf[count],curDPR->getStationAddr(),AsduAddrLength);
	count += ValToBuf(&buf[count],0x14,FrameTypeLength);
	count += ValToBuf(&buf[count],0x81,InfoNumLength);
	count += ValToBuf(&buf[count],0x14,TransReasonLength);
	count += ValToBuf(&buf[count],curDPR->vDevs[devIndex].getDevAddr(),AsduAddrLength);
	buf[count++]=0xff;
	buf[count++]=devIndex;
	buf[count++]=0x10;
	buf[count++]=0;

	buf[count++] = CalcCheckSum8Bit(&buf[4],count - 4);
	buf[count++]=0x16;

	buf[1] = (count - 6) % 0x100;
	buf[2] = ((count - 6) / 0x100) & 0x0f;

	return count;
}

int CH103::AssembleCallDevTime(byte * buf,UINT stationIndex,UINT devIndex)
{
	int count = 0;

	buf[count++]=0x68;
	buf[count++]=0;
	buf[count++]=0;	
	buf[count++]=0x68;
	buf[count++]=0x53 + curDPR->getFCB() | DIR_PRM;
	count += ValToBuf(&buf[count],curDPR->getStationAddr(),AsduAddrLength);
	count += ValToBuf(&buf[count],0x05,FrameTypeLength);
	count += ValToBuf(&buf[count],0x81,InfoNumLength);
	count += ValToBuf(&buf[count],0x14,TransReasonLength);
	count += ValToBuf(&buf[count],curDPR->vDevs[devIndex].getDevAddr(),AsduAddrLength);
	buf[count++]=0xff;
	buf[count++]=devIndex;
	buf[count++]=0x10;
	buf[count++]=0;

	buf[count++] = CalcCheckSum8Bit(&buf[4],count - 4);
	buf[count++]=0x16;

	buf[1] = (count - 6) % 0x100;
	buf[2] = ((count - 6) / 0x100) & 0x0f;

	return count;
}

int CH103::AssembleSetDevTime( byte * buf,UINT stationIndex,UINT devIndex,SYSTEMTIME time)
{
	int count = 0;

	buf[count++]=0x68;
	buf[count++]=0;
	buf[count++]=0;	
	buf[count++]=0x68;
	buf[count++]=0x53 + curDPR->getFCB() | DIR_PRM;
	count += ValToBuf(&buf[count],curDPR->getStationAddr(),AsduAddrLength);
	count += ValToBuf(&buf[count],0x06,FrameTypeLength);
	count += ValToBuf(&buf[count],0x81,InfoNumLength);
	count += ValToBuf(&buf[count],0x14,TransReasonLength);
	count += ValToBuf(&buf[count],curDPR->vDevs[devIndex].getDevAddr(),AsduAddrLength);
	buf[count++]=0xff;
	buf[count++]=devIndex;
	buf[count++] = (time.wMilliseconds + time.wSecond * 1000) % 0x100;
	buf[count++] = (time.wMilliseconds + time.wSecond * 1000) / 0x100;
	buf[count++] = time.wMinute & 0x3f;
	buf[count++] = time.wHour & 0x1f;
	buf[count++] = time.wDay & 0x1f;
	buf[count++] = time.wMonth & 0x0f;
	buf[count++] = time.wYear % 100;
	
	buf[count++] = CalcCheckSum8Bit(&buf[4],count - 4);
	buf[count++]=0x16;

	buf[1] = (count - 6) % 0x100;
	buf[2] = ((count - 6) / 0x100) & 0x0f;

	return count;
}

int CH103::SendProcess(stSendCmd cmd)
{
	//UINT AllParaByteSum = 0;
	//UINT lastParabyteSum = 0;
	//byte ParaFrameSum = 0;
	//WORD ParaCheckSum = 0;
	//WORD ParaAntiCheckSum = 0;

	if (curDPR == NULL)
	{
		return PROTOCOL_NO_DATABASE_ERR;
	}

	if (!curDPR->getCommActive())
	{
		return PROTOCOL_COMM_ERR;
	}

	unsigned long count = 0;
	int iStationIndex;
	int iDevIndex;
	//int yk_no = -1;
	switch (cmd.cmdType)
	{
		case CALL_PRIMARY_DATA:
			count = AssemblePrimaryData(Sendbuf);
			break;

		case SET_TIME:
			iStationIndex = ((stFaultFileInfo *)cmd.pParam)->stationIndex;
			iDevIndex = ((stFaultFileInfo *)cmd.pParam)->devIndex;
			SYSTEMTIME timeVal;
			timeVal = theApp.g_sDataBases[iStationIndex].vDevs[iDevIndex].getDevTime();
			count = AssembleSetDevTime(Sendbuf,iStationIndex,iDevIndex,timeVal);
			delete (stFaultFileInfo *)cmd.pParam;
			break;

		case CALL_TIME:
			count = AssembleCallDevTime(Sendbuf,((stFaultFileInfo *)cmd.pParam)->stationIndex,((stFaultFileInfo *)cmd.pParam)->devIndex);
			delete (stFaultFileInfo *)cmd.pParam;
			break;

		case TRIG_DEV_RECORD:
			count = AssembleSendTrigRecord(Sendbuf,((stFaultFileInfo *)cmd.pParam)->stationIndex,((stFaultFileInfo *)cmd.pParam)->devIndex);
			delete (stFaultFileInfo *)cmd.pParam;
			break;

		case CALL_FFI_MENU:
			count = AssembleCallFaultFileMenu(Sendbuf,((stFaultFileInfo *)cmd.pParam)->stationIndex,((stFaultFileInfo *)cmd.pParam)->devIndex);
			delete (stFaultFileInfo *)cmd.pParam;
			break;

		case CALL_FFI_FILE:
			count = AssembleCallFaultFile(Sendbuf,((stFaultFileInfo *)cmd.pParam)->stationIndex,((stFaultFileInfo *)cmd.pParam)->devIndex,((stFaultFileInfo *)cmd.pParam)->fileIndex);
			delete (stFaultFileInfo *)cmd.pParam;
			break;

		case CALL_STATION_CFG:
			count = AssembleCallStationInfo(Sendbuf,cmd.thisDPR->getMyIndexOfStations());
			break;

		case CALL_DEV_SETTING:
			count = AssembleCallDevSetting(Sendbuf,((stFaultFileInfo *)cmd.pParam)->stationIndex,((stFaultFileInfo *)cmd.pParam)->devIndex);
			delete (stFaultFileInfo *)cmd.pParam;
			break;

		case START_DEV_SETTING:
			memset(&fileTransInfo,0,sizeof(fileTransInfo));
			fileTransInfo.transStationIndex = ((stFaultFileInfo *)cmd.pParam)->stationIndex;
			fileTransInfo.transDevIndex = ((stFaultFileInfo *)cmd.pParam)->devIndex;
			count = AssembleStartDevSetting(Sendbuf,fileTransInfo.transStationIndex,fileTransInfo.transDevIndex);
			delete (stFaultFileInfo *)cmd.pParam;
			break;

		case SEND_DEV_SETTING:
			count = AssembleSendDevSetting(Sendbuf,((stFaultFileInfo *)cmd.pParam)->stationIndex,((stFaultFileInfo *)cmd.pParam)->devIndex);
			delete (stFaultFileInfo *)cmd.pParam;
			break;

		case END_DEV_SETTING:
			count = AssembleEndDevSetting(Sendbuf,((stFaultFileInfo *)cmd.pParam)->stationIndex,((stFaultFileInfo *)cmd.pParam)->devIndex);
			delete (stFaultFileInfo *)cmd.pParam;
			break;

		default:
			break;
	}

	if (count > 0)
	{
		H103NetClient.SendMsg((char *)Sendbuf,count);
		ShowFrame(SEND_FRAME_MSG,Sendbuf,count);
		curDPR->ChangeFCB();
	}
	
	return 0;
}

// 读报文过程
int CH103::RecvProcess(void)
{
	int ret = 0;
	byte tempChar;
	byte temp_buf[2048];

	int char_left = CLoopBuf::char_num(&recvBuf);
	while (char_left > Syn_Char_Num)
	{
		ret = CheckFrameHead();
		if (!ret)
		{
			char_left = CLoopBuf::char_num(&recvBuf);
			if (char_left >= (int)Excepted_Rece_Bytes)
			{
				CLoopBuf::copyfrom_buf(temp_buf,&recvBuf,Excepted_Rece_Bytes);
				ret = CheckFrameTail(temp_buf);
				if (!ret)
				{
					CLoopBuf::get_buf(temp_buf,&recvBuf,Excepted_Rece_Bytes);
					ret = ParseFrameBody(temp_buf); //不在此处return，要把接收缓冲区的数据读完
					if (ret == DPR_ADDR_ERR)
					{
						CloseConnect();

						switch (ret)
						{
						case DPR_ADDR_ERR:
							//CLog::GetObj().tmWrite( "关闭socket，接收线程发现RTU地址错误\n" );
							H103Log.Add( "关闭socket，接收线程发现RTU地址错误\n" );
							break;

						default:
							//CLog::GetObj().tmWrite( "关闭socket，接收线程未知错误\n" );
							H103Log.Add( "关闭socket，接收线程未知错误\n" );
							break;
						}
					}
					Excepted_Rece_Bytes = 0;
				}
				else
				{
					CLoopBuf::pop_char(&tempChar,&recvBuf);
				}
			}
			else
			{
				return LESS_RECV_BYTE;
			}
		}
		char_left = CLoopBuf::char_num(&recvBuf);
	}

	return ret;
}

int CH103::ResetProtocolProc(void)
{
	CloseConnect();
	//CLog::GetObj().tmWrite( "关闭socket，系统重启socket线程\n" );
	H103Log.Add( "关闭socket，系统重启socket线程\n" );
	return 0;
}

void CALLBACK CH103::H103TimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime)
{

	CH103 * myH103 = (CH103 *)(CMyTimer::m_sTimeMap[idEvent]->TargetObj);


	if (idEvent == myH103->mytimer.iAnyFrameRecv)
	{
		myH103->CloseConnect();
		//CLog::GetObj().tmWrite( "关闭socket，超时未收到任何报文\n" );
		myH103->H103Log.Add( "关闭socket，超时未收到任何报文\n" );
		myH103->mytimer.tAnyFrameRecv.KillMyTimer();
	}

}
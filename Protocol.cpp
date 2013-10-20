#include "StdAfx.h"
#include "Protocol.h"
#include "H103.h"

extern CDPR2UIApp theApp;

CProtocol::CProtocol()
{
	bProtocolActive = false;
	bRepeatLastFrame = false;
	//bYkOperateIsDoing = false;
	Excepted_Rece_Bytes = 0;
	Syn_Char_Num = 0;
	MaxFrameDataBytes = 0;
	LastFrameLength = 0;

	FrameTypeLength = 0;
	InfoNumLength = 0;
	TransReasonLength = 0;
	AsduAddrLength = 0;
	InfoAddrLength = 0;

	SetFrameLocation(0);

	YK_NO = -1;
	//Extend_RTU_Index = -1;

	SYX_START_ADDR = 0xffff;
	DYX_START_ADDR = 0xffff;
	YC_START_ADDR = 0xffff;
	YK_START_ADDR = 0xffff;
	YM_START_ADDR = 0xffff;

	ANY_FRAME_RECV_TIMEOUT       = 0xffffffff;
	HEART_FRAME_SEND_TIMEOUT     = 0xffffffff;
	IGRAME_FRAME_RECV_TIMEOUT    = 0xffffffff;
	CALL_ALL_DATA_SEND_TIMEOUT   = 0xffffffff;
	CALL_ALL_DD_SEND_TIMEOUT     = 0xffffffff;
	SET_TIME_FRAME_SEND_TIMEOUT  = 0xffffffff;
	YK_SEL_FRAME_SEND_TIMEOUT    = 0xffffffff;
	YK_EXE_FRAME_SEND_TIMEOUT    = 0xffffffff;
	YK_CANCLE_FRAME_SEND_TIMEOUT = 0xffffffff;
	REQUIRE_LINK_SEND_TIMEOUT    = 0xffffffff;
	CALL_PARA_SEND_TIMEOUT       = 0xffffffff;
	SET_PARA_SEND_TIMEOUT        = 0xffffffff;

	memset(&recvBuf,0,sizeof(recvBuf));
	memset(&showBuf,0,sizeof(showBuf));
	memset(&Sendbuf,0,sizeof(byte));

	DWORD curTime = GetTickCount();
	//InitTimer(curTime);

	::InitializeCriticalSection(&SendCMDQueSection);
	hProRecvEvent=CreateEvent(NULL,false,false,NULL);
	hProSendEvent=CreateEvent(NULL,false,false,NULL);
	hProExitEvent=CreateEvent(NULL,false,false,NULL);

	curDPR = NULL;
	//lastRTU = NULL;
	//YK_RTU = NULL;
}

CProtocol::CProtocol(CDPRStation * myDataBase)
{
	bProtocolActive = false;
	bRepeatLastFrame = false;
	//bYkOperateIsDoing = false;
	Excepted_Rece_Bytes = 0;
	Syn_Char_Num = 0;
	MaxFrameDataBytes = 0;
	LastFrameLength = 0;

	FrameTypeLength = 0;
	InfoNumLength = 0;
	TransReasonLength = 0;
	AsduAddrLength = 0;
	InfoAddrLength = 0;
	
	SetFrameLocation(0);

	YK_NO = -1;
	//Extend_RTU_Index = -1;

	SYX_START_ADDR = 0xffff;
	DYX_START_ADDR = 0xffff;
	YC_START_ADDR = 0xffff;
	YK_START_ADDR = 0xffff;
	YM_START_ADDR = 0xffff;

	memset(&recvBuf,0,sizeof(recvBuf));
	memset(&showBuf,0,sizeof(showBuf));
	memset(&Sendbuf,0,sizeof(byte));

	DWORD curTime = GetTickCount();
	//InitTimer(curTime);

	::InitializeCriticalSection(&SendCMDQueSection);
	hProRecvEvent=CreateEvent(NULL,false,false,NULL);
	hProSendEvent=CreateEvent(NULL,false,false,NULL);
	hProExitEvent=CreateEvent(NULL,false,false,NULL);

	curDPR = myDataBase;
	//lastRTU = curDPR;
	RTUs.push_back(curDPR);
}

CProtocol::~CProtocol(void)
{
	curDPR = NULL;
	//lastRTU = NULL;
	SetEvent(hProExitEvent);    //停止报文发送线程

	::DeleteCriticalSection(&SendCMDQueSection);
	CloseHandle(hProRecvEvent);
	CloseHandle(hProSendEvent);
	CloseHandle(hProExitEvent);
}

///*
// 串口或者网口数据读入报文存储结构
int CProtocol::ReadFrame()
{
	return 0;
}

// 校验报文头
int CProtocol::CheckFrameHead(void)
{
	return 0;
}

// 校验报文尾
int CProtocol::CheckFrameTail(byte * buf)
{
	return 0;
}

// 读报文过程
int CProtocol::RecvProcess(void)
{
	int ret = 0;
	byte tempChar;
	byte temp_buf[1024];

	int char_left = CLoopBuf::char_num(&recvBuf);
	while (char_left > Syn_Char_Num)
	{
		ret = CheckFrameHead();
		if (!ret)
		{
			char_left = CLoopBuf::char_num(&recvBuf);
			if (char_left >= Excepted_Rece_Bytes)
			{
				CLoopBuf::copyfrom_buf(temp_buf,&recvBuf,Excepted_Rece_Bytes);
				ret = CheckFrameTail(temp_buf);
				if (!ret)
				{
					CLoopBuf::get_buf(temp_buf,&recvBuf,Excepted_Rece_Bytes);
					ret = ParseFrameBody(temp_buf); //不在此处return，要把接收缓冲区的数据读完
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

// 解析报文体
int CProtocol::ParseFrameBody(byte * buf)
{
	return 0;
}
//*/

bool CProtocol::getProtocolActive(void)
{
	return bProtocolActive;
}

void CProtocol::setProtocolActive(bool bVal)
{
	bProtocolActive = bVal;
}

int CProtocol::AddSendCmd(UINT cmdType, int Priority, DWORD_PTR pParam,CDPRStation * thisDPR)
{
	stSendCmd tmpCmd;
	tmpCmd.cmdType = cmdType;
	tmpCmd.Priority = Priority;
	tmpCmd.pParam = pParam;
	tmpCmd.thisDPR = thisDPR;

	//SetEvent(hProSendEvent);

	::EnterCriticalSection(&SendCMDQueSection);
	SendCommandQue.push_back(tmpCmd);
	::LeaveCriticalSection(&SendCMDQueSection);

	SetEvent(hProSendEvent);

	return 0;
}

stSendCmd CProtocol::getMaxPriopriyCmd(void)
{
	byte tmpPriority = 0;
	size_t index = 0;
	stSendCmd stRet;
	for (size_t i=0;i< SendCommandQue.size();i++)
	{
		if (SendCommandQue[i].Priority > tmpPriority)
		{
			tmpPriority = SendCommandQue[i].Priority;
			index = i;
		}
	}
	stRet = SendCommandQue[index];
	SendCommandQue.erase(SendCommandQue.begin() + index);
	return stRet;
}

int CProtocol::ClearSendCommandQue(void)
{
	SendCommandQue.clear();
	return 0;
}

HANDLE CProtocol::gethProExitEvent(void)
{
	return hProExitEvent;
}

HANDLE CProtocol::gethProSendEvent(void)
{
	return hProSendEvent;
}

int CProtocol::RecordLastFrameInfo(int dataLength, CDPRStation * rtu)
{
	LastFrameLength = dataLength;
	//lastRTU = rtu;
	bRepeatLastFrame = true;

	return 0;
}

void CProtocol::ClearLastFrameInfo()
{
	bRepeatLastFrame = false;
	LastFrameLength = 0;
	//lastRTU = NULL;
}


//启动规约通讯
int InitProtocol(void)
{
	return 0;
}

int UnInitProtocol()
{
	for(size_t i=0;i<theApp.g_sDataBases.size();i++)
	{
		if(theApp.g_sDataBases[i].myProtocalObj != NULL)
		{
			SetEvent(theApp.g_sDataBases[i].myProtocalObj->gethProExitEvent());
		}
	}
	Sleep(1000);
	return 0;
}


int CProtocol::ShowFrame(int msgType, byte * buf, int count)
{
	CLoopBuf::put_buf(buf,&showBuf,count);
	PostMessage(curDPR->MainFrame_hWnd,msgType,(WPARAM)curDPR,(LPARAM)count);

	return 0;
}
HANDLE CProtocol::getProRecvEvent(void)
{
	return hProRecvEvent;
}

/*
int CProtocol::InitMyTimer(void)
{
	return 0;
}
*/

int CProtocol::ValToBuf( byte * buf, ULONG val, int lenth )
{
	if (lenth <= 0 || lenth > sizeof(ULONG))
	{
		return 0;
	}

	for (int i=0;i<lenth;i++)
	{
		buf[i] = (val >> (i * 8)) & 0xff;
	}

	return lenth;
}

int CProtocol::BufToVal(byte * buf, int lenth)
{
	UINT sum = 0;

	if (lenth <= 0 || lenth > sizeof(UINT))
	{
		return 0;
	}

	for (int i=0;i<lenth;i++)
	{
		sum += buf[i] << (i * 8);
	}

	return sum;
}


void CProtocol::SetFrameLocation(byte FrameControlHead)
{
	FrameTypeLocation = FrameControlHead;
	InfoNumLocation = FrameTypeLocation + FrameTypeLength;
	TransReasonLocation = InfoNumLocation + InfoNumLength;
	AsduAddrLocation = TransReasonLocation + TransReasonLength;
	InfoAddrLocation = AsduAddrLocation + AsduAddrLength;
	DataLocation = InfoAddrLocation + InfoAddrLength;
}

/*
int CProtocol::StartEngine(CDPRStation * thisDPR)
{
	return 0;
}

int CProtocol::EndEngine(CDPRStation * thisDPR)
{
	return 0;
}
*/

int CProtocol::ResetProtocolProc(void)             //重新初始化线程
{
	return 0;
}

int CProtocol::SetProtocolVersion(int protocol)
{
	return 0;
}
int CProtocol::OpenConnect()
{
	return 0;
}

int CProtocol::CloseConnect()
{
	return 0;
}

void CProtocol::ClearRtuPtr(void)
{
	curDPR = NULL;

	RTUs.clear();
}

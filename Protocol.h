#pragma once

#include "DPR2UI.h"
#include "LoopBuf.h"
#include "MyTimer.h"
#include "StationDataBase.h"
#include <vector>
using namespace std;

//const int SEND_BUF_LENGTH = 512;

int InitProtocol(void);
int UnInitProtocol();

/*
struct stTimer
{
	DWORD tAnyFrameRecv;
	DWORD tIGramFrameRecv;
	DWORD tCallAllDataSend;
	DWORD tCallAllDDSend;
	DWORD tSetTimeSend;
	DWORD tHeartFrameSend;
	DWORD tIGramFrameSend;
	DWORD tRequireLink;

	bool bIGramFrameRecv;
};
*/

struct stMyTimer
{
	UINT     iAnyFrameRecv;
	CMyTimer tAnyFrameRecv;
	//UINT     iIGramFrameRecv;
	//CMyTimer tIGramFrameRecv;
	//UINT     iCallAllDataSend;
	//CMyTimer tCallAllDataSend;
	//UINT     iCallAllDDSend;
	//CMyTimer tCallAllDDSend;
	UINT     iSetTimeSend;
	CMyTimer tSetTimeSend;
	UINT     iHeartFrameSend;
	CMyTimer tHeartFrameSend;
	//UINT     iRequireLink;
	//CMyTimer tRequireLink;
	//UINT     iYkSelSend;
	//CMyTimer tYkSelSend;
	//UINT     iYkExeSend;
	//CMyTimer tYkExeSend;
	//UINT     iYKCancleSend;
	//CMyTimer tYKCanCleSend;
	//UINT     iCallPara;
	//CMyTimer tCallPara;
	//UINT     iSetPara;
	//CMyTimer tSetPara;
};

struct stSendCmd 
{
	UINT cmdType;
	byte Priority;
	DWORD_PTR  pParam;
	CDPRStation * thisDPR;

	stSendCmd()
	{
		cmdType = 0;
		Priority = 1;
		pParam = -1;
		thisDPR = NULL;
	}
};

class CProtocol
{
public:
	CProtocol(void);
	CProtocol(CDPRStation * myDataBase);
public:
	~CProtocol(void);

//属性
protected:
	int YK_NO;
	int Excepted_Rece_Bytes;
	int Syn_Char_Num;
	UINT MaxFrameDataBytes;
	UINT LastFrameLength;
	UINT SYX_START_ADDR;  // 单点YX起始地址
	UINT DYX_START_ADDR;  // 双点YX起始地址
	UINT YC_START_ADDR;   // YC起始地址
	UINT YK_START_ADDR;   // YK起始地址
	UINT YM_START_ADDR;   // YM起始地址

	UINT FrameTypeLength;
	UINT InfoNumLength;
	UINT TransReasonLength;
	UINT AsduAddrLength;
	UINT InfoAddrLength;

	UINT FrameTypeLocation;
	UINT InfoNumLocation;
	UINT TransReasonLocation;
	UINT AsduAddrLocation;
	UINT InfoAddrLocation;
	UINT DataLocation;
	

	DWORD ANY_FRAME_RECV_TIMEOUT;
	DWORD HEART_FRAME_SEND_TIMEOUT;
	DWORD IGRAME_FRAME_RECV_TIMEOUT;
	DWORD CALL_ALL_DATA_SEND_TIMEOUT;
	DWORD CALL_ALL_DD_SEND_TIMEOUT;
	DWORD SET_TIME_FRAME_SEND_TIMEOUT;
	DWORD YK_SEL_FRAME_SEND_TIMEOUT;
	DWORD YK_EXE_FRAME_SEND_TIMEOUT;
	DWORD YK_CANCLE_FRAME_SEND_TIMEOUT;
	DWORD REQUIRE_LINK_SEND_TIMEOUT;
	DWORD CALL_PARA_SEND_TIMEOUT;
	DWORD SET_PARA_SEND_TIMEOUT;
	
	HANDLE hProExitEvent;
	HANDLE hProSendEvent;
	HANDLE hProRecvEvent;
	vector <stSendCmd> SendCommandQue;
	CRITICAL_SECTION SendCMDQueSection;
	//stTimer timer;   //定时器
	
	CDPRStation * curDPR;
	
	bool bProtocolActive;  // 规约线程运行标志
	bool bRepeatLastFrame; // 需要重发上一帧报文标志

	struct loop_buf recvBuf;
	byte Sendbuf[BUFFER_LENGTH];

public:
	vector<CDPRStation *> RTUs;
	stMyTimer mytimer;   //定时器
	struct loop_buf showBuf;
//方法
public:
	//int MainProtocolProcess();
	bool getProtocolActive(void);
	void setProtocolActive(bool bVal);
	HANDLE gethProExitEvent(void);
	HANDLE gethProSendEvent(void);
	HANDLE getProRecvEvent(void);
	int AddSendCmd(UINT cmdType, int Priority = 1, DWORD_PTR pParam = 0,CDPRStation * thisDPR = NULL);
	int ShowFrame(int msgType, byte * buf = NULL, int count = 0);
	/***虚函数***/
	virtual int ResetProtocolProc(void);             //重新初始化线程
	virtual int SetProtocolVersion(int protocol);
	virtual int OpenConnect();
	virtual int CloseConnect();
protected:
	///*
	// 串口或者网口数据读入报文存储结构
	int ReadFrame();
	// 校验报文头
	int CheckFrameHead(void);
	// 校验报文尾
	int CheckFrameTail(byte * buf);
	// 解析报文
	int RecvProcess(void);
	// 解析报文体
	int ParseFrameBody(byte * buf);
	//*/
	//bool CompareTimeMS(DWORD lastTime, DWORD curTime, DWORD diffTime);
	int ClearSendCommandQue(void);
	//int InitTimer(DWORD curTime);
	stSendCmd getMaxPriopriyCmd(void);
	int RecordLastFrameInfo(int dataLength, CDPRStation * rtu);
	void ClearLastFrameInfo();

	int ValToBuf(byte * buf, ULONG val, int lenth);
	int BufToVal(byte * buf, int lenth);

	void SetFrameLocation(byte FrameControlHead);
public:
	void ClearRtuPtr(void);
};
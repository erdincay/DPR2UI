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

//����
protected:
	int YK_NO;
	int Excepted_Rece_Bytes;
	int Syn_Char_Num;
	UINT MaxFrameDataBytes;
	UINT LastFrameLength;
	UINT SYX_START_ADDR;  // ����YX��ʼ��ַ
	UINT DYX_START_ADDR;  // ˫��YX��ʼ��ַ
	UINT YC_START_ADDR;   // YC��ʼ��ַ
	UINT YK_START_ADDR;   // YK��ʼ��ַ
	UINT YM_START_ADDR;   // YM��ʼ��ַ

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
	//stTimer timer;   //��ʱ��
	
	CDPRStation * curDPR;
	
	bool bProtocolActive;  // ��Լ�߳����б�־
	bool bRepeatLastFrame; // ��Ҫ�ط���һ֡���ı�־

	struct loop_buf recvBuf;
	byte Sendbuf[BUFFER_LENGTH];

public:
	vector<CDPRStation *> RTUs;
	stMyTimer mytimer;   //��ʱ��
	struct loop_buf showBuf;
//����
public:
	//int MainProtocolProcess();
	bool getProtocolActive(void);
	void setProtocolActive(bool bVal);
	HANDLE gethProExitEvent(void);
	HANDLE gethProSendEvent(void);
	HANDLE getProRecvEvent(void);
	int AddSendCmd(UINT cmdType, int Priority = 1, DWORD_PTR pParam = 0,CDPRStation * thisDPR = NULL);
	int ShowFrame(int msgType, byte * buf = NULL, int count = 0);
	/***�麯��***/
	virtual int ResetProtocolProc(void);             //���³�ʼ���߳�
	virtual int SetProtocolVersion(int protocol);
	virtual int OpenConnect();
	virtual int CloseConnect();
protected:
	///*
	// ���ڻ����������ݶ��뱨�Ĵ洢�ṹ
	int ReadFrame();
	// У�鱨��ͷ
	int CheckFrameHead(void);
	// У�鱨��β
	int CheckFrameTail(byte * buf);
	// ��������
	int RecvProcess(void);
	// ����������
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
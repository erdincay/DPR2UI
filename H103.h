const int DIR_PRM = 0x40;

#pragma once
#include "Protocol.h"
#include "NetClient.h"
#include "log.h"

DWORD _stdcall H103ProtocalProc(LPVOID pParam);
//UINT H103ProtocalProc(LPVOID pParam);
struct stH103FileTransInfo 
{
	byte RII;
	byte fileType;
	//byte RepCode;
	UINT fileID;
	UINT filePos;
	UINT transStationIndex;
	UINT transDevIndex;
};

class CH103 :
	public CProtocol
{
private:
	CNetClient H103NetClient;
	CLog H103Log;
	stNET curNet;
	stH103FileTransInfo fileTransInfo;

public:
	CH103(void);
	CH103(CDPRStation * myDataBase); 
	~CH103(void);
private:
	//打开连接
	int OpenConnect();
	//关闭连接
	int CloseConnect(void);
	//检查报文头
	int CheckFrameHead(void);
	//校验报文尾
	int CheckFrameTail(byte * buf);
	// 主接收过程
	int RecvProcess(void);
	// 主发送过程
	int SendProcess(stSendCmd cmd);         

	/********接收报文***************/
	// 解析报文体
	int ParseFrameBody(byte * buf);
	int ParseShortFrame(byte * buf);
	int ParseLongFrame(byte * buf);
	//int ParseTimeCon(byte * buf);
	//int ParseEndInit(byte * buf);
	int ParseStationInfo(byte * buf);
	int ParseFaultFileMenu(byte * buf);
	int ParseDataFile(byte * buf);
	int ParseTrigRecordCon(byte * buf);
	int ParseSetTimeCon(byte * buf);
	int ParseCallTimeCon(byte * buf);
	int ParseStartSettingCon(byte * buf);
	int ParseSendSettingCon(byte * buf);
	int ParseEndSettingCon(byte * buf);

	/*******发送报文*******************/
	int AssemblePrimaryData(byte * buf);
	//int AssembleSetTime(byte * buf,SYSTEMTIME time);
	int AssembleCallFaultFileMenu(byte * buf,UINT stationIndex, UINT devIndex);
	int AssembleCallFaultFile(byte * buf,UINT stationIndex,UINT devIndex,UINT fileIndex);
	int AssembleCallStationInfo(byte * buf,UINT stationIndex);
	int AssembleCallDevSetting(byte * buf,UINT stationIndex,UINT devIndex);
	int AssembleStartDevSetting(byte * buf,UINT stationIndex,UINT devIndex);
	int AssembleSendDevSetting(byte * buf,UINT stationIndex,UINT devIndex);
	int AssembleEndDevSetting(byte * buf,UINT stationIndex,UINT devIndex);
	int AssembleSendTrigRecord(byte * buf,UINT stationIndex,UINT devIndex);
	int AssembleCallDevTime(byte * buf,UINT stationIndex,UINT devIndex);
	int AssembleSetDevTime(byte * buf,UINT stationIndex,UINT devIndex,SYSTEMTIME time);

	//初始化&反初始化
	int InitH103(void);
	void UnInitH103(void);

	/******友元函数*********************/
	friend DWORD _stdcall H103ProtocalProc(LPVOID pParam);
	//friend UINT H103ProtocalProc(LPVOID pParam);

	//虚函数接口
	int ResetProtocolProc(void);             //重新初始化线程
		
public:
	//回调函数，用于处理接收线程的数据
	static void CALLBACK ProcessRecvData(void * pH103, char * pData, unsigned long DataLength);
	//static void CALLBACK ProcessRecvData(void * pH103, WCHAR * pData, unsigned long DataLength);	
	//静态成员函数，用于处理定时器的消息
	static void CALLBACK H103TimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime);
};
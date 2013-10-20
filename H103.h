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
	//������
	int OpenConnect();
	//�ر�����
	int CloseConnect(void);
	//��鱨��ͷ
	int CheckFrameHead(void);
	//У�鱨��β
	int CheckFrameTail(byte * buf);
	// �����չ���
	int RecvProcess(void);
	// �����͹���
	int SendProcess(stSendCmd cmd);         

	/********���ձ���***************/
	// ����������
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

	/*******���ͱ���*******************/
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

	//��ʼ��&����ʼ��
	int InitH103(void);
	void UnInitH103(void);

	/******��Ԫ����*********************/
	friend DWORD _stdcall H103ProtocalProc(LPVOID pParam);
	//friend UINT H103ProtocalProc(LPVOID pParam);

	//�麯���ӿ�
	int ResetProtocolProc(void);             //���³�ʼ���߳�
		
public:
	//�ص����������ڴ�������̵߳�����
	static void CALLBACK ProcessRecvData(void * pH103, char * pData, unsigned long DataLength);
	//static void CALLBACK ProcessRecvData(void * pH103, WCHAR * pData, unsigned long DataLength);	
	//��̬��Ա���������ڴ���ʱ������Ϣ
	static void CALLBACK H103TimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime);
};
// PSerialPort.h: interface for the CPSerialPort class.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef void __stdcall LPDataArriveProc(void * m_TargetObjectPtr,char *data,int length,DWORD userdata);

class CPSerialPort  
{
public:
	CPSerialPort();
	virtual ~CPSerialPort();

	BOOL OpenPort(LPCTSTR Port,int Baudrate,int DataBits,int StopBits,int Parity,LPDataArriveProc* proc=NULL,DWORD userdata=0); //�򿪴���
	BOOL ClosePort(); //�رմ���
	
	//������رճ�������
	BOOL Activate();
	BOOL Deactivate();
	BOOL IsActive();
	
	//���������̺߳���
	static DWORD WINAPI ReadPortThread(LPVOID lpParameter);
	
	DWORD ReadPort(char *data,int length); //��ȡһ�����ȵ�����
	DWORD WritePort(char *data,int length); //��������
	DWORD WriteFileToPort(LPCTSTR FileName); //�����ļ�
	
private:
	HANDLE m_hComm; //�����豸handle
	HANDLE m_hReadThread; //�������߳�handle
	BOOL m_bReceiving; //�Ƿ��������
	int m_nBufferSize; //�����С

	char *Buffer; //������
	
	LPDataArriveProc * m_lpDataArriveProc;
	DWORD m_dwUserData;

	//���������Լ���ʱ����
	DCB dcb;
	COMMTIMEOUTS CommTimeOuts;	

private:
	// �ص������ϲ�ʵ��ָ��
	void *m_TargetObjectPtr;
	
public:
	void * getTargetObjectPtr(void);
	void setTargetObjectPtr(void * ObjPtr);
};


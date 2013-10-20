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

	BOOL OpenPort(LPCTSTR Port,int Baudrate,int DataBits,int StopBits,int Parity,LPDataArriveProc* proc=NULL,DWORD userdata=0); //打开串口
	BOOL ClosePort(); //关闭串口
	
	//激活与关闭持续接收
	BOOL Activate();
	BOOL Deactivate();
	BOOL IsActive();
	
	//接收数据线程函数
	static DWORD WINAPI ReadPortThread(LPVOID lpParameter);
	
	DWORD ReadPort(char *data,int length); //读取一定长度的数据
	DWORD WritePort(char *data,int length); //发送数据
	DWORD WriteFileToPort(LPCTSTR FileName); //发送文件
	
private:
	HANDLE m_hComm; //串口设备handle
	HANDLE m_hReadThread; //读串口线程handle
	BOOL m_bReceiving; //是否持续接收
	int m_nBufferSize; //缓冲大小

	char *Buffer; //缓冲区
	
	LPDataArriveProc * m_lpDataArriveProc;
	DWORD m_dwUserData;

	//串口设置以及超时参数
	DCB dcb;
	COMMTIMEOUTS CommTimeOuts;	

private:
	// 回调函数上层实例指针
	void *m_TargetObjectPtr;
	
public:
	void * getTargetObjectPtr(void);
	void setTargetObjectPtr(void * ObjPtr);
};


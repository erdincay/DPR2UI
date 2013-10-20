// NetClient.h: interface for the CNetClient class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_NETCLIENT_H__CF62B9AC_911A_4CE6_81B2_55CB2588A42E__INCLUDED_)
#define AFX_NETCLIENT_H__CF62B9AC_911A_4CE6_81B2_55CB2588A42E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define SERVERPORT 2404
#define BUFFER_SIZE 4096

//#include <queue>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
/*******************************************************************************************
�ļ�����NetClient.h NetClient.cpp
������CNetClient
��˵����������һ�����̼߳��ص�IOģ�͵�����ͻ������� (TCPЭ��)
����ӿ�˵�����������һ���ص�������ַ���ú�����������ԭ��
		void __stdcall ProcessRecvData(char * pData,unsigned long DataLength);
        �˻ص������������մӷ���˷��������ݻ��ߺͷ����������ӶϿ���֪ͨ
		char * pData:���չ������ݵ�ָ��
		unsigned long DataLength:���չ������ݵĳ���
		��ʾ�����pDataΪ�ղ���DataLengthΪ�����ʾ�ͷ������������Ѿ��Ͽ���
		����Ҫ���µ���Init()������ʼ�����档
����ӿ�˵����
    1�� bool Init(ProcessRecvData* pProcessRecvData,LPCTSTR szSvrAddr,unsigned long iSvrPort=SERVERPORT);
        �����ʼ������ ��һ��Ϊ�ص���ַ(������ȷ��д)���ڶ���Ϊ������Ip��ַ��������
		Ϊ����˿ں�(������ʹ�õ�Listen�˿ں�)
	2��	void UnInit();
	    ���淴��ʼ������  ���ú󽫻�ʹϵͳ���»ص�����״̬	
	3�� bool SendMsg(char * pData,unsigned long DataLength);
	    ������Ϣ��������һ��ΪҪ���͵����ݵ�ַ���ڶ���ΪҪ�������ݵ���ʵ����
	4�� LPCTSTR GetLocalIP()
	    ������������ϵͳ��ȷ��ʼ���Ժ󣬿���ʹ�ô˺�����ñ��õ�IP��ַ
*******************************************************************************************/
//������:�׽����ϵ����ݲ�����æ
#define  CLIENT_FUNERROR 0x100
class CClientSocket  
{
public:
	CClientSocket();
	CClientSocket(bool & bSuccess,int iSockType,WCHAR * szSvrAddr,unsigned short iSvrPort)
	{
		bSuccess=false;
		bSuccess=CreateSocket(iSockType);
		bSuccess=ConnectSocket(szSvrAddr,iSvrPort);
	}
	virtual ~CClientSocket();
public:
	void UnInit();
	bool CreateSocket(int iSockType){
		return CreateSocket(&m_Socket,iSockType);
	}
	bool BindSocket(WCHAR * szHostAddr,unsigned short  iHostPort){
		return BindSocket(m_Socket,szHostAddr,iHostPort);
	}
	bool ShutDownSocket(){
		return ShutDownSocket(m_Socket);
	}
	bool CloseSocekt(){
		return CloseSocket(m_Socket);
	}
	bool ConnectSocket(WCHAR * szDestAddr,unsigned short iDestPort);
	bool SendMsg(char * data,DWORD len,DWORD *retlen,DWORD time);
	bool RecvMsg(char * data,DWORD len,DWORD *retlen,DWORD time);
protected:
	bool CreateSocket(SOCKET *pNewSocket,int iSockType);
	bool BindSocket(SOCKET BindSocket,WCHAR *szHostAddr,unsigned short iHostPort);
	bool ShutDownSocket(SOCKET nowSocket);
	bool CloseSocket(SOCKET nowSocket);
	bool SendDataS(SOCKET socket,char *data,DWORD len,DWORD *retlen,WSAEVENT hSendEvent,DWORD time);
	bool RecvDataS(SOCKET socket,char *data,DWORD len,DWORD *retlen,WSAEVENT hRecvEvent,DWORD time);
private:
	bool SendData(SOCKET socket,char *data,DWORD len,DWORD *retlen,WSAEVENT hSendEvent,DWORD time);
	bool RecvData(SOCKET socket,char *data,DWORD len,DWORD *retlen,WSAEVENT hRecvEvent,DWORD time);
private:
	SOCKET m_Socket;
	WSAEVENT m_hExitEvent;
};
/*******************************************************************************
��Ϣ�����ࣺ���ڴ洢��Ϣ���У��������͵���Ϣ�ļ��ϣ������ǽ��չ�������ļ���
*******************************************************************************/
///*
template <class T> class Queue;
//��ʽ���н�㶨��
template <class T> class QueueNode
{
	friend class Queue<T>;
private:
	T data;//������
	QueueNode<T> *link;//����
	QueueNode(T d,QueueNode *l=NULL):link(l){memcpy(&data,&d,sizeof(T));}//���캯��
};
//��ʽ�����ඨ��
template <class T> class Queue
{
public:
	Queue():rear(NULL),front(NULL){}//���캯��
	~Queue();//��������
	void EnQueue(const T & item);//��item���뵽������
	T DeQueue();//ɾ�������ض�ͷԪ��
	T GetFront(){return front->data;}//�鿴��ͷԪ�ص�ֵ
	void MakeEmpty();//�ÿն���
	int IsEmpty() const {return front==NULL;}//�ж϶��пշ�
private:
	QueueNode<T> *front ,*rear;//��ͷ����βָ��
};
//*/
typedef struct _MSG_NODE
{
	unsigned long DataLength;
	char pData[BUFFER_SIZE];
	//WCHAR pData[BUFFER_SIZE];
}MSG_NODE,*PMSG_NODE;

/*********************************************************************************/

//ͨ���ص����������ϲ㴦����
typedef void __stdcall ProcessRecvData(void * m_TargetObjectPtr, char * pData,unsigned long DataLength);
//typedef void __stdcall ProcessRecvData(void * DoObject, WCHAR * pData,unsigned long DataLength);

//�ڲ����ñ�׼���߳�ģ��
DWORD _stdcall SendProc(LPVOID pParam);
DWORD _stdcall WorkProc(LPVOID pParam);
DWORD _stdcall RecvProc(LPVOID pParam);

//static char * WCHARtoChar(WCHAR * src);
//static WCHAR * ChartoWCHAR(char * src,size_t length);

class CNetClient  
{
public:
	CNetClient();
	virtual ~CNetClient();
public:
	int Init(ProcessRecvData* pProcessRecvData,LPCTSTR szSvrAddr,unsigned long iSvrPort=SERVERPORT);
	void UnInit();
	bool SendMsg(char * pData,unsigned long DataLength);
	char * GetLocalIP(){return IsStart ? HostIpAddr:NULL;}
	void * getTargetObjectPtr(void);
	void setTargetObjectPtr(void * ObjPtr);
protected:
	int InitNetWork(LPCTSTR szSvrAddr,
		unsigned int SvrPort=SERVERPORT,
		LPCTSTR HostIpAddr=NULL);
	void WriteLogString(LPCTSTR strLog);
private:
	CClientSocket m_sClient;
	ProcessRecvData* m_pProcessRecvData;
	Queue <MSG_NODE> SendMsgQueue;
	CRITICAL_SECTION SendMsgQueSection;
	HANDLE hSendEvent;
	Queue <MSG_NODE> RecvMsgQueue;
	CRITICAL_SECTION RecvMsgQueSection;
	//��ʼ�����¼�
	HANDLE hWorkEvent;
	//�˳��¼�	
	HANDLE hExitEvent;
	//�Ƿ��û������˷���ʼ������������ǣ������߳̽����ٵ��ûص�����
	bool bOprUnInit;
	//�Ƿ��Ѿ�������
	bool IsStart;
	char HostIpAddr[16];

	friend DWORD _stdcall SendProc(LPVOID pParam);
	friend DWORD _stdcall WorkProc(LPVOID pParam);
	friend DWORD _stdcall RecvProc(LPVOID pParam);
private:
	// �ص������ϲ�ʵ��ָ��
	void *m_TargetObjectPtr;
};
#endif // !defined(AFX_NETCLIENT_H__CF62B9AC_911A_4CE6_81B2_55CB2588A42E__INCLUDED_)

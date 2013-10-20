// NetClient.cpp: implementation of the CNetClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NetClient.h"
#include <fstream>
using namespace std;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CNetClient::CNetClient()
: m_TargetObjectPtr(NULL)
{
	IsStart=false;
	bOprUnInit=false;
	m_pProcessRecvData=NULL;
	hSendEvent=CreateEvent(NULL,false,false,NULL);
	hWorkEvent=CreateEvent(NULL,false,false,NULL);
	hExitEvent=CreateEvent(NULL,false,false,NULL);

	//::InitializeCriticalSection(&SendMsgQueSection);
	//::InitializeCriticalSection(&RecvMsgQueSection);
}
CNetClient::~CNetClient()
{
	CloseHandle(hSendEvent);
	CloseHandle(hWorkEvent);
	CloseHandle(hExitEvent);

	//::DeleteCriticalSection(&SendMsgQueSection);
	//::DeleteCriticalSection(&RecvMsgQueSection);
}
DWORD _stdcall SendProc(LPVOID pParam)
{
	CNetClient * pNetClient=(CNetClient*) pParam;
//	CH103 * H103Object = (CH103 *)pNetClient->m_TargetObjectPtr;
	HANDLE event[2];
	event[0]=pNetClient->hSendEvent;
	event[1]=pNetClient->hExitEvent;
	 
	//while(true)
	//while(pNetClient->IsStart)
	while(!pNetClient->bOprUnInit)
	{
		//Sleep(1);
		::EnterCriticalSection(&pNetClient->SendMsgQueSection);
		//����Ϊ�գ��ȴ������¼�����
		if(pNetClient->SendMsgQueue.IsEmpty())
		{
			::LeaveCriticalSection(&pNetClient->SendMsgQueSection);
			//Ϊ�գ����߷������
			ResetEvent(pNetClient->hSendEvent);
			TRACE("\nTheSendProc Is Waiting....");	
			DWORD Index=::WaitForMultipleObjects(2,event,false,INFINITE);
			if((Index-WAIT_OBJECT_0)==1)return 0L;
		}
		else
		{
			//ȡ��һ����㣬������
			MSG_NODE p=pNetClient->SendMsgQueue.DeQueue();
			//�ͷŶ���
			::LeaveCriticalSection(&pNetClient->SendMsgQueSection);
			DWORD retlen;
			bool bRet=pNetClient->m_sClient.SendMsg(p.pData,p.DataLength,&retlen,WSA_INFINITE);
			if(bRet==false || retlen!=p.DataLength)
			{
				if(GetLastError()!=CLIENT_FUNERROR)
				
				pNetClient->m_pProcessRecvData(pNetClient->m_TargetObjectPtr,NULL,0);
				pNetClient->UnInit();
			}	
		}
	}
	return 0L;
}
DWORD _stdcall WorkProc(LPVOID pParam)
{
	CNetClient* pNetClient=(CNetClient*)pParam;
//	CH103 * H103Object = (CH103 *)pNetClient->m_TargetObjectPtr;
	HANDLE event[2];
	event[0]=pNetClient->hWorkEvent;
	event[1]=pNetClient->hExitEvent;
	
	//while(true)
	//while(pNetClient->IsStart)
	while(!pNetClient->bOprUnInit)
	{
		//Sleep(1);
		::EnterCriticalSection(&pNetClient->RecvMsgQueSection);
		//����Ϊ�գ��ȴ������¼�����
		if(pNetClient->RecvMsgQueue.IsEmpty())
		{
			::LeaveCriticalSection(&pNetClient->RecvMsgQueSection);
			//Ϊ�գ����߷������
			ResetEvent(pNetClient->hWorkEvent);
			TRACE("\nTheWorkProc Is Waiting....");			
			DWORD Index=::WaitForMultipleObjects(2,event,false,INFINITE);
			if((Index-WAIT_OBJECT_0)==1)
				return 0L;
		}
		else
		{
			//ȡ��һ����㣬������
			MSG_NODE p=pNetClient->RecvMsgQueue.DeQueue();
			//�ͷŶ���
			::LeaveCriticalSection(&pNetClient->RecvMsgQueSection);
			//���ûص���������������
			pNetClient->m_pProcessRecvData(pNetClient->m_TargetObjectPtr,p.pData,p.DataLength);
		}
	}
	return 0L;
}
DWORD _stdcall RecvProc(LPVOID pParam)
{
	CNetClient * pNetClient=(CNetClient*)pParam;
//	CH103 * H103Object = (CH103 *)pNetClient->m_TargetObjectPtr;
	char RecvBuf[BUFFER_SIZE];
	DWORD retlen;
	//while (true)
	//while(pNetClient->IsStart)
	while(!pNetClient->bOprUnInit)
	{
		//Sleep(1);
		TRACE("\nTheRecvThread Is Waiting...");
		if(!pNetClient->m_sClient.RecvMsg(RecvBuf,BUFFER_SIZE,&retlen,WSA_INFINITE) &&  GetLastError()!=CLIENT_FUNERROR)
		{
			if(pNetClient->bOprUnInit)return 0L;//Ӧ�ó��������˳�	
			//�����Ѿ����Ͽ���֪ͨ�ϲ�(ͨ�����ûص�����)
			//pNetClient->m_pProcessRecvData(NULL,0);
			pNetClient->m_pProcessRecvData(pNetClient->m_TargetObjectPtr,NULL,0);
			pNetClient->UnInit();
			return 0L;
		}
		else
		{
			//û�յ��ֽڣ����ǳ���
			if(retlen==0)
			{
				//pNetClient->m_pProcessRecvData(NULL,0);
				pNetClient->m_pProcessRecvData(pNetClient->m_TargetObjectPtr,NULL,0);
				pNetClient->UnInit();
				return 0L;
			}
			//�����յ������ݷŵ����ն�����
			MSG_NODE Msg;
			Msg.DataLength=retlen;
			memcpy(Msg.pData,RecvBuf,retlen);
			//������Ϣ����
			::EnterCriticalSection(&pNetClient->RecvMsgQueSection);
			if(pNetClient->SendMsgQueue.IsEmpty())
			{
				pNetClient->RecvMsgQueue.EnQueue(Msg);
				::LeaveCriticalSection(&pNetClient->RecvMsgQueSection);
				//�����Ϣ����Ϊ�գ����߹����߳̿��Խ��й�����
				SetEvent(pNetClient->hWorkEvent);
			}
			else
			{
				pNetClient->RecvMsgQueue.EnQueue(Msg);
				::LeaveCriticalSection(&pNetClient->RecvMsgQueSection);
			}
		}
	}
	return 0L;
}
int CNetClient::Init(ProcessRecvData* pProcessRecvData,LPCTSTR szSvrAddr, unsigned long iSvrPort)
{
	if(pProcessRecvData==NULL //�ص�������
		|| szSvrAddr==NULL //��ַ��
		|| IsStart)//�Ѿ���������
		return false;	
	::InitializeCriticalSection(&SendMsgQueSection);
	::InitializeCriticalSection(&RecvMsgQueSection);	
	IsStart=false;
	bOprUnInit=false;
	m_pProcessRecvData=pProcessRecvData;
	int Ret=InitNetWork(szSvrAddr,iSvrPort,(LPCTSTR)HostIpAddr);
	if(0==Ret)
	{
		IsStart=true;
		return Ret;
	}
	else
	{
		return Ret;
	}
}
void CNetClient::UnInit()
{
	if(!IsStart)return;
	bOprUnInit=true;
	SetEvent(hExitEvent);	
	SetEvent(hExitEvent);
	m_sClient.UnInit();	
	::DeleteCriticalSection(&SendMsgQueSection);
	::DeleteCriticalSection(&RecvMsgQueSection);
	SendMsgQueue.MakeEmpty();
	RecvMsgQueue.MakeEmpty();	
	m_pProcessRecvData=NULL;
	IsStart=false;
}
int CNetClient::InitNetWork(LPCTSTR szSvrAddr,unsigned int SvrPort,LPCTSTR pHostIpAddress)
{
	CString LogStr;
	int Error=0;
	WSADATA wsaData;
	char Name[100];
	hostent *pHostEntry;
	in_addr rAddr;
	memset((void *)pHostIpAddress,0,sizeof(pHostIpAddress));
	//Net Start Up
	Error=WSAStartup(MAKEWORD(0x02,0x02),&wsaData);
	if(Error!=0)
	{
		Error = WSAGetLastError();
		LogStr.Format(_T("WSAStartUp Faild With Error: %d"),Error);
		WriteLogString(LogStr);

		return Error;
	}
	//Make Version
	if ( LOBYTE( wsaData.wVersion ) != 2 ||
         HIBYTE( wsaData.wVersion ) != 2 )
	{
		WSACleanup( );
		
		WriteLogString(_T("The Local Net Version Is not 2"));

		return -1;
	}
	//Get Host Ip
	Error = gethostname ( Name, sizeof(Name) );
	if( 0 == Error )
	{
		pHostEntry = gethostbyname( Name );
		if( pHostEntry != NULL )
		{
			memcpy( &rAddr, pHostEntry->h_addr_list[0], sizeof(struct in_addr) );
			sprintf((char * )pHostIpAddress,"%s",inet_ntoa( rAddr ));
		}
		else
		{
			Error = WSAGetLastError();
			LogStr.Format(_T("GetHostIp faild with Error: %d"),Error);
			WriteLogString(LogStr);

		}
	}
	else
	{
		Error = WSAGetLastError();
		LogStr.Format(_T("gethostname faild with Error: %d"),Error);
		WriteLogString(LogStr);
	}
	//Socket Create
	if(0==Error)
	{
		if(!m_sClient.CreateSocket(SOCK_STREAM))
		{
			Error=WSAGetLastError();
			LogStr.Format(_T("Create Client Socket Faild :%d"),Error);
			//WriteLogString(LogStr);
			return Error;
		}
	}
	if(0==Error)
	{
		if(!m_sClient.ConnectSocket((WCHAR *)szSvrAddr,SvrPort))
		{
			Error=WSAGetLastError();
			LogStr.Format(_T("Create Client Socket Faild :%d"),Error);
			WriteLogString(LogStr);
			return Error;
		}
	}
	//���������̣߳������߹����̵߳ĵȼ������
	if(0==Error)
	{
		HANDLE WorkHan=0;
		unsigned long WorkID;
		if((WorkHan=CreateThread(NULL,0,WorkProc,this,0,&WorkID))==NULL)
		{
			Error=GetLastError();
			LogStr.Format(_T("Create WorkThread Faild With Error %d"),Error);
			WriteLogString(LogStr);
			return Error;
		}
		SetThreadPriority(WorkHan,THREAD_PRIORITY_HIGHEST);
		CloseHandle(WorkHan);
	}
	//���������߳�
	if(0==Error)
	{
		HANDLE RecvHan=0;
		unsigned long RecvID;
		if((RecvHan=CreateThread(NULL,0,RecvProc,this,0,&RecvID))==NULL)
		{
			Error=GetLastError();
			LogStr.Format(_T("Create RecvThread Faild With Error %d"),Error);
			WriteLogString(LogStr);
			SetEvent(hExitEvent);//�˳���ǰ�������߳�
			return Error;
		}
		CloseHandle(RecvHan);
	}
	//���������߳�
	if(0==Error)
	{
		HANDLE ThreHan;
		unsigned long ThrID;
		if((ThreHan=CreateThread(NULL,0,SendProc,this,0,&ThrID))==NULL)
		{
			Error=GetLastError();
			LogStr.Format(_T("Create SEND Thred Faild With Error %d"),Error);
			WriteLogString(LogStr);
			SetEvent(hExitEvent);//�˳���ǰ�������߳�
			return Error;
		}
		CloseHandle(ThreHan);
	}
	return Error;
}
bool CNetClient::SendMsg(char * pData,unsigned long DataLength)
{		
		//δ���ó�ʼ������
		if(!IsStart || pData==NULL || DataLength==0)return false;
		//������Ϣ
		MSG_NODE Msg;
		Msg.DataLength=DataLength;
		memcpy(Msg.pData,pData,DataLength);
		//������Ϣ����
		::EnterCriticalSection(&SendMsgQueSection);
		if(SendMsgQueue.IsEmpty())
		{
			SendMsgQueue.EnQueue(Msg);
			::LeaveCriticalSection(&SendMsgQueSection);
			//�����Ϣ����Ϊ�գ����ߵȴ��ķ����߳̿��Է�����
			SetEvent(hSendEvent);
		}
		else
		{
			SendMsgQueue.EnQueue(Msg);
			::LeaveCriticalSection(&SendMsgQueSection);
		}
		return true;
}
void CNetClient::WriteLogString(LPCTSTR strLog)
{
#ifndef _DEBUG
	return;
#else
	SYSTEMTIME sysTm;
	::GetLocalTime(&sysTm);
	ofstream       OutputFile;
	OutputFile.open( "NetClientLog.txt" ,ios::app);
	OutputFile<<sysTm.wYear<<"��"<<sysTm.wMonth<<"��"<<sysTm.wDay<<"��"<<sysTm.wHour<<"ʱ"<<sysTm.wMinute<<"��"<<sysTm.wSecond<<"��"<<":"<<"    "<<(CString)strLog<<"\n"<<endl;
	OutputFile.close();
#endif
}
/********************************************************************
������  : Queue<T>::~Queue() 
�������:
�������:
��������: ����������������������ж���Ԫ��
ȫ�ֱ���: ��
����ģ��: 
����˵��: 
********************************************************************/
template <class T>  Queue<T>::~Queue()
{
	QueueNode<T> *p=front;
	while(front!=NULL)
	{
		p=front;
		front=front->link;
		delete p;
	}
}
/********************************************************************
������  : Queue<T>::EnQueue
�������:
const T & item :Ҫ����Ľ�������
�������:
��������: �ڶ����в���һ�����
ȫ�ֱ���: ��
����ģ��: 
����˵��: 
********************************************************************/
template <class T> void Queue<T>::EnQueue(const T & item)
{
	if(front==NULL)front=rear=new QueueNode<T>(item,NULL);
	else rear=rear->link=new QueueNode<T>(item,NULL);
}
/********************************************************************
������  : Queue<T>::DeQueue() 
�������:
T  :���ر�ɾ������ֵ
�������:
��������: �Ӷ�����ȡ��һ����㣬�����ظý���ֵ
ȫ�ֱ���: ��
����ģ��: 
����˵��: 
********************************************************************/
template <class T> T Queue<T>::DeQueue()
{
	T  retvalue;
	memset(&retvalue,0,sizeof(T));
	if(IsEmpty())
		return retvalue;
	QueueNode<T> * p=front;
	retvalue=p->data;
	front=front->link;
	delete p;
	return retvalue;
}
/********************************************************************
������  : Queue<T>::MakeEmpty() 
�������:
�������:
��������: ������Ԫ�����
ȫ�ֱ���: ��
����ģ��: 
����˵��: 
********************************************************************/
template <class T> void Queue<T>::MakeEmpty()
{
	if(front==NULL)return ;
	QueueNode<T> * p=front;
	while(front!=NULL)
	{
		p=front;
		front=front->link;
		delete p;
	}
	front=rear=NULL;
}
/*************************************************************************/
CClientSocket::CClientSocket()
{

}			
CClientSocket::~CClientSocket()
{
}
void CClientSocket::UnInit()
{
	ShutDownSocket();
	CloseSocekt();
	if(m_hExitEvent != (WSAEVENT)0xcccccccc && m_hExitEvent != (WSAEVENT)0xfeeefeee)
	{
		WSACloseEvent(m_hExitEvent);
	}
}
bool CClientSocket::CreateSocket(SOCKET *pNewSocket,int iSockType)
{
	m_hExitEvent=WSACreateEvent();
	WSAResetEvent(m_hExitEvent);
	
	return ((*pNewSocket=WSASocket(AF_INET,iSockType,0,NULL,0,WSA_FLAG_OVERLAPPED))==INVALID_SOCKET)?
		false:true;
}
bool CClientSocket::BindSocket(SOCKET BindSocket,WCHAR *szHostAddr,unsigned short iHostPort)
{
	struct sockaddr_in inAddr;

	char * hostAddr = WCHARtoChar(szHostAddr);
	inAddr.sin_addr.S_un.S_addr=inet_addr(hostAddr);
	delete hostAddr;
	//inAddr.sin_addr.S_un.S_addr=inet_addr(WCHARtoChar(szHostAddr));
	inAddr.sin_family=AF_INET;
	inAddr.sin_port=htons(iHostPort);
	return (bind(BindSocket,(PSOCKADDR)&inAddr,sizeof(inAddr)))
		==SOCKET_ERROR?false:true;
}
bool CClientSocket::ShutDownSocket(SOCKET nowSocket)
{
	return shutdown(nowSocket,SD_BOTH)?false:true;
}
bool CClientSocket::CloseSocket(SOCKET nowSocket)
{
	return (closesocket(nowSocket)==SOCKET_ERROR)?false:true;
}
bool CClientSocket::SendData(SOCKET socket,char *data,DWORD len,DWORD *retlen,WSAEVENT hSendEvent,DWORD time)
{
	WSABUF DataBuf;
	WSAEVENT hEvents[2];
	WSAOVERLAPPED SendOverLapp;
	DWORD flag;

	hEvents[0]=m_hExitEvent;
	hEvents[1]=hSendEvent;
	DataBuf.buf=data;
	DataBuf.len=len;
	memset(&SendOverLapp,0,sizeof(WSAOVERLAPPED));
	SendOverLapp.hEvent=hSendEvent;
	flag=0;
	/////////////////////////////////////
	int ret;
	if((ret=WSASend(socket,&DataBuf,1,retlen,flag,&SendOverLapp,NULL))==0)
		return true;
	else if((ret==SOCKET_ERROR)&&(WSAGetLastError()==WSA_IO_PENDING))
	{
		DWORD EventCaused=WSAWaitForMultipleEvents(2,hEvents,FALSE,time,FALSE);
		WSAResetEvent(hSendEvent);
		if(EventCaused == WSA_WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if(EventCaused == WAIT_OBJECT_0)
				SetLastError(CLIENT_FUNERROR);
			return false;
		}
		flag=0;
		return WSAGetOverlappedResult(socket,&SendOverLapp,retlen,false,&flag)?
			true:false;
	}
	else
		return false;
}
bool CClientSocket::RecvData(SOCKET socket,char *data,DWORD len,DWORD *retlen,WSAEVENT hRecvEvent,DWORD time)
{
	WSABUF DataBuf;
	WSAEVENT hEvents[2];
	WSAOVERLAPPED RecvOverLapp;
	DWORD flag;

	hEvents[0]=m_hExitEvent;
	hEvents[1]=hRecvEvent;
	DataBuf.buf=data;
	DataBuf.len=len;
	memset(&RecvOverLapp,0,sizeof(WSAOVERLAPPED));
	RecvOverLapp.hEvent=hRecvEvent;
	flag=0;
	/////////////////////////////////////
	int ret;
	if((ret=WSARecv(socket,&DataBuf,1,retlen,&flag,&RecvOverLapp,NULL))==0)
		return true;
	else if((ret==SOCKET_ERROR)&&(WSAGetLastError()==WSA_IO_PENDING))
	{ 
		DWORD EventCaused=WSAWaitForMultipleEvents(2,hEvents,false,time,false);
		WSAResetEvent(hRecvEvent);
		if(EventCaused == WSA_WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if(EventCaused == WAIT_OBJECT_0)
				SetLastError(CLIENT_FUNERROR);
			return false;
		}
		flag=0;
		return WSAGetOverlappedResult(socket,&RecvOverLapp,retlen,false,&flag)?
			true:false;
	}
	else
		return false;
}
bool CClientSocket::SendDataS(SOCKET socket,char *data,DWORD len,DWORD *retlen,WSAEVENT hSendEvent,DWORD time)
{
	DWORD left,idx,thisret;
	left=len;
	idx=0;
	int oflag=0;
	while(left>0)
	{
		if(!SendData(socket,&data[idx],left,&thisret,hSendEvent,time))
		{
			*retlen=0;
			return false;
		}
		WSAResetEvent(hSendEvent);
		left-=thisret;
		idx+=thisret;
		if(thisret==0)
		{
			oflag++;
			if(oflag>5)
				break;
		}
	}
	*retlen=idx;
	return (idx==len)?true:false;
}
bool CClientSocket::RecvDataS(SOCKET socket,char *data,DWORD len,DWORD *retlen,WSAEVENT hRecvEvent,DWORD time)
{
	DWORD left,idx,thisret;
	left=len;
	idx=0;
	int oflag=0;
	while(left>0)
	{
		if(!RecvData(socket,&data[idx],left,&thisret,hRecvEvent,time))
		{
			*retlen=0;
			return false;
		}
		WSAResetEvent(hRecvEvent);
		left-=thisret;
		idx+=thisret;
		if(thisret==0)
		{
			oflag++;
			if(oflag>5)
				break;
		}
	}
	*retlen=idx;
	return (idx==len)?true:false;
}
bool CClientSocket::SendMsg(char * data,DWORD len,DWORD *retlen,DWORD time)
{
	WSAEVENT hEvent=WSACreateEvent();
	bool bSend=SendDataS(m_Socket,data,len,retlen,hEvent,time);
	WSACloseEvent(hEvent);
	return bSend;
}
bool CClientSocket::RecvMsg(char * data,DWORD len,DWORD *retlen,DWORD time)
{	
	WSAEVENT hEvent=WSACreateEvent();
	bool Recv=RecvData(m_Socket,data,len,retlen,hEvent,time);
	WSACloseEvent(hEvent);
	return Recv;
}
bool CClientSocket::ConnectSocket(WCHAR * szDestAddr,unsigned short iDestPort)
{
	struct sockaddr_in inAddr;
	inAddr.sin_family=AF_INET;
	inAddr.sin_port=htons(iDestPort);
	char * hostAddr = WCHARtoChar(szDestAddr);
	inAddr.sin_addr.S_un.S_addr=inet_addr(hostAddr);
	delete hostAddr;
	//inAddr.sin_addr.S_un.S_addr=inet_addr(WCHARtoChar(szDestAddr));
	return (connect(m_Socket,(PSOCKADDR)&inAddr,sizeof(inAddr)))
		    ==SOCKET_ERROR ? false:true;
}
void * CNetClient::getTargetObjectPtr(void)
{
	return m_TargetObjectPtr;
}

void CNetClient::setTargetObjectPtr(void * ObjPtr)
{
	m_TargetObjectPtr = ObjPtr;
}

/*
static char * WCHARtoChar(WCHAR * src)
{
	int count = WideCharToMultiByte(CP_ACP,   0,   src,   -1,   NULL,   0,   NULL,   NULL); 
	char * dst = new char[count];
	memset(dst,0,count);
	WideCharToMultiByte(CP_ACP,   0,   src,   -1,   dst,count,   NULL,   NULL);   
	return dst;
}

static WCHAR * ChartoWCHAR(char * src,size_t length = 0)
{
	size_t clen = 0;
	if (length <= 0)
	{
		clen = strlen(src); 
	}
	else
	{
		clen = length;
	}
	
	size_t wlen = MultiByteToWideChar(CP_ACP,0,(const char *)src,int(clen),NULL,0);
	wchar_t *dst =new wchar_t[wlen+1];
	//wchar_t dst[2048];
	MultiByteToWideChar(CP_ACP,0,(const char *)src,int(clen),dst,int(wlen));

	return dst;

}
*/
#include "stdafx.h"
#include "log.h"

CLog::CLog()  //���캯����������־�ļ���Ĭ��·��
{
	::InitializeCriticalSection(&m_crit);   //��ʼ���ٽ���
}


CLog::~CLog()
{
	::DeleteCriticalSection(&m_crit);    //�ͷ����ٽ���
}

 
/*================================================================ 
* ��������    InitLog
* ������      LPCTST lpszLogPath
* ��������:   ��ʼ����־(������־�ļ���·��)
* ����ֵ��    void
================================================================*/ 
void CLog::InitLog(LPCTSTR lpszLogPath)   
{	
	m_strLogPath=lpszLogPath;
}

void CLog::Add(const char* fmt, ...)
{
#ifndef _DEBUG
	return;
#else

	if (m_strLogPath.IsEmpty())
		return ;
	
	if (!AfxIsValidString(fmt, -1))
		return ;
/*-----------------------�����ٽ���(д�ļ�)------------------------------------*/	
	::EnterCriticalSection(&m_crit);   
	try      
	{
		va_list argptr;          //�����ַ����ĸ�ʽ
		va_start(argptr, fmt);
		_vsnprintf(m_tBuf, BUFSIZE, fmt, argptr);
		va_end(argptr);
	}
	catch (...)
	{
		m_tBuf[0] = 0;
	}
	
	WCHAR * srcPath = m_strLogPath.GetBuffer();

	int count = WideCharToMultiByte(CP_ACP,   0,   srcPath,   -1,   NULL,   0,   NULL,   NULL); 
	char * dstPath = new char[count];
	memset(dstPath,0,count);
	WideCharToMultiByte(CP_ACP,   0,   srcPath,   -1,  dstPath,count,   NULL,   NULL);

	m_strLogPath.ReleaseBuffer();

	FILE *fp = fopen(dstPath,"a"); //����ӵķ�ʽ������ļ�
	
	if (fp)
	{
		//fprintf(fp,"%s:  ", AfxGetApp()->m_pszExeName);  //���뵱ǰ������
		
		//���뵱ǰʱ��
		//CTime ct ;                          
		//ct = CTime::GetCurrentTime();
		//fprintf(fp,"%s : ",ct.Format(_T("%m/%d/%Y %H:%M:%S")));

		time_t ltime;
		time( &ltime );
		fprintf(fp,"%s",ctime( &ltime ));
		

		//SYSTEMTIME Time;
		//GetLocalTime(&Time);
		//CString tmp;
		//tmp.Format(_T("%m/%d/%Y %H:%M:%S"),Time.wMonth,Time.wDay,Time.wYear,Time.wHour,Time.wMinute,Time.wSecond);
		//fprintf(fp,"%s : ",tmp);
		
		fprintf(fp, "%s\n", m_tBuf);		
		fclose(fp);		
	}

	delete dstPath;
	::LeaveCriticalSection(&m_crit);  
/*-------------------�˳��ٽ���----------------------------------------*/	

#endif
}



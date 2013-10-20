#include "stdafx.h"
#include "log.h"

CLog::CLog()  //构造函数，设置日志文件的默认路径
{
	::InitializeCriticalSection(&m_crit);   //初始化临界区
}


CLog::~CLog()
{
	::DeleteCriticalSection(&m_crit);    //释放里临界区
}

 
/*================================================================ 
* 函数名：    InitLog
* 参数：      LPCTST lpszLogPath
* 功能描述:   初始化日志(设置日志文件的路径)
* 返回值：    void
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
/*-----------------------进入临界区(写文件)------------------------------------*/	
	::EnterCriticalSection(&m_crit);   
	try      
	{
		va_list argptr;          //分析字符串的格式
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

	FILE *fp = fopen(dstPath,"a"); //以添加的方式输出到文件
	
	if (fp)
	{
		//fprintf(fp,"%s:  ", AfxGetApp()->m_pszExeName);  //加入当前程序名
		
		//加入当前时间
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
/*-------------------退出临界区----------------------------------------*/	

#endif
}



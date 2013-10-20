
// stdafx.cpp : 只包括标准包含文件的源文件
// DPR2UI.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include   <Dbghelp.h>


char * WCHARtoChar(const WCHAR * src)
{
	int count = WideCharToMultiByte(CP_ACP,   0,   src,   -1,   NULL,   0,   NULL,   NULL); 
	char * dst = new char[count];
	memset(dst,0,count);
	WideCharToMultiByte(CP_ACP,   0,   src,   -1,   dst,count,   NULL,   NULL);   
	return dst;
}

WCHAR * ChartoWCHAR(const char * src)
{
	int mlen = strlen(src);

	size_t wlen = MultiByteToWideChar(CP_ACP,0,(const char *)src,-1,NULL,0);
	wchar_t *dst =new wchar_t[wlen];
	MultiByteToWideChar(CP_ACP,0,(const char *)src,-1,dst,int(wlen));

	return dst;
}

//判断CString是否为数字并转换
//这种方法是通过逐个字符地判断来确定是否符合数字的模式，不过最方便的方法是用正则表达式，可以简单地判断字符串是否匹配"^\d+$"。可惜还没有在VC2005里面成功配置到正则的环境。 

//判断是否为数字 

BOOL IsDigital(CString &strItem)
{
	//去掉空格字符
	strItem.Replace(_T(" "),_T(""));
	for(int i= 0; i<strItem.GetLength(); i++)
	{
		// check"+"、"-"正负号
		if (i== 0 && (strItem.GetAt(i)== 0x2B || strItem.GetAt(i)== 0x2D))
			continue; 

		// check char
		if ( !isdigit(strItem.GetAt(i)) && strItem.GetAt(i)!= L'.')
			return FALSE;
	}
	// check 小数点
	if (strItem.Find(L'.')!= strItem.ReverseFind(L'.'))
		return FALSE;
	return TRUE;
} 



//判断为正确的字符串后调用系统函数直接转换 

BOOL CString2Float(CString strConv,float &fResult)
{
	CString strTmp;
	strTmp = strConv;
	//先判断是否为数字，然后再转换，如果为非数值，返回－１
	if (IsDigital(strTmp))
	{
		fResult =  (float)(_wtof(strTmp)); //dingke
		return TRUE;
	} 

	//这个返回可以设置为Int类型的负向或者正向的最大值，根据实际的需要调整为一个不需要的使用范围 

	return FALSE;
}

BOOL CString2Int(CString strConv,int &nResult)
{
	CString strTmp;
	strTmp = strConv;
	//先判断是否为数字，然后再转换，如果为非数值，返回－１
	if (IsDigital(strTmp))
	{
		nResult =  _wtoi(strTmp); //dingke
		return TRUE;
	} 

	//这个返回可以设置为Int类型的负向或者正向的最大值，根据实际的需要调整为一个不需要的使用范围 

	return FALSE;
}

BOOL CString2IPByte(const CString &str,WORD ip[4])
{
	int nCount = 0;
	int nBegin = 0;
	int nEnd = 0;
	int nTmp;
	BOOL flag = TRUE;
	CString strTmp;

	while(flag)
	{
		nBegin = str.Find(_T("."),nEnd);
		if(nBegin < 0)
		{
			flag = FALSE;
			nBegin = str.GetLength();
		}
		strTmp = str.Mid(nEnd,min(nBegin - nEnd,3) );
		nEnd = nBegin + 1;
		if(CString2Int(strTmp,nTmp))
		{
			ip[nCount] = BYTE(nTmp);
			nCount++;
		}
		if(nCount > 4)
		{
			flag = FALSE;
		}
	}

	if(nCount < 5)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

void Bytes2CString(CString &str,BYTE *buf,int nLen)
{
	int i;
	CString str1;
	for(i = 0;i < nLen; i++)
	{
		str1.Format(_T("%02X "),buf[i]);
		str += str1;
	}
	return;
}

bool AssertIPFormat(WCHAR * val)
{
	CString strIP = val;
	CString strByte;
	int iByte = 255;
	for (int i=0;i<3;i++)
	{
		int index = strIP.FindOneOf(_T("."));
		if (index < 0)
		{
			return false;
		}
		strByte = strIP.Left(index);
		if(CString2Int(strByte,iByte))
		{
			if (iByte < 0 || iByte > 255)
			{
				return false;
			}
		}
		else
		{
			return false;
		}

		strIP = strIP.Right(strIP.GetLength() - index - 1);
	}

	if(CString2Int(strIP,iByte))
	{
		if (iByte < 0 || iByte > 255)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

int CalcCheckSumWord(const byte * Ptr, int count)
{
	if (count < 0)
	{
		return -1;
	}

	WORD sum = 0;

	for (int i=0;i<count;i += 2)
	{
		sum += Ptr[i]+Ptr[i + 1] * 0x100;
	}

	return sum;
}

int CalcCheckSumByte(const byte * Ptr, int count)
{
	if (count < 0)
	{
		return -1;
	}

	WORD sum = 0;

	for (int i=0;i<count;i++)
	{
		sum += Ptr[i];
	}

	return sum;
}

int CalcCheckSum8Bit(const byte * Ptr, int count)
{
	if (count < 0)
	{
		return -1;
	}

	byte sum = 0;

	for (int i=0;i<count;i++)
	{
		sum += Ptr[i];
	}

	return sum;
}

int CalcAntiCheckSumWord(WORD sum)
{
	return ~sum;
}

bool FolderExist(CString strPath)
{
	WIN32_FIND_DATA wfd;
	bool bRet = false;

	HANDLE hFind = FindFirstFile(strPath,&wfd);

	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		bRet = true;
	}

	FindClose(hFind);

	return bRet;
}

bool FileExist(CString strFileName)
{
	CFileFind ffinder;

	bool ret = ffinder.FindFile(strFileName);

	return ret;
}

bool BuildDirectory(CString path)
{
	/*
	if (!FolderExist(path))
	{
		return ::CreateDirectory(path, NULL); 
	}
	*/

	path += _T("\\");
	char * AllPath = WCHARtoChar(path.GetBuffer());
	path.ReleaseBuffer();
	//MakeSureDirectoryPathExists(AllPath);
	delete AllPath;

	return true;
}

bool FindFileSuffixName(CString src,CString & dst)
{
	int index = src.ReverseFind(_T('.'));
	if (index >= 0)
	{
		dst = src.Right(src.GetLength() - index);
		return true;
	}

	return false;
}
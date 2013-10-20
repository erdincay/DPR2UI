
// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// DPR2UI.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

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

//�ж�CString�Ƿ�Ϊ���ֲ�ת��
//���ַ�����ͨ������ַ����ж���ȷ���Ƿ�������ֵ�ģʽ���������ķ�������������ʽ�����Լ򵥵��ж��ַ����Ƿ�ƥ��"^\d+$"����ϧ��û����VC2005����ɹ����õ�����Ļ����� 

//�ж��Ƿ�Ϊ���� 

BOOL IsDigital(CString &strItem)
{
	//ȥ���ո��ַ�
	strItem.Replace(_T(" "),_T(""));
	for(int i= 0; i<strItem.GetLength(); i++)
	{
		// check"+"��"-"������
		if (i== 0 && (strItem.GetAt(i)== 0x2B || strItem.GetAt(i)== 0x2D))
			continue; 

		// check char
		if ( !isdigit(strItem.GetAt(i)) && strItem.GetAt(i)!= L'.')
			return FALSE;
	}
	// check С����
	if (strItem.Find(L'.')!= strItem.ReverseFind(L'.'))
		return FALSE;
	return TRUE;
} 



//�ж�Ϊ��ȷ���ַ��������ϵͳ����ֱ��ת�� 

BOOL CString2Float(CString strConv,float &fResult)
{
	CString strTmp;
	strTmp = strConv;
	//���ж��Ƿ�Ϊ���֣�Ȼ����ת�������Ϊ����ֵ�����أ���
	if (IsDigital(strTmp))
	{
		fResult =  (float)(_wtof(strTmp)); //dingke
		return TRUE;
	} 

	//������ؿ�������ΪInt���͵ĸ��������������ֵ������ʵ�ʵ���Ҫ����Ϊһ������Ҫ��ʹ�÷�Χ 

	return FALSE;
}

BOOL CString2Int(CString strConv,int &nResult)
{
	CString strTmp;
	strTmp = strConv;
	//���ж��Ƿ�Ϊ���֣�Ȼ����ת�������Ϊ����ֵ�����أ���
	if (IsDigital(strTmp))
	{
		nResult =  _wtoi(strTmp); //dingke
		return TRUE;
	} 

	//������ؿ�������ΪInt���͵ĸ��������������ֵ������ʵ�ʵ���Ҫ����Ϊһ������Ҫ��ʹ�÷�Χ 

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
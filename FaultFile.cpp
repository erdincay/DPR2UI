#include "StdAfx.h"
#include "FaultFile.h"

CFaultFile::CFaultFile(void)
{
	setParentFFs(NULL);
}

CFaultFile::CFaultFile(CFaultFiles * ptr)
{
	setParentFFs(ptr);
}

CFaultFile::~CFaultFile(void)
{
}

WCHAR * CFaultFile::getFaultFileName(void)
{
	return wFileLocalName;
}

int CFaultFile::setFaultFileName(WCHAR * name)
{
	lstrcpy(wFileLocalName,name);

	CString dstName = name;

	int index = dstName.ReverseFind(_T('//'));
	if (index >= 0)
	{
		CString strRight = dstName.Right(dstName.GetLength() - index - 1);
		CString left = dstName.Left(index);

		index = left.ReverseFind(_T('//'));
		if (index >= 0)
		{
			CString menu = left.Right(left.GetLength() - index - 1);

			dstName = menu + _T("\\") + strRight;
		}
	}

	setLocalFileName(dstName.GetBuffer());
	dstName.ReleaseBuffer();

	return 0;
}

WCHAR * CFaultFile::getLocalFileName(void)
{
	return wFileFullName;
}

int CFaultFile::setLocalFileName(WCHAR * name)
{
	lstrcpy(wFileFullName,name);
	return 0;
}

WCHAR * CFaultFile::getTrigReason(void)
{
	return wTrigReason;
}

int CFaultFile::setTrigReason(WCHAR * reason)
{
	memset(&wTrigReason,0,sizeof(wTrigReason));

	lstrcpy(wTrigReason,reason);
	return 0;
}

bool CFaultFile::getFileBeExisted(void)
{
	return bLocalExisted;
}

int CFaultFile::setFileBeExisted(bool val)
{
	bLocalExisted = val;
	return 0;
}

SYSTEMTIME CFaultFile::getFileTime(void)
{
	return sTime;
}

int CFaultFile::setFileTime(SYSTEMTIME val)
{
	sTime = val;
	return 0;
}

WCHAR * CFaultFile::getFileCatalog(void)
{
	return wfileCatalog;
}

int CFaultFile::setFileCatalog(WCHAR * log)
{
	lstrcpy((LPWSTR)wfileCatalog,log);

	return 0;
}

void CFaultFile::setParentFFs(CFaultFiles * ptr)
{
	parentFFs = ptr;
}

const CFaultFiles * CFaultFile::getParentFFs()
{
	return parentFFs;
}

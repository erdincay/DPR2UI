#pragma once

class CFaultFiles;

class CFaultFile
{
protected:
	WCHAR wfileCatalog[MAX_NAME_LENGTH];
	WCHAR wFileFullName[MAX_NAME_LENGTH];
	WCHAR wFileLocalName[MAX_NAME_LENGTH];
	WCHAR wTrigReason[MAX_NAME_LENGTH];
	SYSTEMTIME sTime;
	bool bLocalExisted;
	CFaultFiles * parentFFs;
protected:
	void setParentFFs(CFaultFiles * ptr);
public:
	CFaultFile(void);
	CFaultFile(CFaultFiles * ptr);
	~CFaultFile(void);
	WCHAR * getFaultFileName(void);
	int setFaultFileName(WCHAR * name);
	WCHAR * getLocalFileName(void);
	int setLocalFileName(WCHAR * name);
	WCHAR * getTrigReason(void);
	int setTrigReason(WCHAR * reason);
	bool getFileBeExisted(void);
	int setFileBeExisted(bool val);
	SYSTEMTIME getFileTime(void);
	int setFileTime(SYSTEMTIME val);
	WCHAR * getFileCatalog(void);
	int setFileCatalog(WCHAR * log);
	const CFaultFiles * getParentFFs();
};

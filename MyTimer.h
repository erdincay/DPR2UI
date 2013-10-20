#include <afxtempl.h>

class CMyTimer;
//��ģ�����е�ӳ����ඨ��һ����������
typedef CMap<UINT,UINT,CMyTimer*,CMyTimer*> CTimerMap;

class CMyTimer  
{
public:
	static UINT index;
	void * TargetObj;
public:
	//���ö�ʱ����nElapse��ʾʱ������sz��ʾҪ��ʾ������
	//void SetMyTimer(UINT nElapse,CString sz);
	UINT SetMyTimer(UINT nElapse,TIMERPROC timeProc = NULL,void * obj = NULL);
	//���ٸ�ʵ���Ķ�ʱ��
	void KillMyTimer();
	//�����ʵ���Ķ�ʱ����־ֵ
	UINT m_nTimerID;
	//��̬���ݳ�ԱҪ��ʾ������
	CString szContent;
	//������̬���ݳ�Ա��ӳ����࣬���ڱ������еĶ�ʱ����Ϣ
	static CTimerMap m_sTimeMap;
	//��̬��Ա���������ڴ���ʱ������Ϣ
	static void CALLBACK MyTimerProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime);
	CMyTimer();
	CMyTimer(UINT myID);
	virtual ~CMyTimer();
	void RemoveItemKey();
	void setMyID(UINT id);
};

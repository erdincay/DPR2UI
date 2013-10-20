
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "DPR2UI.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
	m_iStationIndex = NO_STATION_SEl;
	m_iDevIndex = NO_DEV_SEL;
	m_iFaultFileIndex = NO_FAULTFILE_SEL;

	IAmBeingShowed = true;
	bExpandAll = true;
}

CPropertiesWnd::~CPropertiesWnd()
{
	IAmBeingShowed = false;
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_SAVE_PROPERTIES, OnSaveProperties)
	ON_UPDATE_COMMAND_UI(ID_SAVE_PROPERTIES, OnUpdateSaveProperties)
	ON_COMMAND(ID_PROPERTIES2, OnRefreshProperties)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateRefreshProperties)
	ON_CBN_SELCHANGE(IDR_COMBOBOX_COMM, OnComboSelChange)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar ��Ϣ�������

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient,rectCombo;
	GetClientRect(rectClient);

	m_wndObjectCombo.GetWindowRect(&rectCombo);

	int cyCmb = rectCombo.Size().cy;
	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), 200, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + cyCmb + cyTlb, rectClient.Width(), rectClient.Height() -(cyCmb+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// �������:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, IDR_COMBOBOX_COMM))
	{
		TRACE0("δ�ܴ���������� \n");
		return -1;      // δ�ܴ���
	}

	m_wndObjectCombo.AddString(_T(COMM_STR_ETH));
	m_wndObjectCombo.AddString(_T(COMM_STR_PHO));
	m_wndObjectCombo.SetFont(CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT)));
	//m_wndObjectCombo.SetCurSel(1);

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, IDR_MFCPROPERTYGRIDCTRL))
	{
		TRACE0("δ�ܴ�����������\n");
		return -1;      // δ�ܴ���
	}
	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

	//iStationIndex = ((CMainFrame *)AfxGetMainWnd())->getSelectedStation();
	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* ������*/);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* ����*/);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// �������ͨ���˿ؼ�·�ɣ�������ͨ�������·��:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();

	((CMainFrame *)AfxGetMainWnd())->RecalcLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	bExpandAll = !bExpandAll;

	m_wndPropList.ExpandAll(bExpandAll);
	
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* pCmdUI)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnSaveProperties()
{
	// TODO: �ڴ˴���������������
	if (m_iStationIndex < 0 || m_iStationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("ѡ����վ�ٽ��б������");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		return;
	}

	CString str;
	int cur = m_wndObjectCombo.GetCurSel();
	m_wndObjectCombo.GetLBText(cur,str);
	if (str.Compare(_T(COMM_STR_ETH)) == 0)
	{
		theApp.g_sDataBases[m_iStationIndex].setCommType(COMM_TYPE_ETH);
	}
	else if (str.Compare(_T(COMM_STR_PHO)) == 0)
	{
		theApp.g_sDataBases[m_iStationIndex].setCommType(COMM_TYPE_PHO);
	}
	
	//���ø���
	CString strVal;
	int iVal;
	int iRet;

	strVal = m_gridPropName->GetValue();
	iRet = theApp.g_sDataBases[m_iStationIndex].setStationName(strVal.GetBuffer());
	strVal.ReleaseBuffer();
	if (iRet)
	{
		CString str;
		str.Format(_T("��վ%d �������ó���"),m_iStationIndex);
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	strVal = m_gridPropID->GetValue();
	if(!CString2Int(strVal,iVal))
	{
		CString str;
		str.Format(_T("��վ%d ID���ó���"),m_iStationIndex);
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}
	iRet = theApp.g_sDataBases[m_iStationIndex].setStationID(iVal);
	if (iRet)
	{
		CString str;
		str.Format(_T("��վ%d ID���ó���"),m_iStationIndex);
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return;
	}

	strVal = m_gridPropAddr->GetValue();
	if(!CString2Int(strVal,iVal))
	{
		CString str;
		str.Format(_T("��վ%d ��ַ���ó���"),m_iStationIndex);
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		return;
	}
	iRet = theApp.g_sDataBases[m_iStationIndex].setStationAddr(iVal);
	if (iRet)
	{
		CString str;
		str.Format(_T("��վ%d ��ַ���ó���"),m_iStationIndex);
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		return;
		return;
	}

	switch (theApp.g_sDataBases[m_iStationIndex].getCommType())
	{
	case COMM_TYPE_ETH:
		strVal = m_gridPropIP->GetValue();
		iRet = theApp.g_sDataBases[m_iStationIndex].setStationIPAddr(strVal.GetBuffer());
		strVal.ReleaseBuffer();
		if (iRet)
		{
			CString str;
			str.Format(_T("��վ%d IP��ַ���ó���"),m_iStationIndex);
			AfxMessageBox(str);
			((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
			return;
			return;
		}

		strVal = m_gridPropPort->GetValue();
		if(!CString2Int(strVal,iVal))
		{
			CString str;
			str.Format(_T("��վ%d IP�˿����ó���"),m_iStationIndex);
			AfxMessageBox(str);
			((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

			return;
		}
		iRet = theApp.g_sDataBases[m_iStationIndex].setStationIPPort(iVal);
		if (iRet)
		{
			CString str;
			str.Format(_T("��վ%d IP�˿����ó���"),m_iStationIndex);
			AfxMessageBox(str);
			((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

			return;
		}
		break;

	case COMM_TYPE_PHO:
		strVal = m_gridPropPhone->GetValue();
		iRet = theApp.g_sDataBases[m_iStationIndex].setStationPhoneNO(strVal.GetBuffer());
		strVal.ReleaseBuffer();
		if (iRet)
		{
			CString str;
			str.Format(_T("��վ%d �绰�������ó���"),m_iStationIndex);
			AfxMessageBox(str);
			((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

			return;
		}

		strVal = m_gridPropModem->GetValue();
		for (int i=0;i<m_gridPropModem->GetOptionCount();i++)
		{
			if(strVal.Compare(m_gridPropModem->GetOption(i)) == 0)
			{
				iVal = i;
			}
		}
		iRet = theApp.g_sDataBases[m_iStationIndex].setModemIndex(iVal);
		if (iRet)
		{
			CString str;
			str.Format(_T("��վ%d Modem���ó���"),m_iStationIndex);
			AfxMessageBox(str);
			((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

			return;
		}

		strVal = m_gridPropUserName->GetValue();
		iRet = theApp.g_sDataBases[m_iStationIndex].setPhoneUsrName(strVal.GetBuffer());
		strVal.ReleaseBuffer();
		if (iRet)
		{
			CString str;
			str.Format(_T("��վ%d �û������ó���"),m_iStationIndex);
			AfxMessageBox(str);
			((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

			return;
		}

		strVal = m_gridPropPassWord->GetOriginalValue();
		iRet = theApp.g_sDataBases[m_iStationIndex].setPhonePassWord(strVal.GetBuffer());
		strVal.ReleaseBuffer();
		if (iRet)
		{
			CString str;
			str.Format(_T("��վ%d �û��������ó���"),m_iStationIndex);
			AfxMessageBox(str);
			((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

			return;
		}
		break;

	default:
		break;
	}

	for (int i=0;i<theApp.g_sDataBases[m_iStationIndex].vDevs.size();i++)
	{
		if (!CString2Int((m_gridPropDev[i]->GetSubItem(0))->GetValue(),iVal))
		{
			CString str;
			str.Format(_T("��վ%dװ��%d ID���ó���"),m_iStationIndex,i);
			AfxMessageBox(str);
			((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

			return;
		}
		theApp.g_sDataBases[m_iStationIndex].vDevs[i].setDevID(iVal);

		if (!CString2Int((m_gridPropDev[i]->GetSubItem(1))->GetValue(),iVal))
		{
			CString str;
			str.Format(_T("��վ%dװ��%d ��ַ���ó���"),m_iStationIndex,i);
			AfxMessageBox(str);
			((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

			return;
		}
		theApp.g_sDataBases[m_iStationIndex].vDevs[i].setDevAddr(iVal);
	}

	((CMainFrame *)AfxGetMainWnd())->RefreshStationView();
	((CMainFrame *)AfxGetMainWnd())->RefreshFileView();
}

void CPropertiesWnd::OnUpdateSaveProperties(CCmdUI* /*pCmdUI*/)
{
	// TODO: �ڴ˴����������� UI ����������
}

void CPropertiesWnd::OnRefreshProperties()
{
	RefreshPropertyView(m_iStationIndex,m_iDevIndex,m_iFaultFileIndex);
}

void CPropertiesWnd::OnUpdateRefreshProperties(CCmdUI* /*pCmdUI*/)
{
	// TODO: �ڴ˴����������� UI ����������
}

void CPropertiesWnd::InitPropList()
{
	if (m_iStationIndex < 0 || m_iStationIndex >= theApp.g_sDataBases.size())
	{
		return;
	}

	m_wndPropList.RemoveAll();

	SetPropListFont();

	CMFCPropertyGridProperty * pGroupStation = new CMFCPropertyGridProperty(_T("��վ��ʶ"));
	if (!InitStationGroup(pGroupStation,m_iStationIndex))
	{
		m_wndPropList.AddProperty(pGroupStation);
	} 
	else
	{
		return;
	}

	CMFCPropertyGridProperty * pGroupComm = new CMFCPropertyGridProperty(_T("ͨѶ�ӿ�"));
	if(!InitCommGroup(pGroupComm,m_iStationIndex))
	{
		m_wndPropList.AddProperty(pGroupComm);
	}
	else
	{
		return;
	}

	CMFCPropertyGridProperty* pGroupDev = new CMFCPropertyGridProperty(_T("װ����Ϣ"));
	if (!InitDevGroup(pGroupDev,m_iStationIndex))
	{
		m_wndPropList.AddProperty(pGroupDev);
	}
	else
	{
		return;
	}

	
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
}


int CPropertiesWnd::InitCommGroup(CMFCPropertyGridProperty * pGroupComm,UINT index)
{
	CString str;
	int cur = m_wndObjectCombo.GetCurSel();
	m_wndObjectCombo.GetLBText(cur,str);
	if (str.Compare(_T(COMM_STR_ETH)) == 0)
	{
		CString ip = theApp.g_sDataBases[index].getStationIPAddr();
		CString port;
		port.Format(_T("%.4d"),theApp.g_sDataBases[index].getStationIPPort());
		m_gridPropIP = new CMFCPropertyGridProperty(_T("IP��ַ"),(_variant_t)ip, _T("IP��ַ����***.***.***.***"));
		m_gridPropPort = new CMFCPropertyGridProperty(_T("�˿ں�"),(_variant_t)port, _T("����ͨѶ�˿�"));
		pGroupComm->AddSubItem(m_gridPropIP);
		pGroupComm->AddSubItem(m_gridPropPort);
	}

	else if (str.Compare(_T(COMM_STR_PHO)) == 0)
	{
		int count = theApp.rasClient.getEntryCount();

		if (count > 0)
		{
			//CString * strName = new CString[count];
			//theApp.rasClient.GetModemName(strName);

			m_gridPropModem = new CMFCPropertyGridProperty(_T("ѡ������"),(_variant_t)theApp.rasClient.getEntryName(theApp.g_sDataBases[index].getModemIndex()), _T("ѡ�����Ҫʹ�õ�MODEM������"));
			
			for (int i=0;i<count;i++)
			{
				m_gridPropModem->AddOption(theApp.rasClient.getEntryName(i));
			}
			//delete[] strName;
		}
		else
		{
			m_gridPropModem = new CMFCPropertyGridProperty(_T("ѡ������"), _T("None"), _T("ѡ�����Ҫʹ�õ�MODEM������"));
		}		
		
		m_gridPropModem->AllowEdit(false);
		pGroupComm->AddSubItem(m_gridPropModem);

		CString phone = theApp.g_sDataBases[index].getStationPhoneNO();
		m_gridPropPhone = new CMFCPropertyGridProperty(_T("�绰����"),(_variant_t)phone, _T("�绰����"));
		CString usr = theApp.g_sDataBases[index].getPhoneUsrName();
		m_gridPropUserName = new CMFCPropertyGridProperty(_T("�û���"),(_variant_t)usr, _T("�����û���"));
		CString pas = theApp.g_sDataBases[index].getPhonePassWord();
		m_gridPropPassWord = new CMFCPropertyGridProperty(_T("����"),_T(""),_T("��������"));
		int pasLength = pas.GetLength();
		CString pasShow;
		for (int i=0;i<pasLength;i++)
		{
			pasShow += _T("*");
		}
		m_gridPropPassWord->SetValue(pasShow);
		m_gridPropPassWord->SetOriginalValue(pas);
		
		pGroupComm->AddSubItem(m_gridPropPhone);
		pGroupComm->AddSubItem(m_gridPropUserName);
		pGroupComm->AddSubItem(m_gridPropPassWord);
	}

	return 0;
}

void CPropertiesWnd::OnComboSelChange()
{
	InitPropList();
}

int CPropertiesWnd::InitDevGroup(CMFCPropertyGridProperty * pGroupDev, UINT index)
{
	CString strTmp;
	for (int i=0;i<theApp.g_sDataBases[index].getDevNums();i++)
	{
		strTmp.Format(_T("װ�� %d"),i+1);
		m_gridPropDev[i] = new CMFCPropertyGridProperty(strTmp);

		strTmp.Format(_T("%.4d"),((theApp.g_sDataBases[index]).vDevs[i]).getDevID());
		m_gridPropDev[i]->AddSubItem(new CMFCPropertyGridProperty(_T("װ��ID"),(_variant_t)strTmp, _T("��վ�¸���װ�ñ�ʶ")));

		strTmp.Format(_T("%.3d"),((theApp.g_sDataBases[index]).vDevs[i]).getDevAddr());
		m_gridPropDev[i]->AddSubItem(new CMFCPropertyGridProperty(_T("װ�õ�ַ"),(_variant_t)strTmp, _T("��վ�¸���װ�õ�ַ")));
		
		pGroupDev->AddSubItem(m_gridPropDev[i]);
	}
	return 0;
}

int CPropertiesWnd::InitStationGroup(CMFCPropertyGridProperty * pGroupStation, UINT index)
{
	CString strTmp;

	strTmp.Format(_T("%s"),theApp.g_sDataBases[index].getStationName());
	m_gridPropName = new CMFCPropertyGridProperty(_T("��վ����"),(_variant_t)strTmp, _T("��վ����"));

	strTmp.Format(_T("%.4d"),theApp.g_sDataBases[index].getStationID());
	m_gridPropID = new CMFCPropertyGridProperty(_T("��վID"),(_variant_t)strTmp, _T("��վID"));

	strTmp.Format(_T("%.3d"),theApp.g_sDataBases[index].getStationAddr());
	m_gridPropAddr = new CMFCPropertyGridProperty(_T("��վ��ַ"),(_variant_t)strTmp, _T("��վ��ַ"));

	pGroupStation->AddSubItem(m_gridPropName);
	pGroupStation->AddSubItem(m_gridPropID);
	pGroupStation->AddSubItem(m_gridPropAddr);

	return 0;
}

void CPropertiesWnd::RefreshPropertyView(int station,int dev,int file)
{
	bExpandAll = true;

	m_iStationIndex = station;
	m_iDevIndex = dev;
	m_iFaultFileIndex = file;

	if (m_iStationIndex < 0 || m_iStationIndex >= theApp.g_sDataBases.size())
	{
		return;
	}
	int iFindEth = -1;
	int iFindPho = -1;
	CString str;
	for (int i=0;i<m_wndObjectCombo.GetCount();i++)
	{
		m_wndObjectCombo.GetLBText(i,str);
		if(str.Compare(_T(COMM_STR_ETH)) == 0)
		{
			iFindEth = i;
		}
		else if (str.Compare(_T(COMM_STR_PHO)) == 0)
		{
			iFindPho = i;
		}
	}
	if (iFindEth < 0)
	{
		iFindEth = m_wndObjectCombo.AddString(_T(COMM_STR_ETH));
	}
	if (iFindPho < 0)
	{
		iFindPho = m_wndObjectCombo.AddString(_T(COMM_STR_PHO));
	}
	if (iFindEth == CB_ERR || iFindPho == CB_ERR)
	{
		CString str = _T("ͨѶ���ô���");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		
		return;
	}
	byte commType = theApp.g_sDataBases[m_iStationIndex].getCommType();
	switch (commType)
	{
	case COMM_TYPE_ETH:
		m_wndObjectCombo.SetCurSel(iFindEth);
		break;

	case COMM_TYPE_PHO:
		m_wndObjectCombo.SetCurSel(iFindPho);
		break;

	default:
		break;
	}

	InitPropList();
}

bool CPropertiesWnd::getbShow(void)
{
	return IAmBeingShowed;
}

void CPropertiesWnd::setbShow(bool val)
{
	IAmBeingShowed = val;
}

///*
void CPropertiesWnd::ShowPane(BOOL bShow,BOOL bDelay,BOOL bActivate)
{
	CDockablePane::ShowPane(bShow,bDelay,bActivate);
	setbShow(bShow);
	((CMainFrame *)AfxGetMainWnd())->RecalcLayout();
}
//*/

void CPropertiesWnd::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDockablePane::OnShowWindow(bShow, nStatus);

	// TODO: �ڴ˴������Ϣ����������
	setbShow(bShow);
	((CMainFrame *)AfxGetMainWnd())->RecalcLayout();
}

LRESULT CPropertiesWnd::OnPropertyChanged (WPARAM,LPARAM lParam)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*) lParam;
	if (pProp == m_gridPropPassWord)
	{
		CString pas = pProp->GetValue();
		int pasLength = pas.GetLength();
		CString pasShow;
		for (int i=0;i<pasLength;i++)
		{
			pasShow += _T("*");
		}
		pProp->SetValue(pasShow);
		pProp->SetOriginalValue(pas);

	}

	return 0L;
}


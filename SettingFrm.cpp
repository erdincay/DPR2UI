// SettingFrm.cpp : CSettingFrame 类的实现
//

#include "stdafx.h"
#include "MainFrm.h"
#include "SettingFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSettingFrame

IMPLEMENT_DYNCREATE(CSettingFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CSettingFrame, CMDIChildWndEx)
	ON_COMMAND_RANGE(ID_SETTING_VERITY, ID_COLLAPSE_ALL, OnPopSetting)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SETTING_VERITY,ID_COLLAPSE_ALL,OnUpdatePopSetting)
	ON_COMMAND(ID_OPEN_LOCAL_MENU,OnOpenLocalMenu)
	ON_UPDATE_COMMAND_UI(ID_OPEN_LOCAL_MENU,OnUpdateLocalMenu)
	ON_COMMAND(ID_REFRESH,OnRefresh)
	ON_UPDATE_COMMAND_UI(ID_REFRESH,OnUpdateRefresh)
	ON_COMMAND(ID_FILE_CLOSE, &CSettingFrame::OnFileClose)
	ON_MESSAGE(WM_PATCH_PROCESS,OnPatchProcessMsg)
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

// CSettingFrame 构造/析构

CSettingFrame::CSettingFrame()
{
	// TODO: 在此添加成员初始化代码
	m_iStationIndex = NO_STATION_SEl;
	m_iDevIndex = NO_DEV_SEL;
	copyInfo.bCopying = false;
	pRoot = NULL;
	//pAISetting = NULL;
	//pDISetting = NULL;
	//pFreSetting = NULL;
	//pSeqSetting = NULL;

	for (int i=0;i<m_iChannelTypeSum;i++)
	{
		m_strListChannelType.AddTail(m_strChannelType[i]);
	}

	for (int i=0;i<m_iDIChoose;i++)
	{
		m_strListDIChoose.AddTail(m_strDIChoose[i]);
	}

}

CSettingFrame::~CSettingFrame()
{
	((CMainFrame *)AfxGetMainWnd())->ClearSettingFrmPtr();
}


BOOL CSettingFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
	{
		return FALSE;
	}

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CSettingFrame 诊断

#ifdef _DEBUG
void CSettingFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CSettingFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CSettingFrame 消息处理程序
void CSettingFrame::OnFileClose() 
{
	// 若要关闭框架，只需发送 WM_CLOSE，
	// 这相当于从系统菜单中选择关闭。
	SendMessage(WM_CLOSE);
}

int CSettingFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	//GetClientRect(rectDummy);
	rectDummy.SetRectEmpty();
	
	// 创建一个视图以占用框架的工作区
	///*
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,rectDummy, this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("未能创建视图窗口\n");
		return -1;
	}
	//*/

	///*
	if (!m_wndShowList.Create(WS_VISIBLE | WS_CHILD | LVS_REPORT  , rectDummy, this, 2))
	{
		TRACE0("未能创建定值列表\n");
		return -1;      // 未能创建
	}

	m_wndShowList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_wndShowList.ModifyStyleEx(0,WS_EX_CLIENTEDGE);
	//m_wndShowList.EnableMarkSortedColumn();
	m_wndShowList.EnableMultipleSort();
	//*/
	
	//AdjustLayout();
	//InitSettingFrmList();

	return 0;
}

void CSettingFrame::RefreshSettingList(int stationIndex,int devIndex)
{
	if (stationIndex < 0 || stationIndex >= theApp.g_sDataBases.size())
	{
		return;
	}

	if (devIndex < 0 || devIndex >= theApp.g_sDataBases[stationIndex].vDevs.size())
	{
		return;
	}

	m_iStationIndex = stationIndex;
	m_iDevIndex = devIndex;

	m_wndShowList.DeleteAll();

	if (!theApp.g_sDataBases[stationIndex].vDevs[devIndex].setting.getbInitStruct())
	{
		LoadSettingCFG(stationIndex,devIndex);
	}

	CItemInfo * lpItemInfo;

	CSuperGridCtrl::CTreeItem* pAISetting = NULL;
	CSuperGridCtrl::CTreeItem* pDISetting = NULL;
	CSuperGridCtrl::CTreeItem* pFreSetting = NULL;
	CSuperGridCtrl::CTreeItem* pSeqSetting = NULL;

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("装置定值"));
	lpItemInfo->setItemType(1);
	//Create root item
	pRoot = m_wndShowList.InsertRootItem(lpItemInfo);             //previous on N.Y.P.D we call it CreateTreeCtrl(lp)
	if( pRoot == NULL )
	{
		return;
	}

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetImage(1);
	lpItemInfo->SetItemText(_T("模拟量定值"));
	lpItemInfo->setItemType(AI_ITEM_TYPE);
	lpItemInfo->AddSubItemText(_T("批处理"));
	lpItemInfo->SetControlType(lpItemInfo->CONTROLTYPE::spinbutton,0);
	pAISetting = m_wndShowList.InsertItem(pRoot,lpItemInfo);
	InitAISettingList(pAISetting);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetImage(2);
	lpItemInfo->SetItemText(_T("频率定值"));
	lpItemInfo->setItemType(FRE_ITEM_TYPE);
	lpItemInfo->AddSubItemText(_T("批处理"));
	lpItemInfo->SetControlType(lpItemInfo->CONTROLTYPE::spinbutton,0);
	pFreSetting = m_wndShowList.InsertItem(pRoot,lpItemInfo);
	InitFreSettingList(pFreSetting);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetImage(3);
	lpItemInfo->SetItemText(_T("序量定值"));
	lpItemInfo->setItemType(SEQ_ITEM_TYPE);
	lpItemInfo->AddSubItemText(_T("批处理"));
	lpItemInfo->SetControlType(lpItemInfo->CONTROLTYPE::spinbutton,0);
	pSeqSetting = m_wndShowList.InsertItem(pRoot,lpItemInfo);
	InitSeqSettingList(pSeqSetting);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetImage(4);
	lpItemInfo->SetItemText(_T("开关量定值"));
	lpItemInfo->setItemType(DI_ITEM_TYPE);
	lpItemInfo->AddSubItemText(_T("批处理"));
	lpItemInfo->SetControlType(lpItemInfo->CONTROLTYPE::spinbutton,0);
	pDISetting = m_wndShowList.InsertItem(pRoot,lpItemInfo);
	InitDISettingList(pDISetting);

	m_wndShowList.Expand(pRoot,0);
	//m_wndShowList.ExpandUntil(pRoot,pDISetting);
}

void CSettingFrame::AdjustLayout(void)
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect frameClient;
	CRect viewClient;
	GetClientRect(frameClient);
	m_wndView.GetClientRect(viewClient);
	m_wndShowList.SetWindowPos(NULL, frameClient.left, frameClient.top, frameClient.Width(), frameClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CSettingFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWndEx::OnSetFocus(pOldWnd);

	m_wndView.SetFocus();
}

BOOL CSettingFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// 让视图第一次尝试该命令
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	{
		return TRUE;
	}
	
	// 否则，执行默认处理
	return CMDIChildWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
int CSettingFrame::getSelectedFileIndex(void)
{
	if (m_wndShowList.GetItemCount() > 0)
	{
		POSITION pos = m_wndShowList.GetFirstSelectedItemPosition();
		while (pos)
		{
				int nItem = m_wndShowList.GetNextSelectedItem(pos);
				return nItem;
		}
	}

	return -1;
}
int CSettingFrame::InitAISettingList(CSuperGridCtrl::CTreeItem* pParent)
{
	CItemInfo * lpItemInfo;
	CString strTmp;
	CSuperGridCtrl::CTreeItem * ai_channel;

	for (int i=0;i<theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getAIChannelSum();i++)
	{
		lpItemInfo = new CItemInfo();
		WCHAR * wchar = ChartoWCHAR(theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getAIChannelName(i));
		strTmp = wchar;
		delete wchar;
		lpItemInfo->SetItemText(strTmp);
		strTmp.Format(_T("序号 %.2d"),i);
		lpItemInfo->AddSubItemText(strTmp,false);
		lpItemInfo->setItemType(AI_VAL_ITEM);
		ai_channel = m_wndShowList.InsertItem(pParent,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("通道类型"));
		switch (theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getChannelType(i))
		{
		case CHANNEL_AC_I:
			lpItemInfo->AddSubItemText(_T(STR_CHANNEL_AC_I));
			break;

		case CHANNEL_AC_U:
			lpItemInfo->AddSubItemText(_T(STR_CHANNEL_AC_U));
			break;

		case CHANNEL_DC_I:
			lpItemInfo->AddSubItemText(_T(STR_CHANNEL_DC_I));
			break;

		case CHANNEL_DC_U:
			lpItemInfo->AddSubItemText(_T(STR_CHANNEL_DC_U));
			break;

		default:
			lpItemInfo->AddSubItemText(_T(STR_CHANNEL_OTHER));
			break;
		}
		lpItemInfo->SetControlType(lpItemInfo->CONTROLTYPE::combobox,0);
		lpItemInfo->SetListData(0,&m_strListChannelType);
		m_wndShowList.InsertItem(ai_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("变比"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getChannelPtCt(i));
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(ai_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("上限"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getChannelUpperLimit(i));
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(ai_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("下限"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getChannelLowerLimit(i));
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(ai_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("突变"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getChannelAmplitudeChange(i));
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(ai_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("缓变"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getChannelSlowAmplitudeChange(i));
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(ai_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("零门槛"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getChannelAmplitudeNull(i));
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(ai_channel,lpItemInfo);
	}

	//m_wndShowList.Expand(pParent,0);

	return 0;
}

int CSettingFrame::InitDISettingList(CSuperGridCtrl::CTreeItem* pParent)
{
	CItemInfo * lpItemInfo;
	CString strTmp;
	CSuperGridCtrl::CTreeItem * di_channel;

	for (int i=0;i<theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getDIChannelSum();i++)
	{
		lpItemInfo = new CItemInfo();
		WCHAR * wchar = ChartoWCHAR(theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getDIChannelName(i));
		strTmp = wchar;
		delete wchar;
		lpItemInfo->SetItemText(strTmp);
		strTmp.Format(_T("序号 %.3d"),i);
		lpItemInfo->AddSubItemText(strTmp,false);
		lpItemInfo->setItemType(DI_VAL_ITEM);
		di_channel = m_wndShowList.InsertItem(pParent,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T(" 合->分 启动"));
		int di_val = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getDINegativeChangeVal(i);
		if(di_val == 1)
		{
			lpItemInfo->AddSubItemText(_T(STR_DI_YES));
		}
		else
		{
			lpItemInfo->AddSubItemText(_T(STR_DI_NO));
		}
		lpItemInfo->SetControlType(lpItemInfo->CONTROLTYPE::datecontrol,0);
		lpItemInfo->SetListData(0,&m_strListDIChoose);
		m_wndShowList.InsertItem(di_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T(" 分->合 启动"));
		di_val = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getDIPositiveChangeVal(i);
		if(di_val == 1)
		{
			lpItemInfo->AddSubItemText(_T(STR_DI_YES));
		}
		else
		{
			lpItemInfo->AddSubItemText(_T(STR_DI_NO));
		}
		lpItemInfo->SetControlType(lpItemInfo->CONTROLTYPE::datecontrol,0);
		lpItemInfo->SetListData(0,&m_strListDIChoose);
		m_wndShowList.InsertItem(di_channel,lpItemInfo);
	}

	//m_wndShowList.Expand(pParent,0);

	return 0;
}

int CSettingFrame::InitFreSettingList(CSuperGridCtrl::CTreeItem* pParent)
{
	CItemInfo * lpItemInfo;
	CString strTmp;
	CSuperGridCtrl::CTreeItem * fre_channel;

	for (int i=0;i<1;i++)
	{
		lpItemInfo = new CItemInfo();
		strTmp = _T("频率");
		lpItemInfo->SetItemText(strTmp);
		strTmp.Format(_T("序号 %.2d"),0);
		lpItemInfo->AddSubItemText(strTmp,false);
		lpItemInfo->setItemType(FRE_VAL_ITEM);
		fre_channel = m_wndShowList.InsertItem(pParent,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("上限"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getFreqUpperLimit());
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(fre_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("下限"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getFreqLowerLimit());
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(fre_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("高门槛"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getFreqUpperDisLimit());
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(fre_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("低门槛"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getFreqLowerDisLimit());
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(fre_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("变化率"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getFreqChangeRate());
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(fre_channel,lpItemInfo);
	}
		
	//m_wndShowList.Expand(pParent,0);

	return 0;
}

int CSettingFrame::InitSeqSettingList(CSuperGridCtrl::CTreeItem* pParent)
{
	CItemInfo * lpItemInfo;
	CString strTmp;
	CSuperGridCtrl::CTreeItem * group_channel;

	for (int i=0;i<theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getGroupSum();i++)
	{
		lpItemInfo = new CItemInfo();
		WCHAR * wchar = ChartoWCHAR(theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getGroupName(i));
		strTmp = wchar;
		delete wchar;
		lpItemInfo->SetItemText(strTmp);
		strTmp.Format(_T("组号 %.2d"),i);
		lpItemInfo->AddSubItemText(strTmp,false);
		lpItemInfo->setItemType(SEQ_VAL_ITEM);
		group_channel = m_wndShowList.InsertItem(pParent,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("正序上限"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getPosiUpperLimit(i));
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(group_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("正序下限"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getPosiLowerLimit(i));
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(group_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("正序零门槛"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getPosiNull(i));
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(group_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("负序上限"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getNegaUpperLimit(i));
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(group_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("零序上限"));
		strTmp.Format(_T("%.3f"),theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getZeroUpperLimit(i));
		lpItemInfo->AddSubItemText(strTmp,true);
		m_wndShowList.InsertItem(group_channel,lpItemInfo);

		lpItemInfo = new CItemInfo();
		lpItemInfo->SetItemText(_T("组成员"));
		lpItemInfo->AddSubItemText(_T("成员序号"));
		CSuperGridCtrl::CTreeItem * group_member = m_wndShowList.InsertItem(group_channel,lpItemInfo);
		for (int j=0;j<MAX_GROUP_MEMBERS;j++)
		{
			lpItemInfo = new CItemInfo();
			strTmp.Format(_T("成员 %d"),j);
			lpItemInfo->SetItemText(strTmp);
			int memberID = (theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getGroupIDs(i))[j];
			if (memberID < 0 || memberID > MAX_AI_SUM)
			{
				strTmp = _T("NULL");
			}
			else
			{
				strTmp.Format(_T("%.2d"),memberID);
			}			
			lpItemInfo->AddSubItemText(strTmp,true);
			m_wndShowList.InsertItem(group_member,lpItemInfo);
		}
	}

	//m_wndShowList.Expand(pParent,0);

	return 0;
}

void CSettingFrame::InitSettingFrmList(int stationIndex,int devIndex)
{
	//m_wndShowList.DeleteAll();

	int count = m_wndShowList.GetNumCol();
	for (int i=0;i<count;i++)
	{
		m_wndShowList.DeleteColumn(0);
	}

	CRect rectClient;
	m_wndShowList.GetClientRect(&rectClient);

	LPTSTR lpszCols[] = {_T("名称"),_T("值域"),_T("限定"),_T("属性"),_T("备注"),_T("")};
	LV_COLUMN   lvColumn;
	//initialize the columns
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	int narryWeight[] = {rectClient.Width() / 4,rectClient.Width() / 4,rectClient.Width() / 4,rectClient.Width() / 9,(rectClient.Width() - rectClient.Width() / 4 - rectClient.Width() / 4 -rectClient.Width() / 4 - rectClient.Width() / 9)};

	for(int i = 0; wcslen(lpszCols[i])!=0; i++)
	{
		//make the secondary columns smaller
		lvColumn.cx = narryWeight[i];
		lvColumn.pszText = lpszCols[i];
		m_wndShowList.InsertColumn(i,&lvColumn);
	}

	RefreshSettingList(stationIndex,devIndex);
}

void CSettingFrame::RecalcLayout(void)
{
	CRect rectClient;
	m_wndShowList.GetClientRect(&rectClient);

	LPTSTR lpszCols[] = {_T("名称"),_T("值域"),_T("限定"),_T("属性"),_T("备注"),_T("")};
	int narryWeight[] = {rectClient.Width() / 4,rectClient.Width() / 4,rectClient.Width() / 4,rectClient.Width() / 9,(rectClient.Width() - rectClient.Width() / 4 - rectClient.Width() / 4 -rectClient.Width() / 4 - rectClient.Width() / 9)};

	for(int i = 0; wcslen(lpszCols[i])!=0; i++)
	{
		m_wndShowList.SetColumnWidth(i,narryWeight[i]);
	}
}

int CSettingFrame::SaveSettingCFG(int StationIndex, int DevIndex)
{
	if (StationIndex < 0 || StationIndex >= theApp.g_sDataBases.size())
	{
		return -1;
	}

	if (DevIndex < 0 || DevIndex >= theApp.g_sDataBases[StationIndex].vDevs.size())
	{
		return -1;
	}

	CString fileName = theApp.g_sDataBases.getRootCatalog();
	fileName += theApp.g_sDataBases[StationIndex].getStationCatalog();
	fileName += theApp.g_sDataBases[StationIndex].vDevs.getDevsCatalog();
	fileName += theApp.g_sDataBases[StationIndex].vDevs[DevIndex].getDevCatalog();
	fileName += theApp.g_sDataBases[StationIndex].vDevs[DevIndex].setting.getSettingCatalog();
	fileName += _T("\\");
	fileName += _T(STR_SETCFG_NAME);

	return theApp.g_sDataBases[StationIndex].vDevs[DevIndex].setting.SaveSettingCFG(fileName);
}

int CSettingFrame::LoadSettingCFG(int StationIndex, int DevIndex)
{
	if (StationIndex < 0 || StationIndex >= theApp.g_sDataBases.size())
	{
		return -1;
	}

	if (DevIndex < 0 || DevIndex >= theApp.g_sDataBases[StationIndex].vDevs.size())
	{
		return -1;
	}

	CString fileName = theApp.g_sDataBases.getRootCatalog();
	fileName += theApp.g_sDataBases[StationIndex].getStationCatalog();
	fileName += theApp.g_sDataBases[StationIndex].vDevs.getDevsCatalog();
	fileName += theApp.g_sDataBases[StationIndex].vDevs[DevIndex].getDevCatalog();
	fileName += theApp.g_sDataBases[StationIndex].vDevs[DevIndex].setting.getSettingCatalog();
	fileName += _T("\\");
	fileName += _T(STR_SETCFG_NAME);

	if (FileExist(fileName))
	{
		return theApp.g_sDataBases[StationIndex].vDevs[DevIndex].setting.LoadSettingCFG(fileName);
	}
	else
	{
		return -1;
	}
	
}
void CSettingFrame::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	CListCtrl* pWndList = (CListCtrl*)&m_wndShowList;
	ASSERT_VALID(pWndList);

	if (pWnd != pWndList)
	{
		CMDIChildWndEx::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptList = point;
		pWndList->ScreenToClient(&ptList);

		UINT flags = 0;
		int index = pWndList->HitTest(ptList, &flags);
		if (index > 0)
		{
			pWndList->SetItemState(index,LVIS_SELECTED,LVIS_SELECTED);
		}
	}

	pWndList->SetFocus();

	CMenu menu;

	menu.LoadMenu(IDR_POPUP_SETTINGFRM);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
		{
			return;
		}

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}
}

void CSettingFrame::OnPopSetting(UINT id)
{
	switch(id)
	{
	case ID_SETTING_COPY:
		OnCopySettingVal();
		break;

	case ID_SETTING_PASTE:
		OnPasteSettingVal();
		break;

	case ID_EXPAND_ALL:
		OnExpandAll();
		break;

	case ID_COLLAPSE_ALL:
		OnCollapseAll();
		break;

	default:
		break;
	}
}

void CSettingFrame::OnUpdatePopSetting(CCmdUI* pCmdUI)
{
	int itemType = m_wndShowList.getSelectedTreeItemType();
	
	switch(pCmdUI->m_nID)
	{
	case ID_SETTING_COPY:
		switch (itemType)
		{
		case AI_VAL_ITEM:
			pCmdUI->Enable(true);
			break;

		case DI_VAL_ITEM:
			pCmdUI->Enable(true);
			break;

		case FRE_VAL_ITEM:
			pCmdUI->Enable(false);
			break;

		case SEQ_VAL_ITEM:
			pCmdUI->Enable(true);
			break;

		default:
			pCmdUI->Enable(false);
			break;
		}
		break;

	case ID_SETTING_PASTE:
		if (copyInfo.bCopying && itemType == copyInfo.ItemType)
		{
			pCmdUI->Enable(true);
		}
		else
		{
			pCmdUI->Enable(false);
		}
		break;

	case ID_EXPAND_ALL:
		pCmdUI->Enable(true);
		break;

	case ID_COLLAPSE_ALL:
		pCmdUI->Enable(true);
		break;

	default:
		break;
	}
}

void CSettingFrame::OnOpenLocalMenu()
{
	if(m_iStationIndex < 0 || m_iStationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站序号错误，重新选择子站");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
	
		return;
	}

	if (m_iDevIndex < 0 || m_iDevIndex >= theApp.g_sDataBases[m_iStationIndex].vDevs.size())
	{
		CString str = _T("装置序号错误，重新选择装置");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);
		return;
	}

	CString path;
	path += theApp.g_sDataBases.getRootCatalog();
	path += theApp.g_sDataBases[m_iStationIndex].getStationCatalog();
	path += theApp.g_sDataBases[m_iStationIndex].vDevs.getDevsCatalog();
	path += theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].getDevCatalog();
	path += theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.getSettingCatalog();

	if (!FolderExist(path))
	{
		BuildDirectory(path);
	}

	ShellExecute(NULL, _T("open"), path,0, 0, SW_SHOWNORMAL);
}

void CSettingFrame::OnUpdateLocalMenu(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(true);
}

void CSettingFrame::OnCopySettingVal()
{
	int itemType = m_wndShowList.getSelectedTreeItemType();
	if (itemType == AI_VAL_ITEM || itemType == DI_VAL_ITEM || itemType == SEQ_VAL_ITEM)
	{
		copyInfo.strVal.RemoveAll();

		CSuperGridCtrl::CTreeItem * curTreeItem = m_wndShowList.GetTreeItem(m_wndShowList.GetSelectedItem());
		if (curTreeItem != NULL)
		{
			const int nChildren = m_wndShowList.NumChildren(curTreeItem);
			if (nChildren > 0)
			{
				POSITION pos = m_wndShowList.GetHeadPosition(curTreeItem);
				while (pos)
				{
					CSuperGridCtrl::CTreeItem *pChild = m_wndShowList.GetNextChild(curTreeItem, pos);
					CItemInfo* lp = m_wndShowList.GetData(pChild);
					CString strData = lp->GetSubItem(0);
					copyInfo.strVal.AddTail(strData);
				}
			}

			copyInfo.ItemType = itemType;
			copyInfo.bCopying = true;
		}
	}
}

void CSettingFrame::OnPasteSettingVal()
{
	int strCount = copyInfo.strVal.GetCount();
	if (copyInfo.bCopying && strCount > 0)
	{
		int itemType = m_wndShowList.getSelectedTreeItemType();
		if (itemType != copyInfo.ItemType)
		{
			CString str = _T("拷贝项目类型和粘贴类型不一致");
			AfxMessageBox(str);
			((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

			return;
		}

		CSuperGridCtrl::CTreeItem * curTreeItem = m_wndShowList.GetTreeItem(m_wndShowList.GetSelectedItem());
		if (curTreeItem != NULL)
		{
			const int nChildren = m_wndShowList.NumChildren(curTreeItem);
			if (nChildren > 0)
			{
				POSITION pos = m_wndShowList.GetHeadPosition(curTreeItem);
				POSITION strPos = copyInfo.strVal.GetHeadPosition();
				for (int i = 0;(i < (nChildren ? strCount : nChildren < strCount))&&(pos && strPos); i++)
				{
					CSuperGridCtrl::CTreeItem *pChild = m_wndShowList.GetNextChild(curTreeItem, pos);
					CString strData = copyInfo.strVal.GetNext(strPos);
					CItemInfo* lp = m_wndShowList.GetData(pChild);
					lp->SetSubItemText(0,strData,true);
					m_wndShowList.Collapse(curTreeItem);
				}
			}			
		}
	}
}

void CSettingFrame::OnExpandAll()
{
	int nScroll;
	m_wndShowList.ExpandAll(pRoot,nScroll);
}

void CSettingFrame::OnCollapseAll()
{
	m_wndShowList.Collapse(pRoot);
}

void CSettingFrame::OnRefresh()
{
	RefreshSettingList(m_iStationIndex,m_iDevIndex);
}

void CSettingFrame::OnUpdateRefresh(CCmdUI * pCmdUI)
{
	pCmdUI->Enable(true);
}

LRESULT CSettingFrame::OnPatchProcessMsg(WPARAM wParam, LPARAM lParam)
{
	int type = m_wndShowList.getSelectedTreeItemType();
	CPatchDlg dlg(type);
	
	if(dlg.DoModal() == IDOK)
	{
		if (dlg.startIndex > dlg.endIndex)
		{
			CString str = _T("起止点号有误");
			AfxMessageBox(str);
			((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

			return 0L;
		}

		int start = dlg.startIndex;
		int end = dlg.endIndex;

		if (start < 0)
		{
			start = 0;
		}
		if (end >= MAX_DI_SUM)
		{
			end = MAX_DI_SUM - 1;
		}

		int strCount = dlg.strVal.GetCount();
		int itemCount = end - start;
		if (strCount > 0 && itemCount > 0)
		{
			CSuperGridCtrl::CTreeItem * curTreeItem = (CSuperGridCtrl::CTreeItem *)wParam;
			if (curTreeItem != NULL)
			{
				int nChildren = m_wndShowList.NumChildren(curTreeItem);
				if (nChildren <= start)
				{
					CString str = _T("起点号越界");
					AfxMessageBox(str);
					((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

					return 0L;
				}

				int countChild = 0;
				POSITION pos = m_wndShowList.GetHeadPosition(curTreeItem);
				while ((countChild <= end) && pos)
				{
					CSuperGridCtrl::CTreeItem * pParent = m_wndShowList.GetNextChild(curTreeItem, pos);
					if (countChild >= start && countChild <= end)
					{
						int nGrandChildren = m_wndShowList.NumChildren(pParent);
						if (nGrandChildren > 0)
						{
							POSITION childPos = m_wndShowList.GetHeadPosition(pParent);
							POSITION strPos = dlg.strVal.GetHeadPosition();
							for (int i = 0;(i < (nGrandChildren ? strCount : nGrandChildren < strCount))&&(childPos && strPos); i++)
							{
								CSuperGridCtrl::CTreeItem *pChild = m_wndShowList.GetNextChild(pParent, childPos);
								CString strData = dlg.strVal.GetNext(strPos);
								CItemInfo* lp = m_wndShowList.GetData(pChild);
								lp->SetSubItemText(0,strData,true);
								m_wndShowList.Collapse(pParent);
							}
						}
					}
		
					countChild++;
				}
			}
		}

	}

	return 0L;
}

CPatchDlg::CPatchDlg() : CDialog(CPatchDlg::IDD)
{
	startIndex = 0;
	endIndex = 0;

	for (int i=0;i<m_iChannelTypeSum;i++)
	{
		m_strListChannelType.AddTail(m_strChannelType[i]);
	}

	for (int i=0;i<m_iDIChoose;i++)
	{
		m_strListDIChoose.AddTail(m_strDIChoose[i]);
	}

	ListType = -1;
}

CPatchDlg::CPatchDlg(int type) : CDialog(CPatchDlg::IDD)
{
	startIndex = 0;
	endIndex = 0;

	for (int i=0;i<m_iChannelTypeSum;i++)
	{
		m_strListChannelType.AddTail(m_strChannelType[i]);
	}

	for (int i=0;i<m_iDIChoose;i++)
	{
		m_strListDIChoose.AddTail(m_strDIChoose[i]);
	}

	ListType = type;
}

void CPatchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_START, startIndex);
	DDX_Text(pDX, IDC_EDIT_END, endIndex);
	DDV_MinMaxUInt(pDX, startIndex, 0, MAX_DI_SUM - 1);
	DDV_MinMaxUInt(pDX, endIndex, 0, MAX_DI_SUM - 1);
}

BEGIN_MESSAGE_MAP(CPatchDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPatchDlg::OnBnClickedOk)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDCANCEL, &CPatchDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

void CPatchDlg::OnBnClickedOk()
{
	strVal.RemoveAll();

	CSuperGridCtrl::CTreeItem * curTreeItem = m_wndShowList.GetRootItem(0);
	if (curTreeItem != NULL)
	{
		const int nChildren = m_wndShowList.NumChildren(curTreeItem);
		if (nChildren > 0)
		{
			POSITION pos = m_wndShowList.GetHeadPosition(curTreeItem);
			while (pos)
			{
				CSuperGridCtrl::CTreeItem *pChild = m_wndShowList.GetNextChild(curTreeItem, pos);
				CItemInfo* lp = m_wndShowList.GetData(pChild);
				CString strData = lp->GetSubItem(0);
				strVal.AddTail(strData);
			}
		}
	}

	OnOK();
}

int CPatchDlg::InitShowList(int type)
{
	int ret = 0;

	//m_wndShowList.DeleteAll();

	int count = m_wndShowList.GetNumCol();
	for (int i=0;i<count;i++)
	{
		m_wndShowList.DeleteColumn(0);
	}

	CRect rectClient;
	m_wndShowList.GetClientRect(&rectClient);

	LPTSTR lpszCols[] = {_T("名称"),_T("值域"),_T("限定"),_T("")};
	LV_COLUMN   lvColumn;
	//initialize the columns
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;
	int narryWeight[] = {rectClient.Width() / 3,rectClient.Width() / 3,(rectClient.Width() - rectClient.Width() / 3 - rectClient.Width() / 3)};

	for(int i = 0; wcslen(lpszCols[i])!=0; i++)
	{
		//make the secondary columns smaller
		lvColumn.cx = narryWeight[i];
		lvColumn.pszText = lpszCols[i];
		m_wndShowList.InsertColumn(i,&lvColumn);
	}

	switch (type)
	{
	case AI_ITEM_TYPE:
		ret = InitAIShowList();
		break;

	case DI_ITEM_TYPE:
		ret = InitDIShowList();
		break;

	case FRE_ITEM_TYPE:
		ret = InitFREShowList();
		break;

	case SEQ_ITEM_TYPE:
		ret = InitSEQShowList();
		break;

	default:
		ret = -1;
		break;
	}

	return ret;
}

int CPatchDlg::InitAIShowList()
{
	CItemInfo * lpItemInfo;
	CSuperGridCtrl::CTreeItem * ai_channel;

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("模拟量通道"));
	ai_channel = m_wndShowList.InsertRootItem(lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("通道类型"));
	lpItemInfo->AddSubItemText(_T(STR_CHANNEL_OTHER));
	lpItemInfo->SetControlType(lpItemInfo->CONTROLTYPE::combobox,0);
	lpItemInfo->SetListData(0,&m_strListChannelType);
	m_wndShowList.InsertItem(ai_channel,lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("变比"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(ai_channel,lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("上限"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(ai_channel,lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("下限"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(ai_channel,lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("突变"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(ai_channel,lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("缓变"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(ai_channel,lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("零门槛"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(ai_channel,lpItemInfo);

	int nScroll;
	m_wndShowList.ExpandAll(ai_channel,nScroll);

	return 0;
}

int CPatchDlg::InitDIShowList()
{
	CItemInfo * lpItemInfo;
	CSuperGridCtrl::CTreeItem * di_channel;

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("开关量通道"));
	di_channel = m_wndShowList.InsertRootItem(lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T(" 合->分 启动"));
	lpItemInfo->SetControlType(lpItemInfo->CONTROLTYPE::datecontrol,0);
	lpItemInfo->SetListData(0,&m_strListDIChoose);
	lpItemInfo->AddSubItemText(_T(STR_DI_NO));
	m_wndShowList.InsertItem(di_channel,lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T(" 分->合 启动"));
	lpItemInfo->SetControlType(lpItemInfo->CONTROLTYPE::datecontrol,0);
	lpItemInfo->SetListData(0,&m_strListDIChoose);
	lpItemInfo->AddSubItemText(_T(STR_DI_NO));
	m_wndShowList.InsertItem(di_channel,lpItemInfo);

	int nScroll;
	m_wndShowList.ExpandAll(di_channel,nScroll);

	return 0;
}

int CPatchDlg::InitFREShowList()
{
	CItemInfo * lpItemInfo;
	CSuperGridCtrl::CTreeItem * fre_channel;

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("频率"));
	fre_channel = m_wndShowList.InsertRootItem(lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("上限"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(fre_channel,lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("下限"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(fre_channel,lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("高门槛"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(fre_channel,lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("低门槛"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(fre_channel,lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("变化率"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(fre_channel,lpItemInfo);

	int nScroll;
	m_wndShowList.ExpandAll(fre_channel,nScroll);

	return 0;
}

int CPatchDlg::InitSEQShowList()
{
	CItemInfo * lpItemInfo;
	CSuperGridCtrl::CTreeItem * group_channel;

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("序量通道"));
	group_channel = m_wndShowList.InsertRootItem(lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("正序上限"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(group_channel,lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("正序下限"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(group_channel,lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("正序零门槛"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(group_channel,lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("负序上限"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(group_channel,lpItemInfo);

	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("零序上限"));
	lpItemInfo->AddSubItemText(NULL,true);
	m_wndShowList.InsertItem(group_channel,lpItemInfo);

	/*
	lpItemInfo = new CItemInfo();
	lpItemInfo->SetItemText(_T("组成员"));
	lpItemInfo->AddSubItemText(_T("成员序号"));
	CSuperGridCtrl::CTreeItem * group_member = m_wndShowList.InsertItem(group_channel,lpItemInfo);
	CString strTmp;
	for (int j=0;j<MAX_GROUP_MEMBERS;j++)
	{
		lpItemInfo = new CItemInfo();
		strTmp.Format(_T("成员 %d"),j);
		lpItemInfo->SetItemText(strTmp);
		lpItemInfo->AddSubItemText(NULL,true);
		m_wndShowList.InsertItem(group_member,lpItemInfo);
	}
	*/

	int nScroll;
	m_wndShowList.ExpandAll(group_channel,nScroll);

	return 0;
}

int CPatchDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect rectDummy;
	//GetClientRect(rectDummy);
	rectDummy.SetRectEmpty();

	if (!m_wndShowList.Create(WS_VISIBLE | WS_CHILD | LVS_REPORT ,rectDummy, this, 2))
	{
		TRACE0("未能创建定值列表\n");
		return -1;      // 未能创建
	}

	m_wndShowList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_wndShowList.ModifyStyleEx(0,WS_EX_CLIENTEDGE);
	//m_wndShowList.EnableMarkSortedColumn();
	m_wndShowList.EnableMultipleSort();

	AdjustLayout();
	if (InitShowList(ListType))
	{
		CString str = _T("初始化批处理列表失败");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return -1;
	}

	return 0;
}

void CPatchDlg::AdjustLayout(void)
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect frameClient;
	CRect buttonClient;
	GetClientRect(frameClient);
	CButton * bTmp = ((CButton *)GetDlgItem(IDCANCEL));
	bTmp->GetWindowRect(buttonClient);
	//ScreenToClient(buttonClient);
	m_wndShowList.SetWindowPos(NULL, frameClient.left, frameClient.top, frameClient.Width(), frameClient.Height() - buttonClient.top - 10, SWP_NOACTIVATE | SWP_NOZORDER);
}
void CPatchDlg::OnBnClickedCancel()
{
	strVal.RemoveAll();
	OnCancel();
}

int CSettingFrame::SaveSettingVal( int StationIndex,int DevIndex )
{
	int ret = 0;

	if (!(StationIndex == m_iStationIndex))
	{
		CString str = _T("保存的子站序号与打开的子站序号不一致，重新选择子站");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return -1;
	}

	if(!(DevIndex == m_iDevIndex))
	{
		CString str = _T("保存的子站序号与打开的子站序号不一致，重新选择子站");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return -1;
	}

	if(m_iStationIndex < 0 || m_iStationIndex >= theApp.g_sDataBases.size())
	{
		CString str = _T("子站序号错误，重新选择子站");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return -1;
	}

	if (m_iDevIndex < 0 || m_iDevIndex >= theApp.g_sDataBases[m_iStationIndex].vDevs.size())
	{
		CString str = _T("装置序号错误，重新选择装置");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return -1;
	}

	if (pRoot == NULL)
	{
		CString str = _T("找不到树根");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return -1;
	}

	int nChildren = m_wndShowList.NumChildren(pRoot);
	if (nChildren != 4)
	{
		CString str = _T("树根下孩子数目与预期不符");
		AfxMessageBox(str);
		((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(str);

		return -1;
	}

	CSuperGridCtrl::CTreeItem * pParent = NULL;
	POSITION pos = m_wndShowList.GetHeadPosition(pRoot);
	int countChild = 0;
	CString err;
	while (pos && countChild < 4)
	{
		pParent = m_wndShowList.GetNextChild(pRoot, pos);
		if (pParent !=NULL)
		{
			switch (countChild)
			{
			case 0:
				try
				{
					SaveAISettingVal(pParent);
				}
				catch(CPoint p)
				{
					if(p.y >= 0)
					{
						err.Format(_T("模拟量第%d通道第%d个定值错误"),p.x,p.y);
					}
					else
					{
						err.Format(_T("模拟量第%d通道未知的定值编号"),p.x);
					}
					
					AfxMessageBox(err);
					((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(err);
					return -1;
				}
				break;

			case 1:
				try
				{
					SaveFreSettingVal(pParent);
				}
				catch(CPoint p)
				{
					if(p.y >= 0)
					{
						err.Format(_T("频率第%d通道第%d个定值错误"),p.x,p.y);
					}
					else
					{
						err.Format(_T("频率第%d通道未知的定值编号"),p.x);
					}
					AfxMessageBox(err);
					((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(err);
					return -1;
				}
				break;

			case 2:
				try
				{
					SaveSeqSettingVal(pParent);
				}
				catch(CPoint p)
				{
					if(p.y >= 0)
					{
						err.Format(_T("序量第%d通道第%d个定值错误"),p.x,p.y);
					}
					else
					{
						err.Format(_T("序量第%d通道未知的定值编号"),p.x);
					}
					AfxMessageBox(err);
					((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(err);
					return -1;
				}
				break;

			case 3:
				try
				{
					SaveDISettingVal(pParent);
				}
				catch(CPoint p)
				{
					if(p.y >= 0)
					{
						err.Format(_T("开关量第%d通道第%d个定值错误"),p.x,p.y);
					}
					else
					{
						err.Format(_T("开关量第%d通道未知的定值编号"),p.x);
					}
					AfxMessageBox(err);
					((CMainFrame *)AfxGetMainWnd())->AddOutputWarn(err);
					return -1;
				}
				break;

			default:
				break;
			}
		}
		countChild++;
	}
	
	return ret;
}

int CSettingFrame::SaveAISettingVal(CSuperGridCtrl::CTreeItem* pParent)
{
	int ret = 0;

	int nChildren = m_wndShowList.NumChildren(pParent);
	if (nChildren > 0)
	{
		POSITION pos = m_wndShowList.GetHeadPosition(pParent);
		int countChild = 0;
		while (pos)
		{
			CSuperGridCtrl::CTreeItem * pChild = m_wndShowList.GetNextChild(pParent, pos);
			if (pChild != NULL)
			{
				POSITION valPos = m_wndShowList.GetHeadPosition(pChild);
				int countVal = 0;
				while (valPos)
				{
					CSuperGridCtrl::CTreeItem * pVal = m_wndShowList.GetNextChild(pChild,valPos);
					if (pVal != NULL)
					{
						CItemInfo* lp = m_wndShowList.GetData(pVal);
						CString strData = lp->GetSubItem(0);
						float fVal;
						switch (countVal)
						{
						case 0:
							if (strData.Compare(_T(STR_CHANNEL_AC_I)) == 0)
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setChannelType(countChild,CHANNEL_AC_I);
							}
							else if (strData.Compare(_T(STR_CHANNEL_AC_U)) == 0)
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setChannelType(countChild,CHANNEL_AC_U);
							}
							else if (strData.Compare(_T(STR_CHANNEL_DC_U)) == 0)
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setChannelType(countChild,CHANNEL_DC_U);
							}
							else if (strData.Compare(_T(STR_CHANNEL_DC_I)) == 0)
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setChannelType(countChild,CHANNEL_DC_I);
							}
							else
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setChannelType(countChild,CHANNEL_OTHER);
							}

							if (ret)
							{
								throw(CPoint(countChild,countVal));
							}
							break;

						case 1:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setChannelPtCt(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						case 2:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setChannelUpperLimit(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						case 3:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setChannelLowerLimit(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						case 4:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setChannelAmplitudeChange(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						case 5:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setChannelSlowAmplitudeChange(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						case 6:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setChannelAmplitudeNull(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						default:
							throw(CPoint(countChild,-1));
							break;
						}
					}

					countVal++;

				}
			}

			countChild++;
		}
	}

	return ret;
}

int CSettingFrame::SaveDISettingVal(CSuperGridCtrl::CTreeItem* pParent)
{
	int ret = 0;

	int nChildren = m_wndShowList.NumChildren(pParent);
	if (nChildren > 0)
	{
		POSITION pos = m_wndShowList.GetHeadPosition(pParent);
		int countChild = 0;
		while (pos)
		{
			CSuperGridCtrl::CTreeItem * pChild = m_wndShowList.GetNextChild(pParent, pos);
			if (pChild != NULL)
			{
				POSITION valPos = m_wndShowList.GetHeadPosition(pChild);
				int countVal = 0;
				while (valPos)
				{
					CSuperGridCtrl::CTreeItem * pVal = m_wndShowList.GetNextChild(pChild,valPos);
					if (pVal != NULL)
					{
						CItemInfo* lp = m_wndShowList.GetData(pVal);
						CString strData = lp->GetSubItem(0);
						switch (countVal)
						{
						case 0:
							if (strData.Compare(_T(STR_DI_YES)) == 0)
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setDINegativeChangeVal(countChild,true);
							}
							else if (strData.Compare(_T(STR_DI_NO)) == 0)
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setDINegativeChangeVal(countChild,false);
							}
							else
							{
								throw(CPoint(countChild,countVal));
							}
							break;

						case 1:
							if (strData.Compare(_T(STR_DI_YES)) == 0)
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setDIPositiveChangeVal(countChild,true);
							}
							else if (strData.Compare(_T(STR_DI_NO)) == 0)
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setDIPositiveChangeVal(countChild,false);
							}
							else
							{
								throw(CPoint(countChild,countVal));
							}
							break;

						default:
							throw(CPoint(countChild,-1));
							break;
						}
					}

					countVal++;

				}
			}

			countChild++;
		}
	}

	return ret;
}

int CSettingFrame::SaveFreSettingVal(CSuperGridCtrl::CTreeItem* pParent)
{
	int ret = 0;

	int nChildren = m_wndShowList.NumChildren(pParent);
	if (nChildren > 0)
	{
		POSITION pos = m_wndShowList.GetHeadPosition(pParent);
		int countChild = 0;
		while (pos)
		{
			CSuperGridCtrl::CTreeItem * pChild = m_wndShowList.GetNextChild(pParent, pos);
			if (pChild != NULL)
			{
				POSITION valPos = m_wndShowList.GetHeadPosition(pChild);
				int countVal = 0;
				while (valPos)
				{
					CSuperGridCtrl::CTreeItem * pVal = m_wndShowList.GetNextChild(pChild,valPos);
					if (pVal != NULL)
					{
						CItemInfo* lp = m_wndShowList.GetData(pVal);
						CString strData = lp->GetSubItem(0);
						float fVal;
						switch (countVal)
						{
						case 0:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setFreqUpperLimit(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						case 1:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setFreqLowerLimit(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						case 2:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setFreqUpperDisLimit(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						case 3:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setFreqLowerDisLimit(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						case 4:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setFreqChangeRate(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						default:
							throw(CPoint(countChild,-1));
							break;
						}
					}

					countVal++;

				}
			}

			countChild++;
		}
	}

	return ret;
}

int CSettingFrame::SaveSeqSettingVal(CSuperGridCtrl::CTreeItem* pParent)
{
	int ret = 0;

	int nChildren = m_wndShowList.NumChildren(pParent);
	if (nChildren > 0)
	{
		POSITION pos = m_wndShowList.GetHeadPosition(pParent);
		int countChild = 0;
		while (pos)
		{
			CSuperGridCtrl::CTreeItem * pChild = m_wndShowList.GetNextChild(pParent, pos);
			if (pChild != NULL)
			{
				POSITION valPos = m_wndShowList.GetHeadPosition(pChild);
				int countVal = 0;
				while (valPos)
				{
					CSuperGridCtrl::CTreeItem * pVal = m_wndShowList.GetNextChild(pChild,valPos);
					if (pVal != NULL)
					{
						CItemInfo* lp = m_wndShowList.GetData(pVal);
						CString strData = lp->GetSubItem(0);
						float fVal;
						int countGroundChild = 0;
						POSITION IDPos;
						switch (countVal)
						{
						case 0:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setPosiUpperLimit(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						case 1:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setPosiLowerLimit(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						case 2:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setPosiNull(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						case 3:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setNegaUpperLimit(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						case 4:
							if (CString2Float(strData,fVal))
							{
								ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setZeroUpperLimit(countChild,fVal);
								if (ret)
								{
									throw(CPoint(countChild,countVal));;
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));;
							}
							break;

						case 5:
							IDPos = m_wndShowList.GetHeadPosition(pVal);
							countGroundChild = m_wndShowList.NumChildren(pVal);
							if (countGroundChild <= MAX_GROUP_MEMBERS)
							{
								for (int i=0;i<countGroundChild;i++)
								{
									CSuperGridCtrl::CTreeItem * pID = m_wndShowList.GetNextChild(pVal,IDPos);
									if (pID != NULL)
									{
										CItemInfo* lp = m_wndShowList.GetData(pID);
										CString strID = lp->GetSubItem(0);
										int iVal;
										if (CString2Int(strID,iVal))
										{
											ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setGroupID(countChild,i,iVal);
											if(ret)
											{
												throw(CPoint(countChild,countVal));
											}
										}
										else if (strID.CollateNoCase(_T("NULL")) == 0)
										{
											ret = theApp.g_sDataBases[m_iStationIndex].vDevs[m_iDevIndex].setting.setGroupID(countChild,i,97);
											if(ret)
											{
												throw(CPoint(countChild,countVal));
											}
										}
										else
										{
											throw(CPoint(countChild,countVal));
										}

										
									}
									
								}
							}
							else
							{
								throw(CPoint(countChild,countVal));
							}
							break;

						default:
							throw(CPoint(countChild,-1));
							break;
						}
					}

					countVal++;

				}
			}

			countChild++;
		}
	}

	return ret;
}

// LayerSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "pcdraft.h"
#include "LayerSetupDlg.h"

#include "PCDraftView.h"
#include "PCDraftDoc.h"
#include "Layer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLayerSetupDlg dialog


CLayerSetupDlg::CLayerSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLayerSetupDlg::IDD, pParent),m_pDoc(NULL)
{
	//{{AFX_DATA_INIT(CLayerSetupDlg)
	//}}AFX_DATA_INIT
}


void CLayerSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLayerSetupDlg)
	DDX_Control(pDX, IDC_LAYER_NAME, m_LayerName);
	DDX_Control(pDX, IDC_SCALE, m_Scale);
	DDX_Control(pDX, IDC_LAYER_LIST, m_LayerList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLayerSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CLayerSetupDlg)
	ON_LBN_SELCHANGE(IDC_LAYER_LIST, OnSelchangeLayerList)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_RENAME, OnRename)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_MERGE, OnMerge)
	ON_BN_CLICKED(IDC_GRAYED, OnGrayed)
	ON_BN_CLICKED(IDC_HIDDEN, OnHidden)
	ON_BN_CLICKED(IDC_EDIT_LAYER, OnEditLayer)
	ON_LBN_DBLCLK(IDC_LAYER_LIST, OnDblclkLayerList)
	ON_BN_CLICKED(IDC_ARRANGE, OnArrange)
	ON_CBN_SELCHANGE(IDC_SCALE, OnSelchangeScale)
	ON_WM_ACTIVATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLayerSetupDlg message handlers

BOOL CLayerSetupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_bIgnoreActivate=false;
	m_bLastDocument=false;
	m_nLastScale=-1;

	return TRUE;
}

void CLayerSetupDlg::LoadListBox(bool bCheck)
{
	m_LayerList.m_pActiveLayer=m_pDoc->GetActiveLayer();
	if(bCheck && m_pDoc->GetLayersCount()==m_LayerList.GetCount())
	{
		for(int i=0;i<m_pDoc->GetLayersCount();i++)
		{
			m_LayerList.SetItemDataPtr(i,m_pDoc->GetLayer(i));
		}
		if(m_pDoc->m_nActiveLayer!=m_LayerList.GetCaretIndex())
		{
			m_LayerList.SetSel(m_LayerList.GetCaretIndex(),FALSE);
			m_LayerList.SetCaretIndex(m_pDoc->m_nActiveLayer);
			m_LayerList.SetSel(m_pDoc->m_nActiveLayer);
		}
	}
	else
	{
		m_LayerList.ResetContent();
		for(int i=0;i<m_pDoc->GetLayersCount();i++)
		{
			CLayer* pLayer=m_pDoc->GetLayer(i);
			m_LayerList.SetItemDataPtr(m_LayerList.AddString(NULL),pLayer);
		}
		m_LayerList.SetSel(m_pDoc->m_nActiveLayer);
		m_LayerList.SetCaretIndex(m_pDoc->m_nActiveLayer);
	}
}

void CLayerSetupDlg::OnSelchangeLayerList() 
{
	if(m_LayerList.GetSelCount()>1)
	{
		GetDlgItem(IDC_MERGE)->EnableWindow(TRUE);
		GetDlgItem(IDC_RENAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_LAYER)->EnableWindow(FALSE);
		BOOL bEnabled;
		int nGrayed=-1,nHidden=-1;
		for(int i=0;i<m_LayerList.GetCount();i++)
			if(m_LayerList.GetSel(i))
			{
				CLayer* pLayer=m_pDoc->GetLayer(i);
				if(nGrayed<0)
					nGrayed=pLayer->m_bGrayed?1:0;
				else if(nGrayed!=2 && nGrayed!=(pLayer->m_bGrayed?1:0))
					nGrayed=2;
				if(nHidden<0)
					nHidden=pLayer->m_bHidden?1:0;
				else if(nHidden!=2 && nHidden!=(pLayer->m_bHidden?1:0))
					nHidden=2;
				bEnabled=pLayer!=m_pDoc->GetActiveLayer();
				if(!bEnabled)
				{
					nGrayed=0;
					nHidden=0;
					break;
				}
			}
		CheckDlgButton(IDC_HIDDEN,nHidden);
		CheckDlgButton(IDC_GRAYED,nGrayed);
		GetDlgItem(IDC_HIDDEN)->EnableWindow(bEnabled);
		GetDlgItem(IDC_GRAYED)->EnableWindow(bEnabled);
		AfxSetWindowText(m_LayerName.GetSafeHwnd(),"");
		m_LayerName.EnableWindow(FALSE);
		m_Scale.SetCurSel(-1);
		m_Scale.EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_MERGE)->EnableWindow(FALSE);
		GetDlgItem(IDC_RENAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_LAYER)->EnableWindow(TRUE);
		m_LayerName.EnableWindow(TRUE);
		CLayer* pLayer=m_pDoc->GetLayer(m_LayerList.GetCaretIndex());
		BOOL bEnabled=pLayer!=m_pDoc->GetActiveLayer();
		GetDlgItem(IDC_HIDDEN)->EnableWindow(bEnabled);
		GetDlgItem(IDC_GRAYED)->EnableWindow(bEnabled);
		CheckDlgButton(IDC_HIDDEN,(bEnabled && pLayer->m_bHidden)?1:0);
		CheckDlgButton(IDC_GRAYED,(bEnabled && pLayer->m_bGrayed)?1:0);
		AfxSetWindowText(m_LayerName.GetSafeHwnd(),pLayer->m_strName);
		m_Scale.EnableWindow();
		if(m_Scale.GetCurSel()!=pLayer->m_nScale-1)	m_Scale.SetCurSel(pLayer->m_nScale-1);
		m_LayerName.SetSel(0,-1);
		if(GetActiveWindow()==this)
			GotoDlgCtrl(&m_LayerName);
	}
}

void CLayerSetupDlg::OnAdd() 
{
	CLayer* pLayer=m_pDoc->AddLayer();
	int n;
	m_LayerList.m_pActiveLayer=pLayer;
	m_LayerList.SetItemDataPtr(n=m_LayerList.AddString(NULL),pLayer);
	m_LayerList.SelItemRange(FALSE,0,m_LayerList.GetCount()-1);
	m_LayerList.SetSel(n);
	m_LayerList.SetCaretIndex(n);
	OnSelchangeLayerList();
	m_pDoc->LayersChanged();
}

void CLayerSetupDlg::OnRename() 
{
	CLayer* pLayer=m_pDoc->GetLayer(m_LayerList.GetCaretIndex());
	CString sName;
	m_LayerName.GetWindowText(sName);
	if(sName!=pLayer->m_strName)
	{
		if(m_pDoc->CheckLayerName(sName))
		{
			pLayer->m_strName=sName;
			m_pDoc->LayersChanged();
			m_LayerList.Invalidate();
		}
		else AfxMessageBox(IDS_ALREADY_TAKEN);
	}
}

void CLayerSetupDlg::OnDelete() 
{
	if(m_pDoc->GetLayersCount()-m_LayerList.GetSelCount()>=1)
	{
		for(int i=0;i<m_LayerList.GetCount();i++)
			if(m_LayerList.GetSel(i))
			{
				if(m_pDoc->GetLayer(i)->m_ShapeList.GetCount()!=0)
				{
					m_bIgnoreActivate=true;
					CString thePrompt;
					thePrompt.Format(IDS_QUERY_DELETE_LAYER,m_pDoc->GetLayer(i)->m_strName);
					if(AfxMessageBox(thePrompt,MB_YESNO|MB_DEFBUTTON2)==IDNO)
					{
						m_bIgnoreActivate=false;
						continue;
					}
					m_bIgnoreActivate=false;
				}
				m_pDoc->DeleteLayer(i);
				m_LayerList.DeleteString(i);
				--i;
			}
		m_LayerList.m_pActiveLayer=m_pDoc->GetActiveLayer();
		m_LayerList.SelItemRange(FALSE,0,m_LayerList.GetCount()-1);
		m_LayerList.SetSel(0);
		m_LayerList.SetCaretIndex(0);
		OnSelchangeLayerList();
		m_pDoc->LayersChanged();
	}
	else
	{
		m_bIgnoreActivate=true;
		AfxMessageBox(IDS_CANT_DELETE_LAYER,MB_OK|MB_ICONSTOP);
		m_bIgnoreActivate=false;
	}
}

void CLayerSetupDlg::OnMerge() 
{
	m_bIgnoreActivate=true;

	int n=m_LayerList.GetSelCount();
	int* pIndexes=new int[n];
	m_LayerList.GetSelItems(n,pIndexes);
	
	int nScale=m_pDoc->GetLayer(pIndexes[0])->m_nScale;
    int i = 1;
    for (; i<n; i++) if (nScale != m_pDoc->GetLayer(pIndexes[i])->m_nScale) break;

	if(i<n) AfxMessageBox(IDS_CANT_MERGE_LAYERS,MB_OK|MB_ICONEXCLAMATION);
	else if(AfxMessageBox(IDS_LAYER_MERGE_CONFIRM,MB_OKCANCEL|MB_ICONEXCLAMATION|MB_DEFBUTTON2)==IDOK)
	{
		m_pDoc->MergeLayer(n,pIndexes);
		LoadListBox();
		OnSelchangeLayerList();
		m_pDoc->LayersChanged();
	}
	delete []pIndexes;
	m_bIgnoreActivate=false;
}

void CLayerSetupDlg::OnGrayed() 
{
	UINT nState=IsDlgButtonChecked(IDC_GRAYED);
	if(nState==2)
	{
		nState=0;
	}
	else
	{
		nState=nState?0:1;
	}
	CheckDlgButton(IDC_GRAYED,nState);
	bool bGrayed=nState?true:false;
	for(int i=0;i<m_LayerList.GetCount();i++)
		if(m_LayerList.GetSel(i))
			m_pDoc->GetLayer(i)->m_bGrayed=bGrayed;
	m_pDoc->LayersChanged();
}

void CLayerSetupDlg::OnHidden() 
{
	UINT nState=IsDlgButtonChecked(IDC_HIDDEN);
	if(nState==2)
	{
		nState=0;
	}
	else
	{
		nState=nState?0:1;
	}
	CheckDlgButton(IDC_HIDDEN,nState);
	bool bHidden=nState?true:false;
	for(int i=0;i<m_LayerList.GetCount();i++)
		if(m_LayerList.GetSel(i))
			m_pDoc->GetLayer(i)->m_bHidden=bHidden;
	m_LayerList.Invalidate();
	m_pDoc->LayersChanged();
}

void CLayerSetupDlg::OnEditLayer() 
{
	m_pDoc->SetActiveLayer(m_LayerList.GetCaretIndex());
	m_LayerList.m_pActiveLayer=m_pDoc->GetActiveLayer();
	m_LayerList.m_pActiveLayer->m_bHidden=false;
	m_pDoc->LayersChanged();
	OnSelchangeLayerList();
	m_LayerList.Invalidate();
}

void CLayerSetupDlg::OnDblclkLayerList() 
{
	OnEditLayer();
}

void CLayerSetupDlg::OnArrange() 
{
	CMenu theMenu;
	theMenu.CreatePopupMenu();
   CString str;
   str.LoadString(IDS_BRINGTOFRONT);
	theMenu.AppendMenu(MF_ENABLED|MF_STRING,1,str);
   str.LoadString(IDS_SENDTOBACK);
	theMenu.AppendMenu(MF_ENABLED|MF_STRING,2,str);
   str.LoadString(IDS_MOVEFORWARD);
	theMenu.AppendMenu(MF_ENABLED|MF_STRING,3,str);
   str.LoadString(IDS_MOVEBACK);
	theMenu.AppendMenu(MF_ENABLED|MF_STRING,4,str);

	CPoint pt;
	GetCursorPos(&pt);

	int cmd=theMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_NONOTIFY|TPM_RETURNCMD,pt.x,pt.y,this);

	int nCurLayerPos=m_LayerList.GetCaretIndex(),nNewLayerPos=nCurLayerPos;
	switch(cmd)
	{
		case 1:
			nNewLayerPos=m_LayerList.GetCount()-1;
			break;
		case 2:
			nNewLayerPos=0;
			break;
		case 3:
			if(nNewLayerPos<m_LayerList.GetCount()-1) ++nNewLayerPos;
			break;
		case 4:
			if(nNewLayerPos>0) --nNewLayerPos;
			break;
	}

	if(nNewLayerPos!=nCurLayerPos)
	{
		m_LayerList.SetRedraw(FALSE);
		m_LayerList.SetSel(-1,FALSE);
		void* pLayer=m_LayerList.GetItemDataPtr(nCurLayerPos);
		m_LayerList.DeleteString(nCurLayerPos);
		m_LayerList.SetItemDataPtr(m_LayerList.InsertString(nNewLayerPos,NULL),pLayer);
		m_LayerList.SetSel(nNewLayerPos);
		m_LayerList.SetCaretIndex(nNewLayerPos);
		m_LayerList.SetRedraw(TRUE);
		m_LayerList.Invalidate();
		m_pDoc->MoveLayer(nCurLayerPos,nNewLayerPos);
		OnSelchangeLayerList();
		m_pDoc->LayersChanged();
	}
}

void CLayerSetupDlg::OnSelchangeScale() 
{
	CLayer* pLayer=m_pDoc->GetLayer(m_LayerList.GetCaretIndex());
	if(pLayer->SetScale(m_Scale.GetCurSel()+1,m_pDoc))
	{
		m_pDoc->CheckDocSize(true);
		m_pDoc->LayersChanged();
//		Refresh();
	}
	else
	{
		m_Scale.SetCurSel(pLayer->m_nScale-1);
		AfxMessageBox(IDS_NOT_COMPLETED);
	}
}	
	
void CLayerSetupDlg::Refresh()
{
	static UINT gCtrlID[] = { IDC_ADD, IDC_ARRANGE, IDC_DELETE, IDC_EDIT_LAYER, IDC_GRAYED, IDC_HIDDEN, IDC_LAYER_LIST, IDC_LAYER_NAME, IDC_MERGE, IDC_RENAME, IDC_SCALE, 0 };
	CMDIFrameWnd *pFrame=(CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild=(CMDIChildWnd*)pFrame->GetActiveFrame();
	CView *pView=pChild->GetActiveView();
	if(pView)
	{
		if(pView->IsKindOf(RUNTIME_CLASS(CPCDraftView)))
		{
			m_pDoc=(CPCDraftDoc*)pView->GetDocument();
		}
		else
		{
			if(!((CPCDraftApp*)AfxGetApp())->FindDrawingDoc(m_pDoc))
				m_pDoc=NULL;
		}
	}
	else
	{
		m_pDoc=NULL;
	}
	if(m_pDoc)
	{
		CString strText;
		strText.Format(IDS_LAYER_SETUP,(LPCTSTR)m_pDoc->GetTitle());
		AfxSetWindowText(GetSafeHwnd(),strText);
		bool bReloadScale;
		UINT nScaleFirst,nScaleLast;
		if(m_pDoc->m_bEnglishUnits)
		{
			nScaleFirst=IDS_ENGLISH_SCALE_FIRST;
			nScaleLast=IDS_ENGLISH_SCALE_LAST;
			bReloadScale=m_nLastScale!=1;
			m_nLastScale=1;
		}
		else
		{
			nScaleFirst=IDS_METRIC_SCALE_FIRST;
			nScaleLast=IDS_METRIC_SCALE_LAST;
			bReloadScale=m_nLastScale!=2;
			m_nLastScale=2;
		}

		if(bReloadScale)
		{
			m_Scale.ResetContent();
			for(UINT i=nScaleFirst;i<=nScaleLast;i++)
			{
				CString str;
				VERIFY(str.LoadString(i));
				m_Scale.AddString(str);
			}
		}

		if(!m_bLastDocument)
		{
			for(int i=0;gCtrlID[i];i++)
			{
				GetDlgItem(gCtrlID[i])->EnableWindow(TRUE);
			}
			m_bLastDocument=true;
		}

		LoadListBox(true);
		OnSelchangeLayerList();
	}
	else
	{
		CString strText;
		strText.LoadString(IDS_LAYER_SETUP_NODOC);
		AfxSetWindowText(GetSafeHwnd(),strText);
		m_LayerName.SetWindowText("");
		m_LayerList.ResetContent();
		m_Scale.ResetContent();
		for(int i=0;gCtrlID[i];i++)
		{
			GetDlgItem(gCtrlID[i])->EnableWindow(FALSE);
		}
		m_bLastDocument=false;
	}
}

void CLayerSetupDlg::DoIdleUpdate()
{
	if(GetActiveWindow()!=this)
	{
		Refresh();
	}
}

void CLayerSetupDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	if(!m_bIgnoreActivate)
	{
		if(nState)
		{
			Refresh();
		}
	}
}

BOOL CLayerSetupDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message==WM_KEYDOWN && m_LayerList.GetCount())
	{
		switch(pMsg->wParam)
		{
		case VK_DOWN:
			{
				int nCurSel=m_LayerList.GetCaretIndex()+1;
				if(nCurSel==m_LayerList.GetCount()) nCurSel=0;
				m_LayerList.SetCaretIndex(nCurSel);
				m_LayerList.SelItemRange(FALSE,0,m_LayerList.GetCount()-1);
				m_LayerList.SetSel(nCurSel);
				OnSelchangeLayerList();
				return TRUE;
			}
			break;
		case VK_UP:
			{
				int nCurSel=m_LayerList.GetCaretIndex()-1;
				if(nCurSel<0) nCurSel=m_LayerList.GetCount()-1;
				m_LayerList.SetCaretIndex(nCurSel);
				m_LayerList.SelItemRange(FALSE,0,m_LayerList.GetCount()-1);
				m_LayerList.SetSel(nCurSel);
				OnSelchangeLayerList();
				return TRUE;
			}
			break;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CLayerSetupDlg::OnClose() 
{
	DestroyWindow();
}
